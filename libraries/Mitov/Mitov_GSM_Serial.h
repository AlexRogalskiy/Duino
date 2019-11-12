////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2018 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_GSM_SERIAL_h
#define _MITOV_GSM_SERIAL_h

#include <Mitov.h>

//#define __SIMGSM__DEBUG__

#ifdef __TEST_FOR_DEBUG_PRINTS__
#define Serial UNGUARDED DEBUG PRINT!!!
#endif

namespace Mitov
{
//---------------------------------------------------------------------------
//	class MitovGSMSerial;
//	class MitovGSMSerialVoiceCallFunction;
	template<typename T_OWNER, T_OWNER &C_OWNER> class MitovGSMSerialSMSMessageReceivedFunction;
//	class MitovGSMSerialSMSModule;
//	class MitovGSMSerialSMSBasicMessageFunction;
	template<typename T_OWNER, T_OWNER &C_OWNER> class MitovGSMSerialSMSModule;
//---------------------------------------------------------------------------
	namespace GSMFinc
	{
		bool ExtractTimeStamp( Mitov::String ATimeStamp, TDateTime &ADateTime, int32_t &ATimeZone )
		{
			if( ATimeStamp.length() < 17 )
				return false;

			Mitov::String AText = ATimeStamp.substring( 0, 2 );
			int16_t AYear = 2000 + AText.toInt();

			AText = ATimeStamp.substring( 3, 5 );
			int16_t AMonth = AText.toInt();

			AText = ATimeStamp.substring( 6, 8 );
			int16_t ADay = AText.toInt();

			AText = ATimeStamp.substring( 9, 11 );
			int16_t AHour = AText.toInt();

			AText = ATimeStamp.substring( 12, 14 );
			int16_t AMinute = AText.toInt();

			AText = ATimeStamp.substring( 15, 17 );
			int16_t ASecond = AText.toInt();

			AText = ATimeStamp.substring( 17, 20 );
			ATimeZone = AText.toInt();

			return ADateTime.TryEncodeDateTime( AYear, AMonth, ADay, AHour, AMinute, ASecond, 0 );
		}
	}
//---------------------------------------------------------------------------
	template<typename T_OWNER> class MitovGSMSerialBasicFunction : public OpenWire::Object
	{
	public:
		T_OWNER &FOwner;

	public:
		virtual void ElementSystemStart() {} // Make sure it is different than SystemStart
		virtual bool TryProcessPromptInput( bool &ASuccess ) { ASuccess = false; return false; }
		virtual bool TryProcessRequestedInput( Mitov::String ALine, bool &ALockInput, bool &AResponseCompleted, bool &ASuccess ) { return false; }
		virtual bool TryProcessInput( Mitov::String ALine, bool &ALockInput ) { return false; }
		virtual bool ProcessCharacter( char AChar, bool &ASuccess ) { return false; }
		virtual void TimedOut() {}

	public:
		MitovGSMSerialBasicFunction( T_OWNER &AOwner ) :
			FOwner( AOwner )
		{
			AOwner.AddFunction( this );
		}

		~MitovGSMSerialBasicFunction()
		{
			FOwner.RemoveFunction( this );
		}
	};
//---------------------------------------------------------------------------
	template<typename T_OWNER> class MitovGSMSerialBasicExpectOKFunction : public MitovGSMSerialBasicFunction<T_OWNER>
	{
		typedef MitovGSMSerialBasicFunction<T_OWNER> inherited;

	protected:
		bool	FEmptyLineDetected = false;

	public:
		void Reset()
		{
			FEmptyLineDetected = false;
		}

/*
	protected:
		virtual void OKReceived()
		{
		}
*/
	public:
		virtual bool TryProcessRequestedInput( Mitov::String ALine, bool &ALockInput, bool &AResponseCompleted, bool &ASuccess ) override
		{
//			Serial.println( "TryProcessRequestedInput" );
//			Serial.println( ALine );
			if( FEmptyLineDetected )
			{
				ALine.trim();
				if( ALine != "" )
				{
					ASuccess = ( ALine == "OK" );
/*
					if( ASuccess )
					{
//						Serial.println( "OK Processed" );
		//					Serial.println( "ALockInput = false" );
						OKReceived();
					}
*/
					AResponseCompleted = true;
					FEmptyLineDetected = false;
					ALockInput = false;
					return true;
				}
			}

			else if( ALine == "" )
			{
//				Serial.println( "FEmptyLineDetected" );
				FEmptyLineDetected = true;
				return true;
			}

			return false;
		}

	public:
		using inherited::inherited;


	};
//---------------------------------------------------------------------------
	typedef void (OpenWire::Object::*TOnMitovGSMSerialFunctionResult) ( bool AResult );
	typedef bool (OpenWire::Object::*TOnMitovGSMSerialFunctionLine) ( Mitov::String ALine );
//---------------------------------------------------------------------------
	template<typename T_OWNER> class TMitovGSMSerialFunctionExpectOk : public MitovGSMSerialBasicFunction<T_OWNER>
	{
		typedef MitovGSMSerialBasicFunction<T_OWNER> inherited;

	protected:
		OpenWire::Object *FOwnerFunction;
		TOnMitovGSMSerialFunctionResult FCallback;

	protected:
		bool	FLocked = false;
		bool	FEmptyLineDetected = false;
		Mitov::String	FLockStringStart;

	public:
		void Reset()
		{
			FEmptyLineDetected = false;
		}

	public:
		virtual bool TryProcessRequestedInput( Mitov::String ALine, bool &ALockInput, bool &AResponseCompleted, bool &ASuccess ) override
		{
//			Serial.println( "TryProcessRequestedInput" );
//			Serial.println( ALine );
			if( FEmptyLineDetected )
			{
				ALine.trim();
#ifdef __SIMGSM__DEBUG__
				Serial.print( "RES1: " ); Serial.println( ALine );
#endif
				ASuccess = ( ALine == "OK" );
				if( ASuccess || ( ALine == "ERROR" ))
				{
					( FOwnerFunction->*FCallback )( ALine == "OK" );

					AResponseCompleted = true;
					FEmptyLineDetected = false;
					ALockInput = false;
					return true;
				}
			}

			else if( ALine == "" )
			{
//				Serial.println( "FEmptyLineDetected" );
				FEmptyLineDetected = true;
				return true;
			}

			else
			{
				ALine.trim();
	//			Serial.print( "SHORT_LINE: " ); Serial.println( AShortLine );
				if( ALine.startsWith( FLockStringStart ))
				{
					ALockInput = true;
					FLocked = true;
	//				Serial.println( "+CMGR: DETECTED!!!"  );
				}
			}

			return false;
		}

	public:
		TMitovGSMSerialFunctionExpectOk( T_OWNER &AOwner, OpenWire::Object *AOwnerFunction, TOnMitovGSMSerialFunctionResult ACallback, Mitov::String ALockStringStart ) :
			inherited( AOwner ),
			FOwnerFunction( AOwnerFunction ),
			FCallback( ACallback )
		{
		}

	};
	//---------------------------------------------------------------------------
	template<typename T_OWNER> class TMitovGSMSerialFunctionStatus : public MitovGSMSerialBasicFunction<T_OWNER>
	{
		typedef MitovGSMSerialBasicFunction<T_OWNER> inherited;

	protected:
		OpenWire::Object *FOwnerFunction;
		TOnMitovGSMSerialFunctionResult FCallback;

	protected:
		bool	FEmptyLineDetected = false;

	public:
		void Reset()
		{
			FEmptyLineDetected = false;
		}

	public:
		virtual void TimedOut() override
		{
			if( FCallback )
				( FOwnerFunction->*FCallback )( false );
		}

		virtual bool TryProcessRequestedInput( Mitov::String ALine, bool &ALockInput, bool &AResponseCompleted, bool &ASuccess ) override
		{
//			Serial.println( "TryProcessRequestedInput" );
//			Serial.println( ALine );
			if( FEmptyLineDetected )
			{
				ALine.trim();
#ifdef __SIMGSM__DEBUG__
				Serial.print( "RES1: " ); Serial.println( ALine );
#endif
				ASuccess = ( ALine == "OK" );
				if( ASuccess || ( ALine == "ERROR" ))
				{
					if( FCallback )
						( FOwnerFunction->*FCallback )( ALine == "OK" );

					AResponseCompleted = true;
					FEmptyLineDetected = false;
					ALockInput = false;
					return true;
				}
			}

			else if( ALine == "" )
			{
//				Serial.println( "FEmptyLineDetected" );
				FEmptyLineDetected = true;
				return true;
			}

			return false;
		}

	public:
/*
		TMitovGSMSerialFunctionStatus( MitovGSMSerialBasicFunction<T_OWNER> *AOwnerFunction, TOnMitovGSMSerialFunctionResult ACallback ) :
			inherited( AOwnerFunction->FOwner ),
			FOwnerFunction( AOwnerFunction ),
			FCallback( ACallback )
		{
		}
*/
		TMitovGSMSerialFunctionStatus( T_OWNER &AOwner, OpenWire::Object *AOwnerFunction, TOnMitovGSMSerialFunctionResult ACallback ) :
			inherited( AOwner ),
			FOwnerFunction( AOwnerFunction ),
			FCallback( ACallback )
		{
		}

	};
//---------------------------------------------------------------------------
	class TArduinoGSMSerialDelays
	{
	public:
		uint32_t	PowerPulse = 2000000;
		uint32_t	Shutdown = 10000000;
		uint32_t	Startup = 20000000;
	};
//---------------------------------------------------------------------------
	template<typename T_SERIAL, T_SERIAL &C_SERIAL> class MitovGSMSerial : public OpenWire::Object
	{
	protected:
		struct TResponseElement
		{
		public:
			MitovGSMSerialBasicFunction<MitovGSMSerial<T_SERIAL, C_SERIAL>> *Handler;
			bool Delete;
			uint32_t Timeout;
			Mitov::String Command;
			uint8_t RetryCount;

		public:
			TResponseElement() :
				Handler( nullptr ),
				Delete( false ),
				Timeout( 0 )
			{
			}

			TResponseElement( MitovGSMSerialBasicFunction<MitovGSMSerial<T_SERIAL, C_SERIAL>> *AHandler, bool ADelete, uint32_t ATimeout, Mitov::String ACommand, uint8_t ARetryCount ) :
				Handler( AHandler ),
				Delete( ADelete ),
				Timeout( ATimeout ),
				Command( ACommand ),
				RetryCount( ARetryCount )
			{
#ifdef __SIMGSM__DEBUG__
				Serial.print( "TResponseElement: " ); Serial.println( Command );
#endif
			}

			~TResponseElement()
			{
				if( Delete )
					delete Handler;
			}

		};

		struct TQueryElement
		{
		public:
			Mitov::String Command;
			TResponseElement *ResponseElement;

		public:
			TQueryElement() :
				ResponseElement( nullptr )
			{
			}

			TQueryElement( Mitov::String ACommand, TResponseElement *AResponseElement ) :
				Command( ACommand ),
				ResponseElement( AResponseElement )
			{
			}

		};


	public:
		OpenWire::SourcePin	PowerOutputPin;
		OpenWire::SourcePin	ReadyOutputPin;

	public:
		TArduinoGSMSerialDelays	Delays;

		bool	PowerOn : 1; // = true;

	public:
		bool	IsStarted : 1; // = false;

	protected:
		bool	FPowerChecked : 1; // = false;
		bool	FInPowerCheckWait : 1; // = false;
		bool	FInPowerSwitch : 1; // = false;
		bool	FInPowerSwitchDelay : 1; // = false;
		bool	FInStartupWait : 1; // = false;
		TResponseElement *FCurrentElement = nullptr;
		unsigned long FLastTime = 0;
		unsigned long FCurrentMillis = 0;

	public:
		void SetPowerOn( bool AValue )
		{
			if( PowerOn == AValue )
				return;

			PowerOn = AValue;
			if( !PowerOn )
				IsStarted = false;
//			Serial.println( "SetPowerOn" );
//			Serial.println( PowerOn );
			QueryPowerOn();
		}

	protected:
		Mitov::SimpleList<MitovGSMSerialBasicFunction<MitovGSMSerial<T_SERIAL, C_SERIAL>> *>	FFunctions;
		Mitov::SimpleList<TResponseElement *>				FResponseHandlersQueue;
		Mitov::SimpleList<TQueryElement>					FQueryQueue;

	public:
		void	*FSocketConnections[ 7 ];

	public:
		Stream &GetStream() { C_SERIAL.GetStream(); }

	public:
		void AddFunction( MitovGSMSerialBasicFunction<MitovGSMSerial<T_SERIAL, C_SERIAL>> *AFunction )
		{
			FFunctions.push_back( AFunction );
		}

		void RemoveFunction( MitovGSMSerialBasicFunction<MitovGSMSerial<T_SERIAL, C_SERIAL>> *AFunction )
		{
			FFunctions.erase( AFunction );
		}

	public:
		void SendDirect( void *AData, int ASize )
		{
#ifdef __SIMGSM__DEBUG__
			Serial.print( "SENDING : " ); Serial.println( ASize );

			char *APtr = (char *)AData;
			for( int i = 0; i < ASize; ++i )
				Serial.print( *APtr++ );
#endif

			C_SERIAL.GetStream().write( (uint8_t *)AData, ASize );
		}

		void SendQuery( Mitov::String AQuery, TResponseElement *AElement = nullptr )
		{
#ifdef __SIMGSM__DEBUG__
			Serial.print( "QUERY : \"" );	Serial.print( AQuery ); Serial.println( "\"" );
#endif
			FCurrentMillis = millis();

			C_SERIAL.GetStream().println( AQuery );
			FCurrentElement = AElement;
		}

		void TrySendQuery( Mitov::String AQuery, TResponseElement *AElement )
		{
			if( ( PowerOn || ( AQuery == "AT" ) ) && ( FResponseHandlersQueue.size() == 0 ) && ( !FInPowerSwitch ))
				SendQuery( AQuery, AElement );

			else
			{
#ifdef __SIMGSM__DEBUG__
				Serial.print( "ADD TO QUERY : \"" );	Serial.print( AQuery ); Serial.println( "\"" );
#endif
				FQueryQueue.push_back( TQueryElement( AQuery, AElement ));

#ifdef __SIMGSM__DEBUG__
				Serial.println( "QUEUE>>" );
				for( int i = 0; i < FQueryQueue.size(); ++i )
					Serial.println( FQueryQueue[ i ].Command );
				Serial.println( "<<QUEUE" );
#endif
			}
		}

		void SendQueryRegisterResponse( MitovGSMSerialBasicFunction<MitovGSMSerial<T_SERIAL, C_SERIAL>> *ASender, Mitov::String AQuery, bool ADelete = false, uint8_t ARetryCount = 0, uint32_t ATimeout = 10000 )
		{
#ifdef __SIMGSM__DEBUG__
			Serial.print( "PUSHING: " ); Serial.println( AQuery );
#endif
			TResponseElement *AElement = new TResponseElement( ASender, ADelete, ATimeout, ( ARetryCount ) ? AQuery : "", ARetryCount );
			TrySendQuery( AQuery, AElement );
			FResponseHandlersQueue.push_back( AElement );
			
//			SendQuery( AQuery );
//			Serial.print( "PUSHING: " ); Serial.println( AQuery );
//			Serial.println( "PUSHED" );
		}

		void AbortResponseHandler( MitovGSMSerialBasicFunction<MitovGSMSerial<T_SERIAL, C_SERIAL>> *ASender )
		{
			if( FLockRequestedInputIndex )
				if( FResponseHandlersQueue[ FLockRequestedInputIndex - 1 ]->Handler == ASender)
					FLockRequestedInputIndex = 0;

#ifdef __SIMGSM__DEBUG__
			Serial.println( "FResponseHandlersQueue.erase" );
#endif
			for( int i = 0; i < FResponseHandlersQueue.size(); ++i )
				if( FResponseHandlersQueue[ i ]->Handler == ASender )
				{
					if( FCurrentElement == FResponseHandlersQueue[ i ] )
						FCurrentElement =nullptr;

					delete FResponseHandlersQueue[ i ];
					FResponseHandlersQueue.Delete( i );
					break;
				}

//			FResponseHandlersQueue.erase( ASender );
		}

	public:
		TMitovGSMSerialFunctionStatus<MitovGSMSerial<T_SERIAL, C_SERIAL>>	FPowerOnFunction;

	protected:
		char		FBuffer[ 256 ];
		uint8_t		FIndex = 0;
		uint8_t		FLockInputIndex = 0;
		uint8_t		FLockRequestedInputIndex = 0;

	protected:
		void ProcessNextCommand()
		{
//			Serial.println( "ProcessNextCommand" );
//				Serial.print( "RESP_QUEUE: " ); Serial.println( FResponseHandlersQueue.size() );
			if( FQueryQueue.size() )
			{
//					Serial.print( "SEND_QUERY: " ); Serial.println( FQueryQueue.size() );
				TQueryElement AElement = FQueryQueue[ 0 ];
//					Serial.print( "ESTRACT_QUERY: " ); Serial.println( ACommand );
				FQueryQueue.pop_front();

#ifdef __SIMGSM__DEBUG__
				Serial.println( "QUEUE>>" );
				for( int i = 0; i < FQueryQueue.size(); ++i )
					Serial.println( FQueryQueue[ i ].Command );
				Serial.println( "<<QUEUE" );
#endif

				SendQuery( AElement.Command, AElement.ResponseElement );
//				Serial.print( "SEND_QUERY: " ); Serial.println( FQueryQueue.size() );
			}
		}

		void ReadSerial()
		{
//			if( FInPowerSwitch )
//				return;

			int AChar = C_SERIAL.GetStream().read();
//			Serial.print( AChar );
			if( AChar < 0 )
				return;

			if( FLockInputIndex )
			{
				bool ASuccess = false;
				if( FFunctions[ FLockInputIndex - 1 ]->ProcessCharacter( AChar, ASuccess ))
				{
					if( ASuccess )
						FLockInputIndex = 0;

					return;
				}
			}

//			Serial.print( (char)AChar );
//			if( AChar < ' ' )
//				Serial.println( AChar );

			if( AChar == 13 )
				return;

			if( AChar != 10 )
			{
				if( FIndex == 0 )
					if( AChar == '>' )
						for( int i = 0; i < FResponseHandlersQueue.size(); ++i )
						{
							bool ASuccess = false;
							if( FResponseHandlersQueue[ i ]->Handler->TryProcessPromptInput( ASuccess ))
							{
								if( FCurrentElement == FResponseHandlersQueue[ i ] )
									FCurrentElement = nullptr;

								if( ( ! ASuccess ) && FResponseHandlersQueue[ i ]->Command && FResponseHandlersQueue[ i ]->RetryCount )
								{
#ifdef __SIMGSM__DEBUG__
									Serial.println( "RETRY5" );
									Serial.println( FResponseHandlersQueue[ i ]->Command );
#endif
									-- FResponseHandlersQueue[ i ]->RetryCount;
									SendQuery( FResponseHandlersQueue[ i ]->Command, FResponseHandlersQueue[ i ] );
								}

								else
								{
#ifdef __SIMGSM__DEBUG__
									Serial.println( "Queue Delete 5" );
		//							Serial.println( ALockInput );
		//							Serial.print( "RESP_QUEUE: " ); Serial.println( FResponseHandlersQueue.size() );
#endif
									delete FResponseHandlersQueue[ i ];
									FResponseHandlersQueue.Delete( i );
		//							Serial.println( "ProcessNextCommand 1" );
									ProcessNextCommand();
								}
							}
						}

				FBuffer[ FIndex ++ ] = AChar;
				if( FIndex < 255 )
					return;
			}

//			Serial.println( "TEST!!!" );
//			Serial.println( "" );
//			Serial.println( FIndex );

			FBuffer[ FIndex ] = '\0';
			FIndex = 0;

			Mitov::String AString = FBuffer;

#ifdef __SIMGSM__DEBUG__
			Serial.print( "LINE: " ); Serial.println( AString );

//			Serial.print( "QUEUE: " ); Serial.println( FResponseHandlersQueue.size() );
#endif

			bool	ALockInput;
			bool	AResponseCompleted = false;

//			Serial.print( "FLockRequestedInputIndex : " ); Serial.println( FLockRequestedInputIndex );
//			Serial.print( "FLockInputIndex : " ); Serial.println( FLockInputIndex );
			if( FLockRequestedInputIndex )
			{
				ALockInput = true;
				bool ASuccess = false;
				if( FResponseHandlersQueue[ FLockRequestedInputIndex - 1 ]->Handler->TryProcessRequestedInput( AString, ALockInput, AResponseCompleted, ASuccess ))
				{
					if( AResponseCompleted )
					{
						if( FCurrentElement == FResponseHandlersQueue[ FLockRequestedInputIndex - 1 ] )
							FCurrentElement = nullptr;

						if( ( ! ASuccess ) && FResponseHandlersQueue[ FLockRequestedInputIndex - 1 ]->Command && FResponseHandlersQueue[ FLockRequestedInputIndex - 1 ]->RetryCount )
						{
#ifdef __SIMGSM__DEBUG__
							Serial.println( "RETRY1" );
							Serial.println( FResponseHandlersQueue[ FLockRequestedInputIndex - 1 ]->Command );
#endif
							-- FResponseHandlersQueue[ FLockRequestedInputIndex - 1 ]->RetryCount;
							SendQuery( FResponseHandlersQueue[ FLockRequestedInputIndex - 1 ]->Command, FResponseHandlersQueue[ FLockRequestedInputIndex - 1 ] );
						}

						else
						{
#ifdef __SIMGSM__DEBUG__
							Serial.println( "Queue Delete 1" );
//							Serial.println( ALockInput );
//							Serial.print( "RESP_QUEUE: " ); Serial.println( FResponseHandlersQueue.size() );
#endif
							delete FResponseHandlersQueue[ FLockRequestedInputIndex - 1 ];
							FResponseHandlersQueue.Delete( FLockRequestedInputIndex - 1 );
//							Serial.println( "ProcessNextCommand 1" );
							ProcessNextCommand();
						}
					}

					if( ! ALockInput )
						FLockRequestedInputIndex = 0;
				}

				return;
			}

			ALockInput = false;
			AResponseCompleted = false;
			for( int i = 0; i < FResponseHandlersQueue.size(); ++i )
			{
				bool ASuccess = false;
				if( FResponseHandlersQueue[ i ]->Handler->TryProcessRequestedInput( AString, ALockInput, AResponseCompleted, ASuccess ))
				{
					if( ALockInput )
						FLockRequestedInputIndex = i + 1;

					if( AResponseCompleted )
					{
						if( FCurrentElement == FResponseHandlersQueue[ i ] )
							FCurrentElement = nullptr;

						if( ( ! ASuccess ) && FResponseHandlersQueue[ i ]->Command && FResponseHandlersQueue[ i ]->RetryCount )
						{
#ifdef __SIMGSM__DEBUG__
							Serial.println( "RETRY2" );
							Serial.println( FResponseHandlersQueue[ i ]->Command );
#endif
							-- FResponseHandlersQueue[ i ]->RetryCount;
							SendQuery( FResponseHandlersQueue[ i ]->Command, FResponseHandlersQueue[ i ] );
						}

						else
						{
#ifdef __SIMGSM__DEBUG__
							Serial.println( "Queue Delete 2" );
							Serial.print( "DELETING: " ); Serial.println( FResponseHandlersQueue[ i ]->Command );
//							Serial.print( "RESP_QUEUE: " ); Serial.println( FResponseHandlersQueue.size() );
#endif
							delete FResponseHandlersQueue[ i ];
							FResponseHandlersQueue.Delete( i );
//							Serial.println( "ProcessNextCommand 2" );
							ProcessNextCommand();
						}
					}

					return;
				}
			}

			if( FLockInputIndex )
			{
//				Serial.println( "FLockInputIndex" );
				ALockInput = true;
				FFunctions[ FLockInputIndex - 1 ]->TryProcessInput( AString, ALockInput );
				if( ! ALockInput )
					FLockInputIndex = 0;

				return;
			}

//			Serial.println( "*****" );
			ALockInput = false;
			for( int i = 0; i < FFunctions.size(); ++i )
				if( FFunctions[ i ]->TryProcessInput( AString, ALockInput ))
				{
					if( ALockInput )
					{
						FLockInputIndex = i + 1;
#ifdef __SIMGSM__DEBUG__
						Serial.print( "FLockInputIndex = " ); Serial.println( FLockInputIndex );
#endif
					}

					return;
				}
/*
			ALine.trim();
			if( ALine == "RDY" )
			{
				return true;
			}

			if( ALine == "NORMAL POWER DOWN" )
			{
				return true;
			}

			if( ALine == "Call Ready" )
			{
			}

*/
		}

	public:
		void PulsePower()
		{
#ifdef __SIMGSM__DEBUG__
			Serial.println( "PULSE" );
#endif
			FInPowerSwitch = true;
			PowerOutputPin.SendValue( true );
			FLastTime = micros();
		}

		void PulsePowerDelay()
		{
#ifdef __SIMGSM__DEBUG__
			Serial.println( "PULSE DELAY" );
#endif
			FInPowerSwitchDelay = true;
			FLastTime = micros();
		}

		void DoPowerResult( bool ASucess )
		{
			if( ! ASucess )
				return;

//			Serial.println( "PowerOKReceived()" );
			FInPowerCheckWait = false;
			if( PowerOn && FPowerChecked )
				StartStartupWait();

			else
			{
#ifdef __SIMGSM__DEBUG__
				Serial.println( "FPowerChecked = true" );
#endif
				if( PowerOn )
					PulsePower();

				else
					FPowerChecked = true;
			}

		}

	protected:
		void QueryPowerOn()
		{
//			Serial.print( "FInPowerSwitch = " ); Serial.println( FInPowerSwitch );

			if( FInPowerSwitch )
				return;

			if( ! PowerOutputPin.IsConnected() )
			{
				ModuleReady();
				return;
			}

//			Serial.println( "AT..." );
			SendQueryRegisterResponse( &FPowerOnFunction, "AT" );
			FLastTime = micros();
			FInPowerCheckWait = true;
		}

		void ModuleReady()
		{
			IsStarted = PowerOn;
			ReadyOutputPin.Notify( nullptr );

			for( int i = 0; i < FFunctions.size(); ++i )
				FFunctions[ i ]->ElementSystemStart();
		}

		void StartStartupWait()
		{
#ifdef __SIMGSM__DEBUG__
			Serial.println( "STARTUP_WAIT" );
#endif
			FInStartupWait = true;
			FLastTime = micros();
		}

	public:
		inline void SystemLoopBegin( unsigned long currentMicros )
		{
			if( FInStartupWait )
			{
				if( currentMicros - FLastTime >= Delays.Startup ) // 20000000 )
				{
#ifdef __SIMGSM__DEBUG__
					Serial.println( "FInStartupWait" );
#endif
					FInStartupWait = false;
					ModuleReady();
				}
			}

			else if( FInPowerCheckWait )
			{
				if( currentMicros - FLastTime >= 2000000 )
				{
					FInPowerCheckWait = false;
					FPowerOnFunction.Reset();
					AbortResponseHandler( &FPowerOnFunction );
#ifdef __SIMGSM__DEBUG__
					Serial.println( "FInPowerCheckWait" );
#endif
					if( PowerOn )
					{
						FPowerChecked = true;
						PulsePower();
					}

					else
						StartStartupWait();

//					PowerOutputPin.SendValue( false );
				}
			}

			else if( FInPowerSwitchDelay )
			{
				if( currentMicros - FLastTime >= Delays.Shutdown ) //10000000 )
				{
#ifdef __SIMGSM__DEBUG__
					Serial.println( "SECOND_PULSE" );
#endif
					FInPowerSwitchDelay = false;	
					PulsePower();
				}
			}

			else if( FInPowerSwitch )
			{
				if( currentMicros - FLastTime >= Delays.PowerPulse ) //2000000 )
				{
					PowerOutputPin.SendValue( false );
					if( FPowerChecked )
					{
#ifdef __SIMGSM__DEBUG__
						Serial.println( "FInPowerSwitch" );
#endif
						FInPowerSwitch = false;	
						StartStartupWait();
					}

					else
					{
#ifdef __SIMGSM__DEBUG__
						Serial.println( "FPowerChecked = true" );
#endif
						FPowerChecked = true;
						PulsePowerDelay();
					}
				}
			}

			ReadSerial();
//			Serial.println( "TEST6" );
			if( FCurrentElement )
			{
				unsigned long AMillis = millis();
				if( ( AMillis - FCurrentMillis ) > FCurrentElement->Timeout )
				{
#ifdef __SIMGSM__DEBUG__
					Serial.println( "TIMEOUT" );
#endif
					FLockRequestedInputIndex = 0;

					if( FCurrentElement->Command && FCurrentElement->RetryCount )
					{
#ifdef __SIMGSM__DEBUG__
						Serial.println( "RETRY3" );
						Serial.println( FCurrentElement->Command );
#endif
						-- FCurrentElement->RetryCount;
						SendQuery( FCurrentElement->Command, FCurrentElement );
					}

					else
					{			
						FCurrentElement->Handler->TimedOut();
						FResponseHandlersQueue.erase( FCurrentElement );
						delete FCurrentElement;
	//					Serial.println( "ProcessNextCommand 3" );
						ProcessNextCommand();
					}

					FCurrentElement = nullptr;
				}
			}
/*
			if( FResponseHandlersQueue.size() > 0 )
			{
				int AIndex = ( FLockRequestedInputIndex ) ? ( FLockRequestedInputIndex - 1 ) : 0;
				unsigned long AMillis = millis();
//				Serial.print( "TEST7: " ); Serial.println( AIndex );
//				Serial.print( "SIZE: " ); Serial.println( FResponseHandlersQueue.size() );
				if( ( AMillis - FCurrentMillis ) > FResponseHandlersQueue[ AIndex ]->Timeout )
				{
					Serial.print( "TIMEOUT: " ); Serial.println( AIndex );
					FLockRequestedInputIndex = 0;

					if( FResponseHandlersQueue[ AIndex ]->Command && FResponseHandlersQueue[ AIndex ]->RetryCount )
					{
						Serial.println( "RETRY3" );
						Serial.println( FResponseHandlersQueue[ AIndex ]->Command );
						-- FResponseHandlersQueue[ AIndex ]->RetryCount;
						TrySendQuery( FResponseHandlersQueue[ AIndex ]->Command );
					}

					else
					{			
						FResponseHandlersQueue[ AIndex ]->Handler->TimedOut();
						delete FResponseHandlersQueue[ AIndex ];
						FResponseHandlersQueue.Delete( AIndex );
	//					Serial.println( "ProcessNextCommand 3" );
						ProcessNextCommand();
					}
				}
			}
*/
			if( FResponseHandlersQueue.size() == 0 )
			{
//				Serial.println( "FResponseHandlersQueue.size() == 0" );
				ProcessNextCommand();
			}
		}

		inline void SystemStart()
		{
//			Serial.println( "SYSTEM_START" );
			PowerOutputPin.SendValue( false );
			QueryPowerOn();
		}

	public:
		MitovGSMSerial() :
			PowerOn( true ),
			IsStarted( false ),
			FPowerChecked( false ),
			FInPowerCheckWait( false ),
			FInPowerSwitch( false ),
			FInPowerSwitchDelay( false ),
			FInStartupWait( false ),
			FPowerOnFunction( *this, this, (TOnMitovGSMSerialFunctionResult)&MitovGSMSerial::DoPowerResult )
		{
			memset( FSocketConnections, 0, sizeof( FSocketConnections ) );
		}

	};
//---------------------------------------------------------------------------
	class TArduinoGSMReceivingVoiceCallAutoAnswer
	{
	public:
		bool Enabled : 1;
		uint8_t NumberRings : 7;

	public:
		TArduinoGSMReceivingVoiceCallAutoAnswer() :
			Enabled( false ),
			NumberRings( 1 )
		{
		}
	};
//---------------------------------------------------------------------------
    template<typename T_OWNER> class MitovGSMSerialReceivingVoiceCall
	{
	public:
		OpenWire::SourcePin	CallingOutputPin;
		OpenWire::SourcePin	NumberOutputPin;
		OpenWire::SourcePin	AddressTypeOutputPin;
		OpenWire::SourcePin	SubAddressOutputPin;
		OpenWire::SourcePin	SubAddressTypeOutputPin;
		OpenWire::SourcePin	PhoneBookAddressOutputPin;

	public:
		TArduinoGSMReceivingVoiceCallAutoAnswer	AutoAnswer;

	protected:
		uint8_t	FRingCount = 0;

	protected:
		T_OWNER	&FOwner;

	public:
		void Ringing()
		{
	//		Serial.println( "RINGING!!!" );
			CallingOutputPin.Notify( nullptr );
			if( AutoAnswer.Enabled )
			{
				++FRingCount;
				if( FRingCount >= AutoAnswer.NumberRings )
					FOwner.DoReceiveAnswer();
			}
		}

		inline void ClearRingCount()
		{
			FRingCount = 0;
		}

	public:
		inline void AnswerInputPin_o_Receive( void *_Data )
		{
			FOwner.DoReceiveAnswer();
		}

	public:
		MitovGSMSerialReceivingVoiceCall( T_OWNER &AOwner ) :
			FOwner( AOwner )
		{
			FOwner.FReceivingCall = this;
		}
	};
//---------------------------------------------------------------------------
	template<typename T_OWNER, T_OWNER &C_OWNER> class TMitovGSMSerialFunctionResponseLineAndStatus : public MitovGSMSerialBasicFunction<T_OWNER>
	{
		typedef MitovGSMSerialBasicFunction<T_OWNER> inherited;

	protected:
		OpenWire::Object *FOwnerFunction;
		TOnMitovGSMSerialFunctionResult FCallback;
		TOnMitovGSMSerialFunctionLine	FLineCallback;

	protected:
		bool	FEmptyLineDetected : 1; // = false;
		bool	FLocked : 1; // = false;

	public:
		virtual bool TryProcessRequestedInput( Mitov::String ALine, bool &ALockInput, bool &AResponseCompleted, bool &ASuccess ) override
		{
//			Serial.println( "TryProcessRequestedInput" );
//			Serial.println( ALine );
			if( FLocked )
			{
				if( FEmptyLineDetected )
				{
					ALine.trim();
#ifdef __SIMGSM__DEBUG__
					Serial.print( "RES1: " ); Serial.println( ALine );
#endif
					ASuccess = ( ALine == "OK" );
					if( ASuccess || ( ALine == "ERROR" ))
					{
	//					Serial.print( "RES2: " ); Serial.println( ALine );
						if( FCallback )
							( FOwnerFunction->*FCallback )( ALine == "OK" );

						AResponseCompleted = true;
						FEmptyLineDetected = false;
						ALockInput = false;
						FLocked = false;
						return true;
					}
				}

				else if( ALine == "" )
				{
	//				Serial.println( "FEmptyLineDetected" );
					FEmptyLineDetected = true;
					return true;
				}
			}

			else if(( FOwnerFunction->*FLineCallback )( ALine ) )
			{
#ifdef __SIMGSM__DEBUG__
				Serial.println( "READY!!!" );
#endif
				ALockInput = true;
				FLocked = true;
			}

			return false;
		}

	public:
		TMitovGSMSerialFunctionResponseLineAndStatus( OpenWire::Object *AOwnerFunction, TOnMitovGSMSerialFunctionResult ACallback, TOnMitovGSMSerialFunctionLine ALineCallback ) :
			inherited( C_OWNER ),
			FOwnerFunction( AOwnerFunction ),
			FCallback( ACallback ),
			FLineCallback( ALineCallback ),
			FEmptyLineDetected( false ),
			FLocked( false )
		{
		}

	};
//---------------------------------------------------------------------------
	template<typename T_OWNER, T_OWNER &C_OWNER> class MitovGSMSerialVoiceCallAnswerFunction : public OpenWire::Object
	{
	protected:
//		bool	FEmptyLineDetected = false;
		TMitovGSMSerialFunctionStatus<T_OWNER>	FQuery;

	public:
		void Send()
		{
			C_OWNER.SendQueryRegisterResponse( &FQuery, "ATA" );
		}

	public:
//		virtual bool TryProcessRequestedInput( Mitov::String ALine, bool &ALockInput, bool &AResponseCompleted, bool &ASuccess ) override;

	public:
		MitovGSMSerialVoiceCallAnswerFunction() :
			FQuery( C_OWNER, this, nullptr )
		{
		}

	};
//---------------------------------------------------------------------------
	enum TArduinoGSMSerialVoiceModuleExistingCallMode {ccmDrop, ccmHold};
//---------------------------------------------------------------------------
	template<typename T_OWNER, T_OWNER &C_OWNER> class MitovGSMSerialVoiceCallFunction : public MitovGSMSerialBasicFunction<T_OWNER>
	{
		typedef MitovGSMSerialBasicFunction<T_OWNER> inherited;

	public:
		OpenWire::SourcePin BusyOutputPin;
		OpenWire::SourcePin InUseOutputPin;

	protected:
		enum TState { sIdle, sCalling, sReceivingCall, sTalking };

	public:
		MitovGSMSerialReceivingVoiceCall<MitovGSMSerialVoiceCallFunction<T_OWNER, C_OWNER>> *FReceivingCall = nullptr;

	protected:
		TState	FState = sIdle;

		MitovGSMSerialVoiceCallAnswerFunction<T_OWNER, C_OWNER>	FVoiceCallAnswerFunction;

	protected:
		inline void SetState( TState AValue )
		{
			FState = AValue;
//			Serial.print( "STATE: " ); Serial.println( AValue );
		}

	public:
		inline void DoReceiveAnswer()
		{
			FVoiceCallAnswerFunction.Send();
		}

	public:
		void CallAnswered()
		{
			InUseOutputPin.SendValue( true );
			SetState( sTalking );
		}

		inline void MakingCall()
		{
			SetState( sCalling );
		}

		void DropCall()
		{
			if( sIdle )
				return;

			C_OWNER.SendQueryRegisterResponse( new MitovGSMSerialBasicExpectOKFunction<T_OWNER>( C_OWNER ), "ATH", true );
		}

		void PlaceOnHold()
		{
			if( sIdle )
				return;

			C_OWNER.SendQueryRegisterResponse( new MitovGSMSerialBasicExpectOKFunction<T_OWNER>( C_OWNER ), "AT+CHLD=2", true ); // Place on Hold
		}

	public:
		virtual bool TryProcessInput( Mitov::String ALine, bool &ALockInput ) override
		{
//			Serial.println( "????" );
//			Serial.println( ALine );
			ALine.trim();
//			Serial.println( "TEST3333" );
			if( ALine.startsWith( "+CLIP:" )) //"RDY" )
			{
				ALine.remove( 0, 6 );
				ALine.trim();
//				Serial.println( "TEST111" );
//				ProcessLine( ALine, false );
				if( FReceivingCall )
				{
					Mitov::String ANumber;
					if( Func::ExtractOptionallyQuotedCommaText( ALine, ANumber ))
					{
						Mitov::String AAddressType;
						if( Func::ExtractOptionallyQuotedCommaText( ALine, AAddressType ))
						{
							FReceivingCall->NumberOutputPin.SendValue( ANumber );
							FReceivingCall->AddressTypeOutputPin.template SendValue<uint32_t>( AAddressType.toInt() );
							ALine.trim();
							Mitov::String ASubAddress;
							if( Func::ExtractOptionallyQuotedCommaText( ALine, ASubAddress ))
							{
								Mitov::String ASubAddressType;
								if( Func::ExtractOptionallyQuotedCommaText( ALine, ASubAddressType ))
								{
									FReceivingCall->SubAddressOutputPin.SendValue( ASubAddress );
									FReceivingCall->SubAddressTypeOutputPin.SendValue( ASubAddressType.toInt() );
									ALine.trim();

									Mitov::String APhoneBookAddress;
									if( Func::ExtractOptionallyQuotedCommaText( ALine, APhoneBookAddress ))
										FReceivingCall->PhoneBookAddressOutputPin.SendValue( APhoneBookAddress );

									else
										FReceivingCall->PhoneBookAddressOutputPin.SendValue( "" );
								}
							}
							else
							{
								FReceivingCall->SubAddressOutputPin.SendValue( "" );
								FReceivingCall->SubAddressTypeOutputPin.SendValue( 0 );
								FReceivingCall->PhoneBookAddressOutputPin.SendValue( "" );
							}

						}
					}
				}

				InUseOutputPin.SendValue( true );
				SetState( sReceivingCall );
				return true;
			}

			if( ALine == "RING" )
			{
//				Serial.println( "RRRRR" );
				if( FReceivingCall )
					FReceivingCall->Ringing();
//					FReceivingCall->CallingOutputPin.Notify( nullptr );

				InUseOutputPin.SendValue( true );
				SetState( sReceivingCall );
				return true;
			}

			if( ALine == "BUSY" )
			{
				BusyOutputPin.Notify( nullptr );
				InUseOutputPin.SendValue( false );
				SetState( sIdle );
				if( FReceivingCall )
					FReceivingCall->ClearRingCount();

				return true;
			}

			if( ALine == "NO CARRIER" )
			{
				InUseOutputPin.SendValue( false );
				SetState( sIdle );
				if( FReceivingCall )
					FReceivingCall->ClearRingCount();

				return true;
			}

			if( ALine.startsWith( "+COLP:" ))
			{
				InUseOutputPin.SendValue( true );
				SetState( sTalking );
				return true;
			}

			return false;
		}

		virtual void ElementSystemStart() override
		{
			InUseOutputPin.SendValue( false );
		}

	public:
		MitovGSMSerialVoiceCallFunction() :
			inherited( C_OWNER )
		{
		}

	};
//---------------------------------------------------------------------------
	template<typename T_OWNER, T_OWNER &C_OWNER> class MitovVoiceModuleCallFunction : public MitovGSMSerialBasicExpectOKFunction<T_OWNER>
	{
		typedef MitovGSMSerialBasicExpectOKFunction<T_OWNER> inherited;

	public:
		bool	ClockInputPin_o_IsConnected : 1;

	public:
		TArduinoGSMSerialVoiceModuleExistingCallMode ExistingCallMode : 1;
		Mitov::String Number;

	protected:
		MitovGSMSerialVoiceCallFunction<T_OWNER, C_OWNER> &FOwnerFunction;

	public:
		void ClockInputPin_o_Receive( void *_Data )
		{
			if( ExistingCallMode == ccmDrop )
				FOwnerFunction.DropCall();

			else
				FOwnerFunction.PlaceOnHold();

			C_OWNER.SendQueryRegisterResponse( this, Mitov::String( "ATD" ) + Number + ";" );
			FOwnerFunction.MakingCall();
		}

	public:
		virtual void ElementSystemStart() override 
		{
			if( ! ClockInputPin_o_IsConnected )
				ClockInputPin_o_Receive( nullptr );
		}

	public:
		MitovVoiceModuleCallFunction( MitovGSMSerialVoiceCallFunction<T_OWNER, C_OWNER> &AOwnerFunction ) :
			inherited( C_OWNER ),
			ClockInputPin_o_IsConnected( false ),
			ExistingCallMode( ccmDrop ),
			FOwnerFunction( AOwnerFunction )
		{
		}

	};
//---------------------------------------------------------------------------
	template<typename T_OWNER, T_OWNER &C_OWNER> class MitovGSMSerialVoiceSelectAudioChannelFunction : public MitovGSMSerialBasicExpectOKFunction<T_OWNER>
	{
		typedef MitovGSMSerialBasicExpectOKFunction<T_OWNER> inherited;

	public:
		bool	ClockInputPin_o_IsConnected = false;

	public:
		uint8_t	Channel = 0;

	protected:
		MitovGSMSerialVoiceCallFunction<T_OWNER, C_OWNER> &FOwnerFunction;

	public:
		void ClockInputPin_o_Receive( void *_Data )
		{
			C_OWNER.SendQueryRegisterResponse( this, Mitov::String( "AT+CHFA=" ) + Channel );
		}

	public:
		virtual void ElementSystemStart() override 
		{
			if( ! ClockInputPin_o_IsConnected )
				ClockInputPin_o_Receive( nullptr );
		}

	public:
		MitovGSMSerialVoiceSelectAudioChannelFunction( MitovGSMSerialVoiceCallFunction<T_OWNER, C_OWNER> &AOwnerFunction ) :
			inherited( C_OWNER ),
			FOwnerFunction( AOwnerFunction )
		{
		}

	};
//---------------------------------------------------------------------------
	template<typename T_OWNER, T_OWNER &C_OWNER> class MitovGSMSerialVoiceSetVolumeFunction : public MitovGSMSerialBasicExpectOKFunction<T_OWNER>
	{
		typedef MitovGSMSerialBasicExpectOKFunction<T_OWNER> inherited;

	public:
		bool	ClockInputPin_o_IsConnected = false;

	public:
		uint8_t	Channel = 0;
		float	Volume = 0.5;

	protected:
		MitovGSMSerialVoiceCallFunction<T_OWNER, C_OWNER> &FOwnerFunction;

	public:
		void ClockInputPin_o_Receive( void *_Data )
		{
			int AVolume = ( Volume * 15 ) + 0.5;
			C_OWNER.SendQueryRegisterResponse( this, Mitov::String( "AT+CMIC=" ) + Channel + "," + AVolume );
		}

	public:
		virtual void ElementSystemStart() override 
		{
			if( ! ClockInputPin_o_IsConnected )
				ClockInputPin_o_Receive( nullptr );
		}

	public:
		MitovGSMSerialVoiceSetVolumeFunction( MitovGSMSerialVoiceCallFunction<T_OWNER, C_OWNER> &AOwnerFunction ) :
			inherited( C_OWNER ),
			FOwnerFunction( AOwnerFunction )
		{
		}

	};
//---------------------------------------------------------------------------
	template<typename T_OWNER, T_OWNER &C_OWNER> class MitovGSMSerialSMSSendMessageFunction : public MitovGSMSerialBasicFunction<T_OWNER>
	{
		typedef MitovGSMSerialBasicFunction<T_OWNER> inherited;

	public:
		OpenWire::SourcePin			ReferenceOutputPin;
		OpenWire::SourcePin			FailedOutputPin;

	public:
		Mitov::String Address;
		Mitov::String Text;

	public:
		bool	ClockInputPin_o_IsConnected : 1;

	protected:
		bool	FEmptyLineDetected : 1; // = false;
		bool	FTextSent : 1; // = false;
		bool	FLocked : 1; // = false;

	public:
		void SetText( Mitov::String AValue )
		{
			if( Text == AValue )
				return;

			Text = AValue;

			if( ! ClockInputPin_o_IsConnected )
				TrySendValue();

		}

	protected:
		void TrySendValue()
		{
			if( Address == "" )
				return;

			if( Text == "" )
				return;

//			Serial.println( "TEST555" );
			FTextSent = false;

//			C_OWNER.SendQueryRegisterResponse( this, Mitov::String( "AT+CMGS=\"" ) + Address + "\"\r\n" + Text + "\x1a"  );
			C_OWNER.SendQueryRegisterResponse( this, Mitov::String( "AT+CMGS=\"" ) + Address + "\"" );
//			C_OWNER.GetStream().print( Text );
//			C_OWNER.GetStream().print((char)26);	//the ASCII code of the ctrl+z is 26
		}

	public:
		virtual bool TryProcessPromptInput( bool &ASuccess ) override
		{ 
			if( FTextSent)
				return true;

//			Serial.println( "SEND TEXT!!!" );
			C_OWNER.GetStream().print( Text );
			C_OWNER.GetStream().print((char)26);	//the ASCII code of the ctrl+z is 26
			FTextSent = true;
			ASuccess = true;
			return true; 
		}

		virtual void TimedOut() override
		{
			FailedOutputPin.Notify( nullptr );
		}

		virtual bool TryProcessRequestedInput( Mitov::String ALine, bool &ALockInput, bool &AResponseCompleted, bool &ASuccess ) override
		{
			if( FLocked )
			{
				if( FEmptyLineDetected )
				{
					ALine.trim();
					if( ALine != "" )
					{
//						Serial.println( "COMPLETE!!!" );
						ASuccess = ( ALine == "OK" );

						if( !ASuccess )
							FailedOutputPin.Notify( nullptr );
/*
						if( ALine == "OK" )
						{
			//				Serial.println( "OK Processed" );
			//					Serial.println( "ALockInput = false" );
						}
*/
						AResponseCompleted = true;
						FEmptyLineDetected = false;						
						FLocked = false;
						ALockInput = false;
						return true;
					}
				}

				else if( ALine == "" )
				{
//					Serial.println( "FEmptyLineDetected = true" );
					FEmptyLineDetected = true;
					return true;
				}

				return false;
			}

			ALine.trim();
			if( ALine.startsWith( "+CMGS:" ) )
			{
//				Serial.println( "+CMGS: DETECTED!!!" );
				ALine.remove( 0, 6 );
				ALine.trim();
				uint32_t AReference = ALine.toInt();

				ReferenceOutputPin.Notify( &AReference );

				ALockInput = true;
				FLocked = true;
				return true;
			}

/*
			else 
			{
				if( ALine == ">" ) )
				{
				}
			}
*/

			return false;
		}

	protected:
		void DoInputChange( void *_Data )
		{
			if( ClockInputPin_o_IsConnected )
				return;

			TrySendValue();
		}

	public:
		inline void ClockInputPin_o_Receive( void *_Data )
		{
			TrySendValue();
		}

	public:
		MitovGSMSerialSMSSendMessageFunction() :
			inherited( C_OWNER ),
			ClockInputPin_o_IsConnected( false ),
			FEmptyLineDetected( false ),
			FTextSent( false ),
			FLocked( false )
		{
		}

	};
//---------------------------------------------------------------------------
	template<typename T_OWNER, T_OWNER &C_OWNER> class MitovGSMSerialSMSBasicMessageFunction : public MitovGSMSerialBasicFunction<T_OWNER>
	{
		typedef MitovGSMSerialBasicFunction<T_OWNER> inherited;

	protected:
		bool	FLocked = false;
//		bool	FEmptyLineDetected = false;
		Mitov::String	FLines;

	protected:
		virtual void ProcessLine( Mitov::String ALine, bool AIsSecondLine ) {}

	public:
		virtual void ReceivedMessageInfo( Mitov::String AMessageStatus, Mitov::String AAddress, Mitov::String AName, Mitov::TDateTime ADateTime, int32_t ATimeZone ) {}
		virtual void ReceivedMessageText( Mitov::String AText ) {}

	public:
		virtual bool TryProcessInput( Mitov::String ALine, bool &ALockInput ) override
		{
			if( FLocked )
			{
/*
				if( FEmptyLineDetected )
				{
//				Serial.println( ALine );
					FEmptyLineDetected = false;
					ProcessLine( FLines, true );
					ALockInput = false;
					FLocked = false;
//					ReceivedMessageText( FLines );

					Serial.print( "LINES: " ); Serial.println( FLines );
					FLines = "";

					return true;
				}

				else if( ALine == "" )
				{
					Serial.println( "FEmptyLineDetected" );
					FEmptyLineDetected = true;
					return true;
				}
*/
				if( ALine == "" )
				{
//					Serial.println( "FEmptyLineDetected" );
//					FEmptyLineDetected = false;
					ProcessLine( FLines, true );
					ALockInput = false;
					FLocked = false;
//					ReceivedMessageText( FLines );

//					Serial.print( "LINES: " ); Serial.println( FLines );
					FLines = "";

					return true;
				}
			}

			Mitov::String AShortLine = ALine;
			AShortLine.trim();
//			Serial.println( "TEST3333" );
			if( AShortLine.startsWith( "+CMT:" )) //"RDY" )
			{
//				Serial.println( "+CMT: DETECTED!" );
				ProcessLine( ALine, false );

				FLocked = true;
				ALockInput = true;
				return true;
			}

			else if( FLocked )
				FLines += ALine + "\r\n";

			return false;
		}

	public:
		MitovGSMSerialSMSBasicMessageFunction( MitovGSMSerialSMSModule<T_OWNER, C_OWNER> &AOwnerModule ) :
			inherited( C_OWNER )
		{
			AOwnerModule.FMessageInfo = this;
		}
	};
//---------------------------------------------------------------------------
	template<typename T_OWNER, T_OWNER &C_OWNER> class MitovGSMSerialSMSModule : public MitovGSMSerialBasicFunction<T_OWNER>
	{
		typedef MitovGSMSerialBasicFunction<T_OWNER> inherited;

	public:
		MitovGSMSerialSMSBasicMessageFunction<T_OWNER, C_OWNER> *FMessageInfo = nullptr;

	public:
		MitovGSMSerialSMSModule() :
			inherited( C_OWNER )
		{
		}

	};
//---------------------------------------------------------------------------
	template<typename T_OWNER, T_OWNER &C_OWNER> class MitovGSMSerialSMSMessageFunction : public MitovGSMSerialSMSBasicMessageFunction<T_OWNER, C_OWNER>
	{
		typedef MitovGSMSerialSMSBasicMessageFunction<T_OWNER, C_OWNER> inherited;

	public:
		OpenWire::SourcePin	OutputPin;
		OpenWire::SourcePin	AddressOutputPin;
		OpenWire::SourcePin	NameOutputPin;
		OpenWire::SourcePin	TimeOutputPin;
		OpenWire::SourcePin	TimeZoneOutputPin;

	protected:
		bool FIsPDU = false;

	public:
		virtual void ReceivedMessageInfo( Mitov::String AMessageStatus, Mitov::String AAddress, Mitov::String AName, Mitov::TDateTime ADateTime, int32_t ATimeZone ) override
		{
			AddressOutputPin.SendValue( AAddress );
			NameOutputPin.SendValue( AName );
			TimeOutputPin.Notify( &ADateTime );
			TimeZoneOutputPin.Notify( &ATimeZone );
		}

		virtual void ReceivedMessageText( Mitov::String AText ) override
		{
			OutputPin.SendValue( AText );
		}

	public:
		virtual void ProcessLine( Mitov::String ALine, bool AIsSecondLine ) override
		{
			if( AIsSecondLine )
			{
				if( FIsPDU )
				{
//					Serial.println( ALine );
					// DODO: Decode!
					// http://soft.laogu.com/download/sms_pdu-mode.pdf
					// https://www.diafaan.com/sms-tutorials/gsm-modem-tutorial/online-sms-submit-pdu-decoder/
					// http://jazi.staff.ugm.ac.id/Mobile%20and%20Wireless%20Documents/s_gsm0705pdu.pdf
				}

				else
					OutputPin.SendValue( ALine );
			}

			else
			{
				FIsPDU = false;
				ALine.remove( 0, 5 );
				ALine.trim();
				Mitov::String AAddressOrNameOrLength;
				if( Func::ExtractOptionallyQuotedCommaText( ALine, AAddressOrNameOrLength ))
				{
//					Serial.println( "TTT1" );
//					Serial.println( AAddressOrName );
					Mitov::String ANameOrLength;
					if( Func::ExtractOptionallyQuotedCommaText( ALine, ANameOrLength ))
					{
//						Serial.println( "TTT2" );
						Mitov::String ATimeStamp;
						if( Func::ExtractOptionallyQuotedCommaText( ALine, ATimeStamp ))
						{ 
							// Text Mode
							AddressOutputPin.SendValue( AAddressOrNameOrLength );
							NameOutputPin.SendValue( ANameOrLength );

							Mitov::TDateTime ADateTime;
							int32_t ATimeZone;
							if( GSMFinc::ExtractTimeStamp( ATimeStamp, ADateTime, ATimeZone ))
							{
								TimeOutputPin.Notify( &ADateTime );
								TimeZoneOutputPin.Notify( &ATimeZone );
							}
						}

						else 
						{
//							Serial.println( "YYYYYYYYY" );
							FIsPDU = true;
//							int ALength = ANameOrLength.toInt();
							NameOutputPin.SendValue( AAddressOrNameOrLength );
						}
					}

					else
					{
//						Serial.println( "YYYYYYYYY" );
						FIsPDU = true;
	//					int ALength = ANameOrLength.toInt();
						NameOutputPin.Notify( (void *)"" );
					}
				}
			}
		}

	public:
		using inherited::inherited;

	};
//---------------------------------------------------------------------------
	template<typename T_OWNER, T_OWNER &C_OWNER> class MitovArduinoGSMSerialDetectDefinedTextFunction : public MitovGSMSerialBasicFunction<T_OWNER>
	{
		typedef MitovGSMSerialBasicFunction<T_OWNER> inherited;

	public:
		OpenWire::SourcePin	OutputPin;

	protected:
		const char *FText;

	public:
		virtual bool TryProcessInput( Mitov::String ALine, bool &ALockInput ) override
		{
			ALine.trim();
			if( ALine == FText ) //"RDY" )
			{
				OutputPin.Notify( nullptr );
				return true;
			}

			return false;
		}

	public:
		MitovArduinoGSMSerialDetectDefinedTextFunction( const char *AText ) : 
			inherited( C_OWNER ),
			FText( AText )
		{
		}

	};
//---------------------------------------------------------------------------
	template<typename T_OWNER, T_OWNER &C_OWNER> class MitovGSMSerialSMSMessageReceivedFunctionRequestSMSDetails : public MitovGSMSerialBasicFunction<T_OWNER>
	{
		typedef MitovGSMSerialBasicFunction<T_OWNER> inherited;

	protected:
		MitovGSMSerialSMSMessageReceivedFunction<T_OWNER, C_OWNER> &FOwnerFunction;

	protected:
		bool	FLocked : 1; // = false;
		bool	FEmptyLineDetected : 1; // = false;
		Mitov::String	FLines;

	public:
		virtual bool TryProcessRequestedInput( Mitov::String ALine, bool &ALockInput, bool &AResponseCompleted, bool &ASuccess ) override
		{
	//		Serial.println( "MitovGSMSerialSMSMessageReceivedFunctionRequestSMSDetails::TryProcessRequestedInput" );
	//		Serial.println( ALine );
			if( FEmptyLineDetected )
			{
	//			Serial.println( "FEmptyLineDetected" );
				ALine.trim();
				if( ALine != "" )
				{
					ASuccess = ( ALine == "OK" );
	/*
					if( ALine == "OK" )
					{
	//					Serial.println( "OK Processed" );
	//						Serial.println( "ALockInput = false" );
					}
	*/
					AResponseCompleted = true;
					FEmptyLineDetected = false;
					FLocked = false;
	//				Serial.print( "LINES: " ); Serial.println( FLines );
					if( FOwnerFunction.FOwnerModule.FMessageInfo )
						FOwnerFunction.FOwnerModule.FMessageInfo->ReceivedMessageText( FLines );

					FLines = "";
					ALockInput = false;
					return true;
				}
			}

			else if( FLocked && ( ALine == "" ))
			{
	//			Serial.println( "FEmptyLineDetected = true" );
				FEmptyLineDetected = true;
				return true;
			}

			else
			{
				Mitov::String AShortLine = ALine;
				AShortLine.trim();
	//			Serial.print( "SHORT_LINE: " ); Serial.println( AShortLine );
				if( ! AShortLine.startsWith( "+CMGR:" ) )
				{
					if( FLocked )
						FLines += ALine + "\r\n";
				}

				else
				{
					FLocked = true;
	//				Serial.println( "+CMGR: DETECTED!!!"  );
					ALine.remove( 0, 6 );
					ALine.trim();
					Mitov::String AMessageStatus;
					if( Func::ExtractOptionallyQuotedCommaText( ALine, AMessageStatus ))
					{
	//					Serial.println( AMessageStatus );
						Mitov::String AAddress;
						if( Func::ExtractOptionallyQuotedCommaText( ALine, AAddress ))
						{
	//						Serial.println( AAddress );
							Mitov::String AName;
							if( Func::ExtractOptionallyQuotedCommaText( ALine, AName ))
							{
	//							Serial.println( AName );
								Mitov::String ATimeStamp;
								if( Func::ExtractOptionallyQuotedCommaText( ALine, ATimeStamp ))
								{ 
									Mitov::TDateTime ADateTime;
									int32_t ATimeZone;
									if( GSMFinc::ExtractTimeStamp( ATimeStamp, ADateTime, ATimeZone ))
									{
										if( FOwnerFunction.FOwnerModule.FMessageInfo )
											FOwnerFunction.FOwnerModule.FMessageInfo->ReceivedMessageInfo( AMessageStatus, AAddress, AName, ADateTime, ATimeZone );
	//									TimeOutputPin.Notify( &ADateTime );
	//									TimeZoneOutputPin.Notify( &ATimeZone );
									}
								}
							}
						}
					}
				}
			}

			return false;
		}

	public:
		MitovGSMSerialSMSMessageReceivedFunctionRequestSMSDetails( MitovGSMSerialSMSMessageReceivedFunction<T_OWNER, C_OWNER> &AOwnerFunction ) :
			inherited( AOwnerFunction.FOwner ),
			FOwnerFunction( AOwnerFunction ),
			FLocked( false ),
			FEmptyLineDetected( false )
		{
		}

	};
//---------------------------------------------------------------------------
	template<typename T_OWNER, T_OWNER &C_OWNER> class MitovGSMSerialSMSMessageReceivedFunction : public MitovGSMSerialBasicFunction<T_OWNER>
	{
		typedef MitovGSMSerialBasicFunction<T_OWNER> inherited;

	public:
		OpenWire::SourcePin	StorageOutputPin;
		OpenWire::SourcePin	IndexOutputPin;
		OpenWire::SourcePin	ReceivedOutputPin;

	public:
		MitovGSMSerialSMSModule<T_OWNER, C_OWNER> &FOwnerModule;

	protected:
		void RequestDetails( int32_t AIndex )
		{
			C_OWNER.SendQueryRegisterResponse( new MitovGSMSerialSMSMessageReceivedFunctionRequestSMSDetails<T_OWNER, C_OWNER>( *this ), Mitov::String( "AT+CMGR=" ) + AIndex + Mitov::String( ",1" ), true );
		}

	public:
		virtual bool TryProcessInput( Mitov::String ALine, bool &ALockInput ) override
		{
//			Serial.println( "+CMTI: TEST!!!" );
			ALine.trim();
			if( ALine.startsWith( "+CMTI:" ) )
			{
//				Serial.println( "+CMTI: DETECTED!!!" );
//				Serial.println( "ALine.startsWith" );
//				Serial.println( ALine );
				Mitov::String AStorageType;
				if( Func::ExtractOptionallyQuotedCommaText( ALine, AStorageType ))
				{
					Mitov::String AIndexText;
					if( Func::ExtractOptionallyQuotedCommaText( ALine, AIndexText ))
					{
						StorageOutputPin.SendValue( AStorageType );

						int32_t	AIndex = AIndexText.toInt();
						IndexOutputPin.Notify( &AIndex );

						RequestDetails( AIndex );

//						ReceivedOutputPin.Notify( nullptr );
					}
				}

				return true;
			}

			return false;
		}

		void ReportMessageDetails()
		{
			ReceivedOutputPin.Notify( nullptr );
		}

	public:
		MitovGSMSerialSMSMessageReceivedFunction( MitovGSMSerialSMSModule<T_OWNER, C_OWNER> &AOwnerModule ) :
			inherited( AOwnerModule.FOwner ),
			FOwnerModule( AOwnerModule )
		{
		}

	};
//---------------------------------------------------------------------------
	template<typename T_OWNER, T_OWNER &C_OWNER> class MitovSIM900ReadADCFunction
	{
	public:
		OpenWire::TypedSourcePin<float> OutputPin;
		OpenWire::TypedSourcePin<bool>	ErrorOutputPin;

	protected:
		bool FStarted : 1;
		bool FErrorStarted : 1;

		TMitovGSMSerialFunctionResponseLineAndStatus<T_OWNER, C_OWNER> FQuery;

	protected:
		bool DoProcessLine( Mitov::String ALine )
		{
			ALine.trim();
#ifdef __SIMGSM__DEBUG__
			Serial.print( "TESTING: " ); Serial.println( ALine );
#endif
			if( ! ALine.startsWith( "+CADC:" ))
				return false;

			int APos = ALine.indexOf( ",", 6 );
			if( APos >= 0 )
			{
				Mitov::String ALeft = ALine.substring( 6, APos );
				Mitov::String ARight = ALine.substring( APos + 1 );
				ALeft.trim();
				ARight.trim();
				int ASuccess = ALeft.toInt();
				float AValue = ARight.toInt();

//					Serial.println( ASuccess );
//					Serial.println( AValue );

				ErrorOutputPin.SetValue( ASuccess != 0, FErrorStarted );
				FErrorStarted = true;

				if( ASuccess )
				{
					AValue /= 2800;
					OutputPin.SetValue( AValue, FStarted );
					FStarted = true;
				}
			}

			return true;
		}

	public:
		void ClockInputPin_o_Receive( void *_Data )
		{
			if( ! C_OWNER.IsStarted )
				return;

			C_OWNER.AbortResponseHandler( &FQuery );
			C_OWNER.SendQueryRegisterResponse( &FQuery, "AT+CADC?" );
		}

	public:
		MitovSIM900ReadADCFunction() :
			FStarted( false ),
			FErrorStarted( false ),
			FQuery( (OpenWire::Object *)this, nullptr, (TOnMitovGSMSerialFunctionLine)&MitovSIM900ReadADCFunction::DoProcessLine )
		{
		}

	};
//---------------------------------------------------------------------------
	template<typename T_OWNER, T_OWNER &C_OWNER> class MitovGSMSerialGetSignalStrengthFunction : public MitovGSMSerialBasicFunction<T_OWNER>
	{
		typedef MitovGSMSerialBasicFunction<T_OWNER> inherited;

	public:
		OpenWire::TypedSourcePin<float> SignalStrengthOutputPin;
		OpenWire::TypedSourcePin<bool>	SignalStrengthUnknownOutputPin;
		OpenWire::TypedSourcePin<float> BitErrorRateOutputPin;
		OpenWire::TypedSourcePin<bool>	BitErrorRateUnknownOutputPin;

	protected:
		bool FStarted = false;
		TMitovGSMSerialFunctionResponseLineAndStatus<T_OWNER, C_OWNER> FQuery;

	public:
		void ClockInputPin_o_Receive( void *_Data )
		{
//			Serial.println( "DoClockReceive" );
			C_OWNER.AbortResponseHandler( &FQuery );
			C_OWNER.SendQueryRegisterResponse( &FQuery, "AT+CSQ" );
		}

	protected:
		bool DoProcessLine( Mitov::String ALine )
		{
			ALine.trim();
			if( ! ALine.startsWith( "+CSQ:" ) )
				return false;
//				Serial.println( "ALine.startsWith" );
//				Serial.println( ALine );

			int APos = ALine.indexOf( ",", 6 );
			if( APos >= 0 )
			{
				Mitov::String ALeft = ALine.substring( 6, APos );
				Mitov::String ARight = ALine.substring( APos + 1 );
				ALeft.trim();
				ARight.trim();
				int AStrength = ALeft.toInt();
				int ABitErrorRate = ARight.toInt();

//					Serial.println( AStrength );
//					Serial.println( ABitErrorRate );

				if( AStrength > 31 )
				{
					SignalStrengthOutputPin.SetValue( 0, FStarted );
					SignalStrengthUnknownOutputPin.SetValue( true, FStarted );
				}

				else
				{
					SignalStrengthOutputPin.SetValue( float( AStrength ) / 31, FStarted );
					SignalStrengthUnknownOutputPin.SetValue( false, FStarted );
				}

				if( ABitErrorRate > 31 )
				{
					BitErrorRateOutputPin.SetValue( 0, FStarted );
					BitErrorRateUnknownOutputPin.SetValue( true, FStarted );
				}

				else
				{
					BitErrorRateOutputPin.SetValue( float( ABitErrorRate ) / 7, FStarted );
					BitErrorRateUnknownOutputPin.SetValue( false, FStarted );
				}

				FStarted = true;
			}

			return true;
		}

	public:
		virtual void ElementSystemStart() override
		{
			ClockInputPin_o_Receive( nullptr );
		}

	public:
		MitovGSMSerialGetSignalStrengthFunction() :
			inherited( C_OWNER ),
			FQuery( (OpenWire::Object *)this, nullptr, MAKE_CALLBACK_3_1( OpenWire::Object, TOnMitovGSMSerialFunctionLine, MitovGSMSerialGetSignalStrengthFunction::DoProcessLine, Mitov::String ))
		{
		}

	};
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
}

#ifdef __TEST_FOR_DEBUG_PRINTS__
#undef Serial
#endif

#endif
