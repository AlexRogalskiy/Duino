////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2018 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_DISPLAY_ST7735_h
#define _MITOV_DISPLAY_ST7735_h

#include <Mitov_Graphics_Color.h>
#include <SPI.h>

#ifdef _MITOV_DISPLAY_ST7735R_BLACKTAB_
  #define _MITOV_DISPLAY_ST7735R_REDTAB_
#endif

#ifdef __TEST_FOR_DEBUG_PRINTS__
#define Serial UNGUARDED DEBUG PRINT!!!
#endif

// #define __ST7735__DEBUG__

namespace Mitov
{
namespace ST7735
{
	const uint8_t	NOP		= 0x00;
	const uint8_t	SWRESET	= 0x01;
	const uint8_t	RDDID	= 0x04;
	const uint8_t	RDDST	= 0x09;

	const uint8_t	SLPIN	= 0x10;
	const uint8_t	SLPOUT	= 0x11;
	const uint8_t	PTLON	= 0x12;
	const uint8_t	NORON	= 0x13;

	const uint8_t	INVOFF	= 0x20;
	const uint8_t	INVON	= 0x21;
	const uint8_t	DISPOFF	= 0x28;
	const uint8_t	DISPON	= 0x29;
	const uint8_t	CASET	= 0x2A;
	const uint8_t	RASET	= 0x2B;
	const uint8_t	RAMWR	= 0x2C;
	const uint8_t	RAMRD	= 0x2E;

	const uint8_t	PTLAR	= 0x30;
	const uint8_t	COLMOD	= 0x3A;
	const uint8_t	MADCTL	= 0x36;

	const uint8_t	FRMCTR1	= 0xB1;
	const uint8_t	FRMCTR2	= 0xB2;
	const uint8_t	FRMCTR3	= 0xB3;
	const uint8_t	INVCTR	= 0xB4;
	const uint8_t	DISSET5	= 0xB6;

	const uint8_t	PWCTR1	= 0xC0;
	const uint8_t	PWCTR2	= 0xC1;
	const uint8_t	PWCTR3	= 0xC2;
	const uint8_t	PWCTR4	= 0xC3;
	const uint8_t	PWCTR5  = 0xC4;
	const uint8_t	VMCTR1  = 0xC5;

	const uint8_t	RDID1   = 0xDA;
	const uint8_t	RDID2   = 0xDB;
	const uint8_t	RDID3   = 0xDC;
	const uint8_t	RDID4   = 0xDD;

	const uint8_t	PWCTR6  = 0xFC;

	const uint8_t	GMCTRP1 = 0xE0;
	const uint8_t	GMCTRN1 = 0xE1;

	const uint8_t	DELAY	= 0x80;

	const uint8_t	MADCTL_MY  = 0x80;
	const uint8_t	MADCTL_MX  = 0x40;
	const uint8_t	MADCTL_MV  = 0x20;
	const uint8_t	MADCTL_ML  = 0x10;
	const uint8_t	MADCTL_RGB = 0x00;
	const uint8_t	MADCTL_BGR = 0x08;
	const uint8_t	MADCTL_MH  = 0x04;

#if defined (SPI_HAS_TRANSACTION)
#elif defined (__AVR__)
	uint8_t mySPCR;
#endif
}

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

//---------------------------------------------------------------------------
#ifdef _MITOV_DISPLAY_ST7735R_GREENTAB_160_80_
#endif
//---------------------------------------------------------------------------
#ifndef _MITOV_DISPLAY_ST7735B_
static const uint8_t PROGMEM
  Rcmd3[] = {                 // Init for 7735R, part 3 (red or green tab)
    4,                        //  4 commands in list:
    ST7735::GMCTRP1, 16      , //  1: Magical unicorn dust, 16 args, no delay:
      0x02, 0x1c, 0x07, 0x12,
      0x37, 0x32, 0x29, 0x2d,
      0x29, 0x25, 0x2B, 0x39,
      0x00, 0x01, 0x03, 0x10,
    ST7735::GMCTRN1, 16      , //  2: Sparkles and rainbows, 16 args, no delay:
      0x03, 0x1d, 0x07, 0x06,
      0x2E, 0x2C, 0x29, 0x2D,
      0x2E, 0x2E, 0x37, 0x3F,
      0x00, 0x00, 0x02, 0x10,
    ST7735::NORON  ,    ST7735::DELAY, //  3: Normal display on, no args, w/delay
      10,                     //     10 ms delay
    ST7735::DISPON ,    ST7735::DELAY, //  4: Main screen turn on, no args w/delay
      100 };                  //     100 ms delay
#endif
//---------------------------------------------------------------------------
#ifdef _MITOV_DISPLAY_ST7735B_
#endif
//---------------------------------------------------------------------------
	template<typename T_IMPLEMENTATION> class DisplayST7735Basic : public T_IMPLEMENTATION, public GraphicsImpl<DisplayST7735Basic<T_IMPLEMENTATION> > //, public GraphicsImpl
	{
		typedef DisplayST7735Basic<T_IMPLEMENTATION>	self;
		typedef GraphicsImpl<self> inherited;

	public:
		TGraphicsColorTextSettings	Text;

	public:

//#ifdef _GRAPHICS_DRAW_ALPHA_COLOR_BITMAP_

		static inline void drawAlphaColorBitmap( self &AImplementation, int16_t x, int16_t y, const uint32_t *bitmap, int16_t w, int16_t h, uint8_t size )
		{
			uint16_t AHeight = AImplementation.height();
			uint16_t AWidth = AImplementation.width();

			if( ( size > 1 ) || ( x < 0 ) || ( y < 0 ) || ( x + w > AWidth ) || ( x + w > AHeight ))
			{
				inherited::drawAlphaColorBitmap( AImplementation, x, y, bitmap, w, h, size );
				return;
			}

			AImplementation.drawAlphaColorBitmapImplementation( x, y, bitmap, w, h, size );
		}

		void drawAlphaColorBitmapImplementation( int16_t x, int16_t y, const uint32_t *bitmap, int16_t w, int16_t h, uint8_t size )
		{
			// check rotation, move pixel around if necessary

			T_IMPLEMENTATION::setAddrWindow(x,y,x + w - 1, y + h - 1);
			T_IMPLEMENTATION::BeginTransaction();
			T_IMPLEMENTATION::SetRegisterSelectOutput( true );
			T_IMPLEMENTATION::SetChipSelectOutput( false );

			for( int16_t j=0; j<h; j++)
				for( int16_t i=0; i<w; ++i )
				{
					uint16_t AColor = Func::Color565( pgm_read_dword( bitmap++ ));
					T_IMPLEMENTATION::spiwrite16( AColor );
				}

			T_IMPLEMENTATION::SetChipSelectOutput( true );
			T_IMPLEMENTATION::EndTransaction();
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

			AImplementation.drawColorBitmapImplementation( x, y, bitmap, w, h, size );
		}

		static void drawColorBitmapImplementation( int16_t x, int16_t y, const uint8_t *bitmap, int16_t w, int16_t h, uint8_t size )
		{
			T_IMPLEMENTATION::setAddrWindow(x,y,x + w - 1, y + h - 1);
			T_IMPLEMENTATION::BeginTransaction();
			T_IMPLEMENTATION::SetRegisterSelectOutput( true );
			T_IMPLEMENTATION::SetChipSelectOutput( false );

			for( int16_t j=0; j<h; j++)
				for( int16_t i=0; i<w; ++i )
				{
					uint32_t ARed = pgm_read_byte( bitmap ++ );
					uint32_t AGreen = pgm_read_byte( bitmap ++ );
					uint32_t ABlue = pgm_read_byte( bitmap ++ );
					uint16_t AColor = Func::Color565( ( AGreen << 8 ) | ( ARed << 16 ) | ABlue );

					T_IMPLEMENTATION::spiwrite16( AColor );
				}

			T_IMPLEMENTATION::SetChipSelectOutput( true );
			T_IMPLEMENTATION::EndTransaction();
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

			AImplementation.drawGrayscaleBitmapImplementation( x, y, bitmap, w, h, size );
		}

		void drawGrayscaleBitmapImplementation( int16_t x, int16_t y, const uint8_t *bitmap, int16_t w, int16_t h, uint8_t AAlpha, uint8_t size )
		{
			T_IMPLEMENTATION::setAddrWindow(x,y,x + w - 1, y + h - 1);
			T_IMPLEMENTATION::BeginTransaction();
			T_IMPLEMENTATION::SetRegisterSelectOutput( true );
			T_IMPLEMENTATION::SetChipSelectOutput( false );

			for( int16_t j=0; j<h; j++)
				for( int16_t i=0; i<w; ++i )
				{
					uint32_t AGray = pgm_read_byte( bitmap ++ );
					uint16_t AColor = Func::Color565( ( AGray << 8 ) | ( AGray << 16 ) | AGray );

					T_IMPLEMENTATION::spiwrite16( AColor );
				}

			T_IMPLEMENTATION::SetChipSelectOutput( true );
			T_IMPLEMENTATION::EndTransaction();
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

			AImplementation.drawAlphaGrayscaleBitmapImplementation( x, y, _bitmap, w, h, size );
		}

		void drawAlphaGrayscaleBitmapImplementation( int16_t x, int16_t y, const uint16_t *_bitmap, int16_t w, int16_t h, uint8_t size )
		{
			const uint8_t *bitmap = (const uint8_t *)_bitmap;

			T_IMPLEMENTATION::setAddrWindow(x,y,x + w - 1, y + h - 1);
			T_IMPLEMENTATION::BeginTransaction();
			T_IMPLEMENTATION::SetRegisterSelectOutput( true );
			T_IMPLEMENTATION::SetChipSelectOutput( false );

			for( int16_t j=0; j<h; j++)
				for( int16_t i=0; i<w; ++i )
				{
					uint32_t AGray = pgm_read_byte( bitmap );
					uint16_t AColor = Func::Color565( ( AGray << 8 ) | ( AGray << 16 ) | AGray );
					bitmap += 2;

					T_IMPLEMENTATION::spiwrite16( AColor );
				}

			T_IMPLEMENTATION::SetChipSelectOutput( true );
			T_IMPLEMENTATION::EndTransaction();
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

			AImplementation.drawBWBitmapImplementation( x, y, bitmap, w, h, size );
		}

		void drawBWBitmapImplementation( int16_t x, int16_t y, const uint8_t *bitmap, int16_t w, int16_t h, TUniColor color, TUniColor bg, uint8_t size )
		{
			int16_t byteWidth = (w + 7) / 8;

//			color = Func::Color565( color );
//			bg = Func::Color565( bg );
  
			T_IMPLEMENTATION::setAddrWindow(x,y,x + w - 1, y + h - 1);
			T_IMPLEMENTATION::BeginTransaction();
			T_IMPLEMENTATION::SetRegisterSelectOutput( true );
			T_IMPLEMENTATION::SetChipSelectOutput( false );

			for( int16_t j=0; j<h; j++)
				for( int16_t i=0; i<w; ++i )
				{
					uint16_t AColor;
					if(pgm_read_byte(bitmap + j * byteWidth + i / 8) & (128 >> ( i & 0b111 )))
						AColor = 0xFFFF;

					else 
      					AColor = 0;

					T_IMPLEMENTATION::spiwrite16( AColor );
				}

			T_IMPLEMENTATION::SetChipSelectOutput( true );
			T_IMPLEMENTATION::EndTransaction();
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

			AImplementation.drawRepeatAlphaColorBitmapImplementation( x, y, bitmap, w, h, size );
		}

		void drawRepeatAlphaColorBitmapImplementation( int16_t x, int16_t y, const uint8_t *bitmap, int16_t AListCount, int16_t w, int16_t h, uint8_t size )
		{
			T_IMPLEMENTATION::setAddrWindow(x,y, x + w - 1, y + h - 1);
			T_IMPLEMENTATION::BeginTransaction();
			T_IMPLEMENTATION::SetRegisterSelectOutput( true );
			T_IMPLEMENTATION::SetChipSelectOutput( false );

			for( int16_t i = 0; i < AListCount; ++i )
			{
				uint8_t ACount = pgm_read_byte( bitmap ++ );
				uint16_t AColor = Func::Color565( pgm_read_dword( bitmap ));
				uint8_t AHi = AColor >> 8;
				uint8_t ALo = AColor;
				bitmap += 4;
				for( int16_t j = 0; j <= ACount; ++ j )
					T_IMPLEMENTATION::spiwrite( AHi, ALo );

			}

			T_IMPLEMENTATION::SetChipSelectOutput( true );
			T_IMPLEMENTATION::EndTransaction();
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

			AImplementation.drawRepeatColorBitmapImplementation( x, y, bitmap, AListCount, w, h, size );
		}

		void drawRepeatColorBitmapImplementation( int16_t x, int16_t y, const uint8_t *bitmap, int16_t AListCount, int16_t w, int16_t h, uint8_t size )
		{
			T_IMPLEMENTATION::setAddrWindow(x,y, x + w - 1, y + h - 1);
			T_IMPLEMENTATION::BeginTransaction();
			T_IMPLEMENTATION::SetRegisterSelectOutput( true );
			T_IMPLEMENTATION::SetChipSelectOutput( false );

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
					T_IMPLEMENTATION::spiwrite( AHi, ALo );
			}

			T_IMPLEMENTATION::SetChipSelectOutput( true );
			T_IMPLEMENTATION::EndTransaction();
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

			AImplementation.drawRepeatGrayscaleBitmapImplementation( x, y, bitmap, w, h, size );
		}

		void drawRepeatGrayscaleBitmapImplementation( int16_t x, int16_t y, const uint8_t *bitmap, int16_t AListCount, int16_t w, int16_t h, uint8_t AAlpha, uint8_t size )
		{
			T_IMPLEMENTATION::setAddrWindow(x,y, x + w - 1, y + h - 1);
			T_IMPLEMENTATION::BeginTransaction();
			T_IMPLEMENTATION::SetRegisterSelectOutput( true );
			T_IMPLEMENTATION::SetChipSelectOutput( false );

			for( int16_t i = 0; i < AListCount; ++i )
			{
				uint8_t ACount = pgm_read_byte( bitmap ++ );

				uint32_t AGray = pgm_read_byte( bitmap ++ );
				uint16_t AColor = Func::Color565( ( AGray << 8 ) | ( AGray << 16 ) | AGray );

				uint8_t AHi = AColor >> 8;
				uint8_t ALo = AColor;
				for( int16_t j = 0; j <= ACount; ++ j )
					T_IMPLEMENTATION::spiwrite( AHi, ALo );

			}

			T_IMPLEMENTATION::SetChipSelectOutput( true );
			T_IMPLEMENTATION::EndTransaction();
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

			AImplementation.drawAlphaGrayscaleRepeatBitmapImplementation( x, y, bitmap, AListCount, w, h, size );
		}

		void drawAlphaGrayscaleRepeatBitmapImplementation( int16_t x, int16_t y, const uint8_t *bitmap, int16_t AListCount, int16_t w, int16_t h, uint8_t size )
		{
			T_IMPLEMENTATION::setAddrWindow(x,y, x + w - 1, y + h - 1);
			T_IMPLEMENTATION::BeginTransaction();
			T_IMPLEMENTATION::SetRegisterSelectOutput( true );
			T_IMPLEMENTATION::SetChipSelectOutput( false );

			for( int16_t i = 0; i < AListCount; ++i )
			{
				uint8_t ACount = pgm_read_byte( bitmap );

				bitmap += 2;

				uint32_t AGray = pgm_read_byte( bitmap ++ );
				uint16_t AColor = Func::Color565( ( AGray << 8 ) | ( AGray << 16 ) | AGray );

				uint8_t AHi = AColor >> 8;
				uint8_t ALo = AColor;
				for( int16_t j = 0; j <= ACount; ++ j )
					T_IMPLEMENTATION::spiwrite( AHi, ALo );

			}

			T_IMPLEMENTATION::SetChipSelectOutput( true );
			T_IMPLEMENTATION::EndTransaction();
		}

//#endif // _GRAPHICS_DRAW_GRAYSCALE_REPEAT_ALPHA_BITMAP_

		static inline void drawGrayscalePixel( self &AImplementation, int16_t x, int16_t y, TUniColor color )
		{
			uint16_t AGray = ( color & 0xFF );
			AImplementation.drawPixel( x, y, color << 16 | ( AGray << 8 ) | AGray );
		}

		void drawPixel(int16_t x, int16_t y, TUniColor color)
		{
			uint16_t AHeight = T_IMPLEMENTATION::height();
			uint16_t AWidth = T_IMPLEMENTATION::width();

			if((x < 0) ||(x >= AWidth) || (y < 0) || (y >= AHeight)) 
				return;

			color = Func::Color565( color );

			T_IMPLEMENTATION::setAddrWindow(x,y,x+1,y+1);
			T_IMPLEMENTATION::BeginTransaction();
			T_IMPLEMENTATION::SetRegisterSelectOutput( true );
			T_IMPLEMENTATION::SetChipSelectOutput( false );

			T_IMPLEMENTATION::spiwrite16(color);

			T_IMPLEMENTATION::SetChipSelectOutput( true );
			T_IMPLEMENTATION::EndTransaction();
		}

		static inline void fillGrayscaleRect( self &AImplementation, int16_t x, int16_t y, int16_t w, int16_t h, TUniColor color)
		{
			uint16_t AGray = ( color & 0xFF );
			fillRect( AImplementation, x, y, w, h, color << 16 | ( AGray << 8 ) | AGray );
		}

		static inline void drawFastHLine( self &AImplementation, int16_t x, int16_t y, int16_t w, TUniColor color)
		{
			AImplementation.drawFastHLineImplementation( x, y, w, color );
		}

		void drawFastHLineImplementation( int16_t x, int16_t y, int16_t w, TUniColor color )
		{
			// Rudimentary clipping
			uint16_t AHeight = T_IMPLEMENTATION::height();
			uint16_t AWidth = T_IMPLEMENTATION::width();
			if((x >= AWidth ) || (y >= AHeight )) 
				return;

			if((x+w-1) >= AWidth )
				w = AWidth - x;

			color = Func::Color565( color );
			T_IMPLEMENTATION::setAddrWindow(x, y, x+w-1, y);

			uint8_t AHi = color >> 8, ALo = color;

			T_IMPLEMENTATION::BeginTransaction();
			T_IMPLEMENTATION::SetRegisterSelectOutput( true );
			T_IMPLEMENTATION::SetChipSelectOutput( false );
			while (w--) 
				T_IMPLEMENTATION::spiwrite( AHi, ALo );

			T_IMPLEMENTATION::SetChipSelectOutput( true );
			T_IMPLEMENTATION::EndTransaction();
		}

		static inline void drawFastVLine( self &AImplementation, int16_t x, int16_t y, int16_t h, TUniColor color )
		{
			AImplementation.drawFastVLineImplementation( x, y, h, color );
		}

		void drawFastVLineImplementation( int16_t x, int16_t y, int16_t h, TUniColor color )
		{
			// Rudimentary clipping
			uint16_t AHeight = T_IMPLEMENTATION::height();
			uint16_t AWidth = T_IMPLEMENTATION::width();
			if((x >= AWidth) || (y >= AHeight)) 
				return;

			if((y+h-1) >= AHeight) 
				h = AHeight-y;

			color = Func::Color565( color );
			T_IMPLEMENTATION::setAddrWindow(x, y, x, y+h-1);

			uint8_t AHi = color >> 8, ALo = color;
    
			T_IMPLEMENTATION::BeginTransaction();
			T_IMPLEMENTATION::SetRegisterSelectOutput( true );
			T_IMPLEMENTATION::SetChipSelectOutput( false );
			while (h--) 
				T_IMPLEMENTATION::spiwrite( AHi, ALo );

			T_IMPLEMENTATION::SetChipSelectOutput( true );
			T_IMPLEMENTATION::EndTransaction();
		}

		inline TUniColor GetPixelColor( int16_t x, int16_t y )
		{
			return 0x0000; // Not supported
		}

		static inline void Scroll( self &AImplementation, TGraphicsPos X, TGraphicsPos Y, TUniColor color )
		{
			// Not supported
		}

/*
		TUniColor GetPixelColor( int16_t x, int16_t y )
		{
			if((x < 0) || (x >= width() ) || (y < 0) || (y >= height()))
				return 0x0000;

			T_IMPLEMENTATION::setAddrWindow(x,y,x+1,y+1, ST7735::RAMRD);
			T_IMPLEMENTATION::BeginTransaction();
			T_IMPLEMENTATION::SetRegisterSelectOutput( true );
			T_IMPLEMENTATION::SetChipSelectOutput( false );

			uint16_t AColor = uint16_t( spiread()) << 8;
			AColor |= spiread();
//			T_IMPLEMENTATION::spiwrite(color >> 8);
//			T_IMPLEMENTATION::spiwrite(color);

			T_IMPLEMENTATION::SetChipSelectOutput( true );
			T_IMPLEMENTATION::EndTransaction();
			return AColor;
		}
*/
		static inline void fillRect( self &AImplementation, int16_t x, int16_t y, int16_t w, int16_t h, TUniColor color)
		{
			// rudimentary clipping (drawChar w/big text requires this)
			AImplementation.fillRectImplementation( x, y, w, h, color );
		}

		void fillRectImplementation( int16_t x, int16_t y, int16_t w, int16_t h, TUniColor color)
		{
			int16_t AWidth = T_IMPLEMENTATION::width();
			int16_t AHeight = T_IMPLEMENTATION::height();

			if((x >= AWidth) || (y >= AHeight))
				return;

			if((x + w - 1) >= AWidth)
				w = AWidth - x;

			if((y + h - 1) >= AHeight)
				h = AHeight - y;

			T_IMPLEMENTATION::setAddrWindow(x, y, x+w-1, y+h-1);

			color = Func::Color565( color );

			uint8_t AHi = color >> 8, ALo = color;
    
			T_IMPLEMENTATION::BeginTransaction();
			T_IMPLEMENTATION::SetRegisterSelectOutput( true );
			T_IMPLEMENTATION::SetChipSelectOutput( false );
			for(y=h; y>0; y--)
				for(x=w; x>0; x--)
					T_IMPLEMENTATION::spiwrite( AHi, ALo );

			T_IMPLEMENTATION::SetChipSelectOutput( true );
			T_IMPLEMENTATION::EndTransaction();
		}

		static inline void ClearScreen( self &AImplementation, TUniColor color )
		{
			fillRect( AImplementation, 0, 0, AImplementation.width(), AImplementation.height(), 0xFF000000 | color );
		}

	public:
//		inline void SystemInit()
//		{
//		}

		inline void SystemStart()
		{
			T_IMPLEMENTATION::UpdateOrientation();
			T_IMPLEMENTATION::UpdateInverted();

			ClearScreen( *this, inherited::BackgroundColor );

			inherited::setTextSize( Text.Size );
			inherited::setTextColor( Text.Color, Text.BackgroundColor );
			inherited::setTextWrap( Text.Wrap );

			inherited::RenderElements();
//			for( int i = 0; i < FElements.size(); ++ i )
//				FElements[ i ]->Render( false );

		}

	public:
		inline static size_t write( self &AImplementation, uint8_t AValue )
		{
//			Serial.println( "WRITE" );
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

	public:
//		using inherited::inherited;

		DisplayST7735Basic( const unsigned char * AFont ) :
			inherited( AFont )
		{
		}

	};
//---------------------------------------------------------------------------
	template<uint8_t C_PIN_NUMBER>  class ST7735_Reset_StandardPinImplementation
	{
	public:
		OpenWire::SourcePin	ResetOutputPin;

	public:
		void ResetDisplay()
		{
			ResetOutputPin.SendValue<bool>( true );
			delay(500);
			ResetOutputPin.SendValue<bool>( false );
			delay(500);
			ResetOutputPin.SendValue<bool>( true );
			delay(500);
		}

	};
//---------------------------------------------------------------------------
	template<uint8_t C_PIN_NUMBER>  class ST7735_Reset_NoImplementation
	{
	public:
		inline void ResetDisplay()
		{
		}

	};
//---------------------------------------------------------------------------
	template<uint8_t C_PIN_NUMBER>  class ST7735_Reset_DirectBoardPinImplementation
	{
	public:
		void ResetDisplay()
		{
			digitalWrite( C_PIN_NUMBER, HIGH );
			delay(500);
			digitalWrite( C_PIN_NUMBER, LOW );
			delay(500);
			digitalWrite( C_PIN_NUMBER, HIGH );
			delay(500);
		}

	public:
		ST7735_Reset_DirectBoardPinImplementation()
		{
			pinMode( C_PIN_NUMBER, OUTPUT );
		}

	};
//---------------------------------------------------------------------------
	template<typename T_PIN, T_PIN &C_PIN>  class ST7735_Reset_DirectPinImplementation
	{
	public:
		void ResetDisplay()
		{
			C_PIN.DigitalWrite( true );
			delay(500);
			C_PIN.DigitalWrite( false );
			delay(500);
			C_PIN.DigitalWrite( true );
			delay(500);
		}

	public:
		ST7735_Reset_DirectPinImplementation()
		{
			C_PIN.SetMode( OUTPUT );
		}

	};
//---------------------------------------------------------------------------
	template<typename T_SPI, T_SPI &C_SPI, typename T_RESET_IMPLEMENTATION, typename T_REGISTER_SELECT_IMPLEMENTATION, typename T_CHIP_SELECT_IMPLEMENTATION, uint8_t C_OFFSET_X, uint8_t C_OFFSET_Y> class BasicSPI_ST7735 : public T_RESET_IMPLEMENTATION, public T_REGISTER_SELECT_IMPLEMENTATION, public T_CHIP_SELECT_IMPLEMENTATION
	{
//	public:
//		GraphicsOrientation	Orientation : 2; //= goUp;

	public:
//		bool	Inverted : 1; //= false;
		bool	Inverted = false;

	public:
		inline void UpdateInverted() // Used by the Binding Setters
		{
			writecommand( ( Inverted ) ? ST7735::INVON : ST7735::INVOFF );
		}

	protected:
		inline void BeginTransaction()
		{
#if defined (SPI_HAS_TRANSACTION)
			C_SPI.beginTransaction(SPISettings(8000000, MSBFIRST, SPI_MODE0));
#endif
		}

		inline void EndTransaction()
		{
#if defined (SPI_HAS_TRANSACTION)
			C_SPI.endTransaction();
#endif
		}

		inline void SetRegisterSelectOutput( bool AValue )
		{
			T_REGISTER_SELECT_IMPLEMENTATION::SetPinValue( AValue );
		}

		inline void SetChipSelectOutput( bool AValue )
		{
			T_CHIP_SELECT_IMPLEMENTATION::SetPinValue( AValue );
		}

	protected:
		
		inline void spiwrite( uint8_t AHi, uint8_t ALo) 
		{
#ifdef __ST7735__DEBUG__
		  Serial.println(AHi, HEX);
		  Serial.println(ALo, HEX);
#endif

#if defined (SPI_HAS_TRANSACTION)
		  C_SPI.transfer( AHi );
		  C_SPI.transfer( ALo );
#elif defined (__AVR__)
		  uint8_t SPCRbackup = SPCR;
		  SPCR = ST7735::mySPCR;
		  C_SPI.transfer( AHi );
		  C_SPI.transfer( ALo );
		  SPCR = SPCRbackup;
//      SPDR = c;
//      while(!(SPSR & _BV(SPIF)));
#elif defined (__arm__)
		  C_SPI.setClockDivider(21); //4MHz
		  C_SPI.setDataMode(SPI_MODE0);
		  C_SPI.transfer( AHi );
		  C_SPI.transfer( ALo );
#endif
		}

		inline void spiwrite16( uint16_t AColor ) 
		{
#ifdef __ST7735__DEBUG__
		  Serial.println(AColor, HEX);
#endif

#if defined (SPI_HAS_TRANSACTION)
		  C_SPI.transfer( AColor >> 8 );
		  C_SPI.transfer( AColor );
#elif defined (__AVR__)
		  uint8_t SPCRbackup = SPCR;
		  SPCR = ST7735::mySPCR;
		  C_SPI.transfer( AColor >> 8 );
		  C_SPI.transfer( AColor );
		  SPCR = SPCRbackup;
//      SPDR = c;
//      while(!(SPSR & _BV(SPIF)));
#elif defined (__arm__)
		  C_SPI.setClockDivider(21); //4MHz
		  C_SPI.setDataMode(SPI_MODE0);
		  C_SPI.transfer( AColor >> 8 );
		  C_SPI.transfer( AColor );
#endif
		}

		inline void spiwrite(uint8_t c) 
		{
#ifdef __ST7735__DEBUG__
		  Serial.println(c, HEX);
#endif

#if defined (SPI_HAS_TRANSACTION)
		  C_SPI.transfer(c);
#elif defined (__AVR__)
		  uint8_t SPCRbackup = SPCR;
		  SPCR = ST7735::mySPCR;
		  C_SPI.transfer(c);
		  SPCR = SPCRbackup;
//      SPDR = c;
//      while(!(SPSR & _BV(SPIF)));
#elif defined (__arm__)
		  C_SPI.setClockDivider(21); //4MHz
		  C_SPI.setDataMode(SPI_MODE0);
		  C_SPI.transfer(c);
#endif
		}

/*
		inline uint8_t spiread()
		{
		  //Serial.println(c, HEX);

#if defined (SPI_HAS_TRANSACTION)
			uint8_t AResult = C_SPI.transfer(0);
#elif defined (__AVR__)
			uint8_t SPCRbackup = SPCR;
			SPCR = mySPCR;
			uint8_t AResult = C_SPI.transfer(0);
			SPCR = SPCRbackup;
//      SPDR = c;
//      while(!(SPSR & _BV(SPIF)));
#elif defined (__arm__)
			C_SPI.setClockDivider(21); //4MHz
			C_SPI.setDataMode(SPI_MODE0);
			uint8_t AResult = C_SPI.transfer(0);
#endif
			return AResult;
		}
*/
		void writecommand(uint8_t c) 
		{
#ifdef __ST7735__DEBUG__
			Serial.print("C ");
#endif

			BeginTransaction();
			SetRegisterSelectOutput( false );
			SetChipSelectOutput( false );

			spiwrite(c);

			SetChipSelectOutput( true );

			EndTransaction();
		}

		void writedata(uint8_t c) 
		{
#ifdef __ST7735__DEBUG__
			Serial.print("D ");
#endif

			BeginTransaction();
			SetRegisterSelectOutput( true );
			SetChipSelectOutput( false );
    
			spiwrite(c);

			SetChipSelectOutput( true );
			EndTransaction();
		}

		// Companion code to the above tables.  Reads and issues
		// a series of LCD commands stored in PROGMEM byte array.
		void commandList(const uint8_t *addr) 
		{
			uint8_t  numCommands, numArgs;
			uint16_t ms;

			numCommands = pgm_read_byte(addr++);   // Number of commands to follow
			while(numCommands--) 
			{                 // For each command...
				writecommand(pgm_read_byte(addr++)); //   Read, issue command
				numArgs  = pgm_read_byte(addr++);    //   Number of args to follow
				ms       = numArgs & ST7735::DELAY;          //   If hibit set, delay follows args
				numArgs &= ~ST7735::DELAY;                   //   Mask out delay bit
				while(numArgs--)                     //   For each argument...
					writedata(pgm_read_byte(addr++));  //     Read, issue argument

				if(ms) 
				{
					ms = pgm_read_byte(addr++); // Read post-command delay time (ms)
					if(ms == 255) 
						ms = 500;     // If 255, delay for 500 ms

					delay(ms);
				}
			}
		}

		void setAddrWindow(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1 )
		{
			writecommand(ST7735::CASET); // Column addr set
			writedata(0x00);
			writedata(x0 + C_OFFSET_X);     // XSTART 
			writedata(0x00);
			writedata(x1 + C_OFFSET_X);     // XEND

			writecommand(ST7735::RASET); // Row addr set
			writedata(0x00);
			writedata(y0 + C_OFFSET_Y);     // YSTART
			writedata(0x00);
			writedata(y1 + C_OFFSET_Y);     // YEND

			writecommand( ST7735::RAMWR ); // write to RAM
		}

		void commonInit(const uint8_t *cmdList) 
		{
#if defined (SPI_HAS_TRANSACTION)
#elif defined (__AVR__)
			uint8_t SPCRbackup = SPCR;
//			C_SPI.begin();
			C_SPI.setClockDivider(SPI_CLOCK_DIV4);
			C_SPI.setDataMode(SPI_MODE0);
			mySPCR = SPCR; // save our preferred state
			//Serial.print("mySPCR = 0x"); Serial.println(SPCR, HEX);
			SPCR = SPCRbackup;  // then restore
#elif defined (__SAM3X8E__)
//			C_SPI.begin();
			C_SPI.setClockDivider(21); //4MHz
			C_SPI.setDataMode(SPI_MODE0);
#endif
			T_RESET_IMPLEMENTATION::ResetDisplay();
/*
			if( ResetOutputPinIsConnected()) 
			{
				SetResetOutput( true );
				delay(500);
				SetResetOutput( false );
				delay(500);
				SetResetOutput( true );
				delay(500);
			}
*/
			commandList(cmdList);
		}

	public:
//		BasicSPI_ST7735() :
//			Orientation( goUp ),
//			Inverted( false )
//		{
//		}
	};
//---------------------------------------------------------------------------
/*
template<typename T_SPI, T_SPI &C_SPI> class DisplayST7735 : public DisplayST7735Basic<T_SPI, C_SPI>
	{
		typedef DisplayST7735Basic<T_SPI, C_SPI> inherited;

	public:
		using inherited::inherited;

	};
*/
//---------------------------------------------------------------------------
#ifdef _MITOV_DISPLAY_ST7735B_
#endif
//---------------------------------------------------------------------------
}

#ifdef __TEST_FOR_DEBUG_PRINTS__
#undef Serial
#endif

#endif
