////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2018 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_MAXIM_MAX7300_h
#define _MITOV_MAXIM_MAX7300_h

#include <Mitov.h>
#include <SPI.h>

#ifdef __TEST_FOR_DEBUG_PRINTS__
#define Serial UNGUARDED DEBUG PRINT!!!
#endif

namespace Mitov
{
//---------------------------------------------------------------------------
	class BasicMicrochip_MAX7300_Channel
	{
	public:
		OpenWire::SourcePin	OutputPin;

	public:
        bool IsOutput : 1;
        bool IsPullUp : 1;

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
		BasicMicrochip_MAX7300_Channel( bool AInitialValue, bool AIsOutput, bool AIsCombinedInOut, bool AIsPullUp ) :
			IsOutput( AIsOutput ),
			IsPullUp( AIsPullUp ),
			FValue( AInitialValue ),
			FCombinedInOut( AIsCombinedInOut ),
			FInValue( false )
		{
		}
	};
//---------------------------------------------------------------------------
	template<uint8_t C_ADDRESS, typename T_I2C, T_I2C &C_I2C, uint8_t C_CHANNELS_COUNT> class Maxim_MAX7300
	{
	public:
		bool	Enabled : 1;

	protected:
		bool	FModifiedMap : 1;
		bool	FModifiedPorts : 1;
		byte	FConfigMap[ 7 ][ 2 ];
		uint32_t    FPorts[ 2 ];
		uint32_t    FReadMap = 0;
		uint32_t    FReadRegistersMap = 0;
		uint32_t    FReadBits;

	public:
		BasicMicrochip_MAX7300_Channel *FChannels[ C_CHANNELS_COUNT ];

	protected:
		void BeginTransmissionAt( byte ARegister )
		{
			C_I2C.beginTransmission( C_ADDRESS );

			C_I2C.write( ARegister );
		}

		void WriteRegister( byte ARegister, byte AValue )
		{
//			Serial.print( "WriteRegister: " ); Serial.print( ARegister ); Serial.print( " " ); Serial.println( AValue );

			BeginTransmissionAt( ARegister );
			C_I2C.write( AValue );
			C_I2C.endTransmission();
		}

		void UpdateEnable()
		{
			if( Enabled )
				WriteRegister( 4, 1 );

			else
				WriteRegister( 4, 0 );

		}

		void UpdateConfigMap()
		{
//			Serial.println( "UpdateConfigMap" );
			bool AInUpdate = false;
			for( int i = 0; i < 7; ++i )
			{
				if( FConfigMap[ i ][ 0 ] != FConfigMap[ i ][ 1 ] )
				{
					if( ! AInUpdate )
					{
//						Serial.print( "BeginTransmissionAt: " ); Serial.println( 9 + i );
						BeginTransmissionAt( 9 + i );
						AInUpdate = true;
					}

//					Serial.println( i );
//					Serial.println( FConfigMap[ i ][ 0 ] );

					C_I2C.write( FConfigMap[ i ][ 0 ] );
					FConfigMap[ i ][ 1 ] = FConfigMap[ i ][ 0 ];
				}

				else if( AInUpdate )
				{
					C_I2C.endTransmission();
					AInUpdate = false;
				}
			}

			if( AInUpdate )
				C_I2C.endTransmission();

			FReadRegistersMap = 0;
			for( int i = 0; i <= 0x58 - 0x44; ++i )
				if( FReadMap & ((uint32_t)0xFF) << i )
				{
					FReadRegistersMap |= ((uint32_t)1 ) << i;
					i += 7;
				}

//			Serial.print( "FReadRegistersMap: " ); Serial.println( FReadRegistersMap );

			FModifiedMap = false;
		}

		void UpdatePorts()
		{
//			Serial.println( "UpdatePorts" );
//			Serial.println( FPorts[ 0 ] );
//			Serial.println( FPorts[ 1 ] );
			for( int i = 0; i < 28; ++i )
			{
				uint32_t AMask = ((uint32_t)1) << i;
//				Serial.println( AMask );
				if( ( FPorts[ 0 ] & AMask ) != ( FPorts[ 1 ] & AMask ) )
				{
//   					Serial.println( i );
//					Serial.println( AMask );
					WriteRegister( 0x44 + i, FPorts[ 0 ] >> i );
					i += 8;
//					for( int j = i / 8; j < 4; j ++ )
//						WriteRegister( 0x44 + j * 8, FPorts[ 0 ] >> j * 8 );

//					break;
				}
			}
			
			FPorts[ 1 ] = FPorts[ 0 ];
			FModifiedPorts = false;
		}

	public:
		inline void SystemStart()
		{
			UpdateEnable();

			for( int i = 0; i < 7; ++i )
				FConfigMap[ i ][ 1 ] = ~FConfigMap[ i ][ 0 ];

			FPorts[ 1 ] = ~FPorts[ 0 ];

			UpdateConfigMap();
			UpdatePorts();

//			UpdateOutput();
		}

		inline void SystemLoopBegin( unsigned long currentMicros )
		{
//			if( ! ReadInputPin.IsConnected() )
				PerformRead();
		}

		inline void SystemLoopUpdateHardware()
		{
			if( FModifiedMap )
//				if( ! ClockInputPin.IsConnected() )
				UpdateConfigMap();

			if( FModifiedPorts )
				UpdatePorts();

		}

	public:
		bool GetBitValue( uint32_t AIndex )
		{
			return( ( FReadBits & ( ((uint32_t)1 ) << AIndex )) != 0 );
		}

		void SetChannelValue( int AIndex, bool AValue )
		{
//			AIndex += 4;

//			Serial.println( "SetChannelValue" );
//			Serial.println( AIndex );
//			Serial.println( AValue );

			FPorts[ 0 ] &= ~( ((uint32_t)0b1 ) << AIndex );
			FPorts[ 0 ] |= ( ((uint32_t)AValue ) & 1 ) << AIndex;
			FModifiedPorts = true;
		}

		void SetChannelMode( int AIndex, int AMode )
		{
//			AIndex += 4;

//			Serial.println( "SetChannelMode" );
//			Serial.println( AIndex );
//			Serial.println( AMode );

			int AConfigIndex = AIndex / 4;
			int AOffset = ( AIndex % 4 ) * 2;

//			Serial.println( AConfigIndex );
//			Serial.println( AOffset );

			FConfigMap[ AConfigIndex ][ 0 ] &= ~( ((uint32_t)0b11) << AOffset );
			FConfigMap[ AConfigIndex ][ 0 ] |= ((uint32_t)AMode) << AOffset;

			if( AMode & 0b10 ) // Check if it is input
				FReadMap |= ((uint32_t)1) << AIndex;

			else
				FReadMap &= ~( ((uint32_t)1) << AIndex );

			FModifiedMap = true;
		}

	public:
		void PerformRead()
		{
			if( ! FReadRegistersMap)
				return;

			FReadBits = 0;
			for( int i = 0; i <= 0x58 - 0x44; ++i )
				if( (((uint32_t)1 ) << i ) & FReadRegistersMap )
				{
					BeginTransmissionAt( 0x44 + i );
					C_I2C.endTransmission();
					C_I2C.requestFrom( C_ADDRESS, uint8_t( 1 ) );
					while (C_I2C.available() < 1 );

					uint32_t ARegister = C_I2C.read();

					FReadBits |= ARegister << i;

//					Serial.print( i ); Serial.print( ": " ); Serial.println( ARegister );

					i += 7;
				}

			for( int i = 0; i < C_CHANNELS_COUNT; ++i )
				FChannels[ i ]->UpdateOutput( GetBitValue( i ));

		}

	public:
		Maxim_MAX7300() :
			Enabled( true ),
			FModifiedMap( false ),
			FModifiedPorts( false )
		{
			for( int i = 0; i < 7; ++i )
				FConfigMap[ i ][ 0 ] = 0b10101010;
		}

	};
//---------------------------------------------------------------------------
	template<typename T_OWNER, T_OWNER &C_OWNER, uint8_t C_INDEX, uint8_t C_ARRAY_INDEX> class Maxim_MAX7300_Channel : public BasicMicrochip_MAX7300_Channel
	{
		typedef BasicMicrochip_MAX7300_Channel inherited;
		

	public:
        void PinDirectionsInit()
        {
            if( inherited::IsOutput )
			{
                C_OWNER.SetChannelMode( C_INDEX, 1 ); //pinMode( FPinNumber, OUTPUT );
				C_OWNER.SetChannelValue( C_INDEX, inherited::FValue );
			}

            else
            {
                if( inherited::IsPullUp )
                    C_OWNER.SetChannelMode( C_INDEX, 3 ); //pinMode( FPinNumber, INPUT_PULLUP );

                else
                    C_OWNER.SetChannelMode( C_INDEX, 2 ); //pinMode( FPinNumber, INPUT );
            }
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
			if( inherited::IsOutput )
				C_OWNER.SetChannelValue( C_INDEX, AValue );
		}

	public:
		inline void InputPin_o_Receive( void * _Data )
		{
			DigitalWrite( *(bool *)_Data );
		}

	public:
		Maxim_MAX7300_Channel( bool AInitialValue, bool AIsOutput, bool AIsCombinedInOut, bool AIsPullUp ) :
			inherited( AInitialValue, AIsOutput, AIsCombinedInOut, AIsPullUp )
		{
			C_OWNER.FChannels[ C_ARRAY_INDEX ] = this;
			PinDirectionsInit();
		}

	};
//---------------------------------------------------------------------------
}

#ifdef __TEST_FOR_DEBUG_PRINTS__
#undef Serial
#endif

#endif
