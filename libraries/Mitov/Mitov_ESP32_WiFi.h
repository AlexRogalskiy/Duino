////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2018 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_ESP32_WIFI_h
#define _MITOV_ESP32_WIFI_h

#include <Mitov.h>
#include <Mitov_BasicEthernet.h>
#include <WiFi.h>
//#include "ip_addr.h"
//#include "espconn.h"

#ifdef __TEST_FOR_DEBUG_PRINTS__
#define Serial UNGUARDED DEBUG PRINT!!!
#endif

namespace Mitov
{
//---------------------------------------------------------------------------
	class ESP32WiFiModuleOptionalChannel
	{
	public:
		bool	Enabled : 1;
		unsigned long	Channel : 4;

	public:
		ESP32WiFiModuleOptionalChannel() :
			Enabled( false ),
			Channel( 1 )
		{
		}

	};
//---------------------------------------------------------------------------
	class ESP32WiFiModuleOptionalIPConfig
	{
	public:
		bool	Enabled = false;
		::IPAddress	IP;

	};
//---------------------------------------------------------------------------
	class ESP32WiFiModuleAccessPointConfig
	{
	public:
		bool		Enabled = false;

		::IPAddress	IP;
		::IPAddress	Gateway;
		::IPAddress	Subnet;

	};
//---------------------------------------------------------------------------
	class ESP32WiFiModuleRemoteConfig : public ESP32WiFiModuleAccessPointConfig
	{
	public:
		 ESP32WiFiModuleOptionalIPConfig	DNS;
	};
//---------------------------------------------------------------------------
	class ESP32WiFiModuleOptionalMacAddress
	{
	public:
		bool	Enabled = false;
		TMACAddress MacAddress;

	};
//---------------------------------------------------------------------------
	template<typename TOWNER> class ESP32ModuleRemoteAccessPoint
	{
	public:
		bool	Enabled = true;
		Mitov::String	SSID;
	    Mitov::String	Password;
		ESP32WiFiModuleOptionalChannel	Channel;
		ESP32WiFiModuleOptionalMacAddress	MacAddress;
		ESP32WiFiModuleRemoteConfig		Config;

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
//				Serial.print( ARes );
				if( ARes == WL_CONNECTED )
					return true;

				if( ARes == WL_CONNECT_FAILED )
					return false;

				if( ARes == WL_NO_SSID_AVAIL )
					return false;

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
		}

	public:
		ESP32ModuleRemoteAccessPoint( TOWNER &AOwner, uint32_t AIndex, TMACAddress AMacAddress )
		{
			MacAddress.MacAddress = AMacAddress;
			AOwner.AccessPoints[ AIndex ] = this;
		}

		ESP32ModuleRemoteAccessPoint( TOWNER &AOwner, uint32_t AIndex )
		{
			AOwner.AccessPoints[ AIndex ] = this;
		}

	};
//---------------------------------------------------------------------------
	template<typename T_OWNER> class ESP32WiFiModuleAccessPoint
	{
	public:
		bool	Enabled : 1;
		bool	IsHidden : 1;
		unsigned long	Channel : 4;

		Mitov::String	SSID;
		Mitov::String	Password;


		ESP32WiFiModuleAccessPointConfig	Config;

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
	//						WiFi.softAP( SSID.c_str() );
							WiFi.softAP( SSID.c_str(), nullptr, Channel, IsHidden );

						WiFi.softAPIP();
					}

		}

	public:
		ESP32WiFiModuleAccessPoint() :
			Enabled( true ),
			IsHidden( false ),
			Channel( 1 )
		{
		}
	};
//---------------------------------------------------------------------------
	class ESP32ModuleScanNetworksOperation
	{
	public:
		OpenWire::SourcePin	CountOutputPin;

		OpenWire::SourcePin	SignalStrengthOutputPin;
		OpenWire::SourcePin	SSIDOutputPin;
		OpenWire::SourcePin	ChannelOutputPin;
		OpenWire::SourcePin	EncryptionOutputPin;
//		OpenWire::SourcePin	IsHiddenOutputPin;
		OpenWire::SourcePin	MACAddressOutputPin;

		OpenWire::TypedSourcePin<bool>	ScanningOutputPin;
		OpenWire::SourcePin	FailedOutputPin;
		OpenWire::SourcePin	FoundNetworkOutputPin;

	public:
		bool	ShowHidden = false;

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
					CountOutputPin.SendValue( 0 );
					FScanRequest = false;
				}

				else if( AComplete >= 0 )
				{
					CountOutputPin.SendValue( (int)AComplete );
					for( int i = 0; i < AComplete; i ++ )
					{
						Mitov::String ssid;
						uint8_t encryptionType;
						int32_t ASetrength;
						uint8_t* BSSID;
						int32_t channel;
//						bool isHidden;
						if( WiFi.getNetworkInfo( i, ssid, encryptionType, ASetrength, BSSID, channel )) //, isHidden ))
						{
							Mitov::String BSSIDStr = WiFi.BSSIDstr( i );

							SSIDOutputPin.Notify( (void *)ssid.c_str() );
							SignalStrengthOutputPin.Notify( &ASetrength );
							EncryptionOutputPin.Notify( &encryptionType );
							MACAddressOutputPin.Notify( (void *)BSSIDStr.c_str() );
							ChannelOutputPin.Notify( &channel );
//							IsHiddenOutputPin.Notify( &isHidden );
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
//				WiFi.scanNetworks( true, ShowHidden );
				WiFi.scanNetworks( false, ShowHidden );
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
	template<typename T_OWNER, T_OWNER &C_OWNER, typename T_CLIENT>class TCPESP32SecureClientSocket : public TCPSecureClientSocket<T_OWNER, C_OWNER, T_CLIENT>
	{
		typedef TCPSecureClientSocket<T_OWNER, C_OWNER, T_CLIENT> inherited;

	public:
		Mitov::String	CACert;

	protected:
		virtual void StartSocket() override
		{
			if( CACert != "" )
				inherited::FClient.setCACert( CACert.c_str() );

			inherited::StartSocket();
		}

	public:
		using inherited::inherited;

	};
//---------------------------------------------------------------------------
	template<uint32_t C_ACESS_POINTS_COUNT, uint32_t C_SOCKETS_COUNT> class ESP32WiFiModule
	{
	public:
		BasicEthernetSocket< ESP32WiFiModule<C_ACESS_POINTS_COUNT, C_SOCKETS_COUNT> > * Sockets[ C_SOCKETS_COUNT ];

	public:
		bool	Enabled = true;

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
		inline void SystemInit()
		{
			if( Enabled )
				StartEthernet();

		}

	public:
		OpenWire::SourcePin	AddressOutputPin;
		OpenWire::SourcePin	MACOutputPin;

		OpenWire::SourcePin	BSSIDOutputPin;
		OpenWire::SourcePin	GatewayIPOutputPin;
		OpenWire::SourcePin	SubnetMaskIPOutputPin;

		OpenWire::TypedSourcePin<bool>	RemoteConnectedOutputPin;

	public:
		ESP32WiFiModuleAccessPoint< ESP32WiFiModule<C_ACESS_POINTS_COUNT, C_SOCKETS_COUNT> >	AccessPoint;
		ESP32ModuleRemoteAccessPoint< ESP32WiFiModule<C_ACESS_POINTS_COUNT, C_SOCKETS_COUNT> > *AccessPoints[ C_ACESS_POINTS_COUNT ];

	public:
		Mitov::String	HostName;
		bool	AutoReconnect : 1;

	public:
		bool	IsStarted : 1;

	public:
		void UpdateAccessPoint()
		{
			if( AccessPoint.Enabled )
				AccessPoint.TryStart( this );

			else
				WiFi.softAPdisconnect();

		}

        void RestartEthernet()
		{
			if( ! Enabled )
				return;

//			C_SERIAL.end();
//			Ethernet.end();
			StartEthernet();
		}

		bool GetIPFromHostName( Mitov::String AHostName, ::IPAddress &AAdress )
		{
			bool AResult = ( WiFi.hostByName( AHostName.c_str(), AAdress ) == 1 );
			if( ! AResult )
				AAdress = INADDR_NONE;

			return AResult;
		}

	protected:

		void StopEthernet()
		{
			for( int i = 0; i < C_SOCKETS_COUNT; ++i )
				Sockets[ i ]->StopSocket();

			WiFi.disconnect( true );
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
//			Serial.println( "TRY CONNECT" );

			bool AConnected = false;
			for( int i = 0; i < C_ACESS_POINTS_COUNT; ++i )
				if( AccessPoints[ i ]->Connect() )
				{
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

			if( HostName != "" )
			{
				WiFi.setHostname( (char *)HostName.c_str() );
//				espconn_mdns_set_hostname( (char *) HostName.c_str() ); 
			}

			RemoteConnectedOutputPin.SetValue( AConnected, true );

//			WiFi.setAutoReconnect( AutoReconnect ); // For some reason not yet implemented!

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
		ESP32WiFiModule() :
			IsStarted( false ),
			AutoReconnect( true )
		{			
		}

	};
//---------------------------------------------------------------------------
	template<typename T_OWNER, T_OWNER &C_OWNER> class ESP32ModuleSignalStrengthOperation
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
	class ESP32ModuleReconnectOperation
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
