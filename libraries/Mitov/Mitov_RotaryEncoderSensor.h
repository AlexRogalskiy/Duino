////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2018 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_ROTARY_ENCODER_SENSOR_h
#define _MITOV_ROTARY_ENCODER_SENSOR_h

#include <Mitov.h>

namespace Mitov
{
	template<typename T_B_PIN, T_B_PIN &C_B_PIN> class RotaryEncoderSensor
	{
	public:
		OpenWire::SourcePin	UpOutputPin;
		OpenWire::SourcePin	DownOutputPin;

	public:
		bool	Enabled : 1;

	protected:
		bool	FAValue : 1;
//		bool	FBValue : 1;
//		bool    FAPopulated : 1;
//		bool    FBPopulated : 1;

	public:
		void AInputPin_o_Receive( void *_Data )
		{
			bool AValue = *( bool *)_Data;

//			FAPopulated = true;
			if( FAValue == AValue )
				return;

			FAValue = AValue;
			if( Enabled )
				if( AValue )
				{
					if( C_B_PIN.DigitalRead() )
						DownOutputPin.Notify( nullptr );

					else
						UpOutputPin.Notify( nullptr );
				}
				else
				{
					if( C_B_PIN.DigitalRead() )
						UpOutputPin.Notify( nullptr );

					else
						DownOutputPin.Notify( nullptr );
				}

		}

/*
		void DoReceiveB( bool AValue )
		{
			FBPopulated = true;
			if( FBValue == AValue )
				return;

			FBValue = AValue;
		}
*/
	public:
		inline void SystemLoopBegin( unsigned long currentMicros ) {} // Placeholder

	public:
		RotaryEncoderSensor() :
			Enabled( true ),
//			FAPopulated( false ),
//			FBPopulated( false ),
			FAValue( false )
//			FBValue( false )
		{
		}
	};
//---------------------------------------------------------------------------
	template<typename T_B_PIN, T_B_PIN &C_B_PIN> class RotaryEncoderSensor_Interrupt : public OpenWire::InterruptObject
	{
	public:
		OpenWire::SourcePin	UpOutputPin;
		OpenWire::SourcePin	DownOutputPin;

	public:
		bool	Enabled : 1;

	protected:
		uint8_t FUpCount : 7;

		bool	FAValue : 1;
//		bool	FBValue : 1;
//		bool    FAPopulated : 1;
//		bool    FBPopulated : 1;
		uint8_t FDownCount : 7;

	public:
		void InterruptHandlerA( bool AValue )
		{
//			FAPopulated = true;
			if( FAValue == AValue )
				return;

			FAValue = AValue;
			if( Enabled )
				if( AValue )
				{
					if( C_B_PIN.DigitalRead() )
						++FDownCount;

					else
						++FUpCount;
				}
				else
				{
					if( C_B_PIN.DigitalRead() )
						++FUpCount;

					else
						++FDownCount;
				}
		}

/*
	protected:
		void DoReceiveB( void *_Data )
		{
			bool AValue = *( bool *)_Data;
			FBPopulated = true;
			if( FBValue == AValue )
				return;

			FBValue = AValue;
		}
*/
	public:
		inline void SystemLoopBegin( unsigned long currentMicros )
		{
			noInterrupts();
			uint8_t ADownCount = FDownCount;
			FDownCount = 0;

			uint8_t AUpCount = FUpCount;
			FUpCount = 0;

			interrupts();

			while( ADownCount )
			{
				--ADownCount;
				DownOutputPin.Notify( nullptr );
			}

			while( AUpCount )
			{
				--AUpCount;
				UpOutputPin.Notify( nullptr );
			}

		}

	public:
		RotaryEncoderSensor_Interrupt() :
			Enabled( true ),
			FUpCount( 0 ),
//			FAPopulated( false ),
//			FBPopulated( false ),
			FAValue( false ),
			FDownCount( 0 )
//			FBValue( false )
		{
		}

	};
//---------------------------------------------------------------------------
	template<typename T_B_PIN, T_B_PIN &C_B_PIN> class RotaryEncoderSensor_Debounce
	{
	public:
		OpenWire::SourcePin	UpOutputPin;
		OpenWire::SourcePin	DownOutputPin;

	public:
		uint32_t	DebounceInterval = 0;
		bool	Enabled : 1;

	protected:
		bool	FAValue : 1;
//		bool	FBValue : 1;
//		bool    FAPopulated : 1;
//		bool    FBPopulated : 1;
		bool	FALastValue : 1;
//		bool	FInTimer : 1;
//		bool	FBLastValue : 1;

		unsigned long	FALastTime = 0;
//		unsigned long	FBLastTime = 0;

	protected:
		void DoValueReceiveA( bool AValue )
		{
//			FAPopulated = true;
//			FInTimer = false;
			if( FAValue == AValue )
				return;

			FAValue = AValue;
			if( Enabled )
				if( AValue )
				{
					if( C_B_PIN.DigitalRead() )
						DownOutputPin.Notify( nullptr );

					else
						UpOutputPin.Notify( nullptr );
				}
				else
				{
					if( C_B_PIN.DigitalRead() )
						UpOutputPin.Notify( nullptr );

					else
						DownOutputPin.Notify( nullptr );
				}

		}

/*
		void DoValueReceiveB( bool AValue )
		{
			FBPopulated = true;
			if( FBValue == AValue )
				return;

			FBValue = AValue;
		}
*/
	public:
		void AInputPin_o_Receive( void *_Data )
		{
			bool AValue = *( bool *)_Data;
			if( AValue != FALastValue )
			{
				FALastTime = millis();
//				FInTimer = true;
			}

			FALastValue = AValue;
		}

/*
		void DoReceiveB( void *_Data )
		{
			bool AValue = *( bool *)_Data;
			if( AValue != FBLastValue )
				FBLastTime = millis();

			FBLastValue = AValue;
		}
*/

	public:
		inline void SystemLoopBegin( unsigned long currentMicros )
		{
			if( FAValue != FALastValue )
//				if(( ! FInTimer ) || ( millis() - FALastTime > DebounceInterval ))
				if( millis() - FALastTime > DebounceInterval )
					DoValueReceiveA( FALastValue );

/*
			if( FBValue != FBLastValue )
				if( millis() - FBLastTime > DebounceInterval )
					DoValueReceiveB( FBLastValue );
*/
		}

	public:
		RotaryEncoderSensor_Debounce() :
			Enabled( true ),
			FAValue( false ),
			FALastValue( false )
//			FInTimer( false )
		{
		}
	};
//---------------------------------------------------------------------------
	template<typename T_B_PIN, T_B_PIN &C_B_PIN> class RotaryEncoderSensor_Debounce_Interrupt : public OpenWire::InterruptObject
	{
	public:
		OpenWire::SourcePin	UpOutputPin;
		OpenWire::SourcePin	DownOutputPin;

	public:
		uint32_t	DebounceInterval = 0;
		bool		Enabled : 1;

	protected:
		uint8_t FUpCount : 7;
//		bool	FAValue : 1;

		uint8_t FDownCount : 7;
		bool	FInTimer : 1;

		unsigned long	FALastTime = 0;

	public:
		void InterruptHandlerA( bool AValue )
		{
			if( ! AValue )
				return;

//			if( FAValue == AValue )
//				return;

			unsigned long	ATime = millis();
			if( FInTimer )
				if(( ATime - FALastTime ) <= DebounceInterval )
					return;

//			FAValue = AValue;
			if( Enabled )
			{
				FInTimer = true;
				FALastTime = ATime;
				if( AValue )
				{
					if( C_B_PIN.DigitalRead() )
						++FDownCount;

					else
						++FUpCount;
				}
				else
				{
					if( C_B_PIN.DigitalRead() )
						++FUpCount;

					else
						++FDownCount;
				}
			}
		}

	public:
		inline void SystemLoopBegin( unsigned long currentMicros )
		{
			unsigned long ATime = millis();

			noInterrupts();
			if( FInTimer )
				if( ATime - FALastTime > DebounceInterval )
					FInTimer = false;

			uint8_t ADownCount = FDownCount;
			FDownCount = 0;

			uint8_t AUpCount = FUpCount;
			FUpCount = 0;

			interrupts();

			while( ADownCount )
			{
				--ADownCount;
				DownOutputPin.Notify( nullptr );
			}

			while( AUpCount )
			{
				--AUpCount;
				UpOutputPin.Notify( nullptr );
			}

		}

	public:
		RotaryEncoderSensor_Debounce_Interrupt() :
			Enabled( true ),
			FUpCount( 0 ),
//			FAValue( false ),
			FDownCount( 0 ),
			FInTimer( false )
		{
		}

	};
//---------------------------------------------------------------------------
}

#endif
