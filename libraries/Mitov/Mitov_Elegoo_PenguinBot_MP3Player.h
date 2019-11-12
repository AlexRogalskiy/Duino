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
#include <Mitov_SoftwareSerial.h>
#include <Mitov_Maiyout_Serial_MP3.h>

#ifdef __TEST_FOR_DEBUG_PRINTS__
#define Serial UNGUARDED DEBUG PRINT!!!
#endif

namespace Mitov
{
//---------------------------------------------------------------------------
	Mitov::VisuinoSoftwareSerial<16, 17> GSoftwareSerial;
//---------------------------------------------------------------------------
	template<typename T_TIMEOUT_PIN, typename T_ERROR_PIN, typename T_PLAYBACK, typename T_SD_CARD, typename T_ELEMENT_RESPONSE> class ElegooPenguinBotMP3Player : public MitovMaiyoutSerialMP3<Mitov::VisuinoSoftwareSerial<16, 17>, GSoftwareSerial, T_TIMEOUT_PIN, T_ERROR_PIN, T_PLAYBACK, T_SD_CARD, T_ELEMENT_RESPONSE>
	{
	public:
		typedef MitovMaiyoutSerialMP3<Mitov::VisuinoSoftwareSerial<16, 17>, GSoftwareSerial, T_TIMEOUT_PIN, T_ERROR_PIN, T_PLAYBACK, T_SD_CARD, T_ELEMENT_RESPONSE> inherited;

	public:
		void SetEnabled( bool AValue )
		{
			digitalWrite( 7, (AValue) ? HIGH : LOW );
		}

	protected:
		static inherited	*FInstance;

	protected:
		static void PinCallerReceive( void *_Data )
		{
			FInstance->_DirectPinReceive( _Data );
		}

	public:
		inline void SystemInit( bool AEnabled )
		{
			FInstance = this;
			GSoftwareSerial.OutputPin().SetCallback( PinCallerReceive );

			pinMode( 7, OUTPUT );
			digitalWrite( 7, (AEnabled) ? HIGH : LOW );

			GSoftwareSerial.SystemInit();
			inherited::SystemInit();
		}

		inline void SystemLoopBegin( unsigned long currentMicros )
		{
			GSoftwareSerial.SystemLoopBegin( currentMicros );
			inherited::SystemLoopBegin( currentMicros );
		}

	};
//---------------------------------------------------------------------------
	template<typename T_TIMEOUT_PIN, typename T_ERROR_PIN, typename T_PLAYBACK, typename T_SD_CARD, typename T_ELEMENT_RESPONSE> MitovMaiyoutSerialMP3<Mitov::VisuinoSoftwareSerial<16, 17>, GSoftwareSerial, T_TIMEOUT_PIN, T_ERROR_PIN, T_PLAYBACK, T_SD_CARD, T_ELEMENT_RESPONSE> *ElegooPenguinBotMP3Player<T_TIMEOUT_PIN, T_ERROR_PIN, T_PLAYBACK, T_SD_CARD, T_ELEMENT_RESPONSE>::FInstance = nullptr;
//---------------------------------------------------------------------------
}

#ifdef __TEST_FOR_DEBUG_PRINTS__
#undef Serial
#endif

#endif // _MITOV_ELEGOO_PENGUIN_BOT_MP3PLAYER_h
