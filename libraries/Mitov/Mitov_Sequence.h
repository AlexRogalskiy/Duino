////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2018 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_SEQUENCE_h
#define _MITOV_SEQUENCE_h

#include <Mitov.h>

#ifdef __TEST_FOR_DEBUG_PRINTS__
#define Serial UNGUARDED DEBUG PRINT!!!
#endif

namespace Mitov
{
//---------------------------------------------------------------------------
	template<typename T_OWNER> class SequenceElement
	{
	public:
		OpenWire::SourcePin	OutputPin;

	public:
		uint32_t	Delay;

#ifdef _SEQUENCE_DIGIAL_ELEMENT_
	public:
		bool		FIsDigital;
#endif // _SEQUENCE_DIGIAL_ELEMENT_

	public:
		SequenceElement( T_OWNER &AOwner, bool AIsDigital, uint32_t ADelay, uint16_t AIndex ) :
			Delay( ADelay )
#ifdef _SEQUENCE_DIGIAL_ELEMENT_
			, FIsDigital( AIsDigital )
#endif // _SEQUENCE_DIGIAL_ELEMENT_
		{
			AOwner.FElements[ AIndex ] = this;
		}
	};
//---------------------------------------------------------------------------
	template<typename T_OWNER, T_OWNER &C_OWNER> class SequenceElement_Clocked : public SequenceElement<T_OWNER>
	{
		typedef	SequenceElement<T_OWNER> inherited;

	public:
		void ClockInputPin_o_Receive( void *_Data )
		{
			C_OWNER.SignalElement( this );		
		}

	public:
		using inherited::inherited;

	};
//---------------------------------------------------------------------------
	template<uint16_t C_CHANNELS_COUNT> class Sequence
	{
	public:
		bool	Enabled : 1;
		bool	Repeat : 1;
		bool	CanRestart : 1;

	protected:
		bool	IsEnabled : 1;
//		SequenceClockedElement *FCurrentElement = nullptr;
		int16_t		FCurrentIndex = -1;
		uint32_t	FStartTime;

	public:
		SequenceElement<Sequence<C_CHANNELS_COUNT>> *FElements[ C_CHANNELS_COUNT ];

	public:
		void SignalElement( SequenceElement<Sequence<C_CHANNELS_COUNT>> *AElement )
		{
			if( ! Enabled )
				return;

			if( FCurrentIndex < 0 )
				return;

			SequenceElement<Sequence<C_CHANNELS_COUNT>> *AElementItem = FElements[ FCurrentIndex ];

//			Serial.print( "SignalElement: " );
//			Serial.println( FCurrentIndex );

			if( AElement != AElementItem )
				return;

//			FElements[ FCurrentIndex ]->DoClockReceive( nullptr );
			AElementItem->OutputPin.template SendValue<bool>( false );
			if( ++FCurrentIndex < C_CHANNELS_COUNT )
				FStartTime = millis();

			else if( Repeat )
				StartInputPin_o_Receive( nullptr );

			else
			{
				FCurrentIndex = -1;
				return;
			}

#ifdef _SEQUENCE_DIGIAL_ELEMENT_
			AElementItem = FElements[ FCurrentIndex ];
			if( AElementItem->FIsDigital )
				AElementItem->OutputPin.template SendValue<bool>( true );
#endif // _SEQUENCE_DIGIAL_ELEMENT_


//			Serial.print( "FCurrentIndex: " ); Serial.println( FCurrentIndex );
		}

	public:
		void StartInputPin_o_Receive( void *_Data )
		{
			if( Enabled )
				if( C_CHANNELS_COUNT )
					if( CanRestart || ( FCurrentIndex == -1 ))
					{
						FCurrentIndex = 0;
						FStartTime = millis();
					}

		}

	public:
		inline void SystemStart()
		{
			StartInputPin_o_Receive( nullptr );
		}

		inline void SystemLoopBegin( unsigned long currentMicros )
		{
#ifdef _SEQUENCE_DIGIAL_ELEMENT_
			if( IsEnabled != Enabled )
			{
				SequenceElement<Sequence<C_CHANNELS_COUNT>> *AElementItem = FElements[ FCurrentIndex ];
				if( AElementItem->FIsDigital )
					AElementItem->OutputPin.template SendValue<bool>( Enabled );

				IsEnabled = Enabled;
			}
#endif // _SEQUENCE_DIGIAL_ELEMENT_

			if( Enabled )
				if( FCurrentIndex >= 0 )
					if( ( millis() - FStartTime ) >= FElements[ FCurrentIndex ]->Delay )
						SignalElement( FElements[ FCurrentIndex ] );

/*
			if( FDetectTimerRunning )
				if( ( millis() - FDetectStartTime ) >= PacketDetectInterval )
					TryProcessAcumulattor();

			if( FTimeoutTimerRunning )
				if( ( millis() - FTimeoutStartTime ) >= ResponseTimeout )
				{
					// Timeout
					FDetectTimerRunning = false;

					if( FCurrentTransaction )
						FCurrentTransaction->Release();

					FCurrentTransaction = nullptr;
					FReceivedData.clear();
					TrySend();
				}
*/
		}

	public:
		Sequence() :
			Enabled( true ),
			IsEnabled( true ),
			Repeat( false ),
			CanRestart( true )
		{
		}

	};
//---------------------------------------------------------------------------
}

#ifdef __TEST_FOR_DEBUG_PRINTS__
#undef Serial
#endif

#endif
