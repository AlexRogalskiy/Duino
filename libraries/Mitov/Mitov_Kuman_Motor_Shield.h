////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2018 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_KUMAN_MOTOR_SHIELD_h
#define _MITOV_KUMAN_MOTOR_SHIELD_h

#include <Mitov.h>

#ifdef __TEST_FOR_DEBUG_PRINTS__
#define Serial UNGUARDED DEBUG PRINT!!!
#endif

namespace Mitov
{
//---------------------------------------------------------------------------
	class KumanRobotMotorShieldChannel
	{
	public:
		float	InitialValue = 0.5f;
		bool	Enabled : 1;

	protected:
		bool	FIsSecond : 1;

	public:
		void InputPin_o_Receive( void *_Data )
		{
			float ASpeed = constrain( *(float *)_Data, 0.0, 1.0 );
			if( InitialValue == ASpeed )
				return;

			InitialValue = ASpeed;
			UpdateOutputs();
		}

	public:
		void	UpdateOutputs()
		{
			bool AForward;
			bool AReverse;
			float AOutSpeed;
			if( Enabled )
			{
				AOutSpeed = abs( InitialValue - 0.5 ) * 2;
				bool ADirection = InitialValue > 0.5;

				AReverse = ADirection;
				AForward = ! ADirection;
			}

			else
			{
				AOutSpeed = 0.5;
				AReverse = false;
				AForward = false;
			}

			if( FIsSecond )
			{
				digitalWrite( 13, AForward );
				digitalWrite( 12, AReverse );
				analogWrite( 6, ( AOutSpeed * PWMRANGE ) + 0.5 );
			}
			else
			{
				digitalWrite( 7, AForward );
				digitalWrite( 8, AReverse );
				analogWrite( 5, ( AOutSpeed * PWMRANGE ) + 0.5 );
			}

		}

	public:
		inline void SystemStart()
		{
			UpdateOutputs();
		}

		inline void SystemLoopBegin( unsigned long currentMicros ) {} // Placeholder for KumanRobotMotorShieldChannel_Slopped compatibility

	public:
		KumanRobotMotorShieldChannel( bool AIsSecond ) :
			Enabled( true ),
			FIsSecond( AIsSecond )
		{
			if( AIsSecond )
			{
				pinMode( 13, OUTPUT );
				pinMode( 12, OUTPUT );
				pinMode( 6, OUTPUT );
			}
			else
			{
				pinMode( 7, OUTPUT );
				pinMode( 8, OUTPUT );
				pinMode( 5, OUTPUT );
			}
		}
	};
//---------------------------------------------------------------------------
	typedef KumanRobotMotorShieldChannel KumanRobotMotorShieldChannel_Enable;
//---------------------------------------------------------------------------
	class KumanRobotMotorShieldChannel_Slopped
	{
	public:
		float	SlopeRun = 0.0f;
		float	SlopeStop = 0.0f;
		float	InitialValue = 0.5f;
		bool	Enabled : 1;

	protected:
		bool	FIsSecond : 1;
		float	FCurrentValue = 0.0f;
		unsigned long	FLastTime = 0;

	public:
		void	SetEnabled( bool AValue ) 
		{
			if( Enabled == AValue )
				return;

			Enabled = AValue;
			UpdateOutputs();
		}

	public:
		void InputPin_o_Receive( void *_Data )
		{
			float ASpeed = constrain( *(float *)_Data, 0.0, 1.0 );
			if( InitialValue == ASpeed )
				return;

			InitialValue = ASpeed;
			UpdateOutputs();
		}

	public:
		void	UpdateOutputs()
		{
			bool AForward;
			bool AReverse;
			float AOutSpeed;
			if( FCurrentValue != 0.5 )
			{
				AOutSpeed = abs( FCurrentValue - 0.5 ) * 2;
				bool ADirection = FCurrentValue > 0.5;

				AReverse = ADirection;
				AForward = ! ADirection;
			}

			else
			{
				AOutSpeed = 0;
				AReverse = false;
				AForward = false;
			}

			if( FIsSecond )
			{
				digitalWrite( 13, AForward );
				digitalWrite( 12, AReverse );
				analogWrite( 6, ( AOutSpeed * PWMRANGE ) + 0.5 );
			}
			else
			{
				digitalWrite( 7, AForward );
				digitalWrite( 8, AReverse );
				analogWrite( 5, ( AOutSpeed * PWMRANGE ) + 0.5 );
			}
		}

	public:
		inline void SystemStart()
		{
			FCurrentValue = InitialValue;

			UpdateOutputs();
		}

		inline void SystemLoopBegin( unsigned long currentMicros )
		{
			float ATargetValue = (Enabled) ? InitialValue : 0.5;

			if( FCurrentValue != ATargetValue )
			{
				float Slope = SlopeRun;
				if( FCurrentValue > ATargetValue )
				{
					if( ATargetValue > 0.5 )
						Slope = SlopeStop;
				}

				else
				{
					if( ATargetValue < 0.5 )
						Slope = SlopeStop;
				}

				if( Slope == 0 )
					FCurrentValue = ATargetValue;

				else
				{
					float ARamp = abs( ( currentMicros - FLastTime ) * Slope / 1000000 );
					if( FCurrentValue < ATargetValue )
					{
						FCurrentValue += ARamp;
						if( FCurrentValue > ATargetValue )
							FCurrentValue = ATargetValue;

					}
					else
					{
						FCurrentValue -= ARamp;
						if( FCurrentValue < ATargetValue )
							FCurrentValue = ATargetValue;

					}
				}

				UpdateOutputs();
			}

			FLastTime = currentMicros;
		}

	public:
		KumanRobotMotorShieldChannel_Slopped( bool AIsSecond ) :
			Enabled( true ),
			FIsSecond( AIsSecond )
		{
			if( AIsSecond )
			{
				pinMode( 13, OUTPUT );
				pinMode( 12, OUTPUT );
				pinMode( 6, OUTPUT );
			}
			else
			{
				pinMode( 7, OUTPUT );
				pinMode( 8, OUTPUT );
				pinMode( 5, OUTPUT );
			}
		}
	};
//---------------------------------------------------------------------------
	typedef KumanRobotMotorShieldChannel_Slopped KumanRobotMotorShieldChannel_Slopped_Enable;
//---------------------------------------------------------------------------
}

#ifdef __TEST_FOR_DEBUG_PRINTS__
#undef Serial
#endif

#endif
