////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2018 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_CONTROLLINO_RS485_h
#define _MITOV_CONTROLLINO_RS485_h

#include <Mitov.h>
#include <Controllino.h>

#ifdef __TEST_FOR_DEBUG_PRINTS__
#define Serial UNGUARDED DEBUG PRINT!!!
#endif

namespace Mitov
{
//---------------------------------------------------------------------------
	template<typename T_SERIAL_TYPE, T_SERIAL_TYPE &C_SERIAL> class ControllinoRS48SerialPort
	{
	public:
		uint32_t				Speed = 9600;
		TArduinoSerialParity	Parity	 : 4;
		unsigned int			StopBits : 4;
		unsigned int			DataBits : 4;

	protected:
#ifdef _MITOV_SERIAL_SENDING_PIN_
		uint32_t GetByteSpeed()
		{ 
			return Speed / ( StopBits + DataBits + 1 ); 
		}
#endif

		inline void UpdateSending( bool ASending )
		{
#if ((!defined _MITOV_CONTROLLINO_RS485_TRANSMIT_PIN_ ) || (!defined _MITOV_CONTROLLINO_RS485_RECEIVE_PIN_ ))
//			Serial.print( "UpdateSending: " );
//			Serial.println( inherited::FSending );
#ifndef _MITOV_CONTROLLINO_RS485_TRANSMIT_PIN_
			Controllino_SwitchRS485DE( ASending );
#endif

#ifndef _MITOV_CONTROLLINO_RS485_RECEIVE_PIN_
			Controllino_SwitchRS485RE( ASending );
#endif
//			Controllino_SwitchRS485DE( 1 );
//			Controllino_SwitchRS485RE( 1 );

//			inherited::UpdateSending();
#endif
		}

	protected:
		inline void SerialEnd() {} // Placeholder

		void StartPort()
		{
			int AIndex = ((int)Parity) * 8 + ( StopBits - 1 ) * 4 + ( DataBits - 5);
			C_SERIAL.begin( Speed, CSerialInits[ AIndex ] );
		}

	public:
		ControllinoRS48SerialPort() :
			Parity( spNone ),
			StopBits( 1 ),
			DataBits( 8 )
		{
		}
	};
//---------------------------------------------------------------------------
	class ControllinoRS485Module
	{
	public:
		bool	FTransmit : 1;
		bool	FReceive : 1;

	public:
		bool	Enabled : 1;

	public:
		void SetEnabled( bool AValue )
		{
			if( Enabled == AValue )
				return;

			Enabled = AValue;
			if( Enabled )
				StartModule();

			else
				StopModule();

		}

	public:
		inline void SystemInit()
		{
			if( Enabled )
				StartModule();
		}

		inline void SystemLoopBegin( unsigned long currentMicros ) {} // Placeholder

	protected:
		void StopModule()
		{
		}

		void StartModule()
		{
//			Serial.println( "START" );
			Controllino_RS485Init();
#ifdef _MITOV_CONTROLLINO_RS485_TRANSMIT_PIN_
			Controllino_SwitchRS485DE( FTransmit );
#endif
#ifdef _MITOV_CONTROLLINO_RS485_RECEIVE_PIN_
			Controllino_SwitchRS485RE( ! FReceive );
#endif
		}

	public:
		void TransmitInputPin_o_Receive( void *_Data )
		{
			FTransmit = *(bool *)_Data;
			if( ! Enabled )
				return;

			Controllino_SwitchRS485DE( FTransmit );
		}

		void ReceiveInputPin_o_Receive( void *_Data )
		{
			FReceive = *(bool *)_Data;
			if( ! Enabled )
				return;

			Controllino_SwitchRS485RE( ! FReceive );
		}

	public:
		ControllinoRS485Module() :
			Enabled( true ),
			FTransmit( false ),
			FReceive( false )
		{
		}

	};
}

#ifdef __TEST_FOR_DEBUG_PRINTS__
#undef Serial
#endif

#endif
