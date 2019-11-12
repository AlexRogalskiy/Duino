////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2018 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_CUSTOM_CODE_h
#define _MITOV_CUSTOM_CODE_h

#include <Mitov.h>

namespace Mitov
{
//---------------------------------------------------------------------------
	class CustomClockOutput
	{
	public:
		OpenWire::SourcePin	OutputPin;

	public:
		void Clock()
		{
			OutputPin.Notify( nullptr );
		}
	};
//---------------------------------------------------------------------------
	template<typename T_DATA> class CustomOutput
	{
	public:
		OpenWire::SourcePin	OutputPin;

	public:
		void Send( T_DATA AData )
		{
			OutputPin.Notify( &AData );
		}
	};
//---------------------------------------------------------------------------
}

#endif