////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2018 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_HX711_WEIGHT_h
#define _MITOV_HX711_WEIGHT_h

#include <Mitov.h>

namespace Mitov
{
	template<typename T_DATA_PIN, T_DATA_PIN &C_DATA_PIN, int T_CLOCK_PIN> class HX711_Weight
	{
	public:
		OpenWire::SourcePin	OutputPin;

	public:
		float	ScaleDivider = 1.0f;
		float	Offset = 0.0f;
		int32_t	Average = 1;

		bool	UseChannelB : 1;
		bool	Gain128 : 1;
		bool	Powered : 1;

	public:
		bool	ClockInputPin_o_IsConnected;

	protected:
		bool	FNeedsRead : 1;

	protected:
		float	FCurrentValue = 0.0f;

	public:
		void SetScaleDivider( float AValue )
		{
			if( AValue == 0 )
				AValue = 0.000001;

			if( ScaleDivider == AValue )
				return;

			ScaleDivider = AValue;
			SendOutput();
		}

		void SetAverage( int32_t AValue )
		{
			if( AValue < 1 )
				Average = 1;

			else
				Average = AValue;

		}

	public:
		void SendOutput()
		{
			float AValue = ( FCurrentValue + Offset ) / ScaleDivider;
			OutputPin.Notify( &AValue );
		}

		void UpdatePower()
		{
			digitalWrite( T_CLOCK_PIN, LOW );
//			SensorClockOutputPin.SendValue( false );
			if( Powered )
				UpdateGain();

			else
				digitalWrite( T_CLOCK_PIN, HIGH );
//				SensorClockOutputPin.SendValue( true );
		}

		void UpdateGain()
		{
			if( ! Powered )
				return;

			digitalWrite( T_CLOCK_PIN, LOW );
//			SensorClockOutputPin.SendValue( false );
			ReadSensorOnes();
		}

	public:
		inline void SystemInit()
		{
			pinMode( T_CLOCK_PIN, OUTPUT );
			UpdatePower();
		}

		inline void SystemLoopBegin( unsigned long currentMicros )
		{
			if( FNeedsRead || ( ! ClockInputPin_o_IsConnected ) )
				ReadSensor();

		}

	protected:
		void ReadSensor()
		{
//			Serial.println( "ReadSensor" );
			if( OutputPin.IsConnected() )
			{
				if( Powered ) 
				{
//					if( SensorDataInputPin.Value )
					if( C_DATA_PIN.DigitalRead() )
					{
						FNeedsRead = true;
						return;
					}

					uint32_t AAccumulator = 0;
					for( int i = 0; i < Average; ++i )
						AAccumulator += ReadSensorOnes();

					FCurrentValue = AAccumulator / Average;
					FNeedsRead = false;
				}

				SendOutput();
			}
		}

		uint32_t ReadSensorOnes()
		{
			while( C_DATA_PIN.DigitalRead() )
				;

			uint32_t AValue = 0;
			for( int i = 0; i < 24; ++i )
			{
				AValue <<= 1;
				digitalWrite( T_CLOCK_PIN, HIGH );

				if( C_DATA_PIN.DigitalRead() )
					AValue |= 1;

				digitalWrite( T_CLOCK_PIN, LOW );
			}

//			Serial.println( AValue );
			int ACount;
			if( UseChannelB )
				ACount = 2;

			else if( Gain128 )
				ACount = 1;

			else
				ACount = 3;

			// set the channel and the gain factor for the next reading using the clock pin
			for (int i = 0; i < ACount; i++) 
			{
				digitalWrite( T_CLOCK_PIN, HIGH );
				digitalWrite( T_CLOCK_PIN, LOW );
//				SensorClockOutputPin.SendValue( true );
//				SensorClockOutputPin.SendValue( false );
			}				
				
			return AValue;
		}

	public:
		inline void ClockInputPin_o_Receive( void *_Data )
		{
			FNeedsRead = true;
			ReadSensor();
		}

		inline void ZeroInputPin_o_Receive( void *_Data )
		{
//			Serial.print( "TEST---------------: " );
			Offset = -FCurrentValue;
//			Serial.println( Offset );
		}

		void CalibrateInputPin_o_Receive( void *_Data )
		{
			ScaleDivider = ( FCurrentValue + Offset );
			if( ! ScaleDivider )
				ScaleDivider = 1.0;
		}

	public:
		HX711_Weight() :
			UseChannelB( false ),
			Gain128( false ),
			FNeedsRead( false ),
			Powered( true ),
			ClockInputPin_o_IsConnected( false )
		{
		}

	};
}

#endif
