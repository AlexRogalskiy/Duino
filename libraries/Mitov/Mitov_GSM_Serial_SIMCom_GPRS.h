////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2018 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_GSM_SERIAL_SIMCOM_GPRS_h
#define _MITOV_GSM_SERIAL_SIMCOM_GPRS_h

#include <Mitov.h>
#include <Mitov_BasicEthernet.h>
#include <Mitov_GSM_Serial.h>
#include <Mitov_StringPrint.h>

//#define __SIMGPRS__DEBUG__

#ifdef __TEST_FOR_DEBUG_PRINTS__
#define Serial UNGUARDED DEBUG PRINT!!!
#endif

namespace Mitov
{
//	class SIMComGPRSModule;
//	class SIMComGPRSTCPClientSocket;
//---------------------------------------------------------------------------
	template<typename T_MODULE, T_MODULE &C_MODULE> class BasicSIMComGPRSSocket : public Mitov::BasicSocket, public MitovGSMSerialBasicFunction<typename T_MODULE::TOwner>
	{
		typedef Mitov::BasicSocket inherited;

	protected:
		bool	FRunning = false;

	protected:
		BufferPrint	FStringPrint;

	public:
		inline void DisconnectInputPin_o_Receive( void * )
		{
#ifdef __SIMGPRS__DEBUG__
			Serial.println( "DoDisconnect" );
#endif
			SetEnabled( false );
		}

	public:
		virtual bool IsEnabled()
		{
			return Enabled; // && FRunning; //C_MODULE.Enabled;
		}

		virtual Print *GetPrint()
		{
			return &FStringPrint;
		}

		virtual bool CanSend()
		{
			return Enabled && FRunning;
		}

		virtual void BeginPacket()
		{
			FStringPrint.Value.clear();
		}

//		virtual void DataReceived( int connectionId, unsigned char AData ) = 0;

	public:
		BasicSIMComGPRSSocket() :
			MitovGSMSerialBasicFunction<typename T_MODULE::TOwner>( C_MODULE.GetOwner() )
		{
			C_MODULE.Sockets.push_back( this );
		}
	};
//---------------------------------------------------------------------------
	template<typename T_MODULE> class TMitovGSMSerialGPRSGetIPFunctionStatus : public MitovGSMSerialBasicFunction<typename T_MODULE::TOwner>
	{
		typedef MitovGSMSerialBasicFunction<typename T_MODULE::TOwner> inherited;

	protected:
		T_MODULE *FOwnerModule;

	public:
		virtual bool TryProcessRequestedInput( Mitov::String ALine, bool &ALockInput, bool &AResponseCompleted, bool &ASuccess ) override
		{
	//			Serial.println( "TryProcessRequestedInput" );
	//			Serial.println( ALine );
	//			delay( 100 );
			if( ALine != "" )
			{
				ALine.trim();
#ifdef __SIMGPRS__DEBUG__
				Serial.print( "RES IP: " ); Serial.println( ALine );
#endif

				bool AIsValid = true;
				int ACountDots = 0;
				for( int i = 0; i < ALine.length(); ++i )
				{
					if( ALine[ i ] == '.' )
						++ACountDots;

					else if( ! isdigit( ALine[ i ] ))
					{
						AIsValid = false;
						break;
					}
				}

#ifdef __SIMGPRS__DEBUG__
				Serial.println( AIsValid );
				Serial.println( ACountDots );
#endif

				ASuccess = ( ( ACountDots == 3 ) && AIsValid );
				if( ASuccess)
					FOwnerModule->GetIPConfigResult( ALine ); //AddressOutputPin.SendValue( ALine, false );

				AResponseCompleted = true;
				ALockInput = false;
				return true;
			}

			return false;
		}

	public:
		TMitovGSMSerialGPRSGetIPFunctionStatus( T_MODULE *AOwnerModule ) :
			inherited( AOwnerModule->GetOwner() ),
			FOwnerModule( AOwnerModule )
		{
		}
	};
//---------------------------------------------------------------------------
	template<typename T_OWNER, T_OWNER &C_OWNER> class SIMComGPRSModule : public MitovGSMSerialBasicFunction<T_OWNER>
	{
	public:
		OpenWire::SourcePin	AddressOutputPin;

	public:
		typedef	T_OWNER	TOwner;

	public:
		bool	Enabled : 1;

	protected:
		bool	FConnected : 1;
		bool	FFromStart : 1;
		bool	FInConnecting : 1; 
		bool	FMultiConnection : 1;
		bool	FTryNextConnection : 1;
		uint8_t	FAPNContext;

	public:
		Mitov::SimpleList<BasicSocket *>	Sockets;

	public:
		Mitov::SimpleObjectList<GPRSAccessPoint *>	AccessPoints;

	public:
		T_OWNER	&GetOwner() { return C_OWNER; }

	public:
		void SetEnabled( bool AValue )
		{
            if( Enabled == AValue )
                return;

            Enabled = AValue;
			if( Enabled )
				StartEthernet();

			else
				StopEthernet();

		}

	public:
/*
		bool AssignServerID( BasicSIMComGPRSSocket *ASocket )
		{
			bool AResult = false;
			for( int i = 0; i < sizeof( FOwner.FSocketConnections ) / sizeof( FOwner.FSocketConnections[ 0 ] ); ++i )
				if( ! FOwner.FSocketConnections[ i ] )
				{
					FOwner.FSocketConnections[ i ] = ASocket;
//					FServerInstance = ASocket;
					AResult = true;
				}

			return AResult;
		}
*/
		void AddServerConnection( void *ASocket, int AIndex )
		{
			if( ! C_OWNER.FSocketConnections[ AIndex ] )
				C_OWNER.FSocketConnections[ AIndex ] = ASocket;

		}

		bool RemoveServerConnection( void *ASocket, int AIndex )
		{
			if( C_OWNER.FSocketConnections[ AIndex ] == ASocket )
			{
				C_OWNER.FSocketConnections[ AIndex ] = nullptr;
				return true;
			}

			return false;
		}

		bool IsServerConnection( void *ASocket, int AIndex )
		{
			return( C_OWNER.FSocketConnections[ AIndex ] == ASocket );
		}

		void ReleaseServerID( void *ASocket )
		{
//			if( FServerInstance == ASocket )
//				FServerInstance = nullptr;

			for( int i = 0; i < sizeof( C_OWNER.FSocketConnections ) / sizeof( C_OWNER.FSocketConnections[ 0 ] ); ++i )
				if( C_OWNER.FSocketConnections[ i ] == ASocket )
					C_OWNER.FSocketConnections[ i ] = nullptr;
		}

		bool AssignConnectionID( void *ASocket, int &AID )
		{
			for( int i = 0; i < sizeof( C_OWNER.FSocketConnections ) / sizeof( C_OWNER.FSocketConnections[ 0 ] ); ++i )
				if( ! C_OWNER.FSocketConnections[ i ] )
				{
					C_OWNER.FSocketConnections[ i ] = ASocket;
					AID = i;
					return true;
				}

			return false;
		}

		void ReleaseConnectionID( int AID )
		{
//			FOwner.FSocketConnections[ AID ] = FServerInstance;
		}

	public:
		void ConnectResult( bool AValue )
		{
#ifdef __SIMGPRS__DEBUG__
			Serial.print( "STATUS: " ); Serial.println( AValue );
#endif
			if( AValue )
			{
				FConnected = true;
				for( int i = 0; i < AccessPoints.size(); ++ i )
					AccessPoints[ i ]->FRetryCount = 0;

				StartConnection();
			}

			else
				TryNextConnection();
		}

		void GetIPConfigResult( Mitov::String AIPAddress )
		{
#ifdef __SIMGPRS__DEBUG__
			Serial.println( "TEST5" );
#endif
			AddressOutputPin.SendValue( AIPAddress );

			for( int i = 0; i < Sockets.size(); ++i )
				Sockets[ i ]->TryStartSocket();
		}

	protected:
		void SetMultiConnectionResult( bool AValue )
		{
#ifdef __SIMGPRS__DEBUG__
			Serial.println( "TEST3_1" );
#endif
			delay( 200 );
			if( AValue )
				SetContext();

#ifdef __SIMGPRS__DEBUG__
			Serial.println( "TEST3_2" );
#endif
			delay( 200 );
		}

		void SetContextResult( bool AValue )
		{
#ifdef __SIMGPRS__DEBUG__
			Serial.println( "TEST3" );
#endif
			delay( 200 );
			if( AValue )
				TryNextConnection();
		}

		void StartConnectionResult( bool AValue )
		{
#ifdef __SIMGPRS__DEBUG__
			Serial.println( "TEST4" );
#endif
			if( AValue )
				GetIPConfig();

		}

		inline void StartConnection()
		{
//			FOwner.SendQueryRegisterResponse( new TMitovGSMSerialFunctionStatus<T_OWNER>( this, (TOnMitovGSMSerialFunctionResult)&SIMComGPRSModule::StartConnectionResult ), Mitov::String( "AT+CIICR" ), true, 5 );
			C_OWNER.SendQueryRegisterResponse( new TMitovGSMSerialFunctionStatus<T_OWNER>( C_OWNER, MAKE_CALLBACK_3( OpenWire::Object, TOnMitovGSMSerialFunctionResult, SIMComGPRSModule::StartConnectionResult, bool )), Mitov::String( "AT+CIICR" ), true, 5 );
		}

		inline void GetIPConfig()
		{
			C_OWNER.SendQueryRegisterResponse( new TMitovGSMSerialGPRSGetIPFunctionStatus<SIMComGPRSModule<T_OWNER, C_OWNER>>( this ), Mitov::String( "AT+CIFSR" ), true, 5 );
		}

	protected:
		void TryNextConnection()
		{
			FTryNextConnection = false;
			unsigned long currentMicros = micros();
			bool ATryNextConnection = false;

			for( int i = 0; i < AccessPoints.size(); ++ i )
			{
				if( AccessPoints[ i ]->Enabled )
				{
//					Serial.println( "TryNextConnection" );
					bool ACanRetry = true;
					if( ! FFromStart )
						if( ! AccessPoints[ i ]->CanRetry( currentMicros, ACanRetry ) )
						{
							ATryNextConnection |= ACanRetry;
							continue;
						}

					ATryNextConnection |= ACanRetry;
					Mitov::String AUserName = AccessPoints[ i ]->UserName;

					MitovGSMSerialBasicFunction<T_OWNER> *AFunction = new TMitovGSMSerialFunctionStatus<T_OWNER>( C_OWNER, MAKE_CALLBACK_3( OpenWire::Object, TOnMitovGSMSerialFunctionResult, SIMComGPRSModule::ConnectResult, bool ));

					if( AUserName == "" )
						C_OWNER.SendQueryRegisterResponse( AFunction, Mitov::String( "AT+CSTT=\"" ) + AccessPoints[ i ]->AccessPointName + "\"", true );

					else
						C_OWNER.SendQueryRegisterResponse( AFunction, Mitov::String( "AT+CSTT=\"" ) + AccessPoints[ i ]->AccessPointName + "\",\"" + AccessPoints[ i ]->UserName + "\",\"" + AccessPoints[ i ]->Password + "\"", true );

					AccessPoints[ i ]->FLastTime = currentMicros;
					++ AccessPoints[ i ]->FRetryCount;
					return;
				}
			}

			FTryNextConnection = ATryNextConnection;
//			Serial.print( "FTryNextConnection: " ); Serial.println( FTryNextConnection );
		}

		void SetMultiConnection()
		{
			if( FMultiConnection )
				TryNextConnection();

			else
			{
#ifdef __SIMGPRS__DEBUG__
				Serial.println( "TEST1" );
#endif
//				FOwner.SendQueryRegisterResponse( this, Mitov::String( "AT+CIPMUX=1" ), true );
				C_OWNER.SendQueryRegisterResponse( new TMitovGSMSerialFunctionStatus<T_OWNER>( C_OWNER, MAKE_CALLBACK_3( OpenWire::Object, TOnMitovGSMSerialFunctionResult, SIMComGPRSModule::SetMultiConnectionResult, bool )), Mitov::String( "AT+CIPMUX=1" ), true, 5 );
			}
		}

		void SetContext()
		{
#ifdef __SIMGPRS__DEBUG__
			Serial.println( "TEST2" );
			Serial.print( "FAPNContext: " ); Serial.println( FAPNContext );
#endif
			C_OWNER.SendQueryRegisterResponse( new TMitovGSMSerialFunctionStatus<T_OWNER>( C_OWNER, MAKE_CALLBACK_3( OpenWire::Object, TOnMitovGSMSerialFunctionResult, SIMComGPRSModule::SetContextResult, bool )), Mitov::String( "AT+CIPSGTXT=" ) + FAPNContext, true, 5 );
		}

		void TryConnect( bool FromStart, unsigned long currentMicros )
		{
			if( FInConnecting )
				return;

			FInConnecting = true;
			FFromStart = FromStart;
			SetMultiConnection();


/*
			for( int i = 0; i < AccessPoints.size(); i ++ )
				if( AccessPoints[ i ]->Enabled )
				{
					bool ACanRetry = false;
					if( ! FromStart )
						if( ! AccessPoints[ i ]->CanRetry( currentMicros, ACanRetry ) )
							continue;

					Mitov::String AUserName = AccessPoints[ i ]->UserName;

					if( AUserName == "" )
						FOwner.SendQueryRegisterResponse( &FRequestConnectAccessPoint, Mitov::String( "AT+CSTT=\"" ) + AccessPoints[ i ]->AccessPointName + "\"" );

					else
						FOwner.SendQueryRegisterResponse( &FRequestConnectAccessPoint, Mitov::String( "AT+CSTT=\"" ) + AccessPoints[ i ]->AccessPointName + "\",\"" + AccessPoints[ i ]->UserName + "\",\"" + AccessPoints[ i ]->Password + "\"" );
/ *
					FConnected = FGprs.attachGPRS( (char *)AccessPoints[ i ]->AccessPointName.c_str(), (char *)AccessPoints[ i ]->UserName.c_str(), (char *)AccessPoints[ i ]->Password.c_str() );
					if( FConnected )
						break;

					AccessPoints[ i ]->FLastTime = currentMicros;
					++ AccessPoints[ i ]->FRetryCount;
* /
				}
*/
		}

		void StopEthernet()
		{
			for( int i = 0; i < Sockets.size(); ++i )
				Sockets[ i ]->StopSocket();

//			LWiFi.end();
//			IsStarted = false;
			FConnected = false;
		}

		void StartEthernet()
		{
			if( ! C_OWNER.IsStarted )
				return;
/*
			if( ! AccessPoints.length() )
				return;

			if( ! Sockets.length() )
				return;
*/
			TryConnect( true, 0 );
//			if( AccessPointName != "" )
//				IsStarted = FGprs.attachGPRS( (char *)AccessPointName.c_str(), (char *)UserName.c_str(), (char *)Password.c_str() );

//			else
//				AConnected = FGprs.attachGPRS();

/*
			if( AConnected )
			{
			}
*/
		}

	public:
		inline void SystemLoopBegin( unsigned long currentMicros )
		{
			if( Enabled )
				if( ! FConnected )
					if( C_OWNER.IsStarted )
						TryConnect( false, currentMicros );

			if( FTryNextConnection )
				TryNextConnection();

		}

	public:
		virtual void ElementSystemStart() override
		{
#ifdef __SIMGPRS__DEBUG__
			Serial.print( "FAPNContext: " ); Serial.println( FAPNContext );
#endif

			if( Enabled )
				if( ! FConnected )
					if( C_OWNER.IsStarted )
						TryConnect( false, micros() );

		}

	public:
		SIMComGPRSModule( uint8_t AAPNContext ) :			
			MitovGSMSerialBasicFunction<T_OWNER>( C_OWNER ),
			Enabled( true ),
			FFromStart( false ),
			FInConnecting( false ),
			FMultiConnection( false ),
			FTryNextConnection( false ),
			FConnected( false ),
			FAPNContext( AAPNContext )
		{
		}

	};
//---------------------------------------------------------------------------
	template<typename T_MODULE, T_MODULE &C_MODULE> class TSIMComGPRSTCPSocketSendPacket : public MitovGSMSerialBasicFunction<typename T_MODULE::TOwner>
	{
		typedef MitovGSMSerialBasicFunction<typename T_MODULE::TOwner> inherited;

	protected:
		BasicSIMComGPRSSocket<T_MODULE, C_MODULE>* FOwnerSocket;
		BufferPrint	FStringPrint;

	public:
		virtual bool TryProcessPromptInput( bool &ASuccess ) override
		{
			C_MODULE.GetOwner().SendDirect( FStringPrint.Value, FStringPrint.Value.size() );
			ASuccess = true;
			return true;
		}

/*
		virtual bool TryProcessRequestedInput( Mitov::String ALine, bool &ALockInput, bool &AResponseCompleted, bool &ASuccess ) override
		{
			if( ALine.startsWith( ">" ) )
			{
				C_MODULE.GetOwner().SendDirect( FStringPrint.Value, FStringPrint.Value.size() );
				ALockInput = false;
				ASuccess = true;
				return true;
			}

			return false;
		}
*/
	public:
		TSIMComGPRSTCPSocketSendPacket( BasicSIMComGPRSSocket<T_MODULE, C_MODULE> *AOwnerSocket, BufferPrint AStringPrint ) :
			inherited( C_MODULE.GetOwner() ),
			FStringPrint( AStringPrint )
		{
	/*
	#ifdef __SIMGPRS__DEBUG__
			Serial.println( "TCPSocketSendPacket" );

			for( int i = 0; i < FStringPrint.Value.size(); ++i )
				Serial.println( FStringPrint.Value[ i ] );

			Serial.println( "" );

	#endif
	*/
		}

	};
//---------------------------------------------------------------------------
	template<typename T_MODULE, T_MODULE &C_MODULE> class SIMComGPRSTCPServerSocket : public BasicSIMComGPRSSocket<T_MODULE, C_MODULE>
	{
		typedef BasicSIMComGPRSSocket<T_MODULE, C_MODULE> inherited;

//		EthernetServer	*FServer;
//		EthernetClient	FClient;

		int	FClientCurrentID = -1;

	public:
		OpenWire::SourcePin	ListeningOutputPin;

	protected:
		bool	FInOpening = false;
		int		FReadSize = 0;
		int		FReadCounter;
		uint8_t	*FBuffer = nullptr;
		uint8_t	*FBufferPtr;

	public:
		virtual bool ProcessCharacter( char AChar, bool &ASuccess ) override
		{ 
			if( FReadCounter )
			{
				*FBufferPtr++ = *(uint8_t*)&AChar;
				--FReadCounter;
//				Serial.println( FReadSize );
//				Serial.println( AChar );
				if( !FReadCounter )
				{
#ifdef __SIMGPRS__DEBUG__
					Serial.println( "SERVER RECEIVE COMPLETED" );
#endif
					ASuccess = true;
					inherited::OutputPin.SendValue( Mitov::TDataBlock( FReadSize, FBuffer ));
					FClientCurrentID = -1;
//					Serial.println( (char *)FBuffer );
					delete [] FBuffer;
					FBuffer = nullptr;
				}

				return true;
			}

			return false; 
		}

		virtual bool TryProcessInput( Mitov::String ALine, bool &ALockInput ) override
		{ 
			ALine.trim();
			if( FInOpening )
			{
				if( ALine == "SERVER OK" )
				{
#ifdef __SIMGPRS__DEBUG__
					Serial.println( "ServerSocketListening" );
#endif
					ListeningOutputPin.SendValue( true );
					FInOpening = false;
					inherited::FRunning = true;
					return true;
				}
			}

			else if( Func::ContainsTextAtPos( ALine, 1, ", REMOTE IP:" ))
			{
				int AIndex = ALine[ 0 ] - '0';
				if( ( AIndex >= 0 ) && ( AIndex <= 7 ))
				{
					C_MODULE.AddServerConnection( this, AIndex );
#ifdef __SIMGPRS__DEBUG__
					Serial.print( "ServerSocket: Client Connected: " ); Serial.println( AIndex );
#endif
				}

				return true;
			}

			else if( Func::ContainsTextAtPos( ALine, 1, ", CLOSE OK" ) || Func::ContainsTextAtPos( ALine, 1, ", CLOSED" ))
			{
				int AIndex = ALine[ 0 ] - '0';
				if( ( AIndex >= 0 ) && ( AIndex <= 7 ))
				{
					if( C_MODULE.RemoveServerConnection( this, AIndex ))
					{
#ifdef __SIMGPRS__DEBUG__
						Serial.print( "ServerSocket: Client Close: " ); Serial.println( AIndex );
#endif
						return true;
					}
				}
			}

			else if( ALine.startsWith( "+RECEIVE," ))
			{
				int AIndex = ALine[ 9 ] - '0';
				if( ( AIndex >= 0 ) && ( AIndex <= 7 ))
				{
					if( C_MODULE.IsServerConnection( this, AIndex ))
					{
						FClientCurrentID = AIndex;
						ALine.remove( 0, 11 );
						FReadSize = ALine.toInt();
#ifdef __SIMGPRS__DEBUG__
						Serial.println( "SERVER RECEIVE " + Mitov::String( AIndex ) + " -> " + FReadSize );
#endif
						if( FReadSize )
						{
							FReadCounter = FReadSize;
							if( FBuffer )
								delete [] FBuffer;

							FBuffer = new uint8_t[ FReadSize ];
							FBufferPtr = FBuffer;
							ALockInput = true;
							return true;
						}
					}
				}
			}

			return false; 
		}

	protected:
		void OpenedResult( bool AResult )
		{
			if( AResult )
			{
				FInOpening = true;
#ifdef __SIMGPRS__DEBUG__
				Serial.println( "ServerSocketConnected" );
#endif
			}
		}

	public:
		inline void SystemStart()
		{
			ListeningOutputPin.SendValue( false );
		}

	protected:
		virtual void StartSocket() override
		{
#ifdef __SIMGPRS__DEBUG__
				Serial.println( "SERVER::StartSocket" );
#endif
//			if( C_MODULE.AssignConnectionID( this ) )
//			if( C_MODULE.AssignServerID( this ) )
//			{
////				C_MODULE.SendData( "AT+CIPSERVER=1," + Mitov::String( Port ), 2000 );
				C_MODULE.GetOwner().SendQueryRegisterResponse( new TMitovGSMSerialFunctionExpectOk<typename T_MODULE::TOwner>( C_MODULE.GetOwner(), MAKE_CALLBACK_3( OpenWire::Object, TOnMitovGSMSerialFunctionResult, SIMComGPRSTCPServerSocket::OpenedResult, bool ), "AT+CIPSERVER=1," + Mitov::String( inherited::Port )), "AT+CIPSERVER=1," + Mitov::String( inherited::Port ), true, 5 );
//			}

		}

/*
		virtual void DataReceived( int connectionId, unsigned char AData )
		{
#ifdef __SIMGPRS__DEBUG__
				Serial.print( "RECEIVED: " );
				Serial.println( connectionId );
#endif
			FClientCurrentID = connectionId;
			OutputPin.Notify( &AData );
//			FClientCurrentID = -1;
		}
*/
	public:
		virtual void EndPacket()
		{
			if( inherited::FStringPrint.Value.size() == 0 )
				return;

			if( FClientCurrentID >= 0 )
			{
				C_MODULE.GetOwner().SendQueryRegisterResponse( new TSIMComGPRSTCPSocketSendPacket<T_MODULE, C_MODULE>( this, inherited::FStringPrint ), "AT+CIPSEND=" + Mitov::String( FClientCurrentID ) +"," + Mitov::String( inherited::FStringPrint.Value.size()), true, 5, 60000 );
//				if( C_MODULE.SendData( "AT+CIPSEND=" + Mitov::String( FClientCurrentID ) + "," + Mitov::String( FStringPrint.Value.size()), "\r\n> ", 2000 ))
//					C_MODULE.SendDirect( (uint8_t *)FStringPrint.Value, FStringPrint.Value.size() );

//				Serial.println( FStringPrint.Value );
			}

			else
			{
				for( int i = 0; i < 7; ++i )
					if( C_MODULE.IsServerConnection( this, i ))
						C_MODULE.GetOwner().SendQueryRegisterResponse( new TSIMComGPRSTCPSocketSendPacket<T_MODULE, C_MODULE>( this, inherited::FStringPrint ), "AT+CIPSEND=" + Mitov::String( i ) +"," + Mitov::String( inherited::FStringPrint.Value.size()), true, 5, 60000 );
			}

//			FStringPrint.Value.clear();
		}

	public:
		virtual void StopSocket()
		{
			FInOpening = false;
			if( inherited::FRunning )
			{
				ListeningOutputPin.SendValue( false );
//				C_MODULE.SendData( "AT+CIPSERVER=0," + Mitov::String( Port ), 2000 );
				C_MODULE.GetOwner().SendQueryRegisterResponse( new TMitovGSMSerialFunctionExpectOk<typename T_MODULE::TOwner>( C_MODULE.GetOwner(), MAKE_CALLBACK_3( OpenWire::Object, TOnMitovGSMSerialFunctionResult, SIMComGPRSTCPServerSocket::OpenedResult, bool ), "AT+CIPSERVER=0"), "AT+CIPSERVER=0", true, 5 );
				C_MODULE.ReleaseServerID( this );
				inherited::FRunning = false;
			}
		}

	public:
		using inherited::inherited;

	};
//---------------------------------------------------------------------------
	template<typename T_MODULE, T_MODULE &C_MODULE> class SIMComGPRSTCPClientSocket : public BasicSIMComGPRSSocket<T_MODULE, C_MODULE>
	{
		typedef BasicSIMComGPRSSocket<T_MODULE, C_MODULE> inherited;

	public:
		OpenWire::SourcePin	ConnectedOutputPin;

	public:
		Mitov::String	Host;
		int		FID;

	protected:
		int		FReadSize;
		int		FReadCounter;
		uint8_t	*FBuffer = nullptr;
		uint8_t	*FBufferPtr;
		bool	FConnectingRunning = false;

	public:
		virtual bool ProcessCharacter( char AChar, bool &ASuccess ) override
		{ 
			if( FReadCounter )
			{
				*FBufferPtr++ = *(uint8_t*)&AChar;
				--FReadCounter;
//				Serial.println( FReadSize );
//				Serial.println( AChar );
//				OutputPin.SendValue( Mitov::TDataBlock( 1, &AChar ));
				if( !FReadCounter )
				{
					ASuccess = true;
					inherited::OutputPin.SendValue( Mitov::TDataBlock( FReadSize, FBuffer ));
//					Serial.println( (char *)FBuffer );
					delete [] FBuffer;
					FBuffer = nullptr;
				}

				return true;
			}

			return false; 
		}

		virtual bool TryProcessInput( Mitov::String ALine, bool &ALockInput ) override
		{ 
			if( ! FConnectingRunning )
				return false;

			ALine.trim();
			if( ALine == Mitov::String( FID ) + ", CONNECT OK" )
			{
#ifdef __SIMGPRS__DEBUG__
				Serial.println( "CLIENT::CONNECT OK " + Mitov::String( FID ) );
#endif
				inherited::FRunning = true;
				ConnectedOutputPin.SendValue( true );
				return true;
			}

			if( ALine == Mitov::String( FID ) + ", CLOSED" )
			{
				FConnectingRunning = false;
#ifdef __SIMGPRS__DEBUG__
				Serial.println( "CLIENT::CLOSED " + Mitov::String( FID ) );
#endif
				StopSocket();
				return true;
			}

			if( ALine == Mitov::String( FID ) + ", CLOSE OK" )
			{
				FConnectingRunning = false;
#ifdef __SIMGPRS__DEBUG__
				Serial.println( "CLIENT::CLOSE OK " + Mitov::String( FID ) );
#endif
				StopSocket();
				return true;
			}

			if( ALine == Mitov::String( FID ) + ", SEND OK" )
			{
#ifdef __SIMGPRS__DEBUG__
				Serial.println( "CLIENT::SEND OK " + Mitov::String( FID ) );
#endif
				return true;
			}

			if( ALine.startsWith( "+RECEIVE," + Mitov::String( FID ) + "," ))
			{
				ALine.remove( 0, 11 );
				FReadSize = ALine.toInt();
#ifdef __SIMGPRS__DEBUG__
				Serial.println( "RECEIVE " + Mitov::String( FID ) + " -> " + FReadSize );
#endif
				if( FReadSize )
				{
					FReadCounter = FReadSize;
					if( FBuffer )
						delete [] FBuffer;

					FBuffer = new uint8_t[ FReadSize ];
					FBufferPtr = FBuffer;
					ALockInput = true;
				}

				return true;
			}

			return false; 
		}

	protected:
		void OpenedResult( bool AResult )
		{
#ifdef __SIMGPRS__DEBUG__
			if( AResult )
				Serial.println( "SocketConnected" );
#endif
		}

	public:
		inline void SystemStart()
		{
			ConnectedOutputPin.SendValue( false );
		}

	protected:
		virtual void StartSocket() override
		{
#ifdef __SIMGPRS__DEBUG__
				Serial.println( "CLIENT::StartSocket" );
#endif
			if( C_MODULE.AssignConnectionID( this, FID ) )
			{
#ifdef __SIMGPRS__DEBUG__
				Serial.print( "CLIENT::StartSocket ID: " ); Serial.println( FID );
#endif
				FConnectingRunning = true;
				C_MODULE.GetOwner().SendQueryRegisterResponse( new TMitovGSMSerialFunctionStatus<typename T_MODULE::TOwner>( C_MODULE.GetOwner(), MAKE_CALLBACK_3( OpenWire::Object, TOnMitovGSMSerialFunctionResult, SIMComGPRSTCPClientSocket::OpenedResult, bool )), "AT+CIPSTART=" + Mitov::String( FID ) +",\"TCP\",\"" + Host + "\"," + Mitov::String( inherited::Port ), true, 5 );
//				C_MODULE.SendData( "AT+CIPSTART=" + Mitov::String( FID ) +",\"TCP\",\"" + Host + "\"," + Mitov::String( Port ), 2000 );
			}

//			Serial.println( "StartSocket" );
		}

		virtual void StopSocket()
		{
//			FConnectingRunning = false;
			if( inherited::FRunning )
			{
#ifdef __SIMGPRS__DEBUG__
				Serial.print( "CLIENT::CloseSocket ID: " ); Serial.println( FID );
#endif
				ConnectedOutputPin.SendValue( false );
				C_MODULE.ReleaseConnectionID( FID );
				C_MODULE.GetOwner().TrySendQuery( "AT+CIPCLOSE=" + Mitov::String( FID ), nullptr );
//				C_MODULE.SendData( "AT+CIPCLOSE=" + Mitov::String( FID ), 2000 );
				inherited::FRunning = false;
			}
//			FClient.stop();
		}

/*
		virtual void DataReceived( int connectionId, unsigned char AData )
		{
			OutputPin.Notify( &AData );
		}
*/

	public:
		virtual void EndPacket()
		{
			if( inherited::FStringPrint.Value.size() == 0 )
				return;

			C_MODULE.GetOwner().SendQueryRegisterResponse( new TSIMComGPRSTCPSocketSendPacket<T_MODULE, C_MODULE>( this, inherited::FStringPrint ), "AT+CIPSEND=" + Mitov::String( FID ) +"," + Mitov::String( inherited::FStringPrint.Value.size()), true, 5, 60000 );

//			if( C_MODULE.SendData( "AT+CIPSEND=" + Mitov::String( FID ) + "," + Mitov::String( inherited::FStringPrint.Value.size()), "\r\n> ", 2000 ))
//				C_MODULE.SendDirect( (uint8_t *)inherited::FStringPrint.Value, inherited::FStringPrint.Value.size() );

//			inherited::FStringPrint.Value.clear();
		}

	public:
		using inherited::inherited;

	};
//---------------------------------------------------------------------------
	template<typename T_MODULE, T_MODULE &C_MODULE> class SIMComGPRSUDPClientSocket : public SIMComGPRSTCPClientSocket<T_MODULE, C_MODULE>
	{
		typedef SIMComGPRSTCPClientSocket<T_MODULE, C_MODULE> inherited;

	protected:
		virtual void StartSocket() override
		{
#ifdef __SIMGPRS__DEBUG__
			Serial.println( "UDP CLIENT::StartSocket" );
#endif
			if( C_MODULE.AssignConnectionID( this, inherited::FID ) )
			{
#ifdef __SIMGPRS__DEBUG__
				Serial.print( "UDP CLIENT::StartSocket ID: " ); Serial.println( inherited::FID );
#endif
				inherited::FConnectingRunning = true;
				C_MODULE.GetOwner().SendQueryRegisterResponse( new TMitovGSMSerialFunctionStatus<typename T_MODULE::TOwner>( C_MODULE.GetOwner(), MAKE_CALLBACK_3( OpenWire::Object, TOnMitovGSMSerialFunctionResult, SIMComGPRSUDPClientSocket::OpenedResult, bool )), "AT+CIPSTART=" + Mitov::String( inherited::FID ) +",\"UDP\",\"" + inherited::Host + "\"," + Mitov::String( inherited::Port ), true, 5 );
//				C_MODULE.SendData( "AT+CIPSTART=" + Mitov::String( FID ) +",\"TCP\",\"" + Host + "\"," + Mitov::String( Port ), 2000 );
			}

//			Serial.println( "StartSocket" );
		}

	public:
		using inherited::inherited;

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
