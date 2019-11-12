////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2018 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_BASIC_I2C_h
#define _MITOV_BASIC_I2C_h

#include <Mitov.h>
#include <Wire.h>

#ifndef BUFFER_LENGTH
	#define BUFFER_LENGTH 256
#endif

#if defined( VISUINO_MAPLE_MINI)
	typedef HardWire TwoWire;
#endif

namespace Mitov
{
	namespace I2C
	{
		const uint16_t	I2C_DEFAULT_READ_TIMEOUT	= 1000;
//	public:
		template<typename T> bool ReadBytes( T AWire, uint8_t devAddr, uint8_t regAddr, uint8_t length, void *data, uint16_t timeout = I2C_DEFAULT_READ_TIMEOUT )
		{
			int8_t count = 0;
			uint32_t t1 = millis();

            // Arduino v1.0.1+, Wire library
            // Adds official support for repeated start condition, yay!

            // I2C/TWI subsystem uses internal buffer that breaks with large data requests
            // so if user requests more than BUFFER_LENGTH bytes, we have to do it in
            // smaller chunks instead of all at once
			uint8_t *AByteData = (uint8_t *)data;
            for (uint16_t k = 0; k < length; k += MitovMin<uint16_t>(length, BUFFER_LENGTH ))
			{
                AWire.beginTransmission(devAddr);
                AWire.write(regAddr);
                AWire.endTransmission();
                AWire.beginTransmission(devAddr);
                AWire.requestFrom(devAddr, (uint8_t)MitovMin<int16_t>(length - k, int( BUFFER_LENGTH )));

				regAddr += BUFFER_LENGTH;
        
                for (; AWire.available() && (timeout == 0 || millis() - t1 < timeout) && ( count < length ); count++)
                    *AByteData ++ = AWire.read();
        
                AWire.endTransmission();
            }

			return ( count == length );
		}

		template<typename T> bool ReadBytes_16bitAddress( T AWire, uint8_t devAddr, bool ABigIndianAddr, uint16_t regAddr, uint8_t length, void *data, uint16_t timeout = I2C_DEFAULT_READ_TIMEOUT )
		{
			int8_t count = 0;
			uint32_t t1 = millis();

            // Arduino v1.0.1+, Wire library
            // Adds official support for repeated start condition, yay!

            // I2C/TWI subsystem uses internal buffer that breaks with large data requests
            // so if user requests more than BUFFER_LENGTH bytes, we have to do it in
            // smaller chunks instead of all at once
			uint8_t *AByteData = (uint8_t *)data;
            for (uint16_t k = 0; k < length; k += MitovMin<uint16_t>(length, BUFFER_LENGTH )) 
			{
                AWire.beginTransmission(devAddr);
				if( ABigIndianAddr )
				{
					AWire.write(regAddr >> 8);
					AWire.write(regAddr & 0xFF);
				}
				else
				{
					AWire.write(regAddr & 0xFF);
					AWire.write(regAddr >> 8);
				}

                AWire.endTransmission();
                AWire.beginTransmission(devAddr);
                AWire.requestFrom(devAddr, (uint8_t)MitovMin<int16_t>(length - k, int( BUFFER_LENGTH )));

				regAddr += BUFFER_LENGTH;
        
                for (; AWire.available() && (timeout == 0 || millis() - t1 < timeout) && ( count < length ); count++)
                    *AByteData++ = AWire.read();
        
                AWire.endTransmission();
            }

			return ( count == length );
		}

		template<typename T> void WriteByte( T AWire, uint8_t devAddr, uint8_t regAddr, uint8_t AValue )
		{
//			Serial.print( "Address: " ); Serial.print( devAddr, HEX ); Serial.print( " Reg: " ); Serial.print( regAddr, HEX );  Serial.print( " = " ); Serial.println( AValue, BIN );
			AWire.beginTransmission( devAddr );
			AWire.write( regAddr );
			AWire.write( AValue );
			AWire.endTransmission();
		}

		template<typename T> uint8_t ReadByte( T AWire, uint8_t devAddr, uint8_t regAddr )
		{
			AWire.beginTransmission( devAddr );
			AWire.write( regAddr );
			AWire.endTransmission();
			AWire.requestFrom( devAddr, (byte)1 );

			return AWire.read();

/*
			AWire.beginTransmission( devAddr );
			AWire.write( regAddr );
			AWire.endTransmission();
			AWire.requestFrom( devAddr, (byte)1 );

			return AWire.read();
*/
		}

		template<typename T> void WriteByte_16bitAddress( T AWire, uint8_t devAddr, bool ABigIndianAddr, uint16_t regAddr, uint8_t AValue )
		{
//			Serial.print( "Address: " ); Serial.print( devAddr, HEX ); Serial.print( " Reg: " ); Serial.print( regAddr, HEX );  Serial.print( " = " ); Serial.println( AValue, BIN );
			AWire.beginTransmission( devAddr );
			if( ABigIndianAddr )
			{
				AWire.write(regAddr >> 8);
				AWire.write(regAddr & 0xFF);
			}

			else
			{
				AWire.write(regAddr & 0xFF);
				AWire.write(regAddr >> 8);
			}

			AWire.write( AValue );
			AWire.endTransmission();
		}

		template<typename T> void WriteBytes_16bitAddress( T AWire, uint8_t devAddr, bool ABigIndianAddr, uint16_t regAddr, uint8_t length, void *data )
		{
//			Serial.print( "Address: " ); Serial.print( devAddr, HEX ); Serial.print( " Reg: " ); Serial.print( regAddr, HEX );  Serial.print( " = " ); Serial.println( AValue, BIN );
			AWire.beginTransmission( devAddr );
			if( ABigIndianAddr )
			{
				AWire.write(regAddr >> 8);
				AWire.write(regAddr & 0xFF);
			}

			else
			{
				AWire.write(regAddr & 0xFF);
				AWire.write(regAddr >> 8);
			}

			AWire.write( (byte *)data, length );
			AWire.endTransmission();
		}

	};
//---------------------------------------------------------------------------
	template<TwoWire &IWIRE> class ArduinoI2CInput
	{
	public:
		const static bool Enabled = true;

	public:
		void Print( const Mitov::String AValue )
		{
#if ( defined(VISUINO_ESP32) || defined( VISUINO_CHIPKIT ) || defined( VISUINO_MAPLE_MINI) )
			IWIRE.write( (uint8_t *)AValue.c_str(), AValue.length());
#else
			IWIRE.write( AValue.c_str(), AValue.length());
#endif
			IWIRE.write( '\r' );
			IWIRE.write( '\n' );
		}

		void Print( float AValue )
		{
			char AText[ 16 ];
			dtostrf( AValue,  1, 2, AText );
			Print( Mitov::String( AText ));
		}

		void Print( int32_t AValue )
		{
			char AText[ 16 ];
			ltoa( AValue, AText, 10 );
			Print( Mitov::String( AText ));
		}

		void Print( uint32_t AValue )
		{
			char AText[ 16 ];
			ltoa( AValue, AText, 10 );
			Print( Mitov::String( AText ));
		}

		template<typename T> void PrintChar( T AValue )
		{
			IWIRE.write( AValue );
		}

		void Write( uint8_t *AData, uint32_t ASize )
		{
			IWIRE.write( AData, ASize );
		}

	};
//---------------------------------------------------------------------------
	template<TwoWire &IWIRE> class ArduinoI2COutput
	{
	public:
		static OpenWire::SourcePin	OutputPin;
		static OpenWire::SourcePin	RequestOutputPin;

	public:
#if defined( VISUINO_ARDUINO_101 ) || defined ( VISUINO_ESP32 ) || defined( VISUINO_MAPLE_MINI ) || defined( VISUINO_BBC_MICRO_BIT )
		inline void SystemLoopBegin( unsigned long currentMicros )
		{
			if( IWIRE.available() )
			{
				int AData = IWIRE.read();
				if( AData >= 0 )
				{
					byte AByte = AData;
					OutputPin.SendValue( Mitov::TDataBlock( 1, &AByte ));
				}
			}
		}
#else // VISUINO_ARDUINO_101
		inline void SystemInit()
		{
			IWIRE.onReceive( receiveEvent );
			IWIRE.onRequest( requestEvent );
		}

	protected:
		static void receiveEvent(int howMany) 
		{
//			Serial.println( "Test1" );
			while( 1 < IWIRE.available())
			{ // loop through all but the last
//  				Serial.println( "Test2" );
				byte AByte = IWIRE.read();
				OutputPin.SendValue( Mitov::TDataBlock( 1, &AByte ));
			}

			byte AByte = IWIRE.read();
			OutputPin.SendValue( Mitov::TDataBlock( 1, &AByte ));
		}

		static void requestEvent()
		{
			RequestOutputPin.Notify( nullptr );
		}

#endif // VISUINO_ARDUINO_101

	};
//---------------------------------------------------------------------------
template<TwoWire &IWIRE> OpenWire::SourcePin	ArduinoI2COutput<IWIRE>::OutputPin;
template<TwoWire &IWIRE> OpenWire::SourcePin	ArduinoI2COutput<IWIRE>::RequestOutputPin;
//---------------------------------------------------------------------------
	template<uint8_t C_ADDRESS, typename T_I2C, T_I2C &C_I2C> class I2CDevice
	{
	public:
		const static bool Enabled = true;

	public:
		inline T_I2C	&GetWire() { return C_I2C; }
		inline uint8_t	GetAddress() { return C_ADDRESS; }
	/*
		template<typename T> void Print( T AValue )
		{
		}
	*/
		void Print( const Mitov::String AValue )
		{
			C_I2C.beginTransmission( C_ADDRESS );
#if ( defined(VISUINO_ESP32) || defined( VISUINO_CHIPKIT ) || defined( VISUINO_MAPLE_MINI) )
			C_I2C.write( (uint8_t *)AValue.c_str(), AValue.length());
#else
			C_I2C.write( AValue.c_str(), AValue.length());
#endif
			C_I2C.write( '\r' );
			C_I2C.write( '\n' );
			C_I2C.endTransmission();
		}

		void Print( float AValue )
		{
			char AText[ 16 ];
			dtostrf( AValue,  1, 2, AText );
			Print( Mitov::String( AText ));
		}

		void Print( int32_t AValue )
		{
			char AText[ 16 ];
			ltoa( AValue, AText, 10 );
			Print( Mitov::String( AText ));
		}

		void Print( uint32_t AValue )
		{
			char AText[ 16 ];
			ltoa( AValue, AText, 10 );
			Print( Mitov::String( AText ));
		}

		template<typename T> void PrintChar( T AValue )
		{
			C_I2C.beginTransmission( C_ADDRESS );
			C_I2C.write( AValue );
			C_I2C.endTransmission();
		}

		void Write( uint8_t *AData, uint32_t ASize )
		{
			C_I2C.beginTransmission( C_ADDRESS );
			C_I2C.write( AData, ASize );
			C_I2C.endTransmission();
		}

//	protected:
//		virtual void DoReceive( void *_Data ) override
//		{
//			FWire.beginTransmission( Address );
//		}
	};
//---------------------------------------------------------------------------
	template<typename T_OWNER, T_OWNER &C_INSTANCE> class I2CDeviceRequest
	{
	public:
		OpenWire::SourcePin	OutputPin;

	public:
		uint8_t	Size = 1;

	public:
		void ClockInputPin_o_Receive( void *_Data )
		{
			C_INSTANCE.GetWire().requestFrom( C_INSTANCE.GetAddress(), Size );

			for( int i = 0; i < Size; ++i )
				if( C_INSTANCE.GetWire().available() )
				{
					byte AByte = C_INSTANCE.GetWire().read();
//					Serial.println( AByte );
					OutputPin.SendValue( Mitov::TDataBlock( 1, &AByte ));
				}

		}
	};
//---------------------------------------------------------------------------
/*
	void Basic_MultiChannel_SourceI2C::DoClockReceive( void * )
	{
		if( ! FModified )
			return;

		for( int i =0; i < FChannels.size(); ++i )
			FChannels[ i ]->SendOutput();
	}
*/
//---------------------------------------------------------------------------
/*
	void Basic_MultiChannel_SourceI2C::SystemInit()
	{
		inherited::SystemInit();

		for( int i =0; i < FChannels.size(); ++i )
			FChannels[ i ]->InitChannel();
	}
*/
//---------------------------------------------------------------------------
}
#endif
