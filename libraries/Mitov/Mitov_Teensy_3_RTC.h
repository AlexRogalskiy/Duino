////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2018 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_TEENSY_3_RTC_h
#define _MITOV_TEENSY_3_RTC_h

#include <Mitov.h>
#include <TimeLib.h>

#ifdef __TEST_FOR_DEBUG_PRINTS__
#define Serial UNGUARDED DEBUG PRINT!!!
#endif

namespace Mitov
{
//---------------------------------------------------------------------------
	class Teensy3RTC
	{
	public:
		OpenWire::SourcePin	OutputPin;

	public:
		bool	Enabled = true;

#ifdef _MITOV_TEENSY_RTC_MODULE_COMPENSATE_
		float	Compensate = 0.0;
#endif

	protected:
		Mitov::TDateTime FLastDateTime;

	public:
#ifdef _MITOV_TEENSY_RTC_MODULE_COMPENSATE_
		void UpdateCompensate()
		{
			float APPM = Compensate * 1000000;
			Teensy3Clock.compensate( ( APPM / 8 ) + 0.5 );
		}

#endif // _MITOV_TEENSY_RTC_MODULE_COMPENSATE_

	public:
		void SetMilliSecond( uint8_t AValue )
		{
		}

		void SetSecond( uint8_t AValue )
		{
			time_t ATime = Teensy3Clock.get();
			tmElements_t AElements;
			breakTime( ATime, AElements );
			AElements.Second = AValue;
			Teensy3Clock.set( makeTime( AElements ) );
		}

		void SetMinute( uint8_t AValue )
		{
			time_t ATime = Teensy3Clock.get();
			tmElements_t AElements;
			breakTime( ATime, AElements );
			AElements.Minute = AValue;
			Teensy3Clock.set( makeTime( AElements ) );
		}

		void SetHour( uint8_t AValue )
		{
			time_t ATime = Teensy3Clock.get();
			tmElements_t AElements;
			breakTime( ATime, AElements );
			AElements.Hour = AValue;
			Teensy3Clock.set( makeTime( AElements ) );
		}

		void SetWeekDay( uint8_t AValue )
		{
			time_t ATime = Teensy3Clock.get();
			tmElements_t AElements;
			breakTime( ATime, AElements );
			AElements.Wday = AValue;
			Teensy3Clock.set( makeTime( AElements ) );			
		}

		void SetYear( uint16_t AValue )
		{
			time_t ATime = Teensy3Clock.get();
			tmElements_t AElements;
			breakTime( ATime, AElements );
			AElements.Year = AValue - 1970;
			Teensy3Clock.set( makeTime( AElements ) );
		}

		Mitov::TDateTime GetDateTime()
		{
			time_t ATime = Teensy3Clock.get();
			uint16_t ASecond = second( ATime );
			uint16_t AMinute = minute( ATime );
			uint16_t AHour = hour( ATime );
			uint16_t ADate = day( ATime );
			uint16_t AMonth = month( ATime );
			uint16_t AYear = year( ATime );
			
			Mitov::TDateTime ADateTime;

			ADateTime.TryEncodeDateTime( AYear, AMonth, ADate, AHour, AMinute, ASecond, 0 );

			return ADateTime;
		}

		void SetDateTime( const Mitov::TDateTime &ADateTime )
		{
			uint16_t AYear;
			uint16_t AMonth;
			uint16_t ADay;
			uint16_t AWeekDay;
			uint16_t AHour;
			uint16_t AMinute;
			uint16_t ASecond;
			uint16_t AMilliSecond;
			ADateTime.DecodeDateTime( AYear, AMonth, ADay, AWeekDay, AHour, AMinute, ASecond, AMilliSecond );

			tmElements_t tm;
			tm.Second = ASecond; 
			tm.Minute = AMinute; 
			tm.Hour = AHour; 
			tm.Wday = AWeekDay + 1;   // day of week, sunday is day 1
			tm.Day = ADay;
			tm.Month = AMonth; 
			tm.Year = AYear - 1970;   // offset from 1970;
			
			time_t ATime = makeTime( tm );
			Teensy3Clock.set( ATime );
		}

	protected:
		void ReadTime( bool AFromClock )
		{
			time_t ATime = Teensy3Clock.get();
			uint16_t ASecond = second( ATime );
			uint16_t AMinute = minute( ATime );
			uint16_t AHour = hour( ATime );
			uint16_t ADate = day( ATime );
			uint16_t AMonth = month( ATime );
//			uint16_t ADay = weekday( ATime );
			uint16_t AYear = year( ATime );
			
			Mitov::TDateTime ADateTime;

			if( ADateTime.TryEncodeDateTime( AYear, AMonth, ADate, AHour, AMinute, ASecond, 0 ))
			{
				if( AFromClock || ( FLastDateTime != ADateTime ))
				{
					FLastDateTime = ADateTime;
					OutputPin.Notify( &ADateTime );
				}
			}
		}


	public:
		inline void ClockInputPin_o_Receive( void *_Data )
		{
			ReadTime( true );
		}

		void SetInputPin_o_Receive( void *_Data )
		{
			Mitov::TDateTime &ADateTime = *(Mitov::TDateTime *)_Data;

			uint16_t AYear;
			uint16_t AMonth;
			uint16_t ADay;
			uint16_t AWeekDay;
			uint16_t AHour;
			uint16_t AMinute;
			uint16_t ASecond;
			uint16_t AMilliSecond;
			ADateTime.DecodeDateTime( AYear, AMonth, ADay, AWeekDay, AHour, AMinute, ASecond, AMilliSecond );

			tmElements_t tm;
			tm.Second = ASecond; 
			tm.Minute = AMinute; 
			tm.Hour = AHour; 
			tm.Wday = AWeekDay + 1;   // day of week, sunday is day 1
			tm.Day = ADay;
			tm.Month = AMonth; 
			tm.Year = AYear - 1970;   // offset from 1970;
			
			time_t ATime = makeTime( tm );
			Teensy3Clock.set( ATime );
		}

	public:
		inline void SystemLoopBegin( unsigned long currentMicros )
		{
			if( Enabled )
				ReadTime( false );

		}

		inline void SystemInit()
		{
#ifdef _MITOV_TEENSY_RTC_MODULE_COMPENSATE_
//			if( Enabled )
			UpdateCompensate();
#endif // _MITOV_TEENSY_RTC_MODULE_COMPENSATE_
		}

	};
//---------------------------------------------------------------------------
}

#ifdef __TEST_FOR_DEBUG_PRINTS__
#undef Serial
#endif

#endif // _MITOV_TEENSY_3_RTC_h
