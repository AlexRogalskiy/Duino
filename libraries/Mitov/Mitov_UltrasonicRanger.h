////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2018 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_ULTRASONIC_RANGE_h
#define _MITOV_ULTRASONIC_RANGE_h

#include <Mitov.h>
#include <Mitov_MultiSensorStart.h>

namespace Mitov
{
	enum UltrasonicRangerUnits { rdTime, rdCm, rdInch };
//---------------------------------------------------------------------------
	class UltrasonicRanger
	{
	public:
		enum State { sStartUp, sStartDown, sListeningEdgeUp, sListeningEdgeDown, sEchoDetected, sPause };

	public:
		OpenWire::SourcePin	PingOutputPin;
#ifdef _MITOV_ULTRASONIC_RANGE_TIMEOUT_PIN_
		OpenWire::SourcePin	TimeoutOutputPin;
#endif
		OpenWire::SourcePin	OutputPin;

	public:
		uint16_t	Timeout = 1000;
		uint16_t	PingTime = 2;
		uint16_t	PauseTime = 100;
		float		TimeoutValue = -1;

		UltrasonicRangerUnits	Units : 2;

		bool	Enabled : 1;

	public:
		bool	ClockInputPin_o_IsConnected : 1;

	protected:
		State			FState : 3;
		bool			FClocked : 1;

		unsigned long	FStartTime;
		unsigned long	FEchoStartTime;
		unsigned long	FEndTime;

	public:
		void EchoInputPin_o_Receive( void *_Data )
		{
			if( ( FState != sListeningEdgeUp ) && ( FState != sListeningEdgeDown ) )
				return;

			bool AValue = *(bool *)_Data;
			if( FState == sListeningEdgeDown )
			{
				if( AValue )
					return;

				FEndTime = micros();
				FState = sEchoDetected;
			}

			else
			{
			  if( ! AValue )
				  return;

			  FState = sListeningEdgeDown;
			  FEchoStartTime = micros();
			}
		}

	public:
		inline void SystemLoopBegin( unsigned long currentMicros )
		{
			if( !Enabled )
				return;

			switch ( FState )
			{
				case sPause:
				{
					if( ( currentMicros - FStartTime ) < ((unsigned long)PauseTime ) * 1000 )
						return;

#ifdef _MITOV_ULTRASONIC_RANGE_CLOCK_PIN_
					if( ClockInputPin_o_IsConnected )
						if( ! FClocked )
							return;

					FClocked = false;
#endif
				}

				case sStartUp:
				{
//					Serial.println( "start" );
					PingOutputPin.SendValue<bool>( true );
					FStartTime = currentMicros;
					FState = sStartDown;
					break;
				}

				case sStartDown:
				{
					if( ( currentMicros - FStartTime ) < PingTime )
						return;

					PingOutputPin.SendValue<bool>( false );
					FStartTime = currentMicros;
					FState = sListeningEdgeUp;
					break;
				}

				case sListeningEdgeUp:
				case sListeningEdgeDown:
				{
					if( ( currentMicros - FStartTime ) < ((unsigned long)Timeout ) * 1000 )
						return;

					OutputPin.Notify( &TimeoutValue );
#ifdef _MITOV_ULTRASONIC_RANGE_TIMEOUT_PIN_
					TimeoutOutputPin.SendValue<bool>( true );
#endif
					FState = sPause;
					break;
				}

				case sEchoDetected:
				{
					unsigned long APeriod = FEndTime - FEchoStartTime;
					float AValue;
					switch( Units )
					{
						case rdTime:
							AValue = APeriod;
							break;

						case rdCm:
							AValue = ((float)APeriod) / 29 / 2; // / 58.2;
							break;

						case rdInch:
							AValue = ((float)APeriod) / 74 / 2;
							break;
					}

					OutputPin.Notify( &AValue );
#ifdef _MITOV_ULTRASONIC_RANGE_TIMEOUT_PIN_
					TimeoutOutputPin.SendValue<bool>( false );
#endif
					FState = sPause;
					break;
				}
			}
		}

	public:
		inline void ClockInputPin_o_Receive( void *_Data )
		{
			FClocked = true;
		}

	public:
		UltrasonicRanger() :
			FState( sStartUp ),
			Enabled( true ),
			Units( rdCm ),
			FClocked( false )
		{
		}

	};
//---------------------------------------------------------------------------
	class UltrasonicRanger_Interrupt : public OpenWire::InterruptObject
	{
	public:
		OpenWire::SourcePin	PingOutputPin;
#ifdef _MITOV_ULTRASONIC_RANGE_TIMEOUT_PIN_
		OpenWire::SourcePin	TimeoutOutputPin;
#endif
		OpenWire::SourcePin	OutputPin;

	public:
		uint16_t	Timeout = 1000;
		uint16_t	PingTime = 2;
		uint16_t	PauseTime = 100;
		float   TimeoutValue = -1;

		UltrasonicRangerUnits	Units : 2;

		bool	Enabled : 1;

	public:
		bool	ClockInputPin_o_IsConnected : 1;

	protected:
		UltrasonicRanger::State			FState : 3;
		bool			FClocked : 1;

		unsigned long	FStartTime;
		unsigned long	FEchoStartTime;
		unsigned long	FEndTime;

	public:
		void InterruptHandler( bool AValue )
		{
			if( ( FState != UltrasonicRanger::sListeningEdgeUp ) && ( FState != UltrasonicRanger::sListeningEdgeDown ) )
				return;

			if( FState == UltrasonicRanger::sListeningEdgeDown )
			{
				if( AValue )
					return;

				FEndTime = micros();
				FState = UltrasonicRanger::sEchoDetected;
			}

			else
			{
			  if( ! AValue )
				  return;

			  FState = UltrasonicRanger::sListeningEdgeDown;
			  FEchoStartTime = micros();
			}
		}

	public:
		inline void SystemLoopBegin( unsigned long currentMicros )
		{
			if( !Enabled )
				return;

			switch ( FState )
			{
				case UltrasonicRanger::sPause:
				{
					if( ( currentMicros - FStartTime ) < ((unsigned long)PauseTime ) * 1000 )
						return;

#ifdef _MITOV_ULTRASONIC_RANGE_CLOCK_PIN_
					if( ClockInputPin_o_IsConnected )
						if( ! FClocked )
							return;

					FClocked = false;
#endif
				}

				case UltrasonicRanger::sStartUp:
				{
//					Serial.println( "start" );
					PingOutputPin.SendValue<bool>( true );
					FStartTime = currentMicros;
					FState = UltrasonicRanger::sStartDown;
					break;
				}

				case UltrasonicRanger::sStartDown:
				{
					if( ( currentMicros - FStartTime ) < PingTime )
						return;

					PingOutputPin.SendValue<bool>( false );
					FStartTime = currentMicros;
					FState = UltrasonicRanger::sListeningEdgeUp;
					break;
				}

				case UltrasonicRanger::sListeningEdgeUp:
				case UltrasonicRanger::sListeningEdgeDown:
				{
					if( ( currentMicros - FStartTime ) < ((unsigned long)Timeout ) * 1000 )
						return;

					OutputPin.Notify( &TimeoutValue );
#ifdef _MITOV_ULTRASONIC_RANGE_TIMEOUT_PIN_
					TimeoutOutputPin.SendValue<bool>( true );
#endif
					FState = UltrasonicRanger::sPause;
					break;
				}

				case UltrasonicRanger::sEchoDetected:
				{
					unsigned long APeriod = FEndTime - FEchoStartTime;
					float AValue;
					switch( Units )
					{
						case rdTime:
							AValue = APeriod;
							break;

						case rdCm:
							AValue = ((float)APeriod) / 29 / 2; // / 58.2;
							break;

						case rdInch:
							AValue = ((float)APeriod) / 74 / 2;
							break;
					}

					OutputPin.Notify( &AValue );
#ifdef _MITOV_ULTRASONIC_RANGE_TIMEOUT_PIN_
					TimeoutOutputPin.SendValue<bool>( false );
#endif
					FState = UltrasonicRanger::sPause;
					break;
				}
			}
		}

	public:
		inline void ClockInputPin_o_Receive( void *_Data )
		{
			FClocked = true;
		}

	public:
		UltrasonicRanger_Interrupt() :
			FState( UltrasonicRanger::sStartUp ),
			Enabled( true ),
			Units( rdCm ),
			FClocked( false )
		{
		}
	};
//---------------------------------------------------------------------------
	class UltrasonicRangerElement : public MultiSensorStartBasicElement
	{
		typedef MultiSensorStartBasicElement inherited;

	public:
#ifdef _MITOV_ULTRASONIC_RANGE_TIMEOUT_PIN_
		OpenWire::SourcePin	TimeoutOutputPin;
#endif
		OpenWire::SourcePin	OutputPin;

	public:
		uint16_t	Timeout = 1000;
		float   TimeoutValue = -1;

		UltrasonicRangerUnits	Units : 2;

		bool	Enabled : 1;

	protected:
		UltrasonicRanger::State			FState : 3;

		unsigned long	FStartTime;
		unsigned long	FEchoStartTime;
		unsigned long	FEndTime;

	public:
		void EchoInputPin_o_Receive( void *_Data )
		{
//			Serial.print( "TEST: " ); Serial.println( *(bool *)_Data );
//			Serial.println( FState );
			if( ( FState != UltrasonicRanger::sListeningEdgeUp ) && ( FState != UltrasonicRanger::sListeningEdgeDown ) )
				return;

			bool AValue = *(bool *)_Data;
			if( FState == UltrasonicRanger::sListeningEdgeDown )
			{
				if( AValue )
					return;

				FEndTime = micros();
				FState = UltrasonicRanger::sEchoDetected;
//				Serial.println( "DETECTED" );
			}

			else
			{
			  if( ! AValue )
				  return;

			  FState = UltrasonicRanger::sListeningEdgeDown;
			  FEchoStartTime = micros();
			}
		}

	public:
		virtual bool StartElement() override
		{
//			Serial.println( "START" );
			FStartTime = micros();
			FState = UltrasonicRanger::sListeningEdgeUp;
			return true;
		}

	public:
		inline void SystemLoopBegin( unsigned long currentMicros )
		{
			if( !Enabled )
				return;

			switch ( FState )
			{
/*
				case UltrasonicRanger::sStartUp:
				{
//					Serial.println( "start" );
					FStartTime = currentMicros;
					FState = UltrasonicRanger::sStartDown;
					break;
				}

				case UltrasonicRanger::sStartDown:
				{
					if( ( currentMicros - FStartTime ) < PingTime )
						return;

					bool AValue = false;
					PingOutputPin.Notify( &AValue );
					FStartTime = currentMicros;
					FState = UltrasonicRanger::sListeningEdgeUp;
					break;
				}
*/
				case UltrasonicRanger::sListeningEdgeUp:
				case UltrasonicRanger::sListeningEdgeDown:
				{
					currentMicros = micros();
					if( ( currentMicros - FStartTime ) < ((unsigned long)Timeout ) * 1000 )
						return;

					FState = UltrasonicRanger::sPause;
					OutputPin.Notify( &TimeoutValue );
#ifdef _MITOV_ULTRASONIC_RANGE_TIMEOUT_PIN_
					TimeoutOutputPin.SendValue<bool>( true );
#endif
//					Serial.println( "TIMEOUT" );
//					Serial.println( FStartTime );
//					Serial.println( currentMicros - FStartTime );
					FOwner.ElementReady( this );
					break;
				}

				case UltrasonicRanger::sEchoDetected:
				{
					unsigned long APeriod = FEndTime - FEchoStartTime;
					float AValue;
					switch( Units )
					{
						case rdTime:
							AValue = APeriod;
							break;

						case rdCm:
							AValue = ((float)APeriod) / 29 / 2; // / 58.2;
							break;

						case rdInch:
							AValue = ((float)APeriod) / 74 / 2;
							break;
					}

					FState = UltrasonicRanger::sPause;
					OutputPin.Notify( &AValue );
#ifdef _MITOV_ULTRASONIC_RANGE_TIMEOUT_PIN_
					TimeoutOutputPin.SendValue<bool>( false );
#endif
//					Serial.println( "VALUE" );
					FOwner.ElementReady( this );
					break;
				}
			}
		}

	public:
		UltrasonicRangerElement( MultiSensorPulseStart &AOwner ) :
			inherited( AOwner ),
			FState( UltrasonicRanger::sStartUp ),
			Enabled( true ),
			Units( rdCm )
		{
		}

	};
//---------------------------------------------------------------------------
	class UltrasonicRangerElement_Interrupt : public MultiSensorStartBasicElement
	{
		typedef MultiSensorStartBasicElement inherited;

	public:
		OpenWire::SourcePin	PingOutputPin;
#ifdef _MITOV_ULTRASONIC_RANGE_TIMEOUT_PIN_
		OpenWire::SourcePin	TimeoutOutputPin;
#endif
		OpenWire::SourcePin	OutputPin;

	public:
		uint16_t	Timeout = 1000;
		float   TimeoutValue = -1;

		UltrasonicRangerUnits	Units : 2;

		bool	Enabled : 1;

	protected:
		UltrasonicRanger::State			FState : 3;
		unsigned long	FStartTime;
		unsigned long	FEchoStartTime;
		unsigned long	FEndTime;

	public:
		void InterruptHandler( bool AValue )
		{
			if( ( FState != UltrasonicRanger::sListeningEdgeUp ) && ( FState != UltrasonicRanger::sListeningEdgeDown ) )
				return;

			if( FState == UltrasonicRanger::sListeningEdgeDown )
			{
				if( AValue )
					return;

				FEndTime = micros();
				FState = UltrasonicRanger::sEchoDetected;
			}

			else
			{
			  if( ! AValue )
				  return;

			  FState = UltrasonicRanger::sListeningEdgeDown;
			  FEchoStartTime = micros();
			}
		}

	public:
		virtual bool StartElement() override
		{
//			Serial.println( "START" );
			FStartTime = micros();
			FState = UltrasonicRanger::sListeningEdgeUp;
			return true;
		}

	public:
		inline void SystemLoopBegin( unsigned long currentMicros )
		{
			if( !Enabled )
				return;

			switch ( FState )
			{
/*
				case UltrasonicRanger::sStartUp:
				{
//					Serial.println( "start" );
					PingOutputPin.SendValue<bool>( true );
					FStartTime = currentMicros;
					FState = UltrasonicRanger::sStartDown;
					break;
				}

				case UltrasonicRanger::sStartDown:
				{
					if( ( currentMicros - FStartTime ) < PingTime )
						return;

					PingOutputPin.SendValue<bool>( false );
					FStartTime = currentMicros;
					FState = UltrasonicRanger::sListeningEdgeUp;
					break;
				}
*/
				case UltrasonicRanger::sListeningEdgeUp:
				case UltrasonicRanger::sListeningEdgeDown:
				{
					currentMicros = micros();
					if( ( currentMicros - FStartTime ) < ((unsigned long)Timeout ) * 1000 )
						return;

					FState = UltrasonicRanger::sPause;
					OutputPin.Notify( &TimeoutValue );
#ifdef _MITOV_ULTRASONIC_RANGE_TIMEOUT_PIN_
					TimeoutOutputPin.SendValue<bool>( true );
#endif
//					Serial.println( "TIMEOUT" );
//					Serial.println( FStartTime );
//					Serial.println( currentMicros - FStartTime );
//					FState = UltrasonicRanger::sPause;
					FOwner.ElementReady( this );
					break;
				}

				case UltrasonicRanger::sEchoDetected:
				{
					unsigned long APeriod = FEndTime - FEchoStartTime;
					float AValue;
					switch( Units )
					{
						case rdTime:
							AValue = APeriod;
							break;

						case rdCm:
							AValue = ((float)APeriod) / 29 / 2; // / 58.2;
							break;

						case rdInch:
							AValue = ((float)APeriod) / 74 / 2;
							break;
					}

					FState = UltrasonicRanger::sPause;
					OutputPin.Notify( &AValue );
#ifdef _MITOV_ULTRASONIC_RANGE_TIMEOUT_PIN_
					TimeoutOutputPin.SendValue<bool>( false );
#endif
//					FState = UltrasonicRanger::sPause;
					FOwner.ElementReady( this );
					break;
				}
			}
		}

	public:
		UltrasonicRangerElement_Interrupt( MultiSensorPulseStart &AOwner ) :
			inherited( AOwner ),
			FState( UltrasonicRanger::sStartUp ),
			Enabled( true ),
			Units( rdCm )
		{
		}
	};
//---------------------------------------------------------------------------
	template<int PIN_PING, int PIN_ECHO> class UltrasonicRangerShieldElement
	{
	public:
		enum State { sStartUp, sStartDown, sListeningEdgeUp, sListeningEdgeDown, sEchoDetected, sPause };

	public:
#ifdef _MITOV_SHIELD_ELEMENT_ULTRASONIC_RANGE_TIMEOUT_PIN_
		OpenWire::SourcePin	TimeoutOutputPin;
#endif
		OpenWire::SourcePin	OutputPin;

	public:
		uint16_t	Timeout = 1000;
		uint16_t	PingTime = 2;
		uint16_t	PauseTime = 100;
		float		TimeoutValue = -1;

		UltrasonicRangerUnits	Units : 2;

		bool	Enabled : 1;

	protected:
		State			FState : 3;
		bool			FClocked : 1;

		unsigned long	FStartTime;
		unsigned long	FEchoStartTime;
		unsigned long	FEndTime;

	protected:
		void DoReceive( bool AValue )
		{
//			Serial.println( AValue );
			if( ( FState != sListeningEdgeUp ) && ( FState != sListeningEdgeDown ) )
				return;

			if( FState == sListeningEdgeDown )
			{
				if( AValue )
					return;

				FEndTime = micros();
				FState = sEchoDetected;
			}

			else
			{
			  if( ! AValue )
				  return;

			  FState = sListeningEdgeDown;
			  FEchoStartTime = micros();
			}
		}

	public:
		inline void SystemLoopBegin( unsigned long currentMicros )
		{
			if( ! Enabled )
				return;

			bool AValue = ( digitalRead( PIN_ECHO ) == HIGH ); // Echo
			DoReceive( AValue );

			switch ( FState )
			{
				case sPause:
				{
					if( ( currentMicros - FStartTime ) < ((unsigned long)PauseTime ) * 1000 )
						return;

#ifdef _MITOV_SHIELD_ELEMENT_ULTRASONIC_RANGE_CLOCK_PIN_
					if( ClockInputPin_o_IsConnected )
						if( ! FClocked )
							return;

					FClocked = false;
#endif
				}

				case sStartUp:
				{
//					Serial.println( "start" );
					digitalWrite( PIN_PING, true ); // Ping
//					PingOutputPin.SendValue<bool>( true );
					FStartTime = currentMicros;
					FState = sStartDown;
					break;
				}

				case sStartDown:
				{
					if( ( currentMicros - FStartTime ) < PingTime )
						return;

					digitalWrite( PIN_PING, false ); // Ping
//					PingOutputPin.SendValue<bool>( false );
					FStartTime = currentMicros;
					FState = sListeningEdgeUp;
					break;
				}

				case sListeningEdgeUp:
				case sListeningEdgeDown:
				{
					if( ( currentMicros - FStartTime ) < ((unsigned long)Timeout ) * 1000 )
						return;

					OutputPin.Notify( &TimeoutValue );
#ifdef _MITOV_SHIELD_ELEMENT_ULTRASONIC_RANGE_TIMEOUT_PIN_
					TimeoutOutputPin.SendValue<bool>( true );
#endif
					FState = sPause;
					break;
				}

				case sEchoDetected:
				{
					unsigned long APeriod = FEndTime - FEchoStartTime;
					float AValue;
					switch( Units )
					{
						case rdTime:
							AValue = APeriod;
							break;

						case rdCm:
							AValue = ((float)APeriod) / 29 / 2; // / 58.2;
							break;

						case rdInch:
							AValue = ((float)APeriod) / 74 / 2;
							break;
					}

					OutputPin.Notify( &AValue );
#ifdef _MITOV_SHIELD_ELEMENT_ULTRASONIC_RANGE_TIMEOUT_PIN_
					TimeoutOutputPin.SendValue<bool>( false );
#endif
					FState = sPause;
					break;
				}
			}
		}

	public:
		inline void ClockInputPin_o_Receive( void *_Data )
		{
			FClocked = true;
		}

	public:
		UltrasonicRangerShieldElement() :
			FState( sStartUp ),
			Enabled( true ),
			Units( rdCm )
#ifdef _MITOV_SHIELD_ELEMENT_ULTRASONIC_RANGE_CLOCK_PIN_
			,
			FClocked( false )
#endif
		{
			pinMode( PIN_PING, OUTPUT ); // Ping
		}

	};
//---------------------------------------------------------------------------
}

#endif
