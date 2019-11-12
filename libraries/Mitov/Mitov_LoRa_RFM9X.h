////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2018 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_LORA_RFM9X_h
#define _MITOV_LORA_RFM9X_h

#include <Mitov.h>
#include <SPI.h>

#ifdef __TEST_FOR_DEBUG_PRINTS__
#define Serial UNGUARDED DEBUG PRINT!!!
#endif

//#define ___MITOV_LORA_RFM9X_DEBUG__

namespace Mitov
{
//---------------------------------------------------------------------------
	enum TRFM9XGainControl { gcAuto, gcG1, gcG2, gcG3, gcG4, gcG5, gcG6 };
	enum TRFM9XCodingRate { cr4_5, cr4_6, cr4_7, cr4_8 };
//---------------------------------------------------------------------------
/*
	class MitovBasicLoRaRFM9X
	{
	public:
		#include <Mitov_LoRa_RFM9X_Functions.h>

	public:
		MitovBasicLoRaRFM9X() :
			SymbolTimeout( 0x64 ),
			LoRaTMMode( true ),
			MobileNode( false ),
			LowNoiseAmplifierHighFrequencyBoost( false ),
			Gain( gcAuto ),
			CodingRate( cr4_5 ),
			SpreadingFactorOrder( 7 ),
			FReady( false ),
			FModeChange( false ),
			FMode( RHModeInitialising )
		{
		}
	};
*/
//---------------------------------------------------------------------------
	template<typename T_SPI, T_SPI &C_SPI, int PIN_INT> class MitovLoRaRFM9X_Interrupt : public OpenWire::InterruptObject
	{
		#include <Mitov_LoRa_RFM9X_Functions.h>

	public:
		OpenWire::SourcePin	ChipSelectOutputPin;
		OpenWire::SourcePin	ResetOutputPin;

	protected:
		void Reset()
		{
			ChipSelectOutputPin.SendValue( true );

			if( ResetOutputPin.IsConnected() )
			{
//				Serial.println( "RESET" );
				ResetOutputPin.SendValue( false );
				delay( 100 );
				ResetOutputPin.SendValue( true );
				delay( 100 );
			}
		}

		uint8_t spiRead(uint8_t reg)
		{
			uint8_t val;
			C_SPI.beginTransaction( SPISettings() );
			ChipSelectOutputPin.SendValue( false );
			C_SPI.transfer(reg & ~0x80); // Send the address with the write mask off
			val = C_SPI.transfer(0); // The written value is ignored, reg value is read
			ChipSelectOutputPin.SendValue( true );
			C_SPI.endTransaction();
//Serial.print( "spiRead: " ); Serial.print( reg, HEX ); Serial.print( " = " ); Serial.println( val, HEX );
			return val;
		}

		uint8_t spiWrite(uint8_t reg, uint8_t val)
		{
//Serial.print( "spiWrite: " ); Serial.print( reg, HEX ); Serial.print( " = " ); Serial.println( val, HEX );
			C_SPI.beginTransaction( SPISettings() );
			ChipSelectOutputPin.SendValue( false );
			uint8_t status = C_SPI.transfer(reg | 0x80); // Send the address with the write mask on
			C_SPI.transfer(val); // New value follows
			ChipSelectOutputPin.SendValue( true );
			C_SPI.endTransaction();
			return status;
		}

	protected:
		void readFifo( uint8_t ALength )
		{
//			Serial.print( "LN: " ); Serial.println( ALength );
			// Reset the fifo read ptr to the beginning of the packet
			spiWrite(RH_RF95_REG_0D_FIFO_ADDR_PTR, spiRead(RH_RF95_REG_10_FIFO_RX_CURRENT_ADDR));

			C_SPI.beginTransaction( SPISettings() );
			ChipSelectOutputPin.SendValue( false );

			C_SPI.transfer(RH_RF95_REG_00_FIFO); // Send the start address with the write mask off

			uint8_t *ABuffer = new uint8_t[ 1 + ALength ];
			FReceivedPackets.push_back( ABuffer );
			*ABuffer++ = ALength; // Skip the ARxHeaderTo

			for ( int i = 0; i < ALength; ++i )
				*ABuffer++ = C_SPI.transfer(0);

			ChipSelectOutputPin.SendValue( true );
			C_SPI.endTransaction();
		}

		void TransferPacket( const uint8_t* data, uint8_t len )
		{
			C_SPI.beginTransaction( SPISettings() );
			ChipSelectOutputPin.SendValue( false );
			C_SPI.transfer( RH_RF95_REG_00_FIFO | 0x80 ); // Send the start address with the write mask on
//			C_SPI.transfer( len + 1 ); // Include length of headers
			// First the 4 headers
//			C_SPI.transfer( ATxHeaderTo );
//			C_SPI.transfer( Address );
//			C_SPI.transfer(_txHeaderId);
//			C_SPI.transfer(_txHeaderFlags);
			// Now the payload
			while( len-- )
				C_SPI.transfer( *data++ );

			ChipSelectOutputPin.SendValue( true );
			C_SPI.endTransaction();
		}

	public:
//#ifdef _MITOV_CONNECTED_LORA_RFM9X_INTERRUPT_PIN_
		// C++ level interrupt handler for this instance
		// RH_RF69 is unusual in that it has several interrupt lines, and not a single, combined one.
		// On Moteino, only one of the several interrupt lines (DI0) from the RH_RF69 is connnected to the processor.
		// We use this to get PACKETSDENT and PAYLOADRADY interrupts.
		void InterruptHandler( bool )
		{
			// Read the interrupt register
			uint8_t irq_flags = spiRead(RH_RF95_REG_12_IRQ_FLAGS);
			if ( FMode == RHModeRx && irq_flags & (RH_RF95_RX_TIMEOUT | RH_RF95_PAYLOAD_CRC_ERROR))
				; //_rxBad++;

			else if ( FMode == RHModeRx && irq_flags & RH_RF95_RX_DONE)
			{
				// Have received a packet
				uint8_t len = spiRead(RH_RF95_REG_13_RX_NB_BYTES);

				// Reset the fifo read ptr to the beginning of the packet
//				spiWrite(RH_RF95_REG_0D_FIFO_ADDR_PTR, spiRead(RH_RF95_REG_10_FIFO_RX_CURRENT_ADDR));
//				spiBurstRead(RH_RF95_REG_00_FIFO, _buf, len);
//				_bufLen = len;
//				spiWrite(RH_RF95_REG_12_IRQ_FLAGS, 0xff); // Clear all IRQ flags

				// Remember the RSSI of this packet
				// this is according to the doc, but is it really correct?
				// weakest receiveable signals are reported RSSI at about -66
				FLastRssi = spiRead(RH_RF95_REG_1A_PKT_RSSI_VALUE) - 137;

				readFifo( len );

				// We have received a message.
//				validateRxBuf(); 
//				if (_rxBufValid)
					setModeIdle(); // Got one 
			}
			else if ( FMode == RHModeTx && irq_flags & RH_RF95_TX_DONE)
			{
//				_txGood++;
				setModeIdle();
			}
    
			spiWrite(RH_RF95_REG_12_IRQ_FLAGS, 0xff); // Clear all IRQ flags
		}
//#endif // _MITOV_CONNECTED_LORA_RFM9X_INTERRUPT_PIN_

	protected:
		void ProcessInterrupt()
		{
		}

	public:
		inline void SystemInit()
		{
			// Do in startup for M0 compatibility! (Fails in constructor!)
			int AInterruptNumber = digitalPinToInterrupt( PIN_INT );
			C_SPI.usingInterrupt( AInterruptNumber );
			IntSystemInit();
		}

	public:
		MitovLoRaRFM9X_Interrupt() :
			SymbolTimeout( 0x64 ),
			LoRaTMMode( true ),
			MobileNode( false ),
			LowNoiseAmplifierHighFrequencyBoost( false ),
			Gain( gcAuto ),
			CodingRate( cr4_5 ),
			SpreadingFactorOrder( 7 ),
			FReady( false ),
			FModeChange( false ),
			FMode( RHModeInitialising )
		{
		}

	};
//---------------------------------------------------------------------------
	template<typename T_SPI, T_SPI &C_SPI> class MitovLoRaRFM9X
	{
		#include <Mitov_LoRa_RFM9X_Functions.h>

	public:
		OpenWire::SourcePin	ChipSelectOutputPin;
		OpenWire::SourcePin	ResetOutputPin;

	protected:
		void Reset()
		{
			ChipSelectOutputPin.SendValue( true );

			if( ResetOutputPin.IsConnected() )
			{
//				Serial.println( "RESET" );
				ResetOutputPin.SendValue( false );
				delay( 100 );
				ResetOutputPin.SendValue( true );
				delay( 100 );
			}
		}

		uint8_t spiRead(uint8_t reg)
		{
			uint8_t val;
			C_SPI.beginTransaction( SPISettings() );
			ChipSelectOutputPin.SendValue( false );
			C_SPI.transfer(reg & ~0x80); // Send the address with the write mask off
			val = C_SPI.transfer(0); // The written value is ignored, reg value is read
			ChipSelectOutputPin.SendValue( true );
			C_SPI.endTransaction();
//Serial.print( "spiRead: " ); Serial.print( reg, HEX ); Serial.print( " = " ); Serial.println( val, HEX );
			return val;
		}

		uint8_t spiWrite(uint8_t reg, uint8_t val)
		{
//Serial.print( "spiWrite: " ); Serial.print( reg, HEX ); Serial.print( " = " ); Serial.println( val, HEX );
			C_SPI.beginTransaction( SPISettings() );
			ChipSelectOutputPin.SendValue( false );
			uint8_t status = C_SPI.transfer(reg | 0x80); // Send the address with the write mask on
			C_SPI.transfer(val); // New value follows
			ChipSelectOutputPin.SendValue( true );
			C_SPI.endTransaction();
			return status;
		}

	protected:
		void readFifo( uint8_t ALength )
		{
//			Serial.print( "LN: " ); Serial.println( ALength );
			// Reset the fifo read ptr to the beginning of the packet
			spiWrite(RH_RF95_REG_0D_FIFO_ADDR_PTR, spiRead(RH_RF95_REG_10_FIFO_RX_CURRENT_ADDR));

			C_SPI.beginTransaction( SPISettings() );
			ChipSelectOutputPin.SendValue( false );

			C_SPI.transfer(RH_RF95_REG_00_FIFO); // Send the start address with the write mask off

			uint8_t *ABuffer = new uint8_t[ 1 + ALength ];
			FReceivedPackets.push_back( ABuffer );
			*ABuffer++ = ALength; // Skip the ARxHeaderTo

			for ( int i = 0; i < ALength; ++i )
				*ABuffer++ = C_SPI.transfer(0);

			ChipSelectOutputPin.SendValue( true );
			C_SPI.endTransaction();
		}

		void TransferPacket( const uint8_t* data, uint8_t len )
		{
			C_SPI.beginTransaction( SPISettings() );
			ChipSelectOutputPin.SendValue( false );
			C_SPI.transfer( RH_RF95_REG_00_FIFO | 0x80 ); // Send the start address with the write mask on
//			C_SPI.transfer( len + 1 ); // Include length of headers
			// First the 4 headers
//			C_SPI.transfer( ATxHeaderTo );
//			C_SPI.transfer( Address );
//			C_SPI.transfer(_txHeaderId);
//			C_SPI.transfer(_txHeaderFlags);
			// Now the payload
			while( len-- )
				C_SPI.transfer( *data++ );

			ChipSelectOutputPin.SendValue( true );
			C_SPI.endTransaction();
		}

   	protected:
		void ProcessInterrupt()
		{
			// Read the interrupt register
			uint8_t irq_flags = spiRead(RH_RF95_REG_12_IRQ_FLAGS);

			if ( FMode == RHModeRx && irq_flags & (RH_RF95_RX_TIMEOUT | RH_RF95_PAYLOAD_CRC_ERROR))
			{
				spiWrite(RH_RF95_REG_12_IRQ_FLAGS, 0xff); // Clear all IRQ flags
				; //_rxBad++;
			}

			else if ( FMode == RHModeRx && irq_flags & RH_RF95_RX_DONE)
			{
				spiWrite(RH_RF95_REG_12_IRQ_FLAGS, 0xff); // Clear all IRQ flags
				// Have received a packet
				uint8_t len = spiRead(RH_RF95_REG_13_RX_NB_BYTES);

				// Reset the fifo read ptr to the beginning of the packet
//				spiWrite(RH_RF95_REG_0D_FIFO_ADDR_PTR, spiRead(RH_RF95_REG_10_FIFO_RX_CURRENT_ADDR));
//				spiBurstRead(RH_RF95_REG_00_FIFO, _buf, len);
//				_bufLen = len;
//				spiWrite(RH_RF95_REG_12_IRQ_FLAGS, 0xff); // Clear all IRQ flags

				// Remember the RSSI of this packet
				// this is according to the doc, but is it really correct?
				// weakest receiveable signals are reported RSSI at about -66
				FLastRssi = spiRead(RH_RF95_REG_1A_PKT_RSSI_VALUE) - 137;

				readFifo( len );

				// We have received a message.
//				validateRxBuf(); 
//				if (_rxBufValid)
					setModeIdle(); // Got one 
			}

			else if ( FMode == RHModeTx && irq_flags & RH_RF95_TX_DONE)
			{
				spiWrite(RH_RF95_REG_12_IRQ_FLAGS, 0xff); // Clear all IRQ flags
//				_txGood++;
				setModeIdle();
			}
		}

	public:
		inline void SystemInit()
		{
			IntSystemInit();
		}

	public:
		MitovLoRaRFM9X() :
			SymbolTimeout( 0x64 ),
			LoRaTMMode( true ),
			MobileNode( false ),
			LowNoiseAmplifierHighFrequencyBoost( false ),
			Gain( gcAuto ),
			CodingRate( cr4_5 ),
			SpreadingFactorOrder( 7 ),
			FReady( false ),
			FModeChange( false ),
			FMode( RHModeInitialising )
		{
		}

	};
//---------------------------------------------------------------------------
	template<int PIN_CS, int PIN_RST, int PIN_INT> class MitovLoRaRFM9XShield_Interrupt
	{
		#include <Mitov_LoRa_RFM9X_Functions.h>

	protected:
		static MitovLoRaRFM9XShield_Interrupt<PIN_CS,PIN_RST,PIN_INT>	*FInstnce;

	protected:
		static void StaticInterruptHandler()
		{
			FInstnce->InterruptHandler( true );
		}

	protected:
		void Reset()
		{
			digitalWrite( PIN_CS, HIGH );

//				Serial.println( "RESET" );
			digitalWrite( PIN_RST, LOW );
			delay( 100 );
			digitalWrite( PIN_RST, HIGH );
			delay( 100 );
		}

		void readFifo( uint8_t ALength )
		{
//			Serial.print( "LN: " ); Serial.println( ALength );

			// Reset the fifo read ptr to the beginning of the packet
			spiWrite(RH_RF95_REG_0D_FIFO_ADDR_PTR, spiRead(RH_RF95_REG_10_FIFO_RX_CURRENT_ADDR));

			SPI.beginTransaction( SPISettings() );
			digitalWrite( PIN_CS, LOW );

			SPI.transfer(RH_RF95_REG_00_FIFO); // Send the start address with the write mask off

			uint8_t *ABuffer = new uint8_t[ 1 + ALength ];
			FReceivedPackets.push_back( ABuffer );
			*ABuffer++ = ALength; // Skip the ARxHeaderTo

			for ( int i = 0; i < ALength; ++i )
				*ABuffer++ = SPI.transfer(0);

			digitalWrite( PIN_CS, HIGH );
			SPI.endTransaction();
		}

		void TransferPacket( const uint8_t* data, uint8_t len )
		{
			SPI.beginTransaction( SPISettings() );
			digitalWrite( PIN_CS, LOW );
			SPI.transfer( RH_RF95_REG_00_FIFO | 0x80 ); // Send the start address with the write mask on
//			SPI.transfer( len + 1 ); // Include length of headers
			// First the 4 headers
//			SPI.transfer( ATxHeaderTo );
//			SPI.transfer( Address );
//			SPI.transfer(_txHeaderId);
//			SPI.transfer(_txHeaderFlags);
			// Now the payload
			while( len-- )
				SPI.transfer( *data++ );

			digitalWrite( PIN_CS, HIGH );
			SPI.endTransaction();
		}

		uint8_t spiRead(uint8_t reg)
		{
			SPI.beginTransaction( SPISettings() );
			digitalWrite( PIN_CS, LOW );
			SPI.transfer(reg & ~0x80); // Send the address with the write mask off
			uint8_t val = SPI.transfer(0); // The written value is ignored, reg value is read
			digitalWrite( PIN_CS, HIGH );
			SPI.endTransaction();
//Serial.print( "spiRead: " ); Serial.print( reg, HEX ); Serial.print( " = " ); Serial.println( val, HEX );
			return val;
		}

		uint8_t spiWrite(uint8_t reg, uint8_t val)
		{
//Serial.print( "spiWrite: " ); Serial.print( reg, HEX ); Serial.print( " = " ); Serial.println( val, HEX );
			SPI.beginTransaction( SPISettings() );
			digitalWrite( PIN_CS, LOW );
			uint8_t status = SPI.transfer(reg | 0x80); // Send the address with the write mask on
			SPI.transfer(val); // New value follows
			digitalWrite( PIN_CS, HIGH );
			SPI.endTransaction();
			return status;
		}

	protected:
		void ProcessInterrupt()
		{
		}

	public:
//#ifdef _MITOV_CONNECTED_LORA_RFM9X_INTERRUPT_PIN_
		// C++ level interrupt handler for this instance
		// RH_RF69 is unusual in that it has several interrupt lines, and not a single, combined one.
		// On Moteino, only one of the several interrupt lines (DI0) from the RH_RF69 is connnected to the processor.
		// We use this to get PACKETSDENT and PAYLOADRADY interrupts.
		void InterruptHandler( bool )
		{
			// Read the interrupt register
			uint8_t irq_flags = spiRead(RH_RF95_REG_12_IRQ_FLAGS);
			if ( FMode == RHModeRx && irq_flags & (RH_RF95_RX_TIMEOUT | RH_RF95_PAYLOAD_CRC_ERROR))
				; //_rxBad++;

			else if ( FMode == RHModeRx && irq_flags & RH_RF95_RX_DONE)
			{
				// Have received a packet
				uint8_t len = spiRead(RH_RF95_REG_13_RX_NB_BYTES);

				// Reset the fifo read ptr to the beginning of the packet
//				spiWrite(RH_RF95_REG_0D_FIFO_ADDR_PTR, spiRead(RH_RF95_REG_10_FIFO_RX_CURRENT_ADDR));
//				spiBurstRead(RH_RF95_REG_00_FIFO, _buf, len);
//				_bufLen = len;
//				spiWrite(RH_RF95_REG_12_IRQ_FLAGS, 0xff); // Clear all IRQ flags

				// Remember the RSSI of this packet
				// this is according to the doc, but is it really correct?
				// weakest receiveable signals are reported RSSI at about -66
				FLastRssi = spiRead(RH_RF95_REG_1A_PKT_RSSI_VALUE) - 137;

				readFifo( len );

				// We have received a message.
//				validateRxBuf(); 
//				if (_rxBufValid)
					setModeIdle(); // Got one 
			}
			else if ( FMode == RHModeTx && irq_flags & RH_RF95_TX_DONE)
			{
//				_txGood++;
				setModeIdle();
			}
    
			spiWrite(RH_RF95_REG_12_IRQ_FLAGS, 0xff); // Clear all IRQ flags
		}
//#endif // _MITOV_CONNECTED_LORA_RFM9X_INTERRUPT_PIN_

	public:
		inline void SystemInit()
		{
			SPI.begin();
			int AInterruptNumber = digitalPinToInterrupt( PIN_INT );
			attachInterrupt( AInterruptNumber, StaticInterruptHandler, RISING );
#ifndef VISUINO_ESP32
			SPI.usingInterrupt( AInterruptNumber );
#endif
			IntSystemInit();
		}

	public:
		MitovLoRaRFM9XShield_Interrupt() :
			SymbolTimeout( 0x64 ),
			LoRaTMMode( true ),
			MobileNode( false ),
			LowNoiseAmplifierHighFrequencyBoost( false ),
			Gain( gcAuto ),
			CodingRate( cr4_5 ),
			SpreadingFactorOrder( 7 ),
			FReady( false ),
			FModeChange( false ),
			FMode( RHModeInitialising )
		{
			FInstnce = this;
			pinMode( PIN_RST, OUTPUT );
			pinMode( PIN_CS, OUTPUT );
		}
	};
//---------------------------------------------------------------------------
	template<int PIN_CS, int PIN_RST, int PIN_INT> MitovLoRaRFM9XShield_Interrupt<PIN_CS,PIN_RST,PIN_INT>	*MitovLoRaRFM9XShield_Interrupt<PIN_CS,PIN_RST,PIN_INT>::FInstnce = nullptr;
//---------------------------------------------------------------------------
	template<int PIN_CS, int PIN_RST> class MitovLoRaRFM9XShield
	{
		#include <Mitov_LoRa_RFM9X_Functions.h>

	protected:
		void Reset()
		{
			digitalWrite( PIN_CS, HIGH );

//				Serial.println( "RESET" );
			digitalWrite( PIN_RST, LOW );
			delay( 100 );
			digitalWrite( PIN_RST, HIGH );
			delay( 100 );
		}

		void readFifo( uint8_t ALength )
		{
//			Serial.print( "LN: " ); Serial.println( ALength );

			// Reset the fifo read ptr to the beginning of the packet
			spiWrite(RH_RF95_REG_0D_FIFO_ADDR_PTR, spiRead(RH_RF95_REG_10_FIFO_RX_CURRENT_ADDR));

			SPI.beginTransaction( SPISettings() );
			digitalWrite( PIN_CS, LOW );

			SPI.transfer(RH_RF95_REG_00_FIFO); // Send the start address with the write mask off

			uint8_t *ABuffer = new uint8_t[ 1 + ALength ];
			FReceivedPackets.push_back( ABuffer );
			*ABuffer++ = ALength; // Skip the ARxHeaderTo

			for ( int i = 0; i < ALength; ++i )
				*ABuffer++ = SPI.transfer(0);

			digitalWrite( PIN_CS, HIGH );
			SPI.endTransaction();
		}

		void TransferPacket( const uint8_t* data, uint8_t len )
		{
			SPI.beginTransaction( SPISettings() );
			digitalWrite( PIN_CS, LOW );
			SPI.transfer( RH_RF95_REG_00_FIFO | 0x80 ); // Send the start address with the write mask on
//			SPI.transfer( len + 1 ); // Include length of headers
			// First the 4 headers
//			SPI.transfer( ATxHeaderTo );
//			SPI.transfer( Address );
//			SPI.transfer(_txHeaderId);
//			SPI.transfer(_txHeaderFlags);
			// Now the payload
			while( len-- )
				SPI.transfer( *data++ );

			digitalWrite( PIN_CS, HIGH );
			SPI.endTransaction();
		}

		uint8_t spiRead(uint8_t reg)
		{
			SPI.beginTransaction( SPISettings() );
			digitalWrite( PIN_CS, LOW );
			SPI.transfer(reg & ~0x80); // Send the address with the write mask off
			uint8_t val = SPI.transfer(0); // The written value is ignored, reg value is read
			digitalWrite( PIN_CS, HIGH );
			SPI.endTransaction();
//Serial.print( "spiRead: " ); Serial.print( reg, HEX ); Serial.print( " = " ); Serial.println( val, HEX );
			return val;
		}

		uint8_t spiWrite(uint8_t reg, uint8_t val)
		{
//Serial.print( "spiWrite: " ); Serial.print( reg, HEX ); Serial.print( " = " ); Serial.println( val, HEX );
			SPI.beginTransaction( SPISettings() );
			digitalWrite( PIN_CS, LOW );
			uint8_t status = SPI.transfer(reg | 0x80); // Send the address with the write mask on
			SPI.transfer(val); // New value follows
			digitalWrite( PIN_CS, HIGH );
			SPI.endTransaction();
			return status;
		}

	protected:
		void ProcessInterrupt()
		{
			// Read the interrupt register
			uint8_t irq_flags = spiRead(RH_RF95_REG_12_IRQ_FLAGS);

			if ( FMode == RHModeRx && irq_flags & (RH_RF95_RX_TIMEOUT | RH_RF95_PAYLOAD_CRC_ERROR))
			{
				spiWrite(RH_RF95_REG_12_IRQ_FLAGS, 0xff); // Clear all IRQ flags
				; //_rxBad++;
			}

			else if ( FMode == RHModeRx && irq_flags & RH_RF95_RX_DONE)
			{
				spiWrite(RH_RF95_REG_12_IRQ_FLAGS, 0xff); // Clear all IRQ flags
				// Have received a packet
				uint8_t len = spiRead(RH_RF95_REG_13_RX_NB_BYTES);

				// Reset the fifo read ptr to the beginning of the packet
//				spiWrite(RH_RF95_REG_0D_FIFO_ADDR_PTR, spiRead(RH_RF95_REG_10_FIFO_RX_CURRENT_ADDR));
//				spiBurstRead(RH_RF95_REG_00_FIFO, _buf, len);
//				_bufLen = len;
//				spiWrite(RH_RF95_REG_12_IRQ_FLAGS, 0xff); // Clear all IRQ flags

				// Remember the RSSI of this packet
				// this is according to the doc, but is it really correct?
				// weakest receiveable signals are reported RSSI at about -66
				FLastRssi = spiRead(RH_RF95_REG_1A_PKT_RSSI_VALUE) - 137;

				readFifo( len );

				// We have received a message.
//				validateRxBuf(); 
//				if (_rxBufValid)
					setModeIdle(); // Got one 
			}

			else if ( FMode == RHModeTx && irq_flags & RH_RF95_TX_DONE)
			{
				spiWrite(RH_RF95_REG_12_IRQ_FLAGS, 0xff); // Clear all IRQ flags
//				_txGood++;
				setModeIdle();
			}
		}

	public:
		inline void SystemInit()
		{
			SPI.begin();
			IntSystemInit();
		}

	public:
		MitovLoRaRFM9XShield() :
			SymbolTimeout( 0x64 ),
			LoRaTMMode( true ),
			MobileNode( false ),
			LowNoiseAmplifierHighFrequencyBoost( false ),
			Gain( gcAuto ),
			CodingRate( cr4_5 ),
			SpreadingFactorOrder( 7 ),
			FReady( false ),
			FModeChange( false ),
			FMode( RHModeInitialising )
		{
			pinMode( PIN_RST, OUTPUT );
			pinMode( PIN_CS, OUTPUT );
		}
	};
//---------------------------------------------------------------------------
}

#ifdef __TEST_FOR_DEBUG_PRINTS__
#undef Serial
#endif

#endif
