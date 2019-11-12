////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2018 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_PLAY_FREQUENCY_TONE_h
#define _MITOV_PLAY_FREQUENCY_TONE_h

#include <Mitov.h>

#ifdef __TEST_FOR_DEBUG_PRINTS__
#define Serial UNGUARDED DEBUG PRINT!!!
#endif

namespace Mitov
{
//---------------------------------------------------------------------------
	class PlayFrequencyTone
	{
	public:
		float	InitialFrequency = 0.0; //100000.0;
		bool	Enabled : 1; // = true;

	protected:
#ifdef VISUINO_ESP32
		bool		FAttached : 1;
		uint8_t		FPin : 6;
#else
		uint8_t		FPin : 7;
#endif

#ifdef VISUINO_ESP32
	protected:
		void noTone( uint8_t APin )
		{
			if( ! FAttached )
				return;

			::Mitov::ToneDetach( APin );
			FAttached = false;
		}

		void tone( uint8_t APin, float AFrequency )
		{
			if( ! AFrequency )
				noTone( APin );

			else
			{
				if( ! FAttached )
				{
					::Mitov::ToneAttach( APin );
					FAttached = true;
				}

				::Mitov::PlayTone( APin, AFrequency );
			}
		}

#endif
	public:
		void UpdateEnabled()
		{
			if( Enabled )
				UpdateFrequency();

			else
				noTone( FPin );
		}

	protected:
		void UpdateFrequency()
		{
			if( InitialFrequency == 0.0 )
				noTone( FPin );

			else
				tone( FPin, InitialFrequency );
		}

	public:
		void InputPin_o_Receive( void *_Data )
		{
			if( ! Enabled )
				return;

			float AValue = *((float *)_Data);
			if( AValue < 0.0 )
				AValue = 0.0;

			if( InitialFrequency == AValue )
				return;

			InitialFrequency = AValue;
			UpdateFrequency();
//			FServo.write( AValue );
		}

	public:
		inline void SystemStart()
		{
			UpdateFrequency();
		}

		inline void SystemLoopBegin( unsigned long currentMicros ) {}

	public:
		PlayFrequencyTone( uint8_t APin ) :
			Enabled( true ),
			FPin( APin )
		{
		}
	};
//---------------------------------------------------------------------------
	class PlayFrequencyToneDuration
	{
	public:
		float	InitialFrequency = 0.0; //100000.0;
		bool	Enabled : 1; // = true;

	protected:
#ifdef VISUINO_ESP32
		bool		FAttached : 1;
		uint8_t		FPin : 6;
		bool		FStarted : 1;

		unsigned long	FLastTime = 0;
#else
		uint8_t		FPin : 7;
#endif
		uint32_t	FCurentDuration = 0;

#ifdef VISUINO_ESP32
	protected:
		void noTone( uint8_t APin )
		{
			if( ! FAttached )
				return;

			::Mitov::ToneDetach( APin );
			FAttached = false;
			FStarted = false;
		}

		void tone( uint8_t APin, float AFrequency )
		{
			FStarted = false;
			if( ! AFrequency )
				noTone( APin );

			else
			{
				if( ! FAttached )
				{
					::Mitov::ToneAttach( APin );
					FAttached = true;
				}

				::Mitov::PlayTone( APin, AFrequency );
			}
		}

		void tone( uint8_t APin, float AFrequency, uint32_t ADuration )
		{
			if( ! AFrequency )
			{
				noTone( APin );
				return;
			}

			FLastTime = millis();
			tone( APin, AFrequency );
			FStarted = true;
		}

#endif
	public:
		void UpdateEnabled()
		{
			if( Enabled )
				UpdateFrequency();

			else
				noTone( FPin );
		}

	protected:
		void UpdateFrequency()
		{
			if( InitialFrequency == 0.0 )
				noTone( FPin );

			else if( FCurentDuration )
				tone( FPin, InitialFrequency, FCurentDuration );

			else
				tone( FPin, InitialFrequency );
		}

	public:
		void ExternalSetValue( float AFrequency, uint32_t ADuration )
		{
			if( ! ADuration )
				if( ( InitialFrequency == AFrequency ) && ( ! FCurentDuration ))
					return;

			InitialFrequency = AFrequency;
			FCurentDuration = ADuration;
			UpdateEnabled();
		}

	public:
		void InputPin_o_Receive( void *_Data )
		{
			if( ! Enabled )
				return;

			float AValue = *((float *)_Data);
			if( AValue < 0.0 )
				AValue = 0.0;

			if( InitialFrequency == AValue )
				return;

			InitialFrequency = AValue;
			FCurentDuration = 0;
			UpdateFrequency();
//			FServo.write( AValue );
		}

	public:
		inline void SystemStart()
		{
			UpdateFrequency();
		}

		inline void SystemLoopBegin( unsigned long currentMicros )
		{
#ifdef VISUINO_ESP32
			if( FStarted )
				if( millis() - FLastTime > FCurentDuration )
					noTone( FPin );
					
#endif
		}

	public:
		PlayFrequencyToneDuration( uint8_t APin ) :
			Enabled( true ),
			FPin( APin )
		{
		}
	};
//---------------------------------------------------------------------------
	template<typename T_OWNER, T_OWNER &C_OWNER> class PlayFrequencyToneSetValueElement
	{
	public:
		float		Frequency = 0.0;
		uint32_t	Duration = 0;

	public:
		inline void InputPin_o_Receive( void *_Data )
		{
			C_OWNER.ExternalSetValue( Frequency, Duration );
		}

	};
//---------------------------------------------------------------------------
	template<typename T_OWNER> class PlayFrequencyToneStateSetValueElementBasic
	{
	public:
		float		Frequency = 0.0;
		uint32_t	Duration = 0;

	protected:
		bool	FInputValue = false;

	public:
		bool TryActive( T_OWNER *AOwner )
		{
			if( FInputValue )
			{
				AOwner->ExternalSetValue( Frequency, Duration );
				return true;
			}

			return false;
		}

	};
//---------------------------------------------------------------------------
	template<typename T_OWNER, T_OWNER &C_OWNER> class PlayFrequencyToneStateSetValueElement : public PlayFrequencyToneStateSetValueElementBasic<T_OWNER>
	{
		typedef PlayFrequencyToneStateSetValueElementBasic<T_OWNER> inherited;

	public:
		void InputPin_o_Receive( void *_Data )
		{
			if( inherited::FInputValue == *(bool *)_Data )
				return;

			inherited::FInputValue = *(bool *)_Data;
			C_OWNER.ResetValue();
		}

	public:
		PlayFrequencyToneStateSetValueElement()
		{
			C_OWNER.RegisterElement( this );
		}
	};
//---------------------------------------------------------------------------
	class PlayFrequencyToneStates
	{
	public:
		float	InitialFrequency = 0.0; //100000.0;
		bool	Enabled : 1; // = true;

	protected:
#ifdef VISUINO_ESP32
		bool		FAttached : 1;
		uint8_t		FPin : 6;
		bool		FStarted : 1;

		unsigned long	FLastTime = 0;
#else
		uint8_t		FPin : 7;
#endif

	protected:
		float		FCurrentFrequency;
		uint32_t	FCurentDuration = 0;

	protected:
		Mitov::SimpleList<PlayFrequencyToneStateSetValueElementBasic<PlayFrequencyToneStates> *>	FElements;

	public:
		void ResetValue()
		{
			for( int i = FElements.size(); i -- ; )
				if( FElements[ i ]->TryActive( this ) )
					return;

			FCurrentFrequency = InitialFrequency;
			FCurentDuration = 0;
			UpdateEnabled();
		}

		void ExternalSetValue( float AFrequency, uint32_t ADuration )
		{
			if( ! ADuration )
				if( ( InitialFrequency == AFrequency ) && ( ! FCurentDuration ))
					return;

			FCurrentFrequency = AFrequency;
			FCurentDuration = ADuration;
			UpdateEnabled();
		}

#ifdef VISUINO_ESP32
	protected:
		void noTone( uint8_t APin )
		{
			if( ! FAttached )
				return;

			::Mitov::ToneDetach( APin );
			FAttached = false;
			FStarted = false;
		}

		void tone( uint8_t APin, float AFrequency )
		{
			FStarted = false;
			if( ! AFrequency )
				noTone( APin );

			else
			{
				if( ! FAttached )
				{
					::Mitov::ToneAttach( APin );
					FAttached = true;
				}

				::Mitov::PlayTone( APin, AFrequency );
			}
		}

		void tone( uint8_t APin, float AFrequency, uint32_t ADuration )
		{
			if( ! AFrequency )
			{
				noTone( APin );
				return;
			}

			FLastTime = millis();
			tone( APin, AFrequency );
			FStarted = true;
		}

#endif
	public:
		inline void RegisterElement( PlayFrequencyToneStateSetValueElementBasic<PlayFrequencyToneStates> *AValue )
		{
			FElements.push_back( AValue );
		}

	public:
		void UpdateEnabled()
		{
			if( Enabled )
				UpdateFrequency();

			else
				noTone( FPin );
		}

	protected:
		void UpdateFrequency()
		{
			if( FCurrentFrequency == 0.0 )
				noTone( FPin );

			else if( FCurentDuration )
				tone( FPin, FCurrentFrequency, FCurentDuration );

			else
				tone( FPin, FCurrentFrequency );
		}

	public:
		void InputPin_o_Receive( void *_Data )
		{
			if( ! Enabled )
				return;

			float AValue = *((float *)_Data);
			if( AValue < 0.0 )
				AValue = 0.0;

			if( InitialFrequency == AValue )
				return;

			InitialFrequency = AValue;
			ResetValue();
//			FServo.write( AValue );
		}

	public:
		inline void SystemStart()
		{
			UpdateFrequency();
		}

		inline void SystemLoopBegin( unsigned long currentMicros )
		{
#ifdef VISUINO_ESP32
			if( FStarted )
				if( millis() - FLastTime > FCurentDuration )
					noTone( FPin );
					
#endif
		}

	public:
		PlayFrequencyToneStates( uint8_t	APin ) :
			Enabled( true ),
			FPin( APin )
		{
		}
	};
//---------------------------------------------------------------------------
}

#ifdef __TEST_FOR_DEBUG_PRINTS__
#undef Serial
#endif

#endif
