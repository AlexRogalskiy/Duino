////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2018 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_LOGIC_GATES_h
#define _MITOV_LOGIC_GATES_h

#include <Mitov.h>

namespace Mitov
{
	class BooleanOr
	{
	public:
		static bool CalculateOutput( uint8_t *AInputValues, uint16_t ANumInputs )
		{
			bool AValue = false;
			for ( int i = 0; i < ANumInputs; ++i )
				AValue |= (( AInputValues[ i / 8 ] & ( 1 << ( i & 0b111 ) )) != 0 );

			return AValue;
		}

	};
//---------------------------------------------------------------------------
	class BooleanAnd
	{
	public:
		static bool CalculateOutput( uint8_t *AInputValues, uint16_t ANumInputs )
		{
			bool AValue = (ANumInputs > 0);
			for ( int i = 0; i < ANumInputs; ++i )
				AValue &= (( AInputValues[ i / 8 ] & ( 1 << ( i & 0b111 ) )) != 0 );

			return AValue;
		}

	};
//---------------------------------------------------------------------------
	class BooleanXor
	{
	public:
		static bool CalculateOutput( uint8_t *AInputValues, uint16_t ANumInputs )
		{
			bool AValue = false;
			for ( int i = 0; i < ANumInputs; ++i )
				AValue ^= (( AInputValues[ i / 8 ] & ( 1 << ( i & 0b111 ) )) != 0 );

			return AValue;
		}

	};
//---------------------------------------------------------------------------
	template<typename T_IMPLEMENTATION_CLASS, int C_NUM_INPUTS> class BooleanOperation
	{
	public:
		OpenWire::SourcePin	OutputPin;

	protected:
		bool	FModified : 1; // For speed optimization!
		bool	FLastOutput : 1;

		uint8_t	FValues[ ( C_NUM_INPUTS + 7 ) / 8 ] = { 0 };

	public:
		void InputPins_o_Receive( int AIndex, void *_Data )
		{
			if( Func::SetBitField( FValues, AIndex, *(bool *)_Data ))
				FModified = true;

		}

	protected:
		inline void CallCalculateSendOutput( bool AFromStart ) // Work around compiler bug
		{
			CalculateSendOutput( AFromStart );
		}

		void CalculateSendOutput( bool AFromStart )
		{
//			Serial.println( "CalculateSendOutput+++" );
			bool AValue = T_IMPLEMENTATION_CLASS::CalculateOutput( FValues, C_NUM_INPUTS );
//			Serial.println( AValue );
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

	public:
		BooleanOperation() :
			FModified( false ),
			FLastOutput( false )
		{
		}

	};
//---------------------------------------------------------------------------
	class BooleanInverter
	{
	public:
		inline static bool CalculateOutput( bool AInValue )
        {
			return ! AInValue;
        }

	};
//---------------------------------------------------------------------------
}

#endif
