////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2018 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_BUTTON_h
#define _MITOV_BUTTON_h

#include <Mitov.h>

#ifdef __TEST_FOR_DEBUG_PRINTS__
#define Serial UNGUARDED DEBUG PRINT!!!
#endif

namespace Mitov
{
	class ESP32HallSensorModule
	{
	public:
		OpenWire::TypedSourcePin<float>	OutputPin;

	public:
		bool	Enabled = true;

	public:
		void ClockInputPin_o_Receive( void *_Data )
		{
			if( Enabled )
			{
				float AValue = hallRead();
				OutputPin.Notify( &AValue );
			}
		}

	protected:
		void ReadSensor( bool AOnlyChange )
		{
			if( Enabled )
			{
				float AValue = hallRead();
				OutputPin.SetValue( AValue, AOnlyChange );
			}
		}

	public:
		inline void SystemLoopBegin( unsigned long currentMicros )
		{
			ReadSensor( true );
		}

		inline void SystemStart()
		{
			ReadSensor( false );
		}

	};
}

#ifdef __TEST_FOR_DEBUG_PRINTS__
#undef Serial
#endif

#endif
