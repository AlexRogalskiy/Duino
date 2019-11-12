////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2018 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_7SEGMENT_h
#define _MITOV_7SEGMENT_h

#include <Mitov.h>

namespace Mitov
{
	const byte C_Segments[16] =
	{
			0b0111111,  // = 0
			0b0000110,  // = 1
			0b1011011,  // = 2
			0b1001111,  // = 3
			0b1100110,  // = 4
			0b1101101,  // = 5
			0b1111101,  // = 6
			0b0000111,  // = 7
			0b1111111,  // = 8
			0b1101111,  // = 9
			0b1110111,  // = A
			0b1111100,  // = B
			0b1011000,  // = C
			0b1011110,  // = D
			0b1111001,  // = E
			0b1110001   // = F
	};
//---------------------------------------------------------------------------
	class Display7Segment
	{
	public:
		OpenWire::SourcePin	SegmentPins[ 16 ];

	public:
		bool	InvertedSegments : 1;
		bool	Enabled : 1;
		int32_t		InitialValue = 0;

	public:
		void SetEnabled( bool AValue )
		{
			if( Enabled == AValue )
				return;

			Enabled = AValue;
			UpdateDisplay();
		}

	public:
		void InputPin_o_Receive( void *_Data )
		{
			InitialValue = *(int32_t*)_Data;
			UpdateDisplay();
		}

	public:
		inline void SystemInit()
		{
			UpdateDisplay();
		}

	public:
		void UpdateDisplay()
		{
			int AValue = (Enabled) ? C_Segments[ InitialValue & 0xF ] : 0;
			for( int i = 0; i < 16; ++i )
			{
				bool ABitValue = AValue & 1;
				ABitValue ^= InvertedSegments;
				SegmentPins[ i ].Notify( &ABitValue );
				AValue >>= 1;
			}
		}

	public:
		Display7Segment() :
			InvertedSegments( false ),
			Enabled( true )
		{
		}

	};
//---------------------------------------------------------------------------
	template<int C_NUM_INPUTS, typename T> class DisplayBasicDynamic7Segment
	{
	public:
		OpenWire::SourcePin	SegmentPins[ 16 ];
		OpenWire::SourcePin	DigitPins[ C_NUM_INPUTS ];

	public:
		bool	InvertedSegments : 1;
		bool	InvertedDigits : 1;
		bool	Enabled : 1;
		T		InitialValue = 0;

	public:
		unsigned long	RefreshInterval = 1000;

	protected:
		unsigned long	FLastTime = 0;
		int				FDigit = 0;

	public:
		inline void InputPin_o_Receive( void *_Data )
		{
			InitialValue = *(T*)_Data;
		}

	public:
		void UpdateDisplay()
		{
			int AValue;
			if( Enabled )
				AValue = GetSegmentsValue();

			else
				AValue = 0;

			bool ABitValue = false;
			ABitValue ^= InvertedDigits;
			for( int i = 0; i < C_NUM_INPUTS; ++i )
				DigitPins[ i ].Notify( &ABitValue );

			ComponentsHardware::SystemUpdateHardware();

			for( int i = 0; i < 16; ++i )
			{
				bool ABitValue = AValue & 1;
				ABitValue ^= InvertedSegments;
				SegmentPins[ i ].Notify( &ABitValue );
				AValue >>= 1;
			}

			AValue = FDigit;
			for( int i = 0; i < C_NUM_INPUTS; ++i )
			{
				bool ABitValue = ( AValue-- == 0 );
				ABitValue ^= InvertedDigits;
				DigitPins[ i ].Notify( &ABitValue );
			}
		}

	protected:
		virtual int	GetSegmentsValue() = 0;

	public:
		inline void SystemInit()
		{
			UpdateDisplay();
		}

		inline void SystemLoopBegin( unsigned long currentMicros )
		{
			if( ( currentMicros - FLastTime ) < RefreshInterval )
				return;

			++FDigit;

			if( FDigit >= C_NUM_INPUTS )
				FDigit = 0;

			FLastTime = currentMicros;
			UpdateDisplay();
		}

	public:
		DisplayBasicDynamic7Segment() :
			InvertedSegments( false ),
			InvertedDigits( false ),
			Enabled( true )
		{
		}

	};
//---------------------------------------------------------------------------
	template<int C_NUM_INPUTS> class DisplayIntegerDynamic7Segment : public Mitov::DisplayBasicDynamic7Segment<C_NUM_INPUTS, long>
	{
		typedef	Mitov::DisplayBasicDynamic7Segment<C_NUM_INPUTS, long>	inherited;

	public:
		bool	LeadingZeroes = false;

	protected:
		Mitov::SimpleList<unsigned int>	FPowers;
		long	FMaxValue; // The biggest possible to display value
		long	FMinValue; // The smallest possible to display value

	protected:
		virtual int	GetSegmentsValue()
		{
			int AValue;
			if( inherited::InitialValue > FMaxValue )
				AValue = 0b0000001;

			else if( inherited::InitialValue < FMinValue )
				AValue = 0b0001000;

			else
			{
				AValue = ( (int)abs( inherited::InitialValue ) ) / FPowers[ inherited::FDigit ];
				if( ( inherited::InitialValue < 0 ) && ( inherited::FDigit > 0 ))
				{
					if( ( !LeadingZeroes ) && ( AValue == 0 ) )
					{
						int APreviousValue = ( -inherited::InitialValue ) / FPowers[ inherited::FDigit - 1 ];
						if( APreviousValue > 0 )
							AValue = 0b1000000;

						else
							AValue = 0;

					}
					else
					{
						if( inherited::FDigit == C_NUM_INPUTS - 1 )
							AValue = 0b1000000;

						else
						{
							int ADigitValue = AValue % 10;
							AValue = C_Segments[ ADigitValue ];
						}
					}
				}
				else
				{
					if( ( !LeadingZeroes ) && ( AValue == 0 ) )
						AValue = 0;

					else
					{
						int ADigitValue = AValue % 10;
						AValue = C_Segments[ ADigitValue ];
					}
				}
			}

			return AValue;
		}

	public:
		inline void SystemInit()
		{
			FPowers.SetCount( C_NUM_INPUTS );
			for( int i = 0; i < C_NUM_INPUTS; ++i )
				FPowers[ i ] = pow( 10, i ) + 0.5;

			FMaxValue = pow( 10, C_NUM_INPUTS ) + 0.5 - 1;
			FMinValue = -( pow( 10, C_NUM_INPUTS - 1 ) + 0.5 - 1 );
		}

	};
//---------------------------------------------------------------------------
	template<int C_NUM_INPUTS> class DisplayAnalogDynamic7Segment : public Mitov::DisplayBasicDynamic7Segment<C_NUM_INPUTS, float>
	{
		typedef	Mitov::DisplayBasicDynamic7Segment<C_NUM_INPUTS, float>	inherited;

	public:
		uint8_t	Precision = 2;

	protected:
		char	*FBuffer;
		char	*FStartChar;
		unsigned char	FTextLength;
		unsigned char	FFirstPos;

		char			FDecimalPointPos;

	protected:
		virtual int	GetSegmentsValue()
		{
			int AValue;
			int ANumDigits = C_NUM_INPUTS;
			if( ! FStartChar )
			{
				FStartChar = dtostrf( inherited::InitialValue, 1, Precision, FBuffer );
//				Serial.println( FStartChar );
				Mitov::String AText( FStartChar );
				FTextLength = AText.length(); // strlen( FStartChar ); 
//				FTextLength = strlen( FStartChar ); 
//				Serial.println( FTextLength );
				FDecimalPointPos = AText.indexOf( '.' );
				if( Precision == 0 )
				{
					if( FTextLength <= ANumDigits )
						FFirstPos = FTextLength - 1;

					else
						FFirstPos = ANumDigits - 1;

				
//					Serial.println( FStartChar );
//					Serial.println( FTextLength );
//					Serial.println( FFirstPos );
//					Serial.println( FDecimalPointPos );
				}

				else
				{
					if( FTextLength <= ANumDigits + 1 )
						FFirstPos = FTextLength - 1;

					else
						FFirstPos = ANumDigits + 1 - 1;
				}				

//				Serial.println( FFirstPos );
			}

			int ACorrectedTextLength = FTextLength;
			if( Precision > 0 )
				--ACorrectedTextLength;

			if( ( ACorrectedTextLength - Precision ) > ANumDigits )
			{
				if( inherited::InitialValue > 0 )
					AValue = 0b0000001; // Overflow +

				else
					AValue = 0b0001000; // Overflow -
			}

			else
			{
				int ATextPos = FFirstPos - inherited::FDigit;
				if( ATextPos < 0 )
					AValue = 0;

				else
				{
					if( ATextPos < 0 )
						return( 0 );

					bool ADecimalPoint = ( FStartChar[ ATextPos ] == '.' );
					if( ATextPos <= FDecimalPointPos )
						--ATextPos;

//					if( ADecimalPoint )
//						--ATextPos;

					if( ATextPos < 0 )
						return( 0 );

/*
					if( FDigit == 0 )
					{
						Serial.println( FStartChar );
						Serial.println( ATextPos );
					}
*/
					if( FStartChar[ ATextPos ] == '-' )
						AValue = 0b1000000;

					else
					{
						AValue = FStartChar[ ATextPos ] - '0';
						AValue = C_Segments[ AValue ];
					}

					if( ADecimalPoint )
						AValue |= 0x80;

				}
			}

			return AValue;
		}

	public:
		void InputPin_o_Receive( void *_Data )
		{
			FStartChar = nullptr;
			inherited::InputPin_o_Receive( _Data );
		}

	public:
		inline void SystemInit()
		{
			FBuffer = new char[ 15 + Precision ];
		}

/*
		virtual ~DisplayAnalogDynamic7Segment()
		{
			delete [] FBuffer;
		}
*/
	};

}

#endif
