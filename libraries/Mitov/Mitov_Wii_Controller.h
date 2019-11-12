////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2018 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_WII_CONTROLLER_h
#define _MITOV_WII_CONTROLLER_h

#include <Mitov.h>

// http://wiibrew.org/wiki/Wiimote/Extension_Controllers/Nunchuck

namespace Mitov
{
//---------------------------------------------------------------------------
	template<uint8_t C_ADDRESS, typename T_I2C, T_I2C &C_I2C> class WiiNunchuck
	{
	public:
#ifdef _MITOV_WII_NUNCHUCK_ANGLE_OUTPUT_PINS_
		OpenWire::TypedSourcePin<float>	AngleOutputPins[ 3 ];
#endif

#ifdef _MITOV_WII_NUNCHUCK_STICK_OUTPUT_PINS_
		OpenWire::TypedSourcePin<float>	StickOutputPins[ 2 ];
#endif

#ifdef _MITOV_WII_NUNCHUCK_BUTTONS_OUTPUT_PINS_
		OpenWire::TypedSourcePin<bool>	ButtonOutputPins[ 2 ];
#endif

	public:
		uint32_t	ReadDelay = 100;
		bool Enabled : 1;

	public:
		bool	ClockInputPin_o_IsConnected : 1;

	protected:
		bool	FStarted : 1;
		uint8_t	FClockedCount : 6;
		unsigned long	FLastTime = 0;

	public:
		void ClockInputPin_o_Receive( void *_Data )
		{
			++ FClockedCount;
			StartRead();
		}

	protected:
		void ReadData( bool AChangeOnly )
		{
			if( ! Enabled )
				return;

			ReadController( AChangeOnly );
		}

		void StartRead()
		{
			C_I2C.beginTransmission(C_ADDRESS);
			C_I2C.write(0x00);
			C_I2C.endTransmission();

			delayMicroseconds( 10 );

			C_I2C.beginTransmission(C_ADDRESS);
			C_I2C.write(0x00);
			C_I2C.endTransmission();
			FLastTime = micros();
		}

		void ReadController( bool AChangeOnly )
		{
			uint8_t AData[6];
			C_I2C.requestFrom(C_ADDRESS, (byte)6);

			for( int i = 0; i < 6; ++i )
				AData[ i ] = C_I2C.read();

#ifdef _MITOV_WII_NUNCHUCK_STICK_OUTPUT_PINS_
			float AStickX = (float)AData[0] / 255;
			float AStickY = (float)AData[1] / 255;
			StickOutputPins[ 0 ].SetValue( AStickX, AChangeOnly );
			StickOutputPins[ 1 ].SetValue( AStickY, AChangeOnly );
#endif // _MITOV_WII_NUNCHUCK_STICK_OUTPUT_PINS_

#ifdef _MITOV_WII_NUNCHUCK_ANGLE_OUTPUT_PINS_
			float AAngleX = ((float)(((uint16_t)AData[2] ) << 2) + ((AData[5] >> 2) & 0x03)) / 1023;
			float AAngleY = ((float)(((uint16_t)AData[3] ) << 2) + ((AData[5] >> 4) & 0x03)) / 1023;
			float AAngleZ = ((float)(((uint16_t)AData[4] ) << 2) + ((AData[5] >> 6) & 0x03)) / 1023;

			AngleOutputPins[ 0 ].SetValue( AAngleX, AChangeOnly );
			AngleOutputPins[ 1 ].SetValue( AAngleY, AChangeOnly );
			AngleOutputPins[ 2 ].SetValue( AAngleZ, AChangeOnly );
#endif // _MITOV_WII_NUNCHUCK_ANGLE_OUTPUT_PINS_

#ifdef _MITOV_WII_NUNCHUCK_BUTTONS_OUTPUT_PINS_
			bool AButtonValue = !( AData[5] & 0b10 );
			ButtonOutputPins[ 0 ].SetValue( AButtonValue, AChangeOnly );

			AButtonValue = !( AData[5] & 1 );
			ButtonOutputPins[ 1 ].SetValue( AButtonValue, AChangeOnly );
#endif // _MITOV_WII_NUNCHUCK_BUTTONS_OUTPUT_PINS_

			if( ( ! ClockInputPin_o_IsConnected ) || FClockedCount )
				StartRead();
		}

	public:
		inline void SystemLoopBegin( unsigned long currentMicros )
		{
			if( ! FClockedCount )
				if( ! ClockInputPin_o_IsConnected )
					++ FClockedCount;

			if( FClockedCount )
				if( currentMicros - FLastTime > ReadDelay )
				{
					-- FClockedCount;
					ReadData( FStarted );
					FStarted = true;
				}

		}

		inline void SystemStart()
		{
			// Init the controller
			C_I2C.beginTransmission(C_ADDRESS);
			C_I2C.write(0xF0);
			C_I2C.write(0x55);
			C_I2C.endTransmission();

			C_I2C.beginTransmission(C_ADDRESS);
			C_I2C.write(0xFB);
			C_I2C.write(0x00);
			C_I2C.endTransmission();

			StartRead();
		}

	public:
		WiiNunchuck() :
			Enabled( true ),
			ClockInputPin_o_IsConnected( false ),
			FStarted( false ),
			FClockedCount( 0 )
		{
		}

	};
//---------------------------------------------------------------------------
}

#endif
