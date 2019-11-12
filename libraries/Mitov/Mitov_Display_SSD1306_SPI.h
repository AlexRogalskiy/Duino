////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2018 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_DISPLAY_SSD1306_SPI_h
#define _MITOV_DISPLAY_SSD1306_SPI_h

#include <Mitov_Display_SSD1306.h>

#ifdef __TEST_FOR_DEBUG_PRINTS__
#define Serial UNGUARDED DEBUG PRINT!!!
#endif

namespace Mitov
{
//---------------------------------------------------------------------------
	template<typename T_RESET_IMPLEMENTATION, typename T_SPI, T_SPI &C_SPI, typename T_CHIP_SELECT_IMPLEMENTATION, typename T_REGISTER_SELECT_IMPLEMENTATION> class Display_SPI : public T_RESET_IMPLEMENTATION, public T_CHIP_SELECT_IMPLEMENTATION, public T_REGISTER_SELECT_IMPLEMENTATION
	{
	public:
		inline OpenWire::SourcePin & ChipSelectOutputPin() 
		{
			return T_CHIP_SELECT_IMPLEMENTATION::GetPinInstance();
		}

	public:
		void SendCommand6( uint8_t ACommand1, uint8_t ACommand2, uint8_t ACommand3, uint8_t ACommand4, uint8_t ACommand5, uint8_t ACommand6 )
		{
			T_REGISTER_SELECT_IMPLEMENTATION::SetPinValueLow();
			T_CHIP_SELECT_IMPLEMENTATION::SetPinValueLow();

			C_SPI.transfer( ACommand1 );
			C_SPI.transfer( ACommand2 );
			C_SPI.transfer( ACommand3 );
			C_SPI.transfer( ACommand4 );
			C_SPI.transfer( ACommand5 );
			C_SPI.transfer( ACommand6 );

			T_CHIP_SELECT_IMPLEMENTATION::SetPinValueHigh();
		}

		void SendCommand( uint8_t ACommand )
		{
//			T_CHIP_SELECT_IMPLEMENTATION::SetPinValue( true );
			T_REGISTER_SELECT_IMPLEMENTATION::SetPinValueLow();
			T_CHIP_SELECT_IMPLEMENTATION::SetPinValueLow();

			C_SPI.transfer( ACommand );

			T_CHIP_SELECT_IMPLEMENTATION::SetPinValueHigh();
		}

		void SendData16( uint16_t AData )
		{
			StartSendData();

//			C_SPI.transfer( AData >> 8 );
			C_SPI.transfer16( AData );

			T_CHIP_SELECT_IMPLEMENTATION::SetPinValueHigh();
		}

		inline void spiwrite( uint8_t AHi, uint8_t ALo) 
		{
			C_SPI.transfer( AHi );
			C_SPI.transfer( ALo );
		}

		inline void transfer( uint8_t ACommand )
		{
			C_SPI.transfer( ACommand );
		}

		inline void StartSendData()
		{
			T_REGISTER_SELECT_IMPLEMENTATION::SetPinValueHigh();
			T_CHIP_SELECT_IMPLEMENTATION::SetPinValueLow();
		}

		inline void SetChipSelectHigh()
		{
			T_CHIP_SELECT_IMPLEMENTATION::SetPinValueHigh();
		}

		inline void SetChipSelectLow()
		{
			T_CHIP_SELECT_IMPLEMENTATION::SetPinValueLow();
		}

		inline void IntSystemInitStart() 
		{
			T_CHIP_SELECT_IMPLEMENTATION::SetPinValueHigh();
		}

		inline void SetResetHigh()
		{
			T_RESET_IMPLEMENTATION::SetPinValueHigh();
		}

		inline void SetResetLow()
		{
			T_RESET_IMPLEMENTATION::SetPinValueLow();
		}

		inline void SetRegisterSelectHigh()
		{
			T_REGISTER_SELECT_IMPLEMENTATION::SetPinValueHigh();
		}

		inline void SetRegisterSelectLow()
		{
			T_REGISTER_SELECT_IMPLEMENTATION::SetPinValueLow();
		}

		inline void IntSystemInitEnd() {} // Placeholder
	};
//---------------------------------------------------------------------------
	template<typename T_SPI, T_SPI &C_SPI, typename T_RESET_IMPLEMENTATION, typename T_CHIP_SELECT_IMPLEMENTATION, typename T_REGISTER_SELECT_IMPLEMENTATION, uint8_t WIDTH, uint8_t HEIGHT> class Display_odtSSD1306_SPI : public Display_SPI<T_RESET_IMPLEMENTATION, T_SPI, C_SPI, T_CHIP_SELECT_IMPLEMENTATION, T_REGISTER_SELECT_IMPLEMENTATION>
	{
		typedef Display_SPI<T_RESET_IMPLEMENTATION, T_SPI, C_SPI, T_CHIP_SELECT_IMPLEMENTATION, T_REGISTER_SELECT_IMPLEMENTATION> inherited;

	public:
/*
		inline void SendPinsConfigCommand()
		{
			inherited::SendCommand( C_COMPinsConfig );
		}
*/
		void SendBuffer( uint8_t *ABuffer )
		{
/*
			inherited::SendCommand(SSD1306Const::SSD1306_COLUMNADDR);
			inherited::SendCommand(0);   // Column start address (0 = reset)
			inherited::SendCommand( WIDTH - 1 ); // Column end address (127 = reset)

			inherited::SendCommand(SSD1306Const::SSD1306_PAGEADDR);
			inherited::SendCommand(0); // Page start address (0 = reset)
*/
			T_REGISTER_SELECT_IMPLEMENTATION::SetPinValueHigh();
			T_CHIP_SELECT_IMPLEMENTATION::SetPinValueLow();
//			T_CHIP_SELECT_IMPLEMENTATION::StartSendData();
//			T_CHIP_SELECT_IMPLEMENTATION::SetPinValue( true );

			for (uint16_t i = 0; i < ( WIDTH * HEIGHT / 8 ); ++i )
				C_SPI.transfer( ABuffer[ i ] );

			T_CHIP_SELECT_IMPLEMENTATION::SetPinValueHigh();
		}

	};
//---------------------------------------------------------------------------
	template<typename T_SPI, T_SPI &C_SPI, typename T_RESET_IMPLEMENTATION, typename T_CHIP_SELECT_IMPLEMENTATION, typename T_REGISTER_SELECT_IMPLEMENTATION, uint8_t WIDTH, uint8_t HEIGHT> class Display_odtSH1106_SPI : public Display_odtSSD1306_SPI<T_SPI, C_SPI, T_RESET_IMPLEMENTATION, T_CHIP_SELECT_IMPLEMENTATION, T_REGISTER_SELECT_IMPLEMENTATION, WIDTH, HEIGHT> 
	{
		typedef Display_odtSSD1306_SPI<T_SPI, C_SPI, T_RESET_IMPLEMENTATION, T_CHIP_SELECT_IMPLEMENTATION, T_REGISTER_SELECT_IMPLEMENTATION, WIDTH, HEIGHT>  inherited;

	public:
		void SendBuffer( uint8_t *ABuffer )
		{
			inherited::SendCommand(SSD1306Const::SSD1306_SETLOWCOLUMN | 0x0);  // low col = 0
			inherited::SendCommand(SSD1306Const::SSD1306_SETHIGHCOLUMN | 0x0);  // hi col = 0
			inherited::SendCommand(SSD1306Const::SSD1306_SETSTARTLINE | 0x0); // line #0

			byte height=64;
			byte width=132; 
			height >>= 3;
			width >>= 3;
			// I2C
			byte m_row = 0;
			byte m_col = 2;
			int p = 0;
			for ( byte i = 0; i < height; i++)
			{
				// send a bunch of data in one xmission
				inherited::SendCommand(0xB0 + i + m_row);//set page address
				inherited::SendCommand(m_col & 0xf);//set lower column address
				inherited::SendCommand(0x10 | (m_col >> 4));//set higher column address

				for( byte j = 0; j < 8; j++)
				{
//					T_CHIP_SELECT_IMPLEMENTATION::SetPinValue( true );
//					T_REGISTER_SELECT_IMPLEMENTATION::SetPinValue( true );
//					T_CHIP_SELECT_IMPLEMENTATION::SetPinValue( false );

					T_REGISTER_SELECT_IMPLEMENTATION::SetPinValueHigh();
					T_CHIP_SELECT_IMPLEMENTATION::SetPinValueLow();
					for ( byte k = 0; k < width; k++, p++)
						C_SPI.transfer( ABuffer[ p ] );

					T_CHIP_SELECT_IMPLEMENTATION::SetPinValueHigh();
				}
			}
		}
	};
//---------------------------------------------------------------------------
}

#ifdef __TEST_FOR_DEBUG_PRINTS__
#undef Serial
#endif

#endif
