////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2018 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_MAXIM_LED_CONTROL_h
#define _MITOV_MAXIM_LED_CONTROL_h

#include <Mitov.h>
#include <Mitov_Graphics_Monochrome.h>
#include <SPI.h>

#ifdef __TEST_FOR_DEBUG_PRINTS__
#define Serial UNGUARDED DEBUG PRINT!!!
#endif

namespace Mitov
{
	static const uint8_t C_MaximSegments[16] =
	{
		0b1111110,  // = 0
		0b0110000,  // = 1
		0b1101101,  // = 2
		0b1111001,  // = 3
		0b0110011,  // = 4
		0b1011011,  // = 5
		0b1011111,  // = 6
		0b1110000,  // = 7
		0b1111111,  // = 8
		0b1111011,  // = 9
		0b1110111,  // = A
		0b0011111,  // = B
		0b0001101,  // = C
		0b0111101,  // = D
		0b1001111,  // = E
		0b1000111   // = F
	};
//---------------------------------------------------------------------------
	static const uint8_t C_MaximDecimalPoint[2] =
	{
		0b00000000,
		0b10000000
	};
//---------------------------------------------------------------------------
	template<typename T_CS_IMPLEMENTATION, typename T_SPI, T_SPI &C_SPI, int COUNT_CONTROLLERS> class MaximLedController : public T_CS_IMPLEMENTATION
	{
	protected:
		static const	uint8_t OP_DIGIT0 = 1;
		static const	uint8_t OP_DECODEMODE = 9;
		static const	uint8_t OP_INTENSITY = 10;
		static const	uint8_t OP_SCANLIMIT = 11;
		static const	uint8_t OP_SHUTDOWN = 12;
		static const	uint8_t OP_DISPLAYTEST = 15;

	public:
		bool	Enabled : 1;

	protected:
		bool	FModified : 1 ;

		uint8_t	FRegistersCurrent[ COUNT_CONTROLLERS * 8 ];
		uint8_t	FRegisters[ COUNT_CONTROLLERS * 8 ];

	public:
		float	Intensity = 1.0f;

	protected:
		void SetIntensity( float AValue )
		{
			if( Intensity == AValue )
				return;

			Intensity = AValue;
			UpdateIntensity();
		}

		void SetEnabled( bool AValue )
		{
			if( Enabled == AValue )
				return;

			Enabled = AValue;
			UpdateShutDown();
		}

	public:
		void SetPixelValue( int AIndex, bool AValue )
		{
//			Serial.print( "SetPixelValue( " );
//			Serial.print( AIndex );
//			Serial.print( ", " );
//			Serial.print( AValue );
//			Serial.println( " )" );
			if( AValue )
				FRegisters[ AIndex / 8 ] |= 1 << ( AIndex & 0b111 );  // same as % 8

			else
				FRegisters[ AIndex / 8 ] &= ~( 1 << ( AIndex & 0b111 ));  // same as % 8

			FModified = true;
		}

		bool GetPixelValue( int AIndex )
		{
			return FRegisters[ AIndex / 8 ] & ( 1 << ( AIndex & 0b111 ));  // same as % 8
		}

	protected:
		void spiTransferAll( uint8_t opcode, uint8_t data )
		{
			//enable the line 
			T_CS_IMPLEMENTATION::SetPinValue( false );
			//Now shift out the data 

//			C_SPI.beginTransaction( SPISettings( 100000, MSBFIRST, SPI_MODE0) );

			for(int i = 0; i < COUNT_CONTROLLERS; ++i )
			{    
				C_SPI.transfer( opcode );
				C_SPI.transfer( data );
			}
    
//			C_SPI.endTransaction();

			//latch the data onto the display
			T_CS_IMPLEMENTATION::SetPinValue( true );
		}

/*
		void spiTransfer( int addr, uint8_t opcode, uint8_t data )
		{
			//enable the line 
			T_CS_IMPLEMENTATION::SetPinValue( false );
			//Now shift out the data 

			for(int i = 1; i < COUNT_CONTROLLERS; ++i )
				C_SPI.transfer16( 0 ); // Skip one chip
      
			C_SPI.transfer( opcode );
			C_SPI.transfer( data );
    
			//latch the data onto the display
			T_CS_IMPLEMENTATION::SetPinValue( true );
		}    
*/
	protected:
		inline void UpdateIntensity()
		{
			spiTransferAll( OP_INTENSITY, Intensity * 15 + 0.5 );
		}

		inline void UpdateShutDown()
		{
			spiTransferAll( OP_SHUTDOWN, Enabled );
		}

		void UpdatePixels()
		{
			uint16_t ABuffer[ COUNT_CONTROLLERS ];
			FModified = false;

			for(;;)
			{
				bool HasChange = false;
				for( int AControllerIndex = 0; AControllerIndex < COUNT_CONTROLLERS; ++AControllerIndex )
				{
					ABuffer[ AControllerIndex ] = 0;
					for( int j = 0; j < 8; ++j )
					{
						int ARegIndex = AControllerIndex * 8 + j;
						uint16_t ANewRegValue = FRegisters[ ARegIndex ];
						if( FRegistersCurrent[ ARegIndex ] != ANewRegValue )
						{
							FRegistersCurrent[ ARegIndex ] = ANewRegValue;
							//Serial.println( ANewRegValue, HEX );
							ABuffer[ AControllerIndex ] = ( ANewRegValue << 8 ) | ( OP_DIGIT0 + j );
							HasChange = true;
							break;
						}
					}
				}

				if( ! HasChange )
					break;

//				Serial.print( "ABuffer = " );
//				Serial.println( ABuffer[ 0 ], HEX );

				//enable the line 
				T_CS_IMPLEMENTATION::SetPinValue( false );
				C_SPI.transfer( ABuffer, COUNT_CONTROLLERS * 2 );

				//latch the data onto the display
				T_CS_IMPLEMENTATION::SetPinValue( true );
			}
		}

	public:
		inline void SystemInit()
		{
//			int AStartPixel = 0;
//			for( int i = 0; i < FPixelGroups.size(); ++i )
//				FPixelGroups[ i ]->StartPixels( AStartPixel );	

//			FNumDevices = ( AStartPixel + 63 ) / 64;

//			FRegisters.SetCount( FNumDevices * 8, 0 );
//			FRegistersCurrent.SetCount( FNumDevices * 8, 0 );

			spiTransferAll( OP_DISPLAYTEST, 0 );
			spiTransferAll( OP_SCANLIMIT, 7 );
			spiTransferAll( OP_DECODEMODE, 0 );

			UpdateShutDown();
			UpdateIntensity();

//			if( FModified )


			for( int i = 0; i < COUNT_CONTROLLERS * 8; ++i )
				FRegistersCurrent[ i ] = FRegisters[ i ] + 1;

			UpdatePixels();

//			spiTransferAll( OP_DIGIT0, 255 );
		}

		inline void SystemLoopEnd( unsigned long currentMicros )
		{
			ClockInputPin_o_Receive( nullptr );
		}

	public:
		void ClockInputPin_o_Receive( void *_Data )
		{
			if( FModified )
				UpdatePixels();
		}

	public:
		MaximLedController() :
			Enabled( true ),
			FModified( false )
		{
			for( int i = 0; i < COUNT_CONTROLLERS * 8; ++i )
				FRegisters[ i ] = 0;
		}

	};
//---------------------------------------------------------------------------
	template<typename T_OWNER, T_OWNER &C_INSTANCE, uint16_t C_COUNT_PIXELS, uint16_t C_START_PIXEL> class MaximLedGroup
	{
	public:
		bool	Enabled : 1;
		bool	InitialValue : 1;

	protected:
		uint8_t		FValues[ ( C_COUNT_PIXELS + 7 ) / 8 ] = { 0 };

	public:
		void UpdateDisplay() // Used by Enabled setter!
		{
			for( uint8_t i = 0; i < C_COUNT_PIXELS; ++i )
			{
				if( Enabled )
					C_INSTANCE.SetPixelValue( C_START_PIXEL + i, ( FValues[ i / 8 ] & ( 1 << ( i & 0b111 ))) != 0 );

				else
					C_INSTANCE.SetPixelValue( C_START_PIXEL + i, false );
			}
		}

	public:
		void InputPins_o_Receive( int AIndex, void *_Data )
		{
			bool AValue = *(bool *)_Data;
			if( Func::SetBitField( FValues, AIndex, AValue ) )
				if( Enabled )
					C_INSTANCE.SetPixelValue( AIndex, AValue );

		}

	public:
		inline void SystemStart()
		{
			for( int i = 0; i < C_COUNT_PIXELS; ++i )
				C_INSTANCE.SetPixelValue( C_START_PIXEL + i, InitialValue );

		}

	public:
		MaximLedGroup() :
			Enabled( true ),
			InitialValue( false )
		{
		}

	};
//---------------------------------------------------------------------------
/*
	template<typename T_OWNER, T_OWNER &C_INSTANCE, uint16_t C_COUNT_PIXELS, uint16_t C_START_PIXEL> class MaximLedGroupSection7Segments
	{
	public:
		bool	Enabled = true;
		bool	InitialValue = false;

	public:
		virtual void StartPixels( int &AStartPixel ) override
		{
			for( int i = 0; i < 7; ++i )
			{
				InputPins[ i ].FOwner = this;
				InputPins[ i ].FIndex = AStartPixel + 6 - i;
				InputPins[ i ].Value = InitialValue;
				C_INSTANCE.SetPixelValue( AStartPixel + 6 - i, InitialValue );
			}

			InputPins[ 7 ].FOwner = this;
			InputPins[ 7 ].FIndex = AStartPixel + 7;
			InputPins[ 7 ].Value = InitialValue;
			C_INSTANCE.SetPixelValue( AStartPixel + 7, InitialValue );

			inherited::StartPixels( AStartPixel );
		}

	public:
		MaximLedGroupSection7Segments()
		{
//			InputPins.SetCount( 8 );
//			CountPixels = 8;
		}
	};
*/
//---------------------------------------------------------------------------
	template<typename T_OWNER, T_OWNER &C_INSTANCE, uint16_t C_COUNT_PIXELS, uint16_t C_START_PIXEL> class MaximLedGroupValueSection7Segments
	{
	public:
		bool	Enabled : 1;
		bool	InitialDecmalPointValue : 1; // = false;
		uint8_t	InitialValue : 4;

	public:
/*
		virtual void StartPixels( int &AStartPixel ) override
		{
//			inherited::StartPixels( AStartPixel );
			FStartPixel = AStartPixel;
			AStartPixel += 8;
		}
*/

	public:
		void UpdateDisplay() // Used by Enabled setter!
		{
			uint8_t AValue = (Enabled) ? C_MaximSegments[ InitialValue & 0xF ] | C_MaximDecimalPoint[ InitialDecmalPointValue & 1 ] : 0;

			for( int i = 0; i < 8; ++i )
				C_INSTANCE.SetPixelValue( C_START_PIXEL + i, ( AValue >> i ) & 1 );
		}

	public:
		void InputPin_o_Receive( void *_Data )
		{
			int32_t AValue = *(int32_t *)_Data;
			if( AValue > 0xF )
				AValue = 0xF;

			if( InitialValue == AValue )
				return;

			InitialValue = AValue;
			UpdateDisplay();
		}

		void DecmalPointInputPin_o_Receive( void *_Data )
		{
			bool AValue = *(bool *)_Data;
			if( InitialDecmalPointValue == AValue )
				return;

			InitialDecmalPointValue = AValue;
			UpdateDisplay();
		}

	public:
		inline void SystemStart() 
		{
			UpdateDisplay();
		}

	public:
		MaximLedGroupValueSection7Segments() :
			Enabled( true ),
			InitialDecmalPointValue( false ),
			InitialValue( 0 )
		{
		}
	};
//---------------------------------------------------------------------------
	template<typename T_OWNER, T_OWNER &C_INSTANCE, typename TBASE_CLASS, typename T_DATA, uint16_t C_COUNT_PIXELS, uint16_t C_START_PIXEL> class MaximLedGroupTypedValueDisplay7Segments : public TBASE_CLASS
	{
		typedef TBASE_CLASS inherited;

	public:
		bool	Enabled : 1;
		bool	ReversedOrder : 1;

	public:
		T_DATA	InitialValue = ::Mitov::GetDefaultValue<T_DATA>();

//	protected:
//		virtual uint8_t GetSegmentsValue( int &ADigit ) = 0;

	public:
		void UpdateDisplay() // Used by Enabled setter!
		{
			inherited::UpdateDisplay();
			for( int ADigit = 0; ADigit < C_COUNT_PIXELS / 8; ++ ADigit )
			{
				uint8_t AValue;

				if( Enabled )
					AValue = TBASE_CLASS::template GetSegmentsValue<C_COUNT_PIXELS>( ADigit, InitialValue );

				else
					AValue = 0;

//				Serial.print( "ADigit = " ); Serial.println( ADigit );
//				Serial.print( "AValue = " ); Serial.println( AValue, HEX );
//				Serial.println( "" );

				if( ReversedOrder )
				{
					for( int i = 0; i < 8; ++i )
						C_INSTANCE.SetPixelValue( C_START_PIXEL + ( ( C_COUNT_PIXELS / 8 ) - ADigit - 1 ) * 8 + i, ( AValue >> i ) & 1 );
				}

				else
				{
					for( int i = 0; i < 8; ++i )
						C_INSTANCE.SetPixelValue( C_START_PIXEL + ADigit * 8 + i, ( AValue >> i ) & 1 );
				}
			}
		}

	public:
		void InputPin_o_Receive( void *_Data )
		{
			T_DATA AValue = *(T_DATA *)_Data;
			if( InitialValue == AValue )
				return;

			InitialValue = AValue;
//			Serial.println( InitialValue );
			UpdateDisplay();
		}

	public:
		inline void SystemStart()
		{
			inherited::template SystemStart<C_COUNT_PIXELS>();
			UpdateDisplay();
		}

	public:
		MaximLedGroupTypedValueDisplay7Segments() :
			Enabled( true ),
			ReversedOrder( false )
		{
		}
	};
//---------------------------------------------------------------------------
/*
	template<typename T_OWNER, T_OWNER &C_INSTANCE, uint16_t C_COUNT_PIXELS, uint16_t C_START_PIXEL, typename T_DATA> MaximLedGroupTypedValueDisplay7Segments<T_OWNER, C_INSTANCE, C_COUNT_PIXELS, C_START_PIXEL, T_DATA, Mitov::String>::MaximLedGroupTypedValueDisplay7Segments() :
		{
		}
*/
//---------------------------------------------------------------------------
	class MaximLedGroupIntegerDisplay7Segments // : public MaximLedGroupTypedValueDisplay7Segments<T_OWNER, C_INSTANCE, C_COUNT_PIXELS, C_START_PIXEL, int32_t>
	{
	public:
		bool	LeadingZeroes = false;

	protected:
		Mitov::SimpleList<unsigned int>	FPowers;
		long	FMaxValue; // The biggest possible to display value
		long	FMinValue; // The smallest possible to display value

	public:
		inline void UpdateDisplay() {} // Used by Enabled setter! Placeholder!

	protected:
		template<uint16_t C_COUNT_PIXELS> uint8_t	GetSegmentsValue( int &ADigit, int32_t AInitialValue )
		{
			int AValue;
			if( AInitialValue > FMaxValue )
				AValue = 0b1000000;

			else if( AInitialValue < FMinValue )
				AValue = 0b0001000;

			else
			{
				AValue = int( abs( AInitialValue ) ) / FPowers[ ADigit ];
				if( ( AInitialValue < 0 ) && ( ADigit > 0 ))
				{
					if( ( !LeadingZeroes ) && ( AValue == 0 ) )
					{
						int APreviousValue = ( -AInitialValue ) / FPowers[ ADigit - 1 ];
						if( APreviousValue > 0 )
							AValue = 0b0000001;

						else
							AValue = 0;

					}

					else
					{
						if( ADigit == ( C_COUNT_PIXELS / 8 ) - 1 )
							AValue = 0b1000000;

						else
						{
							int ADigitValue = AValue % 10;
							AValue = C_MaximSegments[ ADigitValue ];
						}
					}
				}

				else if( ( AInitialValue == 0 ) && ( ADigit == 0 ) )
					AValue = C_MaximSegments[ 0 ];

				else
				{
					
					if( ( !LeadingZeroes ) && ( AValue == 0 ) )
						AValue = 0;

					else
					{
						int ADigitValue = AValue % 10;
						AValue = C_MaximSegments[ ADigitValue ];
					}
				}
			}

			return AValue;
		}

	public:
		template<uint16_t C_COUNT_PIXELS> inline void SystemStart() 
		{
			FPowers.SetCount( ( C_COUNT_PIXELS / 8 ) );
			for( int i = 0; i < ( C_COUNT_PIXELS / 8 ); ++i )
				FPowers[ i ] = pow( 10, i ) + 0.5;

			FMaxValue = pow( 10, ( C_COUNT_PIXELS / 8 ) ) + 0.5 - 1;
			FMinValue = -( pow( 10, ( C_COUNT_PIXELS / 8 ) - 1 ) + 0.5 - 1 );

//			inherited::SystemStart();
		}

	};
//---------------------------------------------------------------------------
	template<typename T_OWNER, T_OWNER &C_INSTANCE, uint16_t C_COUNT_PIXELS, uint16_t C_START_PIXEL> class MaximLedGroupUnsignedBitPixelsSection
	{
	public:
		bool	Enabled = true;

	public:
		uint32_t	InitialValue = 0;

	public:
		void UpdateDisplay() // Used by Enabled setter!
		{
			for( int i = 0; i < C_COUNT_PIXELS; ++i )
				C_INSTANCE.SetPixelValue( C_START_PIXEL + i, Enabled && ( InitialValue >> i ) & 1 );

		}

	public:
		inline void SystemStart()
		{
			UpdateDisplay();
		}

	public:
		void InputPin_o_Receive( void *_Data )
		{
			uint32_t AValue = *(uint32_t *)_Data;
			if( InitialValue == AValue )
				return;

			InitialValue = AValue;
			UpdateDisplay();
		}

	};
//---------------------------------------------------------------------------
	class MaximLedGroupAnalogDisplay7Segments
	{
	public:
		uint8_t		Precision = 2;

	protected:
		char	*FBuffer;
		char	*FStartChar;
		uint8_t	FTextLength;
		uint8_t	FFirstPos;

		char	FDecimalPointPos;

	public:
		template<uint16_t C_COUNT_PIXELS> uint8_t	GetSegmentsValue( int &ADigit, float AInitialValue )
		{
			int AValue;
			int ANumDigits = ( C_COUNT_PIXELS / 8 );

//			Serial.print( "ANumDigits = " );
//			Serial.println( ANumDigits );

			if( ! FStartChar )
			{
				FStartChar = dtostrf( AInitialValue, 1, Precision, FBuffer );
//				Serial.print( "FStartChar = " );
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

//				Serial.print( "FFirstPos = " );
//				Serial.println( FFirstPos );
			}

			int ACorrectedTextLength = FTextLength;
			if( Precision > 0 )
				--ACorrectedTextLength;

			if( ( ACorrectedTextLength - Precision ) > ANumDigits )
			{
				if( AInitialValue > 0 )
					AValue = 0b1000000; // Overflow +

				else
					AValue = 0b0001000; // Overflow -
			}

			else
			{
				int ATextPos = FFirstPos - ADigit;
				if( ATextPos < 0 )
					AValue = 0;

				else
				{
					if( ATextPos < 0 )
						return( 0 );

					bool ADecimalPoint = ( FStartChar[ ATextPos ] == '.' );
					if( ATextPos <= FDecimalPointPos )
						--ATextPos;

					else if( ADecimalPoint )
						--ATextPos;

					if( ATextPos < 0 )
						return( 0 );

//					Serial.print( "ATextPos = " );
//					Serial.println( ATextPos );
/*
					if( FDigit == 0 )
					{
						Serial.println( FStartChar );
						Serial.println( ATextPos );
					}
*/
					if( FStartChar[ ATextPos ] == '-' )
						AValue = 0b0000001;

					else
					{
						AValue = FStartChar[ ATextPos ] - '0';
						AValue = C_MaximSegments[ AValue ];
					}

					if( ADecimalPoint )
						AValue |= 0x80;

//					Serial.println( ADigit );
//					Serial.println( AValue, HEX );
				}
			}

			return AValue;
		}

	public:
		inline void UpdateDisplay() // Used by Enabled setter!
		{
			FStartChar = nullptr;
		}

	public:
		template<uint16_t C_COUNT_PIXELS> inline void SystemStart() 
		{
			FBuffer = new char[ 15 + Precision ];
//			inherited::SystemStart();
		}

//	public:
//		using inherited::inherited;

	};
//---------------------------------------------------------------------------
	template<typename T_OWNER, T_OWNER &C_INSTANCE, uint16_t C_COUNT_PIXELS, uint16_t C_START_PIXEL> class MaximLedSingleValueGroup
	{
	public:
		bool	Enabled : 1;
		bool	InitialValue : 1;

	public:
		void UpdateDisplay() // Used by Enabled setter!
		{
			for( int i = 0; i < C_COUNT_PIXELS; ++i )
			{
				if( Enabled )
					C_INSTANCE.SetPixelValue( C_START_PIXEL + i, InitialValue );

				else
					C_INSTANCE.SetPixelValue( C_START_PIXEL + i, false );
			}
		}

	public:
		void InputPin_o_Receive( void *_Data )
		{
			bool AValue = *(bool *)_Data;
			if( InitialValue == AValue )
				return;

			InitialValue = AValue;
			UpdateDisplay();
		}

	public:
		inline void SystemStart()
		{
			UpdateDisplay();
		}

	public:
		MaximLedSingleValueGroup() :
			Enabled( true ),
			InitialValue( false )
		{
		}
	};
//---------------------------------------------------------------------------
	template<typename T_OWNER, T_OWNER &C_INSTANCE, uint16_t C_COUNT_PIXELS, uint16_t C_START_PIXEL> class MaximLedIndexedPixelGroup
	{
	public:
		bool	Enabled : 1;
		bool	InitialValue : 1;
		uint32_t InitialIndex = 0;

	public:
		void IndexInputPin_o_Receive( void *_Data )
		{
			uint32_t AIndex = *(uint32_t *)_Data;
			if( AIndex > C_COUNT_PIXELS )
				AIndex = C_COUNT_PIXELS;

			if( InitialIndex == AIndex )
				return;

			InitialIndex = AIndex;
			UpdateDisplay();
		}

		void InputPin_o_Receive( void *_Data )
		{
			bool AValue = *(bool *)_Data;
			if( InitialValue == AValue )
				return;

			InitialValue = AValue;
			UpdateDisplay();
		}

	public:
		inline void SystemStart()
		{
			UpdateDisplay();
		}

	public:
		void UpdateDisplay() // Used by Enabled setter!
		{
			if( Enabled )
				C_INSTANCE.SetPixelValue( C_START_PIXEL + InitialValue, InitialValue );

			else
				C_INSTANCE.SetPixelValue( C_START_PIXEL + InitialValue, false );
		}

	public:
		MaximLedIndexedPixelGroup() :
			Enabled( true ),
			InitialValue( false )
		{
		}

	};
//---------------------------------------------------------------------------
	template<typename T_OWNER, T_OWNER &C_INSTANCE, uint16_t C_COUNT_PIXELS, uint16_t C_START_PIXEL> class MaximLedIndexed2DPixelGroup
	{
	public:
		bool	Enabled : 1;
		bool	InitialValue : 1;
		uint32_t Width = 8;
		uint32_t InitialIndexX = 0;
		uint32_t InitialIndexY = 0;

	public:
		void IndexInputPins_o_Receive( int AIndex, void *_Data )
		{
			if( AIndex )
				DoReceiveIndexY( _Data );

			else
				DoReceiveIndexX( _Data );

		}

		void InputPin_o_Receive( void *_Data )
		{
			bool AValue = *(bool *)_Data;
			if( InitialValue == AValue )
				return;

			InitialValue = AValue;
			UpdateDisplay();
		}

	public:
		inline void SystemStart()
		{
			UpdateDisplay();
		}

	protected:
		void DoReceiveIndexX( void *_Data )
		{
			uint32_t AIndex = *(uint32_t *)_Data;
			if( AIndex > Width )
				AIndex = Width;

			if( InitialIndexX == AIndex )
				return;

			InitialIndexX = AIndex;
			UpdateDisplay();
		}

		void DoReceiveIndexY( void *_Data )
		{
			uint32_t AIndex = *(uint32_t *)_Data;
			uint32_t AHeight = ( C_COUNT_PIXELS + Width - 1 ) / Width;
			if( AIndex > AHeight )
				AIndex = AHeight;

			if( InitialIndexY == AIndex )
				return;

			InitialIndexY = AIndex;
			UpdateDisplay();
		}

	public:
		void UpdateDisplay() // Used by Enabled setter!
		{
			uint32_t AIndex = C_START_PIXEL + InitialIndexY * Width + InitialIndexX;
			if( Enabled )
				C_INSTANCE.SetPixelValue( AIndex, InitialValue );

			else
				C_INSTANCE.SetPixelValue( AIndex, false );
		}

	public:
		MaximLedIndexed2DPixelGroup() :
			Enabled( true ),
			InitialValue( false )
		{
		}

	};
//---------------------------------------------------------------------------
	template<typename T_OWNER, T_OWNER &C_INSTANCE, uint32_t C_WIDTH, uint32_t C_HEIGHT, uint16_t C_COUNT_PIXELS, uint16_t C_START_PIXEL> class MaximLedGraphics2DPixelGroup : public GraphicsImpl<MaximLedGraphics2DPixelGroup<T_OWNER, C_INSTANCE, C_WIDTH, C_HEIGHT, C_COUNT_PIXELS, C_START_PIXEL>>
	{
		typedef MaximLedGraphics2DPixelGroup<T_OWNER, C_INSTANCE, C_WIDTH, C_HEIGHT, C_COUNT_PIXELS, C_START_PIXEL>	self;
		typedef GraphicsImpl<self> inherited;

	public:
		bool	Enabled : 1;
		GraphicsOrientation	Orientation : 2; //= goUp;

	public:
		bool	MirrorHorizontal : 1;
		bool	MirrorVertical : 1;

		TGraphicsMonochromeTextSettings	Text;

//		uint32_t Width = 8;
//		uint32_t Height = 8;

//	protected:
//		int		FStartPixel;

/*
	public:
		virtual void StartPixels( int &AStartPixel ) override
		{
			FStartPixel = AStartPixel;
//			inherited::StartPixels( AStartPixel );
			AStartPixel += Width * Height;
		}
*/
	public:
		void drawPixel( int16_t x, int16_t y, TUniColor color )
		{
			if( color == tmcNone )
				return;

//			Serial.println( height() );

			if((x < 0) || (x >= width() ) || (y < 0) || (y >= height() ))
				return;

//			Serial.println( "drawPixel" );
			// check rotation, move pixel around if necessary
			switch ( Orientation ) 
			{
				case goRight:
					swap(x, y);
					x = C_WIDTH - x - 1;
					break;

				case goDown:
					x = C_WIDTH - x - 1;
					y = C_HEIGHT - y - 1;
					break;

				case goLeft:
					swap(x, y);
					y = C_HEIGHT - y - 1;
					break;
			}  

			uint32_t AXBlock = x / 8;
			uint32_t AYBlock = y / 8;

			uint32_t AXOffset = x & 0b111;
			uint32_t AYOffset = y & 0b111;

			if( MirrorHorizontal )
				AYOffset = 7 - AYOffset;

			if( MirrorVertical )
				AXOffset = 7 - AXOffset;

			uint32_t ABlockIndex = AYBlock * ( C_WIDTH / 8 ) + AXBlock;
			uint32_t ABlockOffset = AYOffset * 8 + AXOffset;

//			uint32_t AIndex = C_START_PIXEL + y * Width + x;
			uint32_t AIndex = C_START_PIXEL + ABlockIndex * 64 + ABlockOffset;

//			Serial.print( AIndex ); Serial.print( " " ); Serial.println( color );

			switch( color )
			{
				case tmcWhite: C_INSTANCE.SetPixelValue( AIndex, true ); break;
				case tmcBlack: C_INSTANCE.SetPixelValue( AIndex, false ); break;
				case tmcInvert: C_INSTANCE.SetPixelValue( AIndex, ! C_INSTANCE.GetPixelValue( AIndex )); break;
			}
		}

		TUniColor GetPixelColor( int16_t x, int16_t y )
		{
			if ((x < 0) || (x >= width() ) || (y < 0) || (y >= height() ))
				return false;

			// check rotation, move pixel around if necessary
			switch ( Orientation ) 
			{
				case goRight:
					swap(x, y);
					x = C_WIDTH - x - 1;
					break;

				case goDown:
					x = C_WIDTH - x - 1;
					y = C_HEIGHT - y - 1;
					break;

				case goLeft:
					swap(x, y);
					y = C_HEIGHT - y - 1;
					break;
				}  

			uint32_t AXBlock = x / 8;
			uint32_t AYBlock = y / 8;

			uint32_t AXOffset = x & 0b111;
			uint32_t AYOffset = y & 0b111;

			if( MirrorHorizontal )
				AYOffset = 7 - AYOffset;

			if( MirrorVertical )
				AXOffset = 7 - AXOffset;

			uint32_t ABlockIndex = AYBlock * ( width() / 8 ) + AXBlock;
			uint32_t ABlockOffset = AYOffset * 8 + AXOffset;

//			uint32_t AIndex = C_START_PIXEL + y * Width + x;
			uint32_t AIndex = C_START_PIXEL + ABlockIndex * 64 + ABlockOffset;

			return ( C_INSTANCE.GetPixelValue( AIndex ) );
		}

	public:
		inline int16_t width(void) const
		{
			if( Orientation == goUp || Orientation == goDown )
				return C_WIDTH;

			return C_HEIGHT;
		}

		inline int16_t height(void) const
		{
			if( Orientation == goUp || Orientation == goDown )
				return C_HEIGHT;

			return C_WIDTH;
		}

	public:
		inline void SystemStart()
		{
			inherited::setTextSize( Text.Size );
			inherited::setTextColor( Text.Color, Text.BackgroundColor );
			inherited::setTextWrap( Text.Wrap );

			inherited::RenderElements();
//			for( int i = 0; i < FElements.size(); ++ i )
//				FElements[ i ]->Render( false );
		}

	public:
		inline static size_t write( self &AImplementation, uint8_t AValue )
		{
			return inherited::write( AImplementation, AValue );
		}

		template<typename T> void PrintChar( T AValue )
		{
			write( *this, AValue );
		}

	public:
		template<typename T> void Print( T AValue )
		{
			println( AValue );
		}

	public:
		template<typename T> size_t print( T AValue )
		{
			GraphicsPrint<self> APrint( this, (typename GraphicsPrint<self>::TCallback) &self::write );
			return APrint.print( AValue );
		}

		template<typename T> size_t println( T AValue )
		{
			GraphicsPrint<self> APrint( this, (typename GraphicsPrint<self>::TCallback) &self::write );
			return APrint.println( AValue );
		}

	public:
		MaximLedGraphics2DPixelGroup( const unsigned char * AFont ) :
			inherited( AFont ),
			Enabled( true ),
			Orientation( goUp ),
			MirrorHorizontal( false ),
			MirrorVertical( false )
		{
		}

	};
//---------------------------------------------------------------------------
	class MaximLedPixelsReversedProperty
	{
	public:
		bool Reversed : 1;
		bool AllPixels : 1;

	public:
		bool FCurrentReversed : 1;

	public:
		MaximLedPixelsReversedProperty() :
			Reversed( false ),
			AllPixels( false ),
			FCurrentReversed( false )
		{
		}
	};
//---------------------------------------------------------------------------
	template<typename T_OWNER, T_OWNER &C_INSTANCE, uint16_t C_COUNT_PIXELS, uint16_t C_START_PIXEL> class MaximLedRunningPixelGroup
	{
	public:
		OpenWire::SourcePin	OutputPin;

	public:
		bool	Enabled : 1;
		bool	InitialValue : 1;
		MaximLedPixelsReversedProperty	Reversed;

	public:
		void UpdateReversePixels()
		{
			if( Reversed.AllPixels )
				if( Reversed.Reversed != Reversed.FCurrentReversed )
				{
					Reversed.FCurrentReversed = Reversed.Reversed;

					for( int i = 0; i < C_COUNT_PIXELS / 2; ++i )
					{
						bool AOldValue1 = C_INSTANCE.GetPixelValue( C_START_PIXEL + ( C_COUNT_PIXELS - i - 1 ));
						bool AOldValue2 = C_INSTANCE.GetPixelValue( C_START_PIXEL + i );

						C_INSTANCE.SetPixelValue( C_START_PIXEL + i, AOldValue1 );
						C_INSTANCE.SetPixelValue( C_START_PIXEL + ( C_COUNT_PIXELS - i - 1 ), AOldValue2 );
					}
				}

		}

	protected:
		void AnimatePixels()
		{
			if( Reversed.Reversed )
			{
				bool AOldValue = C_INSTANCE.GetPixelValue( C_START_PIXEL );
				OutputPin.Notify( &AOldValue );
				for( int i = 0; i < C_COUNT_PIXELS - 1; ++i )
				{
					AOldValue = C_INSTANCE.GetPixelValue( C_START_PIXEL + i + 1 );
					C_INSTANCE.SetPixelValue( C_START_PIXEL + i, AOldValue );
				}

				C_INSTANCE.SetPixelValue( C_START_PIXEL + C_COUNT_PIXELS - 1, InitialValue );
			}

			else
			{
				bool AOldValue = C_INSTANCE.GetPixelValue( C_START_PIXEL + C_COUNT_PIXELS - 1 );
				OutputPin.Notify( &AOldValue );
				for( int i = C_COUNT_PIXELS - 1; i--; )
				{
					AOldValue = C_INSTANCE.GetPixelValue( C_START_PIXEL + i );
					C_INSTANCE.SetPixelValue( C_START_PIXEL + i + 1, AOldValue );
				}

				C_INSTANCE.SetPixelValue( C_START_PIXEL, InitialValue );
			}
//			FOwner->FModified = true;
		}

	public:
		void UpdateDisplay() // Used by Enabled setter!
		{
			if( Enabled )
				C_INSTANCE.SetPixelValue( C_START_PIXEL + InitialValue, InitialValue );

			else
				C_INSTANCE.SetPixelValue( C_START_PIXEL + InitialValue, false );
		}

	public:
		inline void StepInputPin_o_Receive( void *_Data )
		{
			AnimatePixels();
		}

		void InputPin_o_Receive( void *_Data )
		{
			bool AValue = *(bool *)_Data;
			if( InitialValue == AValue )
				return;

			InitialValue = AValue;
			UpdateDisplay();
		}

	public:
		inline void SystemStart()
		{
			UpdateDisplay();
		}

		inline void SystemLoopEnd( unsigned long currentMicros )
		{
			AnimatePixels();
		}

	public:
		MaximLedRunningPixelGroup() :
			Enabled( true ),
			InitialValue( false )
		{
		}
	};
//---------------------------------------------------------------------------
	template<typename T_OWNER, T_OWNER &C_INSTANCE, uint16_t C_COUNT_PIXELS, uint16_t C_START_PIXEL, uint16_t C_REPEAT_COUNT, uint16_t C_SUB_PIXEL_COUNT> class MaximLedRepeatGroup
	{
	public:
		bool	Enabled : 1;
		bool	InitialValue : 1;

	public:
		void SetPixelValue( int AIndex, bool AValue )
		{
			for( int i = 0; i < C_REPEAT_COUNT; ++i )
				C_INSTANCE.SetPixelValue( C_START_PIXEL + AIndex + i * C_SUB_PIXEL_COUNT, AValue );
		}

		bool GetPixelValue( int AIndex )
		{
			return C_INSTANCE.GetPixelValue( C_START_PIXEL + AIndex );
		}

	public:
		void UpdateDisplay() // Used by Enabled setter!
		{
			// TODO: Try to implement
		}

/*
		virtual void StartPixels( int &AStartPixel )  override
		{
			FStartPixel = AStartPixel;
			inherited::StartPixels( AStartPixel );

			FSubPixelCount = 0;
			for( int i = 0; i < FPixelGroups.size(); ++i )
				FPixelGroups[ i ]->StartPixels( FSubPixelCount );

			if( FSubPixelCount == 0 )
				FRepeatCount = 0;

			else
				FRepeatCount = ( C_COUNT_PIXELS + FSubPixelCount - 1 ) / FSubPixelCount;

		}
*/
	public:
		MaximLedRepeatGroup() :
			Enabled( true ),
			InitialValue( false )
		{
		}
	};
//---------------------------------------------------------------------------
}

#ifdef __TEST_FOR_DEBUG_PRINTS__
#undef Serial
#endif

#endif
