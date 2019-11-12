////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2018 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_ELEGOO_PENGUIN_BOT_h
#define _MITOV_ELEGOO_PENGUIN_BOT_h

#include <Mitov.h>

#ifdef __TEST_FOR_DEBUG_PRINTS__
#define Serial UNGUARDED DEBUG PRINT!!!
#endif

namespace Mitov
{
//---------------------------------------------------------------------------
	template<typename T_PIN_0, typename T_PIN_1> class ElegooPenguinBotLineFollowingSensors : public T_PIN_0, public T_PIN_1
	{
	public:
		OpenWire::SourcePin &OutputPins0() { return T_PIN_0::GetPinInstance(); }
		OpenWire::SourcePin &OutputPins1() { return T_PIN_1::GetPinInstance(); }

	public:
		void ReadValues()
		{
			float AValue = analogRead( A0 );
			AValue /= VISUINO_ANALOG_IN_RANGE;
			T_PIN_0::SetPinValue( AValue );

			AValue = analogRead( A1 );
			AValue /= VISUINO_ANALOG_IN_RANGE;
			T_PIN_1::SetPinValue( AValue );
		}

	public:
		inline void ClockInputPin_o_Receive( void *_Data )
		{
			ReadValues();
		}

	public:
		inline void SystemStart()
		{
			ReadValues();
		}

		inline void SystemLoopBegin( unsigned long currentMicros )
		{
			ReadValues();
		}

	};
//---------------------------------------------------------------------------
	class ElegooPenguinBotBatterySensor
	{
	public:
		OpenWire::SourcePin	OutputPin;

	public:
		void ReadValue()
		{
			const float RES1 = 10000;
			const float RES2 = 2000;
			const float VREF = 1.1;

			float AValue = analogRead( A4 );
			float volMeasure = AValue * VREF / 1024;
			float VCC = volMeasure * (RES1 + RES2) / RES2;
			OutputPin.Notify( &VCC );
		}

	public:
		inline void ClockInputPin_o_Receive( void *_Data )
		{
			ReadValue();
		}

	public:
		inline void SystemStart()
		{
			pinMode( A4, INPUT );
			ReadValue();
		}

		inline void SystemLoopBegin( unsigned long currentMicros )
		{
			ReadValue();
		}

	};
//---------------------------------------------------------------------------
/*
	template<typename T_TIMEOUT_PIN, typename T_ERROR_PIN, typename T_PLAYBACK, typename T_SD_CARD> class Mitov_Elegoo_PenguinBot_MP3Player_Excluded
	{
	public:
		Mitov_Elegoo_PenguinBot_MP3Player_Excluded()
		{
			pinMode( 7, OUTPUT );
			digitalWrite( 7, LOW ); // Disable the Audio Amplifier
		}
	};
*/
	//---------------------------------------------------------------------------
}

#ifdef __TEST_FOR_DEBUG_PRINTS__
#undef Serial
#endif

#endif // _MITOV_ELEGOO_PENGUIN_BOT_h
