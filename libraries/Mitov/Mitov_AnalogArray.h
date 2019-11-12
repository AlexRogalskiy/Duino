////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2018 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_ANALOGARRAY_h
#define _MITOV_ANALOGARRAY_h

#include <Mitov.h>

#ifdef __TEST_FOR_DEBUG_PRINTS__
#define Serial UNGUARDED DEBUG PRINT!!!
#endif

namespace Mitov
{
	class SlidingWindowAnalogArray
	{
	public:
		OpenWire::SourcePin	OutputPin;

	public:
		uint16_t	Count = 128;
		uint16_t	Step = 1;
		bool		Normalize : 1;

	public:
		bool	ClockInputPin_o_IsConnected : 1;

	protected:
		uint16_t	FIndex = 0;
		uint16_t	FStepCounter = 0;
		float		*FQueue = nullptr;

	public:
		void SetCount( uint16_t AValue )
		{
			if( Count == AValue )
				return;

			float	*AOldBuffer = FQueue;
			FQueue = new float[ AValue ];

			if( AValue > Count )
				memset( FQueue + Count, 0, ( AValue - Count ) * sizeof( float ) );

			memcpy( FQueue, AOldBuffer, MitovMin( Count, AValue ) * sizeof( float ));

			Count = AValue;
			delete [] AOldBuffer;
		}

	public:
		void InputPin_o_Receive( void *_Data )
		{
			if( FIndex >= Count )
				FIndex = 0;

//			Serial.println( *(float *)_Data );

			FQueue[ FIndex ++ ] = *(float *)_Data;

//			TArray<float> ABuffer( Count, FQueue );
//			Serial.println( ABuffer.ToString() );
//			return;

			if( ClockInputPin_o_IsConnected )
				return;

			uint16_t AStep = ( Step ) ? Step : Count;
			if( ++ FStepCounter >= AStep )
			{
				FStepCounter = 0;
				ClockInputPin_o_Receive( nullptr );
			}
		}

		void ClockInputPin_o_Receive( void *_Data )
		{
			float *AData = new float[ Count ];

			float *APtr = AData;
			int AIndex = FIndex;

			if( Normalize )
			{
				float AMin = FQueue[ 0 ]; //MitovMin
				float AMax = AMin; //MitovMax

				for( int i = 1; i < Count; ++i )
				{
					AMin = MitovMin( AMin, FQueue[ i ] );
					AMax = MitovMax( AMax, FQueue[ i ] );
				}

				for( uint16_t i = 0; i < Count; ++i )
				{
					if( AIndex >= Count )
						AIndex = 0;

					*APtr++ = (( FQueue[ AIndex ++ ] - AMin ) / ( AMax - AMin ));
				}
			}

			else
			{
				for( uint16_t i = 0; i < Count; ++i )
				{
					if( AIndex >= Count )
						AIndex = 0;

					*APtr++ = FQueue[ AIndex ++ ];
				}
			}

//			Serial.println( "TEST" );

			TArray<float> ABuffer( Count, AData );
			OutputPin.Notify( &ABuffer );

			delete [] AData;
		}

	public:
		inline void SystemInit()
		{
			if( FQueue )
				delete [] FQueue;

			FQueue = new float[ Count ];
			memset( FQueue, 0, sizeof( float ) * Count );
		}

	public:
		SlidingWindowAnalogArray() :
			Normalize( false ),
			ClockInputPin_o_IsConnected( false )
		{
		}

	};

}

#ifdef __TEST_FOR_DEBUG_PRINTS__
#undef Serial
#endif

#endif
