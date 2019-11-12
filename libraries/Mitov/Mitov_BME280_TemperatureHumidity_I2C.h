////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2018 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_BME280_TEMPERATUREHUMIDITY_I2C_h
#define _MITOV_BME280_TEMPERATUREHUMIDITY_I2C_h

#include <Wire.h> //I2C Arduino Library
#include <Mitov_BME280_TemperatureHumidity.h>
#include <Mitov_Basic_I2C.h>

namespace Mitov
{
//---------------------------------------------------------------------------
	template<typename T_I2C, T_I2C &C_I2, uint8_t C_ADDRESSC> class BME280_TemperatureHumidityI2C
	{
	protected:
		uint8_t read8(uint8_t reg)
		{
			C_I2.beginTransmission( C_ADDRESSC );
			C_I2.write( reg );
			C_I2.endTransmission();
			C_I2.requestFrom( C_ADDRESSC, (uint8_t)1 );
			return C_I2.read();
		}

		uint16_t read16(uint8_t reg)
		{
			C_I2.beginTransmission( C_ADDRESSC );
			C_I2.write( reg );
			C_I2.endTransmission();
			C_I2.requestFrom( C_ADDRESSC, (uint8_t)2 );
			uint16_t AValue = C_I2.read();
			return ( AValue << 8 ) | C_I2.read();
		}

		uint16_t read16_LE(uint8_t reg)
		{
			C_I2.beginTransmission( C_ADDRESSC );
			C_I2.write( reg );
			C_I2.endTransmission();
			C_I2.requestFrom( C_ADDRESSC, (uint8_t)2 );
			uint16_t AValue = C_I2.read();
			return AValue | ( uint16_t( C_I2.read() ) << 8 );
		}

		uint32_t read24(uint8_t reg)
		{
			C_I2.beginTransmission( C_ADDRESSC );
			C_I2.write( reg );
			C_I2.endTransmission();
			C_I2.requestFrom( C_ADDRESSC, (uint8_t)3 );
    
			uint32_t AValue = C_I2.read();
			AValue <<= 8;
			AValue |= C_I2.read();
			AValue <<= 8;
			AValue |= C_I2.read();

			return AValue;
		}

		void write8(uint8_t reg, uint8_t value)
		{
			C_I2.beginTransmission( C_ADDRESSC );
			C_I2.write((uint8_t)reg);
			C_I2.write((uint8_t)value);
			C_I2.endTransmission();
		}

	};
//---------------------------------------------------------------------------
}

#endif
