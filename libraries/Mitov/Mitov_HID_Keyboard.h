////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2018 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_HID_KEYBOARD_h
#define _MITOV_HID_KEYBOARD_h

#include <Mitov.h>

#ifndef VISUINO_TEENSY
  #include <Keyboard.h>
#endif

namespace Mitov
{
	class MitovHIDKeyboard
	{
	public:
		bool	Enabled = true;

	public:
		void UpdateEnabled()
		{
			if( Enabled )
				Keyboard.begin();

			else
				Keyboard.end();

		}

	public:
		template<typename T> void Print( T AValue )
		{
			if( Enabled )
			{
				Keyboard.println( AValue );
/*
				FSending = true;
				SendingOutputPin.SendValue( FSending );
				FStartTime = micros();
				GetStream().println( AValue );
*/
			}
		}

		void PrintChar( char AValue )
		{
			if( Enabled )
			{
				Keyboard.write( AValue );
/*
				FSending = true;
				SendingOutputPin.SendValue( FSending );
				FStartTime = micros();
				GetStream().print( AValue );
*/
			}
		}

		void Write( uint8_t *AData, uint32_t ASize )
		{
			for( uint32_t i = 0; i < ASize; ++ i )
				Keyboard.write( AData[ i ] );

//			GetStream().write( AData, ASize );
		}

	public:
		inline void SystemInit()
		{
			if( Enabled )
				Keyboard.begin();
		}

	};
//---------------------------------------------------------------------------
	template<typename T_OWNER, T_OWNER &C_OWNER> class MitovHIDKeyboardElementKey
	{
	public:
		char	Character = 0;

	protected:
		bool	FPressed = false;;

	public:
		void InputPin_o_Receive( void *_Data )
		{
			if( C_OWNER.Enabled )
			{
				bool AValue = *(bool *)_Data;
				if( AValue != FPressed )
				{
					FPressed = AValue;
					if( AValue )
						Keyboard.press( Character );

					else
						Keyboard.release( Character );

				}
			}
		}
	};
//---------------------------------------------------------------------------
}

#endif
