////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2018 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_FORMATTED_SERIAL_h
#define _MITOV_FORMATTED_SERIAL_h

#include <Mitov.h>

namespace Mitov
{
#if ! ( defined(VISUINO_CHIPKIT) || defined( VISUINO_STMDUINO ) || defined( VISUINO_INTEL_EDISON ))

#ifdef VISUINO_ESP8266
	static const	SerialConfig CSerialInits[] = 
#else
	static const	long CSerialInits[] = 
#endif
	{
		SERIAL_5N1,
		SERIAL_6N1,
		SERIAL_7N1,
		SERIAL_8N1,
		SERIAL_5N2,
		SERIAL_6N2,
		SERIAL_7N2,
		SERIAL_8N2,
		SERIAL_5E1,
		SERIAL_6E1,
		SERIAL_7E1,
		SERIAL_8E1,
		SERIAL_5E2,
		SERIAL_6E2,
		SERIAL_7E2,
		SERIAL_8E2,
		SERIAL_5O1,
		SERIAL_6O1,
		SERIAL_7O1,
		SERIAL_8O1,
		SERIAL_5O2,
		SERIAL_6O2,
		SERIAL_7O2,
		SERIAL_8O2
	};

#endif // VISUINO_CHIPKIT
//---------------------------------------------------------------------------
	template<typename T_SERIAL_TYPE, T_SERIAL_TYPE &C_SERIAL> class SerialPort
	{
	public:
		uint32_t	Speed = 9600;

	public:
#ifndef VISUINO_CHIPKIT
		TArduinoSerialParity	Parity	 : 4;
		unsigned int			StopBits : 4;
		unsigned int			DataBits : 4;
#endif // VISUINO_CHIPKIT

#ifdef VISUINO_ESP8266
		bool	ReceiveEnabled : 1;
		bool	TransmitEnabled : 1;
		bool	UseAlternativePins : 1;

#endif // VISUINO_ESP8266

	protected:
		inline void UpdateSending( bool ASending ) {} // Placeholder

#ifdef _MITOV_SERIAL_SENDING_PIN_
	#ifndef VISUINO_CHIPKIT
		uint32_t GetByteSpeed()
		{ 
			return Speed / ( StopBits + DataBits + 1 ); 
		}
	#endif // VISUINO_CHIPKIT
#endif

	protected:
		inline void SerialEnd() {} // Placeholder

		void StartPort()
		{
#if ( defined( VISUINO_CHIPKIT ) || defined( VISUINO_STMDUINO ) || defined( VISUINO_INTEL_EDISON ))
			C_SERIAL.begin( Speed );

#else // VISUINO_CHIPKIT

			int AIndex = ((int)Parity) * 8 + ( StopBits - 1 ) * 4 + ( DataBits - 5);
#ifdef VISUINO_ESP8266
			if( ReceiveEnabled && TransmitEnabled )
				C_SERIAL.begin( Speed, CSerialInits[ AIndex ], SERIAL_FULL );

			else if( ReceiveEnabled )
				C_SERIAL.begin( Speed, CSerialInits[ AIndex ], SERIAL_RX_ONLY );

			else if( TransmitEnabled )
				C_SERIAL.begin( Speed, CSerialInits[ AIndex ], SERIAL_TX_ONLY );

			if( UseAlternativePins )
				C_SERIAL.swap();

#else // VISUINO_ESP8266
			C_SERIAL.begin( Speed, CSerialInits[ AIndex ] );
#endif // VISUINO_ESP8266

#endif // VISUINO_CHIPKIT
		}

	public:
#if ! ( defined(VISUINO_CHIPKIT) || defined(VISUINO_STMDUINO ))
		SerialPort() :
#ifdef VISUINO_ESP8266
			ReceiveEnabled( true ),
			TransmitEnabled( true ),
#endif // VISUINO_ESP8266
			Parity( spNone ),
			StopBits( 1 ),
			DataBits( 8 )
		{
		}
#endif // VISUINO_CHIPKIT
	};
//---------------------------------------------------------------------------
#ifdef VISUINO_ESP32
	template<typename T_SERIAL_TYPE, T_SERIAL_TYPE &C_SERIAL, int C_RX_PIN, int C_TX_PIN> class SerialESP32Port : public Mitov::SerialPort<T_SERIAL_TYPE, C_SERIAL>
	{
		typedef	Mitov::SerialPort<T_SERIAL_TYPE, C_SERIAL> inherited;

	protected:
		inline void SerialEnd() {} // Placeholder

		void StartPort()
		{
			int AIndex = ((int)inherited::Parity) * 8 + ( inherited::StopBits - 1 ) * 4 + ( inherited::DataBits - 5);
			C_SERIAL.begin( inherited::Speed, CSerialInits[ AIndex ], C_RX_PIN, C_TX_PIN );
		}
	};
#endif // VISUINO_ESP32
//---------------------------------------------------------------------------
} // Mitov

#endif

