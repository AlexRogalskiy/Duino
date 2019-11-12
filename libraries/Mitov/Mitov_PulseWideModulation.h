////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2018 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_PULSE_WIDE_MODULATION_h
#define _MITOV_PULSE_WIDE_MODULATION_h

#include <Mitov.h>

namespace Mitov
{
	class PulseWideModulation
	{
	public:
		OpenWire::SourcePin	OutputPin;

	public:
		float Period;

	protected:
		float FValue = 0.0f;

	public:
		inline void InputPin_o_Receive( void *_Data )
		{
			FValue = ((OpenWire::ValueData<float>*)&_Data)->Value;
/*
			if(((OpenWire::ValueData<bool>*)&_Data)->Value )
				digitalWrite( PinNumber, HIGH );

			else
				digitalWrite( PinNumber, LOW );
*/
		}

	public:
		PulseWideModulation( float _period = 20 ) :
			Period( _period )
		{
		}

	};
}

#endif
