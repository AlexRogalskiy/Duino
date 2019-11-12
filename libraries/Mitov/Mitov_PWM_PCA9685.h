////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2018 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_PWM_PCA9685_h
#define _MITOV_PWM_PCA9685_h

#include <Mitov.h>

#ifdef __TEST_FOR_DEBUG_PRINTS__
#define Serial UNGUARDED DEBUG PRINT!!!
#endif

namespace Mitov
{
	class PCA9685PWMChannel
	{
	public:
		float	InitialValue = 0.0;
		float	OnValue = 0.0;

	};
//---------------------------------------------------------------------------
	class PCA9685ServoChannel
	{
	public:
		float	InitialValue = 0.0;

	};
//---------------------------------------------------------------------------
	enum TArduinoAnalogPCA9685OutputLogicMode { olmZero, olmOne, olmHighImpedance };
//---------------------------------------------------------------------------
	class TArduinoAnalogPCA9685OutputLogic
	{
	public:
		bool	Inverted : 1;
		TArduinoAnalogPCA9685OutputLogicMode	Mode : 2;

	public:
		TArduinoAnalogPCA9685OutputLogic() :
			Inverted( false ),
			Mode( olmZero )
		{
		}
	};
//---------------------------------------------------------------------------
	namespace PCA9685Const
	{
		const uint8_t	PCA9685_MODE1 = 0x0;
		const uint8_t	PCA9685_MODE2 = 0x1;

		const uint8_t	PCA9685_SUBADR1 = 0x2;
		const uint8_t	PCA9685_SUBADR2 = 0x3;
		const uint8_t	PCA9685_SUBADR3 = 0x4;

		const uint8_t	PCA9685_PRESCALE = 0xFE;

		const uint8_t	LED0_ON_L = 0x6;
		const uint8_t	LED0_ON_H = 0x7;
		const uint8_t	LED0_OFF_L = 0x8;
		const uint8_t	LED0_OFF_H = 0x9;

		const uint8_t	ALLLED_ON_L = 0xFA;
		const uint8_t	ALLLED_ON_H = 0xFB;
		const uint8_t	ALLLED_OFF_L = 0xFC;
		const uint8_t	ALLLED_OFF_H = 0xFD;
	}
//---------------------------------------------------------------------------
	template<uint8_t C_INDEX, typename T_OWNER, T_OWNER &C_OWNER> class PCA9685PWMChannelAccess
	{
	public:
		inline void AnalogWrite( float AValue )
		{
			C_OWNER.SetChannelValue( C_INDEX, AValue );
		}

		inline void SetPinValue( float AValue )
		{
			C_OWNER.SetChannelValue( C_INDEX, AValue );
		}
	};
//---------------------------------------------------------------------------
	template<uint8_t C_ADDRESS, typename T_I2C, T_I2C &C_I2C> class PCA9685PWM
	{
	public:
		PCA9685PWMChannel	Channels[ 16 ];

	public:
		TArduinoAnalogPCA9685OutputLogic	OutputLogic;
		float	OscilatorFrequency = 25000000;
		float	Frequency = 1000;
		bool	Enabled : 1;
		bool	OpenDrain : 1;
		bool	Sleep : 1;
		bool	UpdateOnAck : 1;
		bool	ExternalClock : 1;

	public:
		bool	ClockInputPin_o_IsConnected : 1;

	protected:
		uint16_t	FChangedChannels = 0xFFFF;

	public:
		void UpdateFrequency()
		{
			float Afreq = Frequency * 0.9;  // Correct for overshoot in the frequency setting (see issue #11).
			float prescaleval = OscilatorFrequency;
			prescaleval /= 4096;
			prescaleval /= Afreq;
			prescaleval -= 1;
			uint8_t prescale = floor(prescaleval + 0.5);

			uint8_t oldmode = Read8(PCA9685Const::PCA9685_MODE1);
			uint8_t newmode = (oldmode&0x7F) | 0x10; // sleep
			Write8(PCA9685Const::PCA9685_MODE1, newmode); // go to sleep
			Write8(PCA9685Const::PCA9685_PRESCALE, prescale); // set the prescaler
//			Serial.println( prescale );
			Write8(PCA9685Const::PCA9685_MODE1, oldmode);
			delay(1);
			Write8(PCA9685Const::PCA9685_MODE1, oldmode | 0xA0);  //  This sets the MODE1 register to turn on auto increment.
													// This is why the beginTransmission below was not working.
		}

		void UpdateConfig()
		{
			UpdateConfigOnly();
			UpdateAll();
		}

	protected:
		uint8_t Read8(uint8_t addr) 
		{
			C_I2C.beginTransmission( C_ADDRESS );
			C_I2C.write(addr);
			C_I2C.endTransmission();

			C_I2C.requestFrom( C_ADDRESS, (uint8_t)1);
			return C_I2C.read();
		}

		void Write8(uint8_t addr, uint8_t d) 
		{
//  Serial.print( "addr: " ); Serial.print( addr, HEX );
//  Serial.print( " = " ); Serial.println( d, HEX );
			C_I2C.beginTransmission( C_ADDRESS );
			C_I2C.write(addr);
			C_I2C.write(d);
			C_I2C.endTransmission();
		}

	public:
		void SetChannelValue( int AIndex, float AOnValue ) // Used for direct access
		{
			AOnValue = constrain( AOnValue, 0.0, 1.0 );
			if( Channels[ AIndex ].InitialValue == AOnValue )
				return;

			Channels[ AIndex ].InitialValue = AOnValue;
			if( Sleep || ( ! Enabled ))
			{
				FChangedChannels |= ( 1 << AIndex );
				return;
			}

#ifdef _MITOV_PWM_PCA9685_CLOCK_PIN_
			if( ClockInputPin_o_IsConnected )
			{
				FChangedChannels |= ( 1 << AIndex );
				return;
			}
#endif

			UpdateChannelValue( AIndex, AOnValue );
		}

	public:
		inline void ClockInputPin_o_Receive( void *_Data )
		{
			UpdateAll();
		}

		inline void Channel_InputPin_o_Receive( int AIndex, void *_Data )
		{
			float AOnValue = *(float *)_Data;
			SetChannelValue( AIndex, AOnValue );
		}

	protected:
		void UpdateConfigOnly()
		{
			uint8_t	AValue = 0b100000;

			if( ExternalClock )
				AValue |= 0b1000000;

			if( Sleep )
				AValue |= 0b10000;

//			Serial.print( "PCA9685_MODE1: " );
//			Serial.println( AValue, BIN );

			Write8( PCA9685Const::PCA9685_MODE1, AValue );

			AValue = 0;

			if( OutputLogic.Inverted )
				AValue |= 0b10000;

			if( UpdateOnAck )
				AValue |= 0b1000;

			if( ! OpenDrain )
				AValue |= 0b100;

			AValue |= OutputLogic.Mode;

//			Serial.print( "PCA9685_MODE2: " );
//			Serial.println( AValue, BIN );

			Write8( PCA9685Const::PCA9685_MODE2, AValue );
		}

		void UpdateAll()
		{
			if( ! Enabled )
				return;

			if( Sleep )
				return;

			for( int i = 0; i < 16; ++i )
				if( FChangedChannels & ( 1 << i ) )
					UpdateChannelValue( i, Channels[ i ].InitialValue );

		}

		void UpdateChannelValue( int AIndex, float AValue )
		{
			uint16_t	on = Channels[ AIndex ].OnValue * 4095 + 0.5;
			uint16_t	off = AValue * 4095 + 0.5;

//			Serial.print( "AIndex: " );
//			Serial.println( AIndex );
//			Serial.println( on );
//			Serial.println( off );

			C_I2C.beginTransmission( C_ADDRESS );
			C_I2C.write(PCA9685Const::LED0_ON_L + 4 * AIndex );
			C_I2C.write(on);
			C_I2C.write(on>>8);
			C_I2C.write(off);
			C_I2C.write(off>>8);
			C_I2C.endTransmission();

			FChangedChannels &= ~( 1 << AIndex );
		}

	public:
		inline void SystemStart()
		{
			Write8( PCA9685Const::PCA9685_MODE1, 0x0);
			UpdateConfigOnly();
			UpdateFrequency();
			UpdateAll();
		}

	public:
		PCA9685PWM() :
			Enabled( true ),
			OpenDrain( false ),
			Sleep( false ),
			UpdateOnAck( false ),
			ExternalClock( false ),
			ClockInputPin_o_IsConnected( false )
		{
		}

	};
//---------------------------------------------------------------------------
	template<uint8_t C_ADDRESS, typename T_I2C, T_I2C &C_I2C> class PCA9685Servo
	{
	public:
		PCA9685ServoChannel	Channels[ 16 ];

	public:
		TArduinoAnalogPCA9685OutputLogic	OutputLogic;
		float	OscilatorFrequency = 25000000;
		bool	Enabled : 1;
		bool	OpenDrain : 1;
		bool	Sleep : 1;
		bool	UpdateOnAck : 1;
		bool	ExternalClock : 1;

	public:
		bool	ClockInputPin_o_IsConnected : 1;

	protected:
		uint16_t	FChangedChannels = 0xFFFF;

	public:
		void UpdateFrequency()
		{
			float Afreq = 60; // * 0.9;  // Correct for overshoot in the frequency setting (see issue #11).
			float prescaleval = OscilatorFrequency;
			prescaleval /= 4096;
			prescaleval /= Afreq;
			prescaleval -= 1;
			uint8_t prescale = floor(prescaleval + 0.5);

			uint8_t oldmode = Read8( PCA9685Const::PCA9685_MODE1);
			uint8_t newmode = (oldmode&0x7F) | 0x10; // sleep
			Write8( PCA9685Const::PCA9685_MODE1, newmode); // go to sleep
			Write8( PCA9685Const::PCA9685_PRESCALE, prescale); // set the prescaler
//			Serial.println( prescale );
			Write8( PCA9685Const::PCA9685_MODE1, oldmode);
			delay(1);
			Write8( PCA9685Const::PCA9685_MODE1, oldmode | 0xA0);  //  This sets the MODE1 register to turn on auto increment.
													// This is why the beginTransmission below was not working.
		}

		void UpdateConfig()
		{
			UpdateConfigOnly();
			UpdateAll();
		}

	protected:
		uint8_t Read8(uint8_t addr) 
		{
			C_I2C.beginTransmission( C_ADDRESS );
			C_I2C.write(addr);
			C_I2C.endTransmission();

			C_I2C.requestFrom( C_ADDRESS, (uint8_t)1);
			return C_I2C.read();
		}

		void Write8(uint8_t addr, uint8_t d) 
		{
//  Serial.print( "addr: " ); Serial.print( addr, HEX );
//  Serial.print( " = " ); Serial.println( d, HEX );
			C_I2C.beginTransmission( C_ADDRESS );
			C_I2C.write(addr);
			C_I2C.write(d);
			C_I2C.endTransmission();
		}

	public:
		inline void ClockInputPin_o_Receive( void *_Data )
		{
			UpdateAll();
//			ProcessData( false, false );
		}

		void Channel_InputPin_o_Receive( int AIndex, void *_Data )
		{
			float AOnValue = *(float *)_Data;
			AOnValue = constrain( AOnValue, 0.0, 1.0 );
			if( Channels[ AIndex ].InitialValue == AOnValue )
				return;

			Channels[ AIndex ].InitialValue = AOnValue;
			if( Sleep || ( ! Enabled ))
			{
				FChangedChannels |= ( 1 << AIndex );
				return;
			}

#ifdef _MITOV_SERVO_PCA9685_CLOCK_PIN_
			if( ClockInputPin_o_IsConnected )
			{
				FChangedChannels |= ( 1 << AIndex );
				return;
			}
#endif

			UpdateChannelValue( AIndex, AOnValue );
		}

	protected:
		void UpdateConfigOnly()
		{
			uint8_t	AValue = 0b100000;

			if( ExternalClock )
				AValue |= 0b1000000;

			if( Sleep )
				AValue |= 0b10000;

//			Serial.print( "PCA9685_MODE1: " );
//			Serial.println( AValue, BIN );

			Write8( PCA9685Const::PCA9685_MODE1, AValue );

			AValue = 0;

			if( OutputLogic.Inverted )
				AValue |= 0b10000;

			if( UpdateOnAck )
				AValue |= 0b1000;

			if( ! OpenDrain )
				AValue |= 0b100;

			AValue |= OutputLogic.Mode;

//			Serial.print( "PCA9685_MODE2: " );
//			Serial.println( AValue, BIN );

			Write8( PCA9685Const::PCA9685_MODE2, AValue );
		}

		void UpdateAll()
		{
			if( ! Enabled )
				return;

			if( Sleep )
				return;

			for( int i = 0; i < 16; ++i )
				if( FChangedChannels & ( 1 << i ) )
					UpdateChannelValue( i, Channels[ i ].InitialValue );

		}

		void UpdateChannelValue( int AIndex, float AValue )
		{
//			uint16_t	off = AValue * 4095 + 0.5;
			uint16_t	off = AValue * ( 600 - 150 ) + 150.5;

//			Serial.print( "AIndex: " );
//			Serial.println( AIndex );
//			Serial.println( on );
//			Serial.println( off );

			C_I2C.beginTransmission( C_ADDRESS );
			C_I2C.write( PCA9685Const::LED0_OFF_L + 4 * AIndex );
			C_I2C.write(off);
			C_I2C.write(off>>8);
			C_I2C.endTransmission();

			FChangedChannels &= ~( 1 << AIndex );
		}

	public:
		inline void SystemStart()
		{
			Write8( PCA9685Const::PCA9685_MODE1, 0x0);
			UpdateConfigOnly();
			UpdateFrequency();
			UpdateAll();
		}

	public:
		PCA9685Servo() :
			Enabled( true ),
			OpenDrain( false ),
			Sleep( false ),
			UpdateOnAck( false ),
			ExternalClock( false ),
			ClockInputPin_o_IsConnected( false )
		{
		}

	};
//---------------------------------------------------------------------------
}

#ifdef __TEST_FOR_DEBUG_PRINTS__
#undef Serial
#endif

#endif
