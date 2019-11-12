////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2018 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_KEYPAD_h
#define _MITOV_KEYPAD_h

#include <Mitov.h>

namespace Mitov
{
//---------------------------------------------------------------------------
	class KeypadBasicKeyElement
	{
	public:
		virtual void SetButtonValue( unsigned long currentMicros, bool AValue )
		{
		}

	};
//---------------------------------------------------------------------------
	class BasicPinRead
	{
	public:
		virtual void SetMode( int AMode ) = 0;
		virtual bool DigitalRead() = 0;
	};
//---------------------------------------------------------------------------
	template<typename T_OWNER, T_OWNER &C_OWNER> class TypedPinRead : public BasicPinRead
	{
	public:
		virtual void SetMode( int AMode ) override
		{
			C_OWNER.SetMode( AMode );
		}

		virtual bool DigitalRead() override
		{
			return C_OWNER.DigitalRead();
		}
	};
//---------------------------------------------------------------------------
	template<int C_X, int C_Y> class Keypad
	{
	public:
		Mitov::SimpleList<OpenWire::SourcePin>	ColumnsOutputPins;

	public:
		Mitov::BasicPinRead	**_Pins;

	public:
		uint32_t	DebounceInterval = 50;

	protected:
		bool	FInScanning = false;

		KeypadBasicKeyElement	*FKeyMap[ C_X ][ C_Y ];

	public:
		inline Keypad<C_X, C_Y> *GetRootOwner()
		{
			return this;
		}

		inline void RegisterKey( int ARow, int ACol, KeypadBasicKeyElement *AKey )
		{
			FKeyMap[ ACol ][ ARow ] = AKey;
		}

	public:
		inline void SystemInit()
		{
			for( int i = 0; i < C_Y ; ++i )
				_Pins[ i ]->SetMode( INPUT_PULLUP );
//				pinMode( _Pins[ i ], INPUT_PULLUP );

		}

		inline void SystemLoopBegin( unsigned long currentMicros )
		{
			if( ! FInScanning )
			{
				FInScanning = true;
				for( int i = 0; i < C_X; ++i )
					ColumnsOutputPins[ i ].SendValue( false );
			}

			for( int i = 0; i < C_Y; ++i )
  				if( ! _Pins[ i ]->DigitalRead() )
					FInScanning = false;

	//		if( ! FInScanning )
	//			Serial.println( "----------------------------" );

			for( int i = 0; i < C_X; ++i )
			{
				if( ! FInScanning )
				{
					for( int j = 0; j < C_X; ++j )
					{
	//					Serial.print( i != j ); Serial.print( " " );
						ColumnsOutputPins[ j ].SendValue( i != j );
					}

	//				Serial.println( "" );
				}

	//			delay( 1000 );

	//			if( ! FInScanning )
//					Serial.println( "+++++++++++++++++++++++++" );

				ComponentsHardware::SystemUpdateHardware();
				for( int j = 0; j < C_Y; ++j )
					if( FKeyMap[ i ][ j ] )
					{
	/*
						if( ! FInScanning )
						{
							if( ! _Pins[ j ]->DigitalRead() )
							{
								Serial.print( i ); Serial.print( "," ); Serial.println( j );
							}
	//						Serial.print( i ); Serial.print( "," ); Serial.print( j ); Serial.print( " = " ); Serial.print( _Pins[ j ]->DigitalRead() ); Serial.print( "  " ); 
						}
	*/
						FKeyMap[ i ][ j ]->SetButtonValue( currentMicros, _Pins[ j ]->DigitalRead() );
					}

	//			if( ! FInScanning )
	//				Serial.println( "" );
			}
		}

	};
//---------------------------------------------------------------------------
/*
	template<typename T_OWNER, T_OWNER &C_OWNER> class KeypadKeyElement : public KeypadBasicKeyElement
	{
		typedef KeypadBasicKeyElement inherited;

	public:
		KeypadKeyElement()
		{
		}

		KeypadKeyElement( int ARow, int ACol )
		{
			AOwner.RegisterKey( ACol, ARow, this );
		}

	};
*/
//---------------------------------------------------------------------------
	template<typename T_OWNER, T_OWNER &C_OWNER> class DigitalKeypadKey : public KeypadBasicKeyElement
	{
		typedef KeypadBasicKeyElement inherited;

	public:
		OpenWire::SourcePin	OutputPin;

	protected:
		unsigned long	FLastTime = 0;
		bool	FValue : 1;
		bool	FLastValue : 1;

	public:
		virtual void SetButtonValue( unsigned long currentMicros, bool AValue )
		{
			if( AValue != FLastValue )
				FLastTime = millis();

			FLastValue = AValue;
			if( FValue != FLastValue )
				if( millis() - FLastTime > C_OWNER.DebounceInterval )
				{
					FValue = FLastValue;
					OutputPin.SendValue( ! FValue );
				}

		}

	public:
		DigitalKeypadKey( int ARow, int ACol ) :
			FLastValue( false ),
			FValue( false )
		{
			C_OWNER.RegisterKey( ACol, ARow, this );
		}
	};
//---------------------------------------------------------------------------
	template<typename T_OWNER, T_OWNER &C_OWNER> class CharacterKeyGroup : public KeypadBasicKeyElement
	{
		typedef KeypadBasicKeyElement inherited;

	public:
		OpenWire::SourcePin	OutputPin;

	public:
		inline T_OWNER *GetRootOwner()
		{
			return C_OWNER.GetRootOwner();
		}

		inline void RegisterKey( int ARow, int ACol, KeypadBasicKeyElement *AKey )
		{
			C_OWNER.RegisterKey( ARow, ACol, AKey );
		}

	};
//---------------------------------------------------------------------------
	template<typename T_OWNER, T_OWNER &C_OWNER> class CharKeypadKey : public KeypadBasicKeyElement
	{
		typedef KeypadBasicKeyElement inherited;

	public:
		char	Character = 'a';

	protected:
		unsigned long	FLastTime = 0;
		bool	FValue : 1;
		bool	FLastValue : 1;

	public:
		virtual void SetButtonValue( unsigned long currentMicros, bool AValue )
		{
			if( AValue != FLastValue )
				FLastTime = millis();

			FLastValue = AValue;
			if( FValue != FLastValue )
				if( millis() - FLastTime > C_OWNER.GetRootOwner()->DebounceInterval )
				{
					FValue = FLastValue;
					if( ! FValue )
						C_OWNER.OutputPin.Notify( &Character );

//					OutputPin.Notify( &FValue );
				}

		}

	public:
		CharKeypadKey( int ARow, int ACol ) :
			FValue( false ),
			FLastValue( false )
		{
			C_OWNER.RegisterKey( ACol, ARow, this );
		}
	};
//---------------------------------------------------------------------------
}

#endif
