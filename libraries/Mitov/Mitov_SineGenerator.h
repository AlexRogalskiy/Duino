////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2018 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_SINE_GENERATOR_h
#define _MITOV_SINE_GENERATOR_h

#include <Mitov.h>
#include "Mitov_BasicGenerator.h"

namespace Mitov
{
//---------------------------------------------------------------------------
	class SineAnalogGenerator
	{
	public:
		template<typename T> void CalculateValue( T *AInherited )
		{
			AInherited->FValue = sin( AInherited->FPhase * 2 * PI ) * AInherited->Amplitude + AInherited->Offset;
		}

	};
//---------------------------------------------------------------------------
	class SineIntegerGenerator
	{
	protected:
		template<typename T> void CalculateValue( T *AInherited )
		{
			AInherited->FValue = sin( AInherited->FPhase * 2 * PI ) * AInherited->Amplitude + AInherited->Offset + 0.5;
		}

	};
//---------------------------------------------------------------------------
}

#endif
