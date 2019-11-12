////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2018 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_MAXIM_MOTOR_DRIVER_SHIELD_h
#define _MITOV_MAXIM_MOTOR_DRIVER_SHIELD_h

#include <Mitov.h>

namespace Mitov
{
//---------------------------------------------------------------------------
	enum TArduinoMaximMotorCurrentRegulationMode { crmRipple25, crmFastDecay, crmSlowDecay };
//---------------------------------------------------------------------------
	template<typename T_OWNER> class Maxim_MotorDriverBasicChannel
	{
	public:
		OpenWire::SourcePin	FaultOutputPin;

	public:
		float MaxCurrent = 0.0f;
		TArduinoMaximMotorCurrentRegulationMode	CurrentRegulationMode : 2;
		bool Enabled : 1;
		bool FFailure : 1;

	public:
        void SetEnabled( bool AValue )
		{
            if( Enabled == AValue )
                return;

            Enabled = AValue;
			FOwner.FChanged |= 0b00010000 << FIndex; 
			FOwner.FModified = true;
		}

        void SetMaxCurrent( float AValue )
        {
			AValue = constrain( AValue, 0, 2.6 );
            if( MaxCurrent == AValue )
                return;

            MaxCurrent = AValue;
			FOwner.FChanged |= 0b1 << FIndex; 
			FOwner.FModified = true;
        }

        void SetCurrentRegulationMode( TArduinoMaximMotorCurrentRegulationMode AValue )
        {
            if( CurrentRegulationMode == AValue )
                return;

            CurrentRegulationMode = AValue;
			FOwner.FChanged |= 0b00010000 << FIndex; 
			FOwner.FModified = true;
        }

	protected:
		T_OWNER &FOwner;
		int FIndex;

		float FSpeed;
		float FSenseResistor;

	public:
		void SpeedInputPin_o_Receive( void * _Data )
		{
			float AValue = constrain( *(float *)_Data, 0, 1.0 );
//			Serial.println( AValue );
			if( FSpeed == AValue )
				return;

			if(( AValue > 0.5 && FSpeed <= 0.5 ) || ( AValue <= 0.5 && FSpeed > 0.5 ))
				FOwner.FChanged |= 0b00010000 << FIndex; 

			FSpeed = AValue;

			FOwner.FModified = true;

//			FOwner.SetChannelValue( FIndex, AValue );
		}

	public:
		byte	GetRegisterPins()
		{
			byte	Result;
			if( Enabled )
				Result = 0;

			else
				Result = 1;

			if( FSpeed > 0.5 )
				Result |= 0b10;

			switch( CurrentRegulationMode )
			{
				case crmRipple25:
					Result |= 0b1000;

				case crmFastDecay:
					Result |= 0b1100;

				case crmSlowDecay:
					Result |= 0b0100;

			}

			return Result;
		}

		byte GetPotentiometer()
		{
//			byte ATest1 = Reference * 255 + 0.5;
//			Serial.println( ATest1 );
			return ( MaxCurrent * FSenseResistor / 2.6 ) * 255 + 0.5;
//			return Reference * 255 + 0.5;
//			return abs( FSpeed - 0.5 ) * 2 * 255 + 0.5;
		}

		virtual void UpdateSpeed() = 0;

		void UpdateFailure( bool AValue )
		{
			if( FFailure == AValue )
				return;

			FFailure = AValue;
			FaultOutputPin.Notify( &AValue );
		}

		void InitFailure()
		{
			FaultOutputPin.SendValue( FFailure );
		}

	public:
		Maxim_MotorDriverBasicChannel( T_OWNER &AOwner, int AIndex, float AInitialSpeed, float ASenseResistor ) :
			FOwner( AOwner ),
			FIndex( AIndex ),
			Enabled( true ),
			FSpeed( AInitialSpeed ),
			FSenseResistor( ASenseResistor * 10 ),
			CurrentRegulationMode( crmRipple25 ),
			FFailure( false )
		{
			FOwner.FChannels.push_back( this );
		}

	};
//---------------------------------------------------------------------------
	template<typename T_I2C, T_I2C &C_I2C, typename T_INSTANCE> class Maxim_MotorDriverBasicShield
	{
	public:
		Mitov::SimpleList<Maxim_MotorDriverBasicChannel<T_INSTANCE> *>	FChannels;
		byte FChanged = 0xFF;
		bool FModified = true;

	protected:
		byte F_MAX7300;
		byte F_MAX5387_1;
		byte F_MAX5387_2;

	protected:
		void DoPotentiometerTransmission( byte Address, byte ARegister, byte AData )
		{
			byte AAddress = 0b0101000 | (( Address & 0b111 ) << 1 );

//			Serial.print( AAddress );
//			Serial.print( " " );
//			Serial.print( ARegister );
//			Serial.print( " " );
//			Serial.println( AData );

			C_I2C.beginTransmission( AAddress );
			C_I2C.write( ARegister );
			C_I2C.write( AData );
			C_I2C.endTransmission();
		}

		void DoGPIOTransmissionAt( byte ARegister, byte AData )
		{
			byte AAddress = 0b01000000 | ( F_MAX7300 & 0b1111 );

			C_I2C.beginTransmission( AAddress );
			C_I2C.write( ARegister );
			C_I2C.write( AData );
			C_I2C.endTransmission();
		}

		byte DoGPIOReadAt( byte ARegister )
		{
			byte AAddress = 0b01000000 | ( F_MAX7300 & 0b1111 );

			C_I2C.beginTransmission( AAddress );
			C_I2C.write( ARegister );
			C_I2C.endTransmission();
			C_I2C.requestFrom( AAddress, (byte)1 );
			return C_I2C.read();
		}

	protected:
		void UpdateSettings();
		void UpdateFailures();

	public:
		void SystemStart();

		inline void SystemLoopUpdateHardware()
		{
			if( FModified )
				UpdateSettings();

		}

	public:
		Maxim_MotorDriverBasicShield( byte A_MAX7300, byte A_MAX5387_1, byte A_MAX5387_2 ) :
			F_MAX7300( A_MAX7300 ),
			F_MAX5387_1( A_MAX5387_1 ),
			F_MAX5387_2( A_MAX5387_2 )
		{
		}

	};
//---------------------------------------------------------------------------
	template<typename T_I2C, T_I2C &C_I2C> class Maxim_MotorDriverShieldComponent : public Maxim_MotorDriverBasicShield<T_I2C, C_I2C, Maxim_MotorDriverShieldComponent<T_I2C, C_I2C>>
	{
		typedef Maxim_MotorDriverBasicShield<T_I2C, C_I2C, Maxim_MotorDriverShieldComponent<T_I2C, C_I2C>> inherited;

	protected:
		bool	FFaultReceived = false;

	public:
		void FaultInputPin_o_Receive( void * _Data )
		{
			if( *(bool *)_Data )
				inherited::UpdateFailures();
		}

	public:
		using inherited::inherited;

	};
//---------------------------------------------------------------------------
	class Maxim_MotorDriverShield : public Maxim_MotorDriverBasicShield<TwoWire, Wire, Maxim_MotorDriverShield>
	{
		typedef Maxim_MotorDriverBasicShield<TwoWire, Wire, Maxim_MotorDriverShield> inherited;

	protected:
		int FFailureChangePin;

	public:
		inline void SystemLoopBegin( unsigned long currentMicros )
		{
			if( digitalRead( FFailureChangePin ) == HIGH )
				inherited::UpdateFailures();
		}

	public:
		Maxim_MotorDriverShield( byte A_MAX7300, byte A_MAX5387_1, byte A_MAX5387_2, int AFailureChangePin ) :
			inherited( A_MAX7300, A_MAX5387_1, A_MAX5387_2 ),
			FFailureChangePin( AFailureChangePin )
		{
		}
	};
//---------------------------------------------------------------------------
	template<typename T_OWNER> class Maxim_MotorDriverChannel : public Maxim_MotorDriverBasicChannel<T_OWNER>
	{
		typedef Maxim_MotorDriverBasicChannel<T_OWNER> inherited;

	protected:
		int FPWMPin;

	protected:
		virtual void UpdateSpeed()
		{
			byte AValue = abs( inherited::FSpeed - 0.5 ) * 2 * 255 + 0.5;
//			Serial.println( AValue );
//			Serial.println( FPWMPin );
			analogWrite( FPWMPin, AValue );
//			Serial.println( "---" );
		}

	public:
		Maxim_MotorDriverChannel( T_OWNER &AOwner, int AIndex, int APWMPin, float AInitialSpeed, float ASenseResistor ) :
			inherited( AOwner, AIndex, AInitialSpeed, ASenseResistor ),
			FPWMPin( APWMPin )
		{
		}
	};
//---------------------------------------------------------------------------
	template<typename T_OWNER> class Maxim_MotorDriverComponentChannel : public Maxim_MotorDriverBasicChannel<T_OWNER>
	{
		typedef Maxim_MotorDriverBasicChannel<T_OWNER> inherited;

	public:
		OpenWire::SourcePin	MotorControlOutputPin;

	protected:
		virtual void UpdateSpeed()
		{
			float AValue = abs( inherited::FSpeed - 0.5 ) * 2;
//			Serial.println( inherited::FSpeed );
			MotorControlOutputPin.Notify( &AValue );
		}

	public:
		using inherited::inherited;
	};
//---------------------------------------------------------------------------
	template<typename T_I2C, T_I2C &C_I2C, typename T_INSTANCE> void Maxim_MotorDriverBasicShield<T_I2C, C_I2C, T_INSTANCE>::SystemStart()
	{
		DoGPIOTransmissionAt( 0x4, 0b10000001 ); // Enable and enable the interrupts
		for( int i = 0x9; i < 0x9 + 4; ++i )
			DoGPIOTransmissionAt( i, 0b01010101 ); // All Output

		DoGPIOTransmissionAt( 0x0E, 0xFF ); // All inputs
		DoGPIOTransmissionAt( 0x0E, 0b01111111 ); // P31 Poutput, the rest inputs

		UpdateSettings();

		for( int i = 0; i < FChannels.size(); ++i )
			FChannels[ i ]->InitFailure();

	}
//---------------------------------------------------------------------------
	template<typename T_I2C, T_I2C &C_I2C, typename T_INSTANCE> void Maxim_MotorDriverBasicShield<T_I2C, C_I2C, T_INSTANCE>::UpdateFailures()
	{
		DoGPIOReadAt( 6 ); // Clear the interrupt bit

		byte AFailureFlags = DoGPIOReadAt( 0x5B ); // Read pins 27 and up
		for( int i = 0; i < FChannels.size(); ++ i )
			FChannels[ i ]->UpdateFailure( AFailureFlags & ( 1 << i ) == 0 );

	}
//---------------------------------------------------------------------------
	template<typename T_I2C, T_I2C &C_I2C, typename T_INSTANCE> void Maxim_MotorDriverBasicShield<T_I2C, C_I2C, T_INSTANCE>::UpdateSettings()
	{
		if( FChanged & 0b00110000 )
		{
			byte	MAX7300Reg = FChannels[ 0 ]->GetRegisterPins() | ( FChannels[ 1 ]->GetRegisterPins() << 4 );
//			Serial.println( MAX7300Reg );
			DoGPIOTransmissionAt( 0x44, MAX7300Reg );
		}

		if( FChanged & 0b11000000 )
		{
			byte	MAX7300Reg = FChannels[ 2 ]->GetRegisterPins() | ( FChannels[ 3 ]->GetRegisterPins() << 4 );
			DoGPIOTransmissionAt( 0x4C, MAX7300Reg );
		}

		if( FChanged & 0b00000011 )
		{
			if( FChanged & 0b00000001 )
				DoPotentiometerTransmission( F_MAX5387_1, 0b00010001, FChannels[ 0 ]->GetPotentiometer() );

			if( FChanged & 0b00000010 )
				DoPotentiometerTransmission( F_MAX5387_1, 0b00010010, FChannels[ 1 ]->GetPotentiometer() );

		}

		if( FChanged & 0b00001100 )
		{
			if( FChanged & 0b00000100 )
				DoPotentiometerTransmission( F_MAX5387_2, 0b00010001, FChannels[ 2 ]->GetPotentiometer() );

			if( FChanged & 0b00001000 )
				DoPotentiometerTransmission( F_MAX5387_2, 0b00010010, FChannels[ 3 ]->GetPotentiometer() );
		}

		for( int i = 0; i < FChannels.size(); ++i )
			FChannels[ i ]->UpdateSpeed();

		FChanged = 0;
		FModified = false;
	}
//---------------------------------------------------------------------------
}

#endif
