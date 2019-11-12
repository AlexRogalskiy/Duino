////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2018 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_MICROCHIP_MCP23017_h
#define _MITOV_MICROCHIP_MCP23017_h

#include <Mitov.h>
#include <SPI.h>

namespace Mitov
{
//---------------------------------------------------------------------------
	class BasicMicrochip_MCP23017Channel
	{
	public:
		OpenWire::SourcePin	OutputPin;

	public:
        bool IsOutput : 1;
        bool IsPullUp : 1;
        bool IsInverted : 1;

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
		BasicMicrochip_MCP23017Channel( bool AInitialValue, bool AIsOutput, bool AIsCombinedInOut, bool AIsPullUp, bool AIsInverted ) :
			IsOutput( AIsOutput ),
			IsPullUp( AIsPullUp ),
			IsInverted( AIsInverted ),
			FValue( AInitialValue ),
			FCombinedInOut( AIsCombinedInOut ),
			FInValue( false )
		{
		}
	};
//---------------------------------------------------------------------------
	namespace Microchip_MCP23017_TRegisterIndex
	{
		enum TRegisterIndex { riDirection, riPullUp, riInvert, riValue };
	}
//---------------------------------------------------------------------------
	template<uint8_t C_ADDRESS, typename T_I2C, T_I2C &C_I2C, uint8_t C_CHANNELS_COUNT> class Microchip_MCP23017
	{
	protected:
		uint16_t	FRegValues[ 4 ][ 2 ];
		uint16_t	FReadRegisterMap = 0;
		uint16_t	FReadBits = 0;

	public:
		BasicMicrochip_MCP23017Channel *FChannels[ C_CHANNELS_COUNT ];

	protected:
		void BeginTransmissionAt( byte ARegister )
		{
			C_I2C.beginTransmission( C_ADDRESS );

			C_I2C.write( ARegister );
		}

		void WriteAt( byte ARegister, uint16_t AData )
		{
//			Serial.print( "WriteAt: " ); Serial.print( ARegister ); Serial.print( " " ); Serial.println( AData ); 
			BeginTransmissionAt( ARegister );

			C_I2C.write( (byte *)&AData, 2 );

			C_I2C.endTransmission();
		}

		void StartReadFrom( byte ARegister, uint8_t ALength )
		{
			BeginTransmissionAt( ARegister );
			C_I2C.endTransmission();
			C_I2C.requestFrom( C_ADDRESS, ALength );
			while( C_I2C.available() < ALength );
		}

		void UpdateAll()
		{
			static const byte CPorts[] = 
			{ 
				0x00,	// IODIR register
				0x02,	// IPOL register
				0x0C,	// GPPU register
				0x14	// OLAT register
			};

			for( int i = 0; i < 4; ++i )
				if( FRegValues[ i ][ 0 ] != FRegValues[ i ][ 1 ] )
				{
					WriteAt( CPorts[ i ], FRegValues[ i ][ 0 ] );
					FRegValues[ i ][ 1 ] = FRegValues[ i ][ 0 ];
				}

/*
			// Read the port 
			BeginTransmissionAt( 0x12 ); // GPIO register
			C_I2C.endTransmission();

			C_I2C.requestFrom( 0b0100000 | ( Address & 0b111 ), 2 );
			uint16_t	AValue;
			((byte *)&AValue )[ 0 ] = C_I2C.read();
			((byte *)&AValue )[ 1 ] = C_I2C.read();

			for( int i = 0; i < C_CHANNELS_COUNT; ++i )
				FChannels[ i ]->UpdateOutput( AValue & ( 1 << i ) != 0 );
*/
		}

	public:
		void PerformRead()
		{
			if( ! FReadRegisterMap )
				return;

//			Serial.println( FReadRegisterMap );

			if( FReadRegisterMap & 0x0F )
			{
				if( FReadRegisterMap & 0xF0 )
				{
					StartReadFrom( 0x12, 2 ); // GPIO register
					((byte *)&FReadBits )[ 0 ] = C_I2C.read();
					((byte *)&FReadBits )[ 1 ] = C_I2C.read();
				}

				else
				{
					StartReadFrom( 0x12, 1 ); // GPIO register
					((byte *)&FReadBits )[ 0 ] = C_I2C.read();
					((byte *)&FReadBits )[ 1 ] = 0;

//					Serial.print( "FReadBits: " ); Serial.println( FReadBits );
				}
			}
			else
			{
				// Read the port 
				StartReadFrom( 0x13, 1 ); // GPIO register
				((byte *)&FReadBits )[ 0 ] = 0;
				((byte *)&FReadBits )[ 1 ] = C_I2C.read();
			}

			for( int i = 0; i < C_CHANNELS_COUNT; ++i )
				FChannels[ i ]->UpdateOutput( GetBitValue( i ));

		}

	public:
		bool GetBitValue( uint32_t AIndex )
		{
			return( ( FReadBits & ( ((uint32_t)1 ) << AIndex )) != 0 );
		}

	public:
		void SetChannelValue( int AIndex, bool AValue )
		{
			SetChannelRegisterValue( AIndex, Microchip_MCP23017_TRegisterIndex::riValue, AValue );
		}

	protected:
//		void DoReadInputReceive( void * _Data )
//		{
//			PerformRead();
//		}

	public:
		inline void SystemLoopBegin( unsigned long currentMicros )
		{
//			if( ! ReadInputPin.IsConnected() )
				PerformRead();
		}

		inline void SystemStart()
		{
			for( int i = 0; i < 4; ++i )
				FRegValues[ i ][ 1 ] = ~FRegValues[ i ][ 0 ];

			UpdateAll();

			for( int i = 0; i < C_CHANNELS_COUNT; ++i )
				FChannels[ i ]->SendOutput();
		}

		inline void SystemLoopUpdateHardware()
		{
			UpdateAll();
		}

	public:
		void SetChannelRegisterValue( int AIndex, int ARegisterIndex, bool AValue )
		{
			FRegValues[ ARegisterIndex ][ 0 ] &= ~( ((uint16_t) 0b1 ) << AIndex );
			FRegValues[ ARegisterIndex ][ 0 ] |= ( (uint16_t)( AValue & 1 ) ) << AIndex;

			if( ARegisterIndex == Microchip_MCP23017_TRegisterIndex::riDirection )
			{
				if( AValue )
					FReadRegisterMap |= ((uint16_t) 1) << AIndex;

				else
					FReadRegisterMap &= ~( ((uint16_t) 1) << AIndex );
			}
		}

	public:
		Microchip_MCP23017()
		{
			for( int i = 0; i < 4; ++i )
				FRegValues[ i ][ 0 ] = 0;
		}

	};
//---------------------------------------------------------------------------
	template<typename T_OWNER, T_OWNER &C_OWNER, uint8_t C_INDEX, uint8_t C_ARRAY_INDEX> class Microchip_MCP23017_Channel : public BasicMicrochip_MCP23017Channel
	{
		typedef BasicMicrochip_MCP23017Channel inherited;
		
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
        void PinDirectionsInit()
        {
			C_OWNER.SetChannelRegisterValue( C_INDEX, Microchip_MCP23017_TRegisterIndex::riDirection, ! IsOutput );
			C_OWNER.SetChannelRegisterValue( C_INDEX, Microchip_MCP23017_TRegisterIndex::riPullUp, IsPullUp );
			C_OWNER.SetChannelRegisterValue( C_INDEX, Microchip_MCP23017_TRegisterIndex::riInvert, IsInverted );
        }

	public:
		Microchip_MCP23017_Channel( bool AInitialValue, bool AIsOutput, bool AIsCombinedInOut, bool AIsPullUp, bool AIsInverted ) :
			inherited( AInitialValue, AIsOutput, AIsCombinedInOut, AIsPullUp, AIsInverted )
		{
			C_OWNER.FChannels[ C_ARRAY_INDEX ] = this;
			PinDirectionsInit();
		}

	};
//---------------------------------------------------------------------------
}

#endif
