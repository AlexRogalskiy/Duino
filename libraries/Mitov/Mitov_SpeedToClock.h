////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2018 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_SPEED_TO_CLOCK_h
#define _MITOV_SPEED_TO_CLOCK_h

#include <Mitov.h>

namespace Mitov
{
	class SpeedToClock
	{
	public:
		OpenWire::SourcePin	DirectionOutputPin;
		OpenWire::SourcePin	ClockOutputPin;

	public:
		float	MaxFrequency = 100.0f;
		bool	Enabled : 1;

	protected:
		bool          FSendOutput : 1;
		unsigned long FLastTime = 0;
		unsigned long FPeriod = 0;

	public:
		inline void SystemLoopBegin( unsigned long currentMicros )
		{
			if( ! FSendOutput )
				FLastTime = currentMicros;

			else
				while( currentMicros - FLastTime >= FPeriod )
				{
					if( Enabled )
						ClockOutputPin.Notify( nullptr );

					FLastTime += FPeriod;
				}

		}

		inline void SystemStart()
		{
			FLastTime = micros();
		}

	public:
		void InputPin_o_Receive( void *_Data )
		{
			float ASpeed = constrain( *(float *)_Data, 0.0, 1.0 );
			float AFrequency = abs( ASpeed - 0.5 ) * 2;
			FSendOutput = ( AFrequency != 0 );
			if( FSendOutput )
			{
				AFrequency *= MaxFrequency;
				FPeriod = ( (( 1 / AFrequency ) * 1000000 ) + 0.5 );
			}

			else
				FPeriod = 0;

			bool ADirection = ASpeed > 0.5;

			DirectionOutputPin.Notify( &ADirection );
		}

	public:
		SpeedToClock() :
			Enabled( true ),
			FSendOutput( false )
		{
		}
	};
}

#endif
