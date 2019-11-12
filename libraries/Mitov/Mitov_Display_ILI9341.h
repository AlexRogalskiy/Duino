////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2018 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_DISPLAY_ILI9341_h
#define _MITOV_DISPLAY_ILI9341_h

#include <Mitov_Graphics_Color.h>

#ifdef __TEST_FOR_DEBUG_PRINTS__
#define Serial UNGUARDED DEBUG PRINT!!!
#endif

namespace Mitov
{
/*
static const uint8_t PROGMEM
  Rcmd1[] = {                 // Init for 7735R, part 1 (red or green tab)
    15,                       // 15 commands in list:
    ST7735::SWRESET,   ST7735::DELAY,  //  1: Software reset, 0 args, w/delay
      150,                    //     150 ms delay
    ST7735::SLPOUT ,   ST7735::DELAY,  //  2: Out of sleep mode, 0 args, w/delay
      255,                    //     500 ms delay
    ST7735::FRMCTR1, 3      ,  //  3: Frame rate ctrl - normal mode, 3 args:
      0x01, 0x2C, 0x2D,       //     Rate = fosc/(1x2+40) * (LINE+2C+2D)
    ST7735::FRMCTR2, 3      ,  //  4: Frame rate control - idle mode, 3 args:
      0x01, 0x2C, 0x2D,       //     Rate = fosc/(1x2+40) * (LINE+2C+2D)
    ST7735::FRMCTR3, 6      ,  //  5: Frame rate ctrl - partial mode, 6 args:
      0x01, 0x2C, 0x2D,       //     Dot inversion mode
      0x01, 0x2C, 0x2D,       //     Line inversion mode
    ST7735::INVCTR , 1      ,  //  6: Display inversion ctrl, 1 arg, no delay:
      0x07,                   //     No inversion
    ST7735::PWCTR1 , 3      ,  //  7: Power control, 3 args, no delay:
      0xA2,
      0x02,                   //     -4.6V
      0x84,                   //     AUTO mode
    ST7735::PWCTR2 , 1      ,  //  8: Power control, 1 arg, no delay:
      0xC5,                   //     VGH25 = 2.4C VGSEL = -10 VGH = 3 * AVDD
    ST7735::PWCTR3 , 2      ,  //  9: Power control, 2 args, no delay:
      0x0A,                   //     Opamp current small
      0x00,                   //     Boost frequency
    ST7735::PWCTR4 , 2      ,  // 10: Power control, 2 args, no delay:
      0x8A,                   //     BCLK/2, Opamp current small & Medium low
      0x2A,  
    ST7735::PWCTR5 , 2      ,  // 11: Power control, 2 args, no delay:
      0x8A, 0xEE,
    ST7735::VMCTR1 , 1      ,  // 12: Power control, 1 arg, no delay:
      0x0E,
    ST7735::INVOFF , 0      ,  // 13: Don't invert display, no args, no delay
    ST7735::MADCTL , 1      ,  // 14: Memory access control (directions), 1 arg:
      0xC8,                   //     row addr/col addr, bottom to top refresh
    ST7735::COLMOD , 1      ,  // 15: set color mode, 1 arg, no delay:
      0x05 };                 //     16-bit color
*/
namespace ILI9341Extended
{
	static const uint8_t INIT_DELAY = 0xFF;

	static const uint8_t MADCTL		= 0x36;
	static const uint8_t MADCTL_MY  = 0x80;
	static const uint8_t MADCTL_MX  = 0x40;
	static const uint8_t MADCTL_MV  = 0x20;
	static const uint8_t MADCTL_ML  = 0x10;
	static const uint8_t MADCTL_RGB = 0x00;
	static const uint8_t MADCTL_BGR = 0x08;
	static const uint8_t MADCTL_MH  = 0x04;

	static const uint8_t COLADDRSET = 0x2A;
	static const uint8_t PAGEADDRSET = 0x2B;

/*
	static const uint8_t PROGMEM // Adafruit version
		Init[] =
		{
			0x01, 0, // ILI9341_SOFTRESET

			INIT_DELAY, 50,

			0x28, 0, // ILI9341_DISPLAYOFF

			0xC0, 1, // ILI9341_POWERCONTROL1
			0x23,

			0xC1, 1, // ILI9341_POWERCONTROL2
			0x10,

			0xC5, 2, // ILI9341_VCOMCONTROL1
			0x2B,
			0x2B,

			0xC7, 1, // ILI9341_VCOMCONTROL2
			0xC0,

			MADCTL, 1, // ILI9341_MEMCONTROL
			MADCTL_MY | MADCTL_BGR,

			0x3A, 1, // ILI9341_PIXELFORMAT
			0x55,

			0xB1, 2, // ILI9341_FRAMECONTROL
			0x00,
			0x1B,

			0xB7, 1, // ILI9341_ENTRYMODE
			0x07,

			0x11, 0, // ILI9341_SLEEPOUT

			INIT_DELAY, 150,

			0x29, 0, // ILI9341_DISPLAYON

			INIT_DELAY, 250,
			INIT_DELAY, 250

//			setAddrWindow( 0, 0, WIDTH - 1, HEIGHT - 1 );
		};
*/
	static const uint8_t PROGMEM
		Init[] =
		{
//			24,		// 23 commands

			0xCF, 3,	// 1. Power Control B - http://www.newhavendisplay.com/app_notes/ILI9341.pdf
			0x00,
			0xC1,
			0x30,

			0xED, 4,	//  2. Power on sequence contro
			0x64,
			0x03, 
			0x12, 
			0x81,

			0xE8, 3,	//  3. Driver timing control A
			0x85,
			0x00, 
			0x78, 

			0xCB, 5,	//  4. Power control A 
			0x39,
			0x2C, 
			0x00, 
			0x34, 
			0x02, 

			0xF7, 1,	// 5. Pump ratio control
			0x20,

			0xEA, 2,	// 6. Driver timing control B
			0x00, 
			0x00,

			0xC0, 1,    //  7. Power control - https://cdn-shop.adafruit.com/datasheets/ILI9341.pdf
			0x18,		//VRH[5:0] 

			0xC1, 1,    // 8. Power control 
			0x12,		//SAP[2:0];BT[3:0] 

			0xC5, 2,    // 9. VCOM control 
			0x31,
			0x3C,

			0xC7, 1,    // 10. VCOM control2 
			0x99,

			//--

			0x3A, 1,    // 11. Pixel Format Set
			0x55,

			0xB1, 2,    // 12. Frame Control
			0x00,
			0x16,

			0xB6, 2,    // 13. Display Function Control 
			0x0A,
			0xA2,

			0xF6, 2,	// 14. Interface Control 
			0x01,
			0x30,

			0xF2, 1,    // 15. 3Gamma Function Disable 
			0x00,

			0x26, 1,    // 16. Gamma curve selected 
			0x02,

			0xE0, 15,   // 17. Set Gamma 
			0x0F,
			0x1D,
			0x1A,
			0x0A,
			0x0D,
			0x07,
			0x49,
			0x66,
			0x3B,
			0x07,
			0x11,
			0x01,
			0x09,
			0x05,
			0x04,

			0xE1, 15,   // 18. Set Gamma 
			0x00,
			0x18,
			0x1D,
			0x02,
			0x0F,
			0x04,
			0x36,
			0x13,
			0x4C,
			0x07,
			0x13,
			0x0F,
			0x2E,
			0x2F,
			0x05,


			0x11, 0,    // 19. Exit Sleep 

			INIT_DELAY, 120,	// 20.

			0x29, 0,    // 21. Display on 
	
			0x2A, 4,    // 22. Set Gamma 
			0x00,
			0x00,
			0x00,
			0xEF,

			0x2B, 4,    // 23. Set Gamma 
			0x00,
			0x00,
			0x01,
			0x3F,

			0x2C		// 24. Memory Write  
		};

};

/*
	namespace DisplayILI9341Const
	{
		const uint16_t WIDTH = 240;
		const uint16_t HEIGHT = 320;
	};
*/
	template<typename T_IMPLEMENTATION, uint8_t C_MADCTL, uint16_t WIDTH, uint16_t HEIGHT> class DisplayILI9341 : public T_IMPLEMENTATION, public GraphicsImpl<DisplayILI9341<T_IMPLEMENTATION, C_MADCTL, WIDTH, HEIGHT>>
	{
		typedef DisplayILI9341<T_IMPLEMENTATION, C_MADCTL, WIDTH, HEIGHT> self;
		typedef GraphicsImpl<self> inherited;

	protected:
		static const uint8_t CHIP_SELECT_PIN = 0;
		static const uint8_t COMMAND_DATA_PIN = 1;
		static const uint8_t RESET_PIN = 2;
		static const uint8_t READ_PIN = 3;
		static const uint8_t WRITE_PIN = 4;
		static const uint8_t FIRST_DATA_PIN = 5;

//	public:
//		GraphicsOrientation	Orientation = goUp;

	public:
		TGraphicsColorTextSettings	Text;

	public:
		inline int16_t width(void) const
		{
			return WIDTH;
//			return T_IMPLEMENTATION::width( Orientation, DisplayILI9341Const::WIDTH, DisplayILI9341Const::HEIGHT );
		}

		inline int16_t height(void) const
		{
			return HEIGHT;
//			return T_IMPLEMENTATION::height( Orientation, DisplayILI9341Const::WIDTH, DisplayILI9341Const::HEIGHT );
		}

	public:
		inline void SystemInit()
		{
			T_IMPLEMENTATION::InitPins();

			int i = 0;
			while(i < sizeof( ILI9341Extended::Init )) 
			{
				uint8_t a = pgm_read_byte(& ILI9341Extended::Init[i++]);
				uint8_t ACount = pgm_read_byte(& ILI9341Extended::Init[i++]);
				if( a == ILI9341Extended::INIT_DELAY ) 
					delay( ACount );

				else
				{
					cmd( a );
					for( int j = 0; j < ACount; ++j )
						w_data( pgm_read_byte(& ILI9341Extended::Init[i++] ));

				}
			}


/*
			cmd(0x0036);    // Memory Access Control 
//#ifdef V_SCREEN	
			w_data(0x004a); 
//#endif
*/
/*
#ifdef H_SCREEN	
			w_data(0x2a); 
#endif
*/

/*
			clear( Func::Color565( 255 ) );
			UpdateInverted();


*/
		}

		inline void SystemStart()
		{
			UpdateOrientation();

			ClearScreen( *this, inherited::BackgroundColor );

			inherited::setTextSize( Text.Size );
			inherited::setTextColor( Text.Color, Text.BackgroundColor );
			inherited::setTextWrap( Text.Wrap );

			inherited::RenderElements();
//			for( int i = 0; i < FElements.size(); ++ i )
//				FElements[ i ]->Render( false );

		}

		void UpdateOrientation()
		{
			cmd( ILI9341Extended::MADCTL);

			w_data( C_MADCTL );

			setAddrWindow( 0, 0, width() - 1, height() - 1 ); // CS_IDLE happens here
		}

		void setAddrWindow(int x1, int y1, int x2, int y2)
		{
			T_IMPLEMENTATION::setAddrWindow( x1, y1, x2, y2 );
		}

		void writeRegister32(uint8_t r, uint32_t d) 
		{
			T_IMPLEMENTATION::writeRegister32( r, d );
		}

	public:
//#ifdef _GRAPHICS_DRAW_ALPHA_COLOR_BITMAP_

		static void drawAlphaColorBitmap( self &AImplementation, int16_t x, int16_t y, const uint32_t *bitmap, int16_t w, int16_t h, uint8_t size )
		{
			uint16_t AHeight = AImplementation.height();
			uint16_t AWidth = AImplementation.width();

			if( ( size > 1 ) || ( x < 0 ) || ( y < 0 ) || ( x + w > AWidth ) || ( x + w > AHeight ))
			{
				inherited::drawAlphaColorBitmap( AImplementation, x, y, bitmap, w, h, size );
				return;
			}

			// check rotation, move pixel around if necessary

			AImplementation.setAddrWindow(x,y,x + w - 1, y + h - 1);

			AImplementation.cmd( 0x2C );

			for( int16_t j=0; j<h; j++)
				for( int16_t i=0; i<w; ++i )
				{
					uint16_t AColor = Func::Color565( pgm_read_dword( bitmap++ ));
					AImplementation.w_data( AColor >> 8 );
					AImplementation.w_data( AColor );
				}

		}

//#endif // _GRAPHICS_DRAW_ALPHA_COLOR_BITMAP_

//#ifdef _GRAPHICS_DRAW_COLOR_BITMAP_
		static void drawColorBitmap( self &AImplementation, int16_t x, int16_t y, const uint8_t *bitmap, int16_t w, int16_t h, uint8_t size )
		{
			uint16_t AHeight = AImplementation.height();
			uint16_t AWidth = AImplementation.width();

			if( ( size > 1 ) || ( x < 0 ) || ( y < 0 ) || ( x + w > AWidth ) || ( x + w > AHeight ))
			{
				inherited::drawColorBitmap( AImplementation, x, y, bitmap, w, h, size );
				return;
			}

			AImplementation.setAddrWindow(x,y,x + w - 1, y + h - 1);

			AImplementation.cmd( 0x2C );

			for( int16_t j=0; j<h; j++)
				for( int16_t i=0; i<w; ++i )
				{
					uint32_t ARed = pgm_read_byte( bitmap ++ );
					uint32_t AGreen = pgm_read_byte( bitmap ++ );
					uint32_t ABlue = pgm_read_byte( bitmap ++ );
					uint16_t AColor = Func::Color565( ( AGreen << 8 ) | ( ARed << 16 ) | ABlue );

					AImplementation.w_data( AColor >> 8 );
					AImplementation.w_data( AColor );
				}

		}

//#endif // _GRAPHICS_DRAW_COLOR_BITMAP_

//#ifdef _GRAPHICS_DRAW_GRAYSCALE_BITMAP_

		static void drawGrayscaleBitmap( self &AImplementation, int16_t x, int16_t y, const uint8_t *bitmap, int16_t w, int16_t h, uint8_t AAlpha, uint8_t size )
		{
			uint16_t AHeight = AImplementation.height();
			uint16_t AWidth = AImplementation.width();

			if( ( size > 1 ) || ( x < 0 ) || ( y < 0 ) || ( x + w > AWidth ) || ( x + w > AHeight ))
			{
				inherited::drawGrayscaleBitmap( AImplementation, x, y, bitmap, w, h, AAlpha, size );
				return;
			}

			AImplementation.setAddrWindow(x,y,x + w - 1, y + h - 1);

			AImplementation.cmd( 0x2C );

			for( int16_t j=0; j<h; j++)
				for( int16_t i=0; i<w; ++i )
				{
					uint32_t AGray = pgm_read_byte( bitmap ++ );
					uint16_t AColor = Func::Color565( ( AGray << 8 ) | ( AGray << 16 ) | AGray );

					AImplementation.w_data( AColor >> 8 );
					AImplementation.w_data( AColor );
				}

		}

//#endif // _GRAPHICS_DRAW_GRAYSCALE_BITMAP_

//#ifdef _GRAPHICS_DRAW_GRAYSCALE_ALPHA_BITMAP_

		static void drawAlphaGrayscaleBitmap( self &AImplementation, int16_t x, int16_t y, const uint16_t *_bitmap, int16_t w, int16_t h, uint8_t size )
		{
			uint16_t AHeight = AImplementation.height();
			uint16_t AWidth = AImplementation.width();

			if( ( size > 1 ) || ( x < 0 ) || ( y < 0 ) || ( x + w > AWidth ) || ( x + w > AHeight ))
			{
				inherited::drawAlphaGrayscaleBitmap( AImplementation, x, y, _bitmap, w, h, size );
				return;
			}

			const uint8_t *bitmap = (const uint8_t *)_bitmap;

			AImplementation.setAddrWindow(x,y,x + w - 1, y + h - 1);

			AImplementation.cmd( 0x2C );

			for( int16_t j=0; j<h; j++)
				for( int16_t i=0; i<w; ++i )
				{
					uint32_t AGray = pgm_read_byte( bitmap );
					uint16_t AColor = Func::Color565( ( AGray << 8 ) | ( AGray << 16 ) | AGray );
					bitmap += 2;

					AImplementation.w_data( AColor >> 8 );
					AImplementation.w_data( AColor );
				}

		}

//#endif // _GRAPHICS_DRAW_GRAYSCALE_ALPHA_BITMAP_

//#ifdef _GRAPHICS_DRAW_BLACK_WHITE_BITMAP_

		static void drawBWBitmap( self &AImplementation, int16_t x, int16_t y, const uint8_t *bitmap, int16_t w, int16_t h, TUniColor color, TUniColor bg, uint8_t size )
		{
			uint16_t AHeight = AImplementation.height();
			uint16_t AWidth = AImplementation.width();

			if( ( size > 1 ) || ( x < 0 ) || ( y < 0 ) || ( x + w > AWidth ) || ( x + w > AHeight ))
			{
				inherited::drawBWBitmap( AImplementation, x, y, bitmap, w, h, color, bg, size );
				return;
			}

			int16_t byteWidth = (w + 7) / 8;

//			color = Func::Color565( color );
//			bg = Func::Color565( bg );
  
			AImplementation.setAddrWindow(x,y,x + w - 1, y + h - 1);

			AImplementation.cmd( 0x2C );

			for( int16_t j=0; j<h; j++)
				for( int16_t i=0; i<w; ++i )
				{
					uint16_t AColor;
					if(pgm_read_byte(bitmap + j * byteWidth + i / 8) & (128 >> ( i & 0b111 )))
						AColor = 0xFFFF;

					else 
      					AColor = 0;

					AImplementation.w_data( AColor >> 8 );
					AImplementation.w_data( AColor );
				}

		}

//#endif // _GRAPHICS_DRAW_BLACK_WHITE_BITMAP_

//#ifdef _GRAPHICS_DRAW_REPEAT_ALPHA_COLOR_BITMAP_

		static void drawRepeatAlphaColorBitmap( self &AImplementation, int16_t x, int16_t y, const uint8_t *bitmap, int16_t AListCount, int16_t w, int16_t h, uint8_t size )
		{
			uint16_t AHeight = AImplementation.height();
			uint16_t AWidth = AImplementation.width();

			if( ( size > 1 ) || ( x < 0 ) || ( y < 0 ) || ( x + w > AWidth ) || ( x + w > AHeight ))
			{
				inherited::drawRepeatAlphaColorBitmap( AImplementation, x, y, bitmap, AListCount, w, h, size );
				return;
			}

			AImplementation.setAddrWindow(x,y, x + w - 1, y + h - 1);

			AImplementation.cmd( 0x2C );

			for( int16_t i = 0; i < AListCount; ++i )
			{
				uint8_t ACount = pgm_read_byte( bitmap ++ );
				uint16_t AColor = Func::Color565( pgm_read_dword( bitmap ));
				uint8_t AHi = AColor >> 8;
				uint8_t ALo = AColor;
				bitmap += 4;
				for( int16_t j = 0; j <= ACount; ++ j )
				{
					AImplementation.w_data( AHi );
					AImplementation.w_data( ALo );
				}
			}
		}

//#endif // _GRAPHICS_DRAW_REPEAT_ALPHA_COLOR_BITMAP_

//#ifdef _GRAPHICS_DRAW_REPEAT_COLOR_BITMAP_

		static void drawRepeatColorBitmap( self &AImplementation, int16_t x, int16_t y, const uint8_t *bitmap, int16_t AListCount, int16_t w, int16_t h, uint8_t size )
		{
			uint16_t AHeight = AImplementation.height();
			uint16_t AWidth = AImplementation.width();

			if( ( size > 1 ) || ( x < 0 ) || ( y < 0 ) || ( x + w > AWidth ) || ( x + w > AHeight ))
			{
				inherited::drawRepeatColorBitmap( AImplementation, x, y, bitmap, AListCount, w, h, size );
				return;
			}

			AImplementation.setAddrWindow(x,y, x + w - 1, y + h - 1);

			AImplementation.cmd( 0x2C );

			for( int16_t i = 0; i < AListCount; ++i )
			{
				uint8_t ACount = pgm_read_byte( bitmap ++ );

				uint32_t ARed = pgm_read_byte( bitmap ++ );
				uint32_t AGreen = pgm_read_byte( bitmap ++ );
				uint32_t ABlue = pgm_read_byte( bitmap ++ );
				uint16_t AColor = Func::Color565( ( ARed << 16 ) | ( AGreen << 8 ) | ABlue );

				uint8_t AHi = AColor >> 8;
				uint8_t ALo = AColor;
				for( int16_t j = 0; j <= ACount; ++ j )
				{
					AImplementation.w_data( AHi );
					AImplementation.w_data( ALo );
				}
			}
		}

//#endif // _GRAPHICS_DRAW_REPEAT_COLOR_BITMAP_

//#ifdef _GRAPHICS_DRAW_GRAYSCALE_REPEAT_BITMAP_

		static void drawRepeatGrayscaleBitmap( self &AImplementation, int16_t x, int16_t y, const uint8_t *bitmap, int16_t AListCount, int16_t w, int16_t h, uint8_t AAlpha, uint8_t size )
		{
			uint16_t AHeight = AImplementation.height();
			uint16_t AWidth = AImplementation.width();

			if( ( size > 1 ) || ( x < 0 ) || ( y < 0 ) || ( x + w > AWidth ) || ( x + w > AHeight ))
			{
				inherited::drawRepeatGrayscaleBitmap( AImplementation, x, y, bitmap, AListCount, w, h, AAlpha, size );
				return;
			}

			AImplementation.setAddrWindow(x,y, x + w - 1, y + h - 1);

			AImplementation.cmd( 0x2C );

			for( int16_t i = 0; i < AListCount; ++i )
			{
				uint8_t ACount = pgm_read_byte( bitmap ++ );

				uint32_t AGray = pgm_read_byte( bitmap ++ );
				uint16_t AColor = Func::Color565( ( AGray << 8 ) | ( AGray << 16 ) | AGray );

				uint8_t AHi = AColor >> 8;
				uint8_t ALo = AColor;
				for( int16_t j = 0; j <= ACount; ++ j )
				{
					AImplementation.w_data( AHi );
					AImplementation.w_data( ALo );
				}
			}
		}

//#endif // _GRAPHICS_DRAW_GRAYSCALE_REPEAT_BITMAP_

//#ifdef _GRAPHICS_DRAW_GRAYSCALE_REPEAT_ALPHA_BITMAP_

		static void drawAlphaGrayscaleRepeatBitmap( self &AImplementation, int16_t x, int16_t y, const uint8_t *bitmap, int16_t AListCount, int16_t w, int16_t h, uint8_t size )
		{
			uint16_t AHeight = AImplementation.height();
			uint16_t AWidth = AImplementation.width();

			if( ( size > 1 ) || ( x < 0 ) || ( y < 0 ) || ( x + w > AWidth ) || ( x + w > AHeight ))
			{
				inherited::drawAlphaGrayscaleRepeatBitmap( AImplementation, x, y, bitmap, AListCount, w, h, size );
				return;
			}

			AImplementation.setAddrWindow(x,y, x + w - 1, y + h - 1);

			AImplementation.cmd( 0x2C );

			for( int16_t i = 0; i < AListCount; ++i )
			{
				uint8_t ACount = pgm_read_byte( bitmap );

				bitmap += 2;

				uint32_t AGray = pgm_read_byte( bitmap ++ );
				uint16_t AColor = Func::Color565( ( AGray << 8 ) | ( AGray << 16 ) | AGray );

				uint8_t AHi = AColor >> 8;
				uint8_t ALo = AColor;
				for( int16_t j = 0; j <= ACount; ++ j )
				{
					AImplementation.w_data( AHi );
					AImplementation.w_data( ALo );
				}
			}
		}

//#endif // _GRAPHICS_DRAW_GRAYSCALE_REPEAT_ALPHA_BITMAP_

		static void drawGrayscalePixel( self &AImplementation, int16_t x, int16_t y, TUniColor color )
		{
			uint16_t AGray = ( color & 0xFF );
			AImplementation.drawPixel( x, y, color << 16 | ( AGray << 8 ) | AGray );
		}

		void drawPixel( int16_t x, int16_t y, TUniColor color )
		{
			uint16_t AHeight = height();
			uint16_t AWidth = width();

			if((x < 0) ||(x >= AWidth) || (y < 0) || (y >= AHeight)) 
				return;

			uint8_t AAlpha = color >> 24;
			if( AAlpha == 0 )
				return;

			T_IMPLEMENTATION::drawPixel( x, y, color, AAlpha );
		}

		static void fillGrayscaleRect( self &AImplementation, int16_t x, int16_t y, int16_t w, int16_t h, TUniColor color)
		{
//			Serial.print( x ); Serial.print( " " ); Serial.print( y ); Serial.print( " " ); Serial.print( w ); Serial.print( " " ); Serial.print( h ); Serial.print( " " ); Serial.println( uint32_t( color ));

			uint16_t AGray = ( color & 0xFF );
			fillRect( AImplementation, x, y, w, h, color << 16 | ( AGray << 8 ) | AGray );
		}

//#if ( defined(__ILI9341_SCROLL__) && defined(__PARALLEL_COMPONENT_DISPLAY_ILI9341__))

		static inline void Scroll( self &AImplementation, TGraphicsPos X, TGraphicsPos Y, TUniColor color )
		{
			T_IMPLEMENTATION::template InternalScroll<self>( AImplementation, AImplementation.width(), AImplementation.height(), X, Y, color );
		}

//#endif

		static void drawFastHLine( self &AImplementation, int16_t x, int16_t y, int16_t w, TUniColor color )
		{
			// Rudimentary clipping
			uint16_t AHeight = AImplementation.height();
			uint16_t AWidth = AImplementation.width();
			if((x >= AWidth ) || (y >= AHeight )) 
				return;

			uint8_t AAlpha = color >> 24;
			if( AAlpha < 255 )
			{
				inherited::drawFastHLine( AImplementation, x, y, w, color );
				return;
			}

			if((x+w-1) >= AWidth )
				w = AWidth - x;

			color = Func::Color565( color );
			AImplementation.setAddrWindow(x, y, x+w-1, y);

			uint8_t hi = color >> 8, lo = color;

			AImplementation.cmd( 0x2C );

			while (w--) 
			{
				AImplementation.w_data(hi);
				AImplementation.w_data(lo);
			}
		}

		static void drawFastVLine( self &AImplementation, int16_t x, int16_t y, int16_t h, TUniColor color )
		{
			// Rudimentary clipping
			uint16_t AHeight = AImplementation.height();
			uint16_t AWidth = AImplementation.width();
			if((x >= AWidth) || (y >= AHeight)) 
				return;

			uint8_t AAlpha = color >> 24;
			if( AAlpha == 0 )
				return;

			if( AAlpha < 255 )
			{
				inherited::drawFastVLine( AImplementation, x, y, h, color );
				return;
			}

			if((y+h-1) >= AHeight) 
				h = AHeight-y;

			color = Func::Color565( color );
			AImplementation.setAddrWindow(x, y, x, y+h-1);

			uint8_t hi = color >> 8, lo = color;

			AImplementation.cmd( 0x2C );

			while (h--) 
			{
				AImplementation.w_data(hi);
				AImplementation.w_data(lo);
			}
    
		}

		static void fillRect( self &AImplementation, int16_t x, int16_t y, int16_t w, int16_t h, TUniColor color )
		{
			// rudimentary clipping (drawChar w/big text requires this)
			int16_t AWidth = AImplementation.width();
			int16_t AHeight = AImplementation.height();

			if((x >= AWidth) || (y >= AHeight))
				return;

			uint8_t AAlpha = color >> 24;
			if( AAlpha == 0 )
				return;

			if( AAlpha < 255 )
			{
				inherited::fillRect( AImplementation, x, y, w, h, color );
				return;
			}

			AImplementation.fillRectImplementation( AWidth, AHeight, x, y, w, h, color );
		}

		static inline void ClearScreen( self &AImplementation, TUniColor color )
		{
			AImplementation.fillRectImplementation( AImplementation.width(), AImplementation.height(), 0, 0, AImplementation.width(), AImplementation.height(), 0xFF000000 | color );
		}

		TUniColor GetPixelColor( int16_t x, int16_t y )
		{
			if((x < 0) ||(x >= width()) || (y < 0) || (y >= height())) 
				return 0x0000;

			return T_IMPLEMENTATION::GetPixelColor( x, y );
		}

	protected:
		void cmd( uint8_t cmd )
		{
			T_IMPLEMENTATION::cmd( cmd );
		}

		void  w_data( uint8_t data )
		{
			T_IMPLEMENTATION::w_data( data );
		}

	public:
		inline static size_t write( self &AImplementation, uint8_t AValue )
		{
			return inherited::write( AImplementation, AValue );
		}

		template<typename T> void PrintChar( T AValue )
		{
			write( *this, AValue );
		}

	public:
		template<typename T> void Print( T AValue )
		{
			println( AValue );
		}

	public:
		template<typename T> size_t print( T AValue )
		{
			GraphicsPrint<self> APrint( this, (typename GraphicsPrint<self>::TCallback) &self::write );
			return APrint.print( AValue );
		}

		template<typename T> size_t println( T AValue )
		{
			GraphicsPrint<self> APrint( this, (typename GraphicsPrint<self>::TCallback) &self::write );
			return APrint.println( AValue );
		}

/*
	public:
		inline void SystemLoopBegin( unsigned long currentMicros )
		{
			T_IMPLEMENTATION::SystemLoopBegin( currentMicros, Orientation );
		}
*/

	public:
		DisplayILI9341( const unsigned char * AFont ) :
			inherited( AFont )
		{
		}
	};
//---------------------------------------------------------------------------
}

#ifdef __TEST_FOR_DEBUG_PRINTS__
#undef Serial
#endif

#endif
