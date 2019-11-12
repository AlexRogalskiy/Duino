////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2018 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_ROMEO_BLE_h
#define _MITOV_ROMEO_BLE_h

#include <Mitov.h>

namespace Mitov
{
//---------------------------------------------------------------------------
	class ArduinoRoMeoBLEButtons
	{
	public:
		OpenWire::TypedSourcePin<bool>	OutputPins[ 5 ];	

	protected:
		void ReadButtons( bool AOnlyChanged )
		{
			const int  adc_key_val[5] = { 30, 150, 360, 535, 760 };

			int input = analogRead(7);
			int AButtonIndex = -1;
			for( int i = 0; i < 5; ++ i )
				if (input < adc_key_val[i])
				{
					AButtonIndex = i;
					break;
				}
		
			for( int i = 0; i < 5; ++ i )
				OutputPins[ i ].SetValue( ( i == AButtonIndex ), AOnlyChanged );

		}

	public:
		inline void SystemStart()
		{
			ReadButtons( false );
		}

		inline void SystemLoopBegin( unsigned long currentMicros )
		{
			ReadButtons( true );
		}

	};
//---------------------------------------------------------------------------
	class ArduinoRoMeoBLEMotors
	{
	protected:
		bool	FMotor1_Populated : 1;
		bool	FMotor2_Populated : 1;
		bool	FMotor1_Modified : 1;
		bool	FMotor2_Modified : 1;

		float	FMotor1_Value = 0.0f;
		float	FMotor2_Value = 0.0f;

	protected:
		void SetSpeed( int ADirectionPin, int ASpeedPin, float AValue )
		{
			AValue = constrain( AValue, 0.0, 1.0 );
			if( AValue > 0.5 )
			{
				digitalWrite( ADirectionPin, HIGH );
				AValue -= 0.5;
			}

			else
			{
				digitalWrite( ADirectionPin, LOW );
				AValue = 0.5 - AValue;
			}

			AValue = AValue * 511 + 0.5;
			analogWrite( ASpeedPin, AValue );
		}

	public:
		void InputPins_o_Receive( int AIndex, void *_Data )
		{
			float AValue = *(float*)_Data;
			if( AIndex )
			{
				if( FMotor2_Populated )
					if( AValue == FMotor2_Value )
						return;

				FMotor2_Populated = true;
				FMotor2_Value = AValue;
				FMotor2_Modified = true;
			}

			else
			{
				if( FMotor1_Populated )
					if( AValue == FMotor1_Value )
						return;

				FMotor1_Populated = true;
				FMotor1_Value = AValue;
				FMotor1_Modified = true;
			}
		}

	public:
		inline void SystemLoopBegin( unsigned long currentMicros )
		{
			if( FMotor1_Modified )
				SetSpeed( 4, 5, FMotor1_Value );

			if( FMotor2_Modified )
				SetSpeed( 7, 6, FMotor2_Value );
		}

		inline void SystemStart()
		{
			SetSpeed( 4, 5, FMotor1_Value );
			SetSpeed( 7, 6, FMotor2_Value );
		}

	public:
		ArduinoRoMeoBLEMotors() :
			FMotor1_Populated( false ),
			FMotor2_Populated( false ),
			FMotor1_Modified( false ),
			FMotor2_Modified( false )
		{
		}
	};
//---------------------------------------------------------------------------
}

#endif
