////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2018 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_BINARY_GENERATORS_h
#define _MITOV_BINARY_GENERATORS_h

#include <Mitov.h>

namespace Mitov
{
	class PulseGeneratorSimple
	{
	public:
		OpenWire::SourcePin	OutputPin;

	public:
		bool	Enabled = true;
		bool	InitialValue = false;
		float	Frequency = 1.0;

	protected:
		unsigned long FLastTime = 0;
		unsigned long FPeriod = 0;

	public:
		void SetFrequency( float AValue )
		{
			if( Frequency == AValue )
				return;

			Frequency = AValue;
			CalculateFields();
		}

	public:
		void CalculateFields()
		{
			if( Frequency == 0 )
				FPeriod = 1000000;

			else
				FPeriod = ( (( 1 / ( Frequency * 2 )) * 1000000 ) + 0.5 ); // 2 times one for each switch

		}

	public:
		inline void SystemLoopBegin( unsigned long currentMicros )
		{
			while( currentMicros - FLastTime >= FPeriod )
			{
				if( Enabled && ( Frequency != 0 ))
				{
					InitialValue = !InitialValue;
					OutputPin.Notify( &InitialValue );
				}

				FLastTime += FPeriod;
			}
		}

		inline void SystemInit()
		{
			CalculateFields();
			OutputPin.Notify( &InitialValue );
		}

		inline void SystemStart()
		{
			FLastTime = micros();
		}
	};
//---------------------------------------------------------------------------
	class PulseGenerator
	{
	public:
		OpenWire::SourcePin	OutputPin;

	public:
		float	Frequency = 1.0f;

		// -1 - 1
		float	Asymmetry = 0.0f;

		// 0 - 1
		float	Phase = 0.0f;

		bool	InitialValue : 1;
		bool	Enabled : 1;

	public:
		void SetPhase( float AValue )
		{
			if( Phase == AValue )
				return;

			Phase = AValue;
			FPhase = Phase;
		}

		void SetFrequency( float AValue )
		{
			if( Frequency == AValue )
				return;

			Frequency = AValue;
//			CalculateFields();
		}

	protected:
		float	FPhase = 0.0f;
		unsigned long FLastTime = 0;

	public:
		inline void SystemStart()
		{
			FPhase = Phase;
		}

		inline void SystemInit()
		{
			OutputPin.SendValue<bool>( InitialValue );
		}

		inline void SystemLoopBegin( unsigned long currentMicros )
		{
			if( Enabled && ( Frequency != 0 ) )
			{
				float APeriod = 1000000 / Frequency;

				float ATime = ( currentMicros - FLastTime );
				ATime /= APeriod;
				FPhase += ATime;
				FPhase = V_FMOD( FPhase, 1 );

				bool AValue = ( FPhase < 0.5 + Asymmetry / 2 );
				if( AValue != InitialValue )
				{
					InitialValue = AValue;
					OutputPin.SendValue<bool>( InitialValue );
				}
			}

			FLastTime = currentMicros;
//			OutputPin.SendValue( FValue );
		}

	public:
		PulseGenerator() :
			InitialValue( false ),
			Enabled( true )
		{
		}

	};
//---------------------------------------------------------------------------
}

#endif
