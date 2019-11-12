////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2018 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_CIRCUIT_PLAYGROUND_RGB_LEDS_h
#define _MITOV_CIRCUIT_PLAYGROUND_RGB_LEDS_h

#include <Mitov.h>
#include <MitovEmbedded_Adafruit_NeoPixel/MitovEmbedded_Adafruit_NeoPixel.h>

#ifdef __TEST_FOR_DEBUG_PRINTS__
#define Serial UNGUARDED DEBUG PRINT!!!
#endif

namespace Mitov
{
//---------------------------------------------------------------------------
	class ArduinoCircuitPlaygroundRGBLedsModule
	{
	public:
		float   Brightness = 1.0f;

	public:
		MitovEmbedded_Adafruit_NeoPixel	FPixel;
		bool	FModified = true;

	public:
		void SetBrightness( float AValue )
		{
			if( Brightness == AValue )
				return;

			IntSetBrightness( AValue );
		}

	public:
		void SetPixelColor( int AIndex, TColor AColor )
		{
//			Serial.println( AColor.ToString() );
			FPixel.setPixelColor( AIndex, AColor.Red, AColor.Green, AColor.Blue );
//			FPixel.setPixelColor( AIndex, 255, 0, 0 );
			FModified = true;
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

	public:
		inline void SystemInit()
		{
			FPixel.setPin( 17 );

			FPixel.begin();
			FPixel.updateType(NEO_GRB + NEO_KHZ800);
			FPixel.updateLength(10);
			IntSetBrightness( Brightness );
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

	};
//---------------------------------------------------------------------------
	class ArduinoCircuitPlaygroundRGBLed
	{
	public:
		TColor	InitialValue;

	protected:
		ArduinoCircuitPlaygroundRGBLedsModule &FOwner;
		uint8_t	FIndex;

	public:
		void ColorInputPin_o_Receive( void *_Data )
		{
			TColor &AColor = *(TColor *)_Data;
			if( AColor == InitialValue )
				return;

			InitialValue = AColor;
			FOwner.SetPixelColor( FIndex, InitialValue );

//			digitalWrite( 5, ( *(bool*)_Data ) ? HIGH : LOW );
		}

	public:
		void SystemStart()
		{
			FOwner.SetPixelColor( FIndex, InitialValue );
		}

	public:
		ArduinoCircuitPlaygroundRGBLed( ArduinoCircuitPlaygroundRGBLedsModule &AOwner, uint8_t AIndex ) :
			FOwner( AOwner ),
			FIndex( AIndex )
		{
//			pinMode( 5, OUTPUT );
		}
	};
//---------------------------------------------------------------------------
}

#ifdef __TEST_FOR_DEBUG_PRINTS__
#undef Serial
#endif

#endif // _MITOV_CIRCUIT_PLAYGROUND_RGB_LEDS_h
