////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2018 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_SERVO_h
#define _MITOV_SERVO_h

#include <Mitov.h>

#ifdef VISUINO_ESP32
#else
#include <Servo.h>
#endif

namespace Mitov
{
	template<int PIN_NUMBER, int PULSE_MIN, int PULSE_MAX> class MitovServo
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
#ifdef VISUINO_ESP32
				Mitov::ServoAttach( PIN_NUMBER );
#else
				FServo.attach( PIN_NUMBER, PULSE_MIN, PULSE_MAX );
#endif
				InputPin_o_Receive( &InitialValue );
			}
			else
#ifdef VISUINO_ESP32
				Mitov::ServoDetach( PIN_NUMBER );
#else
				FServo.detach();
#endif
		}

	protected:
#ifndef VISUINO_ESP32
		Servo	FServo;
#endif
		
	public:
		void InputPin_o_Receive( void *_Data )
		{
			InitialValue = *((float *)_Data);
			if( ! Enabled )
				return;

			float AValue = constrain( InitialValue, 0.0f, 1.0f ) * 180;
#ifdef VISUINO_ESP32
			Mitov::ServoWrite( PIN_NUMBER, AValue, PULSE_MIN, PULSE_MAX );
#else
			FServo.write( AValue );
#endif
		}

	public:
		inline void SystemStart()
		{
			if( Enabled )
			{
#ifdef VISUINO_ESP32
				Mitov::ServoAttach( PIN_NUMBER );
#else
				FServo.attach( PIN_NUMBER, PULSE_MIN, PULSE_MAX );
#endif
				InputPin_o_Receive( &InitialValue );
			}
//			float AValue = constrain( InitialValue, 0.0f, 1.0f ) * 180;
//			FServo.write( AValue );
		}

	};
//---------------------------------------------------------------------------
	template<int PIN_NUMBER, int PULSE_MIN, int PULSE_MAX> class ServoShieldElement
	{
	protected:
#ifndef VISUINO_ESP32
		Servo	FServo;
#endif

	public:
		inline void SystemInit( float AInitialValue )
		{
#ifdef VISUINO_ESP32
			Mitov::ServoAttach( PIN_NUMBER );
#else
			FServo.attach( PIN_NUMBER, PULSE_MIN, PULSE_MAX );
#endif
			InputPin_o_Receive( &AInitialValue );
		}

	public:
		void InputPin_o_Receive( void *_Data )
		{
			float AValue = constrain( *((float *)_Data), 0.0f, 1.0f ) * 180;
#ifdef VISUINO_ESP32
			Mitov::ServoWrite( PIN_NUMBER, AValue, PULSE_MIN, PULSE_MAX );
#else
			FServo.write( AValue );
#endif
		}

	};
//---------------------------------------------------------------------------
}

#endif
