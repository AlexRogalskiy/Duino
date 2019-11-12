////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2018 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_ESP8266_WIFI_h
#define _MITOV_ESP8266_WIFI_h

#include <Mitov.h>
#include <Mitov_BasicEthernet.h>
#include <ESP8266WiFi.h>
//#include "ip_addr.h"
//#include "espconn.h"

#ifdef __TEST_FOR_DEBUG_PRINTS__
#define Serial UNGUARDED DEBUG PRINT!!!
#endif

namespace Mitov
{
	class ESP8266WiFiModuleOptionalChannel
	{
	public:
		uint8_t	Channel : 4;
		bool	Enabled : 1;

	public:
		ESP8266WiFiModuleOptionalChannel() :
			Channel( 1 ),
			Enabled( false )
		{
		}

	};
//---------------------------------------------------------------------------
	class ESP8266WiFiModuleOptionalIPConfig
	{
	public:
		bool	Enabled = false;
		::IPAddress	IP;

	};
//---------------------------------------------------------------------------
	class ESP8266WiFiModuleAccessPointConfig
	{
	public:
		bool		Enabled = false;

		::IPAddress	IP;
		::IPAddress	Gateway;
		::IPAddress	Subnet;

	};
//---------------------------------------------------------------------------
	class ESP8266WiFiModuleRemoteConfig : public ESP8266WiFiModuleAccessPointConfig
	{
	public:
		 ESP8266WiFiModuleOptionalIPConfig	DNS;
	};
//---------------------------------------------------------------------------
	class ESP8266WiFiModuleOptionalMacAddress
	{
	public:
		bool		Enabled = false;
		TMACAddress MacAddress;

	};
//---------------------------------------------------------------------------
	template<typename T_OWNER> class ESP8266ModuleRemoteAccessPoint
	{
	public:
		bool	Enabled = true;
		Mitov::String	SSID;
	    Mitov::String	Password;
		ESP8266WiFiModuleOptionalChannel	Channel;
		ESP8266WiFiModuleOptionalMacAddress	MacAddress;
		ESP8266WiFiModuleRemoteConfig		Config;

	public:
		bool Connect()
		{
			if( ! Enabled )
				return false;

			if( SSID == "" )
				return false;

			const char *APassword;
			if( Password == "" )
				APassword = nullptr;

			else
				APassword = Password.c_str();

			int32_t channel;

			if( Channel.Enabled )
				channel = Channel.Channel;

			else
				channel = 0;

			const uint8_t* bssid;

			if( MacAddress.Enabled )
				bssid = MacAddress.MacAddress.FMacAddress;

			else
				bssid = nullptr;

//			Serial.println( SSID );
//			Serial.println( APassword );

			if( Config.Enabled )
			{
				if( Config.DNS.Enabled )
					WiFi.config( Config.IP, Config.Gateway, Config.Subnet, Config.DNS.IP );

				else
					WiFi.config( Config.IP, Config.Gateway, Config.Subnet);
			}

			WiFi.begin( (char *)SSID.c_str(), APassword, channel, bssid );

			for(;;)
			{
//				Serial.print( "." );
				int ARes = WiFi.status();
//				Serial.print( ";" );
//				Serial.println( ARes );
				if( ARes == WL_CONNECTED )
					return true;

//				Serial.println( "+" );
				if( ARes == WL_CONNECT_FAILED )
					return false;

//				Serial.println( "++" );
				if( ARes == WL_NO_SSID_AVAIL )
					return false;

//				Serial.println( "+++" );
				delay(500);
			}

/*
			while (WiFi.status() != WL_CONNECTED) {
			WL_CONNECT_FAILED

			return ( ARes == WL_CONNECTED );
*/
//			return ( WiFi.begin( (char *)SSID.c_str(), APassword, channel, bssid ) == WL_CONNECTED );


//			return ( WiFi.begin( (char *)SSID.c_str() ) == WL_CONNECTED );


/*
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
*/
			return false;
		}

	public:
		ESP8266ModuleRemoteAccessPoint( T_OWNER &AOwner, uint32_t AIndex, TMACAddress AMacAddress )
		{
			AOwner.AccessPoints[ AIndex ] = this;
			MacAddress.MacAddress = AMacAddress;
		}

		ESP8266ModuleRemoteAccessPoint( T_OWNER &AOwner, uint32_t AIndex )
		{
			AOwner.AccessPoints[ AIndex ] = this;
		}

	};
//---------------------------------------------------------------------------
	template<typename T_OWNER> class ESP8266WiFiModuleAccessPoint
	{
	public:
		bool	Enabled : 1;
		bool	IsHidden : 1;
		uint8_t	Channel : 4;
		Mitov::String	SSID;
		Mitov::String	Password;
		ESP8266WiFiModuleAccessPointConfig	Config;

	public:
		void TryStart( T_OWNER *AOwner )
		{
			if( AOwner->Enabled )
				if( Enabled )
					if( SSID != "" )
					{
//Serial.println( "TEST1" );
//Serial.println( SSID );
//Serial.println( IsHidden );
						if( Config.Enabled )
							WiFi.softAPConfig( Config.IP, Config.Gateway, Config.Subnet );

						if( Password != "" )
							WiFi.softAP( SSID.c_str(), Password.c_str(), Channel, IsHidden );

						else
//								WiFi.softAP( SSID.c_str() );
							WiFi.softAP( SSID.c_str(), nullptr, Channel, IsHidden );

						WiFi.softAPIP();
					}

		}

	public:
		ESP8266WiFiModuleAccessPoint() :
			Enabled( true ),
			IsHidden( false ),
			Channel( 1 )
		{
		}
	};
//---------------------------------------------------------------------------
	class ESP8266ModuleScanNetworksOperation
	{
	public:
		OpenWire::SourcePin	CountOutputPin;

		OpenWire::SourcePin	SignalStrengthOutputPin;
		OpenWire::SourcePin	SSIDOutputPin;
		OpenWire::SourcePin	ChannelOutputPin;
		OpenWire::SourcePin	EncryptionOutputPin;
		OpenWire::SourcePin	IsHiddenOutputPin;
		OpenWire::SourcePin	MACAddressOutputPin;

		OpenWire::TypedSourcePin<bool>	ScanningOutputPin;
		OpenWire::SourcePin	FailedOutputPin;
		OpenWire::SourcePin	FoundNetworkOutputPin;

	protected:
		bool	FScanRequest = false;

	public:
		inline void SystemStart()
		{
			ScanInputPin_o_Receive( nullptr );
		}

		inline void SystemLoopBegin( unsigned long currentMicros )
		{
			ScanningOutputPin.SetValue( FScanRequest, true );
			if( FScanRequest )
			{
				int8_t AComplete = WiFi.scanComplete();
				if( AComplete == WIFI_SCAN_FAILED )
				{
					FailedOutputPin.Notify( nullptr );
					CountOutputPin.SendValue<uint32_t>( 0 );
					FScanRequest = false;
				}

				else if( AComplete >= 0 )
				{
					CountOutputPin.SendValue<uint32_t>( AComplete );
					for( int i = 0; i < AComplete; i ++ )
					{
						Mitov::String ssid;
						uint8_t encryptionType;
						int32_t ASetrength;
						uint8_t* BSSID;
						int32_t channel;
						bool isHidden;
						if( WiFi.getNetworkInfo( i, ssid, encryptionType, ASetrength, BSSID, channel, isHidden ))
						{
							Mitov::String BSSIDStr = WiFi.BSSIDstr( i );

							SSIDOutputPin.Notify( (void *)ssid.c_str() );
							SignalStrengthOutputPin.Notify( &ASetrength );
							EncryptionOutputPin.Notify( &encryptionType );
							MACAddressOutputPin.Notify( (void *)BSSIDStr.c_str() );
							ChannelOutputPin.Notify( &channel );
							IsHiddenOutputPin.Notify( &isHidden );
						}

						FoundNetworkOutputPin.Notify( nullptr );
					}

					FScanRequest = false;
				}
			}
		}

	public:
		void ScanInputPin_o_Receive( void *_Data )
		{
			if( WiFi.scanComplete() != WIFI_SCAN_RUNNING )
			{
				WiFi.scanNetworks( true );
				FScanRequest = true;
			}
/*
			for( int i = 0; i < nearbyAccessPointCount; i ++ )
			{
				FoundSSIDOutputPin.Notify( LWiFi.SSID( i ));

				int32_t ASetrength = LWiFi.RSSI( i );
				FoundSignalStrengthOutputPin.Notify( &ASetrength );
			}
*/
		}

	};
//---------------------------------------------------------------------------
	template<uint32_t C_ACESS_POINTS_COUNT, uint32_t C_SOCKETS_COUNT> class ESP8266WiFiModule
	{
	public:
		BasicEthernetSocket< ESP8266WiFiModule<C_ACESS_POINTS_COUNT, C_SOCKETS_COUNT> > * Sockets[ C_SOCKETS_COUNT ];

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
		OpenWire::SourcePin	AddressOutputPin;
		OpenWire::SourcePin	MACOutputPin;

		OpenWire::SourcePin	BSSIDOutputPin;
		OpenWire::SourcePin	GatewayIPOutputPin;
		OpenWire::SourcePin	SubnetMaskIPOutputPin;

		OpenWire::TypedSourcePin<bool>	RemoteConnectedOutputPin;

	public:
		ESP8266WiFiModuleAccessPoint< ESP8266WiFiModule<C_ACESS_POINTS_COUNT, C_SOCKETS_COUNT> >	AccessPoint;
		ESP8266ModuleRemoteAccessPoint< ESP8266WiFiModule<C_ACESS_POINTS_COUNT, C_SOCKETS_COUNT> > *AccessPoints[ C_ACESS_POINTS_COUNT ];

	public:
		Mitov::String	HostName;
		bool	Enabled : 1;
		bool	AutoReconnect : 1;

	public:
		bool	IsStarted : 1;
		bool	RemoteStarted : 1;

	public:
		inline void SetAutoReconnect( bool AValue )
		{
			WiFi.setAutoReconnect( AValue );
		}

	public:
		void UpdateAccessPoint()
		{
			if( AccessPoint.Enabled )
				AccessPoint.TryStart( this );

			else
				WiFi.softAPdisconnect();

		}

	public:
		inline void SystemInit()
		{
			if( Enabled )
				StartEthernet();

		}

	public:
		bool GetIPFromHostName( Mitov::String AHostName, ::IPAddress &AAdress )
		{
			bool AResult = ( WiFi.hostByName( AHostName.c_str(), AAdress ) == 1 );
			if( ! AResult )
				AAdress = INADDR_NONE;

			return AResult;
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

	protected:
		void StopEthernet()
		{
			for( int i = 0; i < C_SOCKETS_COUNT; ++i )
				Sockets[ i ]->StopSocket();

			WiFi.disconnect( true );
			IsStarted = false;
			RemoteStarted = false;
		}

		void StartEthernet()
		{
/*
			if( ! AccessPoints.length() )
				return;

			if( ! Sockets.length() )
				return;
*/
//			Serial.println( "TRY CONNECT" );
			if( HostName != "" )
			{
				WiFi.hostname( HostName );
//				espconn_mdns_set_hostname( (char *) HostName.c_str() ); 
			}

//			Serial.println( "TRY CONNECT1" );

			bool AConnected = false;
			for( int i = 0; i < C_ACESS_POINTS_COUNT; ++i )
				if( AccessPoints[ i ]->Connect() )
				{
					RemoteStarted = true;
					AConnected = true;
//					Serial.println( "CONNECT" );

					if( AddressOutputPin.IsConnected() )
					{
						Mitov::String IPAddress = IPAdressToString( WiFi.localIP());
						AddressOutputPin.Notify( (void *)IPAddress.c_str() );
					}

					if( MACOutputPin.IsConnected() )
					{
						Mitov::String AAddress = WiFi.macAddress();
						MACOutputPin.Notify( (void *)AAddress.c_str() );
					}

					IsStarted = true;
					break;
				}

			if( ! AConnected )
				WiFi.begin();

			RemoteConnectedOutputPin.SetValue( AConnected, true );

			WiFi.setAutoReconnect( AutoReconnect );

			AccessPoint.TryStart( this );

			if( BSSIDOutputPin.IsConnected() )
				BSSIDOutputPin.SendValue( WiFi.BSSIDstr() );

			if( GatewayIPOutputPin.IsConnected() )
				GatewayIPOutputPin.SendValue( IPAdressToString( WiFi.gatewayIP() ));

			if( SubnetMaskIPOutputPin.IsConnected() )
				SubnetMaskIPOutputPin.SendValue( IPAdressToString( WiFi.subnetMask() ));

			for( int i = 0; i < C_SOCKETS_COUNT; ++i )
				Sockets[ i ]->TryStartSocket();

		}

	public:
		inline void SystemStart()
		{
			if( RemoteStarted )
			{
				if( AddressOutputPin.IsConnected() )
				{
					Mitov::String IPAddress = IPAdressToString( WiFi.localIP());
					AddressOutputPin.Notify( (void *)IPAddress.c_str() );
				}

				if( MACOutputPin.IsConnected() )
				{
					Mitov::String AAddress = WiFi.macAddress();
					MACOutputPin.Notify( (void *)AAddress.c_str() );
				}
			}

			if( BSSIDOutputPin.IsConnected() )
				BSSIDOutputPin.SendValue( WiFi.BSSIDstr() );

			if( GatewayIPOutputPin.IsConnected() )
				GatewayIPOutputPin.SendValue( IPAdressToString( WiFi.gatewayIP() ));

			if( SubnetMaskIPOutputPin.IsConnected() )
				SubnetMaskIPOutputPin.SendValue( IPAdressToString( WiFi.subnetMask() ));
		}

		inline void SystemLoopBegin( unsigned long currentMicros ) // Called only for RemoteConnectedOutputPin.IsConnected
		{
			RemoteConnectedOutputPin.SetValue( WiFi.isConnected(), true );
		}

/*
	void DoCheckSignalStrength( void *_Data )
		{
			if( IsStarted )
				if( SignalStrengthOutputPin.IsConnected() )
				{
					int32_t ASetrength = WiFi.RSSI();
					SignalStrengthOutputPin.Notify( &ASetrength );
				}

		}
*/
	public:
		ESP8266WiFiModule() :
			Enabled( true ),
			AutoReconnect( true ),
			IsStarted( false ),
			RemoteStarted( false )
		{			
		}

	};
//---------------------------------------------------------------------------
	template<typename T_OWNER, T_OWNER &C_OWNER> class ESP8266ModuleSignalStrengthOperation
	{
	public:
		OpenWire::SourcePin	SignalStrengthOutputPin;

	public:
		void ClockInputPin_o_Receive( void *_Data )
		{
			if( C_OWNER.IsStarted )
				if( SignalStrengthOutputPin.IsConnected() )
				{
					int32_t ASetrength = WiFi.RSSI();
					SignalStrengthOutputPin.Notify( &ASetrength );
				}

		}

	};
//---------------------------------------------------------------------------
	class ESP8266ModuleReconnectOperation
	{
	public:
		inline void ClockInputPin_o_Receive( void *_Data )
		{
			WiFi.reconnect();
		}

	};
//---------------------------------------------------------------------------
}

#ifdef __TEST_FOR_DEBUG_PRINTS__
#undef Serial
#endif

#endif
