////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2018 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_ARDUINO_PROCESSOR_h
#define _MITOV_ARDUINO_PROCESSOR_h

#include <Mitov.h>
#include <avr/wdt.h>

#ifdef __TEST_FOR_DEBUG_PRINTS__
#define Serial UNGUARDED DEBUG PRINT!!!
#endif

extern unsigned int __heap_start;
extern void *__brkval;

// The free list structure as maintained by the 
// avr-libc memory allocation routines.

namespace Mitov
{
	struct __freelist {
	  size_t sz;
	  struct __freelist *nx;
	};
}

// The head of the free list structure
extern struct Mitov::__freelist *__flp;

namespace Mitov
{
//---------------------------------------------------------------------------
	enum TArduinoWatchdogTimerInterval 
	{
		wdt15mS		= WDTO_15MS,
		wdt30mS		= WDTO_30MS,
		wdt60mS		= WDTO_60MS,
		wdt120mS	= WDTO_120MS,
		wdt250mS	= WDTO_250MS,
		wdt500mS	= WDTO_500MS,
		wdt1S		= WDTO_1S,
		wdt2S		= WDTO_2S,
		wdt4S		= WDTO_4S,
		wdt8S		= WDTO_8S
	};
//---------------------------------------------------------------------------
	template<TArduinoWatchdogTimerInterval C_INTERVAL> class ArduinoWatchdogTimer
	{
	public:
		bool	Enabled = false;

	public:
		void UpdateEnabled()
		{
			if( Enabled )
				wdt_enable( C_INTERVAL );

			else
				wdt_disable();

		}

	public:
		inline void ClockInputPin_o_Receive( void *_Data )
		{
			if( Enabled )
				wdt_reset();
		}

		inline void SystemStart()
		{
			UpdateEnabled();
		}

	};
//---------------------------------------------------------------------------
	class ArduinoProcessorReset
	{
	public:
		void ClockInputPin_o_Receive( void *_Data )
		{
			void(* resetFunc) (void) = 0; //declare reset function at address 0
			resetFunc();
		}
	};
//---------------------------------------------------------------------------
	class GetArduinoAvailableRAM
	{
	public:
		OpenWire::SourcePin	OutputPin;

	public:
		bool	Enabled = true;

	protected:
		uint32_t	FValue;

	protected:
		// Calculates the size of the free list
		int freeListSize() 
		{
			struct __freelist* current;
			int total = 0;
			for (current = __flp; current; current = current->nx) 
			{
				total += 2; /* Add two bytes for the memory block's header  */
				total += (int) current->sz;
			}

			return total;
		}

		int freeMemory()
		{
			int free_memory;
			if ((int)__brkval == 0) 
				free_memory = ((int)&free_memory) - ((int)&__heap_start);

			else 
			{
				free_memory = ((int)&free_memory) - ((int)__brkval);
				free_memory += freeListSize();
			}

			return free_memory;
		}

		void ReadValue( bool AChangeOnly )
		{
			if( ! Enabled )
				return;

			uint32_t	AValue = freeMemory();

			if( AChangeOnly )
				if( AValue == FValue )
					return;

			FValue = AValue;

			OutputPin.Notify( &FValue );
		}

	public:
		inline void ClockInputPin_o_Receive( void *_Data )
		{
			ReadValue( false );
		}

	public:
		inline void SystemStart()
		{
			ReadValue( false );
		}

		inline void SystemLoopBegin( unsigned long currentMicros )
		{
			ReadValue( true );
		}

	};
//---------------------------------------------------------------------------
}

#ifdef __TEST_FOR_DEBUG_PRINTS__
#undef Serial
#endif

#endif
