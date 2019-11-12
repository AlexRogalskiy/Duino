////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2018 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_ARDUINO_SPI_h
#define _MITOV_ARDUINO_SPI_h

#include <Mitov.h>
#include <SPI.h>

namespace Mitov
{
	template<SPIClass &ISPI> class ArduinoSPI
	{
	public:
		inline uint16_t transfer16(uint16_t data)
		{
			return ISPI.transfer16( data );
		}

		inline uint8_t transfer(uint8_t data)
		{
//			Serial.println( data, HEX );
			return ISPI.transfer( data );
		}

		inline void transfer(void *buf, size_t count)
		{
#if defined(VISUINO_ESP8266) || defined(VISUINO_ESP32)
			ISPI.writeBytes( (uint8_t*)buf, count );
#else // VISUINO_ESP8266
			ISPI.transfer( buf, count );
#endif // VISUINO_ESP8266
		}

		inline void beginTransaction(SPISettings settings)
		{
		#if defined(SPI_HAS_TRANSACTION)
			ISPI.beginTransaction( settings );
		#endif
		}

		inline void endTransaction()
		{
		#if defined(SPI_HAS_TRANSACTION)
			ISPI.endTransaction();
		#endif
		}

		inline void setDataMode( uint8_t dataMode )
		{
			ISPI.setDataMode( dataMode );
		}

/*
		inline void setBitOrder( uint8_t bitOrder )
		{
			ISPI.setBitOrder( bitOrder );
		}
*/
		inline void setClockDivider( uint8_t clockDiv )
		{
			ISPI.setClockDivider( clockDiv );
		}

		inline void usingInterrupt( uint8_t interruptNumber )
		{
#if ! ( defined(VISUINO_ESP8266) || defined(VISUINO_ESP32) )
			ISPI.usingInterrupt( interruptNumber );
#endif
		}

	};
//---------------------------------------------------------------------------
}
#endif
