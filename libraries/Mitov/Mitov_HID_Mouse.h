////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2018 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_HID_MOUSE_h
#define _MITOV_HID_MOUSE_h

#include <Mitov.h>

#ifndef VISUINO_TEENSY
  #include <Mouse.h>
#endif

namespace Mitov
{
	class MitovHIDMouse;
//---------------------------------------------------------------------------
	class MitovHIDMouse
	{
	public:
		bool	Enabled : 1;

	protected:
		bool	FButton1 : 1;
		bool	FButton2 : 1;
		bool	FButton3 : 1;

	public:
		void UpdateEnabled()
		{
			if( Enabled )
				Mouse.begin();

			else
				Mouse.end();

		}

	public:
		void InputPins_o_Receive( int AIndex, void *_Data )
		{
			if( ! Enabled )
				return;

			bool AValue = *(bool *)_Data;
			switch( AIndex )
			{
				case 0 : DoButtonReceive1( AValue ); break;
				case 1 : DoButtonReceive2( AValue ); break;
				case 2 : DoButtonReceive3( AValue ); break;
			}
		}

	protected:
		void DoButtonReceive1( bool AValue )
		{
			if( FButton1 == AValue )
				return;

			FButton1 = AValue;
			if( AValue )
				Mouse.press( MOUSE_LEFT );

			else
				Mouse.release( MOUSE_LEFT );
		}

		void DoButtonReceive2( bool AValue )
		{
			if( FButton2 == AValue )
				return;

			FButton2 = AValue;
			if( AValue )
				Mouse.press( MOUSE_MIDDLE );

			else
				Mouse.release( MOUSE_MIDDLE );
		}

		void DoButtonReceive3( bool AValue )
		{
			if( FButton3 == AValue )
				return;

			FButton3 = AValue;
			if( AValue )
				Mouse.press( MOUSE_RIGHT );

			else
				Mouse.release( MOUSE_RIGHT );
		}

	public:
		inline void SystemInit()
		{
			if( Enabled )
				Mouse.begin();
		}

	public:
		MitovHIDMouse() :
			Enabled( true ),
			FButton1( false ),
			FButton2( false ),
			FButton3( false )
		{
		}

	};
//---------------------------------------------------------------------------
	template<typename T_OWNER, T_OWNER &C_OWNER> class MitovHIDMouseElementMove
	{
	public:
		int16_t	X = 0;
		int16_t	Y = 0;
		int16_t	Wheel = 0;

	protected:
		bool	FPressed = false;

	public:
		void ClockInputPin_o_Receive( void *_Data )
		{
			if( C_OWNER.Enabled )
				Mouse.move( X, Y, Wheel );
		}
	};
//---------------------------------------------------------------------------
	template<typename T_OWNER, T_OWNER &C_OWNER> class MitovHIDMousePosition
	{
	protected:
		int32_t	FCurrentX = 0;
		int32_t	FCurrentY = 0;

	public:
		void XInputPin_o_Receive( void *_Data )
		{
			int32_t	AValue = *(int32_t *)_Data;
			if( FCurrentX == AValue )
				return;

			if( C_OWNER.Enabled )
			{
				Mouse.move( AValue - FCurrentX, 0, 0 );
				FCurrentX = AValue;
			}
		}

		void YInputPin_o_Receive( void *_Data )
		{
			int32_t	AValue = *(int32_t *)_Data;
			if( FCurrentY == AValue )
				return;

			if( C_OWNER.Enabled )
			{
				Mouse.move( 0, AValue - FCurrentY, 0 );
				FCurrentY = AValue;
			}
		}

		void ZeroInputPin_o_Receive( void *_Data )
		{
			FCurrentX = 0;
			FCurrentY = 0;
		}

	};
//---------------------------------------------------------------------------
	template<typename T_OWNER, T_OWNER &C_OWNER> class MitovHIDMouseWheelPosition
	{
	protected:
		int32_t	FCurrent = 0;

	public:
		void InputPin_o_Receive( void *_Data )
		{
			int32_t	AValue = *(int32_t *)_Data;
			if( FCurrent == AValue )
				return;

			if( C_OWNER.Enabled )
			{
				Mouse.move( 0, 0, AValue - FCurrent );
				FCurrent = AValue;
			}
		}

		inline void ZeroInputPin_o_Receive( void *_Data )
		{
			FCurrent = 0;
		}
	};
//---------------------------------------------------------------------------
}

#endif
