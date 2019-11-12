////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2018 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_PWM_SERVO_h
#define _MITOV_PWM_SERVO_h

#include <Mitov.h>

#include <PWMServo.h>

namespace Mitov
{
	template<int PIN_NUMBER, int PULSE_MIN, int PULSE_MAX> class MitovPWMServo
	{
	public:
		bool	Enabled = true;
		float	InitialValue = 0.5;

	public:
		void SetEnabled( bool AValue )
		{
			if( Enabled == AValue )
				return;

			Enabled = AValue;
			if( Enabled )
			{
				FServo.attach( PIN_NUMBER, PULSE_MIN, PULSE_MAX );
				InputPin_o_Receive( &InitialValue );
			}
			else
				FServo.detach();
		}

	protected:
		PWMServo	FServo;
		
	public:
		void InputPin_o_Receive( void *_Data )
		{
			InitialValue = *((float *)_Data);
			if( ! Enabled )
				return;

			float AValue = constrain( InitialValue, 0.0f, 1.0f ) * 180;
			FServo.write( AValue );
		}

	public:
		inline void SystemStart()
		{
			if( Enabled )
			{
				FServo.attach( PIN_NUMBER, PULSE_MIN, PULSE_MAX );
				InputPin_o_Receive( &InitialValue );
			}
//			float AValue = constrain( InitialValue, 0.0f, 1.0f ) * 180;
//			FServo.write( AValue );
		}

	};
//---------------------------------------------------------------------------
	template<int PIN_NUMBER, int PULSE_MIN, int PULSE_MAX> class PWMServoShieldElement
	{
	protected:
		PWMServo	FServo;

	public:
		inline void SystemInit( float AInitialValue )
		{
			FServo.attach( PIN_NUMBER, PULSE_MIN, PULSE_MAX );
			InputPin_o_Receive( &AInitialValue );
		}

	public:
		void InputPin_o_Receive( void *_Data )
		{
			float AValue = constrain( *((float *)_Data), 0.0f, 1.0f ) * 180;
			FServo.write( AValue );
		}
	};
//---------------------------------------------------------------------------
}

#endif
