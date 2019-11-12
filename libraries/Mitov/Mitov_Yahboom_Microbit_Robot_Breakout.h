////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2018 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_YAHBOOM_MICROBIT_ROBOT_BREAKOUT_h
#define _MITOV_YAHBOOM_MICROBIT_ROBOT_BREAKOUT_h

#include <Mitov_PWM_PCA9685.h>

#ifdef __TEST_FOR_DEBUG_PRINTS__
#define Serial UNGUARDED DEBUG PRINT!!!
#endif

namespace Mitov
{
#ifdef VISUINO_ESP32
	const	uint8_t YahboomMicrobitRobotNeoPixelPin = 5;
	const	uint8_t YahboomMicrobitRobotIRReceiverPin = 16;
	const	uint8_t YahboomMicrobitRobotIRObstacleDetectPin = 13;
	const	uint8_t YahboomMicrobitRobotIRObstacleLightPin = 17;
	const	uint8_t YahboomMicrobitRobotBuzzerPin = 25;
	const	uint8_t YahboomMicrobitRobotLeftLineFollowingSensorPin = 33;
	const	uint8_t YahboomMicrobitRobotRightLineFollowingSensorPin = 32;
	const	uint8_t YahboomMicrobitRobotSonicPingPin = 19;
	const	uint8_t YahboomMicrobitRobotSonicEchoPin = 23;
#else
	const	uint8_t YahboomMicrobitRobotNeoPixelPin = 16;
	const	uint8_t YahboomMicrobitRobotIRReceiverPin = 8;
	const	uint8_t YahboomMicrobitRobotIRObstacleDetectPin = 3;
	const	uint8_t YahboomMicrobitRobotIRObstacleLightPin = 9;
	const	uint8_t YahboomMicrobitRobotBuzzerPin = 0;
	const	uint8_t YahboomMicrobitRobotLeftLineFollowingSensorPin = 2;
	const	uint8_t YahboomMicrobitRobotRightLineFollowingSensorPin = 1;
	const	uint8_t YahboomMicrobitRobotSonicPingPin = 14;
	const	uint8_t YahboomMicrobitRobotSonicEchoPin = 15;
#endif

//---------------------------------------------------------------------------
	template<typename T_I2C, T_I2C &C_I2C> class YahboomMicrobitPWM : public PCA9685PWM<65, T_I2C, C_I2C>
	{
		typedef PCA9685PWM<65, T_I2C, C_I2C> inherited;

	public:
		YahboomMicrobitPWM()
		{
			inherited::Frequency = 60; // For Servo compatibility
		}
	};
//---------------------------------------------------------------------------
	template<typename T_PWM, T_PWM &C_PWM> class YahboomMicrobitRobotLEDLights
	{
	public:
		TColor	InitialValue;

	public:
		inline void SystemStart()
		{
			C_PWM.SetChannelValue( 0, float( InitialValue.Red ) / 255 );
			C_PWM.SetChannelValue( 1, float( InitialValue.Green ) / 255 );
			C_PWM.SetChannelValue( 2, float( InitialValue.Blue ) / 255 );
		}

	public:
		void InputPin_o_Receive( void *_Data )
		{
			TColor AColor = *(TColor *)_Data;
			if( AColor == InitialValue )
				return;

			InitialValue = AColor;
			SystemStart();
		}

	};
//---------------------------------------------------------------------------
	template<uint8_t C_INDEX, typename T_OWNER, T_OWNER &C_OWNER> class YahboomMicrobitRobotLED
	{
	public:
		float	InitialValue = 0;

	public:
		inline void SystemStart()
		{
			C_OWNER.SetChannelValue( C_INDEX, 1 - InitialValue );
		}

	public:
		void InputPin_o_Receive( void *_Data )
		{
			float AValue = *(float *)_Data;
			if( AValue == InitialValue )
				return;

			InitialValue = AValue;
			SystemStart();
		}
	};
//---------------------------------------------------------------------------
	template<uint8_t C_INDEX, typename T_OWNER, T_OWNER &C_OWNER> class YahboomMicrobitRobotLED_Enable
	{
	public:
		bool	Enabled = false;
		float	InitialValue = 0;

	public:
		void UpdateOutputs()
		{
			if( Enabled )
				C_OWNER.SetChannelValue( C_INDEX, 1 - InitialValue );

			else
				C_OWNER.SetChannelValue( C_INDEX, 1 );

		}

	public:
		inline void SystemStart()
		{
			UpdateOutputs();
		}

	public:
		void InputPin_o_Receive( void *_Data )
		{
			float AValue = *(float *)_Data;
			if( AValue == InitialValue )
				return;

			InitialValue = AValue;
			UpdateOutputs();
		}
	};
//---------------------------------------------------------------------------
	template<uint8_t C_INDEX, typename T_OWNER, T_OWNER &C_OWNER> class YahboomMicrobitRobotServo
	{
	public:
		float	InitialValue = 0;

	public:
		inline void SystemStart()
		{
			float	AValue = constrain( InitialValue, 0.0, 1.0 );
			AValue = Func::MapRange( AValue, 0.0f, 1.0f, 0.036630037f, 0.146520147f );
			C_OWNER.SetChannelValue( C_INDEX + 3, AValue );
		}

	public:
		void InputPin_o_Receive( void *_Data )
		{
			float AValue = *(float *)_Data;
			if( AValue == InitialValue )
				return;

			InitialValue = AValue;
			SystemStart();
		}
	};
//---------------------------------------------------------------------------
	template<uint8_t C_INDEX, typename T_OWNER, T_OWNER &C_OWNER> class YahboomMicrobitRobotServo_Enable
	{
	public:
		bool	Enabled = false;
		float	InitialValue = 0;

	public:
		void UpdateOutputs()
		{
			if( Enabled )
			{
				float	AValue = constrain( InitialValue, 0.0, 1.0 );
				AValue = Func::MapRange( AValue, 0.0f, 1.0f, 0.036630037f, 0.146520147f );
				C_OWNER.SetChannelValue( C_INDEX + 3, AValue );
			}

		}

	public:
		inline void SystemStart()
		{
			UpdateOutputs();
		}

	public:
		void InputPin_o_Receive( void *_Data )
		{
			float AValue = *(float *)_Data;
			if( AValue == InitialValue )
				return;

			InitialValue = AValue;
			UpdateOutputs();
		}
	};
//---------------------------------------------------------------------------
	template<typename T_OWNER, T_OWNER &C_OWNER, uint8_t C_INDEX> class YahboomMicrobitRobotRGBLED
	{
	public:
		TColor	InitialValue;

	public:
		inline void SystemStart()
		{
			C_OWNER.SetPixelColor( C_INDEX, TRGBWColor( InitialValue ));
//			C_OWNER.SetChannelValue( C_INDEX, 1 - InitialValue );
		}

	public:
		void InputPin_o_Receive( void *_Data )
		{
			TColor AColor = *(TColor *)_Data;
			if( AColor == InitialValue )
				return;

			InitialValue = AColor;
			SystemStart();
		}
	};
//---------------------------------------------------------------------------
	class YahboomMicrobitRobotObstacleAvoidanceSensor
	{
	public:
		bool Enabled = true;

	public:
		OpenWire::SourcePin	OutputPin;

	public:
		void UpdateEnabled()
		{
			digitalWrite( YahboomMicrobitRobotIRObstacleLightPin, ( Enabled ) ? LOW : HIGH );				
		}

	public:
		inline void SystemInit()
		{
			pinMode( YahboomMicrobitRobotIRObstacleLightPin, OUTPUT );
			pinMode( YahboomMicrobitRobotIRObstacleDetectPin, INPUT );
		}

		inline void SystemStart()
		{
			UpdateEnabled();
		}

		inline void SystemLoopBegin( unsigned long currentMicros )
		{
			if( Enabled )
			{
				bool AValue = ! digitalRead( YahboomMicrobitRobotIRObstacleDetectPin );
				OutputPin.Notify( &AValue );
			}

			else
				OutputPin.SendValue<bool>( false );
		}

	};
//---------------------------------------------------------------------------
	class YahboomMicrobitRobotLineFollowingSensors
	{
	public:
		OpenWire::SourcePin	OutputPins[ 2 ];

	public:
		void ReadValues()
		{
			float AValue = analogRead( YahboomMicrobitRobotLeftLineFollowingSensorPin );
			AValue /= VISUINO_ANALOG_IN_RANGE;
			OutputPins[ 0 ].Notify( &AValue );

			AValue = analogRead( YahboomMicrobitRobotRightLineFollowingSensorPin );
			AValue /= VISUINO_ANALOG_IN_RANGE;
			OutputPins[ 1 ].Notify( &AValue );
		}

	public:
		inline void ClockInputPin_o_Receive( void *_Data )
		{
			ReadValues();
		}

	public:
		inline void SystemLoopBegin( unsigned long currentMicros )
		{
			ReadValues();
		}

	};
//---------------------------------------------------------------------------
}

#ifdef __TEST_FOR_DEBUG_PRINTS__
#undef Serial
#endif

#endif // _MITOV_YAHBOOM_MICROBIT_ROBOT_BREAKOUT_h
