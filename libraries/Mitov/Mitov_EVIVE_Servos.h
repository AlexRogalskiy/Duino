////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2018 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_EVIVE_SERVOS_h
#define _MITOV_EVIVE_SERVOS_h

#include <Mitov.h>

#include <Servo.h>

namespace Mitov
{
	class ArduinoEVIVEServos
	{
	protected:
		Servo	FServos[ 2 ];

	public:
		void InputPins_o_Receive( int AIndex, void *_Data )
		{
			float AValue = constrain( *((float *)_Data), 0.0f, 1.0f ) * 180;
			FServos[ AIndex ].write( AValue );
		}

	public:
		inline void SystemStart()
		{
			FServos[ 0 ].attach( 44 );
			FServos[ 1 ].attach( 45 );
		}

	};
//---------------------------------------------------------------------------
}

#endif
