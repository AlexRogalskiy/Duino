////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2018 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_DHT_SENSOR_h
#define _MITOV_DHT_SENSOR_h

#include <Mitov.h>

#ifdef __TEST_FOR_DEBUG_PRINTS__
#define Serial UNGUARDED DEBUG PRINT!!!
#endif

namespace Mitov
{
//---------------------------------------------------------------------------
	namespace DHTSensor
	{
		uint32_t Maxcycles;
	};

	template<int PIN_NUMBER> class DHT11Sensor
	{
	public:
		OpenWire::SourcePin			TemperatureOutputPin;
		OpenWire::SourcePin			HumidityOutputPin;

	public:
		bool	ClockInputPin_o_IsConnected : 1;

	public:
		bool	Enabled : 1;
		bool	InFahrenheit : 1;

	protected:
		bool _lastresult : 1;
		uint8_t data[5];
		uint32_t FLastReadTime;

#ifdef __AVR
		// Use direct GPIO access on an 8-bit AVR so keep track of the port and bitmask
		// for the digital pin connected to the DHT.  Other platforms will use digitalRead.
		uint8_t _bit, _port;
#endif

//		DHT	*FSensor;

	public:
		inline void SystemInit()
		{
			FLastReadTime = millis() - 2000;
			pinMode( PIN_NUMBER, INPUT_PULLUP );
//			FSensor = new DHT( PIN_NUMBER, SENSOR_TYPE );
//			FSensor->begin();
		}

		inline void SystemStart() {} // Placeholder

		inline void SystemLoopBegin( unsigned long currentMicros )
		{
#ifdef _MITOV_DHT11_CLOCK_PIN_
			if( ! ClockInputPin_o_IsConnected )
#endif
				ReadSensor();
		}

	protected:
		void ReadSensor()
		{
			if( ! TryRead())
				return;

			if( TemperatureOutputPin.IsConnected() )
			{
				float   AValue = data[2];
				if( InFahrenheit )
					AValue = Func::ConvertCtoF( AValue );

				TemperatureOutputPin.Notify( &AValue );
			}

			if( HumidityOutputPin.IsConnected() )
			{
				float   AValue = data[0];
				HumidityOutputPin.Notify( &AValue );
			}
		}

		uint32_t expectPulse(bool level) 
		{
			uint32_t count = 0;
			// On AVR platforms use direct GPIO port access as it's much faster and better
			// for catching pulses that are 10's of microseconds in length:
#ifdef __AVR
			uint8_t portState = level ? _bit : 0;
			while ((*portInputRegister(_port) & _bit) == portState) 
			// Otherwise fall back to using digitalRead (this seems to be necessary on ESP8266
			// right now, perhaps bugs in direct port access functions?).
#else
			while (digitalRead(PIN_NUMBER) == level) 
#endif
			{
				if (count++ >= DHTSensor::Maxcycles ) 
					return 0; // Exceeded timeout, fail.
			}

			return count;
		}

		bool TryRead()
		{
			if( ! Enabled )
				return false;

			uint32_t currenttime = millis();
			if ( (currenttime - FLastReadTime) < 2000 ) 
				return _lastresult; // return last correct measurement

			FLastReadTime = currenttime;

			// Reset 40 bits of received data to zero.
			data[0] = data[1] = data[2] = data[3] = data[4] = 0;

			// Send start signal.  See DHT datasheet for full signal diagram:
			//   http://www.adafruit.com/datasheets/Digital%20humidity%20and%20temperature%20sensor%20AM2302.pdf

			// Go into high impedence state to let pull-up raise data line level and
			// start the reading process.
			digitalWrite( PIN_NUMBER, HIGH);
			delay(250);

			// First set data line low for 20 milliseconds.
			pinMode( PIN_NUMBER, OUTPUT);
			digitalWrite( PIN_NUMBER, LOW);
			delay(20);

			uint32_t cycles[80];
			{
				// Turn off interrupts temporarily because the next sections are timing critical
				// and we don't want any interruptions.
				InterruptLock lock;

				// End the start signal by setting data line high for 40 microseconds.
				digitalWrite( PIN_NUMBER, HIGH);
				delayMicroseconds(40);

				// Now start reading the data line to get the value from the DHT sensor.
				pinMode( PIN_NUMBER, INPUT_PULLUP );
				delayMicroseconds(10);  // Delay a bit to let sensor pull data line low.

				// First expect a low signal for ~80 microseconds followed by a high signal
				// for ~80 microseconds again.
				if (expectPulse(LOW) == 0) 
				{
//					DEBUG_PRINTLN(F("Timeout waiting for start signal low pulse."));
					_lastresult = false;
					return _lastresult;
				}
				if (expectPulse(HIGH) == 0) 
				{
//					DEBUG_PRINTLN(F("Timeout waiting for start signal high pulse."));
					_lastresult = false;
					return _lastresult;
				}

				// Now read the 40 bits sent by the sensor.  Each bit is sent as a 50
				// microsecond low pulse followed by a variable length high pulse.  If the
				// high pulse is ~28 microseconds then it's a 0 and if it's ~70 microseconds
				// then it's a 1.  We measure the cycle count of the initial 50us low pulse
				// and use that to compare to the cycle count of the high pulse to determine
				// if the bit is a 0 (high state cycle count < low state cycle count), or a
				// 1 (high state cycle count > low state cycle count). Note that for speed all
				// the pulses are read into a array and then examined in a later step.
				for (int i=0; i<80; i+=2) 
				{
					cycles[i]   = expectPulse(LOW);
					cycles[i+1] = expectPulse(HIGH);
				}

			} // Timing critical code is now complete.

			// Inspect pulses and determine which ones are 0 (high state cycle count < low
			// state cycle count), or 1 (high state cycle count > low state cycle count).
			for (int i=0; i<40; ++i) 
			{
				uint32_t lowCycles  = cycles[2*i];
				uint32_t highCycles = cycles[2*i+1];
				if ((lowCycles == 0) || (highCycles == 0)) 
				{
//					DEBUG_PRINTLN(F("Timeout waiting for pulse."));
					_lastresult = false;
					return _lastresult;
				}

				data[i/8] <<= 1;
				// Now compare the low and high cycle times to see if the bit is a 0 or 1.
				if (highCycles > lowCycles) 
					// High cycles are greater than 50us low cycle count, must be a 1.
					data[i/8] |= 1;

				// Else high cycles are less than (or equal to, a weird case) the 50us low
				// cycle count so this must be a zero.  Nothing needs to be changed in the
				// stored data.
			}

/*
  DEBUG_PRINTLN(F("Received:"));
  DEBUG_PRINT(data[0], HEX); DEBUG_PRINT(F(", "));
  DEBUG_PRINT(data[1], HEX); DEBUG_PRINT(F(", "));
  DEBUG_PRINT(data[2], HEX); DEBUG_PRINT(F(", "));
  DEBUG_PRINT(data[3], HEX); DEBUG_PRINT(F(", "));
  DEBUG_PRINT(data[4], HEX); DEBUG_PRINT(F(" =? "));
  DEBUG_PRINTLN((data[0] + data[1] + data[2] + data[3]) & 0xFF, HEX);
*/
			// Check we read 40 bits and that the checksum matches.
			if (data[4] == ((data[0] + data[1] + data[2] + data[3]) & 0xFF)) 
			{
				_lastresult = true;
				return _lastresult;
			}

			else 
			{
	//			DEBUG_PRINTLN(F("Checksum failure!"));
				_lastresult = false;
				return _lastresult;
			}
		}

	public:
		inline void ClockInputPin_o_Receive( void *_Data )
		{
			ReadSensor();
		}

	public:
		DHT11Sensor() :
			ClockInputPin_o_IsConnected( false ),
			Enabled( true ),
			InFahrenheit( false ),
			 _lastresult( false )
		{
  #ifdef __AVR
			_bit = digitalPinToBitMask( PIN_NUMBER );
			_port = digitalPinToPort( PIN_NUMBER );
  #endif
			DHTSensor::Maxcycles = microsecondsToClockCycles(1000);  // 1 millisecond timeout for
                                                 // reading pulses from DHT sensor.
		}
	};
//---------------------------------------------------------------------------
	template<int PIN_NUMBER> class DHT22Sensor
	{
	public:
		OpenWire::SourcePin			TemperatureOutputPin;
		OpenWire::SourcePin			HumidityOutputPin;

	public:
		bool	ClockInputPin_o_IsConnected : 1;

	public:
		bool	Enabled : 1;
		bool	InFahrenheit : 1;

	protected:
		bool _lastresult : 1;
		uint8_t data[5];
		uint32_t FLastReadTime;

#ifdef __AVR
		// Use direct GPIO access on an 8-bit AVR so keep track of the port and bitmask
		// for the digital pin connected to the DHT.  Other platforms will use digitalRead.
		uint8_t _bit, _port;
#endif

//		DHT	*FSensor;

	public:
		inline void SystemInit()
		{
			FLastReadTime = millis() - 2000;
			pinMode( PIN_NUMBER, INPUT_PULLUP );
//			FSensor = new DHT( PIN_NUMBER, SENSOR_TYPE );
//			FSensor->begin();
		}

		inline void SystemStart() {} // Placeholder

		inline void SystemLoopBegin( unsigned long currentMicros )
		{
#ifdef _MITOV_DHT11_CLOCK_PIN_
			if( ! ClockInputPin_o_IsConnected )
#endif
				ReadSensor();

		}

	protected:
		void ReadSensor()
		{
			if( ! TryRead())
				return;

			if( TemperatureOutputPin.IsConnected() )
			{
				float   AValue = data[2] & 0x7F;
				AValue *= 256;
				AValue += data[3];
				AValue *= 0.1;
				if( data[2] & 0x80 ) 
					AValue *= -1;
				
				if( InFahrenheit )
					AValue = Func::ConvertCtoF( AValue );

				TemperatureOutputPin.Notify( &AValue );
			}

			if( HumidityOutputPin.IsConnected() )
			{
				float   AValue = data[0];
				AValue *= 256;
				AValue += data[1];
				AValue *= 0.1;
				HumidityOutputPin.Notify( &AValue );
			}
		}

		uint32_t expectPulse(bool level) 
		{
			uint32_t count = 0;
			// On AVR platforms use direct GPIO port access as it's much faster and better
			// for catching pulses that are 10's of microseconds in length:
#ifdef __AVR
			uint8_t portState = level ? _bit : 0;
			while ((*portInputRegister(_port) & _bit) == portState) 
			// Otherwise fall back to using digitalRead (this seems to be necessary on ESP8266
			// right now, perhaps bugs in direct port access functions?).
#else
			while (digitalRead(PIN_NUMBER) == level) 
#endif
			{
				if (count++ >= DHTSensor::Maxcycles ) 
					return 0; // Exceeded timeout, fail.
			}

			return count;
		}

		bool TryRead()
		{
			if( ! Enabled )
				return false;

			uint32_t currenttime = millis();
			if ( (currenttime - FLastReadTime) < 2000 ) 
				return _lastresult; // return last correct measurement

			FLastReadTime = currenttime;

			// Reset 40 bits of received data to zero.
			data[0] = data[1] = data[2] = data[3] = data[4] = 0;

			// Send start signal.  See DHT datasheet for full signal diagram:
			//   http://www.adafruit.com/datasheets/Digital%20humidity%20and%20temperature%20sensor%20AM2302.pdf

			// Go into high impedence state to let pull-up raise data line level and
			// start the reading process.
			digitalWrite( PIN_NUMBER, HIGH);
			delay(250);

			// First set data line low for 20 milliseconds.
			pinMode( PIN_NUMBER, OUTPUT);
			digitalWrite( PIN_NUMBER, LOW);
			delay(20);

			uint32_t cycles[80];
			{
				// Turn off interrupts temporarily because the next sections are timing critical
				// and we don't want any interruptions.
				InterruptLock lock;

				// End the start signal by setting data line high for 40 microseconds.
				digitalWrite( PIN_NUMBER, HIGH);
				delayMicroseconds(40);

				// Now start reading the data line to get the value from the DHT sensor.
				pinMode( PIN_NUMBER, INPUT_PULLUP );
				delayMicroseconds(10);  // Delay a bit to let sensor pull data line low.

				// First expect a low signal for ~80 microseconds followed by a high signal
				// for ~80 microseconds again.
				if (expectPulse(LOW) == 0) 
				{
//					DEBUG_PRINTLN(F("Timeout waiting for start signal low pulse."));
					_lastresult = false;
					return _lastresult;
				}
				if (expectPulse(HIGH) == 0) 
				{
//					DEBUG_PRINTLN(F("Timeout waiting for start signal high pulse."));
					_lastresult = false;
					return _lastresult;
				}

				// Now read the 40 bits sent by the sensor.  Each bit is sent as a 50
				// microsecond low pulse followed by a variable length high pulse.  If the
				// high pulse is ~28 microseconds then it's a 0 and if it's ~70 microseconds
				// then it's a 1.  We measure the cycle count of the initial 50us low pulse
				// and use that to compare to the cycle count of the high pulse to determine
				// if the bit is a 0 (high state cycle count < low state cycle count), or a
				// 1 (high state cycle count > low state cycle count). Note that for speed all
				// the pulses are read into a array and then examined in a later step.
				for (int i=0; i<80; i+=2) 
				{
					cycles[i]   = expectPulse(LOW);
					cycles[i+1] = expectPulse(HIGH);
				}

			} // Timing critical code is now complete.

			// Inspect pulses and determine which ones are 0 (high state cycle count < low
			// state cycle count), or 1 (high state cycle count > low state cycle count).
			for (int i=0; i<40; ++i) 
			{
				uint32_t lowCycles  = cycles[2*i];
				uint32_t highCycles = cycles[2*i+1];
				if ((lowCycles == 0) || (highCycles == 0)) 
				{
//					DEBUG_PRINTLN(F("Timeout waiting for pulse."));
					_lastresult = false;
					return _lastresult;
				}

				data[i/8] <<= 1;
				// Now compare the low and high cycle times to see if the bit is a 0 or 1.
				if (highCycles > lowCycles) 
					// High cycles are greater than 50us low cycle count, must be a 1.
					data[i/8] |= 1;

				// Else high cycles are less than (or equal to, a weird case) the 50us low
				// cycle count so this must be a zero.  Nothing needs to be changed in the
				// stored data.
			}

/*
  DEBUG_PRINTLN(F("Received:"));
  DEBUG_PRINT(data[0], HEX); DEBUG_PRINT(F(", "));
  DEBUG_PRINT(data[1], HEX); DEBUG_PRINT(F(", "));
  DEBUG_PRINT(data[2], HEX); DEBUG_PRINT(F(", "));
  DEBUG_PRINT(data[3], HEX); DEBUG_PRINT(F(", "));
  DEBUG_PRINT(data[4], HEX); DEBUG_PRINT(F(" =? "));
  DEBUG_PRINTLN((data[0] + data[1] + data[2] + data[3]) & 0xFF, HEX);
*/
			// Check we read 40 bits and that the checksum matches.
			if (data[4] == ((data[0] + data[1] + data[2] + data[3]) & 0xFF)) 
			{
				_lastresult = true;
				return _lastresult;
			}

			else 
			{
	//			DEBUG_PRINTLN(F("Checksum failure!"));
				_lastresult = false;
				return _lastresult;
			}
		}

	public:
		inline void ClockInputPin_o_Receive( void *_Data )
		{
			ReadSensor();
		}

	public:
		DHT22Sensor() :
			ClockInputPin_o_IsConnected( false ),
			Enabled( true ),
			InFahrenheit( false ),
			 _lastresult( false )
		{
  #ifdef __AVR
			_bit = digitalPinToBitMask( PIN_NUMBER );
			_port = digitalPinToPort( PIN_NUMBER );
  #endif
			DHTSensor::Maxcycles = microsecondsToClockCycles(1000);  // 1 millisecond timeout for
                                                 // reading pulses from DHT sensor.
		}
	};
//---------------------------------------------------------------------------
    template<int PIN_NUMBER, void (*INTERRUPT_HANDLER)()> class DHT11Sensor_Interrupt : public OpenWire::InterruptObject
	{
	public:
		OpenWire::SourcePin			TemperatureOutputPin;
		OpenWire::SourcePin			HumidityOutputPin;

	protected:
		// error codes
		static const uint8_t IDDHTLIB_ERROR_CHECKSUM	= 1;
		static const uint8_t IDDHTLIB_ERROR_TIMEOUT		= 2;
		static const uint8_t IDDHTLIB_ERROR_ACQUIRING	= 3;
		static const uint8_t IDDHTLIB_ERROR_DELTA		= 4;
		static const uint8_t IDDHTLIB_ERROR_NOTSTARTED	= 5;

	protected:
		enum TStates { RESPONSE, DATA, ACQUIRED, STOPPED, ACQUIRING, RAW_DATA_READY, START_SIGNAL };

	public:
		bool	ClockInputPin_o_IsConnected : 1;

	public:
		bool	Enabled : 1;
		bool	InFahrenheit : 1;

	protected:
		bool FClocked : 1;
		volatile TStates FState : 3;
		volatile uint8_t FStatus : 3;
	    volatile uint8_t FIndex : 3;
	    volatile uint8_t FBitIndex : 3;
		volatile uint8_t data[5];
		uint32_t FLastReadTime;
		volatile unsigned long us;
//		int FCount = 0;
//		int32_t cycles[80];
//		volatile uint8_t bits[5];
//		uint32_t FInterruptCount = 0;

#ifdef __AVR
		// Use direct GPIO access on an 8-bit AVR so keep track of the port and bitmask
		// for the digital pin connected to the DHT.  Other platforms will use digitalRead.
		uint8_t _bit, _port;
#endif

//		DHT	*FSensor;

	public:
		void InterruptHandler( bool AValue )
		{
//			FInterruptCount ++;
//			Serial.println( "TEST" );
			unsigned long newUs = micros();
			unsigned long delta = newUs - us;
			if ( delta > 255 ) 
			{
//				Serial.print( "TIMEOUT: " );
//				Serial.println( delta );

				FStatus = IDDHTLIB_ERROR_TIMEOUT;
				FState = STOPPED;
				detachInterrupt( digitalPinToInterrupt( PIN_NUMBER ));
				return;
			}

/*
			if( AValue )
				cycles[ FCount++ ] = delta;

			else
				cycles[ FCount++ ] = -delta;
*/
			us = newUs;

			switch( FState ) 
			{
				case RESPONSE:
//					if( AValue )
//						return;

//					Serial.print( "RESPONSE: " );
//					Serial.println( delta );
					if (delta < 25) 
					{
						us -= delta;
						break; //do nothing, it started the response signal
					} 
					
					if (125 < delta && delta < 190) 
					{
//						Serial.println( "DATA 2" );
						FState = DATA;
					}

					else 
					{
//						Serial.print( "REPONSE: " );
//						Serial.println( delta );
//						Serial.println( AValue );

						detachInterrupt(digitalPinToInterrupt( PIN_NUMBER ));
						FStatus = IDDHTLIB_ERROR_TIMEOUT;
						FState = STOPPED;
					}
					break;

				case DATA:
//					Serial.println( "DATA 1" );
//					Serial.println( delta );
					if (60 < delta && delta < 145) 
					{ //valid in timing
						data[FIndex] <<= 1; //shift the data
						if (delta > 100) //is a one
							data[FIndex] |= 1;

						if (FBitIndex == 0) 
						{  // when we have fulfilled the byte, go to the next
							FBitIndex = 7;    // restart at MSB
							if (++FIndex == 5) 
							{ // go to next byte; when we have got 5 bytes, stop.
								detachInterrupt(digitalPinToInterrupt( PIN_NUMBER ));
/*
								Serial.println( "DATA" );
								for( int i = 0; i < 5; ++i )
								{
									Serial.print( data[i] ); Serial.print( " " );
								}
								Serial.println( "" );

								for( int i = 0; i < FCount; ++i )
								{
									Serial.print( cycles[i] ); Serial.print( " " );
								}
								Serial.println( "" );
*/
								FState = RAW_DATA_READY;
								break;
							}
						} 

						else 
							FBitIndex--;

					}

					else if (delta < 10) 
					{
//						Serial.println( "IDDHTLIB_ERROR_DELTA" );
//						Serial.println( delta );
						detachInterrupt(digitalPinToInterrupt( PIN_NUMBER ));
						FStatus = IDDHTLIB_ERROR_DELTA;
						FState = STOPPED;
					}

					else 
					{
//						Serial.println( "IDDHTLIB_ERROR_TIMEOUT" );
//						Serial.println( delta );
						detachInterrupt(digitalPinToInterrupt( PIN_NUMBER ));
						FStatus = IDDHTLIB_ERROR_TIMEOUT;
						FState = STOPPED;
					}
					break;

				default:
					break;
			}

		}

	public:
		inline void SystemInit() {} // Placeholder

		inline void SystemStart()
		{
			FLastReadTime = millis() - 2000;
			detachInterrupt(digitalPinToInterrupt( PIN_NUMBER ));
			pinMode( PIN_NUMBER, INPUT_PULLUP );
//			FSensor = new DHT( PIN_NUMBER, SENSOR_TYPE );
//			FSensor->begin();
		}

		inline void SystemLoopBegin( unsigned long currentMicros )
		{
//			Serial.println( FInterruptCount );
			ReadResult();

			if( FClocked )
			{
				ReadSensor();
				return;
			}

#ifdef _MITOV_DHT11_CLOCK_PIN_
			if( ! ClockInputPin_o_IsConnected )
#endif
				ReadSensor();

		}

	protected:
		void ReadSensor()
		{
			uint32_t currenttime = millis();
			if ( (currenttime - FLastReadTime) < 2000 ) 
			{
				if( FState == STOPPED && FStatus == 0 )
					FState = RAW_DATA_READY;

				return; // return last correct measurement
			}

			FLastReadTime = currenttime;

			FClocked = ( ! TryRead());
		}

		void ReadResult()
		{
			if( FState != RAW_DATA_READY )
				return;

			if( ! Enabled )
				return;

			if( TemperatureOutputPin.IsConnected() )
			{
				float   AValue = data[2];
				if( InFahrenheit )
					AValue = Func::ConvertCtoF( AValue );

				TemperatureOutputPin.Notify( &AValue );
			}

			if( HumidityOutputPin.IsConnected() )
			{
				float   AValue = data[0];
				HumidityOutputPin.Notify( &AValue );
			}

			FState = STOPPED;
		}

		bool TryRead()
		{
			if ( FState == STOPPED || FState == ACQUIRED) 
			{
				if( ! Enabled )
					return true;

				detachInterrupt(digitalPinToInterrupt( PIN_NUMBER ));
				//set the state machine for interruptions analisis of the signal
//				FState = useDelay ? RESPONSE : START_SIGNAL;
				FState = RESPONSE;

				// EMPTY BUFFER and vars
				memset( (void *)data, 0, sizeof( data ));
//				for (byte i = 0; i < 5; i++) 
//					bits[i] = 0;

				FBitIndex = 7;
				FIndex = 0;
				FStatus = 0;
//				FCount = 0;
//				hum = 0; // This will be the Humidity result!
//				temp = 0; // This will be the Temperature result!

				// REQUEST SAMPLE
				pinMode( PIN_NUMBER, OUTPUT);
/*
				// Go into high impedence state to let pull-up raise data line level and
				// start the reading process.
				digitalWrite( PIN_NUMBER, HIGH);
				delay(250);
*/
				digitalWrite( PIN_NUMBER, LOW);

//				if (useDelay) 
				{
					delay(18);
					digitalWrite( PIN_NUMBER, HIGH);
					delayMicroseconds(25);
					pinMode( PIN_NUMBER, INPUT);
				}

				us = micros();
				// Analize the data in an interrupt
//				if (useDelay)
//					attachInterrupt(intNumber, pCallbackArray[intNumber], FALLING);
					attachInterrupt( digitalPinToInterrupt( PIN_NUMBER ), INTERRUPT_HANDLER, FALLING );
//					INTERRUPT_HANDLER.Reattach();

//				Serial.println( "START" );
				return true;
			} 

			return false;
		}

	public:
		inline void ClockInputPin_o_Receive( void *_Data )
		{
			ReadSensor();
		}

	public:
		DHT11Sensor_Interrupt() :
			ClockInputPin_o_IsConnected( false ),
			Enabled( true ),
			InFahrenheit( false ),
			FClocked( false ),
			FState( STOPPED ),
			FStatus( IDDHTLIB_ERROR_NOTSTARTED )
		{
  #ifdef __AVR
			_bit = digitalPinToBitMask( PIN_NUMBER );
			_port = digitalPinToPort( PIN_NUMBER );
  #endif
//			DHTSensor::Maxcycles = microsecondsToClockCycles(1000);  // 1 millisecond timeout for
                                                 // reading pulses from DHT sensor.
		}
	};
//---------------------------------------------------------------------------
    template<int PIN_NUMBER, void (*INTERRUPT_HANDLER)()> class DHT22Sensor_Interrupt : public OpenWire::InterruptObject
	{
	public:
		OpenWire::SourcePin	TemperatureOutputPin;
		OpenWire::SourcePin	HumidityOutputPin;

	protected:
		// error codes
		static const uint8_t IDDHTLIB_ERROR_CHECKSUM	= 1;
		static const uint8_t IDDHTLIB_ERROR_TIMEOUT		= 2;
		static const uint8_t IDDHTLIB_ERROR_ACQUIRING	= 3;
		static const uint8_t IDDHTLIB_ERROR_DELTA		= 4;
		static const uint8_t IDDHTLIB_ERROR_NOTSTARTED	= 5;

	protected:
		enum TStates { RESPONSE, DATA, ACQUIRED, STOPPED, ACQUIRING, RAW_DATA_READY, START_SIGNAL };

	public:
		bool	ClockInputPin_o_IsConnected : 1;

	public:
		bool	Enabled : 1;
		bool	InFahrenheit : 1;

	protected:
		bool FClocked : 1;
		volatile TStates FState : 3;
		volatile uint8_t FStatus : 3;
	    volatile uint8_t FIndex : 3;
	    volatile uint8_t FBitIndex : 3;
		volatile uint8_t data[5];
		uint32_t FLastReadTime;
		volatile unsigned long us;
//		int FCount = 0;
//		int32_t cycles[80];
//		volatile uint8_t bits[5];
//		uint32_t FInterruptCount = 0;

#ifdef __AVR
		// Use direct GPIO access on an 8-bit AVR so keep track of the port and bitmask
		// for the digital pin connected to the DHT.  Other platforms will use digitalRead.
		uint8_t _bit, _port;
#endif

//		DHT	*FSensor;

	public:
		void InterruptHandler( bool AValue )
		{
//			FInterruptCount ++;
//			Serial.println( "TEST" );
			unsigned long newUs = micros();
			unsigned long delta = newUs - us;
			if ( delta > 255 ) 
			{
//				Serial.print( "TIMEOUT: " );
//				Serial.println( delta );

				FStatus = IDDHTLIB_ERROR_TIMEOUT;
				FState = STOPPED;
				detachInterrupt( digitalPinToInterrupt( PIN_NUMBER ));
				return;
			}

/*
			if( AValue )
				cycles[ FCount++ ] = delta;

			else
				cycles[ FCount++ ] = -delta;
*/
			us = newUs;

			switch( FState ) 
			{
				case RESPONSE:
//					if( AValue )
//						return;

//					Serial.print( "RESPONSE: " );
//					Serial.println( delta );
					if (delta < 25) 
					{
						us -= delta;
						break; //do nothing, it started the response signal
					} 
					
					if (125 < delta && delta < 190) 
					{
//						Serial.println( "DATA 2" );
						FState = DATA;
					}

					else 
					{
//						Serial.print( "REPONSE: " );
//						Serial.println( delta );
//						Serial.println( AValue );

						detachInterrupt(digitalPinToInterrupt( PIN_NUMBER ));
						FStatus = IDDHTLIB_ERROR_TIMEOUT;
						FState = STOPPED;
					}
					break;

				case DATA:
//					Serial.println( "DATA 1" );
//					Serial.println( delta );
					if (60 < delta && delta < 145) 
					{ //valid in timing
						data[FIndex] <<= 1; //shift the data
						if (delta > 100) //is a one
							data[FIndex] |= 1;

						if (FBitIndex == 0) 
						{  // when we have fulfilled the byte, go to the next
							FBitIndex = 7;    // restart at MSB
							if (++FIndex == 5) 
							{ // go to next byte; when we have got 5 bytes, stop.
								detachInterrupt(digitalPinToInterrupt( PIN_NUMBER ));
/*
								Serial.println( "DATA" );
								for( int i = 0; i < 5; ++i )
								{
									Serial.print( data[i] ); Serial.print( " " );
								}
								Serial.println( "" );

								for( int i = 0; i < FCount; ++i )
								{
									Serial.print( cycles[i] ); Serial.print( " " );
								}
								Serial.println( "" );
*/
								FState = RAW_DATA_READY;
								break;
							}
						} 

						else 
							FBitIndex--;

					}

					else if (delta < 10) 
					{
//						Serial.println( "IDDHTLIB_ERROR_DELTA" );
//						Serial.println( delta );
						detachInterrupt(digitalPinToInterrupt( PIN_NUMBER ));
						FStatus = IDDHTLIB_ERROR_DELTA;
						FState = STOPPED;
					}

					else 
					{
//						Serial.println( "IDDHTLIB_ERROR_TIMEOUT" );
//						Serial.println( delta );
						detachInterrupt(digitalPinToInterrupt( PIN_NUMBER ));
						FStatus = IDDHTLIB_ERROR_TIMEOUT;
						FState = STOPPED;
					}
					break;

				default:
					break;
			}

		}

	public:
		inline void SystemInit() {} // Placeholder

		inline void SystemStart()
		{
			FLastReadTime = millis() - 2000;
			detachInterrupt(digitalPinToInterrupt( PIN_NUMBER ));
			pinMode( PIN_NUMBER, INPUT_PULLUP );
//			FSensor = new DHT( PIN_NUMBER, SENSOR_TYPE );
//			FSensor->begin();
		}

		inline void SystemLoopBegin( unsigned long currentMicros )
		{
//			Serial.println( FInterruptCount );
			ReadResult();

			if( FClocked )
			{
				ReadSensor();
				return;
			}

#ifdef _MITOV_DHT11_CLOCK_PIN_
			if( ! ClockInputPin_o_IsConnected )
#endif
				ReadSensor();

		}

	protected:
		void ReadSensor()
		{
			uint32_t currenttime = millis();
			if ( (currenttime - FLastReadTime) < 2000 ) 
			{
				if( FState == STOPPED && FStatus == 0 )
					FState = RAW_DATA_READY;

				return; // return last correct measurement
			}

			FLastReadTime = currenttime;

			FClocked = ( ! TryRead());
		}

		void ReadResult()
		{
			if( FState != RAW_DATA_READY )
				return;

			if( ! Enabled )
				return;

			if( TemperatureOutputPin.IsConnected() )
			{
				float   AValue = data[2] & 0x7F;
				AValue *= 256;
				AValue += data[3];
				AValue *= 0.1;
				if( data[2] & 0x80 ) 
					AValue *= -1;
				
				if( InFahrenheit )
					AValue = Func::ConvertCtoF( AValue );

				TemperatureOutputPin.Notify( &AValue );
			}

			if( HumidityOutputPin.IsConnected() )
			{
				float   AValue = data[0];
				AValue *= 256;
				AValue += data[1];
				AValue *= 0.1;
				HumidityOutputPin.Notify( &AValue );
			}

			FState = STOPPED;
		}

		bool TryRead()
		{
			if ( FState == STOPPED || FState == ACQUIRED) 
			{
				if( ! Enabled )
					return true;

				detachInterrupt(digitalPinToInterrupt( PIN_NUMBER ));
				//set the state machine for interruptions analisis of the signal
//				FState = useDelay ? RESPONSE : START_SIGNAL;
				FState = RESPONSE;

				// EMPTY BUFFER and vars
				memset( (void *)data, 0, sizeof( data ));
//				for (byte i = 0; i < 5; i++) 
//					bits[i] = 0;

				FBitIndex = 7;
				FIndex = 0;
				FStatus = 0;
//				FCount = 0;
//				hum = 0; // This will be the Humidity result!
//				temp = 0; // This will be the Temperature result!

				// REQUEST SAMPLE
				pinMode( PIN_NUMBER, OUTPUT);
/*
				// Go into high impedence state to let pull-up raise data line level and
				// start the reading process.
				digitalWrite( PIN_NUMBER, HIGH);
				delay(250);
*/
				digitalWrite( PIN_NUMBER, LOW);

//				if (useDelay) 
				{
					delay(18);
					digitalWrite( PIN_NUMBER, HIGH);
					delayMicroseconds(25);
					pinMode( PIN_NUMBER, INPUT);
				}

				us = micros();
				// Analize the data in an interrupt
//				if (useDelay)
//					attachInterrupt(intNumber, pCallbackArray[intNumber], FALLING);
					attachInterrupt( digitalPinToInterrupt( PIN_NUMBER ), INTERRUPT_HANDLER, FALLING );
//					INTERRUPT_HANDLER.Reattach();

//				Serial.println( "START" );
				return true;
			} 

			return false;
		}

	public:
		inline void ClockInputPin_o_Receive( void *_Data )
		{
			ReadSensor();
		}

	public:
		DHT22Sensor_Interrupt() :
			ClockInputPin_o_IsConnected( false ),
			Enabled( true ),
			InFahrenheit( false ),
			FClocked( false ),
			FState( STOPPED ),
			FStatus( IDDHTLIB_ERROR_NOTSTARTED )
		{
  #ifdef __AVR
			_bit = digitalPinToBitMask( PIN_NUMBER );
			_port = digitalPinToPort( PIN_NUMBER );
  #endif
//			DHTSensor::Maxcycles = microsecondsToClockCycles(1000);  // 1 millisecond timeout for
                                                 // reading pulses from DHT sensor.
		}
	};
//---------------------------------------------------------------------------
}

#ifdef __TEST_FOR_DEBUG_PRINTS__
#undef Serial
#endif

#endif
