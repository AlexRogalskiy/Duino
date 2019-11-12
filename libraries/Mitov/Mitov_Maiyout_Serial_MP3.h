////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2018 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_MAIYOUT_SERIAL_MP3_h
#define _MITOV_MAIYOUT_SERIAL_MP3_h

#include <Mitov.h>
#include <Mitov_DFRobot_Serial_MP3.h>

// http://www.maiyout.com/pd.jsp?id=12#fai_14_top&pfc=null

#ifdef __TEST_FOR_DEBUG_PRINTS__
#define Serial UNGUARDED DEBUG PRINT!!!
#endif

// #define _MAIYOUT_SERIAL_MP3_DEBUG

namespace Mitov
{
	namespace TMitovMaiyoutSerialMP3PlayMode
	{
		enum TMitovMaiyoutSerialMP3PlayMode { FullDisk, Folder, Single, Random, NoLoop };
	};
//---------------------------------------------------------------------------
	class TArduinoMaiyoutMP3PlayerVolume
	{
	public:
		float	Value = 0.5;
	};
//---------------------------------------------------------------------------
	template<typename T_STOPPED_PIN> class TArduinoMaiyoutMP3PlayerPlayback : public T_STOPPED_PIN
	{
	public:
		inline OpenWire::SourcePin & StoppedOutputPin()
		{
			return T_STOPPED_PIN::GetPinInstance();
		}

	public:
		inline void SendStoppedClock()
		{
			T_STOPPED_PIN::ClockPin();
		}

	};
//---------------------------------------------------------------------------
	class TArduinoMaiyoutMP3PlayerTrack
	{
	public:
		TMitovMaiyoutSerialMP3PlayMode::TMitovMaiyoutSerialMP3PlayMode	Mode = TMitovMaiyoutSerialMP3PlayMode::NoLoop;

	};
//---------------------------------------------------------------------------
	template<typename T_INSERTED_PIN, typename T_REMOVED_PIN> class TArduinoMaiyoutMP3PlayerSDCard : public T_INSERTED_PIN, public T_REMOVED_PIN
	{
	public:
		inline OpenWire::SourcePin & InsertedOutputPin()
		{
			return T_INSERTED_PIN::GetPinInstance();
		}

		inline OpenWire::SourcePin & RemovedOutputPin()
		{
			return T_REMOVED_PIN::GetPinInstance();
		}

	public:
		inline void SendInsertedClock()
		{
#ifdef _MAIYOUT_SERIAL_MP3_DEBUG
			Serial.println(F( "SD Inserted" ));
#endif
			T_INSERTED_PIN::ClockPin();
		}

		inline void SendRemovedClock()
		{
#ifdef _MAIYOUT_SERIAL_MP3_DEBUG
			Serial.println(F( "SD Removed" ));
#endif
			T_REMOVED_PIN::ClockPin();
		}

	};
//---------------------------------------------------------------------------
	class MaiyoutMP3PlayerElement_BasicOutput
	{
	public:
		OpenWire::SourcePin	OutputPin;

	};
//---------------------------------------------------------------------------
	class TArduinoMaiyoutMP3Player_Element_Response_Empty
	{
	protected:
		inline void SetResponseElement( MaiyoutMP3PlayerElement_BasicOutput	*AResponseElement )
		{
		}

		inline void SendResponse( uint8_t *AReceiveBuffer, uint8_t AFormat )
		{
		}
	};
//---------------------------------------------------------------------------
	class TArduinoMaiyoutMP3Player_Element_Response
	{
	protected:
		MaiyoutMP3PlayerElement_BasicOutput	*FResponseElement = nullptr;

	protected:
		inline void SetResponseElement( MaiyoutMP3PlayerElement_BasicOutput	*AResponseElement )
		{
			FResponseElement = AResponseElement;
		}

		inline void SendResponse( uint8_t *AReceiveBuffer, uint8_t AFormat )
		{
			if( FResponseElement )
			{
				char *AEnd;
				uint32_t AValue = strtol( ((char *)AReceiveBuffer + 2 ), &AEnd, 16 );
#ifdef _MAIYOUT_SERIAL_MP3_DEBUG
				Serial.print( "Result = " ); Serial.println( AValue );
#endif
				switch( AFormat )
				{
					case 0:
						FResponseElement->OutputPin.Notify( &AValue );
						break;

					case 1:
					{
						float AFloatValue = float( AValue ) / 30;
						FResponseElement->OutputPin.Notify( &AFloatValue );
						break;
					}

					case 2:
					{
						String AStrValue( AValue );
						FResponseElement->OutputPin.Notify( (void *)AStrValue.c_str() );
					}
				};
			}
		}
	};
//---------------------------------------------------------------------------
	template<typename T_SERIAL, T_SERIAL &C_SERIAL, typename T_TIMEOUT_PIN, typename T_ERROR_PIN, typename T_PLAYBACK, typename T_SD_CARD, typename T_ELEMENT_RESPONSE> class MitovMaiyoutSerialMP3 : public T_TIMEOUT_PIN, public T_ERROR_PIN, public T_ELEMENT_RESPONSE
	{
	public:
		inline OpenWire::SourcePin & TimeoutErrorOutputPin()
		{
			return T_TIMEOUT_PIN::GetPinInstance();
		}

		inline OpenWire::SourcePin & ErrorOutputPin()
		{
			return T_ERROR_PIN::GetPinInstance();
		}

	public:
		TArduinoMaiyoutMP3PlayerVolume	Volume;
		TArduinoMaiyoutMP3PlayerTrack	Track;
		T_PLAYBACK	Playback;
		T_SD_CARD	SDCard;

		unsigned long TimeOutPeriod = 500;

		TArduinoDFRobotMP3PlayerEqualizerMode	EqualizerMode = emNormal;


	protected:
		bool	FIsSending : 1;
		bool	FFirstTry : 1;
		bool	FExpectReset : 1;
		uint8_t	FExpectFormatResult : 2;
		unsigned long FTimeOutTimer;

	protected:
		uint8_t	FReceiveIndex = 0;
		uint8_t FReceiveBuffer[ 10 ];
		uint8_t FSendBuffer[ 7 ] = {0x7E, 0, 0, 0, 0, 0, 0};

	public:
		void SendReadQuery( MaiyoutMP3PlayerElement_BasicOutput *AElement, uint8_t ACommand, uint8_t AFormat )
		{
			T_ELEMENT_RESPONSE::SetResponseElement( AElement );
			SendQuery( ACommand );
			FExpectFormatResult = AFormat + 1;
		}

		void SendCommand( uint8_t ACommand, bool ADelay = false )
		{
			FSendBuffer[ 2 ] = ACommand;
			SendCommandSize( 3, ADelay );
			FExpectFormatResult = 0;
		}

		void SendOneParamCommand( uint8_t ACommand, uint8_t AParam )
		{
			FSendBuffer[ 2 ] = ACommand;
			FSendBuffer[ 3 ] = AParam;
			SendCommandSize( 4 );
			FExpectFormatResult = 0;
		}

		void SendOneParam16Command( uint8_t ACommand, uint8_t AParam )
		{
			FSendBuffer[ 2 ] = ACommand;
			FSendBuffer[ 3 ] = AParam >> 8;
			FSendBuffer[ 4 ] = AParam;
			SendCommandSize( 5 );
			FExpectFormatResult = 0;
		}

		void SendTwoParamCommand( uint8_t ACommand, uint8_t AParam1, uint8_t AParam2 )
		{
			FSendBuffer[ 2 ] = ACommand;
			FSendBuffer[ 3 ] = AParam1;
			FSendBuffer[ 4 ] = AParam2;
			SendCommandSize( 5 );
			FExpectFormatResult = 0;
		}

	protected:
		void SendCommandSize( uint8_t ASize, bool ADelay = false )
		{
			FSendBuffer[ 1 ] = ASize;
			FSendBuffer[ ASize + 1 ] = 0xEF;

			int ACheckSum = ASize;
			for( int i = 2; i < ASize; ++i )
				ACheckSum ^= FSendBuffer[i];

			FSendBuffer[ ASize ] = ACheckSum;
//			if( ADelay )
//				delay( 1000 );

			SendBuffer();
        }

		void SendQuery( uint8_t ACommand )
		{
			FSendBuffer[ 2 ] = ACommand;
			SendCommandSize( 3 );
//			FExpectResult = true;
		}

		void SendBuffer()
		{
			while ( FIsSending ) 
			{
				C_SERIAL.SystemLoopBegin( micros() );
				ProcessTimeout();
			}

			DirectSendBuffer();
		}

		void DirectSendBuffer()
		{

//			Serial.println( FSendBuffer[ 2 ], HEX );
#ifdef _MAIYOUT_SERIAL_MP3_DEBUG
			Serial.println();
			Serial.print(F("sending:"));
			for (int i=0; i<sizeof( FSendBuffer ); i++) 
			{
				Serial.print( FSendBuffer[i],HEX);
				Serial.print(F(" "));
			}
			Serial.println();
#endif
			C_SERIAL.GetStream().write( FSendBuffer, FSendBuffer[ 1 ] + 2 );
			FTimeOutTimer = millis();
			FIsSending = true; //_sending[Stack_ACK];
		}

		void CheckResult()
		{
			if( ! FExpectFormatResult )
				return;

			if( FReceiveBuffer[ 0 ] == 'O' )
				if( FReceiveBuffer[ 1 ] == 'K' )
				{
					T_ERROR_PIN::SetPinValue( 0 );

					FReceiveBuffer[ FReceiveIndex ] = '\0';
					T_ELEMENT_RESPONSE::SendResponse( FReceiveBuffer, FExpectFormatResult - 1 );
					FIsSending = false;
					FFirstTry = false;
//					char *AEnd;
//					uint32_t AValue = strtol( ((char *)FReceiveBuffer + 2 ), &AEnd, 16 );
//#ifdef _MAIYOUT_SERIAL_MP3_DEBUG
//					Serial.print( "Result = " ); Serial.println( AValue );
//#endif
					FExpectFormatResult = 0;
					return;
				}

			FExpectFormatResult = 0;
			T_ERROR_PIN::SetPinValue( 1 );
			// Report Error
		}

		void ProcessChar( uint8_t AChar )
		{
#ifdef _MAIYOUT_SERIAL_MP3_DEBUG
			Serial.print( FReceiveIndex ); Serial.print( " = " ); Serial.println( AChar, HEX );
#endif
			if( AChar == 13 )
			{
//				CheckResult();
//				FReceiveIndex = 0;
//				FIsSending = false;
				return;
			}

			if( AChar == 10 )
			{
				CheckResult();
				FReceiveIndex = 0;
				FIsSending = false;
				FFirstTry = false;
				return;
			}

			FReceiveBuffer[ FReceiveIndex ++ ] = AChar;
			if( FExpectReset )
			{
				if( FReceiveIndex < 2 )
					return;

				if( FReceiveBuffer[ 0 ] == 'O' )
					if( FReceiveBuffer[ 1 ] == 'K' )
					{
						FReceiveIndex = 0;
						T_ERROR_PIN::SetPinValue( 0 );
						return;
					}

				if( FReceiveIndex < 5 )
					return;

				FReceiveIndex = 0;

				FIsSending = false;
				FFirstTry = false;

				FExpectReset = false;
				if( ! memcmp( FReceiveBuffer, "MP3OK", 5 ) )
//				if( FReceiveBuffer[ 0 ] == 'M' )
//					if( FReceiveBuffer[ 1 ] == 'P' )
//						if( FReceiveBuffer[ 2 ] == '3' )
//							if( FReceiveBuffer[ 3 ] == 'O' )
//								if( FReceiveBuffer[ 4 ] == 'K' )
								{
#ifdef _MAIYOUT_SERIAL_MP3_DEBUG
									Serial.println( "RESET Detected" );
#endif
									return;
								}

				T_ERROR_PIN::SetPinValue( 1 );
				// Report Error
				return;
			}

			if( ! FExpectFormatResult )
				if( FReceiveIndex == 2 )
				{
					if( FReceiveBuffer[ 0 ] == 'O' )
						if( FReceiveBuffer[ 1 ] == 'K' )
						{
#ifdef _MAIYOUT_SERIAL_MP3_DEBUG
							Serial.println(F( "OK Detected" ));
#endif
							T_ERROR_PIN::SetPinValue( 0 );
							FReceiveIndex = 0;
							FIsSending = false;
							FFirstTry = false;
							return;
						}
				}

			if( FReceiveIndex == 3 )
			{
				if( FReceiveBuffer[ 0 ] == 'e' )
					if( FReceiveBuffer[ 1 ] == 'r' )
						if( FReceiveBuffer[ 2 ] == 'r' )
						{
#ifdef _MAIYOUT_SERIAL_MP3_DEBUG
							Serial.println(F( "Error Detected" ));
#endif
							// Report Error
							FReceiveIndex = 0;
							FIsSending = false;
							FFirstTry = false;
							T_ERROR_PIN::SetPinValue( 2 );
							return;
						}

				if( FReceiveBuffer[ 0 ] == 'S' )
				{
					if( FReceiveBuffer[ 1 ] == 'D' )
					{
						if( FReceiveBuffer[ 2 ] == 'I' )
						{
							SDCard.SendInsertedClock();
//							SDInserted();
							FReceiveIndex = 0;
							return;
						}

						else if( FReceiveBuffer[ 2 ] == 'O' )
						{
							SDCard.SendRemovedClock();
//							SDRemoved();
							FReceiveIndex = 0;
							return;
						}
					}
				}
			}

			else if( FReceiveIndex == 4 )
			{
				if( ! memcmp( FReceiveBuffer, "STOP", 4 ) )
				{
#ifdef _MAIYOUT_SERIAL_MP3_DEBUG
					Serial.println(F( "STOP Detected" ));
#endif
					Playback.SendStoppedClock();
					FReceiveIndex = 0;
				}
			}
		}

		void ProcessTimeout()
		{
			if ( FIsSending && ( millis() - FTimeOutTimer >= TimeOutPeriod )) 
			{
//				Serial.println( "Timeout" );
//				Serial.println( FExpectReset );
				T_TIMEOUT_PIN::ClockPin();
//				ErrorTimeout();
				if( ! FFirstTry )
				{
					FFirstTry = true;
					DirectSendBuffer();
					return;
				}

				FFirstTry = false;
				FIsSending = false;
			}
		}

//		void ErrorTimeout()
//		{
//		}

/*
		void SDInserted()
		{
#ifdef _MAIYOUT_SERIAL_MP3_DEBUG
			Serial.println(F( "SD Inserted" ));
#endif
		}

		void SDRemoved()
		{
#ifdef _MAIYOUT_SERIAL_MP3_DEBUG
			Serial.println(F( "SD Removed" ));
#endif
		}
*/
		void Reset()
		{
			SendCommand( 0x19 );
			FExpectReset = true;
		}

	public:
		void _DirectPinReceive( void *_Data )
		{
			Mitov::TDataBlock ADataBlock = *(Mitov::TDataBlock*)_Data;
			for( int i = 0; i < ADataBlock.Size; ++i )
				ProcessChar( ADataBlock.Data[ i ] );
		}

	public:
		void UpdatePlayMode()
		{
			SendOneParamCommand( 0x33, Track.Mode );
		}

		void UpdateVolume()
		{
			SendOneParamCommand( 0x31, Volume.Value * 30 + 0.5 );
		}

		void UpdateEqualizerMode()
		{
			SendOneParamCommand( 0x32, EqualizerMode );
		}

	public:
		inline void Playback_PlayInputPin_o_Receive( void *_Data )
		{
			SendCommand( 0x11 );
			UpdateVolume();
		}

		inline void ResetInputPin_o_Receive( void *_Data )
		{
			Reset();
		}

		inline void Playback_PauseInputPin_o_Receive( void *_Data )
		{
			SendCommand( 0x12 );
		}

		inline void Playback_StopInputPin_o_Receive( void *_Data )
		{
			SendCommand( 0x1E );
		}

		inline void Playback_FastForwardInputPin_o_Receive( void *_Data )
		{
			SendCommand( 0x1A );
		}

		inline void Playback_RewindInputPin_o_Receive( void *_Data )
		{
			SendCommand( 0x1B );
		}

		inline void Track_NextInputPin_o_Receive( void *_Data )
		{
			SendCommand( 0x13 );
		}

		inline void Track_PreviousInputPin_o_Receive( void *_Data )
		{
			SendCommand( 0x14 );
		}

	public:
		inline void SystemInit()
		{
			Reset();
			delay( 100 );
//			return;
			
/*
			SendQuery( 0x20 );
			SendQuery( 0x21 );
			SendQuery( 0x22 );
			SendQuery( 0x23 );
*/
			SendCommand( 0x1E ); // Stop any playback to clear the queue

			UpdateVolume();
			UpdatePlayMode();
			UpdateEqualizerMode();

//			SendCommand( 0x11 );

/*
			SendCommand( 0x1C );
			SendQuery( 0x20 );
			SendCommand( 0x1C );
			SendQuery( 0x20 );
*/
//			SendQuery( 0x23 );
		}

		inline void SystemLoopBegin( unsigned long currentMicros )
		{
			ProcessTimeout();
		}

	public:
		MitovMaiyoutSerialMP3() :
			FIsSending( false ),
			FFirstTry( false ),
			FExpectReset( false ),
			FExpectFormatResult( 0 )
		{
		}

	};
//---------------------------------------------------------------------------
	template<typename T_OWNER, T_OWNER &C_OWNER> class MaiyoutMP3PlayerElementPlayFile
	{
	public:
		uint16_t	File = 1;

	public:
		void ClockInputPin_o_Receive( void *_Data )
		{
			C_OWNER.SendCommand( 0x1E );
			C_OWNER.SendOneParam16Command( 0x41, File );
			C_OWNER.SendCommand( 0x11 );
			C_OWNER.UpdateVolume();

//			Serial.println( "+++" );
		}

	};
//---------------------------------------------------------------------------
	template<typename T_OWNER, T_OWNER &C_OWNER> class MaiyoutMP3PlayerElementAddPlayFile
	{
	public:
		uint16_t	File = 1;

	public:
		void ClockInputPin_o_Receive( void *_Data )
		{
			C_OWNER.SendOneParam16Command( 0x41, File );
		}

	};
//---------------------------------------------------------------------------
	template<typename T_OWNER, T_OWNER &C_OWNER> class MaiyoutMP3PlayerElementInsertPlayFile
	{
	public:
		uint16_t	File = 1;

	public:
		void ClockInputPin_o_Receive( void *_Data )
		{
			C_OWNER.SendOneParam16Command( 0x43, File );
		}

	};
//---------------------------------------------------------------------------
	template<typename T_OWNER, T_OWNER &C_OWNER> class MaiyoutMP3PlayerElementPlayFolderFile
	{
	public:
		uint8_t	Folder = 0;
		uint8_t	File = 1;

	public:
		inline void ClockInputPin_o_Receive( void *_Data )
		{
			C_OWNER.SendCommand( 0x1E );
			C_OWNER.SendTwoParamCommand( 0x42, Folder, File );
			C_OWNER.SendCommand( 0x11, true );
			C_OWNER.UpdateVolume();
		}

	};
//---------------------------------------------------------------------------
	template<typename T_OWNER, T_OWNER &C_OWNER> class MaiyoutMP3PlayerElementAddPlayFolderFile
	{
	public:
		uint8_t	Folder = 0;
		uint8_t	File = 1;

	public:
		inline void ClockInputPin_o_Receive( void *_Data )
		{
			C_OWNER.SendTwoParamCommand( 0x42, Folder, File );
		}

	};
//---------------------------------------------------------------------------
	template<typename T_OWNER, T_OWNER &C_OWNER> class MaiyoutMP3PlayerElementInsertPlayFolderFile
	{
	public:
		uint8_t	Folder = 0;
		uint8_t	File = 1;

	public:
		inline void ClockInputPin_o_Receive( void *_Data )
		{
			C_OWNER.SendTwoParamCommand( 0x44, Folder, File );
		}

	};
//---------------------------------------------------------------------------
	template<typename T_OWNER, T_OWNER &C_OWNER, uint8_t C_CODE, uint8_t C_FORMAT> class MaiyoutMP3PlayerElementGetTyped : public MaiyoutMP3PlayerElement_BasicOutput
	{
	public:
		inline void ClockInputPin_o_Receive( void *_Data )
		{
			C_OWNER.SendReadQuery( this, C_CODE, C_FORMAT );
		}
	};
//---------------------------------------------------------------------------
}

#ifdef __TEST_FOR_DEBUG_PRINTS__
#undef Serial
#endif

#endif
