////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2018 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_ESP8266_PROCESSOR_h
#define _MITOV_ESP8266_PROCESSOR_h

#include <Mitov.h>
#include <Esp.h>

#ifdef __TEST_FOR_DEBUG_PRINTS__
#define Serial UNGUARDED DEBUG PRINT!!!
#endif

namespace Mitov
{
	class ESP8266ProcessorReset
	{
	public:
		inline void ClockInputPin_o_Receive( void *_Data )
		{
			ESP.restart();
		}

	};
//---------------------------------------------------------------------------
	class ESP8266GetProcessorResetReason
	{
	public:
		OpenWire::SourcePin OutputPin;

	public:
		inline void ClockInputPin_o_Receive( void *_Data )
		{
			OutputPin.SendValue( ESP.getResetReason());
		}

	public:
		inline void SystemStart()
		{
			OutputPin.SendValue( ESP.getResetReason());
		}

	};
//---------------------------------------------------------------------------
	class ESP8266GetProcessorResetInfo
	{
	public:
		OpenWire::SourcePin OutputPin;

	public:
		inline void ClockInputPin_o_Receive( void *_Data )
		{
			OutputPin.SendValue( ESP.getResetInfo());
		}

	public:
		inline void SystemStart()
		{
			OutputPin.SendValue( ESP.getResetInfo());
		}

	};
//---------------------------------------------------------------------------
	enum TArduinoESP8266ProcessorModuleDeepSleepMode { dsmDefault = int( RF_DEFAULT ), dsmCalibration = int( RF_CAL ), dsmNoCalibration = int( RF_NO_CAL ), dsmDisableRF = int( RF_DISABLED ) };
//---------------------------------------------------------------------------
	class ESP8266ProcessorDeepSleep
	{
	public:
		TArduinoESP8266ProcessorModuleDeepSleepMode	Mode = dsmDefault;
		uint32_t	Period = 1000000;

	public:
		inline void ClockInputPin_o_Receive( void *_Data )
		{
			ESP.deepSleep( Period, RFMode( Mode ));
		}

	};
//---------------------------------------------------------------------------
}

#ifdef __TEST_FOR_DEBUG_PRINTS__
#undef Serial
#endif

#endif
