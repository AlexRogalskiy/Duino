////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2018 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_DISPLAY_ST7735R_GREEN_TAB_160_80_h
#define _MITOV_DISPLAY_ST7735R_GREEN_TAB_160_80_h

#include <Mitov_Display_ST7735.h>
#include <Mitov_Display_ST7735_Const_R.h>

#ifdef __TEST_FOR_DEBUG_PRINTS__
#define Serial UNGUARDED DEBUG PRINT!!!
#endif

namespace Mitov
{
namespace ST7735
{
static const uint8_t PROGMEM Rcmd2green160x80[] = 
	{              // Init for 7735R, part 2 (mini 160x80)
		2,                  //  2 commands in list:
		ST7735::CASET,	4,  //  1: Column addr set, 4 args, no delay:
		  0x00, 0x00,       //     XSTART = 0
		  0x00, 0x7F,       //     XEND = 79
		ST7735::RASET,	4,	//  2: Row addr set, 4 args, no delay:
		  0x00, 0x00,       //     XSTART = 0
		  0x00, 0x9F		//     XEND = 159
	};           
} // ST7735
//---------------------------------------------------------------------------
	template<typename T_RESET_IMPLEMENTATION, typename T_REGISTER_SELECT_IMPLEMENTATION, typename T_SPI, T_SPI &C_SPI, typename T_CHIP_SELECT_IMPLEMENTATION, uint8_t C_OFFSET_X, uint8_t C_OFFSET_Y, uint8_t C_WIDTH, uint8_t C_HEIGHT, uint8_t C_PIXEL_MODE> class Display_ST7735R_GreenTab_160_80 : public BasicSPI_ST7735<T_SPI, C_SPI, T_RESET_IMPLEMENTATION, T_REGISTER_SELECT_IMPLEMENTATION, T_CHIP_SELECT_IMPLEMENTATION, C_OFFSET_X, C_OFFSET_Y>
	{
		typedef BasicSPI_ST7735<T_SPI, C_SPI, T_RESET_IMPLEMENTATION, T_REGISTER_SELECT_IMPLEMENTATION, T_CHIP_SELECT_IMPLEMENTATION, C_OFFSET_X, C_OFFSET_Y> inherited;

//	protected:
//		static const uint16_t WIDTH = 80;
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
			inherited::commonInit( ST7735::Rcmd1 );
			inherited::commandList( ST7735::Rcmd2green160x80 );
			inherited::commandList( ST7735::Rcmd3 );
			inherited::writecommand(ST7735::MADCTL);
			inherited::writedata(0xC0);

//			inherited::SystemInit();
		}

/*
	public:
		Display_ST7735R_GreenTab_160_80( const unsigned char * AFont ) :
			inherited( AFont )
		{
			inherited::colstart = 24;
			inherited::rowstart = 0;
		}
*/
	};
//---------------------------------------------------------------------------
}

#ifdef __TEST_FOR_DEBUG_PRINTS__
#undef Serial
#endif

#endif // _MITOV_DISPLAY_ST7735R_GREEN_TAB_160_80_h
