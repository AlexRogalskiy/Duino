////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2018 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_DISPLAY_SSD1306_h
#define _MITOV_DISPLAY_SSD1306_h

#include <Mitov.h>
#include <Mitov_Graphics_Monochrome.h>

#ifdef __TEST_FOR_DEBUG_PRINTS__
#define Serial UNGUARDED DEBUG PRINT!!!
#endif

namespace Mitov
{
	namespace SSD1306Const
	{
		const uint8_t SSD1306_SETCONTRAST = 0x81;
		const uint8_t SSD1306_DISPLAYALLON_RESUME = 0xA4;
		const uint8_t SSD1306_DISPLAYALLON = 0xA5;
		const uint8_t SSD1306_NORMALDISPLAY = 0xA6;
		const uint8_t SSD1306_INVERTDISPLAY = 0xA7;
		const uint8_t SSD1306_DISPLAYOFF = 0xAE;
		const uint8_t SSD1306_DISPLAYON = 0xAF;

		const uint8_t SSD1306_SETDISPLAYOFFSET = 0xD3;
		const uint8_t SSD1306_SETCOMPINS = 0xDA;

		const uint8_t SSD1306_SETVCOMDETECT = 0xDB;

		const uint8_t SSD1306_SETDISPLAYCLOCKDIV = 0xD5;
		const uint8_t SSD1306_SETPRECHARGE = 0xD9;

		const uint8_t SSD1306_SETMULTIPLEX = 0xA8;

		const uint8_t SSD1306_SETLOWCOLUMN = 0x00;
		const uint8_t SSD1306_SETHIGHCOLUMN = 0x10;

		const uint8_t SSD1306_SETSTARTLINE = 0x40;

		const uint8_t SSD1306_MEMORYMODE = 0x20;
		const uint8_t SSD1306_COLUMNADDR = 0x21;
		const uint8_t SSD1306_PAGEADDR = 0x22;

		const uint8_t SSD1306_COMSCANINC = 0xC0;
		const uint8_t SSD1306_COMSCANDEC = 0xC8;

		const uint8_t SSD1306_SEGREMAP = 0xA0;

		const uint8_t SSD1306_CHARGEPUMP = 0x8D;

		const uint8_t SSD1306_EXTERNALVCC = 0x1;
		const uint8_t SSD1306_SWITCHCAPVCC = 0x2;
	}
//---------------------------------------------------------------------------
	class TArduinoDisplaySSD1306PreChargePeriod
	{
	public:
		uint8_t	Phase1 : 4;
		uint8_t	Phase2 : 4;

	public:
		TArduinoDisplaySSD1306PreChargePeriod() :
			Phase1( 1 ),
			Phase2( 15 )
		{
		}
	};
//---------------------------------------------------------------------------
	template<typename T_IMPLEMENTATION> class DisplaySSD1306NoReset : public T_IMPLEMENTATION
	{
	protected:
		inline void Reset() {}

	};
//---------------------------------------------------------------------------
	template<typename T_IMPLEMENTATION> class DisplaySSD1306ResetPin : public T_IMPLEMENTATION
	{
	public:
		OpenWire::SourcePin	ResetOutputPin;

	protected:
		inline void Reset()
		{
			ResetOutputPin.SendValue( true );
			// VDD (3.3V) goes high at start, lets just chill for a ms
			delay(1);
			// bring reset low
			ResetOutputPin.SendValue( false );
			// wait 10ms
			delay(10);
			// bring out of reset
			ResetOutputPin.SendValue( true );
			// turn on VCC (9V?)
		}
	};
//---------------------------------------------------------------------------
	template<typename T_IMPLEMENTATION, uint8_t WIDTH, uint8_t HEIGHT> class DisplaySSD1306_Up : public T_IMPLEMENTATION
	{
	protected:
		static inline void CorrectOrientation( int16_t &x, int16_t &y )
		{
		}

		static inline void CorrectScrollOrientation( TGraphicsPos &x, TGraphicsPos &y )
		{
		}

		template<typename T> static inline void drawFastHLineDispatch( T &AImplementation, int16_t x, int16_t y, int16_t w, TUniColor color) 
		{
			AImplementation.drawFastHLineInternal(x, y, w, color);
		}

		template<typename T> static inline void drawFastVLineDispatch( T &AImplementation, int16_t x, int16_t y, int16_t h, TUniColor color) 
		{
			AImplementation.drawFastVLineInternal(x, y, h, color);
		}

	public:
		inline int16_t width(void) const
		{
			return WIDTH;
		}

		inline int16_t height(void) const
		{
			return HEIGHT;
		}

	};
//---------------------------------------------------------------------------
	template<typename T_IMPLEMENTATION, uint8_t WIDTH, uint8_t HEIGHT> class DisplaySSD1306_Down : public T_IMPLEMENTATION
	{
	protected:
		static inline void CorrectOrientation( int16_t &x, int16_t &y )
		{
			x = WIDTH - x - 1;
			y = HEIGHT - y - 1;
		}

		static inline void CorrectScrollOrientation( TGraphicsPos &x, TGraphicsPos &y )
		{
			x = -x;
			y = -y;
		}

		template<typename T> static inline void drawFastHLineDispatch( T &AImplementation, int16_t x, int16_t y, int16_t w, TUniColor color) 
		{
			x = WIDTH - x - 1;
			y = HEIGHT - y - 1;
			x -= (w-1);

			AImplementation.drawFastHLineInternal(x, y, w, color);
		}

		template<typename T> static inline void drawFastVLineDispatch( T &AImplementation, int16_t x, int16_t y, int16_t h, TUniColor color) 
		{
			x = WIDTH - x - 1;
//			y = HEIGHT - y - 1;
			y = HEIGHT - y - h;
//			x -= (w-1);

			AImplementation.drawFastVLineInternal(x, y, h, color);
		}

	public:
		inline int16_t width(void) const
		{
			return WIDTH;
		}

		inline int16_t height(void) const
		{
			return HEIGHT;
		}

	};
//---------------------------------------------------------------------------
	template<typename T_IMPLEMENTATION, uint8_t WIDTH, uint8_t HEIGHT> class DisplaySSD1306_Left : public T_IMPLEMENTATION
	{
	protected:
		static inline void CorrectOrientation( int16_t &x, int16_t &y )
		{
			swap(x, y);
			y = HEIGHT - y - 1;
		}

		static inline void CorrectScrollOrientation( TGraphicsPos &x, TGraphicsPos &y )
		{
			swap(x, y);
			y = -y;
		}

		template<typename T> static inline void drawFastHLineDispatch( T &AImplementation, int16_t x, int16_t y, int16_t w, TUniColor color) 
		{
			// 270 degree rotation, swap x & y for rotation, then invert y  and adjust y for w (not to become h)
			swap(x, y);
			y = HEIGHT - y - 1;
			y -= (w-1);

			AImplementation.drawFastVLineInternal(x, y, w, color);

//			AImplementation.drawFastHLineInternal(x, y, w, color);
		}

		template<typename T> static inline void drawFastVLineDispatch( T &AImplementation, int16_t x, int16_t y, int16_t h, TUniColor color) 
		{
			swap(x, y);
			y = HEIGHT - y - 1;

			AImplementation.drawFastHLineInternal(x, y, h, color);

//			AImplementation.drawFastVLineInternal(x, y, h, color);
		}

	public:
		inline int16_t width(void) const
		{
			return HEIGHT;
		}

		inline int16_t height(void) const
		{
			return WIDTH;
		}

	};
//---------------------------------------------------------------------------
	template<typename T_IMPLEMENTATION, uint8_t WIDTH, uint8_t HEIGHT> class DisplaySSD1306_Right : public T_IMPLEMENTATION
	{
	protected:
		static inline void CorrectOrientation( int16_t &x, int16_t &y )
		{
			swap(x, y);
			x = WIDTH - x - 1;
		}

		static inline void CorrectScrollOrientation( TGraphicsPos &x, TGraphicsPos &y )
		{
			swap(x, y);
			x = -x;
		}

		template<typename T> static inline void drawFastHLineDispatch( T &AImplementation, int16_t x, int16_t y, int16_t w, TUniColor color) 
		{
			// 90 degree rotation, swap x & y for rotation, then invert x
			swap(x, y);
			x = WIDTH - x - 1;

			AImplementation.drawFastVLineInternal(x, y, w, color);

//			AImplementation.drawFastHLineInternal(x, y, w, color);
		}

		template<typename T> static inline void drawFastVLineDispatch( T &AImplementation, int16_t x, int16_t y, int16_t h, TUniColor color) 
		{
			swap(x, y);
			x = WIDTH - x - h;

			AImplementation.drawFastHLineInternal(x, y, h, color);

//			AImplementation.drawFastVLineInternal(x, y, h, color);
		}

	public:
		inline int16_t width(void) const
		{
			return HEIGHT;
		}

		inline int16_t height(void) const
		{
			return WIDTH;
		}

	};
//---------------------------------------------------------------------------
/*
	template<typename T_IMPLEMENTATION, uint8_t WIDTH, uint8_t HEIGHT> class DisplaySSD1306Direct : public T_IMPLEMENTATION
	{
	protected:
		uint8_t		FData;
		uint16_t	FAddress : 12;
		bool		FModified : 1;

	public:
		bool	UseChargePump : 1;
		bool	PowerOn : 1;

	protected:
		inline void drawPixelInternal( int16_t AAddress, int8_t ABitMask, TUniColor AColor )
		{
			Serial.println( "drawPixelInternal" );
			Serial.println( AAddress );

//			return;
			if( FAddress != AAddress )
			{
				display();

				FAddress = AAddress;
				T_IMPLEMENTATION::SendStartAddress( FAddress );
				T_IMPLEMENTATION::ReadBuffer( &FData, 1 );
			}

			uint8_t AData = FData;

//			Serial.println( int( AColor ));

			// x is which column
			switch ( AColor ) 
			{
				case tmcWhite :		AData |=  ABitMask; break;
				case tmcBlack :		AData &= ~ABitMask; break; 
				case tmcInvert :	AData ^=  ABitMask; break; 
			}

//			Serial.print( FData );
//			Serial.print( " = " );
//			Serial.println( AData );

//			if( FData == AData )
//				return;

//			Serial.println( "FModified" );
			FData = AData;
			FModified = true;
		}

		inline TUniColor GetPixelColorInternal( int16_t AAddress, int8_t ABitMask )
		{
			if( FAddress != AAddress )
				display();
		}

		inline void drawFastHLineInternalImpl( int16_t x, int16_t y, int16_t w, TUniColor AColor )
		{
		}

		inline void drawFastVLineInternalImpl(int16_t x, int16_t __y, int16_t __h, TUniColor color)
		{
		}

	public:
		inline void ClearScreenInternal( TUniColor color )
		{
			static uint8_t ABuffer[ WIDTH * HEIGHT / 8 ] = { 0 };
			T_IMPLEMENTATION::SendStartAddress( 0 );
			T_IMPLEMENTATION::SendBuffer( ABuffer, WIDTH * HEIGHT / 8 );

			FData = 0;

/ *
			uint8_t AByte = 0xFF;
			T_IMPLEMENTATION::SendStartAddress( 0 );
			T_IMPLEMENTATION::SendBuffer( &AByte, 1 );

//			T_IMPLEMENTATION::SendStartAddress( 63 );
			T_IMPLEMENTATION::SendStartAddress( 120 );
			T_IMPLEMENTATION::SendBuffer( &AByte, 1 );
* /
		}

		inline void ScrollInternal( TGraphicsPos X, TGraphicsPos Y, TUniColor color )
		{
		}


	public:
		inline void display()
		{
			if( ! FModified )
				return;

			T_IMPLEMENTATION::SendStartAddress( FAddress );
			T_IMPLEMENTATION::SendBuffer( &FData, 1 );

			FModified = false;
		}

	public:
		DisplaySSD1306Direct() :
			UseChargePump( false ),
			PowerOn( true ),
			FModified( false ),
			FAddress( 0 )
		{
		}

	};
*/
//---------------------------------------------------------------------------
	template<typename T_IMPLEMENTATION, uint8_t WIDTH, uint8_t HEIGHT> class DisplaySSD1306Buffered : public T_IMPLEMENTATION
	{
	public:
		bool	UseChargePump : 1;
		bool	PowerOn : 1;

	protected:
		bool	FModified : 1;
		uint8_t FBuffer[ WIDTH * HEIGHT / 8 ];

	protected:
		inline void drawPixelInternal( int16_t AAddress, int8_t ABitMask, TUniColor AColor )
		{
			// x is which column
			switch ( AColor ) 
			{
				case tmcWhite :		FBuffer[ AAddress ] |=  ABitMask; break;
				case tmcBlack :		FBuffer[ AAddress ] &= ~ABitMask; break; 
				case tmcInvert :	FBuffer[ AAddress ] ^=  ABitMask; break; 
			}

			FModified = true;
		}

		inline TUniColor GetPixelColorInternal( int16_t AAddress, int8_t ABitMask )
		{
			return (( FBuffer[ AAddress ] & ABitMask ) != 0 );
		}

		inline void drawFastHLineInternalImpl( int16_t x, int16_t y, int16_t w, TUniColor AColor )
		{
			// set up the pointer for  movement through the FBuffer
			register uint8_t *pBuf = FBuffer;
			// adjust the FBuffer pointer for the current row
			pBuf += ((y/8) * WIDTH);
			// and offset x columns in
			pBuf += x;

			register uint8_t mask = 1 << (y&7);

			switch ( AColor ) 
			{
				case tmcWhite:	while(w--) { *pBuf++ |= mask; }; break;
				case tmcBlack:	mask = ~mask;   while(w--) { *pBuf++ &= mask; }; break;
				case tmcInvert: while(w--) { *pBuf++ ^= mask; }; break;
			}
		}

		inline void drawFastVLineInternalImpl(int16_t x, int16_t __y, int16_t __h, TUniColor color)
		{
			// this display doesn't need ints for coordinates, use local byte registers for faster juggling
			register uint8_t y = __y;
			register uint8_t h = __h;

			// set up the pointer for fast movement through the FBuffer
			register uint8_t *pBuf = FBuffer;
			// adjust the FBuffer pointer for the current row
			pBuf += ((y/8) * WIDTH);
			// and offset x columns in
			pBuf += x;

			// do the first partial byte, if necessary - this requires some masking
			register uint8_t mod = (y&7);
			if(mod) 
			{
				// mask off the high n bits we want to set 
				mod = 8-mod;

				// note - lookup table results in a nearly 10% performance improvement in fill* functions
				// register uint8_t mask = ~(0xFF >> (mod));
				static uint8_t premask[8] = {0x00, 0x80, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC, 0xFE };
				register uint8_t mask = premask[mod];

				// adjust the mask if we're not going to reach the end of this byte
				if( h < mod)
					mask &= (0XFF >> (mod-h));

				switch (color) 
				{
					case tmcWhite :  *pBuf |=  mask;  break;
					case tmcBlack :  *pBuf &= ~mask;  break;
					case tmcInvert : *pBuf ^=  mask;  break;
				}
  
				// fast exit if we're done here!
				if(h<mod) 
					return; 

				h -= mod;

				pBuf += WIDTH;
			}

			// write solid bytes while we can - effectively doing 8 rows at a time
			if(h >= 8) 
			{ 
				if (color == tmcInvert)  
				{          // separate copy of the code so we don't impact performance of the black/white write version with an extra comparison per loop
					do  
					{
						*pBuf=~(*pBuf);

						// adjust the FBuffer forward 8 rows worth of data
						pBuf += WIDTH;

						// adjust h & y (there's got to be a faster way for me to do this, but this should still help a fair bit for now)
						h -= 8;
					} while(h >= 8);
				}

				else 
				{
					// store a local value to work with 
					register uint8_t val = (color == tmcWhite) ? 255 : 0;

					do  
					{
						// write our value in
						*pBuf = val;

						// adjust the FBuffer forward 8 rows worth of data
						pBuf += WIDTH;

						// adjust h & y (there's got to be a faster way for me to do this, but this should still help a fair bit for now)
						h -= 8;
					} while(h >= 8);
				}
			}

			// now do the final partial byte, if necessary
			if(h) 
			{
				mod = h & 0b111;
				// this time we want to mask the low bits of the byte, vs the high bits we did above
				// register uint8_t mask = (1 << mod) - 1;
				// note - lookup table results in a nearly 10% performance improvement in fill* functions
				static uint8_t postmask[8] = {0x00, 0x01, 0x03, 0x07, 0x0F, 0x1F, 0x3F, 0x7F };
				register uint8_t mask = postmask[mod];
				switch (color) 
				{
					case tmcWhite:   *pBuf |=  mask;  break;
					case tmcBlack:   *pBuf &= ~mask;  break;
					case tmcInvert:  *pBuf ^=  mask;  break;
				}
			}
		}

	public:
		inline void ClearScreenInternal( TUniColor color )
		{
			switch( color )
			{
				case tmcBlack :
					memset( FBuffer, 0, ( WIDTH * HEIGHT / 8));
					break;

				case tmcWhite :
					memset( FBuffer, 255, ( WIDTH * HEIGHT / 8));
					break;

				case tmcInvert :
				{
					for( int i = 0; i < WIDTH * HEIGHT / 8; ++i )
						FBuffer[ i ] = ~FBuffer[ i ];

					break;
				}
			}
		}

		inline void ScrollInternal( TGraphicsPos X, TGraphicsPos Y, TUniColor color )
		{
			uint8_t	AOffset = abs( Y ) & 0b111;
			int32_t	AYBytes = Y / 8;
			AYBytes *= WIDTH;
/*
			if( ! Y )
			{
				uint8_t *ABuffer = FBuffer;
				if( X > 0 )
					for( int ayi = 0; ayi < HEIGHT / 8; ++ ayi, ABuffer += WIDTH )
					{
						memmove( ABuffer + X, ABuffer, WIDTH - X );
						switch( color )
						{
							case tmcBlack:
								memset( ABuffer, 0, X ); break;

							case tmcWhite:
								memset( ABuffer, 255, X ); break;

							case tmcInvert:
								for( int axi = 0; axi < X; ++ axi )
									ABuffer[ axi ] = ~ABuffer[ axi ];
						}
					}

				else // Y = 0 && X < 0
					for( int ayi = 0; ayi < HEIGHT / 8; ++ ayi, ABuffer += WIDTH )
					{
						memmove( ABuffer, ABuffer - X, WIDTH + X );
						switch( color )
						{
							case tmcBlack:
								memset( ABuffer + WIDTH + X, 0, -X ); break;

							case tmcWhite:
								memset( ABuffer + WIDTH + X, 255, -X ); break;

							case tmcInvert:
								for( int axi = WIDTH + X; axi < WIDTH; ++ axi )
									ABuffer[ axi ] = ~ABuffer[ axi ];
						}
					}

			}

			else // Y <> 0
*/
			{
				if( Y > 0 )
				{
					if( X == 0 )
					{
						if( AYBytes )
							memmove( FBuffer + AYBytes, FBuffer, ( HEIGHT / 8 ) * WIDTH - AYBytes );

					}

					else if( X > 0 )
					{
						register uint8_t *ABuffer = FBuffer + WIDTH * (( ( HEIGHT - Y + 7 ) / 8 ) - 1 );
						for( int ayi = 0; ayi < ( HEIGHT - Y + 7 ) / 8; ++ ayi, ABuffer -= WIDTH )
						{
							memmove( ABuffer + AYBytes + X, ABuffer, WIDTH - X );
							switch( color )
							{
								case tmcBlack:
									memset( ABuffer + AYBytes, 0, X ); break;

								case tmcWhite:
									memset( ABuffer + AYBytes, 255, X ); break;

								case tmcInvert:
									for( int axi = 0; axi < X; ++ axi )
										ABuffer[ axi + AYBytes ] = ~ABuffer[ axi + AYBytes ];
							}
						}
					}

					else // Y > 0 && X < 0
					{ 
						register uint8_t *ABuffer = FBuffer + WIDTH * (( ( HEIGHT - Y + 7 ) / 8 ) - 1 );
						for( int ayi = 0; ayi < ( HEIGHT - Y + 7 ) / 8; ++ ayi, ABuffer -= WIDTH )
						{
							memmove( ABuffer + AYBytes, ABuffer - X, WIDTH + X );
							switch( color )
							{
								case tmcBlack:
									memset( ABuffer + AYBytes + WIDTH + X, 0, -X ); break;

								case tmcWhite:
									memset( ABuffer + AYBytes + WIDTH + X, 255, -X ); break;

								case tmcInvert:
									for( int axi = WIDTH + X; axi < WIDTH; ++ axi )
										ABuffer[ axi + AYBytes ] = ~ABuffer[ axi + AYBytes ];
							}
						}
					}

					register uint8_t * AToPtr = FBuffer + WIDTH * HEIGHT / 8 - 1;
					register uint8_t *AFromPtr = AToPtr - WIDTH;
					for( int ayi = 0; ayi < ( HEIGHT - Y + 7 ) / 8 - 1; ++ ayi )
						for( int axi = 0; axi < WIDTH; ++ axi, --AFromPtr, --AToPtr )
							*AToPtr = *AToPtr << AOffset | ( *AFromPtr >> ( 8 - AOffset ));

					if( color != tmcNone )
					{
						uint8_t	AMask;
						if( color == tmcBlack )
							AMask = 0xFF << AOffset;

						else
							AMask = 0xFF >> ( 8 - AOffset );

						AToPtr = FBuffer + AYBytes;
						for( int axi = 0; axi < WIDTH; ++ axi, ++AToPtr )
						{
							switch( color )
							{
								case tmcBlack:
									*AToPtr <<= AOffset;
									*AToPtr &= AMask; 
									break;

								case tmcWhite:
									*AToPtr <<= AOffset;
									*AToPtr |= AMask; 
									break;

								case tmcInvert:
									*AToPtr <<= AOffset | ( *AToPtr & ( ~AMask ));
									*AToPtr ^= AMask; 
									break;
							}
						}
					}
				}

				else // Y < 0
				{
					if( X == 0 )
					{
						if( AYBytes )
							memmove( FBuffer, FBuffer - AYBytes, ( HEIGHT / 8 ) * WIDTH + AYBytes );

					}

					else if( X > 0 )
					{
						register uint8_t *ABuffer = FBuffer;
						for( int ayi = 0; ayi < ( HEIGHT + Y + 7 ) / 8; ++ ayi, ABuffer += WIDTH )
						{
							memmove( ABuffer + X, ABuffer - AYBytes, WIDTH - X );
							switch( color )
							{
								case tmcBlack:
									memset( ABuffer, 0, X ); break;

								case tmcWhite:
									memset( ABuffer, 255, X ); break;

								case tmcInvert:
									for( int axi = 0; axi < X; ++ axi )
										ABuffer[ axi ] = ~ABuffer[ axi ];
							}
						}
					}
					else // Y < 0 && X < 0
					{
						register uint8_t *ABuffer = FBuffer;
						for( int ayi = 0; ayi < ( HEIGHT + Y + 7 ) / 8; ++ ayi, ABuffer += WIDTH )
						{
							memmove( ABuffer, ABuffer - AYBytes - X, WIDTH + X );
							switch( color )
							{
								case tmcBlack:
									memset( ABuffer + WIDTH + X, 0, -X ); break;

								case tmcWhite:
									memset( ABuffer + WIDTH + X, 255, -X ); break;

								case tmcInvert:
									for( int axi = WIDTH + X; axi < WIDTH; ++ axi )
										ABuffer[ axi ] = ~ABuffer[ axi ];
							}
						}
					}

					if( AOffset )
					{
						register uint8_t * AToPtr = FBuffer;
						register uint8_t *AFromPtr = AToPtr + WIDTH;
						for( int ayi = 0; ayi < ( HEIGHT + Y + 7 ) / 8 - 1; ++ ayi )
							for( int axi = 0; axi < WIDTH; ++ axi, ++AFromPtr, ++AToPtr )
								*AToPtr = *AToPtr >> AOffset | ( *AFromPtr << ( 8 - AOffset ));

						if( color != tmcNone )
						{
							uint8_t	AMask;
							if( color == tmcBlack )
								AMask = 0xFF >> AOffset;

							else
								AMask = 0xFF << ( 8 - AOffset );

							AToPtr = FBuffer + WIDTH * HEIGHT / 8 + AYBytes - WIDTH;
							for( int axi = 0; axi < WIDTH; ++ axi, ++AToPtr )
							{
								switch( color )
								{
									case tmcBlack:
										*AToPtr >>= AOffset;
										*AToPtr &= AMask; 
										break;

									case tmcWhite:
										*AToPtr >>= AOffset;
										*AToPtr |= AMask; 
										break;

									case tmcInvert:
										*AToPtr >>= AOffset | ( *AToPtr & ( ~AMask ));
										*AToPtr ^= AMask; 
										break;
								}
							}
						}
					}
				}

				if( AYBytes )
				{
					if( Y > 0 )
					{
						switch( color )
						{
							case tmcBlack:
								memset( FBuffer, 0, AYBytes ); break;

							case tmcWhite:
								memset( FBuffer, 255, AYBytes ); break;

							case tmcInvert:
							{
								register uint8_t *ABuffer = FBuffer;
								for( int axi = 0; axi < WIDTH; ++ axi )
								{
									*ABuffer = ~*ABuffer;
									ABuffer++;
								}
							}
						}
					}

					else // Y < 0
					{
						switch( color )
						{
							case tmcBlack:
								memset( FBuffer + HEIGHT * WIDTH / 8 + AYBytes, 0, - AYBytes ); break;

							case tmcWhite:
								memset( FBuffer + HEIGHT * WIDTH / 8 + AYBytes, 255, - AYBytes ); break;

							case tmcInvert:
							{
								register uint8_t *ABuffer = FBuffer + HEIGHT * WIDTH / 8 + AYBytes;
								for( int axi = 0; axi < WIDTH; ++ axi )
								{
									*ABuffer = ~*ABuffer;
									ABuffer++;
								}
							}
						}
					}
				}
			}
		}

	public:
		void display()
		{
			if( ! FModified )
				return;

			if( ! PowerOn )
				return;

			T_IMPLEMENTATION::SendBuffer( FBuffer );
			FModified = false;
		}

	public:
		DisplaySSD1306Buffered() :
			UseChargePump( false ),
			PowerOn( true ),
			FModified( true )
		{
		}

	};
//---------------------------------------------------------------------------
	template<typename T_IMPLEMENTATION, uint8_t WIDTH, uint8_t HEIGHT, uint8_t C_SETCOMPINS, uint8_t C_PAGE_END_ADDR> class DisplaySSD1306 : public T_IMPLEMENTATION, public GraphicsImpl<DisplaySSD1306<T_IMPLEMENTATION, WIDTH, HEIGHT, C_SETCOMPINS, C_PAGE_END_ADDR>>
	{
		typedef DisplaySSD1306<T_IMPLEMENTATION, WIDTH, HEIGHT, C_SETCOMPINS, C_PAGE_END_ADDR>	self;
		typedef GraphicsImpl<self> inherited;

	public:
		float	Contrast = 0.812;

		TGraphicsMonochromeTextSettings	Text;

		TArduinoDisplaySSD1306PreChargePeriod	PreChargePeriod;

	public:
		void SetPowerOn( bool AValue )
		{
			if( T_IMPLEMENTATION::PowerOn == AValue )
				return;

			T_IMPLEMENTATION::PowerOn = AValue;
			if( T_IMPLEMENTATION::PowerOn )
				DoPowerOn();

			else
				DoPowerOff();
		}

	public:
		void UpdateContrast()
		{
			T_IMPLEMENTATION::SendCommand( SSD1306Const::SSD1306_SETCONTRAST );
			T_IMPLEMENTATION::SendCommand( Contrast * 255 + 0.5 );
		}

	public:
		void drawPixel(int16_t x, int16_t y, TUniColor AColor )
		{
			if( AColor == tmcNone )
				return;

//			Serial.println( "drawPixel" );

			if ((x < 0) || (x >= T_IMPLEMENTATION::width() ) || (y < 0) || (y >= T_IMPLEMENTATION::height() ))
				return;

			// check rotation, move pixel around if necessary
			T_IMPLEMENTATION::CorrectOrientation( x, y );

			T_IMPLEMENTATION::drawPixelInternal( x+ (y/8) * WIDTH, 1 << (y & 0b111), AColor );
/*
			// x is which column
			switch (color) 
			{
				case tmcWhite :		FBuffer[x+ (y/8) * WIDTH ] |=  (1 << (y&7)); break;
				case tmcBlack :		FBuffer[x+ (y/8) * WIDTH ] &= ~(1 << (y&7)); break; 
				case tmcInvert :	FBuffer[x+ (y/8) * WIDTH ] ^=  (1 << (y&7)); break; 
			}

			FModified = true;
*/
		}

		static void ClearScreen( self &AImplementation, TUniColor color )
		{
			AImplementation.ClearScreenInternal( color );
		}

		TUniColor GetPixelColor( int16_t x, int16_t y )
		{
			if ((x < 0) || (x >= T_IMPLEMENTATION::width() ) || (y < 0) || (y >= T_IMPLEMENTATION::height() ))
				return false;

			// check rotation, move pixel around if necessary
			T_IMPLEMENTATION::CorrectOrientation( x, y );

			return T_IMPLEMENTATION::GetPixelColorInternal( x + (y/8) * WIDTH, 1 << ( y & 0b111 ) );

//			return (( FBuffer[ x + ( y / 8 ) * WIDTH ] & ( 1 << ( y & 0b111 ))) != 0 );
		}
		
		static inline void drawFastHLine( self &AImplementation, int16_t x, int16_t y, int16_t w, TUniColor color) 
		{
			T_IMPLEMENTATION::drawFastHLineDispatch( AImplementation, x, y, w, color );
//			AImplementation.drawFastHLineInternal(x, y, w, color);
		}

		static inline void drawFastVLine( self &AImplementation, int16_t x, int16_t y, int16_t h, TUniColor color) 
		{
			T_IMPLEMENTATION::drawFastVLineDispatch( AImplementation, x, y, h, color ); 
//			AImplementation.drawFastVLineInternal(x, y, h, color);
		}

		static void Scroll( self &AImplementation, TGraphicsPos X, TGraphicsPos Y, TUniColor color )
		{
			if( X == 0 && Y == 0 )
				return;

			// check rotation, move pixel around if necessary
			T_IMPLEMENTATION::CorrectScrollOrientation( X, Y );
			AImplementation.ScrollInternal( X, Y, color );
		}

	public:
		void drawFastHLineInternal( int16_t x, int16_t y, int16_t w, TUniColor AColor ) 
		{
			if( AColor == tmcNone )
				return;

			// Do bounds/limit checks
			if(y < 0 || y >= HEIGHT) 
				return;

			// make sure we don't try to draw below 0
			if(x < 0) 
			{ 
				w += x;
				x = 0;
			}

			// make sure we don't go off the edge of the display
			if( (x + w) > WIDTH) 
				w = (WIDTH - x);

			// if our width is now negative, punt
			if(w <= 0) 
				return;

			T_IMPLEMENTATION::drawFastHLineInternalImpl( x, y, w, AColor );
		}

		void drawFastVLineInternal(int16_t x, int16_t __y, int16_t __h, TUniColor AColor ) 
		{
			if( AColor == tmcNone )
				return;

			// do nothing if we're off the left or right side of the screen
			if(x < 0 || x >= WIDTH) 
				return;

			// make sure we don't try to draw below 0
			if(__y < 0) 
			{ 
				// __y is negative, this will subtract enough from __h to account for __y being 0
				__h += __y;
				__y = 0;
			} 

			// make sure we don't go past the height of the display
			if( (__y + __h) > HEIGHT)
				__h = (HEIGHT - __y);
			
			// if our height is now negative, punt 
			if(__h <= 0)
				return;			

			T_IMPLEMENTATION::drawFastVLineInternalImpl( x, __y, __h, AColor );
		}

		void SendCommands( const uint8_t ACommands[], int ASize ) 
		{
			for( int i = 0; i < ASize; ++ i )
				T_IMPLEMENTATION::SendCommand( ACommands[ i ] );
		}

	protected:
		void DoPowerOff()
		{
			T_IMPLEMENTATION::SendCommand( SSD1306Const::SSD1306_DISPLAYOFF );
			delay( 100 );
		}

		void DoPowerOn()
		{
			T_IMPLEMENTATION::Reset();

			static const uint8_t InitCommands1[] =
			{
				SSD1306Const::SSD1306_DISPLAYOFF,                    // 0xAE
				SSD1306Const::SSD1306_SETDISPLAYCLOCKDIV,            // 0xD5
				0x80,                                  // the suggested ratio 0x80

				SSD1306Const::SSD1306_SETDISPLAYOFFSET,              // 0xD3
				0x0,                                   // no offset
				SSD1306Const::SSD1306_SETSTARTLINE | 0x0,            // line #0
				SSD1306Const::SSD1306_CHARGEPUMP                     // 0x8D
			};

			// Init sequence for 128x64 OLED module
			SendCommands( ARRAY_PARAM( InitCommands1 ) );
			T_IMPLEMENTATION::SendCommand( T_IMPLEMENTATION::UseChargePump ? 0x10 : 0x14 );

			static const uint8_t InitCommands2[] =
			{
				SSD1306Const::SSD1306_MEMORYMODE,                    // 0x20
				0x00,                                  // 0x0 act like ks0108
				SSD1306Const::SSD1306_SEGREMAP | 0x1,
				SSD1306Const::SSD1306_COMSCANDEC,

				SSD1306Const::SSD1306_SETCOMPINS,  // 0xDA
				C_SETCOMPINS,

				SSD1306Const::SSD1306_COLUMNADDR,
				0x00,
				WIDTH - 1,
				SSD1306Const::SSD1306_SETMULTIPLEX,
				HEIGHT - 1,
				SSD1306Const::SSD1306_PAGEADDR,
				0x00,				
				C_PAGE_END_ADDR,
				SSD1306Const::SSD1306_SETPRECHARGE  // 0xd9
			};

			SendCommands( ARRAY_PARAM( InitCommands2 ) );
			T_IMPLEMENTATION::SendCommand( ( uint8_t( PreChargePeriod.Phase2 ) << 4 ) | PreChargePeriod.Phase1 );

//			T_IMPLEMENTATION::SendPinsConfigCommand();
			UpdateContrast();


/*
			T_IMPLEMENTATION::SendCommand(SSD1306Const::SSD1306_COLUMNADDR);
			T_IMPLEMENTATION::SendCommand(0);   // Column start address (0 = reset)
			T_IMPLEMENTATION::SendCommand( WIDTH - 1 ); // Column end address (127 = reset)

			T_IMPLEMENTATION::SendCommand(SSD1306Const::SSD1306_SETMULTIPLEX);
			T_IMPLEMENTATION::SendCommand( HEIGHT - 1 );

			T_IMPLEMENTATION::SendCommand(SSD1306Const::SSD1306_PAGEADDR);
			T_IMPLEMENTATION::SendCommand(0); // Page start address (0 = reset)

			SendCommands( T_HEIGHT_IMPLEMENTATION::GetHeightInit(), T_HEIGHT_IMPLEMENTATION::GetHeightLength() );
*/
/*
			if( HEIGHT == 64 )
				T_IMPLEMENTATION::SendCommand(7); // Page end address

			if( HEIGHT == 32 )
				T_IMPLEMENTATION::SendCommand(3); // Page end address

			if( HEIGHT == 16 )
				T_IMPLEMENTATION::SendCommand(1); // Page end address



  T_IMPLEMENTATION::SendCommand(SSD1306Const::SSD1306_SETCOMPINS);                    // 0xDA
  T_IMPLEMENTATION::SendCommand(0x02);
  T_IMPLEMENTATION::SendCommand(SSD1306Const::SSD1306_SETCONTRAST);                   // 0x81
  T_IMPLEMENTATION::SendCommand(0x8F);
*/

			static const uint8_t InitCommands3[] =
			{
				SSD1306Const::SSD1306_SETVCOMDETECT,                 // 0xDB
				0x40,
				SSD1306Const::SSD1306_DISPLAYALLON_RESUME,           // 0xA4
				SSD1306Const::SSD1306_NORMALDISPLAY,                 // 0xA6
				SSD1306Const::SSD1306_DISPLAYON
			};

			SendCommands( ARRAY_PARAM( InitCommands3 ) );

			inherited::setTextSize( Text.Size );
			inherited::setTextColor( Text.Color, Text.BackgroundColor );
			inherited::setTextWrap( Text.Wrap );
			

//			FDisplay.begin( SSD1306Const::SSD1306_SWITCHCAPVCC, Address, false );
//			FValue = InitialValue;

//			drawPixel( 20, 20, WHITE );

			ClearScreen( *this, inherited::BackgroundColor );
			inherited::RenderElements();
//			for( int i = 0; i < FElements.size(); ++ i )
//				FElements[ i ]->Render( false );

			T_IMPLEMENTATION::display();
		}

	public:
		inline void RefreshInputPin_o_Receive( void * )
		{
			T_IMPLEMENTATION::display();
		}

	public:
		inline void SystemLoopEnd( unsigned long currentMicros )
		{
			T_IMPLEMENTATION::display();
		}

		inline void SystemInit()
		{
//			Serial.println( "Test1" );

			T_IMPLEMENTATION::IntSystemInitStart();
		}

		inline void SystemStart()
		{
			if( T_IMPLEMENTATION::PowerOn )
				DoPowerOn();

			T_IMPLEMENTATION::IntSystemInitEnd();
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

	public:
		DisplaySSD1306( const unsigned char * AFont ) :
			inherited( AFont )
//			Orientation( goUp ),
//			UseChargePump( false ),
//			PowerOn( true ),
//			FModified( true )
		{
		}

	};
//---------------------------------------------------------------------------
}

#ifdef __TEST_FOR_DEBUG_PRINTS__
#undef Serial
#endif

#endif
