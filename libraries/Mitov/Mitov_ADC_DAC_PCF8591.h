////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2018 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_ADC_DAC_PCF8591_h
#define _MITOV_ADC_DAC_PCF8591_h

#include <Mitov.h>

#ifdef __TEST_FOR_DEBUG_PRINTS__
#define Serial UNGUARDED DEBUG PRINT!!!
#endif

namespace Mitov
{
//---------------------------------------------------------------------------
	class PCF8591_DAC
	{
	public:
		bool	Enabled = true;
		float	InitialValue = 0;
	};
//---------------------------------------------------------------------------
	enum TArduinoPCF8591ADCMode { adcSingleEnded, adc3Differential, adcMixed, adc2Differential };
//---------------------------------------------------------------------------
	template<uint8_t C_ADDRESS, typename T_I2C, T_I2C &C_I2C> class PCF8591
	{
	public:
		OpenWire::TypedSourcePin<float>	OutputPins[ 4 ];

	public:
		PCF8591_DAC	DAC;

	public:
//		bool	ClockInputPin_o_IsConnected : 1;
		bool	DAC_ClockInputPin_o_IsConnected : 1;

	public:
		bool					Enabled : 1;
		TArduinoPCF8591ADCMode	Mode : 2;

	protected:
		uint8_t FStartChannel : 2;
		uint8_t FEndChannel : 2;

//		uint8_t	FConfig = 0xFF;

	public:
		inline void ClockInputPin_o_Receive( void *_Data )
		{
			ProcessData( false, false );
		}

#ifdef _MITOV_PCF8591_DAC_CLOCK_PIN_
		inline void DAC_ClockInputPin_o_Receive( void *_Data )
		{
			ProcessData( true, false );
		}
#endif

#ifdef _MITOV_PCF8591_DAC_INPUT_PIN_
		void DAC_InputPin_o_Receive( void *_Data )
		{
			float AValue = *(float *)_Data;
			if( AValue == DAC.InitialValue )
				return;

			DAC.InitialValue = AValue;
#ifdef _MITOV_PCF8591_DAC_CLOCK_PIN_
			if( ! DAC_ClockInputPin_o_IsConnected )
#endif
				ProcessData( true, false );

		}
#endif

	protected:
		void ProcessData( bool AFromDAC, bool AFirstTime )
		{
			if( ! Enabled )
				return;

			uint8_t	AStartChannel;
			uint8_t AMaxChannel;
			switch( Mode )
			{
				case adcSingleEnded:	AMaxChannel = 4; break;
				case adc2Differential:	AMaxChannel = 2; break;
				default: AMaxChannel = 3;
			}

			if( FStartChannel > AMaxChannel )
				AStartChannel = AMaxChannel;

			else
				AStartChannel = FStartChannel;


//			for( AStartChannel = 0; AStartChannel < AMaxChannel; ++AStartChannel )
//				if( OutputPins[ i ].IsConnected() )
//					break;

/*
			while( AMaxChannel > AStartChannel )
			{
				if( OutputPins[ AMaxChannel ].IsConnected() )
					break;

				--AMaxChannel;
			}
*/

			uint8_t	AConfig =	AStartChannel | 
								0b100 | 
								uint8_t( Mode ) << 4;

			if( DAC.Enabled )
				AConfig |= 0b01000000;

			uint8_t	AOutValue = ( DAC.InitialValue * 255 ) + 0.5;

			C_I2C.beginTransmission( C_ADDRESS );
			C_I2C.write( AConfig );
//			Serial.println( AConfig, BIN );
			if( AFromDAC )
				C_I2C.write( AOutValue );

			C_I2C.endTransmission();

			if( AFromDAC )
				if( ! AFirstTime )
					return;

			if( FEndChannel < AStartChannel )
				return;

			int8_t ACount = FEndChannel - AStartChannel + 1;

			C_I2C.requestFrom( C_ADDRESS, uint8_t( ACount + 1 ));

			C_I2C.read(); // Skip old value
			for( int8_t i = 0; i < ACount; ++i )
			{
				bool AIsDiferential = false;
				if((( AStartChannel + i ) < 2 ) && ( Mode == adc2Differential ) )
					AIsDiferential = true;

				else if((( AStartChannel + i ) == 2 ) && ( Mode == adcMixed ) )
					AIsDiferential = true;

				float AValue;
				if( AIsDiferential )
					AValue = ( float( int8_t( C_I2C.read() )) / 255.0 ) + 0.5;

				else
  					AValue = float( C_I2C.read() ) / 255.0;

				OutputPins[ AStartChannel + i ].SetValue( AValue, ! AFirstTime );
			}
		}

	public:
		inline void SystemStart()
		{
			ProcessData( true, true );
		}

		inline void SystemLoopBegin( unsigned long currentMicros )
		{
			ProcessData( false, false );
		}

	public:
		PCF8591( uint8_t AStartChannel, uint8_t AEndChannel ) :
//			ClockInputPin_o_IsConnected( false ),
			DAC_ClockInputPin_o_IsConnected( false ),
			Enabled( true ),
			Mode( adcSingleEnded ),
			FStartChannel( AStartChannel ),
			FEndChannel( AEndChannel )
		{
		}
	};
//---------------------------------------------------------------------------
}

#ifdef __TEST_FOR_DEBUG_PRINTS__
#undef Serial
#endif

#endif