////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2018 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_STEPPER_h
#define _MITOV_STEPPER_h

#include <Mitov.h>

namespace Mitov
{

/*
	enum StepperMotorType =
	{
		smtDirectional,
		smtFull2Wire,
		smtFull3Wire,
		smtFull4Wire,
		smtHalf3Wire,
		smtHalf4Wire
	};
*/
	const uint8_t C_StepperBits4[] =
	{
			0b1100,
			0b0110,
			0b0011,
			0b1001
	};

	const uint8_t C_StepperBits8[] =
	{
			0b1000,
			0b1100,
			0b0100,
			0b0110,
			0b0010,
			0b0011,
			0b0001,
			0b1001
	};
//---------------------------------------------------------------------------
	const uint8_t C_Stepper3Bits3[] =
	{
			0b110,
			0b011,
			0b101
	};

	const uint8_t C_Stepper3Bits6[] =
	{
			0b100,
			0b110,
			0b010,
			0b011,
			0b001,
			0b101
	};
//---------------------------------------------------------------------------
	class StepperMotor4Wire
	{
	public:
		static uint8_t GetUpdatePinsValue( bool AHalfStep, uint8_t AStep )
		{
			return AHalfStep ? C_StepperBits8[ AStep & 0b0111 ] : C_StepperBits4[ AStep & 0b0011 ];
		}

	};
//---------------------------------------------------------------------------
	class StepperMotor3Wire
	{
	public:
		static uint8_t GetUpdatePinsValue( bool AHalfStep, uint8_t AStep )
		{
			return AHalfStep ? C_Stepper3Bits3[ AStep % 3 ] : C_Stepper3Bits6[ AStep % 6 ];
		}

	};
//---------------------------------------------------------------------------
	template <typename T_IMPLEMENTATION_CLASS, int C_NUMOUTPINS> class StepperMotor
	{
	public:
		OpenWire::SourcePin	OutputPins[ C_NUMOUTPINS ];

	protected:
		unsigned long	FLastTime = 0;
		uint8_t	FStep : 4;

	public:
		bool	Enabled : 1;
		bool	Reversed : 1;
		bool	HalfStep : 1; // In base class to save RAM
		float	StepsPerSecond = 300.f;

	public:
		inline void StepInputPin_o_Receive( void *_Data )
		{
			Step();
		}

	protected:
		void UpdatePins()
		{
			uint8_t AValue = T_IMPLEMENTATION_CLASS::GetUpdatePinsValue( HalfStep, FStep );
			for( int i = 0; i < C_NUMOUTPINS; ++i )
			{
				bool APinValue = ( AValue & 1 );
				OutputPins[ i ].Notify( &APinValue );
				AValue >>= 1;
			}
		}

	public:
		inline void SystemLoopBegin( unsigned long currentMicros )
		{
			if( ! Enabled )
				FLastTime = currentMicros;

			else
			{
				unsigned long APeriod = ( 1000000 / abs( StepsPerSecond )) + 0.5;
				if( ( currentMicros - FLastTime ) >= APeriod )
				{
					FLastTime += APeriod;
					if( StepsPerSecond )
					{
						if(( StepsPerSecond > 0 ) ^ Reversed )
							++FStep;

						else
							--FStep;

						UpdatePins();
					}
				}
			}
		}

	public:
		void Step()
		{
			if( Reversed ) //^ ( StepsPerSecond > 0 ) )
				--FStep;

			else
				++FStep;

			UpdatePins();
		}

	public:
		StepperMotor() : 
			Enabled( true ),
			Reversed( false ),
			HalfStep( true ),
			FStep( 0 )
		{
		}

	};
//---------------------------------------------------------------------------
}

#endif
