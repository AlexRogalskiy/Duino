////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2018 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_BASIC_ETHERNET_h
#define _MITOV_BASIC_ETHERNET_h

#include <Mitov.h>
#include <IPAddress.h>
//#include <Ethernet.h>

#ifdef __TEST_FOR_DEBUG_PRINTS__
#define Serial UNGUARDED DEBUG PRINT!!!
#endif

namespace Mitov
{
//---------------------------------------------------------------------------
	class TMACAddress
	{
	public:
		byte FMacAddress[6];

	public:
		TMACAddress(
				uint8_t mac_0, 
				uint8_t mac_1, 
				uint8_t mac_2, 
				uint8_t mac_3, 
				uint8_t mac_4, 
				uint8_t mac_5 )
		{
			FMacAddress[ 0 ] = mac_0;
			FMacAddress[ 1 ] = mac_1;
			FMacAddress[ 2 ] = mac_2;
			FMacAddress[ 3 ] = mac_3;
			FMacAddress[ 4 ] = mac_4;
			FMacAddress[ 5 ] = mac_5;
		}

		TMACAddress()
		{
			memset( FMacAddress, 0, sizeof( FMacAddress ) );
		}
	};
//---------------------------------------------------------------------------
	inline Mitov::String MACAdressToString( uint8_t *AMACAddress )
	{
		char AMACString[ 6 * 3 + 1 ];
		sprintf( AMACString, "%02X-%02X-%02X-%02X-%02X-%02X", AMACString[ 0 ], AMACString[ 1 ], AMACString[ 2 ], AMACString[ 3 ], AMACString[ 4 ], AMACString[ 5 ] );
		return AMACString;
//		MACOutputPin.Notify( AMACString );
	}
//---------------------------------------------------------------------------
	inline Mitov::String IPAdressToString( ::IPAddress AIPAddress )
	{
		char AIPString[ 4 * 4 + 1 ];
		sprintf( AIPString, "%u.%u.%u.%u", AIPAddress[ 0 ], AIPAddress[ 1 ], AIPAddress[ 2 ], AIPAddress[ 3 ] );
		return AIPString;
	}
//---------------------------------------------------------------------------
	class BasicShieldIPAddress
	{
	public:
		bool		Enabled = false;
		::IPAddress	IP;

	};
//---------------------------------------------------------------------------
	class ShieldGatewayAddress : public BasicShieldIPAddress
	{
	public:
		BasicShieldIPAddress	Subnet;
	};
//---------------------------------------------------------------------------
	class ShieldDNSAddress : public BasicShieldIPAddress
	{
	public:
		ShieldGatewayAddress	Gateway;
	};
//---------------------------------------------------------------------------
	class ShieldIPAddress : public BasicShieldIPAddress
	{
	public:
		ShieldDNSAddress	DNS;
	};
//---------------------------------------------------------------------------
	class ShieldIPDNS2Address : public ShieldIPAddress
	{
	public:
		BasicShieldIPAddress	DNS2;
	};
//---------------------------------------------------------------------------
	class BasicSocket
	{
	public:
//		OpenWire::SinkPin	InputPin;
		OpenWire::SourcePin	OutputPin;

	public:
		bool			Enabled = true;
		unsigned int	Port = 80;

	protected:
		virtual void StartSocket() = 0;

	public:
		void SetEnabled( bool AValue )
		{
            if( Enabled == AValue )
                return;

//			Serial.println( "SetEnabled" );
            Enabled = AValue;
			if( IsEnabled() )
				StartSocket();

			else
				StopSocket();

		}

	public:
		virtual bool IsEnabled()
		{
			return Enabled;
		}

		virtual bool CanSend()
		{
			return Enabled;
		}

		virtual void BeginPacket()
		{
		}

		virtual void EndPacket()
		{
		}

		virtual void StopSocket() = 0;

	public:
		virtual void TryStartSocket()
		{
//			Serial.println( Enabled );
			if( Enabled )
				StartSocket();

		}

	};
//---------------------------------------------------------------------------
	template<typename T_OWNER> class BasicEthernetSocket : public Mitov::BasicSocket
	{
		typedef Mitov::BasicSocket inherited;

	public:
		virtual Print *GetPrint() = 0;

	public:
		BasicEthernetSocket( T_OWNER &AOwner, uint32_t AIndex )
		{
			AOwner.Sockets[ AIndex ] = this;
		}

	};
//---------------------------------------------------------------------------
	template<typename T_OWNER, T_OWNER &C_OWNER, typename T_CLIENT> class BasicEthernetConnectSocket : public Mitov::BasicEthernetSocket<T_OWNER>
	{
		typedef Mitov::BasicEthernetSocket<T_OWNER> inherited;

	public:
		OpenWire::TypedSourcePin<bool>	ConnectedOutputPin;

	protected:
		T_CLIENT	FClient;

	public:
		void DisconnectInputPin_o_Receive( void *_Data )
		{
			FClient.flush();
			FClient.stop();
			ConnectedOutputPin.SetValue( false, true );
//			inherited::Enabled = false;
//			Serial.println( "DoDisconnect" );
//			Serial.println( FClient.connected() );
		}

		inline void FlushInputPin_o_Receive( void *_Data )
		{
			FClient.flush();
		}

	public:
		virtual bool IsEnabled() override
		{
			return inherited::Enabled && C_OWNER.Enabled;
		}

		virtual bool CanSend() override
		{
			return inherited::Enabled && C_OWNER.Enabled;
		}

		virtual Print *GetPrint()
		{
			return &FClient;
		}

	public:
		inline void SystemInit()
		{
			ConnectedOutputPin.SetValue( false, false );
		}

	public:
		BasicEthernetConnectSocket( uint32_t AIndex ) :
			inherited( C_OWNER, AIndex )
		{
		}

	};
//---------------------------------------------------------------------------
	template<typename T_OWNER, T_OWNER &C_OWNER, typename T_CLIENT> class TCPClientSocket : public BasicEthernetConnectSocket<T_OWNER, C_OWNER, T_CLIENT>
	{
		typedef BasicEthernetConnectSocket<T_OWNER, C_OWNER, T_CLIENT> inherited;

	public:
		Mitov::String		Host;
		::IPAddress	IPAddress;

	public:
		void DisconnectInputPin_o_Receive( void *_Data )
		{
			inherited::DisconnectInputPin_o_Receive( _Data );
			inherited::Enabled = false;
//			Serial.println( "DoDisconnect" );
//			Serial.println( FClient.connected() );
		}

	public:
		virtual void StartSocket() override
		{
//			Serial.println( "StartSocket" );
			if( Host.length() )
				inherited::FClient.connect( Host.c_str(), inherited::Port );

			else
			{
//				IPAddress.printTo( Serial );
				inherited::FClient.connect( IPAddress, inherited::Port );
			}
		}

		virtual void StopSocket() override
		{
			inherited::FClient.flush();
			inherited::FClient.stop();
			inherited::ConnectedOutputPin.SetValue( false, true );
//			inherited::Enabled = false;
		}

	public:
		inline void SystemLoopBegin( unsigned long currentMicros ) 
		{
			if ( inherited::FClient.available() )
			{
				unsigned char AByte = inherited::FClient.read();
				inherited::OutputPin.SendValue( Mitov::TDataBlock( 1, &AByte ));
//				inherited::OutputPin.Notify( &AByte );
			}

			bool AIsConnected = inherited::FClient.connected();

			inherited::ConnectedOutputPin.SetValue( AIsConnected, true );

			if (! AIsConnected ) 
			{
				inherited::FClient.stop(); // Do we need this?
				inherited::ConnectedOutputPin.SetValue( false, true );

				if( C_OWNER.Enabled )
					inherited::Enabled = false;
			}
		}

	public:
		TCPClientSocket( uint32_t AIndex, ::IPAddress AIPAddress ) :
			inherited( AIndex ),
			IPAddress( AIPAddress )
		{
		}
	};
//---------------------------------------------------------------------------
	template<typename T_OWNER, T_OWNER &C_OWNER, typename T_CLIENT> class TCPSecureClientSocket : public TCPClientSocket<T_OWNER, C_OWNER, T_CLIENT>
	{
		typedef TCPClientSocket<T_OWNER, C_OWNER, T_CLIENT> inherited;

	public:
		Mitov::String	Certificate;
		Mitov::String	PrivateKey;

	protected:
		virtual void StartSocket() override
		{
			if( Certificate != "" )
#if defined( VISUINO_ESP8266 )
				inherited::FClient.setCertificate( (uint8_t*)Certificate.c_str(), Certificate.length() );
#else
				inherited::FClient.setCertificate( Certificate.c_str() );
#endif

			if( PrivateKey != "" )
#if defined( VISUINO_ESP8266 )
				inherited::FClient.setPrivateKey( (uint8_t*)Certificate.c_str(), Certificate.length() );
#else
				inherited::FClient.setPrivateKey( PrivateKey.c_str() );
#endif

			inherited::StartSocket();
		}

	public:
		using inherited::inherited;

	};
//---------------------------------------------------------------------------
	template<typename T_OWNER, T_OWNER &C_OWNER, typename T_SERVER, typename T_CLIENT> class TCPServerSocket : public BasicEthernetConnectSocket<T_OWNER, C_OWNER, T_CLIENT>
	{
		typedef BasicEthernetConnectSocket<T_OWNER, C_OWNER, T_CLIENT> inherited;

	protected:
		T_SERVER	*FServer = nullptr;

	protected:
		virtual void StartSocket() override
		{
//			Serial.println( "StartSockect" );
			if( FServer )
				return;

//			Serial.println( Port );
			FServer = new T_SERVER( inherited::Port );
			FServer->begin();
//			Serial.println( "Start Server Sockect" );
//			Serial.println( inherited::Port );
		}

	public:
		inline void SystemLoopBegin( unsigned long currentMicros ) 
		{
			if( FServer )
			{
				if( ! inherited::FClient )
				{
//					Serial.println( inherited::Port );
//					Serial.println( "TRY CLIENT" );
					inherited::FClient = FServer->available();

//					if( inherited::FClient )
//						Serial.println( "CLIENT" );

//					Serial.println( "TRY CLIENT OUT" );
				}

/*
				if( inherited::FClient )
 					if (! inherited::FClient.connected()) 
					{
						Serial.println( "STOP" );
						inherited::FClient.stop(); // Do we need this?
					}
*/
				if( inherited::FClient )
				{
//					Serial.println( "CLIENT" );
					if( inherited::FClient.available() )
					{
						unsigned char AByte = inherited::FClient.read();
//				Serial.println( "RECEIVED" );
						inherited::OutputPin.SendValue( Mitov::TDataBlock( 1, &AByte ));
//						inherited::OutputPin.Notify( &AByte );
					}

 					if( ! inherited::FClient.connected() ) 
					{
//						Serial.println( "STOP" );
						inherited::FClient.stop(); // Do we need this?
						inherited::ConnectedOutputPin.SetValue( false, true );
					}
				}
			}

//			Serial.println( inherited::FClient.connected() );

			bool AIsConnected = inherited::FClient.connected();

			inherited::ConnectedOutputPin.SetValue( AIsConnected, true );

//			if( ! AIsConnected )
//				if( C_OWNER.Enabled )
//					inherited::Enabled = false;

//			Serial.println( "SYSTEM_LOOP" );
//			Serial.println( "SYSTEM_LOOP_OUT" );
		}

	public:
		virtual bool CanSend()
		{
//			return inherited::Enabled && C_OWNER.Enabled && inherited::FClient;
			return inherited::Enabled && C_OWNER.Enabled;
		}

		virtual void StopSocket() override
		{
			if( FServer )
			{
				delete FServer;
				FServer = nullptr;
			}
		}

	public:
		virtual Print *GetPrint() override
		{
			if( inherited::FClient )
				return inherited::GetPrint();

			return FServer;
		}

/*
		virtual void EndPacket()
		{
			Serial.println( "EndPacket" );
//			inherited::FClient.flush();
//			delay(1000);
//			inherited::FClient.stop();
		}
*/
	public:
		using inherited::inherited;

/*
		virtual ~TCPServerSocket()
		{
			if( FServer )
				delete FServer;
		}
*/
	};
//---------------------------------------------------------------------------
	template<typename T_OWNER, T_OWNER &C_OWNER, typename T_UDP> class UDPSocket : public BasicEthernetSocket<T_OWNER>
	{
		typedef BasicEthernetSocket<T_OWNER> inherited;

	public:
		OpenWire::SourcePin	RemoteIPOutputPin;
		OpenWire::SourcePin	RemotePortOutputPin;

	public:
		unsigned int	RemotePort = 80;
		::IPAddress		RemoteIPAddress;

	protected:
		T_UDP FSocket;
		Mitov::String FHostName;
		bool   FResolved;

	public:
		virtual bool IsEnabled() override
		{
			return inherited::Enabled && C_OWNER.Enabled;
		}

		virtual bool CanSend() override
		{
			return FResolved && inherited::CanSend();
		}

	protected:
		virtual void StartSocket() override
		{
			if( FHostName != "" )
			{
//				Serial.println( "TEST1" );
				FResolved = C_OWNER.GetIPFromHostName( FHostName, RemoteIPAddress );
			}

//			Serial.print( "StartSocket: " );
//			Serial.println( inherited::Port );
			FSocket.begin( inherited::Port );
		}

	public:
		virtual void BeginPacket()
		{
//			Serial.print( "BeginPacket: " );
//			Serial.print( RemoteIPAddress );
//			Serial.print( " " );
//			Serial.println( RemotePort );
			FSocket.beginPacket( RemoteIPAddress, RemotePort );
////  FSocket.beginPacket( RemoteIPAddress, 8888 );
//  FSocket.println( "Hello1" );
////  FSocket.endPacket();
		}

		virtual void EndPacket()
		{
//			Serial.println( "EndPacket" );
//			FSocket.println( "Hello" );
			FSocket.endPacket();
//			delay( 1000 );
		}

		virtual void StopSocket() override
		{
//			Serial.println( "StopSocket" );
			FSocket.stop();
		}

		virtual Print *GetPrint()
		{
//  FSocket.println( "Hello2" );
//			return &Serial;
//			Serial.println( "GetPrint" );
			return &FSocket;
		}

	public:
		inline void SystemLoopBegin( unsigned long currentMicros ) 
		{
			int APacketSize = FSocket.parsePacket();
			if( APacketSize )
			{
//				Serial.println( APacketSize );
				byte *Adata = new byte[ APacketSize ];
				FSocket.read( Adata, APacketSize );

				inherited::OutputPin.SendValue( Mitov::TDataBlock( APacketSize, Adata ));
				RemoteIPOutputPin.SendValue( IPAdressToString( FSocket.remoteIP() ));
				RemotePortOutputPin.SendValue( FSocket.remotePort() );

				delete []Adata;
			}
/*
			if ( FSocket.available() )
			{
				unsigned char AByte = FSocket.read();
				inherited::OutputPin.Notify( &AByte );
			}
*/
		}

	public:
		UDPSocket( uint32_t AIndex, ::IPAddress ARemoteIPAddress ) :
			inherited( C_OWNER, AIndex ),
			RemoteIPAddress( ARemoteIPAddress ),
			FResolved( true )
		{
		}

		UDPSocket( uint32_t AIndex, Mitov::String AHostName ) :
			inherited( C_OWNER, AIndex ),
			FHostName( AHostName ),
			FResolved( false )
		{
		}
	};
//---------------------------------------------------------------------------
	template<typename T_ROOT, T_ROOT &C_ROOT, typename T> class EthernetSocketInput
	{
	public:
		void InputPin_o_Receive( void *_Data )
		{
//			Serial.print( "DoReceive" );
			if( C_ROOT.CanSend() )
			{
				C_ROOT.BeginPacket();

//				Serial.print( "PRINT: " );
//				Serial.println( *(T*)_Data );

				C_ROOT.GetPrint()->println( *(T*)_Data );
				C_ROOT.EndPacket();
			}
		}

	};
//---------------------------------------------------------------------------
	template<typename T_ROOT, T_ROOT &C_ROOT> class EthernetSocketBinaryInput
	{
	public:
		void InputPin_o_Receive( void *_Data )
		{
			if( C_ROOT.CanSend() )
			{
				C_ROOT.BeginPacket();
				Mitov::TDataBlock ADataBlock = *(Mitov::TDataBlock*)_Data;

//				Serial.println( ADataBlock.Size );
				C_ROOT.GetPrint()->write((uint8_t *) ADataBlock.Data, ADataBlock.Size );
//				C_ROOT.GetPrint()->write( *(unsigned char*)_Data );
				C_ROOT.EndPacket();
			}
		}
	};
//---------------------------------------------------------------------------
	template<typename T_ROOT, T_ROOT &C_ROOT> class EthernetSocketBufferInput
	{
	public:
		void InputPin_o_Receive( void *_Data )
		{
			if( C_ROOT.CanSend() )
			{
				C_ROOT.BeginPacket();
				Mitov::TArray<float> ADataBlock = *(Mitov::TArray<float>*)_Data;

//				Serial.println( ADataBlock.Size );
				C_ROOT.GetPrint()->write((uint8_t *) ADataBlock.Data, ADataBlock.Size );
//				C_ROOT.GetPrint()->write( *(unsigned char*)_Data );
				C_ROOT.EndPacket();
			}
		}
	};
//---------------------------------------------------------------------------
	template<typename T_ROOT, T_ROOT &C_ROOT> class EthernetSocketStringInput
	{
	public:
		void InputPin_o_Receive( void *_Data )
		{
			if( C_ROOT.CanSend() )
			{
				C_ROOT.BeginPacket();
				C_ROOT.GetPrint()->println( (char*)_Data );
				C_ROOT.EndPacket();
			}
		}
	};
//---------------------------------------------------------------------------
	template<typename T_ROOT, T_ROOT &C_ROOT> class EthernetSocketClockInput
	{
	public:
		void InputPin_o_Receive( void *_Data )
		{
			if( C_ROOT.CanSend() )
			{
				C_ROOT.BeginPacket();
				C_ROOT.GetPrint()->println( "(Clock)" );
				C_ROOT.EndPacket();
			}
		}
	};
//---------------------------------------------------------------------------
	template<typename T_ROOT, T_ROOT &C_ROOT, typename T_OBJECT> class EthernetSocketObjectInput
	{
	public:
		void InputPin_o_Receive( void *_Data )
		{
			if( C_ROOT.CanSend() )
			{
				C_ROOT.BeginPacket();
				Print *APrint = C_ROOT.GetPrint();
				APrint->println( ((T_OBJECT *)_Data)->ToString().c_str() );
				C_ROOT.EndPacket();
			}
		}
	};
//---------------------------------------------------------------------------
}	

#ifdef __TEST_FOR_DEBUG_PRINTS__
#undef Serial
#endif

#endif
