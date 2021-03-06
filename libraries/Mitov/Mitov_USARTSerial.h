////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2018 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_USART_SERIAL_h
#define _MITOV_USART_SERIAL_h

#include <Mitov.h>

namespace Mitov
{
#if ! defined( VISUINO_STMDUINO )
	static const	USARTClass::USARTModes CUSARTSerialInits[] = 
	{
		SERIAL_5N1,
		SERIAL_6N1,
		SERIAL_7N1,
		SERIAL_8N2,

		SERIAL_5N2,
		SERIAL_6N2,
		SERIAL_7N2,
		SERIAL_8N2,

		SERIAL_5E1,
		SERIAL_6E1,
		SERIAL_7E1,
		SERIAL_8E2,

		SERIAL_5E2,
		SERIAL_6E2,
		SERIAL_7E2,
		SERIAL_8E2,

		SERIAL_5O1,
		SERIAL_6O1,
		SERIAL_7O1,
		SERIAL_8O2,

		SERIAL_5O2,
		SERIAL_6O2,
		SERIAL_7O2,
		SERIAL_8O2,

		SERIAL_5M1,
		SERIAL_6M1,
		SERIAL_7M1,
		SERIAL_8M2,

		SERIAL_5M2,
		SERIAL_6M2,
		SERIAL_7M2,
		SERIAL_8M2,

		SERIAL_5S1,
		SERIAL_6S1,
		SERIAL_7S1,
		SERIAL_8S2,

		SERIAL_5S2,
		SERIAL_6S2,
		SERIAL_7S2,
		SERIAL_8S2
	};
#endif

//---------------------------------------------------------------------------
	template<typename T_SERIAL_TYPE, T_SERIAL_TYPE &C_SERIAL> class USARTSerialPort : public Mitov::SpeedSerialPort<T_SERIAL_TYPE, C_SERIAL>
	{
		typedef	Mitov::SpeedSerialPort<T_SERIAL_TYPE, C_SERIAL> inherited;

	public:
		TArduinoSerialParity	Parity : 3;
		unsigned int			StopBits : 2;
		unsigned int			DataBits : 4;

#ifdef _MITOV_SERIAL_SENDING_PIN_
	protected:
		uint32_t GetByteSpeed()
		{ 
			return Speed / ( StopBits + DataBits + 1 ); 
		}
#endif

	protected:
		inline void UpdateSending( bool ASending ) {} // Placeholder

		void StartPort()
		{
#if defined( VISUINO_STMDUINO )
			C_SERIAL.begin( inherited::Speed );
#else
			int AIndex = ((int)Parity) * 8 + ( StopBits - 1 ) * 4 + ( DataBits - 5);
			C_SERIAL.begin( inherited::Speed, CUSARTSerialInits[ AIndex ] );
#endif
		}

	public:
		USARTSerialPort() :
			Parity( spNone ),
			StopBits( 2 ),
			DataBits( 8 )
		{
		}
	};
//---------------------------------------------------------------------------
} // Mitov

#endif

