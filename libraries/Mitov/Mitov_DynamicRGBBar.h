////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2018 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_DYNAMIC_RGB_BAR_h
#define _MITOV_DYNAMIC_RGB_BAR_h

#include <Mitov.h>

namespace Mitov
{
	template<int C_NUM_LEDS> class MitovDynamicRGBBar
	{
	public:
		OpenWire::SourcePin	PixelOutputPins[ C_NUM_LEDS ];
		OpenWire::SourcePin	ColorOutputPins[ 3 ];

	public:
		inline void InputPins_o_Receive( int AIndex, void *_Data )
		{
			FColorValues[ AIndex ] = *(TColor *)_Data;
		}

	public:
		void UpdateInverted()
		{
			for( int i = 0; i < C_NUM_LEDS; ++i )
				PixelOutputPins[ i ].SendValue( Inverted );

			PixelOutputPins[ FPixel ].SendValue( ! Inverted );
		}

	public:
		unsigned long	RefreshInterval = 1000;
		bool			Inverted = false;

	protected:
		unsigned long	FLastTime = 0;
		uint8_t			FPixel = 0;

		Mitov::TColor	FColorValues[ C_NUM_LEDS ];

	public:
		inline void SystemLoopBegin( unsigned long currentMicros )
		{
			if( ( currentMicros - FLastTime ) < RefreshInterval )
				return;

			PixelOutputPins[ FPixel ].SendValue( Inverted );

			++FPixel;

			if( FPixel >= C_NUM_LEDS )
				FPixel = 0;

			Mitov::TColor AColor = FColorValues[ FPixel ];

			ColorOutputPins[ 0 ].SendValue<float>( float( AColor.Red ) / 255 );
			ColorOutputPins[ 1 ].SendValue<float>( float( AColor.Green ) / 255 );
			ColorOutputPins[ 2 ].SendValue<float>( float( AColor.Blue ) / 255 );

			PixelOutputPins[ FPixel ].SendValue( ! Inverted );
			FLastTime = currentMicros;
//			UpdateDisplay();
		}

		inline void SystemInit()
		{
			for( int i = 0; i < C_NUM_LEDS; ++i )
				PixelOutputPins[ i ].SendValue( Inverted );

			FLastTime = micros();
		}

	public:
		MitovDynamicRGBBar( Mitov::TColor AInitialColor ) :
			FPixel( C_NUM_LEDS - 1 )
		{			
			for( int i = 0; i < C_NUM_LEDS; ++i )
				FColorValues[ i ] = AInitialColor;
		}

	};
}

#endif
