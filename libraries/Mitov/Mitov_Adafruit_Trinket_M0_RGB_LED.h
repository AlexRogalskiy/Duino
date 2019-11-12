////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2018 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_ADAFRUIT_TRINKET_M0_RGB_LEDS_h
#define _MITOV_ADAFRUIT_TRINKET_M0_RGB_LEDS_h

#include <Mitov.h>
#include <Adafruit_DotStar.h>

#ifdef __TEST_FOR_DEBUG_PRINTS__
#define Serial UNGUARDED DEBUG PRINT!!!
#endif

namespace Mitov
{
//---------------------------------------------------------------------------
	class ArduinoAdafruitTrinketM0_RGBLedModule
	{
	public:
		TColor	InitialValue;
		float   Brightness = 1.0f;

	public:
		Adafruit_DotStar	FPixel;
		bool	FModified = true;

	public:
		void SetBrightness( float AValue )
		{
			if( Brightness == AValue )
				return;

			IntSetBrightness( AValue );
		}

	protected:
		void IntSetBrightness( float AValue )
		{
			Brightness = AValue;
			FPixel.setBrightness( AValue * 255 );
			FModified = true;
		}

	public:
		void ClockInputPin_o_Receive( void *_Data )
		{
			if( FModified )
			{
				FPixel.show();
				FModified = false;
			}
		}

		void ColorInputPin_o_Receive( void *_Data )
		{
			TColor &AColor = *(TColor *)_Data;
			if( AColor == InitialValue )
				return;

			InitialValue = AColor;
			FPixel.setPixelColor( 0, InitialValue.Red, InitialValue.Green, InitialValue.Blue );
			FModified = true;
		}

	public:
		inline void SystemInit()
		{
//			FPixel.setPin( 17 );

			FPixel.begin();
//			FPixel.updateType(NEO_GRB + NEO_KHZ800);
//			FPixel.updateLength(10);
			IntSetBrightness( Brightness );
			FPixel.setPixelColor( 0, InitialValue.Red, InitialValue.Green, InitialValue.Blue );
			FPixel.show();

		}

		inline void SystemLoopEnd( unsigned long currentMicros )
		{
			if( FModified )
			{
				FPixel.show();
				FModified = false;
			}
		}

	public:
		ArduinoAdafruitTrinketM0_RGBLedModule() :
			FPixel( 1, 7, 8, DOTSTAR_BGR )
		{
		}
	};
//---------------------------------------------------------------------------
}

#ifdef __TEST_FOR_DEBUG_PRINTS__
#undef Serial
#endif

#endif // _MITOV_ADAFRUIT_TRINKET_M0_RGB_LEDS_h
