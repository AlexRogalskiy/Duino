////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2018 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_SWITCH_h
#define _MITOV_SWITCH_h

#include <Mitov.h>

namespace Mitov
{
	template<typename T_PIN> class CommonSwitch
	{
	public:
		OpenWire::SourcePin	OutputPin;

	public:
		bool	InitialEnabled : 1;
		bool	IgnoreSame : 1;

	protected:
		bool	FPopulated : 1;
		bool	FOutputStarted : 1;
		T_PIN	FOutputValue;
		T_PIN	FValue;

	protected:
		void SendOutput()
		{
			if( InitialEnabled )
			{
				if( IgnoreSame )
				{
					if( ! FOutputStarted )
						FOutputStarted = true;

					else
						if( FOutputValue == FValue )
							return;

				}

				FOutputStarted = true;
				FOutputValue = FValue;

				OutputPin.SendValue( FOutputValue );
			}
		}

	public:
		void EnableInputPin_o_Receive( void *_Data )
		{
			bool AValue = *(bool *)_Data;
			if( InitialEnabled == AValue )
				return;

			InitialEnabled = AValue;
			if( ! FPopulated )
				return;

			SendOutput();
		}

		void InputPin_o_Receive( void *_Data )
		{
//			FValue = *(T_PIN *)_Data;
			FValue = ::Mitov::template ExtractValue<T_PIN>( _Data );
			FPopulated = true;
			SendOutput();
		}

	public:
		CommonSwitch( T_PIN AValue ) :
			InitialEnabled( false ),
			IgnoreSame( true ),
			FPopulated( false ),
			FValue( AValue )
		{
		}

	};
//---------------------------------------------------------------------------
/*
	template<> void CommonSwitch<Mitov::String>::InputPin_o_Receive( void *_Data )
	{
			FValue = (char *)_Data;
			FPopulated = true;
			SendOutput();
	}
*/
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
	class DigitalSwitch
	{
	public:
		OpenWire::SourcePin	OutputPin;

	public:
		bool	InitialEnabled : 1;
		bool	IgnoreSame : 1;

	protected:
		bool	FPopulated : 1;
		bool	FOutputStarted : 1;
		bool	FOutputValue : 1;
		bool	FValue : 1;

	protected:
		void SendOutput()
		{
			if( InitialEnabled )
			{
				if( IgnoreSame )
				{
					if( ! FOutputStarted )
						FOutputStarted = true;

					else
						if( FOutputValue == FValue )
							return;

				}

				FOutputStarted = true;
				FOutputValue = FValue;

				OutputPin.SendValue( FOutputValue );
			}
		}

	public:
		void EnableInputPin_o_Receive( void *_Data )
		{
			bool AValue = *(bool *)_Data;
			if( InitialEnabled == AValue )
				return;

			InitialEnabled = AValue;
			if( ! FPopulated )
				return;

			SendOutput();
		}

		void InputPin_o_Receive( void *_Data )
		{
			FValue = *(bool *)_Data;
			FPopulated = true;
			SendOutput();
		}

	public:
		DigitalSwitch() :
			InitialEnabled( false ),
			IgnoreSame( true ),
			FPopulated( false ),
			FValue( false )
		{
		}

	};
//---------------------------------------------------------------------------
	class ClockSwitch
	{
	public:
		OpenWire::SourcePin	OutputPin;

	public:
		bool	InitialEnabled = false;

	public:
		inline void EnableInputPin_o_Receive( void *_Data )
		{
			InitialEnabled = *(bool *)_Data;
		}

		void InputPin_o_Receive( void *_Data )
		{
			if( InitialEnabled )
				OutputPin.Notify( nullptr );
		}

	};
//---------------------------------------------------------------------------
}

#endif
