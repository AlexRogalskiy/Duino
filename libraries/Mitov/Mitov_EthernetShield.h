////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2018 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_ETHERNET_SHIELD_h
#define _MITOV_ETHERNET_SHIELD_h

#include <Mitov.h>
#include <SPI.h>

#ifdef VISUINO_M_DUINO_ETHERNET_58
	#include <Ethernet2.h>
	#include <Dns.h>
#else

	#ifdef __SEED_ETHERNET_SHIELD_2_0__
	  #include <EthernetV2_0.h>
	  #include <DnsV2_0.h>
	#else
		#ifdef __ETHERNET_SHIELD_2_0__
			#include <Ethernet2.h>
		#else
			#include <Ethernet.h>
		#endif

		#include <Dns.h>
	#endif
#endif // VISUINO_M_DUINO_ETHERNET_58

#include <Mitov_BasicEthernet.h>

namespace Mitov
{
//---------------------------------------------------------------------------
	template<uint32_t C_SOCKETS_COUNT> class EthernetShield
	{
	public:
		BasicEthernetSocket<EthernetShield<C_SOCKETS_COUNT> > *Sockets[ C_SOCKETS_COUNT ];;

	protected:
		TMACAddress FMacAddress;

	public:
		ShieldIPAddress	IPAddress;
		bool	Enabled : 1;
		bool	UseDHCP : 1;

	protected:
//#ifdef VISUINO_M_DUINO_ETHERNET_58
//		virtual void StopEthernet()
//		{
//			inherited::StopEthernet();
//		}
//#endif
		virtual void StartEthernet()
		{
#ifdef VISUINO_M_DUINO_ETHERNET_58
			pinMode( 53, OUTPUT); // SPI_ETHERNET
			digitalWrite( 53,LOW);
#endif
//			int AIndex = ((int)Parity) * 2 * 4 + ( StopBits - 1 ) + ( DataBits - 5);
//			C_SERIAL.begin( Speed );

//			Serial.println( "StartEthernet" );

			if( ! IPAddress.Enabled )
				Ethernet.begin( FMacAddress.FMacAddress );

			else
			{
				if( UseDHCP )
					if( Ethernet.begin( FMacAddress.FMacAddress ))
						return;

				if( ! IPAddress.DNS.Enabled )
				{
/*
					Serial.println( "StartEthernet IP" );
					Serial.print( FMacAddress.FMacAddress[ 0 ] );
					Serial.print( FMacAddress.FMacAddress[ 1 ] );
					Serial.print( FMacAddress.FMacAddress[ 2 ] );
					Serial.print( FMacAddress.FMacAddress[ 3 ] );
					Serial.println();
					IPAddress.IP.printTo( Serial );
					Serial.println();
*/
					Ethernet.begin( FMacAddress.FMacAddress, IPAddress.IP );
				}

				else
				{
					if( ! IPAddress.DNS.Gateway.Enabled )
						Ethernet.begin( FMacAddress.FMacAddress, IPAddress.IP, IPAddress.DNS.IP );

					else
					{
						if( ! IPAddress.DNS.Gateway.Subnet.Enabled )
							Ethernet.begin( FMacAddress.FMacAddress, IPAddress.IP, IPAddress.DNS.IP, IPAddress.DNS.Gateway.IP );

						else
							Ethernet.begin( FMacAddress.FMacAddress, IPAddress.IP, IPAddress.DNS.IP, IPAddress.DNS.Gateway.IP, IPAddress.DNS.Gateway.Subnet.IP );

					}
				}
			}

			for( int i = 0; i < C_SOCKETS_COUNT; ++i )
				Sockets[ i ]->TryStartSocket();

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
		void StopEthernet()
		{
			for( int i = 0; i < C_SOCKETS_COUNT; ++i )
				Sockets[ i ]->StopSocket();
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
			DNSClient ADNSClient;

			ADNSClient.begin( Ethernet.dnsServerIP() );

			bool AResult = ( ADNSClient.getHostByName( AHostName.c_str(), AAdress ) == 1 );
			if( ! AResult )
				AAdress = INADDR_NONE;

			return AResult;
		}

	public:
		inline void SystemInit()
		{
			if( Enabled )
				StartEthernet();

		}

	public:
		EthernetShield( TMACAddress AMacAddress ) :
			FMacAddress( AMacAddress ),
			Enabled( true ),
			UseDHCP( false )
		{
		}

		EthernetShield( TMACAddress AMacAddress, bool UseDHCP, ::IPAddress local_ip) :
			FMacAddress( AMacAddress ),
			Enabled( true ),
			UseDHCP( UseDHCP )
		{
			IPAddress.Enabled = true;
			IPAddress.IP = local_ip;
		}

		EthernetShield( TMACAddress AMacAddress, bool UseDHCP, ::IPAddress local_ip, ::IPAddress dns_server) :
			FMacAddress( AMacAddress ),
			Enabled( true ),
			UseDHCP( UseDHCP )
		{
			IPAddress.Enabled = ! UseDHCP;
			IPAddress.IP = local_ip;
			IPAddress.DNS.Enabled = true;
			IPAddress.DNS.IP = dns_server;
		}

		EthernetShield( TMACAddress AMacAddress, bool UseDHCP, ::IPAddress local_ip, ::IPAddress dns_server, ::IPAddress gateway) :
			FMacAddress( AMacAddress ),
			Enabled( true ),
			UseDHCP( UseDHCP )
		{
			IPAddress.Enabled = ! UseDHCP;
			IPAddress.IP = local_ip;
			IPAddress.DNS.Enabled = true;
			IPAddress.DNS.IP = dns_server;
			IPAddress.DNS.Gateway.Enabled = true;
			IPAddress.DNS.Gateway.IP = dns_server;
		}

		EthernetShield( TMACAddress AMacAddress, bool UseDHCP, ::IPAddress local_ip, ::IPAddress dns_server, ::IPAddress gateway, ::IPAddress subnet) :
			FMacAddress( AMacAddress ),
			Enabled( true ),
			UseDHCP( UseDHCP )
		{
			IPAddress.Enabled = ! UseDHCP;
			IPAddress.IP = local_ip;
			IPAddress.DNS.Enabled = true;
			IPAddress.DNS.IP = dns_server;
			IPAddress.DNS.Gateway.Enabled = true;
			IPAddress.DNS.Gateway.IP = dns_server;
			IPAddress.DNS.Gateway.Subnet.Enabled = true;
			IPAddress.DNS.Gateway.Subnet.IP = dns_server;
		}
	};
//---------------------------------------------------------------------------
}

#endif
