////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2018 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_HMC5883L_h
#define _MITOV_HMC5883L_h

#include <Mitov.h>
#include <Wire.h> //I2C Arduino Library

namespace Mitov
{

//	const byte HMC5883L_Address = 0x1E;
	const byte HMC5883L_StatusRegister = 9;
	const byte HMC5883L_ModeRegister = 2;
	const byte HMC5883_REGISTER_MAG_CRA_REG_M = 0x00;
	const byte HMC5883_REGISTER_MAG_CRB_REG_M = 0x01;
	const byte HMC5883_REGISTER_MAG_OUT_X_H_M = 0x03;
//---------------------------------------------------------------------------
	enum HMC5883L_AveragedSamples { cas1, cas2, cas4, cas8 };
	enum HMC5883L_Bias { cbNone, cbPositive, cbNegative };
	enum HMC5883L_Gain { cg1370, cg1090, cg820, cg660, cg440, cg390, cg330, cg230 };
//---------------------------------------------------------------------------
	template<uint8_t C_ADDRESS, typename T_I2C, T_I2C &C_I2C> class HMC5883L_Compass
	{
	public:
		OpenWire::TypedSourcePin<float>	XOutputPin;
		OpenWire::TypedSourcePin<float>	YOutputPin;
		OpenWire::TypedSourcePin<float>	ZOutputPin;

	public:
		bool						Enabled : 1;
		HMC5883L_AveragedSamples	AveragedSamples : 2;
		HMC5883L_Bias				Bias : 2;
		HMC5883L_Gain				Gain : 3;
		bool						Idle : 1;

	protected:
		bool	FClocked : 1;
		bool	FChangeOnly : 1;

	protected:
		float	FScale;

	public:
		void UpdateIdle()
		{
			if( Idle )
				WriteTo( C_ADDRESS, 0b11 ); // Idle

			else
				WriteTo( C_ADDRESS, 0b00 ); // Single measurement mode				
//				WriteTo( C_ADDRESS, 0b01 ); // Single measurement mode				
		}

	protected:
		bool DataAvailable()
		{
			StartReadFrom( HMC5883L_StatusRegister, 1 );

			uint8_t	AStatus = C_I2C.read();
			return( AStatus & 1 );
		}

		void ReadCompass()
		{
			if( ! Enabled )
				return;

			if( ! DataAvailable() )
				return;
			FClocked = false;

			StartReadFrom( HMC5883_REGISTER_MAG_OUT_X_H_M, 6 );

			float AValues[ 3 ];

			{ // Free Stack Block
				uint8_t	ABytes[ 2 ];
				for( int i = 0; i < 3; ++i )
				{
					ABytes[ 0 ] = C_I2C.read();
					ABytes[ 1 ] = C_I2C.read();
					AValues[ i ] = ( uint16_t( ABytes[ 0 ] ) << 8 ) | ABytes[ 1 ];
				}
			}

			XOutputPin.SetValue( AValues[ 0 ] * FScale, FChangeOnly );
			YOutputPin.SetValue( AValues[ 2 ] * FScale, FChangeOnly ); // Y is actually last!
			ZOutputPin.SetValue( AValues[ 1 ] * FScale, FChangeOnly ); // Z is second

			FChangeOnly = true;
		}

	protected:
		void WriteTo( byte ARegister, byte AData )
		{
//			Serial.print( "WriteTo :" ); Serial.print( ARegister ); Serial.print( " " ); Serial.println( AData );
			C_I2C.beginTransmission( C_ADDRESS );
			C_I2C.write( ARegister );
			C_I2C.write( AData );
			C_I2C.endTransmission();
		}

		void StartReadFrom( byte ARegister, byte ALength )
		{
//			Serial.print( "StartReadFrom :" ); Serial.print( ARegister ); Serial.print( " " ); Serial.println( ALength );
			C_I2C.beginTransmission( C_ADDRESS );
			C_I2C.write( ARegister );
			C_I2C.endTransmission();

			C_I2C.requestFrom( C_ADDRESS, ALength );
			while (C_I2C.available() < ALength);
		}

	public:
		void ClockInputPin_o_Receive( void *_Data )
		{
			FClocked = true;
			ReadCompass();
		}

	public:
		inline void SystemStart()
		{
			uint8_t	AValue;

			WriteTo( HMC5883L_ModeRegister, 0 );

			AValue = ( Bias & 0b11 ) + (( AveragedSamples & 0b11 ) << 5 );
			WriteTo( HMC5883_REGISTER_MAG_CRA_REG_M, AValue );

			AValue = ( Gain & 0b111 ) << 5;
			WriteTo( HMC5883_REGISTER_MAG_CRB_REG_M, AValue );
//			Serial.println( AValue );
//			delay( 10000 );

			switch( Gain )
			{
				case cg1370: FScale = 0.73; break;
				case cg1090: FScale = 0.92; break;
				case cg820: FScale = 1.22; break;
				case cg660: FScale = 1.52; break;
				case cg440: FScale = 2.27; break;
				case cg390: FScale = 2.56; break;
				case cg330: FScale = 3.03; break;
				case cg230: FScale = 4.35; break;
			}

			UpdateIdle();
		}

		inline void SystemLoopBegin( unsigned long currentMicros )
		{
			if( Enabled && FClocked )
				ReadCompass();
		}

	public:
		HMC5883L_Compass() :
			Enabled( true ),
			AveragedSamples( cas1 ),
			Bias( cbNone ),
			Gain( cg1090 ),
			Idle( false ),
			FClocked( false ),
			FChangeOnly( false )
		{
		}
	};
}

#endif
