////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2018 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_CIRCUIT_PLAYGROUND_h
#define _MITOV_CIRCUIT_PLAYGROUND_h

#include <Mitov.h>

#ifdef __TEST_FOR_DEBUG_PRINTS__
#define Serial UNGUARDED DEBUG PRINT!!!
#endif

namespace Mitov
{
//---------------------------------------------------------------------------
/*
	class ArduinoCircuitPlaygroundButtons
	{
	public:
		OpenWire::TypedSourcePin<bool>	OutputPins[ 2 ];

	public:
		inline void SystemLoopBegin( unsigned long currentMicros )
		{
	    	OutputPins[ 0 ].SetValue( digitalRead( 4 ) != LOW, true );
	    	OutputPins[ 1 ].SetValue( digitalRead( 19 ) != LOW, true );
		}

		inline void SystemStart()
		{
	    	OutputPins[ 0 ].SetValue( digitalRead( 4 ) != LOW, false );
	    	OutputPins[ 1 ].SetValue( digitalRead( 19 ) != LOW, false );
		}
	};
*/
//---------------------------------------------------------------------------
	class ArduinoCircuitPlaygroundThermometer
	{
	protected:
		static const int SERIESRESISTOR = 10000;

		// resistance at 25 degrees C
		static const int THERMISTORNOMINAL = 10000;
		// temp. for nominal resistance (almost always 25 C)
		static const int TEMPERATURENOMINAL = 25;

		// The beta coefficient of the thermistor (usually 3000-4000)
		static const int BCOEFFICIENT = 3380;
		// the value of the 'other' resistor

	public:
		OpenWire::TypedStartSourcePin<float>	OutputPin;

	public:
		bool Enabled : 1;
		bool InFahrenheit : 1;

	public:
		void ReadData( bool ASendAll )
		{
			if( ! Enabled )
				 return;

			float reading = analogRead( A0 );

			//Serial.print("Thermistor reading: "); Serial.println(reading);

			// convert the value to resistance
			reading = ((1023.0 * float( SERIESRESISTOR )) / reading);
			reading -= SERIESRESISTOR;

			//Serial.print("Thermistor resistance: "); Serial.println(reading);

			float steinhart = reading / THERMISTORNOMINAL;     // (R/Ro)
			steinhart = log(steinhart);                  // ln(R/Ro)
			steinhart /= BCOEFFICIENT;                   // 1/B * ln(R/Ro)
			steinhart += 1.0 / (TEMPERATURENOMINAL + 273.15); // + (1/To)
			steinhart = 1.0 / steinhart;                 // Invert
			steinhart -= 273.15;                         // convert to C

			if( InFahrenheit )
				steinhart = steinhart * ( 9.0/5.0 ) + 32.0;

			OutputPin.SetValue( steinhart, ASendAll );
		}

	public:
		inline void ClockInputPin_o_Receive( void *_Data )
		{
			ReadData( true );
		}

	public:
		inline void SystemLoopBegin( unsigned long currentMicros )
		{
			ReadData( false );
		}

		inline void SystemStart()
		{
			ReadData( true );
		}

	public:
		ArduinoCircuitPlaygroundThermometer() :
			Enabled( true ),
			InFahrenheit( false )
		{
		}

	};
//---------------------------------------------------------------------------
	class ArduinoCircuitPlaygroundLightSensor
	{
	public:
		OpenWire::TypedStartSourcePin<float>	OutputPin;

	public:
		bool Enabled : 1;
		bool InLux : 1;

	public:
		void ReadData( bool ASendAll ) // Used By Setters
		{
			if( ! Enabled )
				 return;

			float AValue = analogRead( A5 ) / float( VISUINO_ANALOG_IN_RANGE );
			if( InLux )
				AValue *= 1500.0;

			OutputPin.SetValue( AValue, ASendAll );
		}

	public:
		inline void ClockInputPin_o_Receive( void *_Data )
		{
			ReadData( true );
		}

	public:
		inline void SystemLoopBegin( unsigned long currentMicros )
		{
			ReadData( false );
		}

		inline void SystemStart()
		{
			ReadData( true );
		}

	public:
		ArduinoCircuitPlaygroundLightSensor() :
			Enabled( true ),
			InLux( false )
		{
		}

	};
//---------------------------------------------------------------------------
	class ArduinoCircuitPlaygroundMicrophone
	{
	public:
		OpenWire::SourcePin	OutputPin;

	public:
		bool Enabled = true;

	public:
		void ReadData()
		{
			if( ! Enabled )
				 return;

			float AValue = analogRead( A4 ) / 1023.0;
			OutputPin.Notify( &AValue );
		}

	public:
		inline void ClockInputPin_o_Receive( void *_Data )
		{
			ReadData();
		}

	public:
		inline void SystemLoopBegin( unsigned long currentMicros )
		{
			ReadData();
		}

		inline void SystemStart()
		{
			ReadData();
		}
	};
//---------------------------------------------------------------------------
	class ArduinoCircuitPlaygroundSlideSwitch
	{
	public:
		OpenWire::TypedSourcePin<bool>	OutputPin;

	public:
		inline void SystemLoopBegin( unsigned long currentMicros )
		{
	    	OutputPin.SetValue( digitalRead( 21 ) != LOW, true );
		}

		inline void SystemStart()
		{
	    	OutputPin.SetValue( digitalRead( 21 ) != LOW, false );
		}
	};

//---------------------------------------------------------------------------
}

#ifdef __TEST_FOR_DEBUG_PRINTS__
#undef Serial
#endif

#endif // _MITOV_CIRCUIT_PLAYGROUND_h
