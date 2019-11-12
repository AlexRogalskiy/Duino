////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2018 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_UART_SERIAL_h
#define _MITOV_UART_SERIAL_h

#include <Mitov.h>

namespace Mitov
{
#if ! defined( VISUINO_STMDUINO )
	static const	UARTClass::UARTModes CUARTSerialInits[] = 
	{
		SERIAL_8N1,
		SERIAL_8E1,
		SERIAL_8O1,
		SERIAL_8M1,
		SERIAL_8S1
	};
#endif
//---------------------------------------------------------------------------
	template<typename T_SERIAL_TYPE, T_SERIAL_TYPE &C_SERIAL> class UARTSerialPort : public Mitov::SpeedSerialPort<T_SERIAL_TYPE, C_SERIAL>
	{
		typedef	Mitov::SpeedSerialPort<T_SERIAL_TYPE, C_SERIAL> inherited;

	public:
		TArduinoSerialParity	Parity : 8;

	protected:
		inline void UpdateSending( bool ASending ) {} // Placeholder

		void StartPort()
		{
#if defined( VISUINO_STMDUINO )
			C_SERIAL.begin( inherited::Speed );
#else
			int AIndex = ((int)Parity);
			C_SERIAL.begin( inherited::Speed, CUARTSerialInits[ AIndex ] );
#endif
		}

	public:
		UARTSerialPort() :
			Parity( spNone )
		{
		}
	};
//---------------------------------------------------------------------------
} // Mitov

#endif

