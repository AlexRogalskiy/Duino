////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2018 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_EVIVE_DAC_h
#define _MITOV_EVIVE_DAC_h

#include <Mitov.h>
#include <Mitov_Microchip_MCP4725.h>

namespace Mitov
{
//---------------------------------------------------------------------------
	class ArduinoEVIVEAnalogOutput
	{
	public:
		ClockInputPin_o_IsConnected : 1;

	public:
		bool	Enabled : 1;
		bool	ShitDown : 1;
		bool	WriteToEEPROM : 1;
		TArduinoMicrochipMCP4725ShitDownMode	ShitDownMode : 2;

		float	InitialValue = 0;
		float	CurrentValue = 0;

	protected:
		static const uint8_t	MCP4726_CMD_WRITEDAC		= 0x40;  // Writes data to the DAC
		static const uint8_t	MCP4726_CMD_WRITEDACEEPROM  = 0x60;  // Writes data to the DAC and the EEPROM (persisting the assigned value after reset)

	public:
		void InputPin_o_Receive( void *_Data )
		{
			InitialValue = *(float * )_Data;
			if( Enabled )
				if( ! ClockInputPin_o_IsConnected )
					DoClockReceive( nullptr );

		}

		void ClockInputPin_o_Receive( void *_Data )
		{
			if( ! Enabled )
				return;

//			Serial.println( "DoClockReceive" );
			CurrentValue = InitialValue;
			UpdateValue();
		}

	public:
		void UpdateValue()
		{
			uint16_t AValue = constrain( CurrentValue, 0.0, 1.0 ) * 0xFFF + 0.5;
#ifdef TWBR
			uint8_t twbrback = TWBR;
			TWBR = ((F_CPU / 400000L) - 16) / 2; // Set I2C frequency to 400kHz
#endif
			Wire.beginTransmission( 0x62 );

			uint8_t	ACommand;
  			if( WriteToEEPROM )
				ACommand = MCP4726_CMD_WRITEDACEEPROM;

			else
				ACommand = MCP4726_CMD_WRITEDAC;

			if( ShitDown )
				ACommand |= uint8_t( ShitDownMode ) + 1;

			Wire.write( ACommand );

//			Serial.println( ACommand, HEX );
//			Serial.println( CurrentValue );
//			Serial.println( AValue );

			Wire.write( AValue / 16);                   // Upper data bits          (D11.D10.D9.D8.D7.D6.D5.D4)
			Wire.write(( AValue % 16) << 4);            // Lower data bits          (D3.D2.D1.D0.x.x.x.x)
			Wire.endTransmission();

#ifdef TWBR
			TWBR = twbrback;
#endif		
		}

	public:
		inline void SystemStart()
		{
			if( Enabled )
				DoClockReceive( nullptr );

		}

	public:
		ArduinoEVIVEAnalogOutput() :
			ClockInputPin_o_IsConnected( false ),
			Enabled( true ),
			ShitDown( false ),
			WriteToEEPROM( false ),
			ShitDownMode( sdm500K )
		{
		}

	};
//---------------------------------------------------------------------------
}

#endif
