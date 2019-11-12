////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2018 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_NEO_PIXEL_h
#define _MITOV_NEO_PIXEL_h

#include <Mitov.h>
#include <Mitov_Graphics_RGBW.h>

#ifdef __VISUINO_USE_NEO_PIXELS_BUS_LIBRARY__
  #include <NeoPixelBrightnessBus.h>
#else
  #include <MitovEmbedded_Adafruit_NeoPixel/MitovEmbedded_Adafruit_NeoPixel.h>
#endif

#ifdef __TEST_FOR_DEBUG_PRINTS__
#define Serial UNGUARDED DEBUG PRINT!!!
#endif

namespace Mitov
{
//---------------------------------------------------------------------------
	template<typename T_IMPLEMENTATION, T_IMPLEMENTATION &C_IMPLEMENTATION> class NeoPixels
	{
	public:
		float   Brightness = 0.1f;

	public:
		bool	FModified = true;

	public:
		void SetBrightness( float AValue )
		{
			if( Brightness == AValue )
				return;

			IntSetBrightness( AValue );
		}

	public:
		void SetPixelColor( int AIndex, TRGBWColor AColor )
		{
//			Serial.print( "SetPixelColor: " );
//			Serial.print( AIndex );
//			Serial.print( " " );
//			Serial.print( AColor.ToString() );

#ifdef __VISUINO_USE_NEO_PIXELS_BUS_LIBRARY__
			RgbColor ARgbColor( AColor.Red, AColor.Green, AColor.Blue );
			C_IMPLEMENTATION.SetPixelColor( AIndex, ARgbColor );
#else
			C_IMPLEMENTATION.setPixelColor( AIndex, AColor.Red, AColor.Green, AColor.Blue, AColor.White );
#endif
//			C_IMPLEMENTATION.show();
//			delay( 200 );
			FModified = true;
		}

		TRGBWColor GetPixelColor( int AIndex )
		{
#ifdef __VISUINO_USE_NEO_PIXELS_BUS_LIBRARY__
			RgbColor ARgbColor = C_IMPLEMENTATION.GetPixelColor( AIndex );
			return TRGBWColor( ARgbColor.R, ARgbColor.G, ARgbColor.B, 0 );
#else
			return TRGBWColor( C_IMPLEMENTATION.getPixelColor( AIndex ), true );
#endif
		}

	protected:
		void IntSetBrightness( float AValue )
		{
			Brightness = AValue;
#ifdef __VISUINO_USE_NEO_PIXELS_BUS_LIBRARY__
			C_IMPLEMENTATION.SetBrightness( AValue * 255 );
#else
			C_IMPLEMENTATION.setBrightness( AValue * 255 );
#endif
			FModified = true;
		}

	public:
		inline void SystemInit()
		{
//			FPixel.setPin( C_PIN_NUMBER );

#ifdef __VISUINO_USE_NEO_PIXELS_BUS_LIBRARY__
			C_IMPLEMENTATION.Begin();
#else
			C_IMPLEMENTATION.begin();
#endif
			IntSetBrightness( Brightness );
		}

/*
		inline void SystemLoopBegin( unsigned long currentMicros )
		{
			for( int i = 0; i < PixelGroups.size(); ++i )
				PixelGroups[ i ]->PixelsClock( currentMicros );

		}
*/
		inline void SystemLoopEnd( unsigned long currentMicros )
		{
			ClockInputPin_o_Receive( nullptr );
		}

	public:
		void ClockInputPin_o_Receive(void *)
		{
			if( FModified )
			{
//				Serial.println( "TEST11" );
//				Serial.println( "TEST12" );
#ifdef __VISUINO_USE_NEO_PIXELS_BUS_LIBRARY__
				C_IMPLEMENTATION.Show();
#else
				C_IMPLEMENTATION.show();
#endif
				FModified = false;
			}
		}
	};
//---------------------------------------------------------------------------
	template<typename T_OWNER, T_OWNER &C_OWNER, uint16_t C_COUNT_PIXELS, uint16_t C_START_PIXEL> class NeoPixelsBasicInitialColorGroup
	{
	public:
		TRGBWColor	InitialColor;

	public:
		inline void SystemStart()
		{
			for( uint32_t i = 0; i < C_COUNT_PIXELS; ++i )
				C_OWNER.SetPixelColor( C_START_PIXEL + i, InitialColor );
		}

		inline void SystemLoopBegin( unsigned long currentMicros )	{} // Placeholder
	};
//---------------------------------------------------------------------------
	template<typename T_OWNER, T_OWNER &C_OWNER, uint16_t C_COUNT_PIXELS, uint16_t C_START_PIXEL> class NeoPixelsBasicColorGroup : public NeoPixelsBasicInitialColorGroup<T_OWNER, C_OWNER, C_COUNT_PIXELS, C_START_PIXEL>
	{
		typedef NeoPixelsBasicInitialColorGroup<T_OWNER, C_OWNER, C_COUNT_PIXELS, C_START_PIXEL> inherited;

	public:
		virtual void ApplyColorsAll()
		{
			for( uint32_t i = 0; i < C_COUNT_PIXELS; ++i )
				C_OWNER.SetPixelColor( C_START_PIXEL + i, inherited::InitialColor );
		}

	protected:
		virtual void ApplyColors() {}

		void IntSetColor( TRGBWColor AValue )
		{
			if( inherited::InitialColor == AValue )
				return;

			inherited::InitialColor = AValue;
			ApplyColors(); 
//			FOwner.FModified = true;
		}

	public:
		inline void ColorInputPin_o_Receive( void *_Data )
		{
			IntSetColor( *(TRGBWColor *)_Data );
		}

	public:
		inline void SystemStart()
		{
			inherited::SystemStart();
			IntSetColor( inherited::InitialColor );
		}

	public:
		using inherited::inherited;

	};
//---------------------------------------------------------------------------
	template<typename T_OWNER, T_OWNER &C_OWNER, uint16_t C_COUNT_PIXELS, uint16_t C_START_PIXEL> class NeoPixelsGroup : public NeoPixelsBasicInitialColorGroup<T_OWNER, C_OWNER, C_COUNT_PIXELS, C_START_PIXEL>
	{
		typedef NeoPixelsBasicInitialColorGroup<T_OWNER, C_OWNER, C_COUNT_PIXELS, C_START_PIXEL> inherited;

	protected:
		TRGBWColor	FValues[ C_COUNT_PIXELS ] = { 0 };

	public:
		void InputPins_o_Receive( int AIndex, void *_Data )
		{
			TRGBWColor AValue = *(TRGBWColor *)_Data;
			if( FValues[ AIndex ] == AValue )
				return;

			FValues[ AIndex ] = AValue;
			C_OWNER.SetPixelColor( AIndex, AValue );
		}

	public:
		using inherited::inherited;

	};
//---------------------------------------------------------------------------
	template<typename T_OWNER, T_OWNER &C_OWNER, uint16_t C_COUNT_PIXELS, uint16_t C_START_PIXEL, uint16_t C_REPEAT_COUNT, uint16_t C_SUB_PIXEL_COUNT> class NeoPixelsRepeatGroup : public NeoPixelsBasicInitialColorGroup<T_OWNER, C_OWNER, C_COUNT_PIXELS, C_START_PIXEL>
	{
		typedef NeoPixelsBasicInitialColorGroup<T_OWNER, C_OWNER, C_COUNT_PIXELS, C_START_PIXEL> inherited;

	public:
		void SetPixelColor( int AIndex, TRGBWColor AColor )
		{
			for( int i = 0; i < C_REPEAT_COUNT; ++i )
				C_OWNER.SetPixelColor( C_START_PIXEL + AIndex + i * C_SUB_PIXEL_COUNT, AColor );
		}

		TRGBWColor GetPixelColor( int AIndex )
		{
			return C_OWNER.GetPixelColor( C_START_PIXEL + AIndex );
		}

	public:
/*
		virtual void StartPixels( int &AStartPixel ) 
		{
			inherited::StartPixels( AStartPixel );

			FSubPixelCount = 0;
			for( int i = 0; i < PixelGroups.size(); ++i )
				PixelGroups[ i ]->StartPixels( FSubPixelCount );

			if( FSubPixelCount == 0 )
				FRepeatCount = 0;

			else
				FRepeatCount = ( CountPixels + FSubPixelCount - 1 ) / FSubPixelCount;

		}
*/
	public:
		using inherited::inherited;

	};
//---------------------------------------------------------------------------
	template<typename T_OWNER, T_OWNER &C_OWNER, uint16_t C_COUNT_PIXELS, uint16_t C_START_PIXEL> class NeoPixelsSingleColorGroup : public NeoPixelsBasicColorGroup<T_OWNER, C_OWNER, C_COUNT_PIXELS, C_START_PIXEL>
	{
		typedef NeoPixelsBasicColorGroup<T_OWNER, C_OWNER, C_COUNT_PIXELS, C_START_PIXEL> inherited;

	protected:
		virtual void ApplyColors() override
		{
			inherited::ApplyColorsAll();
		}

	public:
		using inherited::inherited;

	};
//---------------------------------------------------------------------------
	class NeoPixelsReversedProperty
	{
	public:
		bool Reversed : 1;
		bool AllPixels : 1;

	public:
		NeoPixelsReversedProperty() :
			Reversed( false ),
			AllPixels( false )
		{
		}
	};
//---------------------------------------------------------------------------
	template<typename T_OWNER, T_OWNER &C_OWNER, uint16_t C_COUNT_PIXELS, uint16_t C_START_PIXEL> class NeoPixelsRunningColorGroup : public NeoPixelsBasicColorGroup<T_OWNER, C_OWNER, C_COUNT_PIXELS, C_START_PIXEL>
	{
		typedef NeoPixelsBasicColorGroup<T_OWNER, C_OWNER, C_COUNT_PIXELS, C_START_PIXEL> inherited;

	public:
		bool	StepInputPin_o_IsConnected = false;

	public:
		OpenWire::SourcePin	ColorOutputPin;

	public:
		NeoPixelsReversedProperty	Reversed;

	public:
		inline void UpdateReversePixels()
		{
			if( Reversed.AllPixels )
				ReversePixels();

		}

	public:
		void ReversePixels()
		{
			for( uint32_t i = 0; i < C_COUNT_PIXELS / 2; ++i )
			{
				TRGBWColor AOldColor1 = C_OWNER.GetPixelColor( C_START_PIXEL + ( C_COUNT_PIXELS - i - 1 ));
				TRGBWColor AOldColor2 = C_OWNER.GetPixelColor( C_START_PIXEL + i );

				C_OWNER.SetPixelColor( C_START_PIXEL + i, AOldColor1 );
				C_OWNER.SetPixelColor( C_START_PIXEL + ( C_COUNT_PIXELS - i - 1 ), AOldColor2 );
			}
		}

	protected:
		void AnimatePixels()
		{
			if( Reversed.Reversed )
			{
				TRGBWColor AOldColor = C_OWNER.GetPixelColor( C_START_PIXEL );
				ColorOutputPin.Notify( &AOldColor );
				for( uint32_t i = 0; i < C_COUNT_PIXELS - 1; ++i )
				{
					AOldColor = C_OWNER.GetPixelColor( C_START_PIXEL + i + 1 );
					C_OWNER.SetPixelColor( C_START_PIXEL + i, AOldColor );
				}

				C_OWNER.SetPixelColor( C_START_PIXEL + C_COUNT_PIXELS - 1, inherited::InitialColor );
			}

			else
			{
				TRGBWColor AOldColor = C_OWNER.GetPixelColor( C_START_PIXEL + C_COUNT_PIXELS - 1 );
				ColorOutputPin.Notify( &AOldColor );
				for( int i = C_COUNT_PIXELS - 1; i--; )
				{
					AOldColor = C_OWNER.GetPixelColor( C_START_PIXEL + i );
					C_OWNER.SetPixelColor( C_START_PIXEL + i + 1, AOldColor );
				}

				C_OWNER.SetPixelColor( C_START_PIXEL, inherited::InitialColor );
			}
//			FOwner.FModified = true;
		}

	public:
		inline void StepInputPin_o_Receive( void *_Data )
		{
			AnimatePixels();
		}

	public:
		inline void SystemLoopBegin( unsigned long currentMicros )
		{
			if( StepInputPin_o_IsConnected )
				return;

			AnimatePixels();
		}

	public:
		using inherited::inherited;

	};
//---------------------------------------------------------------------------
	template<typename T_OWNER, T_OWNER &C_OWNER, uint16_t C_COUNT_PIXELS, uint16_t C_START_PIXEL> class NeoPixelsColorPixelGroup : public NeoPixelsBasicColorGroup<T_OWNER, C_OWNER, C_COUNT_PIXELS, C_START_PIXEL>
	{
		typedef NeoPixelsBasicColorGroup<T_OWNER, C_OWNER, C_COUNT_PIXELS, C_START_PIXEL> inherited;

	public:
		uint32_t InitialIndex = 0;

	protected:
		uint32_t	FIndex;
		bool		FModified = true;

	public:
		void IndexInputPin_o_Receive( void *_Data )
		{
			uint32_t AIndex = *(uint32_t *)_Data;
			if( AIndex >= C_COUNT_PIXELS )
				AIndex = C_COUNT_PIXELS - 1;

			if( FIndex == AIndex )
				return;

			FIndex = AIndex;
			FModified = true;
		}

	protected:
		virtual void ApplyColors() override
		{
			FModified = true;
		}

	public:
		inline void SystemLoopBegin( unsigned long currentMicros )
		{
			if( FModified )
			{
				C_OWNER.SetPixelColor( C_START_PIXEL + FIndex, inherited::InitialColor );
//				FOwner.FModified = true;
				FModified = false;
			}
		}

	public:
		using inherited::inherited;

	};
//---------------------------------------------------------------------------
	class TRGBWColorTransfrom
	{
	public:
		static const TUniColor BLACK = 0xFF00000000;

	public:
		inline static uint8_t	ExtractGray( TUniColor AColor )
		{
			return ( AColor & 0xFF );
		}

		inline static uint8_t	ExtractAlpha( TUniColor AColor )
		{
			return ( AColor >> 32 );
		}

		inline static TUniColor	ExtractColor( TUniColor AColor )
		{
			return ( AColor & 0xFFFFFFFF );
		}

		inline static uint32_t	MakeGray( TUniColor AColor )
		{
			uint32_t AGray = ( AColor & 0xFF );
			return ( 0xFF00000000 | ( AGray << 16 ) | ( AGray << 16 ) | ( AGray << 8 ) | AGray );
		}

		inline static TUniColor	MakeColor( TUniColor AColor )
		{
			uint64_t AAlpha = ( AColor & 0xFF000000 );
			return ( AAlpha << 8 ) | ( AColor & 0xFFFFFF );
		}

		inline static TUniColor	BlendColor( TUniColor AOldColor, TUniColor AColor, float AAlpha )
		{
			float ACoefficient = AAlpha / 255.0f;

			uint8_t AWhite = (( AOldColor >> 24 ) & 0xFF ) * ( 1 - ACoefficient ) + (( AColor >> 24 ) & 0xFF ) * ACoefficient + 0.5;
			uint8_t ARed = (( AOldColor >> 16 ) & 0xFF ) * ( 1 - ACoefficient ) + (( AColor >> 16 ) & 0xFF ) * ACoefficient + 0.5;
			uint8_t AGreen = (( AOldColor >> 8 ) & 0xFF ) * ( 1 - ACoefficient ) + (( AColor >> 8 ) & 0xFF ) * ACoefficient + 0.5;
			uint8_t ABlue = ( AOldColor & 0xFF ) * ( 1 - ACoefficient ) + ( AColor & 0xFF ) * ACoefficient + 0.5;

			return ( ( uint32_t( AWhite ) << 24 ) | ( uint32_t( ARed ) << 16 ) | ( uint32_t( AGreen ) << 8 ) | ABlue );
		}

		inline static TUniColor	TransformBackgroundColor( TUniColor AColor )
		{
			return AColor;
		}
	};
//---------------------------------------------------------------------------
	class TRGBColorTransfrom
	{
	public:
		static const TUniColor BLACK = 0xFF000000;

	public:
		inline static uint8_t	ExtractGray( TUniColor AColor )
		{
			return ( AColor & 0xFF );
		}

		inline static uint8_t	ExtractAlpha( TUniColor AColor )
		{
			return ( AColor >> 24 );
		}

		inline static TUniColor	ExtractColor( TUniColor AColor )
		{
			return ( AColor & 0xFFFFFF );
		}

		inline static uint32_t	MakeGray( TUniColor AColor )
		{
			uint16_t AGray = ( AColor & 0xFF );
			return ( 0xFF000000 | ( AColor << 16 ) | ( AGray << 8 ) | AGray );
		}

		inline static TUniColor	MakeColor( TUniColor AColor )
		{
			return AColor;
//			uint64_t AAlpha = ( AColor & 0xFF000000 );
//			return ( AAlpha << 8 ) | ( AColor & 0xFFFFFF );
		}

		inline static TUniColor	BlendColor( TUniColor AOldColor, TUniColor AColor, float AAlpha )
		{
			float ACoefficient = AAlpha / 255.0f;

			uint8_t ARed = (( AOldColor >> 16 ) & 0xFF ) * ( 1 - ACoefficient ) + (( AColor >> 16 ) & 0xFF ) * ACoefficient + 0.5;
			uint8_t AGreen = (( AOldColor >> 8 ) & 0xFF ) * ( 1 - ACoefficient ) + (( AColor >> 8 ) & 0xFF ) * ACoefficient + 0.5;
			uint8_t ABlue = ( AOldColor & 0xFF ) * ( 1 - ACoefficient ) + ( AColor & 0xFF ) * ACoefficient + 0.5;

			return ( ( uint32_t( ARed ) << 16 ) | ( uint32_t( AGreen ) << 8 ) | ABlue );
		}

		inline static TUniColor	TransformBackgroundColor( TUniColor AColor )
		{
			return (( AColor & 0xFF ) << 16 ) | ( AColor & 0x00FF00 ) | (( AColor >> 16 ) & 0xFF );
		}
	};
//---------------------------------------------------------------------------
	class TSequentialMatrixMap
	{
	public:
		inline static uint32_t	Remap( uint32_t AIndex )
		{
			return AIndex;
		}

	};
//---------------------------------------------------------------------------
//#if 0
	template<typename T_OWNER, T_OWNER &C_OWNER, typename T_COLOR_TRANSFORM, typename T_MATRIX_MAP, int WIDTH, int HEIGHT, uint16_t C_COUNT_PIXELS, uint16_t C_START_PIXEL> class NeoPixels2DGraphicsGroup : public GraphicsImpl<NeoPixels2DGraphicsGroup<T_OWNER, C_OWNER, T_COLOR_TRANSFORM, T_MATRIX_MAP, WIDTH, HEIGHT, C_COUNT_PIXELS, C_START_PIXEL>>
	{
		typedef NeoPixels2DGraphicsGroup<T_OWNER, C_OWNER, T_COLOR_TRANSFORM, T_MATRIX_MAP, WIDTH, HEIGHT, C_COUNT_PIXELS, C_START_PIXEL>	self;
		typedef GraphicsImpl<self>	graphics;

	public:
		GraphicsOrientation	Orientation = goUp;

	public:
		TGraphicsRGBWTextSettings	Text;

	public:
		int16_t width(void) const
		{
			if( Orientation == goUp || Orientation == goDown )
				return WIDTH;

			return HEIGHT;
		}

		int16_t height(void) const
		{
			if( Orientation == goUp || Orientation == goDown )
				return HEIGHT;

			return WIDTH;
		}

	public:
		inline T_OWNER &GetOwner() { return C_OWNER; }

	public:
		inline void SetBrightness( float AValue ) // Used for Owner Delegation
		{
			C_OWNER.SetBrightness( AValue );
		}

	public:
		inline void RefreshInputPin_o_Receive( void *_Data ) // Used for Owner Delegation
		{
			C_OWNER.ClockInputPin_o_Receive( _Data );
		}

	public:
		static inline void drawGrayscalePixel( self &AImplementation, int16_t x, int16_t y, TUniColor color)
		{
			AImplementation.drawPixel( x, y, T_COLOR_TRANSFORM::MakeGray( color ));
		}

		static inline void fillGrayscaleRect( self &AImplementation, int16_t x, int16_t y, int16_t w, int16_t h, TUniColor color)
		{
			fillGrayscaleRect( AImplementation, x, y, w, h, T_COLOR_TRANSFORM::MakeGray( color ));
		}

		static inline void drawColorPixel( self &AImplementation, int16_t x, int16_t y, TUniColor color )
		{
			AImplementation.drawPixel( x, y, T_COLOR_TRANSFORM::MakeColor( color ) );
		}

		static inline void fillColorRect( self &AImplementation, int16_t x, int16_t y, int16_t w, int16_t h, TUniColor color)
		{
//			Serial.println( "TEST88" );
			fillGrayscaleRect( AImplementation, x, y, w, h, T_COLOR_TRANSFORM::MakeColor( color ) );
		}

		void drawPixel( int16_t x, int16_t y, TUniColor color )
		{
//			Serial.println( "GRAPHICS" );
			if ((x < 0) || (x >= width() ) || (y < 0) || (y >= height() ))
				return;

//			Serial.println( uint32_t( color ), HEX );
//			Serial.print( "drawPixel: " );
//			color = 0xFFFFFF0000;
			uint8_t AAlpha = T_COLOR_TRANSFORM::ExtractAlpha( color );
			if( AAlpha == 0 )
				return;

//			Serial.print( x ); Serial.print( ":" ); Serial.print( y ); Serial.print( " = " ); Serial.print( uint32_t( AAlpha ), HEX ); Serial.print( " " ); Serial.println( uint32_t( color ), HEX ); 

			switch ( Orientation ) 
			{
				case goRight:
					swap(x, y);
					x = WIDTH - x - 1;
					break;

				case goDown:
					x = WIDTH - x - 1;
					y = HEIGHT - y - 1;
					break;

				case goLeft:
					swap(x, y);
					y = HEIGHT - y - 1;
					break;
			}  

			color = T_COLOR_TRANSFORM::ExtractColor( color );

//			Serial.print( "Color: " ); Serial.println( uint32_t( color ), HEX ); 

			uint32_t APixelIndex = C_START_PIXEL + T_MATRIX_MAP::Remap( x + y * WIDTH );

			if( AAlpha < 255 )
			{
				TUniColor AOldColor = C_OWNER.GetPixelColor( APixelIndex );
				color = T_COLOR_TRANSFORM::BlendColor( AOldColor, color, AAlpha );
			}

//			color = 0xFFFF0000;
//			Serial.print( C_START_PIXEL + x + y * WIDTH ); Serial.print( " = " ); Serial.println( uint32_t( color ));
			C_OWNER.SetPixelColor( APixelIndex, color );
//			FBuffer[ x + y * WIDTH ] = color;
//			FModified = true;
		}

		TUniColor GetPixelColor( int16_t x, int16_t y )
		{
			if ((x < 0) || (x >= width() ) || (y < 0) || (y >= height() ))
				return T_COLOR_TRANSFORM::BLACK;

			switch ( Orientation ) 
			{
				case goRight:
					swap(x, y);
					x = WIDTH - x - 1;
					break;

				case goDown:
					x = WIDTH - x - 1;
					y = HEIGHT - y - 1;
					break;

				case goLeft:
					swap(x, y);
					y = HEIGHT - y - 1;
					break;
			}  

			return TUniColor( C_OWNER.GetPixelColor( C_START_PIXEL + T_MATRIX_MAP::Remap( x + y * WIDTH ) )) | T_COLOR_TRANSFORM::BLACK;
//			return TUniColor( FBuffer[ x + y * WIDTH ] ) | 0xFF00000000;
		}

/*
	protected:
		inline void RenderElements()
		{
			graphics::RenderElements();
		}
*/
	public:
		inline void SystemStart()
		{
			graphics::setTextSize( Text.Size );
			graphics::setTextColor( Text.Color, Text.BackgroundColor );
			graphics::setTextWrap( Text.Wrap );

			graphics::ClearScreen( *this, T_COLOR_TRANSFORM::TransformBackgroundColor( graphics::BackgroundColor ) | T_COLOR_TRANSFORM::BLACK );
//			graphics::fillRect( *this, 0, 0, width(), height(), graphics::BackgroundColor | T_COLOR_TRANSFORM::BLACK );
//			Serial.println( "SystemStart" );
			graphics::RenderElements();
//			for( int i = 0; i < FElements.size(); ++ i )
//				FElements[ i ]->Render( false );
		}

		inline void SystemLoopBegin( unsigned long currentMicros )	{} // Placeholder
/*
		virtual void PixelsClock( unsigned long currentMicros )
		{
			if( FModified )
			{
				for( int i = 0; i < WIDTH * HEIGHT; ++i )
				{
//					Serial.print( i ); Serial.print( " = " ); Serial.println( FBuffer[ i ], HEX ); 
					C_OWNER.SetPixelColor( C_START_PIXEL + i, FBuffer[ i ] );
				}

				FModified = false;
			}
		}
*/
	public:
		inline static size_t write( self &AImplementation, uint8_t AValue )
		{
			return graphics::write( AImplementation, AValue );
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
		NeoPixels2DGraphicsGroup( const unsigned char * AFont ) :
			graphics( AFont )
		{
		}
		
	};
//---------------------------------------------------------------------------
}

#ifdef __TEST_FOR_DEBUG_PRINTS__
#undef Serial
#endif

#endif
