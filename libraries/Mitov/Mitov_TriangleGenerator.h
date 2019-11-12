////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2018 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_TRIANGLE_GENERATOR_h
#define _MITOV_TRIANGLE_GENERATOR_h

#include <Mitov.h>
#include "Mitov_BasicGenerator.h"

namespace Mitov
{
//---------------------------------------------------------------------------
	class TriangleAnalogGenerator
	{
	public:
		// -1 - 1
		float	Asymmetry = 0.0f;

	protected:
		template<typename T> void CalculateValue( T *AInherited )
		{
			float AAssymetryPount = 0.5 + Asymmetry / 2;
			if( AInherited->FPhase < AAssymetryPount )
				AInherited->FValue = AInherited->Offset - AInherited->Amplitude + AInherited->Amplitude * 2 * ( AInherited->FPhase / AAssymetryPount );

			else
				AInherited->FValue = AInherited->Offset - AInherited->Amplitude + AInherited->Amplitude * 2 * ( 1 - (( AInherited->FPhase - AAssymetryPount ) / ( 1 - AAssymetryPount )));

		}

	};
//---------------------------------------------------------------------------
	class TriangleIntegerGenerator
	{
	public:
		// -1 - 1
		float	Asymmetry = 0.0f;

	protected:
		template<typename T> void CalculateValue( T *AInherited )
		{
			float AAssymetryPount = 0.5 + Asymmetry / 2;
			if( AInherited->FPhase < AAssymetryPount )
				AInherited->FValue = ( AInherited->Offset - AInherited->Amplitude + float( AInherited->Amplitude ) * 2 * ( AInherited->FPhase / AAssymetryPount ) ) + 0.5;

			else
				AInherited->FValue = ( AInherited->Offset - AInherited->Amplitude + float( AInherited->Amplitude ) * 2 * ( 1 - (( AInherited->FPhase - AAssymetryPount ) / ( 1 - AAssymetryPount ))) ) + 0.5;

		}
	};
//---------------------------------------------------------------------------
}

#endif
