////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2018 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_RANDOM_GENERATOR_h
#define _MITOV_RANDOM_GENERATOR_h

#include <Mitov.h>

namespace Mitov
{
#define Min Min
#define Max Max
	template<typename T, typename T_BASE> class CommonRandomGenerator : public T_BASE
	{
	public:
		OpenWire::SourcePin	OutputPin;

	public:
		bool	Enabled = true;

	public: // Needs to be public due to compiler bug :-(
		T	FValue = T( 0 );

	public:
		T	Min;
		T	Max;
		long	Seed = 0;

	public:
		inline void SystemStart()
		{
			randomSeed( Seed );
		}

		inline void SystemLoopBegin( unsigned long currentMicros )
		{
			Generate();
		}

	protected:
		void Generate()
		{
            if( Enabled )
            {
				if( Min == Max )
					FValue = Min;

				else
					T_BASE::GenerateValue( this );
			}

			OutputPin.Notify( &FValue );
		}

	public:
		inline void ClockInputPin_o_Receive( void *_Data )
		{
			Generate();
		}

	public:
		CommonRandomGenerator( T AMin, T AMax ) :
			Min( AMin ),
			Max( AMax )
		{
		}

	};
//---------------------------------------------------------------------------
	class RandomAnalogGenerator
	{
	protected:
		template<typename T> void GenerateValue( T *AInherited )
		{
			float AMin = MitovMin( AInherited->Min, AInherited->Max );
			float AMax = MitovMax( AInherited->Min, AInherited->Max );
//			double ARandom = random( -2147483648, 2147483647 );
			double ARandom = random( -1147483648, 1147483647 );
//			FValue = ARandom;
			ARandom += 1147483648;
			AInherited->FValue = AMin + ( ARandom / ( (double)1147483647 + (double)1147483648 )) * (AMax - AMin);
		}

	};
//---------------------------------------------------------------------------
	template<typename T_DATA> class RandomIntegerGenerator
	{
	protected:
		template<typename T> void GenerateValue( T *AInherited )
		{
			T_DATA AMin = MitovMin( AInherited->Min, AInherited->Max );
			T_DATA AMax = MitovMax( AInherited->Min, AInherited->Max );
			AInherited->FValue = random( AMin, AMax + 1 );
		}

	};
//---------------------------------------------------------------------------
	class RandomDigitalGenerator
	{
	public:
		OpenWire::SourcePin	OutputPin;

	public:
		bool	Enabled : 1;

	protected:
		bool	FValue : 1;

	public:
		long	Seed = 0;

	public:
		inline void SystemStart()
		{
			randomSeed( Seed );
		}

		inline void SystemLoopBegin( unsigned long currentMicros )
		{
			Generate();
		}

	protected:
		inline void GenerateValue()
		{
			FValue = random( 0, 2 );
		}

		void Generate()
		{
            if( Enabled )
				GenerateValue();

			OutputPin.SendValue<bool>( FValue );
		}

	public:
		inline void ClockInputPin_o_Receive( void *_Data )
		{
			Generate();
		}

	public:
		RandomDigitalGenerator() :
			Enabled( true ),
			FValue( false )
		{
		}

	};
//---------------------------------------------------------------------------
	class RandomClockGenerator
	{
	public:
		OpenWire::SourcePin	OutputPin;

	protected:
		unsigned long FLastTime = 0;
		unsigned long FPeriod = 0;

	public:
		long	Seed = 0;

		bool	Enabled = true;
		float	MinFrequency = 0.1;
		float	MaxFrequency = 1.0;

	protected:
		void GeneratePeriod()
		{
			float AMin = MitovMin( MinFrequency, MaxFrequency );
			float AMax = MitovMax( MinFrequency, MaxFrequency );
			double ARandom = random( -1147483648, 1147483647 );
			ARandom += 1147483648;
			ARandom = AMin + ( ARandom / ( (double)1147483647 + (double)1147483648 )) * (AMax - AMin);
			ARandom = MitovMax( (double)AMin, MitovMin( (double)AMax, (double)ARandom ));

			if( ARandom == 0 )
				FPeriod = 1000000;

			else
				FPeriod = ( (( 1.0 / ARandom ) * 1000000 ) + 0.5 );

		}

	public:
		inline void SystemLoopBegin( unsigned long currentMicros )
		{
			while( currentMicros - FLastTime >= FPeriod )
			{
				if( Enabled )
					OutputPin.Notify( nullptr );

				FLastTime += FPeriod;
				GeneratePeriod();
			}
		}

		inline void SystemStart()
		{
			GeneratePeriod();
			FLastTime = micros();
		}

	};
//---------------------------------------------------------------------------
	class RandomDateTimeGenerator
	{
	protected:
		template<typename T> void GenerateValue( T *AInherited )
		{
			int32_t ARandomDate = random( AInherited->Min.Date, AInherited->Max.Date );
			int32_t ARandomTime;
			if( ARandomDate == AInherited->Min.Date )
				ARandomTime = random( AInherited->Min.Time, MSecsPerDay );

			else if( ARandomDate == AInherited->Max.Date )
				ARandomTime = random( 0, AInherited->Max.Time );

			else
				ARandomTime = random( 0, MSecsPerDay );

			AInherited->FValue.Date = ARandomDate;
			AInherited->FValue.Time = ARandomTime;
/*
			float AMin = MitovMin( Min.Value, Max.Value );
			float AMax = MitovMax( Min.Value, Max.Value );
//			double ARandom = random( -2147483648, 2147483647 );
			double ARandom = random( -1147483648, 1147483647 );
//			FValue = ARandom;
			ARandom += 1147483648;
			FValue.Value = AMin + ( ARandom / ( (double)1147483647 + (double)1147483648 )) * (AMax - AMin);
*/
		}

	};
//---------------------------------------------------------------------------
#undef Min
#undef Max
}

#endif
