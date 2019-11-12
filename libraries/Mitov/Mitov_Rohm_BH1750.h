////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2018 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_ROHM_BH1750_H
#define _MITOV_ROHM_BH1750_H

#include <Mitov.h>

namespace Mitov
{
	enum TArduinoLightSensorRohmBH1750ResolutionMode { rmVeryHigh, rmHigh, rmLow };

	template<uint8_t C_ADDRESS, typename T_I2C, T_I2C &C_I2C> class LightSensorBH1750
	{

		// No active state
		static const uint8_t BH1750_POWER_DOWN = 0x00;

		// Wating for measurment command
		static const uint8_t BH1750_POWER_ON = 0x01;

		// Reset data register value - not accepted in POWER_DOWN mode
		static const uint8_t BH1750_RESET = 0x07;

		// Start measurement at 1lx resolution. Measurement time is approx 120ms.
		static const uint8_t BH1750_CONTINUOUS_HIGH_RES_MODE  = 0x10;

		// Start measurement at 0.5lx resolution. Measurement time is approx 120ms.
		static const uint8_t BH1750_CONTINUOUS_HIGH_RES_MODE_2  = 0x11;

		// Start measurement at 4lx resolution. Measurement time is approx 16ms.
		static const uint8_t BH1750_CONTINUOUS_LOW_RES_MODE  = 0x13;

		// Start measurement at 1lx resolution. Measurement time is approx 120ms.
		// Device is automatically set to Power Down after measurement.
		static const uint8_t BH1750_ONE_TIME_HIGH_RES_MODE  = 0x20;

		// Start measurement at 0.5lx resolution. Measurement time is approx 120ms.
		// Device is automatically set to Power Down after measurement.
		static const uint8_t BH1750_ONE_TIME_HIGH_RES_MODE_2  = 0x21;

		// Start measurement at 1lx resolution. Measurement time is approx 120ms.
		// Device is automatically set to Power Down after measurement.
		static const uint8_t BH1750_ONE_TIME_LOW_RES_MODE  = 0x23;

	public:
		OpenWire::SourcePin	OutputPin;

	public:
		float	MeasurementTime = 1.0;

		bool	Enabled : 1;
		bool	ContinuousMode : 1;
		bool	InLumens : 1;
		TArduinoLightSensorRohmBH1750ResolutionMode	Resolution : 2;

	public:
		bool	ClockInputPin_o_IsConnected : 1;

	public:
		void UpdateEnabled()
		{
			if( ! Enabled )
			{
				write8( BH1750_POWER_DOWN );
				return;
			}

			write8( BH1750_POWER_ON );
            delay(10);

			uint8_t AMode;
			if( ContinuousMode || ! ClockInputPin_o_IsConnected  )
			{
				switch ( Resolution )
				{
					case rmVeryHigh : AMode = BH1750_CONTINUOUS_HIGH_RES_MODE_2; break; 
					case rmHigh : AMode = BH1750_CONTINUOUS_HIGH_RES_MODE; break; 
					case rmLow : AMode = BH1750_CONTINUOUS_LOW_RES_MODE; break; 
				}
			}
			else
			{
				switch ( Resolution )
				{
					case rmVeryHigh : AMode = BH1750_ONE_TIME_HIGH_RES_MODE_2; break; 
					case rmHigh : AMode = BH1750_ONE_TIME_HIGH_RES_MODE; break; 
					case rmLow : AMode = BH1750_ONE_TIME_LOW_RES_MODE; break; 
				}
			}

			write8( AMode );
            delay(10);
			UpdateMeasurementTime();
		}

		void UpdateMeasurementTime()
		{
			uint8_t ATime = Func::MapRange<float>( MeasurementTime, 0.45, 3.68, 31, 254 ) + 0.5;

			uint8_t AHighBits = 0b01000000 | ( ATime >> 5 );
			write8( AHighBits );

			uint8_t ALowBits = 0b01100000 | ( ATime & 0b11111 );
			write8( ALowBits );
		}

	protected:
		void write8(uint8_t AData ) 
		{
			C_I2C.beginTransmission( C_ADDRESS );
			C_I2C.write( AData );
			C_I2C.endTransmission();
		}

	public:
		void ClockInputPin_o_Receive( void *_Data )
		{
			if( ! Enabled )
				return;

			uint16_t level;

			C_I2C.beginTransmission( C_ADDRESS );
			C_I2C.requestFrom( C_ADDRESS, uint8_t( 2 ));
			level = C_I2C.read();
			level <<= 8;
			level |= C_I2C.read();
			C_I2C.endTransmission();

			float AFloatLevel = level;
			AFloatLevel = InLumens ? AFloatLevel / 1.2 : AFloatLevel / 0xFFFF;
			OutputPin.Notify( &AFloatLevel );
		}

		void ResetInputPin_o_Receive( void *_Data )
		{
			if( Enabled )
				write8( BH1750_RESET );
		}

	public:
		inline void SystemStart()
		{
			UpdateEnabled();
		}

		inline void SystemLoopBegin( unsigned long currentMicros )
		{
			ClockInputPin_o_Receive( nullptr );
		}

	public:
		LightSensorBH1750() :
			Enabled( true ),
			ContinuousMode( true ),
			InLumens( true ),
			Resolution( rmVeryHigh ),
			ClockInputPin_o_IsConnected( false )
		{
		}

	};

}

#endif
