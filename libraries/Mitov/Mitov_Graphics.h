////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2018 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_GRAPHICS_h
#define _MITOV_GRAPHICS_h

#include <Mitov.h>

#ifdef __TEST_FOR_DEBUG_PRINTS__
#define Serial UNGUARDED DEBUG PRINT!!!
#endif

namespace Mitov
{
//	class Graphics;
//---------------------------------------------------------------------------
#ifdef __32BIT_GRAPHICS_COORDINATES__
	typedef int32_t		TGraphicsPos;
	typedef uint32_t	TGraphicsSize;
#else
	typedef int16_t		TGraphicsPos;
	typedef uint16_t	TGraphicsSize;
#endif
//---------------------------------------------------------------------------
	enum TArduinoMonochromeColor { tmcBlack, tmcWhite, tmcInvert, tmcNone };
//---------------------------------------------------------------------------
	typedef uint64_t TUniColor;
//---------------------------------------------------------------------------
	struct GraphicsPoint
	{
		TGraphicsPos	X;
		TGraphicsPos	Y;
	};
//---------------------------------------------------------------------------
	template<typename T_OWNER> class GraphicsIntf
	{
	public:
		void ( *Elements_CallChain )() = nullptr;

	protected:
		inline void RenderElements()
		{
			if( Elements_CallChain )
				Elements_CallChain();
		}

//	protected:
//		Mitov::SimpleList<GraphicsElementBasic *>	FElements;

	public:
		inline void GetPosition( TGraphicsPos &AX, TGraphicsPos &AY ) { AX = 0; AY = 0; }
//		virtual void RegisterRender( GraphicsElementBasic *AItem ) 
//		{
//			FElements.push_back( AItem );
//		}

	};
//---------------------------------------------------------------------------
/*
	class GraphicsElementBasic
	{
	protected:
		GraphicsIntf	&FOwner;

//	public:
//		virtual void Render( bool AForce ) = 0;

	public:
		GraphicsElementBasic( GraphicsIntf &AOwner ) :
			FOwner( AOwner )
		{
//			FOwner.RegisterRender( this );
		}
	};
*/
//---------------------------------------------------------------------------
	class GraphicsTextIntf
	{
		public:
			void ( *Elements_CallChain )( bool AEnter ) = nullptr;

		public:
			void EnterLeave( bool AEnter )
			{
				if( Elements_CallChain )
					Elements_CallChain( AEnter );
			}

//	protected:
//		Mitov::SimpleList<GraphicsTextElementBasic *>	FElements;

//	public:
//		virtual void RegisterRender( GraphicsTextElementBasic *AItem ) 
//		{
//			FElements.push_back( AItem );
//		}
	};
//---------------------------------------------------------------------------
/*
	class GraphicsTextElementBasic
	{
	public:
		virtual void Enter( Graphics *AGraphics ) {}
		virtual void Leave( Graphics *AGraphics ) {}

	public:
		GraphicsTextElementBasic( GraphicsTextIntf &AOwner )
		{
			AOwner.RegisterRender( this );
		}
	};
*/
//---------------------------------------------------------------------------
	class TArduinoGraphicsCornerSize
	{
	public:
		int16_t		X = 10;
		int16_t		Y = 10;

	};
//---------------------------------------------------------------------------
	enum TArduinoTextHorizontalAlign { thaLeft, thaCenter, thaRight };
//---------------------------------------------------------------------------
	enum TArduinoTextVerticalAlign { tvaTop, tvaCenter, tvaBottom };
//---------------------------------------------------------------------------
	enum GraphicsOrientation { goUp, goRight, goDown, goLeft };
//---------------------------------------------------------------------------
	template<typename T> class GraphicsPrint : public Print
	{
	public:
		typedef size_t (*TCallback)( T &AImplementation, uint8_t c );

	protected:
		T	*FOwner;
		TCallback			FCallback;

	public:
		virtual size_t write( uint8_t c ) override
		{
//			return ( FOwner->*FCallback )( *FOwner, c );
			return FCallback( *FOwner, c );
//			return FOwner->FCallback( c );
		}

	public:
		GraphicsPrint( T *AOwner, TCallback ACallback ) : 
			FOwner( AOwner ),
			FCallback( ACallback )
		{
		}

	};
//---------------------------------------------------------------------------
	template<typename T_IMPLEMENTATION> class Graphics : public OpenWire::Object
	{
	protected:
		int16_t	cursor_x = 0;
		int16_t	cursor_y = 0;

	protected:
		const unsigned char * FFont;

	protected:
		TUniColor	textcolor = 0xFFFF;
		TUniColor	textbgcolor = 0;

		uint8_t	textsize = 1;

		bool wrap : 1;   // If set, 'wrap' text at right edge of display
		bool _cp437 : 1; // If set, use correct CP437 charset (default is off)

/*
	public:
		void PrintChar( char AValue )
		{
			write( AValue );
		}

		void PrintChar( byte AValue )
		{
			write( AValue );
		}

	public:
		template<typename T> void Print( T AValue )
		{
			println( AValue );
		}

	public:
		template<typename T> size_t print( T AValue )
		{
			GraphicsPrint APrint( this, (GraphicsPrint::TCallback) &Graphics::write );
			return APrint.print( AValue );
		}

		template<typename T> size_t println( T AValue )
		{
			GraphicsPrint APrint( this, (GraphicsPrint::TCallback) &Graphics::write );
			return APrint.println( AValue );
		}
*/
	public:
		static void Scroll( T_IMPLEMENTATION &AImplementation, TGraphicsPos X, TGraphicsPos Y, TUniColor color )
		{
			if( X == 0 && Y == 0 )
				return;

			int AFromYIndex, AToYIndex;
			int AHeight = AImplementation.height() - abs( Y );
			if( Y < 0 )
			{
				AFromYIndex = -Y;
				AToYIndex = 0;
			}

			else
			{
				AFromYIndex = 0;
				AToYIndex = Y;
			}

			int AFromXIndex, AToXIndex;
			int AWidth = AImplementation.width() - abs( X );
			if( X < 0 )
			{
				AFromXIndex = -X;
				AToXIndex = 0;
			}

			else
			{
				AFromXIndex = 0;
				AToXIndex = X;
			}

			if( Y < 0 )
			{
				for( int ay = 0; ay < AHeight; ++ay )
				{
					if( X < 0 )
					{
						for( int ax = 0; ax < AWidth; ++ax )
							AImplementation.drawPixel( ax + AToXIndex, ay + AToYIndex, AImplementation.GetPixelColor( ax + AFromXIndex, ay + AFromYIndex ));

						T_IMPLEMENTATION::drawFastHLine( AImplementation, AWidth, ay + AToYIndex, AImplementation.width() - AWidth, color );
					}

					else
					{
						for( int ax = AWidth; ax--; )
							AImplementation.drawPixel( ax + AToXIndex, ay + AToYIndex, AImplementation.GetPixelColor( ax + AFromXIndex, ay + AFromYIndex ));

						T_IMPLEMENTATION::drawFastHLine( AImplementation, 0, ay + AToYIndex, X, color );
					}
				}

				T_IMPLEMENTATION::fillRect( AImplementation, 0, AHeight, AImplementation.width(), AImplementation.height(), color );
			}

			else
			{
				for( int ay = AHeight; ay--; )
				{
					if( X < 0 )
					{
						for( int ax = 0; ax < AWidth; ++ax )
							AImplementation.drawPixel( ax + AToXIndex, ay + AToYIndex, AImplementation.GetPixelColor( ax + AFromXIndex, ay + AFromYIndex ));

						T_IMPLEMENTATION::drawFastHLine( AImplementation, AWidth, ay + AToYIndex, AImplementation.width() - AWidth, color );
					}

					else
					{
						for( int ax = AWidth; ax--; )
							AImplementation.drawPixel( ax + AToXIndex, ay + AToYIndex, AImplementation.GetPixelColor( ax + AFromXIndex, ay + AFromYIndex ));

						T_IMPLEMENTATION::drawFastHLine( AImplementation, 0, ay + AToYIndex, X, color );
					}
				}

				T_IMPLEMENTATION::fillRect( AImplementation, 0, 0, AImplementation.width(), Y, color );
			}
		}

		static void drawGrayscalePixel( T_IMPLEMENTATION &AImplementation, int16_t x, int16_t y, TUniColor color )
		{
			AImplementation.drawPixel( x, y, color );
		}

		static void drawColorPixel( T_IMPLEMENTATION &AImplementation, int16_t x, int16_t y, TUniColor color )
		{
			AImplementation.drawPixel( x, y, color );
		}

		static void fillGrayscaleRect( T_IMPLEMENTATION &AImplementation, int16_t x, int16_t y, int16_t w, int16_t h, TUniColor color)
		{
			T_IMPLEMENTATION::fillRect( AImplementation, x, y, w, h, color );
		}

		static void fillColorRect( T_IMPLEMENTATION &AImplementation, int16_t x, int16_t y, int16_t w, int16_t h, TUniColor color)
		{
			T_IMPLEMENTATION::fillRect( AImplementation, x, y, w, h, color );
		}

	public:
		// This MUST be defined by the subclass:
//		virtual void drawPixel(int16_t x, int16_t y, TUniColor color) = 0;
//		virtual TUniColor	GetBackgroundColor() = 0;

	public:
		void setFont( const unsigned char *AFont )
		{
			FFont = AFont;
		}

		const unsigned char *getFont()
		{
			return FFont;
		}

		void getTextColor( TUniColor &AColor, TUniColor &ABackgroundColor )
		{
			AColor = textcolor;
			ABackgroundColor = textbgcolor; 
		}

		static void drawEllipse( T_IMPLEMENTATION &AImplementation, int16_t xc, int16_t yc, int16_t rx, int16_t ry, TUniColor color, bool ACorrectX, bool ACorrectY ) 
		{
			if( ACorrectY )
			{
				++yc;
				++ry;
			}

			uint32_t hh = ry * ry;
			uint32_t ww = rx * rx;
			uint32_t hhww = hh * ww;
			TGraphicsPos x0 = rx;
			TGraphicsPos dx = 0;
			TGraphicsPos ALastX = rx;

			if( ! ACorrectY )
			{
				AImplementation.drawPixel( xc - rx, yc, color );
				if( ACorrectX )
					AImplementation.drawPixel( xc + rx + 1, yc, color );

				else
					AImplementation.drawPixel( xc + rx, yc, color );

			}

			for (int y = 1; y <= ry; y++)
			{
				uint32_t x1 = x0 - (dx - 1);  // try slopes of dx - 1 or more

				for ( ; x1 > 0; x1--)
					if (x1*x1*hh + y*y*ww <= hhww)
						break;

				dx = x0 - x1;  // current approximation of the slope
				x0 = x1;

				int ay1 = ( ACorrectY ) ? y - 1 : y;
				int aX0R = ( ACorrectX ) ? x0 - 1 : x0;

				if( ALastX - x0 )
				{
					T_IMPLEMENTATION::drawFastHLine( AImplementation, xc - ALastX, yc - y, ALastX - x0, color );
					T_IMPLEMENTATION::drawFastHLine( AImplementation, xc - ALastX, yc + ay1, ALastX - x0, color );

					T_IMPLEMENTATION::drawFastHLine( AImplementation, xc + x0, yc - y, ALastX - x0, color );
					T_IMPLEMENTATION::drawFastHLine( AImplementation, xc + x0, yc + ay1, ALastX - x0, color );
				}

				else
				{
					AImplementation.drawPixel( xc - x0, yc - y, color );
					AImplementation.drawPixel( xc - x0, yc + ay1, color );

					AImplementation.drawPixel( xc + aX0R, yc - y, color );
					AImplementation.drawPixel( xc + aX0R, yc + ay1, color );
				}

				ALastX = x0;
			}
		}

		static void drawEllipseWH( T_IMPLEMENTATION &AImplementation, int16_t x, int16_t y, int16_t w, int16_t h, TUniColor color ) 
		{
			T_IMPLEMENTATION::drawEllipse( AImplementation, x + w / 2, y + h / 2, w / 2, h / 2, color, !( w & 1 ), !( h & 1 ) );
		}

		static void drawEllipseHelper( T_IMPLEMENTATION &AImplementation, int16_t xc, int16_t yc, int16_t rx, int16_t ry, uint8_t ACorners, TUniColor color ) //, bool ACorrectX = false, bool ACorrectY = false ) 
		{
//			if( ACorrectY )
//				++ry;

			uint32_t hh = ry * ry;
			uint32_t ww = rx * rx;
			uint32_t hhww = hh * ww;
			TGraphicsPos x0 = rx;
			TGraphicsPos dx = 0;
			TGraphicsPos ALastX = rx;

/*
			if( ! ACorrectY )
			{
				drawPixel( xc - rx, yc, color );
				if( ACorrectX )
					drawPixel( xc + rx + 1, yc, color );

				else
					drawPixel( xc + rx, yc, color );

			}
*/
			for (int y = 1; y <= ry; y++)
			{
				uint32_t x1 = x0 - (dx - 1);  // try slopes of dx - 1 or more

				for ( ; x1 > 0; x1--)
					if (x1*x1*hh + y*y*ww <= hhww)
						break;

				dx = x0 - x1;  // current approximation of the slope
				x0 = x1;

//				int ay1 = ( ACorrectY ) ? y - 1 : y;
//				int aX0R = ( ACorrectX ) ? x0 - 1 : x0;

				if( ALastX - x0 )
				{
					if( ACorners & 1 )
						T_IMPLEMENTATION::drawFastHLine( AImplementation, xc - ALastX, yc - y, ALastX - x0, color );

					if( ACorners & 8 )
						T_IMPLEMENTATION::drawFastHLine( AImplementation, xc - ALastX, yc + y, ALastX - x0, color );

					if( ACorners & 2 )
						T_IMPLEMENTATION::drawFastHLine( AImplementation, xc + x0, yc - y, ALastX - x0, color );

					if( ACorners & 4 )
						T_IMPLEMENTATION::drawFastHLine( AImplementation, xc + x0, yc + y, ALastX - x0, color );
				}

				else
				{
					if( ACorners & 1 )
						AImplementation.drawPixel( xc - x0, yc - y, color );

					if( ACorners & 8 )
						AImplementation.drawPixel( xc - x0, yc + y, color );

					if( ACorners & 2 )
						AImplementation.drawPixel( xc + x0, yc - y, color );

					if( ACorners & 4 )
						AImplementation.drawPixel( xc + x0, yc + y, color );
				}

				ALastX = x0;
			}
		}

		static void fillEllipse( T_IMPLEMENTATION &AImplementation, int16_t ax, int16_t ay, int16_t rx, int16_t ry, TUniColor color, bool ACorrectX, bool ACorrectY ) 
		{
//			Serial.print( "fillEllipse: " ); Serial.print( ax ); Serial.print( "x" ); Serial.println( ay );
			if( ACorrectY )
			{
				++ay;
				++ry;
			}

			uint32_t hh = ry * ry;
			uint32_t ww = rx * rx;
			uint32_t hhww = hh * ww;
			TGraphicsPos x0 = rx;
			TGraphicsPos dx = 0;

			if( ! ACorrectY )
			{
				if( ACorrectX )
					T_IMPLEMENTATION::drawFastHLine( AImplementation, ax - rx, ay, rx * 2 + 1, color );

				else
					T_IMPLEMENTATION::drawFastHLine( AImplementation, ax - rx, ay, rx * 2, color );

			}

			for (int y = 1; y <= ry; y++)
			{
				uint32_t x1 = x0 - (dx - 1);  // try slopes of dx - 1 or more

				for ( ; x1 > 0; x1--)
					if (x1*x1*hh + y*y*ww <= hhww)
						break;

				dx = x0 - x1;  // current approximation of the slope
				x0 = x1;

				int ay1 = ( ACorrectY ) ? y - 1 : y;
				int aWidth = ( ACorrectX ) ? x0 * 2 + 1 : x0 * 2;

				T_IMPLEMENTATION::drawFastHLine( AImplementation, ax - x0, ay - y, aWidth, color );
				T_IMPLEMENTATION::drawFastHLine( AImplementation, ax - x0, ay + ay1, aWidth, color );
			}
		}

		static void fillEllipseWH( T_IMPLEMENTATION &AImplementation, int16_t x, int16_t y, int16_t w, int16_t h, TUniColor color ) 
		{
			T_IMPLEMENTATION::fillEllipse( AImplementation, x + w / 2, y + h / 2, w / 2, h / 2, color, !( w & 1 ), !( h & 1 ) );
		}

		static void fillEllipseHelper( T_IMPLEMENTATION &AImplementation, int16_t xc, int16_t yc, int16_t rx, int16_t ry, uint8_t ACorners, TUniColor color ) 
		{
//			if( ACorrectY )
//				++ry;

			uint32_t hh = ry * ry;
			uint32_t ww = rx * rx;
			uint32_t hhww = hh * ww;
			TGraphicsPos x0 = rx;
			TGraphicsPos dx = 0;

/*
			if( ! ACorrectY )
			{
				if( ACorrectX )
					drawFastHLine( ax - rx, ay, rx * 2 + 1, color );

				else
					drawFastHLine( ax - rx, ay, rx * 2, color );

			}
*/
			for (int y = 1; y <= ry; y++)
			{
				uint32_t x1 = x0 - (dx - 1);  // try slopes of dx - 1 or more

				for ( ; x1 > 0; x1--)
					if (x1*x1*hh + y*y*ww <= hhww)
						break;

				dx = x0 - x1;  // current approximation of the slope
				x0 = x1;

//				int ay1 = ( ACorrectY ) ? y - 1 : y;
//				int aWidth = ( ACorrectX ) ? x0 * 2 + 1 : x0 * 2;

				if( ACorners & 1 )
					T_IMPLEMENTATION::drawFastHLine( AImplementation, xc - x0, yc - y, x0, color );

				if( ACorners & 8 )
					T_IMPLEMENTATION::drawFastHLine( AImplementation, xc - x0, yc + y, x0, color );

				if( ACorners & 2 )
					T_IMPLEMENTATION::drawFastHLine( AImplementation, xc, yc - y, x0, color );

				if( ACorners & 4 )
					T_IMPLEMENTATION::drawFastHLine( AImplementation, xc, yc + y, x0, color );

//				drawFastHLine( ax - x0, ay - y, aWidth, color );
//				drawFastHLine( ax - x0, ay + ay1, aWidth, color );

			}
		}

		static inline void ClearScreen( T_IMPLEMENTATION &AImplementation, TUniColor color )
		{
			T_IMPLEMENTATION::fillRect( AImplementation, 0, 0, AImplementation.width(), AImplementation.height(), color );
		}

		inline bool getTextWrap()
		{
			return wrap;
		}

		static void drawRect( T_IMPLEMENTATION &AImplementation, int16_t x, int16_t y, int16_t w, int16_t h, TUniColor color ) 
		{
			T_IMPLEMENTATION::drawFastHLine( AImplementation, x, y, w, color);
			T_IMPLEMENTATION::drawFastHLine( AImplementation, x, y+h-1, w, color);
			T_IMPLEMENTATION::drawFastVLine( AImplementation, x, y, h, color);
			T_IMPLEMENTATION::drawFastVLine( AImplementation, x+w-1, y, h, color);
		}

		static void fillRect( T_IMPLEMENTATION &AImplementation, int16_t x, int16_t y, int16_t w, int16_t h, TUniColor color) 
		{
//			Serial.println( "fillRect" );
			// Update in subclasses if desired!
			for (int16_t i=x; i<x+w; i++)
				T_IMPLEMENTATION::drawFastVLine( AImplementation, i, y, h, color );

		}

		static inline void fillScreen( T_IMPLEMENTATION &AImplementation, TUniColor color ) 
		{
			T_IMPLEMENTATION::fillRect( AImplementation, 0, 0, AImplementation.width(), AImplementation.height(), color);
		}

//#ifdef _GRAPHICS_DRAW_GRAYSCALE_BITMAP_

		static void drawGrayscaleBitmap( T_IMPLEMENTATION &AImplementation, int16_t x, int16_t y, const uint8_t *bitmap, int16_t w, int16_t h, uint8_t AAlpha, uint8_t size ) 
		{
			TUniColor AAlpha16 = uint16_t( AAlpha ) << 8;
  
			if (size == 1) // default size
			{
				for( int16_t j=0; j<h; j++)
					for( int16_t i=0; i<w; ++i )
						T_IMPLEMENTATION::drawGrayscalePixel( AImplementation, x+i, y+j, AAlpha16 | pgm_read_byte( bitmap ++ ));

			}

			else
			{
				for( int16_t j=0; j<h; j++)
					for( int16_t i=0; i<w; ++i )
						T_IMPLEMENTATION::fillGrayscaleRect( AImplementation, x+i * size, y+j * size, size, size, AAlpha16 | pgm_read_byte( bitmap ++ ));

			}
		}

//#endif // _GRAPHICS_DRAW_GRAYSCALE_BITMAP_

//#ifdef _GRAPHICS_DRAW_GRAYSCALE_REPEAT_BITMAP_

		static void drawRepeatGrayscaleBitmap( T_IMPLEMENTATION &AImplementation, int16_t x, int16_t y, const uint8_t *bitmap, int16_t AListCount, int16_t w, int16_t h, uint8_t AAlpha, uint8_t size ) 
		{
			TUniColor AAlpha16 = uint16_t( AAlpha ) << 8;
  
			int16_t x1 = 0;
			for( int16_t i = 0; i < AListCount; ++i )
			{
				uint8_t ACount = pgm_read_byte( bitmap ++ );
				uint32_t AGrayscale = pgm_read_byte( bitmap ++ );

				uint32_t AColor = AAlpha16 | AGrayscale;

				if (size == 1) // default size
				{
					for( int16_t j = 0; j <= ACount; ++ j )
					{
						T_IMPLEMENTATION::drawGrayscalePixel( AImplementation, x + x1++, y, AColor );

						if( x1 >= w )
						{
							x1 = 0;
							++ y;
						}					
					}
				}

				else
				{
					for( int16_t j = 0; j <= ACount; ++ j )
					{
						T_IMPLEMENTATION::fillGrayscaleRect( AImplementation, x + x1 * size, y, size, size, AColor );
						++ x1;

						if( x1 >= w )
						{
							x1 = 0;
							y += size;
						}					
					}
				}
			}
		}

//#endif // _GRAPHICS_DRAW_GRAYSCALE_REPEAT_BITMAP_

//#ifdef _GRAPHICS_DRAW_GRAYSCALE_REPEAT_ALPHA_BITMAP_

		static void drawAlphaGrayscaleRepeatBitmap( T_IMPLEMENTATION &AImplementation, int16_t x, int16_t y, const uint8_t *bitmap, int16_t AListCount, int16_t w, int16_t h, uint8_t size ) 
		{
			int16_t x1 = 0;
			for( int16_t i = 0; i < AListCount; ++i )
			{
				uint8_t ACount = pgm_read_byte( bitmap ++ );
				uint16_t AGrayscale = pgm_read_byte( bitmap ++ );
				uint16_t AColor = ( AGrayscale << 8 ) | pgm_read_byte( bitmap ++ );

				if (size == 1) // default size
				{
					for( int16_t j = 0; j <= ACount; ++ j )
					{
						T_IMPLEMENTATION::drawGrayscalePixel( AImplementation, x + x1++, y, AColor );

						if( x1 >= w )
						{
							x1 = 0;
							++ y;
						}					
					}
				}

				else
				{
					for( int16_t j = 0; j <= ACount; ++ j )
					{
						T_IMPLEMENTATION::fillGrayscaleRect( AImplementation, x + x1 * size, y, size, size, AColor );
						++ x1;

						if( x1 >= w )
						{
							x1 = 0;
							y += size;
						}					
					}
				}
			}
		}

//#endif // _GRAPHICS_DRAW_GRAYSCALE_REPEAT_ALPHA_BITMAP_

//#ifdef _GRAPHICS_DRAW_ALPHA_COLOR_BITMAP_

		static void drawAlphaColorBitmap( T_IMPLEMENTATION &AImplementation, int16_t x, int16_t y, const uint32_t *bitmap, int16_t w, int16_t h, uint8_t size ) 
		{
			if (size == 1) // default size
			{
				for( int16_t j=0; j<h; j++)
					for( int16_t i=0; i<w; ++i )
						AImplementation.drawPixel( x+i, y+j, pgm_read_dword( bitmap ++ ));
			}

			else
			{
				for( int16_t j=0; j<h; j++)
					for( int16_t i=0; i<w; ++i )
						T_IMPLEMENTATION::fillRect( AImplementation, x+i*size, y+j*size, size, size, pgm_read_dword( bitmap ++ ));
			}

		}

//#endif // _GRAPHICS_DRAW_ALPHA_COLOR_BITMAP_

//#ifdef _GRAPHICS_DRAW_ALPHA_RGBW_COLOR_BITMAP_

		static void drawAlphaRGBWColorBitmap( T_IMPLEMENTATION &AImplementation, int16_t x, int16_t y, const uint8_t *bitmap, int16_t w, int16_t h, uint8_t size ) 
		{
			for( int16_t j=0; j<h; j++)
				for( int16_t i=0; i<w; ++i )
				{
					uint32_t ARed = pgm_read_byte( bitmap ++ );
					uint16_t AGreen = pgm_read_byte( bitmap ++ );
					uint8_t ABlue = pgm_read_byte( bitmap ++ );
					uint32_t AWhite = pgm_read_byte( bitmap ++ );
					uint64_t AAlpha = pgm_read_byte( bitmap ++ );
					uint64_t AColor = ( AAlpha << 32 ) | (AWhite << 24) | ( ARed << 16 ) | ( AGreen << 8 ) | ABlue;

					if (size == 1) // default size
						AImplementation.drawPixel( x+i, y+j, AColor );

					else
						T_IMPLEMENTATION::fillRect( AImplementation, x+i*size, y+j*size, size, size, AColor );
				}

		}

//#endif // _GRAPHICS_DRAW_ALPHA_COLOR_BITMAP_

//#ifdef _GRAPHICS_DRAW_COLOR_BITMAP_

		static void drawColorBitmap( T_IMPLEMENTATION &AImplementation, int16_t x, int16_t y, const uint8_t *bitmap, int16_t w, int16_t h, uint8_t size ) 
		{
			for( int16_t j=0; j<h; j++)
				for( int16_t i=0; i<w; ++i )
				{
					uint32_t ARed = pgm_read_byte( bitmap ++ );
					uint32_t AGreen = pgm_read_byte( bitmap ++ );
					uint32_t ABlue = pgm_read_byte( bitmap ++ );
					uint32_t AColor = 0xFF000000 | ( ARed << 16 ) | ( AGreen << 8 ) | ABlue;

					if (size == 1) // default size
						T_IMPLEMENTATION::drawColorPixel( AImplementation, x+i, y+j, AColor );

					else
						T_IMPLEMENTATION::fillColorRect( AImplementation, x+i*size, y+j*size, size, size, AColor );
				}

		}

//#endif // _GRAPHICS_DRAW_COLOR_BITMAP_

//#ifdef _GRAPHICS_DRAW_RGBW_COLOR_BITMAP_

		static void drawRGBWColorBitmap( T_IMPLEMENTATION &AImplementation, int16_t x, int16_t y, const uint8_t *bitmap, int16_t w, int16_t h, uint8_t size ) 
		{
			for( int16_t j=0; j<h; j++)
				for( int16_t i=0; i<w; ++i )
				{
					uint32_t ARed = pgm_read_byte( bitmap ++ );
					uint16_t AGreen = pgm_read_byte( bitmap ++ );
					uint8_t ABlue = pgm_read_byte( bitmap ++ );
					uint64_t AWhite = pgm_read_byte( bitmap ++ );
					uint64_t AColor = 0xFF00000000 | ( AWhite << 32 ) | ( ARed << 16 ) | ( AGreen << 8 ) | ABlue;

					if (size == 1) // default size
						AImplementation.drawPixel( x+i, y+j, AColor );

					else
						T_IMPLEMENTATION::fillRect( AImplementation, x+i*size, y+j*size, size, size, AColor );
				}

		}

//#endif // _GRAPHICS_DRAW_RGBW_COLOR_BITMAP_

//#ifdef _GRAPHICS_DRAW_REPEAT_COLOR_BITMAP_

		static void drawRepeatColorBitmap( T_IMPLEMENTATION &AImplementation, int16_t x, int16_t y, const uint8_t *bitmap, int16_t AListCount, int16_t w, int16_t h, uint8_t size ) 
		{
			int16_t x1 = 0;
			for( int16_t i = 0; i < AListCount; ++i )
			{
				uint8_t ACount = pgm_read_byte( bitmap ++ );

				uint32_t ARed = pgm_read_byte( bitmap ++ );
				uint32_t AGreen = pgm_read_byte( bitmap ++ );
				uint32_t ABlue = pgm_read_byte( bitmap ++ );
				uint32_t AColor = 0xFF000000 | ( ARed << 16 ) | ( AGreen << 8 ) | ABlue;

				if (size == 1) // default size
				{
					for( int16_t j = 0; j <= ACount; ++ j )
					{
						T_IMPLEMENTATION::drawColorPixel( AImplementation, x + x1++, y, AColor );

						if( x1 >= w )
						{
							x1 = 0;
							++ y;
						}					
					}
				}

				else
				{
					for( int16_t j = 0; j <= ACount; ++ j )
					{
						T_IMPLEMENTATION::fillColorRect( AImplementation, x + x1 * size, y, size, size, AColor );
						++ x1;

						if( x1 >= w )
						{
							x1 = 0;
							y += size;
						}					
					}
				}
			}
		}

//#endif // _GRAPHICS_DRAW_REPEAT_COLOR_BITMAP_

//#ifdef _GRAPHICS_DRAW_REPEAT_RGBW_COLOR_BITMAP_

		static void drawRepeatRGBWColorBitmap( T_IMPLEMENTATION &AImplementation, int16_t x, int16_t y, const uint8_t *bitmap, int16_t AListCount, int16_t w, int16_t h, uint8_t size ) 
		{
			int16_t x1 = 0;
			for( int16_t i = 0; i < AListCount; ++i )
			{
				uint8_t ACount = pgm_read_byte( bitmap ++ );

				uint32_t ARed = pgm_read_byte( bitmap ++ );
				uint16_t AGreen = pgm_read_byte( bitmap ++ );
				uint8_t ABlue = pgm_read_byte( bitmap ++ );
				uint64_t AWhite = pgm_read_byte( bitmap ++ );
				uint64_t AColor = 0xFF00000000 | ( AWhite << 24 ) | ( ARed << 16 ) | ( AGreen << 8 ) | ABlue;

				if (size == 1) // default size
				{
					for( int16_t j = 0; j <= ACount; ++ j )
					{
						AImplementation.drawPixel( x + x1++, y, AColor );

						if( x1 >= w )
						{
							x1 = 0;
							++ y;
						}					
					}
				}

				else
				{
					for( int16_t j = 0; j <= ACount; ++ j )
					{
						T_IMPLEMENTATION::fillRect( AImplementation, x + x1 * size, y, size, size, AColor );
						++ x1;

						if( x1 >= w )
						{
							x1 = 0;
							y += size;
						}					
					}
				}
			}
		}

//#endif // _GRAPHICS_DRAW_REPEAT_RGBW_COLOR_BITMAP_

//#ifdef _GRAPHICS_DRAW_REPEAT_ALPHA_COLOR_BITMAP_

		static void drawRepeatAlphaColorBitmap( T_IMPLEMENTATION &AImplementation, int16_t x, int16_t y, const uint8_t *bitmap, int16_t AListCount, int16_t w, int16_t h, uint8_t size ) 
		{
			int16_t x1 = 0;
			for( int16_t i = 0; i < AListCount; ++i )
			{
				uint8_t ACount = pgm_read_byte( bitmap ++ );
				uint32_t AColor = pgm_read_dword( bitmap );
				bitmap += 4;
				if (size == 1) // default size
				{
					for( int16_t j = 0; j <= ACount; ++ j )
					{
						T_IMPLEMENTATION::drawColorPixel( AImplementation, x + x1++, y, AColor );

						if( x1 >= w )
						{
							x1 = 0;
							++ y;
						}					
					}
				}

				else
				{
					for( int16_t j = 0; j <= ACount; ++ j )
					{
						T_IMPLEMENTATION::fillColorRect( AImplementation, x + x1 * size, y, size, size, AColor );
						++ x1;

						if( x1 >= w )
						{
							x1 = 0;
							y += size;
						}					
					}
				}
			}
		}

//#endif // _GRAPHICS_DRAW_REPEAT_ALPHA_COLOR_BITMAP_

//#ifdef _GRAPHICS_DRAW_REPEAT_ALPHA_RGBW_COLOR_BITMAP_

		static void drawRepeatAlphaRGBWColorBitmap( T_IMPLEMENTATION &AImplementation, int16_t x, int16_t y, const uint8_t *bitmap, int16_t AListCount, int16_t w, int16_t h, uint8_t size ) 
		{
			int16_t x1 = 0;
			for( int16_t i = 0; i < AListCount; ++i )
			{
				uint8_t ACount = pgm_read_byte( bitmap ++ );

				uint32_t ARed = pgm_read_byte( bitmap ++ );
				uint16_t AGreen = pgm_read_byte( bitmap ++ );
				uint8_t ABlue = pgm_read_byte( bitmap ++ );
				uint32_t AWhite = pgm_read_byte( bitmap ++ );
				uint64_t AAlpha = pgm_read_byte( bitmap ++ );

				uint64_t AColor = ( AAlpha << 32 ) | (AWhite << 24) | ( ARed << 16 ) | ( AGreen << 8 ) | ABlue;
				if (size == 1) // default size
				{
					for( int16_t j = 0; j <= ACount; ++ j )
					{
						AImplementation.drawPixel( x + x1++, y, AColor );

						if( x1 >= w )
						{
							x1 = 0;
							++ y;
						}					
					}
				}

				else
				{
					for( int16_t j = 0; j <= ACount; ++ j )
					{
						T_IMPLEMENTATION::fillRect( AImplementation, x + x1 * size, y, size, size, AColor );
						++ x1;

						if( x1 >= w )
						{
							x1 = 0;
							y += size;
						}					
					}
				}
			}
		}

//#endif // _GRAPHICS_DRAW_REPEAT_ALPHA_RGBW_COLOR_BITMAP_

//#ifdef _GRAPHICS_DRAW_GRAYSCALE_ALPHA_BITMAP_

		static void drawAlphaGrayscaleBitmap( T_IMPLEMENTATION &AImplementation, int16_t x, int16_t y, const uint16_t *bitmap, int16_t w, int16_t h, uint8_t size ) 
		{
			if (size == 1) // default size
			{
				for( int16_t j=0; j<h; j++)
					for( int16_t i=0; i<w; ++i )
						T_IMPLEMENTATION::drawGrayscalePixel( AImplementation, x+i, y+j, pgm_read_word( bitmap ++ ));

			}

			else
			{
				for( int16_t j=0; j<h; j++)
					for( int16_t i=0; i<w; ++i )
						T_IMPLEMENTATION::fillGrayscaleRect( AImplementation, x+i * size, y+j * size, size, size, pgm_read_word( bitmap ++ ));

			}
		}

//#endif // _GRAPHICS_DRAW_GRAYSCALE_ALPHA_BITMAP_

//#ifdef _GRAPHICS_DRAW_BLACK_WHITE_BITMAP_

		// Draw a 1-bit color bitmap at the specified x, y position from the
		// provided bitmap buffer (must be PROGMEM memory) using color as the
		// foreground color and bg as the background color.
		static void drawBWBitmap( T_IMPLEMENTATION &AImplementation, int16_t x, int16_t y, const uint8_t *bitmap, int16_t w, int16_t h, TUniColor color, TUniColor bg, uint8_t size ) 
		{
			int16_t byteWidth = (w + 7) / 8;
  
			if (size == 1) // default size
			{
				for( int16_t j=0; j<h; ++j ) 
					for( int16_t i=0; i<w; ++i ) 
					{
						if(pgm_read_byte(bitmap + j * byteWidth + i / 8) & (128 >> ( i & 0b111 )))
							T_IMPLEMENTATION::drawGrayscalePixel( AImplementation, x+i, y+j, color );

						else 
      						T_IMPLEMENTATION::drawGrayscalePixel( AImplementation, x+i, y+j, bg );
					}
			}

			else
			{
				for( int16_t j=0; j<h; ++j ) 
					for( int16_t i=0; i<w; ++i ) 
					{
						if(pgm_read_byte(bitmap + j * byteWidth + i / 8) & (128 >> ( i & 0b111 )))
							T_IMPLEMENTATION::fillGrayscaleRect( AImplementation, x+i*size, y+j*size, size, size, color );

						else 
							T_IMPLEMENTATION::fillGrayscaleRect( AImplementation, x+i*size, y+j*size, size, size, bg );
					}

			}

		}

//#endif // _GRAPHICS_DRAW_BLACK_WHITE_BITMAP_

		static void drawTransparencyBitmap( T_IMPLEMENTATION &AImplementation, int16_t x, int16_t y, const uint8_t *bitmap, int16_t w, int16_t h, uint8_t size )
		{
			int16_t byteWidth = (w + 3) / 4;
  
			for( int16_t j=0; j<h; ++j ) 
				for( int16_t i=0; i<w; ++i ) 
				{
					uint8_t AByte = ( pgm_read_byte(bitmap + j * byteWidth + i / 4) >> ((3 - (i & 0b11)) * 2 )) & 0b11;

				if (size == 1) // default size
					switch( AByte )
					{
						case 1 : AImplementation.drawPixel( x+i, y+j, tmcInvert ); break;
						case 2 : AImplementation.drawPixel( x+i, y+j, tmcBlack ); break;
						case 3 : AImplementation.drawPixel( x+i, y+j, tmcWhite ); break;
					}

				else  // big size
					switch( AByte )
					{
						case 1 : T_IMPLEMENTATION::fillRect( AImplementation, x+i*size, y+j*size, size, size, tmcInvert ); break;
						case 2 : T_IMPLEMENTATION::fillRect( AImplementation, x+i*size, y+j*size, size, size, tmcBlack ); break;
						case 3 : T_IMPLEMENTATION::fillRect( AImplementation, x+i*size, y+j*size, size, size, tmcWhite ); break;
					}
				}

		}

		static void drawRoundRect( T_IMPLEMENTATION &AImplementation, int16_t x, int16_t y, int16_t w, int16_t h, int16_t rx, int16_t ry, TUniColor color )
		{
			if( rx == 0 || ry == 0 )
			{
				T_IMPLEMENTATION::drawRect( AImplementation, x, y, w, h, color );
				return;
			}

			T_IMPLEMENTATION::drawFastHLine( AImplementation, x+rx , y    , w-2 * rx - 1, color ); // Top
			T_IMPLEMENTATION::drawFastHLine( AImplementation, x+rx , y+h-1, w-2 * rx - 1, color ); // Bottom
			T_IMPLEMENTATION::drawFastVLine( AImplementation, x    , y+ry , h-2 * ry, color ); // Left
			T_IMPLEMENTATION::drawFastVLine( AImplementation, x+w-1, y+ry , h-2 * ry, color ); // Right

			T_IMPLEMENTATION::drawEllipseHelper( AImplementation, x+rx,		y+ry,	  rx, ry, 1, color);
			T_IMPLEMENTATION::drawEllipseHelper( AImplementation, x+w-rx-1,	y+ry,	  rx, ry, 2, color);
			T_IMPLEMENTATION::drawEllipseHelper( AImplementation, x+w-rx-1,	y+h-ry-1, rx, ry, 4, color);
			T_IMPLEMENTATION::drawEllipseHelper( AImplementation, x+rx,		y+h-ry-1, rx, ry, 8, color);
		}

		static void fillRoundRect( T_IMPLEMENTATION &AImplementation, int16_t x, int16_t y, int16_t w, int16_t h, int16_t rx, int16_t ry, TUniColor color )
		{
			if( rx == 0 || ry == 0 )
			{
				T_IMPLEMENTATION::fillRect( AImplementation, x, y, w, h, color );
				return;
			}

			T_IMPLEMENTATION::fillRect( AImplementation, x+rx, y, w-rx*2 - 1, ry, color );
			T_IMPLEMENTATION::fillRect( AImplementation, x+rx, y+h-ry, w-rx*2 - 1, ry, color );
			T_IMPLEMENTATION::fillRect( AImplementation, x, y+ry, w, h-ry * 2, color );
			T_IMPLEMENTATION::fillEllipseHelper( AImplementation, x+rx,		y+ry,	  rx, ry, 1, color);
			T_IMPLEMENTATION::fillEllipseHelper( AImplementation, x+w-rx-1,	y+ry,	  rx, ry, 2, color);
			T_IMPLEMENTATION::fillEllipseHelper( AImplementation, x+w-rx-1,	y+h-ry-1, rx, ry, 4, color);
			T_IMPLEMENTATION::fillEllipseHelper( AImplementation, x+rx,		y+h-ry-1, rx, ry, 8, color);
		}

		// Bresenham's algorithm - thx wikpedia
		static void drawLine( T_IMPLEMENTATION &AImplementation, int16_t x0, int16_t y0, int16_t x1, int16_t y1, TUniColor color) 
		{
//			Serial.println( "drawLine" );
			int16_t steep = abs(y1 - y0) > abs(x1 - x0);
			if (steep) 
			{
				swap(x0, y0);
				swap(x1, y1);
			}

			if (x0 > x1) 
			{
				swap(x0, x1);
				swap(y0, y1);
			}

			int16_t dx, dy;
			dx = x1 - x0;
			dy = abs(y1 - y0);

			int16_t err = dx / 2;
			int16_t ystep;

			if (y0 < y1) 
				ystep = 1;

			else
				ystep = -1;

			for (; x0<=x1; x0++) 
			{
				if (steep)
					AImplementation.drawPixel(y0, x0, color);

				else
					AImplementation.drawPixel(x0, y0, color);
			
				err -= dy;
				if (err < 0) 
				{
					y0 += ystep;
					err += dx;
				}
			}
		}

		static inline void drawFastVLine( T_IMPLEMENTATION &AImplementation, int16_t x, int16_t y, int16_t h, TUniColor color) 
		{
//			Serial.println( "drawFastVLine" );
			// Update in subclasses if desired!
			T_IMPLEMENTATION::drawLine( AImplementation, x, y, x, y+h-1, color);
		}

		static inline void drawFastHLine( T_IMPLEMENTATION &AImplementation, int16_t x, int16_t y, int16_t w, TUniColor color) 
		{
			// Update in subclasses if desired!
			T_IMPLEMENTATION::drawLine( AImplementation, x, y, x+w-1, y, color );
		}

		static size_t write( T_IMPLEMENTATION &AImplementation, uint8_t c )
		{
			AImplementation.height();
//			return 0;
//			Serial.println( c );
			if(( c != '\n' ) && ( c != '\r' ))
				if( AImplementation.cursor_y >= AImplementation.height() )
				{
//					Serial.println( "Scroll" );
					int ACorrection = ( AImplementation.cursor_y - AImplementation.height() ) + AImplementation.textsize * 8;
					T_IMPLEMENTATION::Scroll( AImplementation, 0, -ACorrection, AImplementation.GetBackgroundColor() );
					AImplementation.cursor_y -= ACorrection;
				}

			if (c == '\n') 
			{
				AImplementation.cursor_y += AImplementation.textsize * 8;
				AImplementation.cursor_x  = 0;
			}

			else if (c == '\r') 
				AImplementation.cursor_x  = 0;

			else 
			{
//				Serial.println( c );
//				Serial.println( AImplementation.cursor_x );
				T_IMPLEMENTATION::drawChar( AImplementation, AImplementation.cursor_x, AImplementation.cursor_y, c, AImplementation.textcolor, AImplementation.textbgcolor, AImplementation.textsize );
				AImplementation.cursor_x += AImplementation.textsize * 6;
				if ( AImplementation.wrap && ( AImplementation.cursor_x > ( AImplementation.width() - AImplementation.textsize * 6))) 
				{
					AImplementation.cursor_y += AImplementation.textsize * 8;
					AImplementation.cursor_x = 0;
				}
			}

			return 1;
		}

		// Draw a character
		static void drawChar( T_IMPLEMENTATION &AImplementation, int16_t x, int16_t y, unsigned char c, TUniColor color, TUniColor bg, uint8_t size )
		{
//		  Serial.println( "drawChar" );
//		  Serial.println( AImplementation.width() );
//		  Serial.println( x );
//		  Serial.println( y );
		  if((x >= AImplementation.width() )            || // Clip right
			 (y >= AImplementation.height() )           || // Clip bottom
			 ((x + 6 * size - 1) < 0) || // Clip left
			 ((y + 8 * size - 1) < 0))   // Clip top
			return;

//		  Serial.println( c );

		  if(! AImplementation.GetCp437() && (c >= 176)) 
			  c++; // Handle 'classic' charset behavior

		  for (int8_t i=0; i<6; i++ ) 
		  {
			uint8_t line;
			if (i == 5) 
			  line = 0x0;

			else 
			  line = pgm_read_byte( AImplementation.FFont + 3 + (c*5)+i); // For now ifnore size information

			for (int8_t j = 0; j<8; j++) 
			{
			  if (line & 0x1) 
			  {
				if (size == 1) // default size
				  AImplementation.drawPixel(x+i, y+j, color);

				else  // big size
				  T_IMPLEMENTATION::fillRect( AImplementation, x+(i*size), y+(j*size), size, size, color );
				 
			  }

			  else if (bg != color) 
			  {
				if (size == 1) // default size
				  AImplementation.drawPixel(x+i, y+j, bg);

				else  // big size
				  T_IMPLEMENTATION::fillRect( AImplementation, x+i*size, y+j*size, size, size, bg );
				
			  }

			  line >>= 1;
			}
		  }
		}

		void setCursor(int16_t x, int16_t y) 
		{
//			Serial.println( "setCursor" );
//			Serial.println( x );
			cursor_x = x;
			cursor_y = y;
		}

		void getCursor( int16_t &x, int16_t &y ) const 
		{
			x = cursor_x;
			y = cursor_y;
		}

		inline void setTextSize(uint8_t s) 
		{
			textsize = (s > 0) ? s : 1;
		}

		inline uint8_t getTextSize()
		{
			return textsize;
		}

		void setTextColor( TUniColor c, TUniColor b) 
		{
			textcolor   = c;
			textbgcolor = b; 
		}

		inline void setTextWrap(bool w) 
		{
			wrap = w;
		}

		inline void SetCp437(bool x) 
		{
			_cp437 = x;
		}

		inline bool GetCp437()
		{
			return _cp437;
		}

	public:
		// Return the size of the display (per current rotation)
//		virtual int16_t width(void) const = 0;
//		virtual int16_t height(void) const = 0;

	public:
		Graphics( const unsigned char * AFont ) :
			FFont( AFont ),
			wrap( true ),
			_cp437( false )
		{
		}
	};
//---------------------------------------------------------------------------
	template<typename T_OWNER, T_OWNER &C_OWNER> class GraphicsTextElementFont
	{
	protected:
		const unsigned char *FFont;

	protected:
		const unsigned char *FOldFont;

	public:
		void EnterLeave( bool AEnter )
		{
			if( AEnter )
			{
				FOldFont = C_OWNER.getFont();
				C_OWNER.setFont( FFont );
			}

			else
			{
				C_OWNER.setFont( FOldFont );
			}
		}

	public:
		GraphicsTextElementFont( const unsigned char *AFont ) :
			FFont( AFont )
		{
		}

	};
//---------------------------------------------------------------------------
	template<typename T_DELEGATOR, typename T_OWNER, T_OWNER &C_OWNER> class GraphicsDelegate // : public GraphicsIntf<T_OWNER>
	{
	public:
		template<typename T> static inline void PrintChar( T AValue )
		{
			C_OWNER.PrintChar( AValue );
		}

	public:
		template<typename T> static inline void Print( T AValue )
		{
			C_OWNER.Print( AValue );
		}

	public:
		template<typename T> static inline size_t print( T AValue )
		{
			return C_OWNER.print( AValue );
		}

		template<typename T> static inline size_t println( T AValue )
		{
			return C_OWNER.println( AValue );
		}

	public:
		inline static void Scroll( T_DELEGATOR &AImplementation, TGraphicsPos X, TGraphicsPos Y, TUniColor color )
		{
			T_OWNER::Scroll( C_OWNER, X, Y, color );
		}

		inline static void drawGrayscalePixel( T_DELEGATOR &AImplementation, int16_t x, int16_t y, TUniColor color )
		{
			T_OWNER::drawGrayscalePixel( C_OWNER, x, y, color );
		}

		inline static void drawColorPixel( T_DELEGATOR &AImplementation, int16_t x, int16_t y, TUniColor color )
		{
			T_OWNER::drawColorPixel( C_OWNER, x, y, color );
		}

		inline static void fillGrayscaleRect( T_DELEGATOR &AImplementation, int16_t x, int16_t y, int16_t w, int16_t h, TUniColor color )
		{
			T_OWNER::fillGrayscaleRect( C_OWNER, x, y, w, h, color );
		}

		inline static void fillColorRect( T_DELEGATOR &AImplementation, int16_t x, int16_t y, int16_t w, int16_t h, TUniColor color )
		{
			T_OWNER::fillColorRect( C_OWNER, x, y, w, h, color );
		}

	public:
		inline static void drawPixel( int16_t x, int16_t y, TUniColor color )
		{
			C_OWNER.drawPixel( x, y, color );
		}

		inline static TUniColor GetBackgroundColor()
		{
			return C_OWNER.GetBackgroundColor();
		}

	public:
		inline static void setFont( const unsigned char *AFont )
		{
			C_OWNER.setFont( AFont );
		}

		inline const unsigned char *getFont()
		{
			return C_OWNER.getFont();
		}

		inline static void getTextColor( TUniColor &AColor, TUniColor &ABackgroundColor )
		{
			C_OWNER.getTextColor( AColor, ABackgroundColor );
		}

		inline static void drawEllipse( T_DELEGATOR &AImplementation, int16_t xc, int16_t yc, int16_t rx, int16_t ry, TUniColor color, bool ACorrectX, bool ACorrectY ) 
		{
			T_OWNER::drawEllipse( C_OWNER, xc, yc, rx, ry, color, ACorrectX, ACorrectY );
		}

		inline static void drawEllipseWH( T_DELEGATOR &AImplementation, int16_t x, int16_t y, int16_t w, int16_t h, TUniColor color ) 
		{
			T_OWNER::drawEllipseWH( C_OWNER, x, y, w, h, color );
		}

		inline static void fillEllipse( T_DELEGATOR &AImplementation, int16_t ax, int16_t ay, int16_t rx, int16_t ry, TUniColor color, bool ACorrectX, bool ACorrectY ) 
		{
			T_OWNER::fillEllipse( C_OWNER, ax, ay, rx, ry, color, ACorrectX, ACorrectY );
		}

		inline static void fillEllipseWH( T_DELEGATOR &AImplementation, int16_t x, int16_t y, int16_t w, int16_t h, TUniColor color ) 
		{
			T_OWNER::fillEllipseWH( C_OWNER, x, y, w, h, color );
		}

		inline static void ClearScreen( T_DELEGATOR &AImplementation, TUniColor color )
		{
			T_OWNER::ClearScreen( C_OWNER, color );
		}

		inline TUniColor GetPixelColor( int16_t x, int16_t y )
		{
			return C_OWNER.GetPixelColor( x, y );
		}

		inline bool getTextWrap()
		{
			return C_OWNER.getTextWrap();
		}

		static inline void drawRect( T_DELEGATOR &AImplementation, int16_t x, int16_t y, int16_t w, int16_t h, TUniColor color ) 
		{
			T_OWNER::drawRect( C_OWNER, x, y, w, h, color );
		}

		static inline void fillRect( T_DELEGATOR &AImplementation, int16_t x, int16_t y, int16_t w, int16_t h, TUniColor color) 
		{
			T_OWNER::fillRect( C_OWNER, x, y, w, h, color );
		}

		static inline void fillScreen( T_DELEGATOR &AImplementation, TUniColor color ) 
		{
			T_OWNER::fillScreen( C_OWNER, color );
		}

//#ifdef _GRAPHICS_DRAW_GRAYSCALE_BITMAP_

		static inline void drawGrayscaleBitmap( T_DELEGATOR &AImplementation, int16_t x, int16_t y, const uint8_t *bitmap, int16_t w, int16_t h, uint8_t AAlpha, uint8_t size )
		{
			T_OWNER::drawGrayscaleBitmap( C_OWNER, x, y, bitmap, w, h, AAlpha, size );
		}

//#endif // _GRAPHICS_DRAW_GRAYSCALE_BITMAP_

//#ifdef _GRAPHICS_DRAW_GRAYSCALE_REPEAT_BITMAP_

		static inline void drawRepeatGrayscaleBitmap( T_DELEGATOR &AImplementation, int16_t x, int16_t y, const uint8_t *bitmap, int16_t AListCount, int16_t w, int16_t h, uint8_t AAlpha, uint8_t size )
		{
			T_OWNER::drawRepeatGrayscaleBitmap( C_OWNER, x, y, bitmap, AListCount, w, h, AAlpha, size );
		}

//#endif // _GRAPHICS_DRAW_GRAYSCALE_REPEAT_BITMAP_

//#ifdef _GRAPHICS_DRAW_GRAYSCALE_REPEAT_ALPHA_BITMAP_

		static inline void drawAlphaGrayscaleRepeatBitmap( T_DELEGATOR &AImplementation, int16_t x, int16_t y, const uint8_t *bitmap, int16_t AListCount, int16_t w, int16_t h, uint8_t size )
		{
			T_OWNER::drawAlphaGrayscaleRepeatBitmap( C_OWNER, x, y, bitmap, AListCount, w, h, size );
		}

//#endif // _GRAPHICS_DRAW_GRAYSCALE_REPEAT_ALPHA_BITMAP_

//#ifdef _GRAPHICS_DRAW_ALPHA_COLOR_BITMAP_

		static inline void drawAlphaColorBitmap( T_DELEGATOR &AImplementation, int16_t x, int16_t y, const uint32_t *bitmap, int16_t w, int16_t h, uint8_t size )
		{
			T_OWNER::drawAlphaColorBitmap( C_OWNER, x, y, bitmap, w, h, size );
		}

//#endif // _GRAPHICS_DRAW_ALPHA_COLOR_BITMAP_

//#ifdef _GRAPHICS_DRAW_ALPHA_RGBW_COLOR_BITMAP_

		static inline void drawAlphaRGBWColorBitmap( T_DELEGATOR &AImplementation, int16_t x, int16_t y, const uint8_t *bitmap, int16_t w, int16_t h, uint8_t size )
		{
			T_OWNER::drawAlphaRGBWColorBitmap( C_OWNER, x, y, bitmap, w, h, size );
		}

//#endif // _GRAPHICS_DRAW_ALPHA_COLOR_BITMAP_

//#ifdef _GRAPHICS_DRAW_COLOR_BITMAP_

		static inline void drawColorBitmap( T_DELEGATOR &AImplementation, int16_t x, int16_t y, const uint8_t *bitmap, int16_t w, int16_t h, uint8_t size )
		{
			T_OWNER::drawColorBitmap( C_OWNER, x, y, bitmap, w, h, size );
		}

//#endif // _GRAPHICS_DRAW_COLOR_BITMAP_

//#ifdef _GRAPHICS_DRAW_RGBW_COLOR_BITMAP_

		static inline void drawRGBWColorBitmap( T_DELEGATOR &AImplementation, int16_t x, int16_t y, const uint8_t *bitmap, int16_t w, int16_t h, uint8_t size )
		{
			T_OWNER::drawRGBWColorBitmap( C_OWNER, x, y, bitmap, w, h, size );
		}

//#endif // _GRAPHICS_DRAW_RGBW_COLOR_BITMAP_

//#ifdef _GRAPHICS_DRAW_REPEAT_COLOR_BITMAP_

		static inline void drawRepeatColorBitmap( T_DELEGATOR &AImplementation, int16_t x, int16_t y, const uint8_t *bitmap, int16_t AListCount, int16_t w, int16_t h, uint8_t size )
		{
			T_OWNER::drawRepeatColorBitmap( C_OWNER, x, y, bitmap, AListCount, w, h, size );
		}

//#endif // _GRAPHICS_DRAW_REPEAT_COLOR_BITMAP_

//#ifdef _GRAPHICS_DRAW_REPEAT_RGBW_COLOR_BITMAP_

		static inline void drawRepeatRGBWColorBitmap( T_DELEGATOR &AImplementation, int16_t x, int16_t y, const uint8_t *bitmap, int16_t AListCount, int16_t w, int16_t h, uint8_t size )
		{
			T_OWNER::drawRepeatRGBWColorBitmap( C_OWNER, x, y, bitmap, AListCount, w, h, size );
		}

//#endif // _GRAPHICS_DRAW_REPEAT_RGBW_COLOR_BITMAP_

//#ifdef _GRAPHICS_DRAW_REPEAT_ALPHA_COLOR_BITMAP_

		static inline void drawRepeatAlphaColorBitmap( T_DELEGATOR &AImplementation, int16_t x, int16_t y, const uint8_t *bitmap, int16_t AListCount, int16_t w, int16_t h, uint8_t size )
		{
			T_OWNER::drawRepeatAlphaColorBitmap( C_OWNER, x, y, bitmap, AListCount, w, h, size );
		}

//#endif // _GRAPHICS_DRAW_REPEAT_ALPHA_COLOR_BITMAP_

//#ifdef _GRAPHICS_DRAW_REPEAT_ALPHA_RGBW_COLOR_BITMAP_

		static inline void drawRepeatAlphaRGBWColorBitmap( T_DELEGATOR &AImplementation, int16_t x, int16_t y, const uint8_t *bitmap, int16_t AListCount, int16_t w, int16_t h, uint8_t size )
		{
			T_OWNER::drawRepeatAlphaRGBWColorBitmap( C_OWNER, x, y, bitmap, AListCount, w, h, size );
		}

//#endif // _GRAPHICS_DRAW_REPEAT_ALPHA_RGBW_COLOR_BITMAP_

//#ifdef _GRAPHICS_DRAW_GRAYSCALE_ALPHA_BITMAP_

		static inline void drawAlphaGrayscaleBitmap( T_DELEGATOR &AImplementation, int16_t x, int16_t y, const uint16_t *bitmap, int16_t w, int16_t h, uint8_t size )
		{
			T_OWNER::drawAlphaGrayscaleBitmap( C_OWNER, x, y, bitmap, w, h, size );
		}

//#endif // _GRAPHICS_DRAW_GRAYSCALE_ALPHA_BITMAP_

//#ifdef _GRAPHICS_DRAW_BLACK_WHITE_BITMAP_

		// Draw a 1-bit color bitmap at the specified x, y position from the
		// provided bitmap buffer (must be PROGMEM memory) using color as the
		// foreground color and bg as the background color.
		static inline void drawBWBitmap( T_DELEGATOR &AImplementation, int16_t x, int16_t y, const uint8_t *bitmap, int16_t w, int16_t h, TUniColor color, TUniColor bg, uint8_t size )
		{
			T_OWNER::drawBWBitmap( C_OWNER, x, y, bitmap, w, h, color, bg, size );
		}

//#endif // _GRAPHICS_DRAW_BLACK_WHITE_BITMAP_

		static inline void drawTransparencyBitmap( T_DELEGATOR &AImplementation, int16_t x, int16_t y, const uint8_t *bitmap, int16_t w, int16_t h, uint8_t size )
		{
			T_OWNER::drawTransparencyBitmap( C_OWNER, x, y, bitmap, w, h, size );
		}

		static inline void drawRoundRect( T_DELEGATOR &AImplementation, int16_t x, int16_t y, int16_t w, int16_t h, int16_t rx, int16_t ry, TUniColor color )
		{
			T_OWNER::drawRoundRect( C_OWNER, x, y, w, h, rx, ry, color );
		}

		static inline void fillRoundRect( T_DELEGATOR &AImplementation, int16_t x, int16_t y, int16_t w, int16_t h, int16_t rx, int16_t ry, TUniColor color )
		{
			T_OWNER::fillRoundRect( C_OWNER, x, y, w, h, rx, ry, color );
		}

		// Bresenham's algorithm - thx wikpedia
		static inline void drawLine( T_DELEGATOR &AImplementation, int16_t x0, int16_t y0, int16_t x1, int16_t y1, TUniColor color )
		{
			T_OWNER::drawLine( C_OWNER, x0, y0, x1, y1, color );
		}

		static inline void drawFastVLine( T_DELEGATOR &AImplementation, int16_t x, int16_t y, int16_t h, TUniColor color )
		{
			T_OWNER::drawFastVLine( C_OWNER, x, y, h, color );
		}

		static inline void drawFastHLine( T_DELEGATOR &AImplementation, int16_t x, int16_t y, int16_t w, TUniColor color )
		{
			T_OWNER::drawFastHLine( C_OWNER, x, y, w, color );
		}

		static inline size_t write( T_DELEGATOR &AImplementation, uint8_t c )
		{
			return T_OWNER::write( C_OWNER, c );
		}

		// Draw a character
		static inline void drawChar( T_DELEGATOR &AImplementation, int16_t x, int16_t y, unsigned char c, TUniColor color, TUniColor bg, uint8_t size )
		{
//			Serial.println( "drawChar" );
			T_OWNER::drawChar( C_OWNER, x, y, c, color, bg, size );
		}

		inline void setCursor( int16_t x, int16_t y )
		{
			C_OWNER.setCursor( x, y );
		}

		inline void getCursor( int16_t &x, int16_t &y ) const
		{
			C_OWNER.getCursor( x, y );
		}

		inline void setTextSize( uint8_t s )
		{
			C_OWNER.setTextSize( s );
		}

		inline uint8_t getTextSize()
		{
			return C_OWNER.getTextSize();
		}

		inline void setTextColor( TUniColor c, TUniColor b )
		{
			C_OWNER.setTextColor( c, b );
		}

		inline void setTextWrap(bool w )
		{
			C_OWNER.setTextWrap( w );
		}

		inline void SetCp437(bool x) 
		{
			C_OWNER.SetCp437( x );
		}

		inline bool GetCp437()
		{
			return C_OWNER.GetCp437();
		}

	public:
		// Return the size of the display (per current rotation)
		inline static int16_t width()
		{
			return C_OWNER.width();
		}

		inline static int16_t height()
		{
			return C_OWNER.height();
		}

	};
//---------------------------------------------------------------------------
	template<typename T_OWNER> class GraphicsImpl : public Graphics<T_OWNER>, public GraphicsIntf<T_OWNER>
	{
		typedef Graphics<T_OWNER> inherited;

	public:
		TUniColor	BackgroundColor = 0;

	public:
		inline TUniColor	GetBackgroundColor()
		{
			return BackgroundColor;
		}

	public:
		using inherited::inherited;

	};
//---------------------------------------------------------------------------
	template<typename T_OWNER, T_OWNER &C_OWNER> class GraphicsElementSetCursor
	{
	public:
		uint32_t X = 0;
		uint32_t Y = 0;

	public:
		inline void ClockInputPin_o_Receive( void *_Data )
		{
			Render();
		}

	public:
		void Render()
		{
			C_OWNER.setCursor( X, Y );
		}

	};
//---------------------------------------------------------------------------
	template<typename T_OWNER, T_OWNER &C_OWNER> class GraphicsElementDrawScene : public GraphicsDelegate<GraphicsElementDrawScene<T_OWNER, C_OWNER>, T_OWNER, C_OWNER>, public GraphicsIntf<T_OWNER>
	{
		typedef GraphicsIntf<T_OWNER> inherited;

	public:
		TGraphicsPos X = 0;
		TGraphicsPos Y = 0;

	public:
		void GetPosition( TGraphicsPos &AX, TGraphicsPos &AY )
		{ 
			TGraphicsPos AParentX, AParentY;
			C_OWNER.GetPosition( AParentX, AParentY );

			AX = AParentX + X;
			AY = AParentY + Y;
		}

	public:
		inline void ClockInputPin_o_Receive( void *_Data )
		{
			Render();
		}

	public:
		void Render()
		{
			inherited::RenderElements();
//			for( int i = 0; i < FElements.size(); ++ i )
//				FElements[ i ]->Render( AForce );
		}

	};
//---------------------------------------------------------------------------
}

#ifdef __TEST_FOR_DEBUG_PRINTS__
#undef Serial
#endif

#endif
