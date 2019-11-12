////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2018 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_DISPLAY_ST7735B_h
#define _MITOV_DISPLAY_ST7735B_h

#include <Mitov_Display_ST7735.h>
//#include <Mitov_Display_ST7735_Const_R.h>
//#include <Mitov_Display_ST7735_Const_R2.h>

#ifdef __TEST_FOR_DEBUG_PRINTS__
#define Serial UNGUARDED DEBUG PRINT!!!
#endif

namespace Mitov
{
namespace ST7735
{
static const uint8_t PROGMEM Bcmd[] = 
	{                  // Initialization commands for 7735B screens
		18,                       // 18 commands in list:
		ST7735::SWRESET,   ST7735::DELAY,  //  1: Software reset, no args, w/delay
		  50,                     //     50 ms delay
		ST7735::SLPOUT ,   ST7735::DELAY,  //  2: Out of sleep mode, no args, w/delay
		  255,                    //     255 = 500 ms delay
		ST7735::COLMOD , 1+ST7735::DELAY,  //  3: Set color mode, 1 arg + delay:
		  0x05,                   //     16-bit color
		  10,                     //     10 ms delay
		ST7735::FRMCTR1, 3+ST7735::DELAY,  //  4: Frame rate control, 3 args + delay:
		  0x00,                   //     fastest refresh
		  0x06,                   //     6 lines front porch
		  0x03,                   //     3 lines back porch
		  10,                     //     10 ms delay
		ST7735::MADCTL , 1      ,  //  5: Memory access ctrl (directions), 1 arg:
		  0x08,                   //     Row addr/col addr, bottom to top refresh
		ST7735::DISSET5, 2      ,  //  6: Display settings #5, 2 args, no delay:
		  0x15,                   //     1 clk cycle nonoverlap, 2 cycle gate
								  //     rise, 3 cycle osc equalize
		  0x02,                   //     Fix on VTL
		ST7735::INVCTR , 1      ,  //  7: Display inversion control, 1 arg:
		  0x0,                    //     Line inversion
		ST7735::PWCTR1 , 2+ST7735::DELAY,  //  8: Power control, 2 args + delay:
		  0x02,                   //     GVDD = 4.7V
		  0x70,                   //     1.0uA
		  10,                     //     10 ms delay
		ST7735::PWCTR2 , 1      ,  //  9: Power control, 1 arg, no delay:
		  0x05,                   //     VGH = 14.7V, VGL = -7.35V
		ST7735::PWCTR3 , 2      ,  // 10: Power control, 2 args, no delay:
		  0x01,                   //     Opamp current small
		  0x02,                   //     Boost frequency
		ST7735::VMCTR1 , 2+ST7735::DELAY,  // 11: Power control, 2 args + delay:
		  0x3C,                   //     VCOMH = 4V
		  0x38,                   //     VCOML = -1.1V
		  10,                     //     10 ms delay
		ST7735::PWCTR6 , 2      ,  // 12: Power control, 2 args, no delay:
		  0x11, 0x15,
		ST7735::GMCTRP1,16      ,  // 13: Magical unicorn dust, 16 args, no delay:
		  0x09, 0x16, 0x09, 0x20, //     (seriously though, not sure what
		  0x21, 0x1B, 0x13, 0x19, //      these config values represent)
		  0x17, 0x15, 0x1E, 0x2B,
		  0x04, 0x05, 0x02, 0x0E,
		ST7735::GMCTRN1,16+ST7735::DELAY,  // 14: Sparkles and rainbows, 16 args + delay:
		  0x0B, 0x14, 0x08, 0x1E, //     (ditto)
		  0x22, 0x1D, 0x18, 0x1E,
		  0x1B, 0x1A, 0x24, 0x2B,
		  0x06, 0x06, 0x02, 0x0F,
		  10,                     //     10 ms delay
		ST7735::CASET  , 4      ,  // 15: Column addr set, 4 args, no delay:
		  0x00, 0x02,             //     XSTART = 2
		  0x00, 0x81,             //     XEND = 129
		ST7735::RASET  , 4      ,  // 16: Row addr set, 4 args, no delay:
		  0x00, 0x02,             //     XSTART = 1
		  0x00, 0x81,             //     XEND = 160
		ST7735::NORON  ,   ST7735::DELAY,  // 17: Normal display on, no args, w/delay
		  10,                     //     10 ms delay
		ST7735::DISPON ,   ST7735::DELAY,  // 18: Main screen turn on, no args, w/delay
		  255								//     255 = 500 ms delay
	};                 

//---------------------------------------------------------------------------
} // ST7735
//---------------------------------------------------------------------------
	template<typename T_RESET_IMPLEMENTATION, typename T_REGISTER_SELECT_IMPLEMENTATION, typename T_SPI, T_SPI &C_SPI, typename T_CHIP_SELECT_IMPLEMENTATION, uint8_t C_OFFSET_X, uint8_t C_OFFSET_Y, uint8_t C_WIDTH, uint8_t C_HEIGHT, uint8_t C_PIXEL_MODE> class Display_ST7735B : public BasicSPI_ST7735<T_SPI, C_SPI, T_RESET_IMPLEMENTATION, T_REGISTER_SELECT_IMPLEMENTATION, T_CHIP_SELECT_IMPLEMENTATION, C_OFFSET_X, C_OFFSET_Y>
	{
		typedef BasicSPI_ST7735<T_SPI, C_SPI, T_RESET_IMPLEMENTATION, T_REGISTER_SELECT_IMPLEMENTATION, T_CHIP_SELECT_IMPLEMENTATION, C_OFFSET_X, C_OFFSET_Y> inherited;

//	protected:
//		static const uint16_t WIDTH = 128;
//		static const uint16_t HEIGHT = 160;

	public:
		void UpdateOrientation() // Used by the bnding setters
		{
			inherited::writecommand(ST7735::MADCTL);

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
			inherited::commonInit( ST7735::Bcmd );

//			inherited::SystemInit();
		}

/*
	public:
		Display_ST7735B( const unsigned char * AFont ) :
			inherited( AFont )
		{
			inherited::colstart = 0;
			inherited::rowstart = 0;
		}
*/
	};
}

#ifdef __TEST_FOR_DEBUG_PRINTS__
#undef Serial
#endif

#endif // _MITOV_DISPLAY_ST7735B_h
