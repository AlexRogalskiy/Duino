////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2018 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

// http://www.legolab.daimi.au.dk/CSaEA/RCX/Manual.dir/Sensors.dir/Sensors.html
// https://seriousrobotics.wordpress.com/2011/11/20/using-rcx-lego-sensors-with-arduino/

#ifndef _MITOV_LEGO_RCX_LIGHT_SENSOR_h
#define _MITOV_LEGO_RCX_LIGHT_SENSOR_h

#include <Mitov.h>

namespace Mitov
{
	template<int DIGITAL_PIN_NUMBER, int ANALOG_PIN_NUMBER> class LEGO_RCX_LightSensor
	{
	public:
		OpenWire::SourcePin			OutputPin;

	public:
		uint32_t ChargePeriod = 2;
		uint32_t MinValue = 90;
		uint32_t MaxValue = 160;

		bool Normalize : 1;
		bool Enabled : 1;

	public:
		bool ClockInputPin_o_IsConnected : 1;

	protected:
		bool			FHasValue : 1;

		float			FValue;
		unsigned long	FStartTime;

	public:
		inline void SystemStart()
		{
			FStartTime = micros();
			pinMode( DIGITAL_PIN_NUMBER, OUTPUT );
			digitalWrite( DIGITAL_PIN_NUMBER, HIGH );
		}

		inline void SystemLoopBegin( unsigned long currentMicros )
		{
			if( ( currentMicros - FStartTime ) >= ChargePeriod * 1000 )
			{
				pinMode( DIGITAL_PIN_NUMBER, INPUT );

				analogRead( ANALOG_PIN_NUMBER ); // Give some settle time
				FValue = analogRead( ANALOG_PIN_NUMBER );
				if( Normalize )
					FValue = ( FValue - MinValue ) / (MaxValue - MinValue);

				pinMode( DIGITAL_PIN_NUMBER, OUTPUT );
				digitalWrite( DIGITAL_PIN_NUMBER, HIGH );
				FHasValue = true;

				FStartTime = micros();
			}

			if( ! ClockInputPin_o_IsConnected )
				ReadSensor();

		}

	protected:
		void ReadSensor()
		{
			if( FHasValue )
				OutputPin.Notify( &FValue );
		}

	public:
		inline void ClockInputPin_o_Receive( void *_Data )
		{
			ReadSensor();
		}

	public:
		LEGO_RCX_LightSensor() :
			Normalize( true ),
			Enabled( true ),
			ClockInputPin_o_IsConnected( false ),
			FHasValue( false )
		{
		}
	};
}

#endif
