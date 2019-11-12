////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2018 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_INTERNET_TIME_h
#define _MITOV_INTERNET_TIME_h

#include <Mitov.h>
#include <Mitov_BasicEthernet.h>

#ifdef __TEST_FOR_DEBUG_PRINTS__
#define Serial UNGUARDED DEBUG PRINT!!!
#endif

namespace Mitov
{
	template<typename T_SOCKET, T_SOCKET &C_SOCKET> class InternetTime
	{
	public:
		OpenWire::SourcePin	OutputPin;

    public:
		float		TimeZoneOffset = 0;
		TDateTime	BaseDate;
        bool		Enabled : 1;
		bool		SetPort : 1;

	protected:
		uint8_t	FIndex : 3;
		uint8_t	FTimeValue[ 4 ];
		unsigned long	FStartTime;

	public:
		void _DirectPinReceive( void *_Data )
		{
			Mitov::TDataBlock ADataBlock = *(Mitov::TDataBlock*)_Data;
//			Serial.println( "Receive" );
//			Serial.println( ADataBlock.Size );
			uint8_t *ASrc = ADataBlock.Data;
			for( int i = 0; i < ADataBlock.Size; ++i )
			{
				FTimeValue[ 3 - FIndex ] = *ASrc++;
				FIndex ++;
				if( FIndex == 4 )
					break;

//				Serial.println( FIndex );
			}

			if( FIndex == 4 )
			{
				FIndex = 0;
				unsigned long	ADelay = ( millis() - FStartTime ) / 2;
				C_SOCKET.StopSocket();

//    Result := ( ((BufCard + (FRoundTripDelay div 1000))/ (24 * 60 * 60) ) + Int(fBaseDate))

				uint32_t	ATimeValue;
				memcpy( &ATimeValue, FTimeValue, sizeof( ATimeValue ));
//				Serial.println( ATimeValue );
				TDateTime ADateTime = BaseDate;
//				ADateTime.AddYears( 100 );
//				ATimeValue -= 3155673600;
//				Serial.println( ATimeValue );
				uint32_t	ATimeValue1 = ATimeValue / ( 60 * 60 );
				ADateTime.AddHours( ATimeValue1 );
				ADateTime.AddSeconds( ATimeValue % ( 60 * 60 ));
//				ADateTime.AddSeconds( ATimeValue );
				ADateTime.AddMilliSeconds( ADelay );

				int32_t	AIntOffset = TimeZoneOffset;

//				Serial.println( AIntOffset );

				ADateTime.AddHours( AIntOffset );
				float AFraction = V_FMOD( TimeZoneOffset, 1 );
//				Serial.println( AFraction );

				AIntOffset = AFraction * 60;

//				Serial.println( AIntOffset );
				ADateTime.AddMinutes( AIntOffset );

				OutputPin.SendValue( ADateTime );
			}


		}

	public:
		void ClockInputPin_o_Receive( void *_Data )
		{
			FIndex = 0;
			C_SOCKET.StopSocket();
//			Serial.println( "Try Start" );
			if( SetPort )
				C_SOCKET.Port = 37;

			C_SOCKET.Enabled = true;
			C_SOCKET.TryStartSocket();
			FStartTime = millis();
		}

	public:
		InternetTime() :
			BaseDate( 693596, 0 ),
			Enabled( true ),
			SetPort( true ),
			FIndex( 0 )
		{
		}

	};
}

#ifdef __TEST_FOR_DEBUG_PRINTS__
#undef Serial
#endif

#endif // _MITOV_INTERNET_TIME_h
