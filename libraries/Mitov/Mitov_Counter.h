////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2018 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_COUNTER_h
#define _MITOV_COUNTER_h

#include <Mitov.h>

namespace Mitov
{
#define Min Min
#define Max Max
	class CounterLimit
	{
	public:
		int32_t Value;
		bool RollOver = true;

	public:
		CounterLimit( int32_t AValue ) :
			Value( AValue )
		{
		}
	};
//---------------------------------------------------------------------------
	class BasicCounter
	{
	public:
		OpenWire::SourcePin	OutputPin;

    public:
        bool Enabled : 1;
		bool Reversed : 1;	// Added here to save memory

	public:
		int32_t	InitialValue = 0;

	public:
		CounterLimit	Min = -2147483648;
		CounterLimit	Max = 2147483647;

	public:
		inline void SetValue( int32_t AValue )
		{
			FCount = AValue;
			OutputPin.Notify( &FCount );
		}

	protected:
		int32_t	FCount = 0;

	public:
		inline void SystemInit()
		{
//			Serial.println( "Counter::SystemInit" );
			FCount = InitialValue;

			OutputPin.Notify( &FCount );
		}

		inline void SystemStart()
		{
			if( FCount != InitialValue )
				OutputPin.Notify( &FCount );

		}

	protected:
		inline void	CountUp()
		{
			if( Max.RollOver || ( FCount < Max.Value ))
			{
				++FCount;

				if( FCount > Max.Value )
					FCount = Min.Value;

				OutputPin.Notify( &FCount );
			}
		}

		inline void	CountDown()
		{
			if( Min.RollOver || ( FCount > Min.Value ))
			{
				--FCount;
				if( FCount < Min.Value )
					FCount = Max.Value;

				OutputPin.Notify( &FCount );
			}
		}

	public:
		void ResetInputPin_o_Receive( void *_Data )
		{
			FCount = InitialValue;
			OutputPin.Notify( &FCount );
		}

	public:
		BasicCounter() :
			Enabled( true ),
			Reversed( false )
		{
		}

	};
//---------------------------------------------------------------------------
	template<typename T_OWNER, T_OWNER &C_OWNER> class ArduinoCounterSetValueElement
	{
	public:
		int32_t	Value = 0;

	public:
		inline void InputPin_o_Receive( void *_Data )
		{
			C_OWNER.SetValue( Value );
		}

	};
//---------------------------------------------------------------------------
	class Counter : public Mitov::BasicCounter
	{
		typedef Mitov::BasicCounter inherited;

//	public:
//		bool	Reversed = false;

	public:
		void InputPin_o_Receive( void *_Data )
		{
            if( ! Enabled )
				return;

			if( Reversed )
				CountDown();

			else
				CountUp();

		}

	};
//---------------------------------------------------------------------------
	class UpDownCounter : public Mitov::BasicCounter
	{
		typedef Mitov::BasicCounter inherited;

	public:
		void UpInputPin_o_Receive( void *_Data )
		{
            if( Enabled )
				CountUp();
		}

		void DownInputPin_o_Receive( void *_Data )
		{
            if( Enabled )
				CountDown();
		}

	};
//---------------------------------------------------------------------------
#undef Min
#undef Max
}

#endif
