////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2018 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_ESP32_DISPLAY_SSD1306_h
#define _MITOV_ESP32_DISPLAY_SSD1306_h

#include <Mitov_Display_SSD1306_I2C.h>

#ifdef __TEST_FOR_DEBUG_PRINTS__
#define Serial UNGUARDED DEBUG PRINT!!!
#endif

namespace Mitov
{
	template<typename T_IMPLEMENTATION> class DisplaySSD1306HeltecReset : public T_IMPLEMENTATION
	{
	protected:
		inline void Reset()
		{
			pinMode( 16, OUTPUT );
			digitalWrite( 16, HIGH );
			// VDD (3.3V) goes high at start, lets just chill for a ms
			delay(1);
			// bring reset low
			digitalWrite( 16, LOW );
			// wait 10ms
			delay(10);
			// bring out of reset
			digitalWrite( 16, HIGH );
			// turn on VCC (9V?)
		}
	};
//---------------------------------------------------------------------------
}

#ifdef __TEST_FOR_DEBUG_PRINTS__
#undef Serial
#endif

#endif
