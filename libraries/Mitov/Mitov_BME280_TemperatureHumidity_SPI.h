////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2018 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_BME280_TEMPERATUREHUMIDITY_SPI_h
#define _MITOV_BME280_TEMPERATUREHUMIDITY_SPI_h

#include <Mitov_BME280_TemperatureHumidity.h>
#include <SPI.h>

#ifdef __TEST_FOR_DEBUG_PRINTS__
#define Serial UNGUARDED DEBUG PRINT!!!
#endif


namespace Mitov
{
//---------------------------------------------------------------------------
	template<typename T_SPI, T_SPI &C_SPI> class BME280_TemperatureHumiditySPI
	{
	public:
		OpenWire::SourcePin	ChipSelectOutputPin;

	protected:
		uint8_t read8(uint8_t reg)
		{
			C_SPI.beginTransaction(SPISettings(500000, MSBFIRST, SPI_MODE0));
			ChipSelectOutputPin.SendValue<bool>( false );

			C_SPI.transfer( reg | 0x80 ); // read, bit 7 high
			uint8_t AValue = C_SPI.transfer( 0 );

			ChipSelectOutputPin.SendValue<bool>( true );
			C_SPI.endTransaction(); 

			return AValue;
		}

		uint16_t read16(uint8_t reg)
		{
			C_SPI.beginTransaction(SPISettings(500000, MSBFIRST, SPI_MODE0));
			ChipSelectOutputPin.SendValue<bool>( false );

			C_SPI.transfer( reg | 0x80 ); // read, bit 7 high
			uint16_t AValue = C_SPI.transfer( 0 );
			AValue = ( AValue << 8 ) | C_SPI.transfer( 0 );

			ChipSelectOutputPin.SendValue<bool>( true );
			C_SPI.endTransaction(); 

			return AValue;
		}

		uint16_t read16_LE(uint8_t reg)
		{
			C_SPI.beginTransaction(SPISettings(500000, MSBFIRST, SPI_MODE0));
			ChipSelectOutputPin.SendValue<bool>( false );

			C_SPI.transfer( reg | 0x80 ); // read, bit 7 high
			uint16_t AValue = C_SPI.transfer( 0 );
			AValue |= uint16_t( C_SPI.transfer( 0 ) ) << 8;

			ChipSelectOutputPin.SendValue<bool>( true );
			C_SPI.endTransaction(); 

			return AValue;
		}

		uint32_t read24(uint8_t reg)
		{
			C_SPI.beginTransaction(SPISettings(500000, MSBFIRST, SPI_MODE0));
			ChipSelectOutputPin.SendValue<bool>( false );

			C_SPI.transfer( reg | 0x80 ); // read, bit 7 high
			uint32_t AValue = C_SPI.transfer( 0 );
			AValue <<= 8;
			AValue |= C_SPI.transfer( 0 );
			AValue <<= 8;
			AValue |= C_SPI.transfer( 0 );

			ChipSelectOutputPin.SendValue<bool>( true );
			C_SPI.endTransaction(); 

			return AValue;
		}

		void write8(uint8_t reg, uint8_t value)
		{
			C_SPI.beginTransaction(SPISettings(500000, MSBFIRST, SPI_MODE0));
			ChipSelectOutputPin.SendValue<bool>( false );

			C_SPI.transfer( reg | 0x80 ); // read, bit 7 high
			C_SPI.transfer( value );

			ChipSelectOutputPin.SendValue<bool>( true );
			C_SPI.endTransaction(); 
		}

	};
//---------------------------------------------------------------------------
}

#ifdef __TEST_FOR_DEBUG_PRINTS__
#undef Serial
#endif

#endif
