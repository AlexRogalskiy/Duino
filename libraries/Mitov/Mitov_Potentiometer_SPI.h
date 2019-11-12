////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2018 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_POTENTIOMETER_SPI_h
#define _MITOV_POTENTIOMETER_SPI_h

#include <Mitov.h>
#include <SPI.h>

namespace Mitov
{
//---------------------------------------------------------------------------
	template<typename T_OWNER, uint8_t C_RESOLUTION> class PotentiometerSPIChannel
	{
	public:
		float	InitialValue = 0.0f;

	public:
		bool	Enabled : 1;

		bool	ConnectTerminalA : 1;
		bool	ConnectWiper : 1;
		bool	ConnectTerminalB : 1;
		bool	NonVolatile : 1;

	protected:
		bool	FConfigModified : 1;
		bool	FValueModified : 1;

	public:
		inline void ConfigModified()
		{
			FValueModified = true;
		}

	public:
		void InputPin_o_Receive( void *_Data )
		{
			float AValue = *(float *)_Data;
			if( InitialValue == AValue )
				return;

			InitialValue = AValue;
			FValueModified = true;
		}

	public:
		void InitChannel( T_OWNER *AOwner, uint8_t AIndex )
		{
			if( ! FConfigModified )
				return;

			FConfigModified = false;

			uint16_t ATerminalControlValue = AOwner->SPI_read( 4 );
			uint16_t ANewValue;
			if( ConnectTerminalB )
				ANewValue = 1;

			else
				ANewValue = 0;

			if( ConnectWiper )
				ANewValue |= B10;

			if( ConnectTerminalA )
				ANewValue |= B100;

			if( ! Enabled )
				ANewValue |= B1000;

			if( AIndex == 1 )
			{
				ATerminalControlValue &= 0xFF0F;
				ATerminalControlValue |= ( ANewValue << 4 );
			}
			else
			{
				ATerminalControlValue &= 0xFFF0;
				ATerminalControlValue |= ANewValue;
			}

			AOwner->SPI_write( 4, ATerminalControlValue );
		}

		void SendOutput( T_OWNER *AOwner, uint8_t AIndex )
		{
			if( ! FValueModified )
				return;

			FValueModified = false;

			int wiper_pos = ( InitialValue * C_RESOLUTION );

			wiper_pos = constrain( wiper_pos, 0, C_RESOLUTION );

			byte data_byte;
			byte cmd_byte = AIndex;

			// Calculate the 9-bit data value to send
			data_byte = (byte)(wiper_pos & 0x00FF);
			if( wiper_pos > C_RESOLUTION - 1 )
				if( C_RESOLUTION == 255 )
					cmd_byte |= B00000001; // Table 5-1 (page 36)

			AOwner->SPI_write( cmd_byte, data_byte );

			if( NonVolatile )
			{
				// EEPROM write cycles take 4ms each. So we block with delay(5); after any NV Writes
				AOwner->SPI_write( cmd_byte | B10, data_byte );
				delay(5);
			}
		}

	public:
		PotentiometerSPIChannel() :
			NonVolatile( false ),
			Enabled( true ),
			ConnectTerminalA( true ),
			ConnectWiper( true ),
			ConnectTerminalB( true ),
			FConfigModified( false ),
			FValueModified( false )
		{
		}

	};
//---------------------------------------------------------------------------
	template<typename T_SPI, T_SPI &C_SPI, uint8_t C_COUNT_CHANNELS, uint8_t C_RESOLUTION> class Potentiometer_SPI
	{
	public:
		OpenWire::SourcePin	ChipSelectOutputPin;

	public:
		bool	Enabled : 1;

	public:
		bool	FModified : 1;

	public:
		PotentiometerSPIChannel<Potentiometer_SPI<T_SPI, C_SPI, C_COUNT_CHANNELS, C_RESOLUTION>, C_RESOLUTION>	Channels[ C_COUNT_CHANNELS ];

	protected:
		static const uint8_t kADR_WIPER0       = B00000000;
		static const uint8_t kADR_WIPER1       = B00010000;

		static const uint8_t kCMD_READ         = B00001100;
	    static const uint8_t kCMD_WRITE        = B00000000;

	    static const uint8_t kADR_VOLATILE     = B00000000;
		static const uint8_t kADR_NON_VOLATILE = B00100000;

		static const uint8_t kTCON_REGISTER    = B01000000;
		static const uint8_t kSTATUS_REGISTER  = B01010000;

	protected:
		inline uint16_t byte2uint16(byte high_byte, byte low_byte)
		{
			return (uint16_t)high_byte<<8 | (uint16_t)low_byte;
		}

	public:
		void	SetEnabled( bool AValue )
		{
			if( Enabled == AValue )
				return;

			Enabled = AValue;
			for( int i =0; i < C_COUNT_CHANNELS; ++i )
				Channels[ i ].InitChannel( this, i );
		}

	public:
		void ClockInputPin_o_Receive( void * )
		{
			for( int i = 0; i < C_COUNT_CHANNELS; ++i )
				Channels[ i ].SendOutput( this, i );
		}

	public:
		void SPI_write(byte cmd_byte, byte data_byte)
		{
			cmd_byte |= kCMD_WRITE;
			ChipSelectOutputPin.SendValue( false );
			C_SPI.transfer(cmd_byte);
			C_SPI.transfer(data_byte);
			ChipSelectOutputPin.SendValue( true );
		}

		uint16_t SPI_read(byte cmd_byte)
		{
			cmd_byte |= kCMD_READ;
			ChipSelectOutputPin.SendValue( false );
			byte high_byte = C_SPI.transfer(cmd_byte);
			byte low_byte  = C_SPI.transfer(0xFF);
			ChipSelectOutputPin.SendValue( true );
			return byte2uint16(high_byte, low_byte);		
		}

	};
//---------------------------------------------------------------------------
}

#endif
