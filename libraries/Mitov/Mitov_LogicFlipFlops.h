////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2018 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_LOGIC_FLIP_FLOPS_h
#define _MITOV_LOGIC_FLIP_FLOPS_h

#include <Mitov.h>

namespace Mitov
{
//---------------------------------------------------------------------------
	class FlipFlop
	{
	protected:
		inline void SendOutput( bool AValue )
		{
		}
	};
//---------------------------------------------------------------------------
	class FlipFlop_Normal
	{
	public:
		OpenWire::SourcePin	OutputPin;

	protected:
		inline void SendOutput( bool AValue )
		{
			OutputPin.SendValue( AValue );
		}
	};
//---------------------------------------------------------------------------
	class FlipFlop_Inverted
	{
	public:
		OpenWire::SourcePin	InvertedOutputPin;

	protected:
		inline void SendOutput( bool AValue )
		{
			InvertedOutputPin.SendValue( ! AValue );
		}
	};
//---------------------------------------------------------------------------
	class FlipFlop_Normal_Inverted
	{
	public:
		OpenWire::SourcePin	OutputPin;
		OpenWire::SourcePin	InvertedOutputPin;

	protected:
		void SendOutput( bool AValue )
		{
			OutputPin.SendValue( AValue );
			InvertedOutputPin.SendValue( ! AValue );
		}
	};
//---------------------------------------------------------------------------
	template<typename T_BASE> class SRFlipFlop : public T_BASE
	{
		typedef T_BASE inherited;

	public:
		bool	InitialValue : 1;

	protected:
		bool	FToggleValue : 1; // In base class to save memory
		bool	FData : 1; // In base class to save memory

	protected:
		void SetValue( bool AValue )
		{
			InitialValue = AValue;
			SendOutput();
		}

		inline void SendOutput()
		{
			inherited::SendOutput( InitialValue );
		}

	public:
		inline void SetInputPin_o_Receive( void *_Data )
		{
			SetValue( true );
		}

		inline void ResetInputPin_o_Receive( void *_Data )
		{
			SetValue( false );
		}

	public:
		inline void SystemStart()
		{
			SendOutput();
		}

	public:
		SRFlipFlop() :
			InitialValue( false ),
			FToggleValue( true ),
			FData( false )
		{
		}
	};
//---------------------------------------------------------------------------
	template<typename T_BASE> class TFlipFlop : public Mitov::SRFlipFlop<T_BASE>
	{
		typedef Mitov::SRFlipFlop<T_BASE> inherited;

	public:
		inline void ToggleInputPin_o_Receive( void *_Data )
		{
			inherited::FToggleValue = *(bool *)_Data;
		}

		void ClockInputPin_o_Receive( void *_Data )
		{
			if( inherited::FToggleValue )
				inherited::SetValue( ! inherited::InitialValue );
		}

	};
//---------------------------------------------------------------------------
	template<typename T_BASE> class DFlipFlop : public Mitov::SRFlipFlop<T_BASE>
	{
		typedef Mitov::SRFlipFlop<T_BASE> inherited;

	public:
		inline void ClockInputPin_o_Receive( void *_Data )
		{
			inherited::SetValue( inherited::FData );
		}

		inline void DataInputPin_o_Receive( void *_Data )
		{
			inherited::FData = *(bool *)_Data;
		}

	};
//---------------------------------------------------------------------------
	template<typename T_BASE> class JKFlipFlop : public T_BASE
	{
		typedef T_BASE inherited;

	public:
		bool	InitialValue : 1;

	protected:
		bool FJ : 1;
		bool FK : 1;

	public:
		inline void JInputPin_o_Receive( void *_Data )
		{
			FJ = *(bool *)_Data;
		}

		inline void KInputPin_o_Receive( void *_Data )
		{
			FK = *(bool *)_Data;
		}

		void ClockInputPin_o_Receive( void *_Data )
		{
			if( FJ && FK )
				SetValue( !InitialValue );

			else if( FJ )
				SetValue( true );

			else if( FK )
				SetValue( false );

		}

	protected:
		void SetValue( bool AValue )
		{
			InitialValue = AValue;
			SendOutput();
		}

		inline void SendOutput()
		{
			inherited::SendOutput( InitialValue );
		}

	public:
		inline void SystemStart()
		{
			SendOutput();
		}

	public:
		JKFlipFlop() :
			FJ( false ),
			FK( false ),
			InitialValue( false )
		{
		}

	};
//---------------------------------------------------------------------------
}

#endif
