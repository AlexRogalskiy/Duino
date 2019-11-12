////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2018 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_BPI_BIT_h
#define _MITOV_BPI_BIT_h

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
	class ArduinoBpiBitThermometer
	{
	protected:
		static const int SERIESRESISTOR = 4700;

		// resistance at 25 degrees C
		static const int THERMISTORNOMINAL = 10000;
		// temp. for nominal resistance (almost always 25 C)
		static const int TEMPERATURENOMINAL = 25;

		// The beta coefficient of the thermistor (usually 3000-4000)
		static const int BCOEFFICIENT = 3950;
		// the value of the 'other' resistor

	public:
		OpenWire::TypedStartSourcePin<float>	OutputPin;

	public:
		bool Enabled : 1;
		bool InFahrenheit : 1;

	public:
		void ReadData( bool ASendAll ) // Used By Setters
		{
			if( ! Enabled )
				 return;

			float reading = analogRead( 34 );

//			Serial.print("Thermistor reading: "); Serial.println(reading);

			// convert the value to resistance
			reading = (( float( VISUINO_ANALOG_IN_RANGE ) * float( SERIESRESISTOR )) / reading);
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
		ArduinoBpiBitThermometer() :
			Enabled( true ),
			InFahrenheit( false )
		{
		}

	};
//---------------------------------------------------------------------------
	template<uint8_t C_PIN> class ArduinoBpiBitLightSensor
	{
	public:
		OpenWire::TypedStartSourcePin<float>	OutputPin;

	public:
		bool Enabled : 1;
		bool InLux : 1;

	public:
		void ReadData( bool ASendAll )
		{
			if( ! Enabled )
				 return;

			float AValue = analogRead( C_PIN ); // / float( VISUINO_ANALOG_IN_RANGE );
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
		ArduinoBpiBitLightSensor() :
			Enabled( true ),
			InLux( false )
		{
		}

	};
//---------------------------------------------------------------------------
	template<typename T_PARENT> class ArduinoBpiBitNeoPixelsModule : public T_PARENT
	{
	public:
		bool	Enabled = true;

	public:
		void	UpdateEnabled()
		{
			digitalWrite( 2, (Enabled) ? HIGH : LOW );
		}

	public:
		int16_t width(void) const
		{
			return 5;
		}

		int16_t height(void) const
		{
			return 5;
		}

	public:
		inline void SystemStart()
		{
//			Serial.println( "TEST1" );
			pinMode( 2, OUTPUT );
			UpdateEnabled();

			T_PARENT::SystemStart();
		}

/*
	public:
		inline void RefreshInputPin_o_Receive()
		{
		}
*/
	public:
		ArduinoBpiBitNeoPixelsModule( float ABrightness, const unsigned char * AFont ) :
			T_PARENT( AFont )
		{
			T_PARENT::SetBrightness( ABrightness );
		}

	};
//---------------------------------------------------------------------------
}

#ifdef __TEST_FOR_DEBUG_PRINTS__
#undef Serial
#endif

#endif // _MITOV_BPI_BIT_h
