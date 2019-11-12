////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2018 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_RTC_DS1307_h
#define _MITOV_RTC_DS1307_h

#include <Mitov.h>

#ifdef __TEST_FOR_DEBUG_PRINTS__
#define Serial UNGUARDED DEBUG PRINT!!!
#endif

namespace Mitov
{
//---------------------------------------------------------------------------
	enum DS1307ClockFrequency { rtc1307fDisabledLow, rtc1307fDisabledHigh, rtc1307f1Hz, rtc1307f4096Hz, rtc1307f8192Hz, rtc1307f32768Hz };
//---------------------------------------------------------------------------
	template<uint8_t C_ADDRESS, typename T_I2C, T_I2C &C_I2C> class RTC_DS1307
	{
		enum Register 
		{
			kSecondReg  = 0,
			kMinuteReg  = 1,
			kHourReg    = 2,
			kWeekDayReg     = 3,
			kDateReg    = 4,
			kMonthReg   = 5,
			kYearReg    = 6,
			kClockReg	= 7,

			// The RAM register space follows the clock register space.
			kRamAddress0     = 8
		};

//		const uint8_t	DS1307_ADDRESS = 0x68;

	public:
		OpenWire::SourcePin	OutputPin;

	public:
		bool					Enabled : 1;
		bool					Halt : 1;
		DS1307ClockFrequency	ClockFrequency : 3;

	public:
		void SetClockFrequency( DS1307ClockFrequency AValue )
		{
			if( ClockFrequency == AValue )
				return;

			ClockFrequency = AValue;
			UpdateClockFrequency();
		}

	public:
		void SetMilliSecond( uint8_t AValue )
		{
		}

		void SetSecond( uint8_t AValue )
		{
			writeRegister( kSecondReg, ( ( Halt & 1 ) << 7 ) | Func::FromDecToBcd( AValue ) );
		}

		void SetMinute( uint8_t AValue )
		{
			writeRegister( kMinuteReg, Func::FromDecToBcd( AValue ) );
		}

		void SetHour( uint8_t AValue )
		{
			writeRegister( kHourReg, Func::FromDecToBcd( AValue ) );
		}

		void SetWeekDay( uint8_t AValue )
		{
			writeRegister( kWeekDayReg, Func::FromDecToBcd( AValue ) );
		}

		void SetYear( uint16_t AValue )
		{
			writeRegister( kYearReg, Func::FromDecToBcd( AValue % 100 ) );
		}

		Mitov::TDateTime GetDateTime()
		{
			C_I2C.beginTransmission( C_ADDRESS );
			C_I2C.write((byte)0);	
			C_I2C.endTransmission();

			C_I2C.requestFrom( C_ADDRESS, (byte)7);
			uint16_t ASecond = Func::FromBcdToDec(C_I2C.read() & 0x7F);
			uint16_t AMinute = Func::FromBcdToDec(C_I2C.read());
			uint16_t AHour = hourFromRegisterValue( C_I2C.read() );
			uint16_t ADay = C_I2C.read();
			uint16_t ADate = Func::FromBcdToDec(C_I2C.read());
			uint16_t AMonth = Func::FromBcdToDec(C_I2C.read());
			uint16_t AYear = Func::FromBcdToDec(C_I2C.read()) + 2000;

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
			C_I2C.beginTransmission( C_ADDRESS );

			C_I2C.write((byte)0); // start at location 0

			C_I2C.write( ( ( Halt & 1 ) << 7 ) | Func::FromDecToBcd( ASecond ));
			C_I2C.write(Func::FromDecToBcd( AMinute ));
			C_I2C.write(Func::FromDecToBcd( AHour ));
			C_I2C.write(Func::FromDecToBcd( AWeekDay ));
			C_I2C.write(Func::FromDecToBcd( ADay ));
			C_I2C.write(Func::FromDecToBcd( AMonth ));
			C_I2C.write(Func::FromDecToBcd( AYear % 100 ));

			C_I2C.endTransmission();
		}


	protected:
		Mitov::TDateTime FLastDateTime;

	public:
		inline void ClockInputPin_o_Receive( void *_Data )
		{
			ReadTime( true );
		}

		void SetInputPin_o_Receive( void *_Data )
		{
//			if( WriteProtect )
//				return;

			Mitov::TDateTime &ADateTime = *(Mitov::TDateTime *)_Data;
			SetDateTime( ADateTime );
		}

	protected:
		uint8_t readRegister(const uint8_t reg) 
		{
			C_I2C.beginTransmission( C_ADDRESS );
			C_I2C.write( reg );	
			C_I2C.endTransmission();

			C_I2C.requestFrom( C_ADDRESS, (uint8_t)1 );

			return C_I2C.read();
		}

		void writeRegister( const uint8_t reg, const uint8_t value ) 
		{
			C_I2C.beginTransmission( C_ADDRESS );
			C_I2C.write( reg );	
			C_I2C.write( value );	
			C_I2C.endTransmission();
		}

		uint8_t hourFromRegisterValue(const uint8_t value) 
		{
			uint8_t adj;
			if (value & 128)  // 12-hour mode
				adj = 12 * ((value & 32) >> 5);

			else           // 24-hour mode
				adj = 10 * ((value & (32 + 16)) >> 4);

			return (value & 15) + adj;
		}

		void UpdateClockFrequency()
		{
			const uint8_t CValues [] = { 0x00, 0x80, 0x40, 0x41, 0x42, 0x43 };
			writeRegister( kClockReg, CValues[ ClockFrequency ] );
		}

		void ReadTime( bool AFromClock )
		{
			if( ! Enabled )
				return;

			if( ! OutputPin.IsConnected() )
				return;

			C_I2C.beginTransmission( C_ADDRESS );
			C_I2C.write((byte)0);	
			C_I2C.endTransmission();

			C_I2C.requestFrom( C_ADDRESS, (byte)7);
			uint16_t ASecond = Func::FromBcdToDec(C_I2C.read() & 0x7F);
			uint16_t AMinute = Func::FromBcdToDec(C_I2C.read());
			uint16_t AHour = hourFromRegisterValue( C_I2C.read() );
			uint16_t ADay = C_I2C.read();
			uint16_t ADate = Func::FromBcdToDec(C_I2C.read());
			uint16_t AMonth = Func::FromBcdToDec(C_I2C.read());
			uint16_t AYear = Func::FromBcdToDec(C_I2C.read()) + 2000;

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

		void UpdateHalt()
		{
			uint8_t sec = readRegister( kSecondReg );

			sec &= ~(1 << 7);
			sec |= ( ( Halt & 1 ) << 7 );
			writeRegister( kSecondReg, sec );
		}

	public:
		inline void SystemInit()
		{
			UpdateHalt();
			UpdateClockFrequency();
		}

		inline void SystemLoopBegin( unsigned long currentMicros )
		{
			ReadTime( false );
		}

	public:
		RTC_DS1307() :
			Enabled( true ),
			Halt( false ),
			ClockFrequency( rtc1307fDisabledLow )
		{
		}
	};
}

#ifdef __TEST_FOR_DEBUG_PRINTS__
#undef Serial
#endif

#endif // _MITOV_RTC_DS1307_h
