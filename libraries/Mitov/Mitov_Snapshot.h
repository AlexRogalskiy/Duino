////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2018 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_SNAPSHOT_h
#define _MITOV_SNAPSHOT_h

#include <Mitov.h>

namespace Mitov
{
//---------------------------------------------------------------------------
	template<typename T_DATA> class Snapshot
	{
	public:
		OpenWire::SourcePin	OutputPin;

    public:
        bool Enabled = true;

	protected:
		T_DATA FData;

	public:
		inline void InputPin_o_Receive( void *_Data )
		{
//			FData = *(T_DATA*)_Data;
			FData = ::Mitov::template ExtractValue<T_DATA>( _Data );
		}

		void SnapshotInputPin_o_Receive( void *_Data )
		{
			if( Enabled )
				TakeSnapshot();
		}

	protected:
		inline void TakeSnapshot()
		{
			OutputPin.SendValue( FData );
		}

	public:
		Snapshot( T_DATA AData ) : 
			FData( AData )
		{
		}
	};
//---------------------------------------------------------------------------
	class DigitalSnapshot
	{
	public:
		OpenWire::SourcePin	OutputPin;

    public:
        bool Enabled	: 1;

	protected:
		bool FData		: 1;

	public:
		inline void InputPin_o_Receive( void *_Data )
		{
			FData = *(bool*)_Data;
		}

		void SnapshotInputPin_o_Receive( void *_Data )
		{
			if( Enabled )
				TakeSnapshot();
		}

	protected:
		inline void TakeSnapshot()
		{
			OutputPin.SendValue( FData );
		}

	public:
		DigitalSnapshot() : 
			Enabled( true ),
			FData( false )
		{
		}
	};
//---------------------------------------------------------------------------
}

#endif
