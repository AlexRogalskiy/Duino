////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2018 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_MICROCHIP_CHIPKIT_BASIC_SHIELD_DISPLAY_h
#define _MITOV_MICROCHIP_CHIPKIT_BASIC_SHIELD_DISPLAY_h

#include <SPI.h>
#include <Mitov_Display_SSD1306.h>
#include <Mitov_Display_SSD1306_SPI.h>

#ifdef __TEST_FOR_DEBUG_PRINTS__
#define Serial UNGUARDED DEBUG PRINT!!!
#endif

namespace Mitov
{
	class DisplayChipKIT_odtSSD1306_SPI
	{
	protected:
		static const uint8_t DATA_COMMAND_PIN = 39;

	public:
/*
		inline void SendPinsConfigCommand()
		{
			SendCommand( 0x20 );
		}
*/
		void SendCommand( uint8_t ACommand )
		{
//			Serial.println( ACommand, HEX );
			digitalWrite( DATA_COMMAND_PIN, LOW ); // Data Command
			SPI.transfer( ACommand );
		}

		void SendBuffer( uint8_t *ABuffer )
		{
			digitalWrite( DATA_COMMAND_PIN, HIGH ); // Data Command

			for (uint16_t i = 0; i < ( 128 * 32 / 8 ); ++i )
				SPI.transfer( ABuffer[ i ] );

		}

		inline void IntSystemInitStart() {} // Placeholder
		inline void IntSystemInitEnd() {} // Placeholder
	};
//---------------------------------------------------------------------------
	template<typename T_IMPLEMENTATION, uint8_t WIDTH, uint8_t HEIGHT, uint8_t C_SETCOMPINS, uint8_t C_PAGE_END_ADDR> class Microchip_ChipKITBasicShieldDisplay : public Mitov::DisplaySSD1306<T_IMPLEMENTATION, WIDTH, HEIGHT, C_SETCOMPINS, C_PAGE_END_ADDR>// DisplaySSD1306<0x20, odtSSD1306>
	{
		typedef Mitov::DisplaySSD1306<T_IMPLEMENTATION, WIDTH, HEIGHT, C_SETCOMPINS, C_PAGE_END_ADDR> inherited;

	protected:
		static const uint8_t RESET_PIN = 10;

	public:
		inline void SystemInit()
		{
			pinMode( 38, OUTPUT ); // VDD
			digitalWrite( 38, LOW );
			delay( 1 );

			pinMode( RESET_PIN, OUTPUT );
			pinMode( DisplayChipKIT_odtSSD1306_SPI::DATA_COMMAND_PIN, OUTPUT ); // Data Command
			digitalWrite( RESET_PIN, HIGH );
			// VDD (3.3V) goes high at start, lets just chill for a ms
			delay(1);
			// bring reset low
			digitalWrite( RESET_PIN, LOW );
			// wait 10ms
			delay(10);
			// bring out of reset
			digitalWrite( RESET_PIN, HIGH );

			inherited::SystemInit();

			pinMode( 40, OUTPUT ); // VBAT
			digitalWrite( 40, LOW );
			delay( 100 );
		}

	public:
		using	inherited::inherited;

	};
//---------------------------------------------------------------------------
}

#ifdef __TEST_FOR_DEBUG_PRINTS__
#undef Serial
#endif

#endif
