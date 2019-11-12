////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2018 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_GPRS_h
#define _MITOV_GPRS_h

#include <Mitov.h>
#include <Mitov_BasicEthernet.h>
#include <Mitov_GSMShield.h>
#include <GSM.h>

namespace Mitov
{
//---------------------------------------------------------------------------
	template<uint32_t C_ACESS_POINTS_COUNT, uint32_t C_SOCKETS_COUNT> class GPRSModule : public Mitov::GSMModule
	{
		typedef Mitov::GSMModule inherited;

	public:
		BasicEthernetSocket< GPRSModule<C_ACESS_POINTS_COUNT, C_SOCKETS_COUNT> > * Sockets[ C_SOCKETS_COUNT ];

	public:
		GPRSAccessPoint *AccessPoints[ C_ACESS_POINTS_COUNT ];

		bool	Enabled : 1;

	public:
		bool	IsStarted : 1;

	protected:
		bool	FConnected : 1;

	protected:
		GSMShield &FOwner;

	protected:
		GPRS	FGprs;

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

	protected:
		virtual void StartModule()
		{
			if( Enabled )
				StartEthernet();

			inherited::StartModule();
		}

		void StopEthernet()
		{
			for( int i = 0; i < C_SOCKETS_COUNT; ++i )
				Sockets[ i ]->StopSocket();

//			LWiFi.end();
			IsStarted = false;
		}

		void StartEthernet()
		{
			if( ! FOwner.IsStarted )
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
			for( int i = 0; i < C_SOCKETS_COUNT; ++i )
				Sockets[ i ]->TryStartSocket();

		}

		void TryConnect( bool FromStart, unsigned long currentMicros )
		{
			for( int i = 0; i < C_ACESS_POINTS_COUNT; i ++ )
				if( AccessPoints[ i ]->Enabled )
				{
					bool ACanRetry = true;
					if( ! FromStart )
						if( ! AccessPoints[ i ]->CanRetry( currentMicros, ACanRetry ) )
							continue;

					FConnected = FGprs.attachGPRS( (char *)AccessPoints[ i ]->AccessPointName.c_str(), (char *)AccessPoints[ i ]->UserName.c_str(), (char *)AccessPoints[ i ]->Password.c_str() );
					if( FConnected )
						break;

					AccessPoints[ i ]->FLastTime = currentMicros;
					++ AccessPoints[ i ]->FRetryCount;
				}

		}

	public:
		inline void SystemLoopBegin( unsigned long currentMicros )
		{
			if( Enabled )
				if( ! FConnected )
					if( FOwner.IsStarted )
						TryConnect( false, currentMicros );

		}

	public:
		GPRSModule( GSMShield &AOwner ) :
			FOwner( AOwner ),
			Enabled( true ),
			IsStarted( false ),
			FConnected( false )
		{			
			AOwner.Modules.push_back( this );
		}

	};
//---------------------------------------------------------------------------
}

#endif
