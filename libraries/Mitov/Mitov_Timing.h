////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2018 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_TIMING_h
#define _MITOV_TIMING_h

#include <Mitov.h>

#ifdef __TEST_FOR_DEBUG_PRINTS__
#define Serial UNGUARDED DEBUG PRINT!!!
#endif

namespace Mitov
{
	class ClockGenerator
	{
	public:
		OpenWire::SourcePin	OutputPin;

	protected:
		unsigned long FLastTime = 0;
		unsigned long FPeriod = 0;

	public:
		bool	Enabled = true;
		float	Frequency = 1.0;

	public:
		void CalculateFields()
		{
			if( Frequency == 0 )
				FPeriod = 1000000;

			else
				FPeriod = ( (( 1 / Frequency ) * 1000000 ) + 0.5 );

		}

	public:
		inline void SystemLoopBegin( unsigned long currentMicros )
		{
			while( currentMicros - FLastTime >= FPeriod )
			{
				if( Enabled && ( Frequency != 0 ))
					OutputPin.Notify(nullptr);

				FLastTime += FPeriod;
			}

		}

		inline void SystemInit()
		{
			CalculateFields();
		}

		inline void SystemStart()
		{
			FLastTime = micros();
		}

	};
//---------------------------------------------------------------------------
	class Timer
	{
	public:
		OpenWire::SourcePin	OutputPin;

	public:
		uint32_t	Interval = 1000000;
		bool		AutoRepeat : 1;
		bool		CanRestart : 1;
		bool		Enabled : 1;

	public:
		void	UpdateEnabled()
		{
			if (Enabled)
			{
				if (AutoRepeat)
					StartInputPin_o_Receive(nullptr);
			}

			else
			{
				if (FValue)
				{
					FValue = false;
					OutputPin.SendValue(FValue);
				}
			}
		}

	protected:
		bool		FValue : 1;

		uint32_t	FStartTime;

	public:
		inline void SystemStart()
		{
			OutputPin.SendValue(FValue);
		}

		inline void SystemLoopBegin(unsigned long currentMicros)
		{
			if (Enabled)
			{
				if (!AutoRepeat)
					if (!FValue)
						return;

				if (FValue)
					if (!Enabled)
					{
						FValue = false;
						OutputPin.SendValue(FValue);
						return;
					}

				currentMicros = micros(); // Use micros(); to make sure the DoReceiveStart is not from the same clock event!
				if (currentMicros - FStartTime >= Interval)
				{
					FValue = !FValue;
					OutputPin.SendValue(FValue);
					if (AutoRepeat)
						FStartTime += Interval;
				}
			}
		}

	public:
		void StartInputPin_o_Receive(void *_Data)
		{
			if (!Enabled)
				return;

			if (!CanRestart)
				if (FValue)
					return;

			FStartTime = micros();
			if (FValue)
				return;

			FValue = true;
			OutputPin.SendValue(FValue);
		}

		void ResetInputPin_o_Receive(void *_Data)
		{
			if (FValue)
			{
				FValue = false;
				OutputPin.SendValue(FValue);

				if (AutoRepeat)
					FStartTime = micros();
			}
		}

	public:
		Timer() :
			AutoRepeat(false),
			CanRestart(true),
			Enabled(true),
			FValue(false)
		{
		}
	};
//---------------------------------------------------------------------------
	class Repeat
	{
	public:
		OpenWire::SourcePin	OutputPin;

	public:
		bool	Enabled = true;

	public:
		inline void SystemLoopBegin(unsigned long currentMicros)
		{
			if (Enabled)
				OutputPin.Notify(nullptr);

		}

	};
//---------------------------------------------------------------------------
	class Start
	{
	public:
		OpenWire::SourcePin	OutputPin;

	public:
		inline void SystemStart()
		{
			OutputPin.Notify(nullptr);
		}

	};
//---------------------------------------------------------------------------
	class MultiSource
	{
	public:
		Mitov::SimpleList<OpenWire::SourcePin> OutputPins;

	public:
		void InputPin_o_Receive(void *_Data)
		{
			for (int i = 0; i < OutputPins.size(); ++i)
				OutputPins[i].Notify(_Data);
		}

	};
//---------------------------------------------------------------------------
	class RepeatSource
	{
	public:
		OpenWire::SourcePin	OutputPin;

	public:
		bool	Enabled = true;
		uint32_t	Count = 1;

	public:
		void InputPin_o_Receive(void *_Data)
		{
			OutputPin.Notify(_Data);
			if (!Enabled)
				return;

			for (uint32_t i = 0; i < Count; ++i)
				OutputPin.Notify(_Data);
		}
	};
//---------------------------------------------------------------------------
	class RepeatSource_Interval
	{
	public:
		OpenWire::SourcePin	OutputPin;

	public:
		bool	Enabled = true;
		uint32_t	Count = 1;
		uint32_t	Interval = 0;

	protected:
		uint32_t	FRemainingCount = 0;
		uint32_t	FStartTime;

	public:
		void InputPin_o_Receive(void *_Data)
		{
			OutputPin.Notify(_Data);
			if (!Enabled)
				return;

			if (!Interval)
			{
				for (uint32_t i = 0; i < Count; ++i)
					OutputPin.Notify(_Data);

				return;
			}

			FStartTime = micros();
			FRemainingCount = Count;
		}

	public:
		inline void SystemLoopBegin(unsigned long currentMicros)
		{
			if (!Enabled)
				return;

			if (!FRemainingCount)
				return;

			uint32_t ACurrentTime = micros();
			if (ACurrentTime - FStartTime < Interval)
				return;

			OutputPin.Notify(nullptr);
			--FRemainingCount;
			FStartTime = ACurrentTime;
		}

	};
//---------------------------------------------------------------------------
	class Delay
	{
	public:
		OpenWire::SourcePin	OutputPin;

	public:
		unsigned long	Interval = 1000000;
		bool			CanRestart : 1;
		bool			Enabled : 1;


	protected:
		bool			FValue : 1;
		unsigned long	FStartTime;

	public:
		inline void SystemLoopBegin(unsigned long currentMicros)
		{
			if (FValue)
			{
				if (!Enabled)
					FValue = false;

				else
				{
					unsigned long ACurrentMicros = micros(); // Use micros(); to make sure the DoReceiveStart is not from the same clock event!
					if (ACurrentMicros - FStartTime >= Interval)
					{
						FValue = false;
						OutputPin.Notify(nullptr);
					}
				}
			}
		}

	public:
		void StartInputPin_o_Receive(void *_Data)
		{
			if (!Enabled)
				return;

			if (!CanRestart)
				if (FValue)
					return;

			FStartTime = micros();
			if (FValue)
				return;

			FValue = true;
		}

		inline void ResetInputPin_o_Receive(void *_Data)
		{
			FValue = false;
		}

	public:
		Delay() :
			CanRestart(true),
			Enabled(true),
			FValue(false)
		{
		}
	};
//---------------------------------------------------------------------------
	class OnOffDelay
	{
	public:
		OpenWire::SourcePin	OutputPin;

	public:
		unsigned long	OnInterval = 1000000;
		unsigned long	OffInterval = 1000000;
		bool			Enabled : 1;
		bool			InitialValue : 1;

	protected:
		bool			FInputValue : 1;
		unsigned long	FStartTime;

	public:
		void InputPin_o_Receive(void *_Data)
		{
			bool AValue = *(bool *)_Data;
			if (FInputValue == AValue)
				return;

			FInputValue = AValue;
			FStartTime = micros();
		}

	public:
		inline void SystemStart()
		{
			FInputValue = InitialValue;
			OutputPin.SendValue(FInputValue);
		}

		inline void SystemLoopBegin(unsigned long ACurrentMicros)
		{
			if (FInputValue == InitialValue)
				return;

			unsigned long AInterval = (FInputValue) ? OnInterval : OffInterval;
			if (micros() - FStartTime < AInterval) // Use micros() to avoid problems when started from the same loop!
				return;

			InitialValue = FInputValue;
			OutputPin.SendValue(FInputValue);
		}

	public:
		OnOffDelay() :
			Enabled(true),
			InitialValue(false)
			//			FInputValue( false )

		{
		}
	};
//---------------------------------------------------------------------------
	class DetectEdge
	{
	public:
		OpenWire::SourcePin	OutputPin;

	public:
		bool	Enabled : 1;
		bool	Rising : 1;

	protected:
		bool	FLastValue : 1;

	public:
		void InputPin_o_Receive(void *_Data)
		{
			bool AValue = *(bool *)_Data;
			if (AValue == FLastValue)
				return;

			FLastValue = AValue;
			if (Rising == AValue)
				OutputPin.Notify(nullptr);

		}

	public:
		DetectEdge() :
			Enabled(true),
			Rising(true),
			FLastValue(false)
		{
		}

	};
//---------------------------------------------------------------------------
	class CPUTimeMicrosMillis
	{
	public:
		OpenWire::SourcePin	MicroSecondsOutputPin;
		OpenWire::SourcePin	MilliSecondsOutputPin;

	protected:
		void SendOutputs()
		{
			MicroSecondsOutputPin.SendValue<uint32_t>(micros());
			MilliSecondsOutputPin.SendValue<uint32_t>(millis());
		}

	public:
		inline void ClockInputPin_o_Receive( void *_Data )
		{
			SendOutputs();
		}

	public:
		inline void SystemStart()
		{
			SendOutputs();
		}

		inline void SystemLoopBegin(unsigned long ACurrentMicros)
		{
			SendOutputs();
		}
	};
//---------------------------------------------------------------------------
	class CPUTimeMicros
	{
	public:
		OpenWire::SourcePin	MicroSecondsOutputPin;

	protected:
		inline void SendOutputs()
		{
			MicroSecondsOutputPin.SendValue<uint32_t>(micros());
		}

	public:
		inline void ClockInputPin_o_Receive( void *_Data )
		{
			SendOutputs();
		}

	public:
		inline void SystemStart()
		{
			SendOutputs();
		}

		inline void SystemLoopBegin(unsigned long ACurrentMicros)
		{
			SendOutputs();
		}
	};
//---------------------------------------------------------------------------
	class CPUTimeMillis
	{
	public:
		OpenWire::SourcePin	MilliSecondsOutputPin;

	protected:
		inline void SendOutputs()
		{
			MilliSecondsOutputPin.SendValue<uint32_t>(millis());
		}

	public:
		inline void ClockInputPin_o_Receive( void *_Data )
		{
			SendOutputs();
		}

	public:
		inline void SystemStart()
		{
			SendOutputs();
		}

		inline void SystemLoopBegin(unsigned long ACurrentMicros)
		{
			SendOutputs();
		}
	};
//---------------------------------------------------------------------------
	class CPUTimeMicrosMillis_Enable
	{
	public:
		OpenWire::SourcePin	MicroSecondsOutputPin;
		OpenWire::SourcePin	MilliSecondsOutputPin;

	public:
		bool	Enabled = true;

	protected:
		void SendOutputs()
		{
			if (Enabled)
			{
				MicroSecondsOutputPin.SendValue<uint32_t>(micros());
				MilliSecondsOutputPin.SendValue<uint32_t>(millis());
			}
		}

	public:
		inline void ClockInputPin_o_Receive( void *_Data )
		{
			SendOutputs();
		}

	public:
		inline void SystemStart()
		{
			SendOutputs();
		}

		inline void SystemLoopBegin(unsigned long ACurrentMicros)
		{
			SendOutputs();
		}
	};
//---------------------------------------------------------------------------
	class CPUTimeMicros_Enable
	{
	public:
		OpenWire::SourcePin	MicroSecondsOutputPin;

	public:
		bool	Enabled = true;

	protected:
		inline void SendOutputs()
		{
			if (Enabled)
				MicroSecondsOutputPin.SendValue<uint32_t>(micros());

		}

	public:
		inline void ClockInputPin_o_Receive( void *_Data )
		{
			SendOutputs();
		}

	public:
		inline void SystemStart()
		{
			SendOutputs();
		}

		inline void SystemLoopBegin(unsigned long ACurrentMicros)
		{
			SendOutputs();
		}
	};
//---------------------------------------------------------------------------
	class CPUTimeMillis_Enable
	{
	public:
		OpenWire::SourcePin	MilliSecondsOutputPin;

	public:
		bool	Enabled = true;

	protected:
		inline void SendOutputs()
		{
			if (Enabled)
				MilliSecondsOutputPin.SendValue<uint32_t>(millis());

		}

	public:
		inline void ClockInputPin_o_Receive( void *_Data )
		{
			SendOutputs();
		}

	public:
		inline void SystemStart()
		{
			SendOutputs();
		}

		inline void SystemLoopBegin(unsigned long ACurrentMicros)
		{
			SendOutputs();
		}
	};
//---------------------------------------------------------------------------
}

#ifdef __TEST_FOR_DEBUG_PRINTS__
#undef Serial
#endif

#endif
