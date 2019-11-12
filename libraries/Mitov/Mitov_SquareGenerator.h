////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2018 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_SQUARE_GENERATOR_h
#define _MITOV_SQUARE_GENERATOR_h

#include <Mitov.h>
#include "Mitov_BasicGenerator.h"

namespace Mitov
{
	class TypedSquareGenerator
	{
	public:
		// -1 - 1
		float	Asymmetry = 0.0f;

	protected:
		template<typename T> void CalculateValue( T *AInherited )
		{
			if( AInherited->FPhase < 0.5 + Asymmetry / 2 )
				AInherited->FValue = AInherited->Offset - AInherited->Amplitude;

			else
				AInherited->FValue = AInherited->Offset + AInherited->Amplitude;

		}
	};
//---------------------------------------------------------------------------
}

#endif
