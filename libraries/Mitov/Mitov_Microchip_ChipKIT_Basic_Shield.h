////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2018 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_MICROCHIP_CHIPKIT_BASIC_SHIELD_h
#define _MITOV_MICROCHIP_CHIPKIT_BASIC_SHIELD_h

#include <Mitov.h>

#ifdef __TEST_FOR_DEBUG_PRINTS__
#define Serial UNGUARDED DEBUG PRINT!!!
#endif

namespace Mitov
{
	template<int C_B0, int C_B1, int C_B2, int C_B3> class Microchip_ChipKITBasicShieldButtons
	{
	public:
		OpenWire::TypedSourcePin<bool>	OutputPins[ 4 ];

	public:
		inline void SystemLoopBegin( unsigned long currentMicros )
		{
	    	OutputPins[ 0 ].SetValue( digitalRead( C_B0 ) != LOW, true );
	    	OutputPins[ 1 ].SetValue( digitalRead( C_B1 ) != LOW, true );
	    	OutputPins[ 2 ].SetValue( digitalRead( C_B2 ) != LOW, true );
	    	OutputPins[ 3 ].SetValue( digitalRead( C_B3 ) != LOW, true );
		}

		inline void SystemStart()
		{
	    	OutputPins[ 0 ].SetValue( digitalRead( C_B0 ) != LOW, false );
	    	OutputPins[ 1 ].SetValue( digitalRead( C_B1 ) != LOW, false );
	    	OutputPins[ 2 ].SetValue( digitalRead( C_B2 ) != LOW, false );
	    	OutputPins[ 3 ].SetValue( digitalRead( C_B3 ) != LOW, false );
		}
	};
//---------------------------------------------------------------------------
	class Microchip_ChipKITBasicShieldLeds
	{
	public:
		inline void InputPins_o_Receive( int AIndex, void *_Data )
		{
			digitalWrite( AIndex, *(bool *)_Data );
		}

	public:
		inline void SystemStart()
		{
			for( int i = 26; i < 34; ++i )
			{
				pinMode( i, OUTPUT );
				digitalWrite( i, LOW );
			}
		}
	};
//---------------------------------------------------------------------------
}

#ifdef __TEST_FOR_DEBUG_PRINTS__
#undef Serial
#endif

#endif
