////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2018 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_EVIVE_DISPLAY_h
#define _MITOV_EVIVE_DISPLAY_h

#include <Mitov.h>
#include <Mitov_Display_ST7735.h>
#include <Mitov_Display_ST7735_Const_R.h>
#include <Mitov_Display_ST7735_Const_R2.h>

#ifdef __TEST_FOR_DEBUG_PRINTS__
#define Serial UNGUARDED DEBUG PRINT!!!
#endif

namespace Mitov
{
/*
	template<typename T_SPI, T_SPI &C_SPI> class ArduinoEVIVEDisplay : public DisplayST7735Basic<T_SPI, C_SPI>
	{
		typedef DisplayST7735Basic<T_SPI, C_SPI> inherited;

	protected:
		static const uint16_t WIDTH = 160;
		static const uint16_t HEIGHT = 128;

	protected:
		virtual void UpdateOrientation() override
		{
			inherited::writecommand(ST7735::MADCTL);

			switch( inherited::Orientation )
			{
				case goUp:		inherited::writedata(ST7735::MADCTL_MY | ST7735::MADCTL_MV | ST7735::MADCTL_BGR); break;
				case goRight: 	inherited::writedata(ST7735::MADCTL_MX | ST7735::MADCTL_MY | ST7735::MADCTL_BGR); break;
				case goDown: 	inherited::writedata(ST7735::MADCTL_MX | ST7735::MADCTL_MV | ST7735::MADCTL_BGR); break;
				case goLeft:	inherited::writedata(ST7735::MADCTL_BGR); break;
			}
		}

	public:
		int16_t width(void) const
		{
			if( inherited::Orientation == goUp || inherited::Orientation == goDown )
				return WIDTH;

			return HEIGHT;
		}

		int16_t height(void) const
		{
			if( inherited::Orientation == goUp || inherited::Orientation == goDown )
				return HEIGHT;

			return WIDTH;
		}

//#ifdef	_MITOV_ST7735_RESET_PIN_
		virtual void SetResetOutput( bool AValue ) override
		{
			digitalWrite( 47, AValue ? HIGH : LOW );
		}
//#endif

		virtual void SetRegisterSelectOutput( bool AValue ) override
		{
			digitalWrite( 49, AValue ? HIGH : LOW );
		}

		virtual void SetChipSelectOutput( bool AValue ) override
		{
			digitalWrite( 48, AValue ? HIGH : LOW );
		}

	public:
		inline void SystemInit()
		{
			inherited::commonInit(Rcmd1);
			inherited::commandList(Rcmd2red);
			inherited::commandList(Rcmd3);
			inherited::writecommand(ST7735::MADCTL);
			inherited::writedata(0xC0);

			inherited::SystemInit();
		}

	public:
		ArduinoEVIVEDisplay( const unsigned char * AFont ) :
			inherited( AFont )
		{
			pinMode( 47, OUTPUT );
			pinMode( 48, OUTPUT );
			pinMode( 49, OUTPUT );

			inherited::colstart = 0;
			inherited::rowstart = 0;
		}
	};
//---------------------------------------------------------------------------
*/
//---------------------------------------------------------------------------
	template<typename T_SPI, T_SPI &C_SPI, uint8_t C_WIDTH, uint8_t C_HEIGHT, uint8_t C_PIXEL_MODE> class ArduinoEVIVEDisplay : public BasicSPI_ST7735<T_SPI, C_SPI, Mitov::ST7735_Reset_DirectBoardPinImplementation<47>,Mitov::DigitalPin_DirectBoardPinImplementation<49>,Mitov::DigitalPin_DirectBoardPinImplementation<48>, 0, 0>
	{
		typedef BasicSPI_ST7735<T_SPI, C_SPI, Mitov::ST7735_Reset_DirectBoardPinImplementation<47>,Mitov::DigitalPin_DirectBoardPinImplementation<49>,Mitov::DigitalPin_DirectBoardPinImplementation<48>, 0, 0> inherited;

//	protected:
//		static const uint16_t WIDTH = 128;
//		static const uint16_t HEIGHT = 160;

	public:
		void UpdateOrientation() // Used by the bnding setters
		{
			inherited::writecommand( ST7735::MADCTL );

			inherited::writedata( C_PIXEL_MODE );
/*
			switch( inherited::Orientation )
			{
				case goUp:		inherited::writedata(ST7735::MADCTL_MX | ST7735::MADCTL_MY | ST7735::MADCTL_RGB); break;
				case goLeft:	inherited::writedata(ST7735::MADCTL_MX | ST7735::MADCTL_MV | ST7735::MADCTL_RGB); break;
				case goDown: 	inherited::writedata(ST7735::MADCTL_RGB); break;
				case goRight: 	inherited::writedata(ST7735::MADCTL_MY | ST7735::MADCTL_MV | ST7735::MADCTL_RGB); break;
			}
*/
		}

	public:
		int16_t width(void) const
		{
			return C_WIDTH;
		}

		int16_t height(void) const
		{
			return C_HEIGHT;
		}

	public:
		inline void SystemInit()
		{
			inherited::commonInit( ST7735::Rcmd1 );
			inherited::commandList( ST7735::Rcmd2red );
			inherited::commandList( ST7735::Rcmd3 );
			inherited::writecommand( ST7735::MADCTL );
			inherited::writedata( 0xC0 );
		}

	};
//---------------------------------------------------------------------------
}

#ifdef __TEST_FOR_DEBUG_PRINTS__
#undef Serial
#endif

#endif
