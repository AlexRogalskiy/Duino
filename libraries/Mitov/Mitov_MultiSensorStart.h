////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2018 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_MULTI_SENSOR_START_h
#define _MITOV_MULTI_SENSOR_START_h

#include <Mitov.h>

namespace Mitov
{
//---------------------------------------------------------------------------
	class MultiSensorPulseStart;
//---------------------------------------------------------------------------
	class MultiSensorStartBasicElement : public OpenWire::InterruptObject // To enable Interrupt implementations
	{
	protected:
		MultiSensorPulseStart	&FOwner;

	public:
		virtual bool StartElement()
		{
			return false;
		}

	public:
		MultiSensorStartBasicElement( MultiSensorPulseStart &AOwner );

	};
//---------------------------------------------------------------------------
	class MultiSensorPulseStart
	{
	public:
		OpenWire::SourcePin	StartOutputPin;

	public:
		uint16_t	PulseTime = 2;
		uint16_t	PauseTime = 100;
		bool		Enabled : 1;

	public:
		bool	ClockInputPin_o_IsConnected : 1;

	protected:
		bool			FClocked : 1;
		bool			FPulsing : 1;
		unsigned long	FStartTime;
		uint16_t		FCountStarted = 0;

	protected:
		SimpleList<MultiSensorStartBasicElement *> FElements;

	public:
		void RegisterElement( MultiSensorStartBasicElement *AElement )
		{
			FElements.push_back( AElement );
		}

		void ElementReady( MultiSensorStartBasicElement *AElement )
		{
//			Serial.print( "READY: " ); Serial.println( FCountStarted );
			if( ! --FCountStarted )
			{
/*
#ifdef _MITOV_MULTI_SENSOR_START_CLOCK_PIN_
				if( ClockInputPin.IsConnected() )
				{
					if( ! FClocked )
						return;
//				Start();
#endif
*/
//				Serial.println( "RESTART" );
				FStartTime = micros();
			}			
		}

	public:
		inline void SystemLoopBegin( unsigned long currentMicros )
		{
			if( !Enabled )
				return;

			if( FPulsing )
			{
				if( ( currentMicros - FStartTime ) < PulseTime )
					return;

				StartOutputPin.SendValue<bool>( false );
				for( int i = 0; i < FElements.size(); ++i )
					if( FElements[ i ]->StartElement())
						++FCountStarted;

				FPulsing = false;
			}

			else
			{
				if( FCountStarted )
					return;

#ifdef _MITOV_MULTI_SENSOR_START_CLOCK_PIN_
				if( ClockInputPin.IsConnected() )
				{
					if( ! FClocked )
						return;
				}
				else
#endif
					if( ( currentMicros - FStartTime ) < ((unsigned long)PauseTime ) * 1000 )
						return;

				StartOutputPin.SendValue<bool>( true );
				FPulsing = true;
				FStartTime = micros();
			}
		}

		inline void SystemStart()
		{
			if( Enabled )
#ifdef _MITOV_MULTI_SENSOR_START_CLOCK_PIN_
				if( ClockInputPin_o_IsConnected )
					if( !FClocked )
						return;
#endif
			StartOutputPin.SendValue<bool>( true );
			FPulsing = true;
			FStartTime = micros();
		}

	public:
		inline void ClockInputPin_o_Receive( void *_Data )
		{
			FClocked = true;
		}

	public:
		MultiSensorPulseStart() :
			ClockInputPin_o_IsConnected( false ),
			Enabled( true ),
			FClocked( false ),
			FPulsing( false )
		{
		}

	};
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
	MultiSensorStartBasicElement::MultiSensorStartBasicElement( MultiSensorPulseStart &AOwner ) : 
		FOwner( AOwner )
	{
		FOwner.RegisterElement( this );
	}
//---------------------------------------------------------------------------
}

#endif
