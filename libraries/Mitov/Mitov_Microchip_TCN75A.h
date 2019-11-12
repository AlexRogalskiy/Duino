////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2018 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_MICROCHIP_TCN75A_h
#define _MITOV_MICROCHIP_TCN75A_h

#include <Mitov.h>

#ifdef __TEST_FOR_DEBUG_PRINTS__
#define Serial UNGUARDED DEBUG PRINT!!!
#endif

namespace Mitov
{
//---------------------------------------------------------------------------
	class TArduinoMicrochipTCN75AAlert
	{
	public:
		uint8_t QueueSize : 3;
		bool	InterruptMode : 1;
		bool	ActiveLow : 1;
		float	Temperature = 80.0;
		float	Hysteresis = 5.0;

	public:
		TArduinoMicrochipTCN75AAlert() :
			QueueSize( 1 ),
			InterruptMode( false ),
			ActiveLow( true )
		{
		}
	};
//---------------------------------------------------------------------------
	template<uint8_t C_ADDRESS, typename T_I2C, T_I2C &C_I2C> class MicrochipTCN75A
	{
	public:
		OpenWire::TypedStartSourcePin<float>	OutputPin;

	public:
		inline void ClockInputPin_o_Receive( void *_Data )
		{
			ReadData();
		}

	public:
		bool Enabled : 1;
		uint8_t Resolution : 4;
		bool InFahrenheit : 1;
		bool ShutDown : 1;
		TArduinoMicrochipTCN75AAlert Alert;

	protected:
		 static const uint8_t TEMPERATURE_REG = 0x00;
		 static const uint8_t CONFIG_REG = 0x01;
		 static const uint8_t HYSTERESIS_REG = 0x02;
		 static const uint8_t LIMIT_REG = 0x03;

	public:
		void	UpdateLimits()
		{
			WriteRegister16Temp( LIMIT_REG, Alert.Temperature );
			UpdateHysteresis();
		}

		void	UpdateHysteresis()
		{
			float AMinValue = Alert.Temperature - Alert.Hysteresis;
			if( AMinValue < -273.15 )
				AMinValue = -273.15;

			WriteRegister16Temp( HYSTERESIS_REG, AMinValue );
		}

		void	UpdateConfig()
		{
			uint8_t	AValue =	(( Resolution - 9 ) & 0b11 << 5 );
			switch( Alert.QueueSize )
			{
				case 2:		AValue |= 0b01000; break;
				case 4:		AValue |= 0b10000; break;
				default:	AValue |= 0b11000;
			}

			if( ! Alert.ActiveLow )
				AValue |= 0b100;

			if( Alert.InterruptMode )
				AValue |= 0b10;

			if( ShutDown )
				AValue |= 0b1;

//			Serial.println( AValue, BIN );

			WriteRegister8( CONFIG_REG, AValue );
		}

	public:
		void ReadData()
		{
			if( ! Enabled )
				 return;

			C_I2C.beginTransmission( C_ADDRESS );
			C_I2C.write( TEMPERATURE_REG );
			C_I2C.endTransmission();

			C_I2C.requestFrom( C_ADDRESS, uint8_t( 2 ));
			if (Wire.available() != 2 )
				return;

			int32_t AValue = C_I2C.read();
			AValue <<= 8;
			AValue |= Wire.read();

//			AValue = 0x7FFFFFFF;
//			AValue = 0x80000000;

//			AValue /= ( 16 << ( 3 - ( Resolution - 9 )));
			AValue /= 16;
//			Serial.println( AValue );

			float ATemp = AValue * 0.0625;
			if( InFahrenheit )
				ATemp = ATemp * ( 9.0/5.0 ) + 32.0;

//			AValue = ( ATemp / 0.0625 ) + 0.5;
//			AValue << 8;
//			Serial.println( AValue );
//			Serial.println( "" );

			OutputPin.Notify( &ATemp );
		}

	protected:
		void WriteRegister8( uint8_t ARegister, uint8_t AValue )
		{
			 C_I2C.beginTransmission( C_ADDRESS );
			 C_I2C.write( ARegister );
			 C_I2C.write( AValue );
			 C_I2C.endTransmission();
		}

		void WriteRegister16Temp( uint8_t ARegister, float ATemp )
		{
			int16_t AValue = ( ATemp / 0.0625 ) + 0.5;
			AValue << 8;
			AValue &= 0xFF80;
			WriteRegister16( ARegister, AValue );
		}

		void WriteRegister16( uint8_t ARegister, uint16_t AValue )
		{
			 C_I2C.beginTransmission( C_ADDRESS );
			 C_I2C.write( ARegister );
			 C_I2C.write( uint8_t( AValue >> 8 ) );
			 C_I2C.write( uint8_t( AValue ) );
			 C_I2C.endTransmission();
		}

	public:
		inline void SystemStart()
		{
			UpdateConfig();
		}

		inline void SystemLoopBegin( unsigned long currentMicros )
		{
			ReadData();
		}

	public:
		MicrochipTCN75A() :
			Enabled( true ),
			Resolution( 12 ),
			InFahrenheit( false ),
			ShutDown( false )
		{
		}
	};
//---------------------------------------------------------------------------
}

#ifdef __TEST_FOR_DEBUG_PRINTS__
#undef Serial
#endif

#endif
