////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2018 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_SABERTOOTH_DUALMOTOR_h
#define _MITOV_SABERTOOTH_DUALMOTOR_h

#include <Mitov.h>

namespace Mitov
{
//---------------------------------------------------------------------------
	template<typename T_SERIAL, T_SERIAL &C_SERIAL> class SabertoothDualMotorSimple
	{
	public:
		OpenWire::SourcePin	SlaveSelectOutputPin;

	public:
		bool	Enabled = true;

	protected:
		float FSpeeds[2] = { -1.0, -1.0 };

	public:
		void SetEnabled( bool AValue )
		{
			if( AValue == Enabled )
				return;

			Enabled = AValue;
			if( Enabled )
				SendStart();

			else
				SendStop();
		}

	protected:
		void SendStart()
		{
			float AValue = FSpeeds[ 0 ];
			FSpeeds[ 0 ] = -1;
			SetSpeed( 0, AValue );

			AValue = FSpeeds[ 1 ];
			FSpeeds[ 1 ] = -1;
			SetSpeed( 1, AValue );
		}

		inline void SendStop()
		{
			SendValue( 0 );
		}

		void SetSpeed( int AIndex, float ASpeed )
		{
			ASpeed = constrain( ASpeed, 0, 1 );
			if( FSpeeds[ AIndex ] == ASpeed )
				return;

			FSpeeds[ AIndex ] = ASpeed;
			if( ! Enabled )
				return;

			byte AValue = ASpeed * 126;

			if( AIndex )
				AValue += 128;

			else
				++ AValue;

			SendValue( AValue );
		}

		void SendValue( byte AValue )
		{
			SlaveSelectOutputPin.SendValue( true );

			C_SERIAL.GetStream().write( AValue );

			if( SlaveSelectOutputPin.IsConnected() )
			{
				delayMicroseconds( 50 );
				SlaveSelectOutputPin.SendValue( false );
			}
		}

	public:
		inline void SystemStart()
		{
			SlaveSelectOutputPin.SendValue( false );
		}

	public:
		inline void MotorsInputPins_o_Receive( int AIndex, void *_Data )
		{
			SetSpeed( AIndex, *(float *)_Data );
		}

	};
//---------------------------------------------------------------------------
	template<typename T_SERIAL, T_SERIAL &C_SERIAL> class SabertoothDualMotorPacket
	{
	public:
		byte	Address = 128;

		float	MinVoltage = 6.0f;
		float	MaxVoltage = 25.0;

		byte	Ramping = 0;

		uint16_t	SerialTimeout = 0;
		byte	Deadband = 0;

		bool	Enabled : 1;
		bool	DirectionalMode : 1;

	protected:
		float FSpeeds[2] = { -1.0, -1.0 };

	public:
		void SetEnabled( bool AValue )
		{
			if( AValue == Enabled )
				return;

			Enabled = AValue;
			if( Enabled )
				SendStart();

			else
				SendStop();
		}

	public:
		void SetMinVoltage( float AValue )
		{
			AValue = constrain( AValue, 6.0, 30.0 );
			if( MinVoltage == AValue )
				return;

			MinVoltage = AValue;
			SendCommand( 2, ( MinVoltage - 6 ) * 5 );
		}

		void SetMaxVoltage( float AValue )
		{
			AValue = constrain( AValue, 0.0, 25.0 );
			if( MaxVoltage == AValue )
				return;

			MaxVoltage = AValue;
			SendCommand( 3, MinVoltage * 5.12 );
		}

		void SetRamping( byte AValue )
		{
			AValue = constrain( AValue, 0, 80 );
			if( Ramping == AValue )
				return;

			Ramping = AValue;
			SendCommand( 16, Ramping );
		}

		void SetSerialTimeout( uint16_t AValue )
		{
			AValue = constrain( AValue, 0, 12700 );
			if( SerialTimeout == AValue )
				return;

			SerialTimeout = AValue;
			SendSerialTimeout();
		}

		void SetDeadband( byte AValue )
		{
			AValue = constrain( AValue, 0, 127 );
			if( Deadband == AValue )
				return;

			Deadband = AValue;
			SendCommand( 17, Deadband );
		}

	protected:
		void SendSerialTimeout()
		{
			SendCommand( 14, (SerialTimeout + 99) / 100 );
		}

		void SendStart()
		{
			SendStartOne( 0 );
			SendStartOne( 1 );
		}

		void SendStartOne( int AIndex )
		{
			int  AValue = ( FSpeeds[ AIndex ] * 255 + 0.5 ) - 128;
			byte  ACommand;
			if( DirectionalMode )
			{
				if( AIndex == 0 )
					ACommand = 10;

				else
					ACommand = 8;
			}

			else
				ACommand = AIndex << 2; // 0 or 4

			if( AValue < 0 )
			{
				AValue = -AValue;
				ACommand |= 1;
			}

			SendCommand( ACommand, AValue & B01111111 );
		}

		void SendStop()
		{
			// Shut down both motors
			SendCommand( 0, 0 );
			SendCommand( 4, 0 );
		}

		void SetSpeed( int AIndex, float ASpeed )
		{
			ASpeed = constrain( ASpeed, 0, 1 );
			if( ASpeed == FSpeeds[ AIndex ] )
				return;

			FSpeeds[ AIndex ] = ASpeed;
			if( Enabled )
				SendStart();
		}

	protected:
		void SendCommand(byte ACommand, byte AValue)
		{
			Stream &AStream = C_SERIAL.GetStream();

			byte AAddress = Address | B10000000;
			AStream.write( AAddress );
			AStream.write( ACommand );
			AStream.write( AValue );
			AStream.write(( AAddress + ACommand + AValue ) & B01111111);
		}

	public:
		inline void SystemStart() 
		{
			if( MinVoltage != 6.0 )
				SendCommand( 2, ( MinVoltage - 6 ) * 5 );

			if( MaxVoltage != 25.0 )
				SendCommand( 3, MinVoltage * 5.12 );

			if( Ramping )
				SendCommand( 16, Ramping );

			if( SerialTimeout )
				SendSerialTimeout();

			if( Deadband )
				SendCommand( 17, Deadband );
		}

	public:
		inline void MotorsInputPins_o_Receive( int AIndex, void *_Data )
		{
			SetSpeed( AIndex, *(float *)_Data );
		}

	public:
		SabertoothDualMotorPacket() :
			Enabled( true ),
			DirectionalMode( false )
		{
		}

	};
//---------------------------------------------------------------------------
}

#endif
