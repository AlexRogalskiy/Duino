////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2018 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_PRIORITY_ENCODER_h
#define _MITOV_PRIORITY_ENCODER_h

#include <Mitov.h>

namespace Mitov
{
	template<int C_NUM_INPUTS> class PriorityEncoder
	{
	public:
		OpenWire::SourcePin	OutputPin;

	protected:
		bool	FModified = false;
		uint32_t	FLastOutput;

		uint8_t	FValues[ ( C_NUM_INPUTS + 7 ) / 8 ] = { 0 };

	public:
		void InputPins_o_Receive( int AIndex, void *_Data )
		{
			if( Func::SetBitField( FValues, AIndex, *(bool *)_Data ))
				FModified = true;

		}

	protected:
		uint32_t CalculateOutput()
		{
			for( uint32_t i = C_NUM_INPUTS; i--; )
				if(( FValues[ i / 8 ] & ( 1 << ( i & 0b111 ))) != 0 )
					return i;

			return 0;
		}

		void CalculateSendOutput( bool AFromStart )
		{
			uint32_t AValue = CalculateOutput();

			if( ! AFromStart )
				if( FLastOutput == AValue )
					return;

			OutputPin.SendValue( AValue ); // Use Send Value for String Compatibility!
			FLastOutput = AValue;

			FModified = false;
		}

	public:
		inline void SystemStart()
		{
			CalculateSendOutput( true );
		}

		inline void SystemLoopEnd( unsigned long currentMicros )
		{
			if( FModified )
				CalculateSendOutput( false );

		}

	};
//---------------------------------------------------------------------------
}

#endif
