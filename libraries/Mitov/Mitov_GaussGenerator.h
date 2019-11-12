////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2018 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_GAUSS_GENERATOR_h
#define _MITOV_GAUSS_GENERATOR_h

#include <Mitov.h>

#ifdef __TEST_FOR_DEBUG_PRINTS__
#define Serial UNGUARDED DEBUG PRINT!!!
#endif

namespace Mitov
{
	template<typename T> class CommonGaussGenerator
	{
	public:
		OpenWire::SourcePin	OutputPin;

	public:
		bool	Enabled = true;

	public: // Needs to be public due to compiler bug :-(
		T	FValue = T( 0 );

	public:
		T	Mean;
		T	StandardDeviation;
		long	Seed = 0;

	protected:
		uint8_t	FIndex = 0;
		T FRandoms[ 2 ];

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
				if( ! FIndex )
				{
					float W;
					float X1;
					float X2;
					do
					{
						X1 = random( -1147483648, 1147483647 ) / 1147483647.0;
						X2 = random( -1147483648, 1147483647 ) / 1147483647.0;
						W = X1 * X1 + X2 * X2;
					}
					while(  W >= 1.0 );

					W = StandardDeviation * sqrt( (-2.0 * log( W ) ) / W );
/*
					Serial.println( W );
					Serial.println( Mean );
					Serial.println( X1 );
					Serial.print( "RESULT1: " ); Serial.println( Mean + X1 * W );
					Serial.println( X2 );
					Serial.print( "RESULT2: " ); Serial.println( Mean + X2 * W );
*/
					FRandoms[ 0 ] = Mean + X1 * W;
					FRandoms[ 1 ] = Mean + X2 * W;
				}

				FValue = FRandoms[ FIndex ];
				FIndex = FIndex ^ 1;
			}

			OutputPin.Notify( &FValue );
		}

	public:
		inline void ClockInputPin_o_Receive( void *_Data )
		{
			Generate();
		}

	public:
		CommonGaussGenerator( T AMean, T AStandardDeviation ) :
			Mean( AMean ),
			StandardDeviation( AStandardDeviation )
		{
		}

	};
//---------------------------------------------------------------------------
}

#ifdef __TEST_FOR_DEBUG_PRINTS__
#undef Serial
#endif

#endif
