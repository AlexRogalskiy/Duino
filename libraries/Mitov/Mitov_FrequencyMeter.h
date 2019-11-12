////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2018 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_FREQUENCY_METER_h
#define _MITOV_FREQUENCY_METER_h

#include <Mitov.h>

namespace Mitov
{
	class FrequencyMeter
	{
	public:
		OpenWire::SourcePin	OutputPin;

    public:
		float	MinFrequency = 0.1;
        bool	Enabled : 1;

	protected:
		bool			FFirstTime	: 1;
		bool			FZeroed		: 1;
		unsigned long	FStartTime = 0;

	public:
		void InputPin_o_Receive( void *_Data )
		{
			if( FFirstTime )
			{
				FFirstTime = false;
				FStartTime = micros();
				return;
			}

            if( ! Enabled )
				return;

			unsigned long ANow = micros();
			unsigned long APeriod = ANow - FStartTime;
			FStartTime = ANow;

			if( APeriod == 0 )
				APeriod = 1;

			float AFrequency =  1000000.0f / APeriod;
			OutputPin.Notify( &AFrequency );
			FZeroed = false;
		}

	public:
		inline void SystemLoopBegin( unsigned long currentMicros ) // For compatibility with Interrupt version
		{
			if( ! Enabled )
				return;

			if( FZeroed )
				return;

			if( FFirstTime )
			{
				OutputPin.SendValue( 0 );
				FZeroed = true;
			}

			currentMicros = micros();
			unsigned long ATimeDiff = ( currentMicros - FStartTime );
			if( ATimeDiff > 1000000 )
			{
				float AFrequency =  1000000.0f / ATimeDiff;
				if( AFrequency < MinFrequency )
				{
					OutputPin.SendValue( 0 );
					FZeroed = true;
				}

				else
					OutputPin.Notify( &AFrequency );
//				FZeroed = true;
//				FStartTime = currentMicros;
			}
		}

	public:
		FrequencyMeter() :
			Enabled( true ),
			FFirstTime( true ),
			FZeroed( false )
		{
		}

	};
//---------------------------------------------------------------------------
	class FrequencyMeter_Interrupt : public OpenWire::InterruptObject
	{
    public:
		OpenWire::SourcePin	OutputPin;

    public:
		float	MinFrequency = 0.1;
        bool	Enabled : 1;

	protected:
		bool			FFirstTime	: 1;
		bool			FZeroed		: 1;
		unsigned long	FLastTime = 0;
		uint32_t FCounter = 0;

	public:
		void InterruptHandler( bool )
		{
			++ FCounter;
		}

/*
	protected:
		void InputPin_o_Receive( void *_Data )
		{
			if( FFirstTime )
			{
				FFirstTime = false;
				FStartTime = micros();
				return;
			}

            if( ! Enabled )
				return;

			unsigned long ANow = micros();
			unsigned long APeriod = ANow - FStartTime;
			FStartTime = ANow;

			if( APeriod == 0 )
				APeriod = 1;

			float AFrequency =  1000000.0f / APeriod;
			OutputPin.Notify( &AFrequency );
		}
*/
	public:
		inline void SystemLoopBegin( unsigned long currentMicros )
		{
			if( ! Enabled )
			{
				noInterrupts(); // Protect the FReceivedPackets from being corrupted
				FCounter = 0;
				interrupts();
				FLastTime = currentMicros;
				return;
			}				

			currentMicros = micros();
			unsigned long ATimeDiff = ( currentMicros - FLastTime );

			noInterrupts(); // Protect the FReceivedPackets from being corrupted
			uint32_t ACount = FCounter;
			if( ( ! ACount ) && ( ATimeDiff < 100000 ) )
			{
				interrupts();
				return;
			}

			FCounter = 0;

			interrupts();

			if( FFirstTime )
				if( ! ACount )
				{
					if( ! FZeroed )
					{
						OutputPin.SendValue( 0 );
						FZeroed = true;
					}

					return;
				}

//			Serial.print( "ACount = " );
//			Serial.println( ACount );

			if( ! ACount )
				ACount = 1;

			else
				FLastTime = currentMicros;

			float AFrequency = ( float( ACount ) / ATimeDiff ) * 1000000;
			if( AFrequency < MinFrequency )
			{
				if( ! FZeroed )
				{
					OutputPin.SendValue( 0 );
					FZeroed = true;
				}
			}

			else
			{
				FFirstTime = false;
				FZeroed = false;
				OutputPin.Notify( &AFrequency );
			}

//			Serial.println( AFrequency );

//			if( ACount )
//				FLastTime = currentMicros;
		}

	public:
/*
		FrequencyMeter_Interrupt( int AInterruptPin, void (*AInterruptRoutine)() )
//			Enabled( true )
//			FFirstTime( true )
		{
			// Add by Adrien van den Bossche <vandenbo@univ-tlse2.fr> for Teensy
			// ARM M4 requires the below. else pin interrupt doesn't work properly.
			// On all other platforms, its innocuous, belt and braces
			pinMode( AInterruptPin, INPUT ); 

			int AInterruptNumber = digitalPinToInterrupt( AInterruptPin );
			attachInterrupt( AInterruptNumber, AInterruptRoutine, RISING );
		}
*/
		FrequencyMeter_Interrupt() :
			Enabled( true ),
			FFirstTime( true ),
			FZeroed( false )
		{
		}
	};
}

#endif
