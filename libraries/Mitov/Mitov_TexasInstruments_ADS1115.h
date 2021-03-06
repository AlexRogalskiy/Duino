////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2018 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_TEXAS_INSTRUMENTS_ADS1115_h
#define _MITOV_TEXAS_INSTRUMENTS_ADS1115_h

#include <Mitov.h>

#ifdef __TEST_FOR_DEBUG_PRINTS__
#define Serial UNGUARDED DEBUG PRINT!!!
#endif

namespace Mitov
{
	enum TArduinoTexasInstrumentsADS1115Range { adsRange_6_144, adsRange_4_096, adsRange_2_048, adsRange_1_024, adsRange_0_512, adsRange_0_256 };
//---------------------------------------------------------------------------
	class TArduinoTexasInstrumentsADS111XComparator
	{
	public:
		bool	Enabled : 1;
		bool	WindowMode : 1;
		bool	Latching : 1;
		bool	ActiveHigh : 1;
		bool	UseAsReady : 1;
		uint8_t	QueueSize : 2;
		float	ThresholdHigh = 0.9;
		float	ThresholdLow = 0.1;

	public:
		TArduinoTexasInstrumentsADS111XComparator() :
			Enabled( false ),
			WindowMode( false ),
			Latching( false ),
			ActiveHigh( false ),
			UseAsReady( true ),
			QueueSize( 1 )
		{
		}
	};
//---------------------------------------------------------------------------
	template<uint8_t C_ADDRESS, typename T_I2C, T_I2C &C_I2C> class TexasInstrumentsADS1115
	{
	public:
		OpenWire::SourcePin	OutputPin;

	public:
		TArduinoTexasInstrumentsADS111XComparator	Comparator;

		bool	Enabled : 1;
		TArduinoTexasInstrumentsADS1115Range	Range : 3;
		uint8_t	InputIndex : 2;
		bool	Differential : 1;
		bool	SingleShot : 1;

	public:
		bool	ClockInputPin_o_IsConnected : 1;

	protected:
		uint8_t	FSampleRate : 3;

	protected:
		static const uint8_t ADS1015_REG_POINTER_CONVERT = 0x00;
		static const uint8_t ADS1015_REG_POINTER_CONFIG = 0x01;
		static const uint8_t ADS1015_REG_POINTER_LOWTHRESH = 0x02;
		static const uint8_t ADS1015_REG_POINTER_HITHRESH = 0x03;

	public:
		void SetComparatorQueueSize( uint32_t AValue )
		{
			if( AValue > 4 )
				AValue = 4;

			if( AValue == 3 )
				AValue = 2;

			if( Comparator.QueueSize != AValue )
			{
				Comparator.QueueSize = AValue;
				UpdateConfig();
			}
		}

		void SetSampleRate( uint32_t AValue )
		{
			const uint16_t CSampleRates[] = { 8, 16, 32, 64, 128, 250, 475, 860 };
			uint8_t ASampleRate = 0;
			if( AValue > 860 )
				ASampleRate = 7;

			else
			{
				for( int i = 0; i < sizeof( CSampleRates ) / sizeof( CSampleRates[ 0 ] ); ++i )
					if( CSampleRates[ i ] >= AValue )
					{
						ASampleRate = i;
						break;
					}

			}

			if( FSampleRate != ASampleRate )
			{
				FSampleRate = ASampleRate;
				UpdateConfig();
			}
		}

	public:
		void UpdateConfig()
		{
			uint8_t AValue =	( uint16_t( Range ) << 9 ) |
								( Comparator.Latching ? ( uint16_t( 1 ) << 2 ) : 0 ) | 
								( Comparator.ActiveHigh ? ( uint16_t( 1 ) << 3 ) : 0 ) | 
								( Comparator.WindowMode ? ( uint16_t( 1 ) << 4 ) : 0 ) | 
								( uint16_t( FSampleRate ) << 5 ) |
								( ( SingleShot && ClockInputPin_o_IsConnected ) ? uint16_t( 1 ) << 8 : 0 ) |
								( Differential ? ( uint16_t( 1 ) << 14 ) : 0 ) | 
								( uint16_t( InputIndex ) << 12 );

			if( ! Comparator.Enabled )
				AValue |= 0b11;

			else
			{
				if( Comparator.QueueSize == 4 )
					AValue |= 0x10;

				else
					AValue |= Comparator.QueueSize - 1;
			}

			writeRegister( ADS1015_REG_POINTER_CONFIG, AValue );

			if( Comparator.UseAsReady )
			{
				writeRegister( ADS1015_REG_POINTER_LOWTHRESH, 0x8000 );
				writeRegister( ADS1015_REG_POINTER_HITHRESH, 0x7FFF );
			}
			else
			{
				uint16_t AHigh = Comparator.ThresholdHigh * 0x7FFF + 0.5;
				uint16_t ALow = Comparator.ThresholdLow * 0x7FFF + 0.5;
				if( ALow > AHigh )
				{
					writeRegister( ADS1015_REG_POINTER_LOWTHRESH, AHigh );
					writeRegister( ADS1015_REG_POINTER_HITHRESH, ALow );
				}
				else
				{
					writeRegister( ADS1015_REG_POINTER_LOWTHRESH, ALow );
					writeRegister( ADS1015_REG_POINTER_HITHRESH, AHigh );
				}
			}
		}

	protected:
		void writeRegister( uint8_t reg, uint16_t value ) 
		{
			C_I2C.beginTransmission( C_ADDRESS );
			C_I2C.write((uint8_t)reg);
			C_I2C.write((uint8_t)(value>>8));
			C_I2C.write((uint8_t)(value & 0xFF));
			C_I2C.endTransmission();
		}

		int16_t readRegister() 
		{
			C_I2C.beginTransmission( C_ADDRESS );
			C_I2C.write( ADS1015_REG_POINTER_CONVERT );
			C_I2C.endTransmission();
			C_I2C.requestFrom( C_ADDRESS, (uint8_t)2 );
			return ((int16_t( int8_t( C_I2C.read() )) << 8) | C_I2C.read());  
		}

	public:
		void ClockInputPin_o_Receive( void *_Data )
		{
			if( ! Enabled )
				return;

			int16_t	AValue = readRegister(); // >> 4;
//			Serial.println( AValue );

			float AFloatValue = float( AValue ) / 0x7FFF;

			OutputPin.Notify( &AFloatValue );

//			Serial.println( "ClockInputPin_o_Receive" );
//			CurrentValue = InitialValue;
//			UpdateValue();
		}

	public:
		inline void SystemStart()
		{
			UpdateConfig();
		}

		inline void SystemLoopBegin( unsigned long currentMicros )
		{
			ClockInputPin_o_Receive( nullptr );
		}

	public:
		TexasInstrumentsADS1115( uint8_t ASampleRate ) :
			Enabled( true ),
			Range( adsRange_2_048 ),
			InputIndex( 0 ),
			Differential( true ),
			SingleShot( false ),
			ClockInputPin_o_IsConnected( false ),
			FSampleRate( ASampleRate )
		{
		}
	};
//---------------------------------------------------------------------------
}

#ifdef __TEST_FOR_DEBUG_PRINTS__
#undef Serial
#endif

#endif
