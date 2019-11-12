////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2018 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_DATE_DIME_h
#define _MITOV_DATE_DIME_h

#include <Mitov.h>

#ifdef __TEST_FOR_DEBUG_PRINTS__
#define Serial UNGUARDED DEBUG PRINT!!!
#endif

namespace Mitov
{
//---------------------------------------------------------------------------
    class EncodeDateTime
	{
	public:
		OpenWire::SourcePin	OutputPin;

    public:
        TDateTime	InitialValue;
		bool	OnlyModified : 1;

	protected:
		bool	FModified : 1;
		TDateTime	FLastOutput;
		int16_t	FValues[ 7 ] = { 0 };

	public:
		void InputPins_o_Receive( int AIndex, void *_Data )
		{
			int32_t	AValue = *(int32_t *)_Data;
			if( FValues[ AIndex ] == AValue )
				return;

			FValues[ AIndex ] = AValue;
			FModified = true;
		}

		void ClockInputPin_o_Receive( void *_Data )
		{
			TDateTime AValue = CalculateOutput();
			OutputPin.Notify( &AValue );
		}

	protected:
		void CalculateSendOutput( bool AFromStart )
		{
			TDateTime AValue = CalculateOutput();
			if( ! AFromStart )
				if( FLastOutput == AValue )
					return;

			OutputPin.SendValue( AValue );
			FLastOutput = AValue;

			FModified = false;
		}

	public:
		inline void SystemStart()
		{
			CalculateSendOutput( true );
		}

		inline void SystemLoopEnd( unsigned long currentMicros )
		{
			if( FModified )
				CalculateSendOutput( false );

		}

	protected:
		TDateTime CalculateOutput()
		{
			TDateTime ADate;
			ADate.TryEncodeDateTime( FValues[ 0 ], FValues[ 1 ], FValues[ 2 ], FValues[ 3 ], FValues[ 4 ], FValues[ 5 ], FValues[ 6 ] );
			return ADate;
		}

	public:
		EncodeDateTime() :
			OnlyModified( false ),
			FModified( false )
		{
		}

	};
//---------------------------------------------------------------------------
	class DecodeDateTime
	{
	public:
		OpenWire::SourcePin	OutputPins[ 8 ];

	public:
		void InputPin_o_Receive( void *_Data )
		{
			TDateTime &ADate = *(TDateTime *)_Data;
			uint16_t AItem16[ 8 ];
			ADate.DecodeDateTime( AItem16[ 0 ], AItem16[ 1 ], AItem16[ 2 ], AItem16[ 7 ], AItem16[ 3 ], AItem16[ 4 ], AItem16[ 5 ], AItem16[ 6 ] );
			for( int i = 0; i < 8; ++ i )
			{
				long int AValue = AItem16[ i ];
				OutputPins[ i ].Notify( &AValue );
			}
		}
	};	
//---------------------------------------------------------------------------
	class CompileDateTime
	{
	public:
		OpenWire::SourcePin	OutputPin;

	protected:
		TDateTime	FValue;

	public:
		inline void SystemStart()
		{
			OutputPin.Notify( &FValue );
		}

	public:
		inline void ClockInputPin_o_Receive( void *_Data )
		{
			OutputPin.Notify( &FValue );
		}

	public:
		CompileDateTime()
		{
			const char *monthName[12] = 
			{
			  "Jan", "Feb", "Mar", "Apr", "May", "Jun",
			  "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
			};

			char AMonth[12];
			int ADay, AYear;
			uint8_t AMonthIndex;
			sscanf( __DATE__, "%s %d %d", AMonth, &ADay, &AYear);

			int AHour, AMin, ASec;
			sscanf( __TIME__, "%d:%d:%d", &AHour, &AMin, &ASec);

			for (AMonthIndex = 0; AMonthIndex < 12; AMonthIndex++) 
				if (strcmp(AMonth, monthName[AMonthIndex]) == 0)
					break;

			FValue.TryEncodeDateTime( AYear, AMonthIndex + 1, ADay, AHour, AMin, ASec, 0 );
		}

	};
//---------------------------------------------------------------------------
	class AddDateTime
	{
	public:
		OpenWire::SourcePin	OutputPin;

	public:
		bool	Enabled = true;

		int32_t	Years = 0;
		int32_t	Months = 0;
		int32_t	Days = 0;
		int32_t	Hours = 0;
		int32_t	Minutes = 0;
		int32_t	Seconds = 0;
		int32_t	MilliSeconds = 0;

	public:
		void InputPin_o_Receive( void *_Data )
		{
			if( ! Enabled )
			{
				OutputPin.Notify( _Data );
				return;
			}

			TDateTime ADate = *(TDateTime *)_Data;
			if( Years )
				ADate.AddYears( Years );

			if( Months )
				ADate.AddMonths( Months );

			if( Days )
				ADate.AddDays( Days );

			if( Hours )
				ADate.AddHours( Hours );

			if( Minutes )
				ADate.AddMinutes( Minutes );

			if( Seconds )
				ADate.AddSeconds( Seconds );

			if( MilliSeconds )
				ADate.AddMilliSeconds( MilliSeconds );

			OutputPin.Notify( &ADate );
		}

	};
//---------------------------------------------------------------------------
}

#ifdef __TEST_FOR_DEBUG_PRINTS__
#undef Serial
#endif

#endif