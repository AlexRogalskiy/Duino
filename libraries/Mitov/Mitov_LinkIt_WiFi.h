////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2018 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_LINKIT_WIFI_h
#define _MITOV_LINKIT_WIFI_h

#include <Mitov.h>
#include <Mitov_BasicEthernet.h>
#include <LWiFi.h>
#include <LWiFiServer.h>
#include <LWiFiClient.h>
#include <LWiFiUDP.h>

namespace Mitov
{
//---------------------------------------------------------------------------
	enum LinkItWiFiEncription { liweAuto, liweWEP, liweWPA };
//---------------------------------------------------------------------------
	class LinkItRemoteAccessPoint
	{
	public:
		bool	Enabled : 1;
		LinkItWiFiEncription	Encription : 2;
		Mitov::String	SSID;
	    Mitov::String	Password;

	public:
		bool Connect()
		{
			if( ! Enabled )
				return false;

			if( SSID == "" )
				return false;

			if( Password == "" )
				return ( LWiFi.connect( SSID.c_str() ) > 0 );


			switch( Encription )
			{
				case liweAuto:
				{
					if( LWiFi.connectWPA( SSID.c_str(), Password.c_str() ) > 0 )
						return true;

					return( LWiFi.connectWEP( SSID.c_str(), Password.c_str() ) > 0 );
				}

				case liweWEP:
					return( LWiFi.connectWEP( SSID.c_str(), Password.c_str() ) > 0 );

				case liweWPA:
					return( LWiFi.connectWPA( SSID.c_str(), Password.c_str() ) > 0 );
			}
		}

	public:
		LinkItRemoteAccessPoint() :
			Enabled( true ),
			Encription( liweAuto )
		{
		}

	};
//---------------------------------------------------------------------------
	class LinkItWiFiModule
	{
	public:
		OpenWire::SourcePin	FoundSignalStrengthOutputPin;
		OpenWire::SourcePin	FoundSSIDOutputPin;

		OpenWire::SourcePin	AddressOutputPin;
		OpenWire::SourcePin	MACOutputPin;

		OpenWire::SourcePin	SignalStrengthOutputPin;
		OpenWire::SourcePin	BSSIDOutputPin;
		OpenWire::SourcePin	GatewayIPOutputPin;
		OpenWire::SourcePin	SubnetMaskIPOutputPin;

	public:
		Mitov::SimpleObjectList<LinkItRemoteAccessPoint*>	AccessPoints;
		Mitov::SimpleList<BasicEthernetSocket<LinkItWiFiModule>*>	Sockets;

		bool	Enabled : 1;

	public:
		bool	IsStarted : 1;

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
		virtual bool GetIPFromHostName( Mitov::String AHostName, ::IPAddress &AAdress )
		{
			bool AResult = ( LWiFi.hostByName( AHostName.c_str(), AAdress ) == 1 );
			if( ! AResult )
				AAdress = INADDR_NONE;

			return AResult;
		}

	protected:
		void StopEthernet()
		{
			for( int i = 0; i < Sockets.size(); ++i )
				Sockets[ i ]->StopSocket();

			LWiFi.end();
			IsStarted = false;
		}

		void StartEthernet()
		{
/*
			if( ! AccessPoints.length() )
				return;

			if( ! Sockets.length() )
				return;
*/
			LWiFi.begin();
			for( int i = 0; i < AccessPoints.size(); ++i )
				if( AccessPoints[ i ]->Connect() )
				{
					IsStarted = true;
					if( MACOutputPin.IsConnected() )
					{
						uint8_t AMACAddress[VM_WLAN_WNDRV_MAC_ADDRESS_LEN] = {0};

						LWiFi.macAddress( AMACAddress );

						char AMACString[ VM_WLAN_WNDRV_MAC_ADDRESS_LEN * 3 + 1 ];
						sprintf( AMACString, "%02X-%02X-%02X-%02X-%02X-%02X", AMACString[ 0 ], AMACString[ 1 ], AMACString[ 2 ], AMACString[ 3 ], AMACString[ 4 ], AMACString[ 5 ] );
						MACOutputPin.Notify( AMACString );
					}

					if( AddressOutputPin.IsConnected() )
					{
						IPAddress ALocalIPAddress = LWiFi.localIP();
						char AIPString[ 4 * 4 + 1 ];
						sprintf( AIPString, "%u.%u.%u.%u", ALocalIPAddress[ 0 ], ALocalIPAddress[ 1 ], ALocalIPAddress[ 2 ], ALocalIPAddress[ 3 ] );

//						Mitov::String ALocalIPAddressString = Mitov::String( ALocalIPAddress[ 0 ] )  + "." + Mitov::String( ALocalIPAddress[ 1 ]  + "." + ALocalIPAddress[ 2 ]  + "." + ALocalIPAddress[ 3 ];
						AddressOutputPin.Notify( AIPString );
					}

					if( SignalStrengthOutputPin.IsConnected() )
					{
						int32_t ASetrength = LWiFi.RSSI();
						SignalStrengthOutputPin.Notify( &ASetrength );
					}

					if( BSSIDOutputPin.IsConnected() )
					{
						uint8_t AMACAddress[VM_WLAN_WNDRV_MAC_ADDRESS_LEN] = {0};

						LWiFi.BSSID( AMACAddress );

						char AMACString[ VM_WLAN_WNDRV_MAC_ADDRESS_LEN * 3 + 1 ];
						sprintf( AMACString, "%02X-%02X-%02X-%02X-%02X-%02X", AMACString[ 0 ], AMACString[ 1 ], AMACString[ 2 ], AMACString[ 3 ], AMACString[ 4 ], AMACString[ 5 ] );
						BSSIDOutputPin.Notify( AMACString );
					}

					if( GatewayIPOutputPin.IsConnected() )
					{
						IPAddress ALocalIPAddress = LWiFi.gatewayIP();
						char AIPString[ 4 * 4 + 1 ];
						sprintf( AIPString, "%u.%u.%u.%u", ALocalIPAddress[ 0 ], ALocalIPAddress[ 1 ], ALocalIPAddress[ 2 ], ALocalIPAddress[ 3 ] );

//						Mitov::String ALocalIPAddressString = Mitov::String( ALocalIPAddress[ 0 ] )  + "." + Mitov::String( ALocalIPAddress[ 1 ]  + "." + ALocalIPAddress[ 2 ]  + "." + ALocalIPAddress[ 3 ];
						AddressOutputPin.Notify( AIPString );
					}

					if( SubnetMaskIPOutputPin.IsConnected() )
					{
						IPAddress ALocalIPAddress = LWiFi.subnetMask();
						char AIPString[ 4 * 4 + 1 ];
						sprintf( AIPString, "%u.%u.%u.%u", ALocalIPAddress[ 0 ], ALocalIPAddress[ 1 ], ALocalIPAddress[ 2 ], ALocalIPAddress[ 3 ] );

//						Mitov::String ALocalIPAddressString = Mitov::String( ALocalIPAddress[ 0 ] )  + "." + Mitov::String( ALocalIPAddress[ 1 ]  + "." + ALocalIPAddress[ 2 ]  + "." + ALocalIPAddress[ 3 ];
						SubnetMaskIPOutputPin.Notify( AIPString );
					}

					break;
				}

			for( int i = 0; i < Sockets.size(); ++i )
				Sockets[ i ]->TryStartSocket();
		}

	public:
		void ScanNetworksInputPin_o_Receive( void *_Data )
		{
			int nearbyAccessPointCount = LWiFi.scanNetworks();
			for( int i = 0; i < nearbyAccessPointCount; i ++ )
			{
				FoundSSIDOutputPin.Notify( LWiFi.SSID( i ));

				int32_t ASetrength = LWiFi.RSSI( i );
				FoundSignalStrengthOutputPin.Notify( &ASetrength );
			}
		}

		void CheckSignalStrengthInputPin_o_Receive( void *_Data )
		{
			if( IsStarted )
				if( SignalStrengthOutputPin.IsConnected() )
				{
					int32_t ASetrength = LWiFi.RSSI();
					SignalStrengthOutputPin.Notify( &ASetrength );
				}

		}

	public:
		inline void SystemInit()
		{
			if( Enabled )
				StartEthernet();

		}

	public:
		LinkItWiFiModule() :
			Enabled( true ),
			IsStarted( false )
		{			
		}

	};
//---------------------------------------------------------------------------
	template<Mitov::LinkItWiFiModule &C_OWNER> class LinkItWiFiTCPClientSocket : public TCPClientSocket<Mitov::LinkItWiFiModule,C_OWNER,LWiFiClient>
	{
		typedef TCPClientSocket<Mitov::LinkItWiFiModule,C_OWNER,LWiFiClient> inherited;

	public:
		virtual bool CanSend() override
		{
			return inherited::Enabled && C_OWNER.Enabled && inherited::FClient;
		}

	public:
		using inherited::inherited;

	};
//---------------------------------------------------------------------------
	template<Mitov::LinkItWiFiModule &C_OWNER> class LinkItWiFiTCPServerSocket : public TCPServerSocket<Mitov::LinkItWiFiModule,C_OWNER,LWiFiServer,LWiFiClient>
	{
		typedef TCPServerSocket<Mitov::LinkItWiFiModule,C_OWNER,LWiFiServer,LWiFiClient> inherited;

	public:
		virtual bool CanSend() override
		{
			return inherited::Enabled && C_OWNER.Enabled; // && inherited::FClient;
		}

	public:
		using inherited::inherited;

	};
//---------------------------------------------------------------------------
	template<Mitov::LinkItWiFiModule &C_OWNER> class LinkItWiFiUDPSocket : public UDPSocket<Mitov::LinkItWiFiModule,C_OWNER,LWiFiUDP>
	{
		typedef UDPSocket<Mitov::LinkItWiFiModule,C_OWNER,LWiFiUDP> inherited;

	public:
		using inherited::inherited;

	};
//---------------------------------------------------------------------------
}

#endif
