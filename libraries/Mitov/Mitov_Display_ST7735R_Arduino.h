////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2018 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_DISPLAY_ST7735R_ARDUINO_h
#define _MITOV_DISPLAY_ST7735R_ARDUINO_h

#include <Mitov_Display_ST7735.h>

#ifdef __TEST_FOR_DEBUG_PRINTS__
#define Serial UNGUARDED DEBUG PRINT!!!
#endif

// #define __ST7735__DEBUG__

namespace Mitov
{
namespace ST7735
{
static const uint8_t PROGMEM
  Gcmd[] = {                  // Initialization commands for 7735B screens
    19,                       // 18 commands in list:
    ST7735::SWRESET,   ST7735::DELAY,  //  1: Software reset, no args, w/delay
      50,                     //     50 ms delay
    ST7735::SLPOUT ,   ST7735::DELAY,  //  2: Out of sleep mode, no args, w/delay
      100,                    //     255 = 500 ms delay
    0x26 , 1,  			// 3: Set default gamma
      0x04,                     //     16-bit color
    0xb1, 2,              	// 4: Frame Rate
      0x0b,
      0x14,
    0xc0, 2,                    // 5: VRH1[4:0] & VC[2:0]
      0x08,
      0x00,
    0xc1, 1,                    // 6: BT[2:0]
      0x05,
    0xc5, 2,                    // 7: VMH[6:0] & VML[6:0]
      0x41,
      0x30,
    0xc7, 1,                    // 8: LCD Driving control
      0xc1,
    0xEC, 1,                    // 9: Set pumping color freq
      0x1b,
    0x3a , 1 + ST7735::DELAY,  	        // 10: Set color format
      0x55,                     //     16-bit color
      100,
    0x2a, 4,                    // 11: Set Column Address
      0x00,
      0x00,
      0x00,
      0x7f,
    0x2b, 4,                    // 12: Set Page Address
      0x00,
      0x00,
      0x00,
      0x9f,
    0x36, 1,                    // 12+1: Set Scanning Direction
      0xc8,
    0xb7, 1,			// 14: Set Source Output Direciton
      0x00,
    0xf2, 1,			// 15: Enable Gamma bit
      0x00,
    0xe0, 15 + ST7735::DELAY,		// 16: magic
      0x28, 0x24, 0x22, 0x31,
      0x2b, 0x0e, 0x53, 0xa5,
      0x42, 0x16, 0x18, 0x12,
      0x1a, 0x14, 0x03,
      50,
    0xe1, 15 + ST7735::DELAY,		// 17: more magic
      0x17, 0x1b, 0x1d, 0x0e,
      0x14, 0x11, 0x2c, 0xa5,
      0x3d, 0x09, 0x27, 0x2d,
      0x25, 0x2b, 0x3c, 
      50, 
    ST7735::NORON  ,   ST7735::DELAY,  // 17: Normal display on, no args, w/delay
      10,                     //     10 ms delay
    ST7735::DISPON ,   ST7735::DELAY,  // 18: Main screen turn on, no args, w/delay
      255 };                  //     255 = 500 ms delay
} // ST7735
//---------------------------------------------------------------------------
	template<typename T_RESET_IMPLEMENTATION, typename T_REGISTER_SELECT_IMPLEMENTATION, typename T_SPI, T_SPI &C_SPI, typename T_CHIP_SELECT_IMPLEMENTATION, uint8_t C_OFFSET_X, uint8_t C_OFFSET_Y, uint8_t C_WIDTH, uint8_t C_HEIGHT, uint8_t C_PIXEL_MODE> class Display_ST7735R_Arduino : public BasicSPI_ST7735<T_SPI, C_SPI, T_RESET_IMPLEMENTATION, T_REGISTER_SELECT_IMPLEMENTATION, T_CHIP_SELECT_IMPLEMENTATION, C_OFFSET_X, C_OFFSET_Y>
	{
		typedef BasicSPI_ST7735<T_SPI, C_SPI, T_RESET_IMPLEMENTATION, T_REGISTER_SELECT_IMPLEMENTATION, T_CHIP_SELECT_IMPLEMENTATION, C_OFFSET_X, C_OFFSET_Y> inherited;

//	protected:
//		static const uint16_t WIDTH = 128;
//		static const uint16_t HEIGHT = 160;

	public:
		void UpdateOrientation() // Used by the bnding setters
		{
			inherited::writecommand( ST7735::MADCTL );

			inherited::writedata( C_PIXEL_MODE );
/*
			switch( inherited::Orientation )
			{
				case goUp:		inherited::writedata(ST7735::MADCTL_MX | ST7735::MADCTL_MY | ST7735::MADCTL_BGR); break;
				case goLeft:	inherited::writedata(ST7735::MADCTL_MX | ST7735::MADCTL_MV | ST7735::MADCTL_BGR); break;
				case goDown: 	inherited::writedata(ST7735::MADCTL_BGR); break;
				case goRight: 	inherited::writedata(ST7735::MADCTL_MY | ST7735::MADCTL_MV | ST7735::MADCTL_BGR); break;
			}
*/
		}

	public:
		int16_t width(void) const
		{
			return C_WIDTH;
/*
			if( inherited::Orientation == goUp || inherited::Orientation == goDown )
				return WIDTH;

			return HEIGHT;
*/
		}

		int16_t height(void) const
		{
			return C_HEIGHT;
/*
			if( inherited::Orientation == goUp || inherited::Orientation == goDown )
				return HEIGHT;

			return WIDTH;
*/
		}

	public:
		inline void SystemInit()
		{
			inherited::commonInit( ST7735::Gcmd );
		}

/*
	public:
		DisplayST7735Arduino()
		{
			inherited::colstart = 2;
			inherited::rowstart = 1;
		}
*/
	};
}

#ifdef __TEST_FOR_DEBUG_PRINTS__
#undef Serial
#endif

#endif
