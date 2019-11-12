////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2018 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_PS2_CONTROLLER_h
#define _MITOV_PS2_CONTROLLER_h

#include <Mitov.h>
#include <Mitov_PS2_Controller_Basic.h>
#include <PS2X_lib.h>

namespace Mitov
{
	class PS2ControllerDirect
	{
	protected:
		PS2X Fps2x;

	public:
		inline bool	ReadDigital( unsigned int AIndex )
		{
			return Fps2x.Button( AIndex );
		}

		inline float	ReadAnalog( unsigned int AIndex )
		{
			return ((float)Fps2x.Analog( AIndex )) / 255;
		}

	public:
		PS2ControllerDirect( int ADataPinNumber, int ACommandPinNumber, int AAttentionPinNumber, int AClockPinNumber )
		{
			Fps2x.config_gamepad( AClockPinNumber, ACommandPinNumber, AAttentionPinNumber, ADataPinNumber, true, true );
		}

	};
//---------------------------------------------------------------------------
	class PS2ControllerImplementation : public PS2ControllerDirect
	{
		typedef PS2ControllerDirect inherited;

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
			Fps2x.read_gamepad( FSmallMotor, FLargeMotor );
		}

	public:
		using inherited::inherited;

	};
//---------------------------------------------------------------------------
	typedef PS2BasicController<PS2ControllerImplementation> PS2Controller;
//---------------------------------------------------------------------------
	class PS2GuitarImplementation : public PS2ControllerDirect
	{
		typedef PS2ControllerDirect inherited;

	protected:
		inline void ReadController()
		{
			Fps2x.read_gamepad( false, 0 );
		}

	public:
		using inherited::inherited;

	};
//---------------------------------------------------------------------------
	typedef PS2BasicController<PS2GuitarImplementation> PS2Guitar;
//---------------------------------------------------------------------------
}

#endif
