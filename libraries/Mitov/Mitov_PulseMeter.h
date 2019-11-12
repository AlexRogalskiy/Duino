////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2018 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_PULSE_METER_h
#define _MITOV_PULSE_METER_h

#include <Mitov.h>

namespace Mitov
{
	class PulseMeter
	{
	public:
		OpenWire::SourcePin	OutputPin;

    public:
        bool Enabled : 1;

	protected:
		bool			FOldValue : 1;
		unsigned long	FStartTime = 0;

    public:
		void InputPin_o_Receive( void *_Data )
		{
            if( ! Enabled )
				return;

			bool AValue = *(bool *)_Data;
//			Serial.println( AValue );
			if( FOldValue == AValue )
				return;

			unsigned long ANow = micros();
			FOldValue = AValue;
			if( AValue )
			{
				FStartTime = ANow;
				return;
			}

			uint32_t APeriod = ANow - FStartTime;

			OutputPin.Notify( &APeriod );
		}

	public:
		inline void SystemStart()
		{
			OutputPin.SendValue<uint32_t>( 0 );
		}

		inline void SystemLoopBegin( unsigned long currentMicros ) {} // Placeholder

	public:
		PulseMeter() :
			Enabled( true ),
			FOldValue( false )
		{
		}
	};
//---------------------------------------------------------------------------
	class PulseMeter_Interrupt : public OpenWire::InterruptObject
	{
    public:
		OpenWire::SourcePin	OutputPin;

    public:
        bool Enabled : 1;

	protected:
		bool			FOldValue : 1;
		bool			FChanged : 1;
		unsigned long	FStartTime = 0;
		uint32_t		FPeriod	= 0;

	public:
		void InterruptHandler( bool AValue )
		{
			if( FOldValue == AValue )
				return;

			unsigned long ANow = micros();
			FOldValue = AValue;
			if( AValue )
			{
				FStartTime = ANow;
				return;
			}

			FPeriod = ANow - FStartTime;
			FChanged = true;
//			Serial.println( "Test" );

		}

	public:
		inline void SystemStart() {} // Placeholder

		inline void SystemLoopBegin( unsigned long currentMicros )
		{
			if( ! Enabled )
				return;

			if( ! FChanged )
				return;

			uint32_t APeriod = FPeriod;
			FChanged = false;
			OutputPin.Notify( &APeriod );
		}

	public:
		PulseMeter_Interrupt() :
			Enabled( true ),
			FOldValue( false ),
			FChanged( false )
		{
		}
	};
}

#endif
