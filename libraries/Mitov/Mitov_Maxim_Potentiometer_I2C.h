////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2018 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_MAXIM_POTENTIOMETER_I2C_h
#define _MITOV_MAXIM_POTENTIOMETER_I2C_h

#include <Mitov.h>
#include <Mitov_Basic_I2C.h>

namespace Mitov
{
//---------------------------------------------------------------------------
	class Maxim_Potentiometer_I2C_Channel
	{
	public:
		float	InitialValue = 0.0f;

	};
//---------------------------------------------------------------------------
	template<uint8_t C_ADDRESS, typename T_I2C, T_I2C &C_I2C> class Maxim_Potentiometer_I2C
	{
	public:
		bool	Enabled : 1;

	protected:
		bool	FModified : 1;

	public:
		Maxim_Potentiometer_I2C_Channel	Channels[ 1 ];

	public:
		void Channel_InputPin_o_Receive( int AIndex, void *_Data )
		{
			float AValue = constrain( *((float *)_Data), 0, 1 ) * 255;
			if( AValue == Channels[ AIndex ].InitialValue )
				return;

			Channels[ AIndex ].InitialValue = AValue;
			FModified = true;
		}

	protected:
		void TransmitAt( byte ARegister, byte Adata )
		{
			C_I2C.beginTransmission( C_ADDRESS );

			C_I2C.write( ARegister );
			C_I2C.write( Adata );

			C_I2C.endTransmission();
		}

	protected:
		void UpdateOutput()
		{
			float	ANewValue = Channels[ 0 ].InitialValue;

			TransmitAt( 0b00010001, (byte)ANewValue );

			FModified = false;
//			Channels[ 0 ]->FValue = ANewValue;
		}

	public:
		inline void SystemStart()
		{
			UpdateOutput();
		}

		inline void SystemLoopUpdateHardware()
		{
			if( FModified )
				UpdateOutput();

		}

	public:
		void ClockInputPin_o_Receive( void * )
		{
			if( FModified )
				UpdateOutput();
		
		}

	public:
		Maxim_Potentiometer_I2C() :
			Enabled( true ),
			FModified( false )
		{
		}
	};
//---------------------------------------------------------------------------
	template<uint8_t C_ADDRESS, typename T_I2C, T_I2C &C_I2C> class Maxim_Potentiometer_I2C2Channel
	{
	public:
		bool	Enabled : 1;

	protected:
		uint8_t	FModified : 2;

	public:
		Maxim_Potentiometer_I2C_Channel	Channels[ 2 ];

	public:
		void Channel_InputPin_o_Receive( int AIndex, void *_Data )
		{
			float AValue = constrain( *((float *)_Data), 0, 1 ) * 255;
			if( AValue == Channels[ AIndex ].InitialValue )
				return;

			Channels[ AIndex ].InitialValue = AValue;
			FModified |= 1 << AIndex;
		}

	protected:
		void TransmitAt( byte ARegister, byte Adata )
		{
			C_I2C.beginTransmission( C_ADDRESS );

			C_I2C.write( ARegister );
			C_I2C.write( Adata );

			C_I2C.endTransmission();
		}

	protected:
		void UpdateOutput()
		{
			float	ANewValue = Channels[ 0 ].InitialValue;
			if( ANewValue == Channels[ 1 ].InitialValue )
				// Update both channels at once
				TransmitAt( 0b00010011, (byte)ANewValue );

			else
			{
				if( FModified & 0b01 )
					TransmitAt( 0b00010001, (byte)ANewValue );

				if( FModified & 0b10 )
					TransmitAt( 0b00010010, (byte)Channels[ 1 ].InitialValue );
			}

			FModified = 0;
		}

	public:
		inline void SystemStart()
		{
			UpdateOutput();
		}

		inline void SystemLoopUpdateHardware()
		{
			if( FModified )
				UpdateOutput();

		}

	public:
		void ClockInputPin_o_Receive( void * )
		{
			if( FModified )
				UpdateOutput();
		
		}

	public:
		Maxim_Potentiometer_I2C2Channel() :
			Enabled( true ),
			FModified( 0 )
		{
		}
	};
//---------------------------------------------------------------------------
}

#endif
