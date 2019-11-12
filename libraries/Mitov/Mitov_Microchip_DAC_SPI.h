////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2018 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_MICROCHIP_DAC_SPI_h
#define _MITOV_MICROCHIP_DAC_SPI_h

#include <Mitov.h>
#include <SPI.h>

namespace Mitov
{
//---------------------------------------------------------------------------
	template<typename T_OWNER, uint8_t C_MULTIPLIER> class Microchip_DACSPIChannel
	{
	public:
		float	InitialValue = 0.0f;

		bool	Enabled : 1;
		bool	Gain : 1;

	protected:
		bool	FValueModified : 1;

	public:
		void InputPin_o_Receive( void *_Data )
		{
			float AValue = *(float *)_Data;
			if( InitialValue == AValue )
				return;

			InitialValue = AValue;
			FValueModified = true;
		}

	public:
		void SendOutput( T_OWNER *AOwner, uint8_t AIndex )
		{
			if( ! FValueModified )
				return;

			FValueModified = true;

			word volt_digits = InitialValue * C_MULTIPLIER + 0.5;

			word packet = volt_digits & 0b111111111111;  //shift voltage setting digits
			if( Enabled )
				if( AOwner->Enabled )
					packet |= 1 << 12;

			if( Gain )
				packet |= 1 << 13;   //add gain setting

			if( AIndex )
				packet |= 1 << 15;   //add gain setting

			AOwner->SPI_write( packet );
		}

	public:
		inline void ValueModified()
		{
			FValueModified = true;
		}

	public:
		Microchip_DACSPIChannel() :
			Enabled( true ),
			Gain( false ),
			FValueModified( false )
		{
		}

	};
//---------------------------------------------------------------------------
	template<typename T_SPI, T_SPI &C_SPI, uint8_t C_COUNT_CHANNELS, uint8_t C_MULTIPLIER> class Microchip_DAC_SPI
	{
	public:
		OpenWire::SourcePin	ChipSelectOutputPin;
		OpenWire::SourcePin	LatchOutputPin;

	public:
		bool	Enabled = true;

	public:
		Microchip_DACSPIChannel<Microchip_DAC_SPI<T_SPI, C_SPI, C_COUNT_CHANNELS, C_MULTIPLIER>, C_MULTIPLIER>	Channels[ C_COUNT_CHANNELS ];

	public:
		void SetEnabled( bool AValue )
		{
			if( Enabled == AValue )
				return;

			Enabled = AValue;
			for( int i =0; i < C_COUNT_CHANNELS; ++i )
				Channels[ i ].ValueModified();
		}

	protected:
		void UpdateLatch()
		{
			LatchOutputPin.SendValue( false );
			LatchOutputPin.SendValue( true );
		}

	public:
		inline void SystemLoopEnd( unsigned long currentMicros )
		{
			ClockInputPin_o_Receive( nullptr );
		}

	public:
		void ClockInputPin_o_Receive( void *_Data )
		{
			UpdateLatch();
			for( int i = 0; i < C_COUNT_CHANNELS; ++i )
				Channels[ i ].SendOutput( this, i );

		}

	public:
		void SPI_write( word APacket )
		{
			ChipSelectOutputPin.SendValue( false );
			C_SPI.transfer( APacket >> 8 );
			C_SPI.transfer( APacket & 0xFF );
			ChipSelectOutputPin.SendValue( true );
		}

	};
//---------------------------------------------------------------------------
}

#endif
