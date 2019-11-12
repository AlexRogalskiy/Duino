////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2018 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_REMEMBER_h
#define _MITOV_REMEMBER_h

#include <Mitov.h>

namespace Mitov
{
//---------------------------------------------------------------------------
	template<typename T_STORE, typename T_DATA> class Remember
	{
	public:
		OpenWire::SourcePin	OutputPin;

    public:
        bool Enabled : 1;

    public:
		bool RecallInputPin_o_IsConnected : 1;

	protected:
		bool FPopulated : 1;
		bool FForceRemember : 1;
		T_STORE FSavedData;
		T_STORE FLastData;	

	public:
		void InputPin_o_Receive( void *_Data )
		{
			T_STORE AValue = *(T_DATA *)_Data;
			FLastData = AValue;
			FPopulated = true;

			if( FForceRemember )
			{
				FSavedData = FLastData;
#ifdef _MITOV_REMEMBER_RECALL_PIN_
				if( ! RecallInputPin_o_IsConnected )
#endif // _MITOV_REMEMBER_RECALL_PIN_
					OutputPin.SendValue( FSavedData );
			}

			FForceRemember = false;
		}

		void RememberInputPin_o_Receive( void *_Data )
		{
			if( Enabled )
			{
				if( ! FPopulated )
				{
					FForceRemember = true;
					return;
				}

				FSavedData = FLastData;
#ifdef _MITOV_REMEMBER_RECALL_PIN_
				if( ! RecallInputPin_o_IsConnected )
#endif // _MITOV_REMEMBER_RECALL_PIN_
					OutputPin.SendValue( FSavedData );
			}
		}

#ifdef _MITOV_REMEMBER_RECALL_PIN_
		void RecallInputPin_o_Receive( void *_Data )
		{
			if( Enabled )
				OutputPin.SendValue( FSavedData );
		}
#endif // _MITOV_REMEMBER_RECALL_PIN_

	public:
		Remember( T_STORE AData ) : 
			Enabled( true ),
			RecallInputPin_o_IsConnected( false ),
			FPopulated( false ),
			FForceRemember( false ),
			FSavedData( AData )
		{
		}
	};
//---------------------------------------------------------------------------
	template<> void Remember<Mitov::String, char>::InputPin_o_Receive( void *_Data )
	{
		Mitov::String AValue = (char *)_Data;
		FLastData = AValue;
		FPopulated = true;

		if( FForceRemember )
		{
			FSavedData = FLastData;
#ifdef _MITOV_REMEMBER_RECALL_PIN_
			if( ! RecallInputPin_o_IsConnected )
#endif // _MITOV_REMEMBER_RECALL_PIN_
				OutputPin.SendValue( FSavedData );
		}

		FForceRemember = false;
	}
//---------------------------------------------------------------------------
}

#endif
