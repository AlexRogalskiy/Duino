////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2018 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_GPIO_PCF857X_h
#define _MITOV_GPIO_PCF857X_h

#include <Mitov.h>
#include <SPI.h>

namespace Mitov
{
//---------------------------------------------------------------------------
	class BasicMicrochip_PCF85X_Channel
	{
	public:
		OpenWire::SourcePin	OutputPin;

	public:
        bool IsOutput : 1;

	protected:
		bool FValue : 1;
		bool FCombinedInOut : 1;
		bool FInValue : 1;

	public:
		void UpdateOutput( bool AValue )
		{
			if( FInValue == AValue )
				return;

			FInValue = AValue;

			OutputPin.SendValue<bool>( FInValue );
		}

		inline void SendOutput()
		{
			OutputPin.SendValue<bool>( FInValue );
		}

	public:
		BasicMicrochip_PCF85X_Channel( bool AInitialValue, bool AIsOutput, bool AIsCombinedInOut ) :
			IsOutput( AIsOutput ),
			FValue( AInitialValue ),
			FCombinedInOut( AIsCombinedInOut ),
			FInValue( false )
		{
		}
	};
//---------------------------------------------------------------------------
	template<uint8_t C_ADDRESS, typename T_I2C, T_I2C &C_I2C, uint8_t C_CHANNELS_COUNT> class GPIO_PCF8574
	{
	protected:
		uint8_t	FDirectionBits = 0xFF;
		uint8_t	FReadBits = 0;

	public:
		BasicMicrochip_PCF85X_Channel *FChannels[ C_CHANNELS_COUNT ];

	protected:
		void UpdateAll()
		{
			C_I2C.beginTransmission( C_ADDRESS );
			C_I2C.write( FDirectionBits );
			C_I2C.endTransmission();
		}

	public:
		void PerformRead()
		{
			if( ! FDirectionBits )
				return;

			C_I2C.requestFrom( C_ADDRESS, uint8_t( 1 ) );
			FReadBits = C_I2C.read();

			for( int i = 0; i < C_CHANNELS_COUNT; ++i )
				FChannels[ i ]->UpdateOutput( GetBitValue( i ));

		}

	public:
		bool GetBitValue( uint8_t AIndex )
		{
			return( ( FReadBits & ( ((uint8_t)1 ) << AIndex )) != 0 );
		}

	public:
		void SetChannelValue( int AIndex, bool AValue )
		{
			if( AValue )
				FDirectionBits |= ( (uint8_t)1 ) << AIndex;

			else
				FDirectionBits &= ~( ((uint8_t)1 ) << AIndex );
		}

	public:
		inline void SystemStart()
		{
			UpdateAll();

			for( int i = 0; i < C_CHANNELS_COUNT; ++i )
				FChannels[ i ]->SendOutput();
		}

		inline void SystemLoopBegin( unsigned long currentMicros )
		{
//			if( ! ReadInputPin.IsConnected() )
				PerformRead();
		}

		inline void SystemLoopUpdateHardware()
		{
			UpdateAll();
		}

	};
//---------------------------------------------------------------------------
	template<uint8_t C_ADDRESS, typename T_I2C, T_I2C &C_I2C, uint8_t C_CHANNELS_COUNT> class GPIO_PCF8575
	{
	public:
		BasicMicrochip_PCF85X_Channel *FChannels[ C_CHANNELS_COUNT ];

	protected:
		uint16_t	FDirectionBits = 0xFF;
		uint16_t	FReadBits = 0;

	protected:
		void UpdateAll()
		{
			C_I2C.beginTransmission( C_ADDRESS );
			C_I2C.write( FDirectionBits );
			C_I2C.endTransmission();
		}

	public:
		void PerformRead()
		{
			if( ! FDirectionBits )
				return;

			C_I2C.requestFrom( C_ADDRESS, (uint8_t)2 );
			FReadBits = C_I2C.read();

			for( int i = 0; i < C_CHANNELS_COUNT; ++i )
				FChannels[ i ]->UpdateOutput( GetBitValue( i ));

		}

	public:
		bool GetBitValue( uint8_t AIndex )
		{
			return( ( FReadBits & ( ((uint16_t)1 ) << AIndex )) != 0 );
		}

	public:
		void SetChannelValue( int AIndex, bool AValue )
		{
			if( AValue )
				FDirectionBits |= ( (uint16_t)1 ) << AIndex;

			else
				FDirectionBits &= ~( ((uint16_t)1 ) << AIndex );
		}

	public:
		inline void SystemStart()
		{
			UpdateAll();

			for( int i = 0; i < C_CHANNELS_COUNT; ++i )
				FChannels[ i ]->SendOutput();
		}

		inline void SystemLoopBegin( unsigned long currentMicros )
		{
//			if( ! ReadInputPin.IsConnected() )
				PerformRead();
		}

		inline void SystemLoopUpdateHardware()
		{
			UpdateAll();
		}

	};
//---------------------------------------------------------------------------
	template<typename T_OWNER, T_OWNER &C_OWNER, uint8_t C_INDEX, uint8_t C_ARRAY_INDEX> class GPIO_PCF857X_Channel : public BasicMicrochip_PCF85X_Channel
	{
		typedef BasicMicrochip_PCF85X_Channel inherited;
		
	public:
        void PinDirectionsInit()
        {
			if( inherited::IsOutput )
				C_OWNER.SetChannelValue( C_INDEX, inherited::FValue );

			else
				C_OWNER.SetChannelValue( C_INDEX, true );

        }

	public:
		void SetMode( int AMode ) // Placeholder for direct pin control!
		{
//			pinMode( FPinNumber, AMode );
		}

		bool DigitalRead()
		{
//			Serial.println( "TEST1" );
			C_OWNER.PerformRead();
//			Serial.println( FInValue );
			return FInValue;
		}

		void DigitalWrite( bool AValue )
		{
			if( inherited::FValue == AValue )
				return;

			inherited::FValue = AValue;
			C_OWNER.SetChannelValue( C_INDEX, AValue );
		}

	public:
		inline void InputPin_o_Receive( void * _Data )
		{
			DigitalWrite( *(bool *)_Data );
		}

	public:
		GPIO_PCF857X_Channel( bool AInitialValue, bool AIsOutput, bool AIsCombinedInOut ) :
			inherited( AInitialValue, AIsOutput, AIsCombinedInOut )
		{
			C_OWNER.FChannels[ C_ARRAY_INDEX ] = this;
			PinDirectionsInit();
		}

	};
//---------------------------------------------------------------------------
}

#endif