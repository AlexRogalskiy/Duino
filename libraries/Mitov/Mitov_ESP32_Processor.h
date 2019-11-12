////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2018 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_ESP32_PROCESSOR_h
#define _MITOV_ESP32_PROCESSOR_h

#include <Mitov.h>
#include <Esp.h>

#ifdef __TEST_FOR_DEBUG_PRINTS__
#define Serial UNGUARDED DEBUG PRINT!!!
#endif

namespace Mitov
{
	class ESP32ProcessorReset
	{
	public:
		inline void ClockInputPin_o_Receive( void *_Data )
		{
			ESP.restart();
		}

	};
//---------------------------------------------------------------------------
	class ESP32ProcessorDeepSleep
	{
	public:
		uint32_t	Period = 1000000;

	public:
		inline void ClockInputPin_o_Receive( void *_Data )
		{
			ESP.deepSleep( Period );
		}

	};
//---------------------------------------------------------------------------
}

#ifdef __TEST_FOR_DEBUG_PRINTS__
#undef Serial
#endif

#endif
