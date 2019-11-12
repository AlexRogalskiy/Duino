////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2018 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_MACETECH_RGBSHADES_MATRIX_h
#define _MITOV_MACETECH_RGBSHADES_MATRIX_h

#include <Mitov.h>

#ifdef __TEST_FOR_DEBUG_PRINTS__
#define Serial UNGUARDED DEBUG PRINT!!!
#endif

namespace Mitov
{
//---------------------------------------------------------------------------
	namespace MacetechRGBShadesNeoPixels
	{
		const	uint8_t Map[] =
		{
			68,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 69,
			29, 28, 27, 26, 25, 24, 23, 22, 21, 20, 19, 18, 17, 16, 15, 14,
			30, 31, 32, 33, 34, 35, 36, 70, 71, 37, 38, 39, 40, 41, 42, 43,
			57, 56, 55, 54, 53, 52, 51, 72, 73, 50, 49, 48, 47, 46, 45, 44,
			74, 58, 59, 60, 61, 62, 75, 76, 77, 78, 63, 64, 65, 66, 67, 79
		};
	}
//---------------------------------------------------------------------------
	class TRGBShadesMatrixMap
	{
	public:
		inline static uint32_t	Remap( uint32_t AIndex )
		{
			return MacetechRGBShadesNeoPixels::Map[ AIndex ];
		}

	};
//---------------------------------------------------------------------------
	template<typename T_PARENT> class ArduinoMacetechRGBShadesNeoPixelsModule : public T_PARENT
	{
	public:
		ArduinoMacetechRGBShadesNeoPixelsModule( float ABrightness, const unsigned char * AFont ) :
			T_PARENT( AFont )
		{
			T_PARENT::SetBrightness( ABrightness );
		}

	};
//---------------------------------------------------------------------------
}

#ifdef __TEST_FOR_DEBUG_PRINTS__
#undef Serial
#endif

#endif // _MITOV_MACETECH_RGBSHADES_MATRIX_h
