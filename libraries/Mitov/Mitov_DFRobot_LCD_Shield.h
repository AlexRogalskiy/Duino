////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2018 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_DFROBOT_LCD_SHIELD_h
#define _MITOV_DFROBOT_LCD_SHIELD_h

#include <Mitov.h>

#ifdef __TEST_FOR_DEBUG_PRINTS__
#define Serial UNGUARDED DEBUG PRINT!!!
#endif

namespace Mitov
{
	template<int C_NAVKEYMOVE_RIGHT, int C_NAVKEYMOVE_UP, int C_NAVKEYMOVE_DOWN, int C_NAVKEYMOVE_LEFT, int C_NAVKEYMOVE_SELECT> class ArduinoDFRobotLCDKeypadKeys
	{
	public:
		OpenWire::TypedSourcePin<bool>	DirectionOutputPins[ 4 ];
		OpenWire::TypedSourcePin<bool>	SelectOutputPin;
//		OpenWire::TypedSourcePin<float>	AnalogOutputPin;

	protected:
		void UpdatePins( bool AChangeOnly )
		{
			int AAnalogValue = analogRead( 0 );
//	    	AnalogOutputPin.SetValue( AAnalogValue / 1023.0, AChangeOnly );

			bool AUp = false;
			bool ADown = false;
			bool ALeft = false;
			bool ARight = false;
			bool ASelect = false;

			if( AAnalogValue < C_NAVKEYMOVE_RIGHT )
				ARight = true;

			else if( AAnalogValue < C_NAVKEYMOVE_UP )
				AUp = true;

			else if( AAnalogValue < C_NAVKEYMOVE_DOWN )
				ADown = true;

			else if( AAnalogValue < C_NAVKEYMOVE_LEFT )
				ALeft = true;

			else if( AAnalogValue < C_NAVKEYMOVE_SELECT )
				ASelect = true;

			SelectOutputPin.SetValue( ASelect, AChangeOnly );
			DirectionOutputPins[ 0 ].SetValue( AUp, AChangeOnly );
			DirectionOutputPins[ 1 ].SetValue( ADown, AChangeOnly );
			DirectionOutputPins[ 2 ].SetValue( ALeft, AChangeOnly );
			DirectionOutputPins[ 3 ].SetValue( ARight, AChangeOnly );
		}

	public:
		inline void SystemLoopBegin( unsigned long currentMicros )
		{
			UpdatePins( true );
		}

		inline void SystemStart()
		{
			UpdatePins( false );
		}
	};
//---------------------------------------------------------------------------
}

#ifdef __TEST_FOR_DEBUG_PRINTS__
#undef Serial
#endif

#endif
