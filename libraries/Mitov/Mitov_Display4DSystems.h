////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2018 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_DISPLAY_4D_SYSTEMS_h
#define _MITOV_DISPLAY_4D_SYSTEMS_h

#include <Mitov.h>
#include <genieArduino.h>

namespace Mitov
{


/*
	void myGenieEventHandler(void)
	{
		Serial.println( "myGenieEventHandler" );
	}
*/
//	class Display4DSystems;
//---------------------------------------------------------------------------
	template<typename T_OWNER> class ViSiGenieBasicObject
	{
	protected:
		T_OWNER &FOwner;

	public:
		virtual void Start() {}
		virtual void Process() {}

	public:
		ViSiGenieBasicObject( T_OWNER &AOwner ) :
			FOwner( AOwner )
		{
			AOwner.FElements.push_back( this );
		}

	};
//---------------------------------------------------------------------------
	template<typename T_OWNER> class ViSiGenieBasicOutObject
	{
	public:
		virtual bool ProcessOut( Genie &AGenie, genieFrame &Event ) = 0;

	public:
		ViSiGenieBasicOutObject( T_OWNER &AOwner )
		{
			AOwner.FOutElements.push_back( this );
		}

	};
//---------------------------------------------------------------------------
	template<typename T_OWNER, int V_OBJECT, int V_INDEX> class ViSiGenieBasicTypedOutObject : public ViSiGenieBasicOutObject<T_OWNER>
	{
		typedef	ViSiGenieBasicOutObject<T_OWNER> inherited;

	protected:
		virtual void PrcessInValue( uint16_t AValue ) = 0;

	public:
		virtual bool ProcessOut( Genie &AGenie, genieFrame &Event ) override
		{
//			Serial.println( "ProcessOut" );
			if( Event.reportObject.cmd == GENIE_REPORT_EVENT )
			{
/*
				Serial.println( "GENIE_REPORT_EVENT" );
				Serial.print( Event.reportObject.object );
				Serial.print( " - " );
				Serial.println( Event.reportObject.index );
*/
				if( Event.reportObject.object == V_OBJECT )
					if( Event.reportObject.index == V_INDEX )
					{
						uint16_t AValue = AGenie.GetEventData(&Event);
//						Serial.println( AValue );
						PrcessInValue( AValue );

						return true;
					}

				return false;
			}
		}


	public:
		using inherited::inherited;

	};
//---------------------------------------------------------------------------
	template<typename T_OWNER, int V_OBJECT, int V_INDEX> class ViSiGenieTypedCharOut : public ViSiGenieBasicTypedOutObject<T_OWNER, V_OBJECT, V_INDEX>
	{
		typedef	ViSiGenieBasicTypedOutObject<T_OWNER, V_OBJECT, V_INDEX> inherited;

	public:
		OpenWire::SourcePin	OutputPin;

//	public:
//		bool	EnterNewLine;

	protected:
		virtual void PrcessInValue( uint16_t AValue ) override
		{
			char *ATypedValue = (char *)AValue;
			OutputPin.Notify( &ATypedValue );
//			if( EnterNewLine )
//				if( *ATypedValue == '\r' )
//					OutputPin.SendValue( '\n' );
		}

	public:
		using inherited::inherited;

	};
//---------------------------------------------------------------------------
	template<typename T_OWNER, int V_OBJECT, int V_INDEX, typename T_DATA> class ViSiGenieBasicTypedIn : public ViSiGenieBasicObject<T_OWNER>
	{
		typedef	ViSiGenieBasicObject<T_OWNER> inherited;

	public:
		bool	OnlyChanged : 1;

	public:
		bool		FStarted : 1;
		bool		FReceived : 1;

	public:
		T_DATA		FOldValue;
		T_DATA		FValue;

		
	public:
		void InputPin_o_Receive( void *_Data );

		inline void RefreshInputPin_o_Receive( void *_Data )
		{
			FStarted = false;
		}

	public:
		ViSiGenieBasicTypedIn( T_OWNER &AOwner, T_DATA AInitialValue ) :
			inherited( AOwner ),
			FValue( AInitialValue ),
			FOldValue( AInitialValue ),
			FStarted( false ),
			FReceived( false ),
			OnlyChanged( true )
		{
		}
	};
//---------------------------------------------------------------------------
	template<typename T_OWNER, int V_OBJECT, int V_INDEX, typename T_DATA> class ViSiGenieTypedIn : public ViSiGenieBasicTypedIn<T_OWNER, V_OBJECT, V_INDEX, T_DATA>
	{
		typedef	ViSiGenieBasicTypedIn<T_OWNER, V_OBJECT, V_INDEX, T_DATA> inherited;

	protected:
		virtual uint16_t	GetValue()	{ return inherited::FValue; };

	public:
		virtual void Process() override;

	public:
		using inherited::inherited;

	};
//---------------------------------------------------------------------------
	template<typename T_OWNER, int V_OBJECT, int V_INDEX, typename T_DATA> class ViSiGenieTypedInOut : public ViSiGenieTypedIn<T_OWNER, V_OBJECT, V_INDEX, T_DATA>, public ViSiGenieBasicTypedOutObject<T_OWNER, V_OBJECT, V_INDEX>
	{
		typedef	ViSiGenieTypedIn<T_OWNER, V_OBJECT, V_INDEX, T_DATA> inherited;

	public:
		OpenWire::SourcePin	OutputPin;
/*
	public:
		virtual bool ProcessOut( Genie &AGenie, genieFrame &Event )
		{
		}
*/
	protected:
		virtual void PrcessInValue( uint16_t AValue ) override
		{
			T_DATA ATypedValue = (T_DATA)AValue;
			OutputPin.Notify( &ATypedValue );
		}

	public:
		ViSiGenieTypedInOut( T_OWNER &AOwner, T_DATA AInitialValue ) :
			inherited( AOwner, AInitialValue ),
			ViSiGenieBasicTypedOutObject<T_OWNER, V_OBJECT, V_INDEX>( AOwner )
		{
		}
	};
//---------------------------------------------------------------------------
/*
	template<typename T_OWNER, int V_OBJECT, int V_INDEX> class ViSiGenieTypedAnalogIn<T_OWNER> : public ViSiGenieTypedIn<T_OWNER, V_OBJECT, V_INDEX, float>
	{
		typedef	ViSiGenieTypedIn<T_OWNER, V_OBJECT, V_INDEX, float> inherited;

	protected:
		virtual uint16_t	GetValue()	{ return inherited::FValue + 0.5; } override;

	public:
		ViSiGenieTypedAnalogIn( T_OWNER &AOwner ) :
			inherited( AOwner, 0.0 )
		{
		}
	};
*/
//---------------------------------------------------------------------------
	template<typename T_OWNER, int V_OBJECT, int V_INDEX> class ViSiGenieTypedUnsignedIn : public ViSiGenieTypedIn<T_OWNER, V_OBJECT, V_INDEX, uint32_t>
	{
		typedef	ViSiGenieTypedIn<T_OWNER, V_OBJECT, V_INDEX, uint32_t> inherited;

	public:
		ViSiGenieTypedUnsignedIn( T_OWNER &AOwner ) :
			inherited( AOwner, 0 )
		{
		}
	};
//---------------------------------------------------------------------------
	template<typename T_OWNER, int V_OBJECT, int V_INDEX> class ViSiGenieTypedUnsignedInOut : public ViSiGenieTypedInOut<T_OWNER, V_OBJECT, V_INDEX, uint32_t>
	{
		typedef	ViSiGenieTypedInOut<T_OWNER, V_OBJECT, V_INDEX, uint32_t> inherited;

	public:
		uint32_t	InitialValue = 0;

	public:
		virtual void Start() override
		{
			inherited::Start();
			inherited::OutputPin.Notify( &InitialValue );
		}

	public:
		ViSiGenieTypedUnsignedInOut( T_OWNER &AOwner ) :
			inherited( AOwner, 0 )
		{
		}
	};
//---------------------------------------------------------------------------
	template<typename T_OWNER, int V_OBJECT, int V_INDEX> class ViSiGenieTypedDigitalIn : public ViSiGenieTypedIn<T_OWNER, V_OBJECT, V_INDEX, bool>
	{
		typedef	ViSiGenieTypedIn<T_OWNER, V_OBJECT, V_INDEX, bool> inherited;

	protected:
		virtual uint16_t	GetValue() override { return inherited::FValue ? 1 : 0; }

	public:
		ViSiGenieTypedDigitalIn( T_OWNER &AOwner ) :
			inherited( AOwner, false )
		{
		}
	};
//---------------------------------------------------------------------------
	template<typename T_OWNER, int V_OBJECT, int V_INDEX> class ViSiGenieTypedDigitalInOut : public ViSiGenieTypedInOut<T_OWNER, V_OBJECT, V_INDEX, bool>
	{
		typedef	ViSiGenieTypedInOut<T_OWNER, V_OBJECT, V_INDEX, bool> inherited;

	protected:
		virtual uint16_t	GetValue() override	{ return inherited::FValue ? 1 : 0; } 

	protected:
		bool	FOldReceivedValue = false;

	protected:
		virtual void PrcessInValue( uint16_t AValue )
		{
			bool ATypedValue = (bool)AValue;
			if( ! FOldReceivedValue )
				if( ! ATypedValue )
					inherited::OutputPin.SendValue( true );

			FOldReceivedValue = ATypedValue;
			inherited::OutputPin.Notify( &ATypedValue );
		}

	public:
		ViSiGenieTypedDigitalInOut( T_OWNER &AOwner ) :
			inherited( AOwner, false )
		{
		}
	};
//---------------------------------------------------------------------------
	template<typename T_OWNER, int V_OBJECT, int V_INDEX> class ViSiGenieTypedColorInOut : public ViSiGenieTypedInOut<T_OWNER, V_OBJECT, V_INDEX, Mitov::TColor>
	{
		typedef	ViSiGenieTypedInOut<T_OWNER, V_OBJECT, V_INDEX, Mitov::TColor> inherited;

	protected:
		virtual uint16_t	GetValue() override	
		{ 
			return ( ( inherited::FValue.Red >> 3 ) << ( 6 + 5 )) | ( ( inherited::FValue.Green >> 2 ) & 0b111111 ) << 5 | ( ( inherited::FValue.Blue >> 3 ) & 0b11111 );
		}

		virtual void PrcessInValue( uint16_t AValue ) override
		{
			Mitov::TColor ATypedValue; // = AValue;
			ATypedValue.Red = ( AValue & 0b1111100000000000 ) >> ( 6 + 5 - 3 ); //0b1111100000000000 
			ATypedValue.Green = ( AValue & 0b11111100000 ) >> ( 5 - 2 ); 
			ATypedValue.Blue = ( AValue & 0b11111 ) << 3; 
			inherited::OutputPin.Notify( &ATypedValue );
		}

	public:
		ViSiGenieTypedColorInOut( T_OWNER &AOwner ) :
			inherited( AOwner, false )
		{
		}
	};
//---------------------------------------------------------------------------
	template<typename T_OWNER, int V_OBJECT, int V_INDEX> class ViSiGenieTypedStringIn : public ViSiGenieBasicTypedIn<T_OWNER, V_OBJECT, V_INDEX, Mitov::String>
	{
		typedef	ViSiGenieBasicTypedIn<T_OWNER, V_OBJECT, V_INDEX, Mitov::String> inherited;

	public:
		void InputPin_o_Receive( void *_Data );

	protected:
		virtual void Process() override;

	public:
		ViSiGenieTypedStringIn( T_OWNER &AOwner ) :
			inherited( AOwner, "" )
		{
		}
	};
//---------------------------------------------------------------------------
	template<typename T_OWNER, int V_OBJECT, int V_INDEX> class ViSiGenieTypedClockInOut : public ViSiGenieBasicObject<T_OWNER>, public ViSiGenieBasicOutObject<T_OWNER>
	{
		typedef	ViSiGenieBasicObject<T_OWNER> inherited;

	public:
		OpenWire::SourcePin	OutputPin;

	public:
		inline void InputPin_o_Receive( void *_Data )
		{
			inherited::FOwner.FGenie.WriteObject( V_OBJECT, V_INDEX, 0 );
		}


	protected:
		virtual bool ProcessOut( Genie &AGenie, genieFrame &Event ) override
		{
//			Serial.println( "ProcessOut" );
			if( Event.reportObject.cmd == GENIE_REPORT_EVENT )
			{
/*
				Serial.println( "GENIE_REPORT_EVENT" );
				Serial.print( Event.reportObject.object );
				Serial.print( " - " );
				Serial.println( Event.reportObject.index );
*/
				if( Event.reportObject.object == V_OBJECT )
					if( Event.reportObject.index == V_INDEX )
					{
						OutputPin.Notify( nullptr );
//						uint16_t AValue = AGenie.GetEventData(&Event);
//						Serial.println( AValue );
//						PrcessInValue( AValue );

						return true;
					}

				return false;
			}
		}

	public:
		ViSiGenieTypedClockInOut( T_OWNER &AOwner ) :
			inherited( AOwner ),
			ViSiGenieBasicOutObject<T_OWNER>( AOwner )
		{
		}
	};
//---------------------------------------------------------------------------
	template<typename T_OWNER, int V_INDEX, uint8_t C_NUM_CHANNELS> class ViSiGenieSpectrum : public ViSiGenieBasicObject<T_OWNER>
	{
		typedef	ViSiGenieBasicObject<T_OWNER> inherited;

	protected:
		float	FValues[ C_NUM_CHANNELS ];
		uint8_t	FModified[ ( C_NUM_CHANNELS + 7 ) / 8 ];

	public:
		void ColumnsInputPins_o_Receive( int AIndex, void *_Data )
		{
			float AValue = constrain( *(float *)_Data, 0.0f, 1.0f );
			if( FValues[ AIndex ] == AValue )
				return;

			Func::SetBitField( FModified, AIndex, true );
			FValues[ AIndex ] = AValue;
		}
		
	public:
		virtual void Process() override
		{
			for( int i = 0; i < C_NUM_CHANNELS; i ++ )
				if(( FModified[ i / 8 ] & ( 1 << ( i & 0b111 ))) != 0 )
				{
					uint16_t AValue = ( i << 8 ) | ( uint8_t( FValues[ i ] * 255 + 0.5 ));
					inherited::FOwner.FGenie.WriteObject( GENIE_OBJ_SPECTRUM, V_INDEX, AValue );
				}

		}

	public:
		using inherited::inherited;

	};
//---------------------------------------------------------------------------
	template<typename T_SERIAL, T_SERIAL &C_SERIAL> class Display4DSystems
	{
	public:
		OpenWire::SourcePin	ResetOutputPin;

	public:
		float	Contrast = 1.0f;

	public:
		Mitov::SimpleList<ViSiGenieBasicObject<Display4DSystems<T_SERIAL, C_SERIAL>> *>		FElements;
		Mitov::SimpleList<ViSiGenieBasicOutObject<Display4DSystems<T_SERIAL, C_SERIAL>> *>	FOutElements;
		bool	FModified = true;

	public:
		Genie FGenie;

	public:
		void SetContrast( float AValue )
		{
			AValue = constrain( AValue, 0.0, 1.0 );
			if( AValue == Contrast )
				return;

			Contrast = AValue;
			FGenie.WriteContrast( Contrast * 15 + 0.5 );
		}

	public:
		inline void SystemStart() 
		{
			FGenie.Begin( C_SERIAL.GetStream() );

  //FGenie.AttachEventHandler(myGenieEventHandler); // Attach the user function Event Handler for processing events
			ResetOutputPin.SendValue( true );

			delay( 100 );

			ResetOutputPin.SendValue( false );

			delay( 3500 );
			FGenie.WriteContrast( Contrast * 15 + 0.5 );

			for( int i = 0; i < FElements.size(); ++ i )
				FElements[ i ]->Start();

		}

		inline void SystemLoopEnd( unsigned long currentMicros ) 
		{
			FGenie.DoEvents(false); // This calls the library each loop to process the queued responses from the display

			if( FModified )
				for( int i = 0; i < FElements.size(); ++ i )
					FElements[ i ]->Process();

			genieFrame AEvent;
			if( FGenie.DequeueEvent(&AEvent))
			{
//				Serial.println( "DequeueEvent" );
				for( int i = 0; i < FOutElements.size(); ++ i )
					if( FOutElements[ i ]->ProcessOut( FGenie, AEvent ))
						break;
			}
		}
	};
//---------------------------------------------------------------------------
	template<typename T_OWNER, uint16_t C_COUNT_CHANNELS> class ViSiGenieSounds : public ViSiGenieBasicObject<T_OWNER>
	{
		typedef	ViSiGenieBasicObject<T_OWNER> inherited;

	public:
		float	Volume;

	public:
		void	SetVolume( float AValue )
		{
			AValue = constrain( AValue, 0.0, 1.0 );
			if( AValue == Volume )
				return;

			Volume = AValue;
			inherited::FOwner.FGenie.WriteObject( GENIE_OBJ_SOUND, 1, Volume * 100 + 0.5 );
		}

	public:
		virtual void Start() override
		{
			inherited::FOwner.FGenie.WriteObject( GENIE_OBJ_SOUND, 1, Volume * 100 + 0.5 );
		}

	public:
		inline void TracksStartInputPins_o_Receive( int AIndex, void *_Data )
		{
			inherited::FOwner.FGenie.WriteObject( GENIE_OBJ_SOUND, 0, AIndex );
		}

		inline void StopInputPin_o_Receive( void *_Data )
		{
			inherited::FOwner.FGenie.WriteObject( GENIE_OBJ_SOUND, 4, 0 );
		}

		inline void PauseInputPin_o_Receive( void *_Data )
		{
			inherited::FOwner.FGenie.WriteObject( GENIE_OBJ_SOUND, 2, 0 );
		}

		inline void ResumeInputPin_o_Receive( void *_Data )
		{
			inherited::FOwner.FGenie.WriteObject( GENIE_OBJ_SOUND, 3, 0 );
		}

	public:
		using inherited::inherited;

	};
//---------------------------------------------------------------------------
	template<typename T_OWNER, int V_OBJECT, int V_INDEX, typename T_DATA> void ViSiGenieBasicTypedIn<T_OWNER, V_OBJECT, V_INDEX, T_DATA>::InputPin_o_Receive( void *_Data )
	{
		FValue = *(T_DATA *)_Data;
		if( OnlyChanged && FStarted )
			if( FValue == FOldValue )
				return;

		FReceived = true;
		inherited::FOwner.FModified = true;
	}
//---------------------------------------------------------------------------
	template<typename T_OWNER, int V_OBJECT, int V_INDEX, typename T_DATA> void ViSiGenieTypedIn<T_OWNER, V_OBJECT, V_INDEX, T_DATA>::Process()
	{
		if( inherited::OnlyChanged && inherited::FStarted )
			if( inherited::FOldValue == inherited::FValue )
				return;

		if( ! inherited::FReceived )
			return;

		inherited::FStarted = true;
		inherited::FOldValue = inherited::FValue;

		inherited::FOwner.FGenie.WriteObject(V_OBJECT, V_INDEX, GetValue() );
	}
//---------------------------------------------------------------------------
	template<typename T_OWNER, int V_OBJECT, int V_INDEX> void ViSiGenieTypedStringIn<T_OWNER, V_OBJECT, V_INDEX>::InputPin_o_Receive( void *_Data )
	{
		inherited::FValue = (char *)_Data;
		if( inherited::OnlyChanged && inherited::FStarted )
			if( inherited::FValue == inherited::FOldValue )
				return;

		inherited::FReceived = true;
		inherited::FOwner.FModified = true;
	}
//---------------------------------------------------------------------------
	template<typename T_OWNER, int V_OBJECT, int V_INDEX> void ViSiGenieTypedStringIn<T_OWNER, V_OBJECT, V_INDEX>::Process()
	{
		if( inherited::OnlyChanged && inherited::FStarted )
			if( inherited::FOldValue == inherited::FValue )
				return;

		if( ! inherited::FReceived )
			return;

		inherited::FStarted = true;
		inherited::FOldValue = inherited::FValue;

		inherited::FOwner.FGenie.WriteStr(V_INDEX, inherited::FValue.c_str() );
	}
//---------------------------------------------------------------------------
}

#endif
