////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2018 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_BUTTON_h
#define _MITOV_BUTTON_h

#include <Mitov.h>

#ifdef __TEST_FOR_DEBUG_PRINTS__
#define Serial UNGUARDED DEBUG PRINT!!!
#endif

namespace Mitov
{
	class Button
	{
	public:
		OpenWire::SourcePin	OutputPin;

	protected:
		unsigned long	FLastTime = 0;
		bool	FLastValue : 1;
		bool	FValue : 1;

	public:
		uint32_t	DebounceInterval = 50;

	public:
		void InputPin_o_Receive( void *_Data )
		{
			bool AValue = *( bool *)_Data;
			if( AValue != FLastValue )
				FLastTime = millis();

			FLastValue = AValue;
		}

	public:
		inline void SystemLoopBegin( unsigned long currentMicros )
		{
			if( FValue != FLastValue )
				if( millis() - FLastTime > DebounceInterval )
				{
					FValue = FLastValue;
					OutputPin.SendValue( FValue );
				}

		}

		inline void SystemStart()
		{
			FValue = FLastValue;
			OutputPin.SendValue( FValue );
		}

	public:
		Button() :
			FLastValue( false ),
			FValue( false )
		{
		}

	};
}

#ifdef __TEST_FOR_DEBUG_PRINTS__
#undef Serial
#endif

#endif
