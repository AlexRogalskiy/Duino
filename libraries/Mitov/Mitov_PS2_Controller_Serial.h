////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2018 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_PS2_CONTROLLER_SERIAL_h
#define _MITOV_PS2_CONTROLLER_SERIAL_h

#include <Mitov.h>
#include <Mitov_PS2_Controller_Basic.h>

namespace Mitov
{
	template<typename T_SERIAL, T_SERIAL &C_SERIAL> class PS2ControllerSerialBasic
	{
	protected:
		void WriteByte( byte AValue )
		{
			while( C_SERIAL.GetStream().available() > 0 )
				Serial.read();

			C_SERIAL.GetStream().write( AValue );
			C_SERIAL.GetStream().flush(); 	//wait for all data transmitted
		}

		byte ReadByte()
		{
			long waitcount=0; 

			while(true)
			{
				if( C_SERIAL.GetStream().available() > 0 )
				{
					byte rec_data = C_SERIAL.GetStream().read();
//					SERIAL_ERR=false; 
					return(rec_data); 
				}
				waitcount++; 
				if(waitcount>50000)
				{
//					SERIAL_ERR=true; 
					return (0xFF); 
				}
			
			}	
		}

	public:
		bool	ReadDigital( unsigned int AIndex )
		{
			WriteByte( AIndex );
			return ( ReadByte() == 0 );
		}

		float	ReadAnalog( unsigned int AIndex )
		{
			WriteByte( AIndex );
			return ((float)ReadByte()) / 255;
//			return ((float)Fps2x.Analog( AIndex )) / 255;
		}

	};
//---------------------------------------------------------------------------
	template<typename T_SERIAL, T_SERIAL &C_SERIAL> class PS2ControllerSerialImplementation : public PS2ControllerSerialBasic<T_SERIAL, C_SERIAL>
	{
		typedef PS2ControllerSerialBasic<T_SERIAL, C_SERIAL> inherited;

	protected:
		bool	FSmallMotor = false;
		int8_t	FLargeMotor = 0;

	public:
		inline void SmallVibrateMotorInputPin_o_Receive( void *_Data )
		{
			FSmallMotor = *(bool *)_Data;
		}

		inline void LargeVibrateMotorInputPin_o_Receive( void *_Data )
		{
			FLargeMotor = (int8_t)( constrain( *(float *)_Data, 0, 1 ) * 255 + 0.5 );
		}

	protected:
		inline void ReadController()
		{
			inherited::WriteByte( 29 ); // Small Motor
			if( FSmallMotor )
				inherited::WriteByte( 1 );

			else
				inherited::WriteByte( 0 );

			inherited::WriteByte( 30 ); // Large Motor
			inherited::WriteByte( FLargeMotor );
		}

	public:
		using inherited::inherited;

	};
//---------------------------------------------------------------------------
	template<typename T_SERIAL, T_SERIAL &C_SERIAL> class PS2ControllerSerial : public PS2BasicController<PS2ControllerSerialImplementation<T_SERIAL, C_SERIAL>> {};
//---------------------------------------------------------------------------
/*
	class PS2GuitarSerial : public PS2BasicControllerSerial
	{
		typedef PS2BasicControllerSerial inherited;

		virtual void ReadController()
		{
			Fps2x.read_gamepad( false, 0 );
		}

	public:
		PS2GuitarSerial( int ADataPinNumber, int ACommandPinNumber, int AAttentionPinNumber, int AClockPinNumber ) :
			inherited( ADataPinNumber, ACommandPinNumber, AAttentionPinNumber, AClockPinNumber )
		{
		}
	};
*/
//---------------------------------------------------------------------------
}

#endif
