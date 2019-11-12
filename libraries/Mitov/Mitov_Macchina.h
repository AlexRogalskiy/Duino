////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2018 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_MACCHINA_h
#define _MITOV_MACCHINA_h

#include <Mitov.h>

namespace Mitov
{
//---------------------------------------------------------------------------
	class ArduinoMacchinaPowerSupplyModule
	{
	public:
		OpenWire::SourcePin	IsPoweredOutputPin;
		OpenWire::SourcePin	OverCurrentOutputPin;

	public:
		bool	Enabled : 1;

	protected:
		bool	FIsPowered : 1;
		bool	FOverCurrent : 1;

	public:
		void UpdateConfig()
		{
			digitalWrite( I_SENSE_EN, ( Enabled ) ? HIGH : LOW );
		}

	protected:
		void UpdateOutputs( bool AChangeOnly )
		{
			if( IsPoweredOutputPin.IsConnected() )
			{
				bool AValue = ( digitalRead( I_SENSE ) == HIGH );
				if( ( ! AChangeOnly ) || ( AValue != FIsPowered ) )
				{
					FIsPowered = AValue;
					IsPoweredOutputPin.SendValue( FIsPowered );
				}
			}

			if( OverCurrentOutputPin.IsConnected() )
			{
				bool AValue = ( digitalRead( I_SENSE_INT ) == HIGH );
				if( ( ! AChangeOnly ) || ( AValue != FOverCurrent ) )
				{
					FOverCurrent = AValue;
					OverCurrentOutputPin.SendValue( FOverCurrent );
				}
			}
		}

	public:
		inline void SystemStart()
		{
			UpdateOutputs( true );
		}

		inline void SystemLoopBegin( unsigned long currentMicros )
		{
			UpdateOutputs( false );
		}

	public:
		ArduinoMacchinaPowerSupplyModule() :
			Enabled( false ),
			FIsPowered( false ),
			FOverCurrent( false )
		{
		}

	};
//---------------------------------------------------------------------------
}

#endif
