////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2018 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_GENERIC_SPI_EEPROM_h
#define _MITOV_GENERIC_SPI_EEPROM_h

#include <Mitov.h>
#include <Mitov_Memory.h>

#ifdef __TEST_FOR_DEBUG_PRINTS__
#define Serial UNGUARDED DEBUG PRINT!!!
#endif

namespace Mitov
{
//---------------------------------------------------------------------------
	namespace ArduinoGenericEEPROM_SPI_Consts
	{
		const uint8_t OPCODE_WREN   = 0b0110;     // Write Enable Latch
		const uint8_t OPCODE_WRDI   = 0b0100;     // Reset Write Enable Latch
		const uint8_t OPCODE_RDSR   = 0b0101;     // Read Status Register
		const uint8_t OPCODE_WRSR   = 0b0001;     // Write Status Register
		const uint8_t OPCODE_READ   = 0b0011;     // Read Memory
		const uint8_t OPCODE_WRITE  = 0b0010;     // Write Memory
//		const uint8_t OPCODE_RDID   = 0b10011111;  // Read Device ID
	};
//---------------------------------------------------------------------------
	template<uint32_t C_UNIQUE, typename T_IMPLEMENTATION, typename T_SPI, T_SPI &C_SPI> class EEPROMWriteProtect_StandardPinImplementation : public T_IMPLEMENTATION
	{
	public:
		OpenWire::SourcePin	WriteProtectedOutputPin;

	protected:
		inline void SetChipSelectHigh()
		{
			T_IMPLEMENTATION::SetPinValueHigh();
		}

		inline void SetChipSelectLow()
		{
			T_IMPLEMENTATION::SetPinValueLow();
		}

	protected:
		inline void SetWriteProtected( bool AValue )
		{
			WriteProtectedOutputPin.SendValue( AValue );
		}

	public:
		inline void SystemLoopBegin( unsigned long currentMicros )
		{
			T_IMPLEMENTATION::SetChipSelectLow();

			C_SPI.transfer( ArduinoGenericEEPROM_SPI_Consts::OPCODE_RDSR );
			uint8_t Avalue = C_SPI.transfer( 0 );

			SetWriteProtected( ( Avalue & 0x80 ) != 0 );

			T_IMPLEMENTATION::SetChipSelectHigh();
		}

	};
//---------------------------------------------------------------------------
	template<uint8_t C_PIN_NUMBER, typename T_IMPLEMENTATION, typename T_SPI, T_SPI &C_SPI> class EEPROMWriteProtect_DirectBoardPinImplementation : public T_IMPLEMENTATION
	{
	protected:
		inline void SetChipSelectHigh()
		{
			T_IMPLEMENTATION::SetPinValueHigh();
		}

		inline void SetChipSelectLow()
		{
			T_IMPLEMENTATION::SetPinValueLow();
		}

	protected:
		inline void SetWriteProtected( bool AValue )
		{
			digitalWrite( C_PIN_NUMBER, AValue ? HIGH : LOW );
		}

	public:
		inline void SystemLoopBegin( unsigned long currentMicros )
		{
			T_IMPLEMENTATION::SetChipSelectLow();

			C_SPI.transfer( ArduinoGenericEEPROM_SPI_Consts::OPCODE_RDSR );
			byte Avalue = C_SPI.transfer( 0 );

			SetWriteProtected( ( Avalue & 0x80 ) != 0 );

			T_IMPLEMENTATION::SetChipSelectHigh();
		}

	public:
		EEPROMWriteProtect_DirectBoardPinImplementation()
		{
			pinMode( C_PIN_NUMBER, OUTPUT );
		}

	};
//---------------------------------------------------------------------------
	template<typename T_PIN, T_PIN &C_PIN, typename T_IMPLEMENTATION, typename T_SPI, T_SPI &C_SPI> class EEPROMWriteProtect_DirectPinImplementation : public T_IMPLEMENTATION
	{
	protected:
		inline void SetChipSelectHigh()
		{
			T_IMPLEMENTATION::SetPinValueHigh();
		}

		inline void SetChipSelectLow()
		{
			T_IMPLEMENTATION::SetPinValueLow();
		}

	protected:
		inline void SetWriteProtected( bool AValue )
		{
			C_PIN.DigitalWrite( AValue );
		}

	public:
		inline void SystemLoopBegin( unsigned long currentMicros )
		{
			T_IMPLEMENTATION::SetChipSelectLow();

			C_SPI.transfer( ArduinoGenericEEPROM_SPI_Consts::OPCODE_RDSR );
			byte Avalue = C_SPI.transfer( 0 );

			SetWriteProtected( ( Avalue & 0x80 ) != 0 );

			T_IMPLEMENTATION::SetChipSelectHigh();
		}

	public:
		EEPROMWriteProtect_DirectPinImplementation()
		{
			C_PIN.SetMode( OUTPUT );
		}

	};
//---------------------------------------------------------------------------
	template<uint32_t C_UNIQUE, typename T_IMPLEMENTATION, typename T_SPI, T_SPI &C_SPI> class EEPROMWriteProtect_NoImplementation : public T_IMPLEMENTATION
	{
	protected:
		inline void SetChipSelectHigh()
		{
			T_IMPLEMENTATION::SetPinValueHigh();
		}

		inline void SetChipSelectLow()
		{
			T_IMPLEMENTATION::SetPinValueLow();
		}

	protected:
		inline void SetWriteProtected( bool AValue )
		{
		}

	public:
		inline void SystemLoopBegin( unsigned long currentMicros )
		{
		}

	};
//---------------------------------------------------------------------------
	enum ArduinoEEPROMProtectedBlocks { pbNone, pbUppeQuarter, pbUppeHalf, pbAll };
//---------------------------------------------------------------------------
	template<typename T_IMPLEMENTATION, typename T_SPI, T_SPI &C_SPI> class ArduinoGenericEEPROM_SPI : public T_IMPLEMENTATION, public Mitov::MemoryIntf
	{
	public:
		OpenWire::SourcePin &ChipSelectOutputPin() { return T_IMPLEMENTATION::GetPinInstance(); }

	public:
		bool	Enabled : 1;
		bool	WriteEnable : 1; // = false;
		ArduinoEEPROMProtectedBlocks ProtectedBlocks : 2;

	protected:
		bool		FCached : 1;
		uint32_t	FCachedAddress = 0;
		uint8_t		FCachedValue = 0;

	public:
		void UpdateWriteProtect()
		{
			T_IMPLEMENTATION::SetChipSelectLow();
			C_SPI.transfer( WriteEnable ? ArduinoGenericEEPROM_SPI_Consts::OPCODE_WREN : ArduinoGenericEEPROM_SPI_Consts::OPCODE_WRDI );
			T_IMPLEMENTATION::SetChipSelectHigh();
		}

		void UpdateProtectedBlocks()
		{
			T_IMPLEMENTATION::SetChipSelectLow();
			C_SPI.transfer( ArduinoGenericEEPROM_SPI_Consts::OPCODE_WREN );
			T_IMPLEMENTATION::SetChipSelectHigh();

			T_IMPLEMENTATION::SetChipSelectLow();
			C_SPI.transfer( ArduinoGenericEEPROM_SPI_Consts::OPCODE_WRSR );
			C_SPI.transfer( ((byte)ProtectedBlocks ) << 2 );
			T_IMPLEMENTATION::SetChipSelectHigh();
		}
		
	public:
		virtual bool ReadData( uint32_t AAddress, uint32_t ASize, byte *AData ) override
		{
			if( ! Enabled )
				return false;

			if( FCached )
				if( AAddress == FCachedAddress )
				{
					*AData ++ = FCachedValue;
					--ASize;
					++ AAddress;
				}

			if( ASize == 0 )
				return	true;

			T_IMPLEMENTATION::SetChipSelectLow();

			C_SPI.transfer( ArduinoGenericEEPROM_SPI_Consts::OPCODE_READ );
			C_SPI.transfer( uint8_t(AAddress >> 8) );
			C_SPI.transfer( uint8_t(AAddress & 0xFF) );
			for( int i = 0; i < ASize; ++i )
				*AData ++ = C_SPI.transfer(0);

			T_IMPLEMENTATION::SetChipSelectHigh();

			FCached = true;
			FCachedAddress = AAddress + ASize - 1;
			FCachedValue = AData[ -1 ];

			return true;
		}

		virtual bool WriteData( uint32_t AAddress, uint32_t ASize, byte *AData ) override
		{
			if( ! Enabled )
				return false;

			FCached = false;

			UpdateWriteProtect();

			T_IMPLEMENTATION::SetChipSelectLow();
			C_SPI.transfer( ArduinoGenericEEPROM_SPI_Consts::OPCODE_WRITE );
			C_SPI.transfer( uint8_t(AAddress >> 8));
			C_SPI.transfer( uint8_t(AAddress & 0xFF));
			for( int i = 0; i < ASize; ++i )
				C_SPI.transfer( *AData ++ );

			T_IMPLEMENTATION::SetChipSelectHigh();
			return true;
		}

	public:
		inline void SystemInit()
		{
			uint32_t AAddress = 0;
			byte AOffset = 0;
			for( int i = 0; i < FElements.size(); ++ i )
				FElements[ i ]->PopulateAddress( AAddress, AOffset );

			UpdateWriteProtect();
			UpdateProtectedBlocks();
		}

		inline void SystemLoopUpdateHardware()
		{
			if( WriteEnable )
				for( int i = 0; i < FElements.size(); ++ i )
					FElements[ i ]->ProcessWrite( this );

		}

	public:
		ArduinoGenericEEPROM_SPI() :
			Enabled( true ),
			WriteEnable( false ),
			ProtectedBlocks( pbNone ),
			FCached( false )
		{
		}

	};
//---------------------------------------------------------------------------
}

#ifdef __TEST_FOR_DEBUG_PRINTS__
#undef Serial
#endif

#endif
