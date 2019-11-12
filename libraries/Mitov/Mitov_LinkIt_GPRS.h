////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2018 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_LINKIT_GPRS_h
#define _MITOV_LINKIT_GPRS_h

#include <Mitov.h>
#include <Mitov_BasicEthernet.h>
#include <LGPRS.h>
#include <LGPRSClient.h>
#include <LGPRSServer.h>
#include <LGPRSUdp.h>

namespace Mitov
{
//---------------------------------------------------------------------------
	class LinkItGPRSModule
	{
	public:
		Mitov::SimpleList<BasicEthernetSocket<LinkItGPRSModule>*>	Sockets;

	public:
		Mitov::SimpleObjectList<GPRSAccessPoint *>	AccessPoints;

		bool	Enabled : 1;

	protected:
		bool	FConnected : 1;

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
			bool AResult = ( LGPRS.hostByName( AHostName.c_str(), AAdress ) == 1 );
			if( ! AResult )
				AAdress = INADDR_NONE;

			return AResult;
		}

	protected:
		void StopEthernet()
		{
			for( int i = 0; i < Sockets.size(); ++i )
				Sockets[ i ]->StopSocket();

//			LWiFi.end();
		}

		void StartEthernet()
		{
			TryConnect( true, 0 );
			if( FConnected )
				for( int i = 0; i < Sockets.size(); ++i )
					Sockets[ i ]->TryStartSocket();
		}

		void TryConnect( bool FromStart, unsigned long currentMicros )
		{
			for( int i = 0; i < AccessPoints.size(); i ++ )
				if( AccessPoints[ i ]->Enabled )
				{
					bool ACanRetry = true;
					if( ! FromStart )
						if( ! AccessPoints[ i ]->CanRetry( currentMicros, ACanRetry ) )
							continue;

					if( AccessPoints[ i ]->AccessPointName != "" )
						FConnected = LGPRS.attachGPRS( AccessPoints[ i ]->AccessPointName.c_str(), AccessPoints[ i ]->UserName.c_str(), AccessPoints[ i ]->Password.c_str() );

					else
						FConnected = LGPRS.attachGPRS();

					if( FConnected )
						break;
				}

		}

	public:
		inline void SystemInit()
		{
			if( Enabled )
				StartEthernet();

		}

		inline void SystemLoopBegin( unsigned long currentMicros )
		{
			if( ! FConnected )
				TryConnect( false, currentMicros );

		}

	public:
		LinkItGPRSModule() :
			Enabled( true ),
			FConnected( false )
		{			
		}

	};
//---------------------------------------------------------------------------
}

#endif
