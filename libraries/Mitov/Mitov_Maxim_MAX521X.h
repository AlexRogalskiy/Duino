////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2018 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_MAXIM_MAX521X_h
#define _MITOV_MAXIM_MAX521X_h

#include <Mitov.h>
#include <SPI.h>

namespace Mitov
{
//---------------------------------------------------------------------------
	enum Maxim_MAX521X_ShutDownMode { msdmHighImpedance, msdm100K, msdm1K };
//---------------------------------------------------------------------------
	template <typename T_SPI, T_SPI &C_SPI> class Maxim_MAX5214
	{
	public:
		static void SPI_transfer( byte AAddress, word AData )
		{
			AData >>= 2;
			AData |= ((word)AAddress) << 14;
			C_SPI.transfer16( AData );
		}

	};
//---------------------------------------------------------------------------
	template <typename T_SPI, T_SPI &C_SPI> class Maxim_MAX5216
	{
	public:
		static void SPI_transfer( byte AAddress, word AData )
		{
			byte AHighByte = AData >> 10;
			AHighByte |= AAddress << 6;

			AData <<= 6;

			C_SPI.transfer( AHighByte );
			C_SPI.transfer16( AData );
		}

	};
//---------------------------------------------------------------------------
	template <typename T_IMPLEMENTATION_CLASS> class Maxim_MAX521X
	{
	public:
		OpenWire::SourcePin	ChipSelectOutputPin;

	public:
		bool	ClockInputPin_o_IsConnected : 1;

	public:
		bool	Enabled : 1;
		Maxim_MAX521X_ShutDownMode	ShutDownMode : 2;

	public:
		void SetEnabled( bool AValue )
		{
			if( Enabled == AValue )
				return;

			Enabled = AValue;
			if( Enabled )
				SPI_write( 2, 0 );

			else
				SPI_write( 2, ( ((word)ShutDownMode) + 1 ) << 10 );

		}

	protected:
		word FValue = 0;

	protected:
		inline void UpdateValue()
		{
			SPI_write( 1, FValue );
		}

	protected:
		void SPI_write( byte AAddress, word AData )
		{
			ChipSelectOutputPin.SendValue( false );
			T_IMPLEMENTATION_CLASS::SPI_transfer( AAddress, AData );
			ChipSelectOutputPin.SendValue( true );
		}

	public:
		inline void ClockInputPin_o_Receive( void *_Data )
		{
			UpdateValue();
		}

		void InputPin_o_Receive( void *_Data )
        {
			FValue = constrain( *(float*)_Data, 0, 1 ) * 65536 + 0.5;
			if( ! ClockInputPin_o_IsConnected )
				UpdateValue();
        }

	public:
		Maxim_MAX521X() :
			ClockInputPin_o_IsConnected( false ),
			Enabled( true ),
			ShutDownMode( msdmHighImpedance )
		{
		}
	};
//---------------------------------------------------------------------------
}

#endif
