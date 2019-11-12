////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2018 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_DISPLAY_IS31FL3731_h
#define _MITOV_DISPLAY_IS31FL3731_h

#include <Mitov.h>
#include <Mitov_Graphics_Grayscale.h>
#include <Mitov_Graphics_Monochrome.h>

namespace Mitov
{
//---------------------------------------------------------------------------
	template<typename T_OWNER> class DisplayIS31FL3731AnimationElement;
//	template<typename T_OWNER> class DisplayIS31FL3731BasicPictureElement;
//---------------------------------------------------------------------------
	template<typename T_OWNER> class DisplayIS31FL3731BasicElement
	{
	public:
		T_OWNER	&FOwner;

	public:
		bool	Enabled = true;

	public:
		virtual void AssignFrames( uint8_t &AFrame ) {}
		virtual void SystemInitStart( bool &AStarted ) {}
		virtual void Refresh() {}

	public:
		DisplayIS31FL3731BasicElement( T_OWNER &AOwner ) :
			FOwner( AOwner )
		{
			AOwner.FElements.push_back( this );
		}

	};
//---------------------------------------------------------------------------
/*
	class TArduinoLedDriverIS31FL3731Blink
	{
	public:
		bool	Enabled : 1;
		uint8_t	Period : 3;

	public:
		TArduinoLedDriverIS31FL3731Blink() :
			Enabled( false ),
			Period( 0 )
		{
		}
	};
*/
//---------------------------------------------------------------------------
	enum TArduinoLedDriverIS31FL3731AudioGainMode { igmDisabled, igmSlow, igmFast };
	enum TArduinoLedDriverIS31FL3731AudioGain { isg0dB, isg3dB, isg6dB, isg9dB, isg12dB, isg15dB, isg18dB, isg21dB };
//---------------------------------------------------------------------------
	class TArduinoLedDriverIS31FL3731AudioModulation
	{
	public:
		bool	Enabled : 1;
		TArduinoLedDriverIS31FL3731AudioGainMode	GainMode : 2;
		TArduinoLedDriverIS31FL3731AudioGain Gain : 3;

	public:
		TArduinoLedDriverIS31FL3731AudioModulation() :
			Enabled( false ),
			GainMode( igmDisabled ),
			Gain( isg0dB )
		{
		}
	};
//---------------------------------------------------------------------------
	namespace IS31FL3731Const
	{
		const uint8_t WIDTH = 16;
		const uint8_t HEIGHT = 9;

		const uint8_t	REG_CONFIG  = 0x00;
		const uint8_t	REG_CONFIG_PICTUREMODE = 0x00;
		const uint8_t	REG_CONFIG_AUTOPLAYMODE = 0x08;
		const uint8_t	REG_CONFIG_AUDIOPLAYMODE = 0x10;

		const uint8_t	REG_PICTUREFRAME = 0x01;

		const uint8_t	REG_SHUTDOWN = 0x0A;

		const uint8_t	COMMANDREGISTER = 0xFD;
		const uint8_t	AUTO_PLAY_CONTROL_REGISTER1 = 0x02;
		const uint8_t	AUTO_PLAY_CONTROL_REGISTER2 = 0x03;
		const uint8_t	DISPLAY_OPTION_REGISTER = 0x05;
		const uint8_t	AUDIO_SYNCHRONIZATION_REGISTER = 0x06;
		const uint8_t	BREATH_CONTROL_REGISTER1 = 0x08;
		const uint8_t	BREATH_CONTROL_REGISTER2 = 0x09;
		const uint8_t	AGC_CONTROL_REGISTER = 0x0B;
		const uint8_t	AUDIO_ADC_RATE_REGISTER = 0x0C;
		const uint8_t	BANK_FUNCTIONREG = 0x0B;    // helpfully called 'page nine'
	}
//---------------------------------------------------------------------------
	template<uint8_t C_ADDRESS, typename T_I2C, T_I2C &C_I2C> class DisplayIS31FL3731
	{
	public:
        bool Enabled : 1;
		uint8_t	BlinkPeriod : 3;

	public:
		TArduinoLedDriverIS31FL3731AudioModulation	AudioModulation;
//		TArduinoLedDriverIS31FL3731Blink	Blink;

	public:
		Mitov::SimpleList<DisplayIS31FL3731BasicElement<DisplayIS31FL3731<C_ADDRESS, T_I2C, C_I2C>> *>	FElements;

	public:
		static inline void SelectBank( uint8_t b ) 
		{
			I2C::WriteByte( C_I2C, C_ADDRESS, IS31FL3731Const::COMMANDREGISTER, b );
		}

		static void WriteRegister8(uint8_t b, uint8_t reg, uint8_t data)
		{
			SelectBank( b );
			I2C::WriteByte( C_I2C, C_ADDRESS, reg, data );
		}

		static inline void I2C_write( uint8_t AValue )
		{
			C_I2C.write( AValue );
		}

		static inline void I2C_beginTransmission()
		{
			C_I2C.beginTransmission( C_ADDRESS );
		}

		static inline void I2C_endTransmission()
		{
			C_I2C.endTransmission();
		}
		
	public:
		void UpdateAssignFrames()
		{
			uint8_t AFrame = 0;
			for( int i = 0; i < FElements.size(); ++i )
				FElements[ i ]->AssignFrames( AFrame );
		}


		void ReassignFrames( DisplayIS31FL3731BasicElement<DisplayIS31FL3731<C_ADDRESS, T_I2C, C_I2C>> *AStartElement )
		{
			int AStartIndex = FElements.IndexOf( AStartElement );

			uint8_t AFrame = 0;
			int ASize = FElements.size();
			for( int i = 0; i < ASize; ++i )
				FElements[ ( i + AStartIndex ) % ASize ]->AssignFrames( AFrame );

		}

		void UpdateAudioModulation()
		{
			uint8_t AData;

			if( AudioModulation.Enabled )
				AData = ( 0b10 | (( AudioModulation.GainMode == igmFast ) ? 0b100 : 0 ) | AudioModulation.Gain );

			else
				AData = 0;

			WriteRegister8( IS31FL3731Const::BANK_FUNCTIONREG, IS31FL3731Const::AGC_CONTROL_REGISTER, AData );
			WriteRegister8( IS31FL3731Const::BANK_FUNCTIONREG, IS31FL3731Const::AUDIO_SYNCHRONIZATION_REGISTER, AudioModulation.Enabled ? 1 : 0 );
		}

/*
		void UpdateBlink()
		{
//			return;
			uint8_t	AData = 0b100000 | (( Blink.Enabled ) ? 0b1000 : 0 ) | Blink.Period;
			WriteRegister8( IS31FL3731Const::BANK_FUNCTIONREG, IS31FL3731Const::DISPLAY_OPTION_REGISTER, AData );
		}
*/
	public:
		inline void SystemLoopEnd( unsigned long currentMicros )
		{
			for( int i = 0; i < FElements.size(); ++i )
				FElements[ i ]->Refresh();

		}

		inline void SystemInit()
		{
			// shutdown
			WriteRegister8( IS31FL3731Const::BANK_FUNCTIONREG, IS31FL3731Const::REG_SHUTDOWN, 0x00 );

			if( ! Enabled )
				return;

			delay(10);

			// out of shutdown
			WriteRegister8( IS31FL3731Const::BANK_FUNCTIONREG, IS31FL3731Const::REG_SHUTDOWN, 0x01 );




//			uint8_t	AData = 0b100000 | (( Blink.Enabled ) ? 0b1000 : 0 ) | Blink.Period;
//			uint8_t	AData = 0b100000 | 0b1000;
			uint8_t	AData = 0b1000 | BlinkPeriod;
			WriteRegister8( IS31FL3731Const::BANK_FUNCTIONREG, IS31FL3731Const::DISPLAY_OPTION_REGISTER, AData );

/*
			SelectBank( 0 );

			FWire.beginTransmission( Address );

			FWire.write( 0x12 );
			FWire.write( 2 );
			for( int i = 1; i < 16; ++i)
				FWire.write( 0 );

			FWire.endTransmission();
*/

			UpdateAssignFrames();
//			UpdateBlink();
			UpdateAudioModulation();

//			UpdateFadeEffect();
//			display();
		}

		inline void SystemStart()
		{
			bool AStarted = false;
			for( int i = 0; i < FElements.size(); ++i )
				FElements[ i ]->SystemInitStart( AStarted );
				
			for (uint8_t f=0; f<8; f++)
				for (uint8_t i=0; i<=0x11; i++)
					WriteRegister8(f, i, 0xff);     // each 8 LEDs on

		}

	public:
		DisplayIS31FL3731() :
			Enabled( true ),
			BlinkPeriod( 2 )
		{
		}
	};
//---------------------------------------------------------------------------
	class TArduinoLedDriverIS31FL3731FadeEffect
	{
	public:
		bool	Enabled : 1;
		uint8_t	FadeIn : 3;
		uint8_t	FadeOut : 3;
		uint8_t	ExtinguishTime : 3;

	public:
		TArduinoLedDriverIS31FL3731FadeEffect() :
			Enabled( false ),
			FadeIn( 0 ),
			FadeOut( 0 ),
			ExtinguishTime( 0 )
		{
		}

	};
//---------------------------------------------------------------------------
	template<typename T_OWNER> class DisplayIS31FL3731BasicBlink
	{
	public:
		bool	Enabled = false;

	public:
		virtual void PopulateReg()
		{
			if( Enabled )
			{
				for( int i = 0; i < 18; ++i)
					T_OWNER::I2C_write( 0xFF );

			}

			else
			{
				for( int i = 0; i < 18; ++i)
					T_OWNER::I2C_write( 0 );

			}
		}

		virtual void SetFrame()
		{
		}

	public:
		DisplayIS31FL3731BasicBlink( T_OWNER &AOwner, void * )
		{		
			AOwner.FBlink = this;
		}

	};
//---------------------------------------------------------------------------
	template<typename T_OWNER> class DisplayIS31FL3731Blink : public DisplayIS31FL3731BasicBlink<T_OWNER>, public GraphicsImpl<DisplayIS31FL3731Blink<T_OWNER>>
	{
		typedef DisplayIS31FL3731BasicBlink<T_OWNER> inherited;
		typedef DisplayIS31FL3731Blink<T_OWNER> self;
		typedef GraphicsImpl<self> graphics;

	public:
		TGraphicsMonochromeTextSettings	Text;

	public:
		GraphicsOrientation	Orientation : 2; //= goUp;

	protected:
		bool	FModified : 1; // = true;

	public:
		uint8_t	FPicture[ IS31FL3731Const::WIDTH * ( IS31FL3731Const::HEIGHT + 7 ) / 8 ];

//	protected:
//		DisplayIS31FL3731BasicPictureElement &FOwner

	public:
		virtual void PopulateReg() override
		{
			if( inherited::Enabled )
			{
				for( int i = 0; i < 18; ++i)
					T_OWNER::I2C_write( FPicture[ i ] );

			}

			else
			{
				for( int i = 0; i < 18; ++i)
					T_OWNER::I2C_write( 0 );

			}
		}

		virtual void SetFrame() override
		{
			graphics::setTextSize( Text.Size );
			graphics::setTextColor( Text.Color, Text.BackgroundColor );
			graphics::setTextWrap( Text.Wrap );

			graphics::RenderElements();
//			for( int i = 0; i < FElements.size(); ++ i )
//				FElements[ i ]->Render( false );
		}

	public:
		inline int16_t width(void) const
		{
			if( Orientation == goUp || Orientation == goDown )
				return IS31FL3731Const::WIDTH;

			return IS31FL3731Const::HEIGHT;
		}

		inline int16_t height(void) const
		{
			if( Orientation == goUp || Orientation == goDown )
				return IS31FL3731Const::HEIGHT;

			return IS31FL3731Const::WIDTH;
		}

	public:
		void drawPixel( int16_t x, int16_t y, TUniColor color )
		{
			if( color == tmcNone )
				return;

			if ((x < 0) || (x >= width() ) || (y < 0) || (y >= height() ))
				return;

			switch ( Orientation ) 
			{
				case goRight:
					swap(x, y);
					x = IS31FL3731Const::WIDTH - x - 1;
					break;

				case goDown:
					x = IS31FL3731Const::WIDTH - x - 1;
					y = IS31FL3731Const::HEIGHT - y - 1;
					break;

				case goLeft:
					swap(x, y);
					y = IS31FL3731Const::HEIGHT - y - 1;
					break;
			}

			// x is which column
			switch (color) 
			{
				case tmcWhite :		FPicture[(x/8) + y * ( IS31FL3731Const::WIDTH / 8 ) ] |=  (1 << (x&7)); break;
				case tmcBlack :		FPicture[(x/8) + y * ( IS31FL3731Const::WIDTH / 8 ) ] &= ~(1 << (x&7)); break; 
				case tmcInvert :	FPicture[(x/8) + y * ( IS31FL3731Const::WIDTH / 8 ) ] ^=  (1 << (x&7)); break; 
			}

			FModified = true;
		}

		TUniColor GetPixelColor( int16_t x, int16_t y )
		{
			if ((x < 0) || (x >= width() ) || (y < 0) || (y >= height() ))
				return false;

			switch ( Orientation ) 
			{
				case goRight:
					swap(x, y);
					x = IS31FL3731Const::WIDTH - x - 1;
					break;

				case goDown:
					x = IS31FL3731Const::WIDTH - x - 1;
					y = IS31FL3731Const::HEIGHT - y - 1;
					break;

				case goLeft:
					swap(x, y);
					y = IS31FL3731Const::HEIGHT - y - 1;
					break;
			}

			return (( FPicture[ ( x / 8 ) + y * ( IS31FL3731Const::WIDTH / 8 ) ] & ( 1 << ( x & 0b111 ))) != 0 );
		}

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
		DisplayIS31FL3731Blink( T_OWNER &AOwner,  const unsigned char * AFont ) :
			inherited( AOwner, nullptr ),
			graphics( AFont ),
			Orientation( goUp ),
			FModified( true )
		{
			memset( FPicture, 0, sizeof( FPicture ));
		}
	};
//---------------------------------------------------------------------------
	template<typename T_OWNER, typename T_INSTANCE> class DisplayIS31FL3731BasicPictureElement : public GraphicsGrayscale<DisplayIS31FL3731BasicPictureElement<T_OWNER, T_INSTANCE>>
	{
		typedef DisplayIS31FL3731BasicPictureElement<T_OWNER, T_INSTANCE> self;
		typedef GraphicsGrayscale<self> inherited;

	public:
		TGraphicsGrayscaleTextSettings	Text;

	protected:
		uint8_t	FPicture[ IS31FL3731Const::WIDTH * IS31FL3731Const::HEIGHT ];

	public:
		GraphicsOrientation	Orientation : 2; //= goUp;

	protected:
		bool	FModified : 1; //= false;

	public:
		DisplayIS31FL3731BasicBlink<T_INSTANCE> *FBlink = nullptr;

	public:
		static inline void I2C_write( uint8_t AValue )
		{
			T_OWNER::I2C_write( AValue );
		}

	public:
		inline int16_t width(void) const
		{
			if( Orientation == goUp || Orientation == goDown )
				return IS31FL3731Const::WIDTH;

			return IS31FL3731Const::HEIGHT;
		}

		inline int16_t height(void) const
		{
			if( Orientation == goUp || Orientation == goDown )
				return IS31FL3731Const::HEIGHT;

			return IS31FL3731Const::WIDTH;
		}

	public:
		void drawPixel( int16_t x, int16_t y, TUniColor color )
		{
			if ((x < 0) || (x >= width() ) || (y < 0) || (y >= height() ))
				return;

//			Serial.println( color );
			uint8_t AAlpha = color >> 8;
			if( AAlpha == 0 )
				return;

			switch ( Orientation ) 
			{
				case goRight:
					swap(x, y);
					x = IS31FL3731Const::WIDTH - x - 1;
					break;

				case goDown:
					x = IS31FL3731Const::WIDTH - x - 1;
					y = IS31FL3731Const::HEIGHT - y - 1;
					break;

				case goLeft:
					swap(x, y);
					y = IS31FL3731Const::HEIGHT - y - 1;
					break;
			}  

			// charlie wing is smaller
			//if (x > 15) return;
			//if (y > 7) return;

/*
			if (x > 7) {
			x=15-x;
			y += 8;
			} else {
			y = 7-y;
			}

			_swap_int16_t(x, y);

 
			if ((x < 0) || (x >= 16)) return;
			if ((y < 0) || (y >= 9)) return;
			if (color > 255) color = 255; // PWM 8bit max
*/
//			setLEDPWM(x + y*16, color, _frame);
//			T_OWNER::WriteRegister8( ( FFrame ) ? 0 : 1 , 0x24 + x + y * IS31FL3731Const::WIDTH, color );

			color &= 0xFF;
//			Serial.println( AAlpha );

			if( AAlpha < 255 )
			{
				float ACoefficient = float( AAlpha ) / 255.0f;
				color = (color * ACoefficient + FPicture[ x + y * IS31FL3731Const::WIDTH ] * ( 1 - ACoefficient )) + 0.5;
			}
//				color = MitovMax( color , FPicture[ x + y * IS31FL3731Const::WIDTH ] );

//			Serial.println( "Color" );
//			Serial.println( color );
			FPicture[ x + y * IS31FL3731Const::WIDTH ] = color;
			FModified = true;
			return;
		}

		TUniColor GetPixelColor( int16_t x, int16_t y )
		{
			if ((x < 0) || (x >= width() ) || (y < 0) || (y >= height() ))
				return 0xFF00;

			switch ( Orientation ) 
			{
				case goRight:
					swap(x, y);
					x = IS31FL3731Const::WIDTH - x - 1;
					break;

				case goDown:
					x = IS31FL3731Const::WIDTH - x - 1;
					y = IS31FL3731Const::HEIGHT - y - 1;
					break;

				case goLeft:
					swap(x, y);
					y = IS31FL3731Const::HEIGHT - y - 1;
					break;
			}  

			return FPicture[ x + y * IS31FL3731Const::WIDTH ] | 0xFF00;
		}

	protected:
		void RenderImage()
		{
			uint8_t *AData = FPicture;
			for( int y = 0; y < IS31FL3731Const::HEIGHT; ++y )
			{
				T_OWNER::I2C_beginTransmission();
				T_OWNER::I2C_write( 0x24 + y * IS31FL3731Const::WIDTH );
				for( int x = 0; x < IS31FL3731Const::WIDTH; ++x )
					T_OWNER::I2C_write( *AData ++ );

				T_OWNER::I2C_endTransmission();
			}

			T_OWNER::I2C_beginTransmission();
			T_OWNER::I2C_write( 0x12 );
			bool APopulate = ( FBlink != nullptr );
			if( APopulate )
				APopulate = FBlink->Enabled;

			if( APopulate )
				FBlink->PopulateReg();

			else
			{
				for( int i = 0; i < 18; ++i)
					T_OWNER::I2C_write( 0 );

			}

			T_OWNER::I2C_endTransmission();
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
		DisplayIS31FL3731BasicPictureElement( const unsigned char * AFont ) :
			inherited( AFont ),
			Orientation( goUp ),
			FModified( false )
		{
			memset( FPicture, 0, sizeof( FPicture ));
		}

	};
//---------------------------------------------------------------------------
	template<typename T_OWNER> class DisplayIS31FL3731FrameElement : public DisplayIS31FL3731BasicPictureElement<T_OWNER, DisplayIS31FL3731FrameElement<T_OWNER>>
	{
		typedef DisplayIS31FL3731BasicPictureElement<T_OWNER, DisplayIS31FL3731FrameElement<T_OWNER>> inherited;

	protected:
		uint8_t	FFrame;

	public:
		void SetFrame( uint8_t	AFrame )
		{
			inherited::setTextSize( inherited::Text.Size );
			inherited::setTextColor( inherited::Text.Color, inherited::Text.BackgroundColor );
			inherited::setTextWrap( inherited::Text.Wrap );

			inherited::RenderElements();
//			for( int i = 0; i < inherited::FElements.size(); ++ i )
//				inherited::FElements[ i ]->Render( false );

			if( inherited::FBlink )
				inherited::FBlink->SetFrame();

			FFrame = AFrame;
			Render();
		}

		void Render()
		{
//			Serial.println( "display" );
//			Serial.println( ( FFrame ) ? FFrame0 : FFrame1 );
//			T_OWNER::SelectBank( IS31FL3731Const::BANK_FUNCTIONREG );
			T_OWNER::SelectBank( FFrame );
			inherited::RenderImage();

//			T_OWNER::WriteRegister8( IS31FL3731Const::BANK_FUNCTIONREG, IS31FL3731Const::REG_PICTUREFRAME, ( FFrame ) ? FFrame0 : FFrame1 );
			inherited::FModified = false;
		}

//	public:
//		void RefreshInputPin_o_Receive( void * )
//		{
//			if( FModified )
//				Render();
//		}

	public:
		DisplayIS31FL3731FrameElement( T_OWNER &AOwner, const unsigned char * AFont ) :
			inherited( AFont )
		{
			AOwner.FFrames.push_back( this );
		}
	};
//---------------------------------------------------------------------------
	class TArduinoLedDriverIS31FL3731AudioControl
	{
	public:
		bool	Enabled : 1;
		uint16_t	SamplingInterval : 14;

	public:
		uint8_t GetRateValue()
		{
			uint16_t AValue = float( SamplingInterval ) / 64 + 0.5;
			return AValue;
		}

	public:
		TArduinoLedDriverIS31FL3731AudioControl() :
			Enabled( false ),
			SamplingInterval( 16383 )
		{
		}
	};
//---------------------------------------------------------------------------
	template<typename T_OWNER> class DisplayIS31FL3731AnimationElement : public DisplayIS31FL3731BasicElement<T_OWNER>
	{
		typedef DisplayIS31FL3731BasicElement<T_OWNER> inherited;

	public:
		TArduinoLedDriverIS31FL3731AudioControl	AudioControl;
		TArduinoLedDriverIS31FL3731FadeEffect	FadeEffect;
		uint8_t	Loops : 3;
		uint8_t	FrameDelay : 6;

	public:
		bool    ClockInputPin_o_IsConnected : 1;

	protected:
		bool    FFrameAssigned : 1;
		uint8_t	FFrameStart : 3;

	public:
		Mitov::SimpleList<DisplayIS31FL3731FrameElement<DisplayIS31FL3731AnimationElement<T_OWNER>> *>	FFrames;

	public:
		static inline void SelectBank( uint8_t b ) 
		{
			T_OWNER::SelectBank( b );
		}

		static inline void I2C_write( uint8_t AValue )
		{
			T_OWNER::I2C_write( AValue );
		}

		static inline void I2C_beginTransmission()
		{
			T_OWNER::I2C_beginTransmission();
		}

		static inline void I2C_endTransmission()
		{
			T_OWNER::I2C_endTransmission();
		}
		
	public:
		void UpdateFadeEffect()
		{
			uint8_t	AData = ( FadeEffect.FadeOut << 4 ) | FadeEffect.FadeIn;
			T_OWNER::WriteRegister8( IS31FL3731Const::BANK_FUNCTIONREG, IS31FL3731Const::BREATH_CONTROL_REGISTER1, AData );

			AData = ( inherited::Enabled ? 0b10000 : 0 ) | FadeEffect.ExtinguishTime;
			T_OWNER::WriteRegister8( IS31FL3731Const::BANK_FUNCTIONREG, IS31FL3731Const::BREATH_CONTROL_REGISTER2, AData );
		}

		void UpdateLoop()
		{
			if( AudioControl.Enabled )
				T_OWNER::WriteRegister8( IS31FL3731Const::BANK_FUNCTIONREG, IS31FL3731Const::AUDIO_ADC_RATE_REGISTER, AudioControl.GetRateValue() );


			uint8_t	AData = ( ( AudioControl.Enabled ) ? IS31FL3731Const::REG_CONFIG_AUDIOPLAYMODE : IS31FL3731Const::REG_CONFIG_AUTOPLAYMODE ) | FFrameStart;

			T_OWNER::WriteRegister8( IS31FL3731Const::BANK_FUNCTIONREG, IS31FL3731Const::REG_CONFIG, AData );

			AData = ( Loops << 4 ) | ( FFrames.size() & 0b111 );
			T_OWNER::WriteRegister8( IS31FL3731Const::BANK_FUNCTIONREG, IS31FL3731Const::AUTO_PLAY_CONTROL_REGISTER1, AData );

			T_OWNER::WriteRegister8( IS31FL3731Const::BANK_FUNCTIONREG, IS31FL3731Const::AUTO_PLAY_CONTROL_REGISTER2, FrameDelay );
		}

	public:
		virtual void SystemInitStart( bool &AStarted ) override
		{
			if( ! inherited::Enabled )
				return;

			for( int i = 0; i < FFrames.size(); ++ i )
				FFrames[ i ]->SetFrame( FFrameStart + i );

			if( AStarted )
				return;

			if( ClockInputPin_o_IsConnected )
				return;

			AStarted = true;

			ClockInputPin_o_Receive( nullptr );
		}

		virtual void AssignFrames( uint8_t &AFrame ) override
		{
			if( AFrame > 7 - MitovMin<uint8_t>( 7, FFrames.size() ) )
			{
				FFrameAssigned = false;
				return;
			}

			FFrameAssigned = true;
			FFrameStart = AFrame;
			AFrame += FFrames.size();
		}

	public:
		void ClockInputPin_o_Receive( void *_Data )
		{
			if( ! inherited::Enabled )
				return;

			if( ! FFrameAssigned )
				inherited::FOwner.ReassignFrames( this );

			UpdateFadeEffect();
			UpdateLoop();
			// picture mode
		}

	public:
		DisplayIS31FL3731AnimationElement( T_OWNER &AOwner ) :
			inherited( AOwner ),
			Loops( 0 ),
			FrameDelay( 0 ),
			ClockInputPin_o_IsConnected( false ),
			FFrameAssigned( false )
		{
		}
	};
//---------------------------------------------------------------------------
	template<typename T_OWNER> class DisplayIS31FL3731PictureElement : public DisplayIS31FL3731BasicElement<T_OWNER>, public DisplayIS31FL3731BasicPictureElement<T_OWNER, DisplayIS31FL3731PictureElement<T_OWNER>>
	{
		typedef DisplayIS31FL3731BasicElement<T_OWNER> inherited;

	public:
		TArduinoLedDriverIS31FL3731FadeEffect	FadeEffect;

	public:
		bool	ClockInputPin_o_IsConnected : 1;
		bool	RefreshInputPin_o_IsConnected : 1;

	protected:
		bool    FFrameAssigned : 1;
		bool	FFrame : 1;
		uint8_t	FFrame0 : 3;
		uint8_t	FFrame1 : 3;

	public:
		void UpdateFadeEffect()
		{
			uint8_t	AData = ( FadeEffect.FadeOut << 4 ) | FadeEffect.FadeIn;
			T_OWNER::WriteRegister8( IS31FL3731Const::BANK_FUNCTIONREG, IS31FL3731Const::BREATH_CONTROL_REGISTER1, AData );

			AData = ( inherited::Enabled ? 0b10000 : 0 ) | FadeEffect.ExtinguishTime;
			T_OWNER::WriteRegister8( IS31FL3731Const::BANK_FUNCTIONREG, IS31FL3731Const::BREATH_CONTROL_REGISTER2, AData );
		}

	public:
		virtual void AssignFrames( uint8_t &AFrame ) override
		{
			if( AFrame > 7 - 2 )
			{
				FFrameAssigned = false;
				return;
			}

			FFrameAssigned = true;
			FFrame0 = AFrame ++;
			FFrame1 = AFrame ++;
		}

	protected:
		void display()
		{
//			Serial.println( "display" );
//			Serial.println( ( FFrame ) ? FFrame0 : FFrame1 );
//			T_OWNER::SelectBank( IS31FL3731Const::BANK_FUNCTIONREG );
			T_OWNER::SelectBank( ( FFrame ) ? FFrame0 : FFrame1 );
			DisplayIS31FL3731BasicPictureElement<T_OWNER, DisplayIS31FL3731PictureElement<T_OWNER>>::RenderImage();

			T_OWNER::WriteRegister8( IS31FL3731Const::BANK_FUNCTIONREG, IS31FL3731Const::REG_PICTUREFRAME, ( FFrame ) ? FFrame0 : FFrame1 );
			FFrame = !FFrame;
			DisplayIS31FL3731BasicPictureElement<T_OWNER, DisplayIS31FL3731PictureElement<T_OWNER>>::FModified = false;
		}

	public:
		virtual void SystemInitStart( bool &AStarted ) override
		{
			for( int i = 0; i < IS31FL3731Const::HEIGHT * IS31FL3731Const::WIDTH; ++i )
				DisplayIS31FL3731BasicPictureElement<T_OWNER, DisplayIS31FL3731PictureElement<T_OWNER>>::FPicture[ i ] = DisplayIS31FL3731BasicPictureElement<T_OWNER, DisplayIS31FL3731PictureElement<T_OWNER>>::BackgroundColor;

			if( ! inherited::Enabled )
				return;

			DisplayIS31FL3731BasicPictureElement<T_OWNER, DisplayIS31FL3731PictureElement<T_OWNER>>::setTextSize( DisplayIS31FL3731BasicPictureElement<T_OWNER, DisplayIS31FL3731PictureElement<T_OWNER>>::Text.Size );
			DisplayIS31FL3731BasicPictureElement<T_OWNER, DisplayIS31FL3731PictureElement<T_OWNER>>::setTextColor( DisplayIS31FL3731BasicPictureElement<T_OWNER, DisplayIS31FL3731PictureElement<T_OWNER>>::Text.Color, DisplayIS31FL3731BasicPictureElement<T_OWNER, DisplayIS31FL3731PictureElement<T_OWNER>>::Text.BackgroundColor );
			DisplayIS31FL3731BasicPictureElement<T_OWNER, DisplayIS31FL3731PictureElement<T_OWNER>>::setTextWrap( DisplayIS31FL3731BasicPictureElement<T_OWNER, DisplayIS31FL3731PictureElement<T_OWNER>>::Text.Wrap );

			DisplayIS31FL3731BasicPictureElement<T_OWNER, DisplayIS31FL3731PictureElement<T_OWNER>>::RenderElements();
//			for( int i = 0; i < DisplayIS31FL3731BasicPictureElement<T_OWNER>::FElements.size(); ++ i )
//				DisplayIS31FL3731BasicPictureElement<T_OWNER>::FElements[ i ]->Render( false );

			if( DisplayIS31FL3731BasicPictureElement<T_OWNER, DisplayIS31FL3731PictureElement<T_OWNER>>::FBlink )
				DisplayIS31FL3731BasicPictureElement<T_OWNER, DisplayIS31FL3731PictureElement<T_OWNER>>::FBlink->SetFrame();

			if( AStarted )
				return;

			if( ClockInputPin_o_IsConnected )
				return;

			AStarted = true;

			ClockInputPin_o_Receive( nullptr );
		}

		virtual void Refresh() override
		{
			if( DisplayIS31FL3731BasicPictureElement<T_OWNER, DisplayIS31FL3731PictureElement<T_OWNER>>::FModified )
				if( ! RefreshInputPin_o_IsConnected )
					display();
		}

	public:
		void RefreshInputPin_o_Receive( void * )
		{
			if( DisplayIS31FL3731BasicPictureElement<T_OWNER, DisplayIS31FL3731PictureElement<T_OWNER>>::FModified )
				display();
		}

		void ClockInputPin_o_Receive( void *_Data )
		{
			if( ! inherited::Enabled )
				return;

			// picture mode
			T_OWNER::WriteRegister8( IS31FL3731Const::BANK_FUNCTIONREG, IS31FL3731Const::REG_CONFIG, IS31FL3731Const::REG_CONFIG_PICTUREMODE );
			UpdateFadeEffect();
			display();
		}

	public:
		DisplayIS31FL3731PictureElement( T_OWNER &AOwner, const unsigned char * AFont ) :
			inherited( AOwner ),
			DisplayIS31FL3731BasicPictureElement<T_OWNER, DisplayIS31FL3731PictureElement<T_OWNER>>( AFont ),
			ClockInputPin_o_IsConnected( false ),
			RefreshInputPin_o_IsConnected( false ),
			FFrameAssigned( false ),
			FFrame( false )
		{
		}

	};
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
/*
	DisplayIS31FL3731BasicElement::DisplayIS31FL3731BasicElement( T_OWNER &AOwner ) :
		FOwner( AOwner )
	{
		AOwner.FElements.push_back( this );
	}
//---------------------------------------------------------------------------
	DisplayIS31FL3731FrameElement::DisplayIS31FL3731FrameElement( DisplayIS31FL3731AnimationElement &AOwner, const unsigned char * AFont ) :
		inherited( AOwner.FOwner.FWire, AFont ),
		FOwner( AOwner.FOwner )
	{
		AOwner.FFrames.push_back( this );
	}
//---------------------------------------------------------------------------
	DisplayIS31FL3731BasicBlink::DisplayIS31FL3731BasicBlink( DisplayIS31FL3731BasicPictureElement &AOwner, void * ) :
		FWire( AOwner.FWire )
	{		
		AOwner.FBlink = this;
	}
*/
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
}

#endif
