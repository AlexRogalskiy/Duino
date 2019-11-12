////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2018 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_ELEGOO_PENGUIN_BOT_MP3PLAYER_h
#define _MITOV_ELEGOO_PENGUIN_BOT_MP3PLAYER_h

#include <Mitov.h>
#include <Mitov_Maiyout_Serial_MP3.h>

#ifdef __TEST_FOR_DEBUG_PRINTS__
#define Serial UNGUARDED DEBUG PRINT!!!
#endif

namespace Mitov
{
//---------------------------------------------------------------------------
	template<typename T_TIMEOUT_PIN, typename T_ERROR_PIN, typename T_PLAYBACK, typename T_SD_CARD, typename T_ELEMENT_RESPONSE> class Mitov_Elegoo_PenguinBot_MP3Player_Excluded
	{
	public:
		Mitov_Elegoo_PenguinBot_MP3Player_Excluded()
		{
			pinMode( 7, OUTPUT );
			digitalWrite( 7, LOW ); // Disable the Audio Amplifier
		}
	};
//---------------------------------------------------------------------------
}

#ifdef __TEST_FOR_DEBUG_PRINTS__
#undef Serial
#endif

#endif // _MITOV_ELEGOO_PENGUIN_BOT_MP3PLAYER_h
