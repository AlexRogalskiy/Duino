////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2018 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_BASIC_GENERATOR_h
#define _MITOV_BASIC_GENERATOR_h

#include <Mitov.h>

namespace Mitov
{
	template<typename T, typename T_BASE> class TypedFrequencyGenerator : public T_BASE
	{
	public:
		OpenWire::SourcePin	OutputPin;

	public:
		bool	Enabled = true;
		float	Frequency = 1.0f;
		T	Amplitude;
		T	Offset;

		// 0 - 1
		float	Phase = 0.0f;

	public: // Needs to be public for the base class
		T	FValue = T( 0 );
		float	FPhase = 0.0f;
		unsigned long FLastTime = 0;

	public:
		inline void SetFrequency( float AValue )
		{
			Frequency = AValue;
//			inherited::CalculateFields();
		}

		void SetPhase( float AValue )
		{
			if( Phase == AValue )
				return;

			Phase = AValue;
			FPhase = Phase;
		}

	public:
		inline void SystemStart()
		{
			FPhase = Phase;
		}

		inline void SystemLoopBegin( unsigned long currentMicros )
		{
			Generate( currentMicros );
		}

	protected:
		void Generate( unsigned long currentMicros )
		{
			if( Enabled && ( Frequency != 0 ) )
			{
				float APeriod = 1000000 / Frequency;

				float ATime = ( currentMicros - FLastTime );
				ATime /= APeriod;
				FPhase += ATime;
				FPhase = V_FMOD( FPhase, 1 );

				T_BASE::CalculateValue( this );
			}

			FLastTime = currentMicros;
			OutputPin.Notify( &FValue );
		}

	public:
		inline void ClockInputPin_o_Receive( void *_Data )
		{
			Generate( micros() );
		}

	public:
		TypedFrequencyGenerator( T AAmplitude, T AOffset ) :
			Amplitude( AAmplitude ),
			Offset( AOffset )
		{
		}

	};
//---------------------------------------------------------------------------
}

#endif
