////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2018 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_LIN_TJA1027_h
#define _MITOV_LIN_TJA1027_h

#include <Mitov.h>

#ifdef __TEST_FOR_DEBUG_PRINTS__
#define Serial UNGUARDED DEBUG PRINT!!!
#endif

namespace Mitov
{
//---------------------------------------------------------------------------
/*
	class SleepPin_NoImplementation
	{
	protected:
		inline void SetSleepPin( bool AValue )
		{
		}

	};
//---------------------------------------------------------------------------
	class SleepPin_StandardPinImplementation
	{
	public:
		OpenWire::SourcePin	SleepOutputPin;

	protected:
		inline void SetSleepPin( bool AValue )
		{
			SleepOutputPin.SendValue( AValue );
		}
	};
//---------------------------------------------------------------------------
	template<typename T_PIN, T_PIN &C_PIN> class SleepPin_DirectPinImplementation
	{
	protected:
		inline void SetSleepPin( bool AValue )
		{
			C_PIN.DigitalWrite( AValue );
		}

	public:
		inline SleepPin_DirectPinImplementation()
		{
			C_PIN.SetMode( OUTPUT );
		}
	};
//---------------------------------------------------------------------------
	template<uint8_t C_PIN_NUMBER> class SleepPin_DirectBoardPinImplementation
	{
	protected:
		inline void SetSleepPin( bool AValue )
		{
			digitalWrite( C_PIN_NUMBER, AValue ? HIGH : LOW );
		}

	public:
		inline SleepPin_DirectBoardPinImplementation()
		{
			pinMode( C_PIN_NUMBER, OUTPUT );
		}
	};
*/
//---------------------------------------------------------------------------
	class TArduinoLinBusBasicReadRawElement
	{
	public:
		OpenWire::SourcePin	OutputPin;
		OpenWire::SourcePin	ErrorOutputPin;

	public:
		void SendResponse( uint8_t *AResponseArray, uint8_t ASize, bool AError )
		{
			if( AError )
				ErrorOutputPin.Notify( nullptr );

			else
			{			
				Mitov::TDataBlock ADataBlock( ASize, AResponseArray );
				OutputPin.Notify( &ADataBlock );
			}
		}

	};
//---------------------------------------------------------------------------
	class TArduinoLinWriteBasicRawElement
	{
	public:
		uint8_t	ID = 0;

	public:
		Mitov::TValueArray<uint8_t> InitialValue;

	public:
		inline void InputPin_o_Receive( void *_Data )
		{
			InitialValue = *(Mitov::TDataBlock*)_Data;
//			Serial.println( InitialValue.ToString() );
		}
	};
//---------------------------------------------------------------------------
	class TArduinoLinBusBasicResponseElement : public TArduinoLinWriteBasicRawElement
	{
	};
//---------------------------------------------------------------------------
	class TArduinoLinBusBasicReceiveElement
	{
	public:
		uint8_t	ID = 0;

	public:
		OpenWire::SourcePin	OutputPin;

	public:
		inline void SendData( uint8_t *AData, uint8_t ASize )
		{
			Mitov::TDataBlock ADataBlock( ASize, AData );
			OutputPin.Notify( &ADataBlock );
		}
	};
//---------------------------------------------------------------------------
	template<typename T_SERIAL, T_SERIAL &C_SERIAL, typename T_SERIAL_OUTPUT, T_SERIAL_OUTPUT &C_SERIAL_OUTPUT, typename T_SLEEP_IMPLEMENTATION, uint8_t C_RESPONSE_COUNT, uint8_t C_RECEIVE_COUNT> class MitovLinTJA1027 : public T_SLEEP_IMPLEMENTATION
	{
	public:
		OpenWire::SourcePin	OutputPin;

		inline OpenWire::SourcePin	& GetSleepPin()
		{
			return T_SLEEP_IMPLEMENTATION::GetPinInstance();
		}

	protected:
		static const uint32_t bound_rate = 10417; // 10417 is best for LIN Interface, most device should work
		static const uint8_t period = 96; // in microseconds, 1s/10417
		static const uint8_t no_bits = 13;
		static const uint32_t del = period * no_bits; // delay for number of bits (no-bits) in microseconds, depends on period

	protected:
		struct TRequestElement
		{
			void *FElement;
			uint8_t FID;
			uint8_t FSize;

		public:
			TRequestElement( TArduinoLinBusBasicReadRawElement *AElement, uint8_t AID, uint8_t ASize ) :
				FElement( AElement ),
				FID( AID ),
				FSize( ASize )
			{
			}

			TRequestElement( TArduinoLinWriteBasicRawElement *AElement, uint8_t AID ) :
				FElement( AElement ),
				FID( AID ),
				FSize( 9 )
			{
			}

			TRequestElement( TRequestElement &AOther ) :
				FElement( AOther.FElement ),
				FID( AOther.FID ),
				FSize( AOther.FSize )
			{
			}

			TRequestElement() :
				FElement( nullptr ),
				FID( 0 ),
				FSize( 0 )
			{
			}
/*
			TRequestElement & operator = ( TRequestElement &AOther )
			{
				FElement = AOther.FElement;
				FID = AOther.FID;
				FSize = AOther.FSize;

				return AOther;
			}
*/
		};

	public:
		bool	Enabled : 1;

	protected:
//		Mitov::TValueArray FResponseArray;
		unsigned long	FLastTime;
		bool	FInProcessByte : 1;
		uint8_t FCountCheck : 4;
		uint8_t FReadIndex : 4;
		uint8_t FSize : 4;
		uint8_t FResponseArray[ 8 + 1 + 2 ];
		TArduinoLinBusBasicReadRawElement *FProcessingRequest = nullptr;
		Mitov::SimpleList<TRequestElement>	FRequests;

	public:
		TArduinoLinBusBasicResponseElement	*FResponses[ C_RESPONSE_COUNT ];
		TArduinoLinBusBasicReceiveElement	*FReceives[ C_RECEIVE_COUNT ];

	protected:
		void SendOutput()
		{
			Mitov::TDataBlock ADataBlock( FReadIndex, FResponseArray );
			OutputPin.Notify( &ADataBlock );
		}

		void ProcessByte( uint8_t AValue )
		{
//			Serial.println( AValue, HEX );
//			Serial.println( FReadIndex );
//			Serial.println( FSize );

			FLastTime = micros();
			if( FReadIndex < FSize )
				FResponseArray[ FReadIndex++ ] = AValue;

			if( FInProcessByte )
				return;

//			Serial.print( "," );
			unsigned long	ACurrentTime = micros();
			if( FProcessingRequest )
			{
//				Serial.println( "TEST1" );
//				Serial.println( FReadIndex );
				while( FReadIndex < FSize )
				{
					FInProcessByte = true;
					C_SERIAL_OUTPUT.SystemLoopBegin( ACurrentTime );
					FInProcessByte = false;
					ACurrentTime = micros();
//					Serial.print( "." );
					if( ( ACurrentTime - FLastTime ) > del )
						break;
				}

//				Serial.print( FReadIndex ); Serial.print( " - " ); Serial.println( FSize );
//				delay( 100 );

/*
				if( FReadIndex < FSize )
				{
//					Serial.println( "TEST5" );
					FProcessingRequest->SendResponse( FResponseArray, FReadIndex, true );
					FProcessingRequest = nullptr;
					return;
				}
*/
//				Serial.println( FReadIndex );
				SendOutput();
				if( FReadIndex < 3 )
				{
//					Serial.println( "TEST5" );
					FProcessingRequest->SendResponse( FResponseArray, FReadIndex, true );
					FProcessingRequest = nullptr;
					FCountCheck = 0;
					return;
				}

				uint8_t AChecksum = FResponseArray[ FReadIndex - 1 ];
				uint8_t ASum = 0;
				for( int i = 0; i < FReadIndex - 1; ++i ) 
					ASum += FResponseArray[i];

				ASum = 254 - ASum;
				FProcessingRequest->SendResponse( FResponseArray, FReadIndex - 1, ( AChecksum != ASum ) );
				FProcessingRequest = nullptr;
				FCountCheck = 0;
				FReadIndex = 0;
				FSize = 8 + 1 + 2;
				return;
			}

//				Serial.println( FReadIndex );
//				Serial.println( FSize );
			FLastTime = ACurrentTime;
			while( FReadIndex < FSize )
			{
				FInProcessByte = true;
				C_SERIAL_OUTPUT.SystemLoopBegin( ACurrentTime );
				FInProcessByte = false;

				if( FReadIndex == 2 )
				{ // Check if it is request!
//						Serial.println( "++++" );
					if( FResponseArray[ 0 ] == 0x55 )
					{
						for( int i = 0; i < C_RESPONSE_COUNT; ++i )
						{
							if( FResponses[ i ]->ID == FResponseArray[ 1 ] )
							{
								SendOutput();
								FReadIndex = 0;
								SetSend( true );
								delayMicroseconds( 5 );

								uint8_t ASum = 0;

//								Serial.println( FResponses[ i ]->InitialValue.ToString() );

								uint8_t *AData = FResponses[ i ]->InitialValue.Data;
								int ASize = FResponses[ i ]->InitialValue.Size;
								for( int i = 0; i < ASize; ++i ) 
									ASum += AData[ i ];

					//			ASum = 255 - ( ASum + 1 );
								ASum = 254 - ASum;

								Stream &AStream = C_SERIAL.GetStream();
//								AData = FResponses[ i ]->InitialValue.Data;
								for( int i = 0; i < ASize; ++i )
								{
//									Serial.print( AData[ i ], HEX ); Serial.print( " " );
									AStream.write( AData[ i ] );
								}

								AStream.write( ASum );

//								Serial.println( "REPLY" );

								FLastTime = micros();
								SetSend( false );
								return;
							}
						}
					}
				}

				ACurrentTime = micros();
				if( ( ACurrentTime - FLastTime ) > del )
					break;
			}

			SendOutput();

			if( FReadIndex < 2 )
			{
				FReadIndex = 0;
				return;
			}

			if( FResponseArray[ 0 ] != 0x55 )
				return; // Error!

//				Serial.println( "TEST1" );
			uint8_t AChecksum = FResponseArray[ FReadIndex - 1 ];
			uint8_t ASum = 0;
			for( int i = 2; i < FReadIndex - 1; ++i )
			{
//					Serial.print( FResponseArray[i], HEX ); Serial.print( " " );
				ASum += FResponseArray[i];
			}

//				Serial.println( "" );

			ASum = 254 - ASum;
//				Serial.print( ASum, HEX ); Serial.print( " - " ); Serial.println( AChecksum, HEX );
			if( AChecksum != ASum )
				return; // Error!

			for( int i = 0; i < C_RECEIVE_COUNT; ++i )
				if( FReceives[ i ]->ID == FResponseArray[ 1 ] )
				{
					FReceives[ i ]->SendData( FResponseArray + 2, FReadIndex - 3 );
					FReadIndex = 0;
					return;
				}

			FReadIndex = 0;
		}

		void SetSend( bool AValue )
		{
			T_SLEEP_IMPLEMENTATION::SetSleepPin( AValue );
		}

		void SendOneRequest( TArduinoLinBusBasicReadRawElement *ASender, uint8_t AID, uint8_t ASize )
		{
			if( ASize > 8 )
				ASize = 8;

//			Serial.println( "REQUEST" );

//			FResponseArray.Resize( ASize );
//			if( ASize )
			{
				FSize = ASize + 1;
				FReadIndex = 0;
				FProcessingRequest = ASender;
			}

			SetSend( true );
//			uint32_t del = period * no_bits; // delay for number of bits (no-bits) in microseconds, depends on period

			unsigned long	ACurrentTime = micros();
			unsigned long	ALastDelay = ACurrentTime - FLastTime;
			if( ALastDelay < del )
				delayMicroseconds( del - ALastDelay ); // delay

			delayMicroseconds( 10 );

			C_SERIAL.GetStream().write( 0x55 );
			C_SERIAL.GetStream().write( AID );

//			return;
//			Serial.println( "TEST1" );

			ACurrentTime = micros();
			FLastTime = ACurrentTime;
/*
			if( ASize )
			{
				while( FReadIndex < FSize )
				{
					C_SERIAL_OUTPUT.SystemLoopBegin( ACurrentTime );
					ACurrentTime = micros();
					if( ( ACurrentTime - FLastTime ) > del )
						break;
				}

				uint8_t AChecksum = FResponseArray[ FReadIndex - 1 ];
				uint8_t ASum = 0;
				for( int i = 0; i < FReadIndex - 1; ++i ) 
					ASum += FResponseArray[i];

				ASum = 254 - ASum;
				FProcessingRequest->SendResponse( FResponseArray, FReadIndex, ( AChecksum != ASum ) );
				FProcessingRequest = nullptr;
				FSize = 8 + 1 + 2;
			}
*/
			SetSend( false );
		}

		void SendOneWrite( TArduinoLinWriteBasicRawElement *ASender, uint8_t AID )
		{
			SetSend( true );
//			uint32_t del = period * no_bits; // delay for number of bits (no-bits) in microseconds, depends on period

			uint8_t ASum = 0;
			uint8_t *AData = ASender->InitialValue.Data;
			for( int i = 0; i < ASender->InitialValue.Size; ++i ) 
				ASum += AData[ i ];

//			ASum = 255 - ( ASum + 1 );
			ASum = 254 - ASum;

			unsigned long	ACurrentTime = micros();
			unsigned long	ALastDelay = ACurrentTime - FLastTime;
			if( ALastDelay < del )
				delayMicroseconds( del - ALastDelay ); // delay

			delayMicroseconds( 10 );

			Stream &AStream = C_SERIAL.GetStream();
			AStream.write( 0x55 );
			AStream.write( AID );

			for( int i = 0; i < ASender->InitialValue.Size; ++i ) 
				AStream.write( AData[ i ] );

			AStream.write( ASum );

			FLastTime = micros();

			SetSend( false );
		}

	public:
		void _DirectPinReceive( void *_Data )
		{
			if( ! Enabled )
				return;

			Mitov::TDataBlock ADataBlock = *(Mitov::TDataBlock*)_Data;
//			Serial.println( ADataBlock.ToString() );
//			delay( 200 );
			for( int i = 0; i < ADataBlock.Size; ++i )
				ProcessByte( ADataBlock.Data[ i ] );
		}

	public:
		void SendRequest( TArduinoLinBusBasicReadRawElement *ASender, uint8_t AID )
		{
			if( ! Enabled )
				return;

			if( ! FProcessingRequest )
				SendOneRequest( ASender, AID, 8 );

			else
			{
				TRequestElement AElement( ASender, AID, 8 ); // Requires temporary variable due to compiler bug! :-(
				FRequests.push_back( AElement );
			}

		}

		void SendWrite( TArduinoLinWriteBasicRawElement *ASender, uint8_t AID )
		{
			if( ! Enabled )
				return;

			if( ! FProcessingRequest )
				SendOneWrite( ASender, AID );

			else
			{
				TRequestElement AElement( ASender, AID ); // Requires temporary variable due to compiler bug! :-(
				FRequests.push_back( AElement );
			}

		}

	public:
		inline void SystemInit()
		{
			FLastTime = micros();
		}

/*
		inline void SystemStart()
		{
//			C_SERIAL.begin( bound_rate );
		}
*/
		inline void SystemLoopBegin( unsigned long currentMicros )
		{
			if( ! Enabled )
				return;

			if( FProcessingRequest )
			{
				if( FCountCheck < 15 )
					++FCountCheck;

				else
				{
					C_SERIAL_OUTPUT.SystemLoopBegin( currentMicros );
	//					Serial.print( "." );
					if( ( micros() - FLastTime ) > del * 2 )
					{
						FProcessingRequest->SendResponse( FResponseArray, FReadIndex, true );
						FProcessingRequest = nullptr;
						FCountCheck = 0;
						FReadIndex = 0;
					}					
				}
			}

			if( ! FProcessingRequest )
				if( FRequests.size() )
				{
					TRequestElement AElement = FRequests[ 0 ];
					FRequests.pop_front();
					if( AElement.FSize > 8 )
						SendOneWrite( (TArduinoLinWriteBasicRawElement *)AElement.FElement, AElement.FID );

					else
						SendOneRequest( (TArduinoLinBusBasicReadRawElement *)AElement.FElement, AElement.FID, AElement.FSize );

				}

		}

	public:
		MitovLinTJA1027() :
			Enabled( true ),
			FInProcessByte( false ),
			FCountCheck( 0 ),
			FReadIndex( 0 ),
			FSize( 8 + 1 + 2 )
		{
		}

	};
//---------------------------------------------------------------------------
	template<typename T_OWNER, T_OWNER &C_OWNER> class TArduinoLinBusReadRawElement : public TArduinoLinBusBasicReadRawElement
	{
	public:
		uint8_t	ID = 0;
//		uint8_t	Size = 1;

	public:
		inline void ClockInputPin_o_Receive( void *_Data )
		{
			C_OWNER.SendRequest( this, ID );
		}
	};
//---------------------------------------------------------------------------
	template<typename T_OWNER, T_OWNER &C_OWNER> class TArduinoLinWriteBasicNonClockedRawElement : public TArduinoLinWriteBasicRawElement
	{
		typedef TArduinoLinWriteBasicRawElement	inherited;

	public:
		inline void InputPin_o_Receive( void *_Data )
		{
			inherited::InputPin_o_Receive( _Data );
			C_OWNER.SendWrite( this, inherited::ID );
		}
	};
//---------------------------------------------------------------------------
	template<typename T_OWNER, T_OWNER &C_OWNER, typename T_BASE> class TArduinoLinBusWriteRawElement : public T_BASE
	{
		typedef T_BASE	inherited;

	public:
		inline void ClockInputPin_o_Receive( void *_Data )
		{
			C_OWNER.SendWrite( this, inherited::ID );
		}

	};
//---------------------------------------------------------------------------
	template<typename T_OWNER, T_OWNER &C_OWNER, uint8_t C_INDEX> class TArduinoLinBusRawDataResponseElement : public TArduinoLinBusBasicResponseElement
	{
	public:
		TArduinoLinBusRawDataResponseElement()
		{
			C_OWNER.FResponses[ C_INDEX ] = this;
		}
	};
//---------------------------------------------------------------------------
	template<typename T_OWNER, T_OWNER &C_OWNER, uint8_t C_INDEX> class TArduinoLinBusRawDataReceiveElement : public TArduinoLinBusBasicReceiveElement
	{
	public:
		TArduinoLinBusRawDataReceiveElement()
		{
			C_OWNER.FReceives[ C_INDEX ] = this;
		}
	};
//---------------------------------------------------------------------------
}

#ifdef __TEST_FOR_DEBUG_PRINTS__
#undef Serial
#endif

#endif
