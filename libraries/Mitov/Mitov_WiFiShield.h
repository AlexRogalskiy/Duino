////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2018 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_WIFI_SHIELD_h
#define _MITOV_WIFI_SHIELD_h

#include <Mitov.h>
#include <Mitov_BasicEthernet.h>

namespace Mitov
{
//---------------------------------------------------------------------------
	template<typename TOWNER> class WiFiRemoteAccessPoint
	{
	public:
		bool	Enabled = true;
		Mitov::String	SSID;

	public:
		bool Connect()
		{
			if( ! Enabled )
				return false;

//			if( SSID == "" )
//				return ( WiFi.begin() == WL_CONNECTED );

			return IntConnect();
		}

	protected:
		virtual bool IntConnect()
		{
			return ( WiFi.begin( (char *)SSID.c_str() ) == WL_CONNECTED );
		}

	public:		
		WiFiRemoteAccessPoint( TOWNER &AOwner, uint32_t AIndex )
		{
			AOwner.AccessPoints[ AIndex ] = this;
		}

	};
//---------------------------------------------------------------------------
	template<typename TOWNER> class WiFiRemoteAccessPointWPA : public WiFiRemoteAccessPoint<TOWNER>
	{
		typedef WiFiRemoteAccessPoint<TOWNER> inherited;

	public:
		Mitov::String	Password;

	protected:
		virtual bool IntConnect()
		{
			return ( WiFi.begin( (char *)inherited::SSID.c_str(), (char *)Password.c_str() ) == WL_CONNECTED );
		}

	public:
		using inherited::inherited;

	};
//---------------------------------------------------------------------------
	template<typename TOWNER> class WiFiRemoteAccessPointWEP : public WiFiRemoteAccessPoint<TOWNER>
	{
		typedef WiFiRemoteAccessPoint<TOWNER> inherited;

	public:
		Mitov::String	Key;
		int	KeyIndex = 0;

	protected:
		virtual bool IntConnect()
		{
			return ( WiFi.begin( (char *)inherited::SSID.c_str(), KeyIndex, (char *)Key.c_str() ) == WL_CONNECTED );
		}

	public:
		using inherited::inherited;

	};
//---------------------------------------------------------------------------
	template<uint32_t C_ACESS_POINTS_COUNT, uint32_t C_SOCKETS_COUNT> class WiFiShield
	{
	public:
		BasicEthernetSocket< WiFiShield<C_ACESS_POINTS_COUNT, C_SOCKETS_COUNT> > * Sockets[ C_SOCKETS_COUNT ];

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
		ShieldIPDNS2Address	IPAddress;

	public:
		bool	Enabled : 1;

	public:
		bool	IsStarted : 1;

	public:
		WiFiRemoteAccessPoint< WiFiShield<C_ACESS_POINTS_COUNT, C_SOCKETS_COUNT> > *AccessPoints[ C_ACESS_POINTS_COUNT ];

	public:
		bool GetIPFromHostName( Mitov::String AHostName, ::IPAddress &AAdress )
		{
			bool AResult = ( WiFi.hostByName( AHostName.c_str(), AAdress ) == 1 );
			if( ! AResult )
				AAdress = INADDR_NONE;

			return AResult;
		}

	protected:
		void StartEthernet()
		{
//			int AIndex = ((int)Parity) * 2 * 4 + ( StopBits - 1 ) + ( DataBits - 5);
//			C_SERIAL.begin( Speed );

//			Serial.println( "StartEthernet" );
			for( int i = 0; i < C_ACESS_POINTS_COUNT; ++i )
				if( AccessPoints[ i ]->Connect() )
				{
					IsStarted = true;
					if( IPAddress.Enabled )
					{
						if( ! IPAddress.DNS.Enabled )
							WiFi.config( IPAddress.IP );

						else
						{
							if( ! IPAddress.DNS.Gateway.Enabled )
								WiFi.config( IPAddress.IP, IPAddress.DNS.IP );

							else
							{
								if( ! IPAddress.DNS.Gateway.Subnet.Enabled )
									WiFi.config( IPAddress.IP, IPAddress.DNS.IP, IPAddress.DNS.Gateway.IP );

								else
									WiFi.config( IPAddress.IP, IPAddress.DNS.IP, IPAddress.DNS.Gateway.IP, IPAddress.DNS.Gateway.Subnet.IP );
							}

							if( IPAddress.DNS2.Enabled )
								WiFi.setDNS( IPAddress.DNS.IP, IPAddress.DNS2.IP );

						}
					}

					if( MACOutputPin.IsConnected() )
					{
						uint8_t AMACAddress[6] = {0};

						WiFi.macAddress( AMACAddress );

						char AMACString[ 6 * 3 + 1 ];
						sprintf( AMACString, "%02X-%02X-%02X-%02X-%02X-%02X", AMACString[ 0 ], AMACString[ 1 ], AMACString[ 2 ], AMACString[ 3 ], AMACString[ 4 ], AMACString[ 5 ] );
						MACOutputPin.Notify( AMACString );
					}

					if( AddressOutputPin.IsConnected() )
					{
						Mitov::String AIPString = IPAdressToString( WiFi.localIP());
						AddressOutputPin.Notify( (void *)AIPString.c_str() );
					}

					if( SignalStrengthOutputPin.IsConnected() )
					{
						int32_t ASetrength = WiFi.RSSI();
						SignalStrengthOutputPin.Notify( &ASetrength );
					}

					if( BSSIDOutputPin.IsConnected() )
					{
						uint8_t AMACAddress[6] = {0};

						WiFi.BSSID( AMACAddress );

						char AMACString[ 6 * 3 + 1 ];
						sprintf( AMACString, "%02X-%02X-%02X-%02X-%02X-%02X", AMACString[ 0 ], AMACString[ 1 ], AMACString[ 2 ], AMACString[ 3 ], AMACString[ 4 ], AMACString[ 5 ] );
						BSSIDOutputPin.Notify( AMACString );
					}

					if( GatewayIPOutputPin.IsConnected() )
					{
						Mitov::String AIPString = IPAdressToString( WiFi.gatewayIP());
						AddressOutputPin.Notify( (void *)AIPString.c_str() );
					}

					if( SubnetMaskIPOutputPin.IsConnected() )
					{
						Mitov::String AIPString = IPAdressToString( WiFi.subnetMask());
						SubnetMaskIPOutputPin.Notify( (void *)AIPString.c_str() );
					}

					break;
				}
			
			for( int i = 0; i < C_SOCKETS_COUNT; ++i )
				Sockets[ i ]->TryStartSocket();
		}

		void StopEthernet()
		{
			for( int i = 0; i < C_SOCKETS_COUNT; ++i )
				Sockets[ i ]->StopSocket();

			WiFi.disconnect();
			IsStarted = false;
		}

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
        void RestartEthernet()
		{
			if( ! Enabled )
				return;

//			C_SERIAL.end();
//			Ethernet.end();
			StartEthernet();
		}

	public:
		void ScanNetworksInputPin_o_Receive( void *_Data )
		{
			int nearbyAccessPointCount = WiFi.scanNetworks();
			for( int i = 0; i < nearbyAccessPointCount; i ++ )
			{
				FoundSSIDOutputPin.Notify( WiFi.SSID( i ));

				int32_t ASetrength = WiFi.RSSI( i );
				FoundSignalStrengthOutputPin.Notify( &ASetrength );
			}
		}

		void CheckSignalStrengthInputPin_o_Receive( void *_Data )
		{
			if( IsStarted )
				if( SignalStrengthOutputPin.IsConnected() )
				{
					int32_t ASetrength = WiFi.RSSI();
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
		WiFiShield() :
			Enabled( true ),
			IsStarted( false )
		{
		}

		WiFiShield( ::IPAddress local_ip ) :
			Enabled( true ),
			IsStarted( false )
		{
			IPAddress.Enabled = true;
			IPAddress.IP = local_ip;
		}

		WiFiShield( ::IPAddress local_ip, ::IPAddress dns_server) :
			WiFiShield( local_ip )
		{

			IPAddress.DNS.Enabled = true;
			IPAddress.DNS.IP = dns_server;
		}

		WiFiShield( ::IPAddress local_ip, ::IPAddress dns_server, ::IPAddress gateway) :
			WiFiShield( local_ip, dns_server )
		{
			IPAddress.DNS.Gateway.Enabled = true;
			IPAddress.DNS.Gateway.IP = gateway;
		}

		WiFiShield( ::IPAddress local_ip, ::IPAddress dns_server, ::IPAddress gateway, ::IPAddress subnet) :
			WiFiShield( local_ip, dns_server, gateway )
		{
			IPAddress.DNS.Gateway.Subnet.Enabled = true;
			IPAddress.DNS.Gateway.Subnet.IP = subnet;
		}
	};
//---------------------------------------------------------------------------
}

#endif
