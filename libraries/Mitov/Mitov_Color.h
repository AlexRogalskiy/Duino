////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2018 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_COLOR_h
#define _MITOV_COLOR_h

#include <Mitov.h>
#include "Mitov_RandomGenerator.h"

#ifdef __TEST_FOR_DEBUG_PRINTS__
#define Serial UNGUARDED DEBUG PRINT!!!
#endif

namespace Mitov
{
#define Min Min
#define Max Max
//---------------------------------------------------------------------------
    class RandomColorImplementation
    {
	protected:
		template<typename T> void GenerateValue( T *AInherited )
		{
			AInherited->FValue.Red = random( AInherited->Min.Red, AInherited->Max.Red + 1 );
			AInherited->FValue.Green = random( AInherited->Min.Green, AInherited->Max.Green + 1 );
			AInherited->FValue.Blue = random( AInherited->Min.Blue, AInherited->Max.Blue + 1 );
		}
    };
//---------------------------------------------------------------------------
	class RandomColor : public CommonRandomGenerator<Mitov::TColor, RandomColorImplementation>
	{
		typedef CommonRandomGenerator<Mitov::TColor, RandomColorImplementation> inherited;

	public:
		inline void SystemInit()
		{
			int AMin = MitovMin( inherited::Min.Red, inherited::Max.Red );
			int AMax = MitovMax( inherited::Min.Red, inherited::Max.Red );

			inherited::Min.Red = AMin;
			inherited::Max.Red = AMax;

			AMin = MitovMin( inherited::Min.Green, inherited::Max.Green );
			AMax = MitovMax( inherited::Min.Green, inherited::Max.Green );

			inherited::Min.Green = AMin;
			inherited::Max.Green = AMax;

			AMin = MitovMin( inherited::Min.Blue, inherited::Max.Blue );
			AMax = MitovMax( inherited::Min.Blue, inherited::Max.Blue );

			inherited::Min.Blue = AMin;
			inherited::Max.Blue = AMax;
		}

	public:
		RandomColor() :
			inherited( 0, 0xFFFFFF )
		{
		}
	};
//---------------------------------------------------------------------------
	class RandomRGBWColorImplementation
    {
	protected:
		template<typename T> void GenerateValue( T *AInherited )
		{
			AInherited->FValue.Red = random( AInherited->Min.Red, AInherited->Max.Red + 1 );
			AInherited->FValue.Green = random( AInherited->Min.Green, AInherited->Max.Green + 1 );
			AInherited->FValue.Blue = random( AInherited->Min.Blue, AInherited->Max.Blue + 1 );
			AInherited->FValue.White = random( AInherited->Min.White, AInherited->Max.White + 1 );
		}
	};
//---------------------------------------------------------------------------
	class RandomRGBWColor : public Mitov::CommonRandomGenerator<Mitov::TRGBWColor, RandomColorImplementation>
    {
		typedef Mitov::CommonRandomGenerator<Mitov::TRGBWColor, RandomColorImplementation> inherited;

	public:
		inline void SystemInit()
		{
			int AMin = MitovMin( inherited::Min.Red, inherited::Max.Red );
			int AMax = MitovMax( inherited::Min.Red, inherited::Max.Red );

			inherited::Min.Red = AMin;
			inherited::Max.Red = AMax;

			AMin = MitovMin( inherited::Min.Green, inherited::Max.Green );
			AMax = MitovMax( inherited::Min.Green, inherited::Max.Green );

			inherited::Min.Green = AMin;
			inherited::Max.Green = AMax;

			AMin = MitovMin( inherited::Min.Blue, inherited::Max.Blue );
			AMax = MitovMax( inherited::Min.Blue, inherited::Max.Blue );

			inherited::Min.Blue = AMin;
			inherited::Max.Blue = AMax;

			AMin = MitovMin( inherited::Min.White, inherited::Max.White );
			AMax = MitovMax( inherited::Min.White, inherited::Max.White );

			inherited::Min.White = AMin;
			inherited::Max.White = AMax;
		}

	public:
		RandomRGBWColor() :
			inherited( 0, 0xFFFFFF )
		{
		}

    };
//---------------------------------------------------------------------------
	class AnalogToColor
	{
	public:
		OpenWire::SourcePin	OutputPin;

    public:
        TColor	InitialValue;

	public:
		void RedInputPin_o_Receive( void *_Data )
		{
			unsigned char AValue = (*(float *)_Data ) * 255;
			if( InitialValue.Red == AValue )
				return;

			InitialValue.Red = AValue;
			OutputPin.Notify( &InitialValue );
		}

		void GreenInputPin_o_Receive( void *_Data )
		{
			unsigned char AValue = (*(float *)_Data ) * 255;
			if( InitialValue.Green == AValue )
				return;

			InitialValue.Green = AValue;
			OutputPin.Notify( &InitialValue );
		}

		void BlueInputPin_o_Receive( void *_Data )
		{
			unsigned char AValue = (*(float *)_Data ) * 255;
			if( InitialValue.Blue == AValue )
				return;

			InitialValue.Blue = AValue;
			OutputPin.Notify( &InitialValue );
		}

	public:
		inline void SystemStart()
		{
			OutputPin.Notify( &InitialValue );			
		}

	};
//---------------------------------------------------------------------------
	class AnalogToRGBWColor
	{
	public:
		OpenWire::SourcePin	OutputPin;

    public:
        TRGBWColor	InitialValue;

	public:
		void RedInputPin_o_Receive( void *_Data )
		{
			unsigned char AValue = (*(float *)_Data ) * 255;
			if( InitialValue.Red == AValue )
				return;

			InitialValue.Red = AValue;
			OutputPin.Notify( &InitialValue );
		}

		void GreenInputPin_o_Receive( void *_Data )
		{
			unsigned char AValue = (*(float *)_Data ) * 255;
			if( InitialValue.Green == AValue )
				return;

			InitialValue.Green = AValue;
			OutputPin.Notify( &InitialValue );
		}

		void BlueInputPin_o_Receive( void *_Data )
		{
			unsigned char AValue = (*(float *)_Data ) * 255;
			if( InitialValue.Blue == AValue )
				return;

			InitialValue.Blue = AValue;
			OutputPin.Notify( &InitialValue );
		}

		void WhiteInputPin_o_Receive( void *_Data )
		{
			unsigned char AValue = (*(float *)_Data ) * 255;
			if( InitialValue.White == AValue )
				return;

			InitialValue.White = AValue;
			OutputPin.Notify( &InitialValue );
		}

	public:
		inline void SystemStart()
		{
			OutputPin.Notify( &InitialValue );			
		}

	};
//---------------------------------------------------------------------------
	class ColorToAnalog
	{
	public:
		OpenWire::SourcePin	RedOutputPin;
		OpenWire::SourcePin	GreenOutputPin;
		OpenWire::SourcePin	BlueOutputPin;

	public:
		void InputPin_o_Receive( void *_Data )
		{
			TColor &AColor = *(TColor *)_Data;

			RedOutputPin.SendValue<float>( float( AColor.Red ) / 255 );
			GreenOutputPin.SendValue<float>( float( AColor.Green ) / 255 );
			BlueOutputPin.SendValue<float>( float( AColor.Blue ) / 255 );
		}

	};	
//---------------------------------------------------------------------------
	class RGBWColorToAnalog
	{
	public:
		OpenWire::SourcePin	RedOutputPin;
		OpenWire::SourcePin	GreenOutputPin;
		OpenWire::SourcePin	BlueOutputPin;
		OpenWire::SourcePin	WhiteOutputPin;

	public:
		void InputPin_o_Receive( void *_Data )
		{
			TRGBWColor &AColor = *(TRGBWColor *)_Data;

			RedOutputPin.SendValue<float>( float( AColor.Red ) / 255 );
			GreenOutputPin.SendValue<float>( float( AColor.Green ) / 255 );
			BlueOutputPin.SendValue<float>( float( AColor.Blue ) / 255 );
			WhiteOutputPin.SendValue<float>( float( AColor.White ) / 255 );
		}

	};	
//---------------------------------------------------------------------------
#undef Min
#undef Max
}

#ifdef __TEST_FOR_DEBUG_PRINTS__
#undef Serial
#endif

#endif
