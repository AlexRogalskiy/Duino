////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2018 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_COMMON_MUX_h
#define _MITOV_COMMON_MUX_h

#include <Mitov.h>

namespace Mitov
{
	template<typename T> class ToggleSwitch
	{
	public:
		OpenWire::SourcePin	OutputPin;

	public:
		bool	InitialSelectValue = false;

	protected:
		T	FTrueValue;
		T	FFalseValue;
		T	FLastOutput;

	public:
		inline void TrueInputPin_o_Receive( void *_Data )
		{
//			FTrueValue = *(T *)_Data;
			FTrueValue = ::Mitov::template ExtractValue<T>( _Data );
		}

		inline void FalseInputPin_o_Receive( void *_Data )
		{
//			FFalseValue = *(T *)_Data;
			FFalseValue = ::Mitov::template ExtractValue<T>( _Data );
		}

		void SelectInputPin_o_Receive( void *_Data )
		{
			InitialSelectValue = *(bool *)_Data;
			CalculateSendOutput( false );
		}

	protected:
		void CalculateSendOutput( bool AFromStart )
		{
			T AValue = ( InitialSelectValue ) ? FTrueValue : FFalseValue;
			if( ! AFromStart )
				if( FLastOutput == AValue )
					return;

			OutputPin.SendValue( AValue ); // Use Send Value for String Compatibility!
			FLastOutput = AValue;
		}

	public:
		inline void SystemStart()
		{
			CalculateSendOutput( true );
		}

		inline void SystemLoopEnd( unsigned long currentMicros )
		{
			CalculateSendOutput( false );
		}

	public:
		ToggleSwitch( T AInitValue ) :
			FTrueValue( AInitValue ),
			FFalseValue( AInitValue )
		{
		}

	};
//---------------------------------------------------------------------------
/*
	template<> void ToggleSwitch<Mitov::String>::TrueInputPin_o_Receive( void *_Data )
	{
		FTrueValue = (char *)_Data;
	}
//---------------------------------------------------------------------------
	template<> void ToggleSwitch<Mitov::String>::FalseInputPin_o_Receive( void *_Data )
	{
		FFalseValue = (char *)_Data;
	}
*/
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
	class DigitalToggleSwitch
	{
	public:
		OpenWire::SourcePin	OutputPin;

	public:
		bool	InitialSelectValue : 1;

	protected:
		bool	FTrueValue : 1;
		bool	FFalseValue : 1;
		bool	FLastOutput : 1;

	public:
		inline void TrueInputPin_o_Receive( void *_Data )
		{
			FTrueValue = *(bool *)_Data;
		}

		inline void FalseInputPin_o_Receive( void *_Data )
		{
			FFalseValue = *(bool *)_Data;
		}

		void SelectInputPin_o_Receive( void *_Data )
		{
			InitialSelectValue = *(bool *)_Data;
			CalculateSendOutput( false );
		}

	protected:
		void CalculateSendOutput( bool AFromStart )
		{
			bool AValue = ( InitialSelectValue ) ? FTrueValue : FFalseValue;
			if( ! AFromStart )
				if( FLastOutput == AValue )
					return;

			OutputPin.SendValue( AValue ); // Use Send Value for String Compatibility!
			FLastOutput = AValue;
		}

	public:
		inline void SystemStart()
		{
			CalculateSendOutput( true );
		}

		inline void SystemLoopEnd( unsigned long currentMicros )
		{
			CalculateSendOutput( false );
		}

	public:
		DigitalToggleSwitch() :
			InitialSelectValue( false ),
			FTrueValue( false ),
			FFalseValue( false ),
			FLastOutput( false )
		{
		}

	};
//---------------------------------------------------------------------------
	class ClockToggleSwitch
	{
	public:
		OpenWire::SourcePin	OutputPin;

	public:
		bool	InitialSelectValue = false;

	public:
		inline void SelectInputPin_o_Receive( void *_Data )
		{
			InitialSelectValue =  *(bool *)_Data;
		}

		void TrueInputPin_o_Receive( void *_Data )
		{
			if( InitialSelectValue )
				OutputPin.Notify( nullptr );
		}

		void FalseInputPin_o_Receive( void *_Data )
		{
			if( ! InitialSelectValue )
				OutputPin.Notify( nullptr );
		}

	};
//---------------------------------------------------------------------------
	template<typename T, int C_NUM_INPUTS> class CommonMux
	{
	public:
		OpenWire::SourcePin	OutputPin;

	public:
		uint8_t	InitialChannel = 0;

	protected:
		T		FLastOutput;
		T		FValues[ C_NUM_INPUTS ] = { 0 };

	public:
		inline void InputPins_o_Receive( int AIndex, void *_Data )
		{
			FValues[ AIndex ] = *(T *)_Data;
		}

		void SelectInputPin_o_Receive( void *_Data )
		{
			uint32_t AChannel = *(uint32_t *)_Data;
			if( AChannel >= C_NUM_INPUTS )
				AChannel = C_NUM_INPUTS - 1;

			if( InitialChannel == AChannel )
				return;

//			Serial.println( AChannel );

			InitialChannel = AChannel;
			CalculateSendOutput( false );
		}

	protected:
		void CalculateSendOutput( bool AFromStart )
		{
//			Serial.println( "CalculateSendOutput+++" );
			T AValue = FValues[ InitialChannel ];
//			Serial.println( AValue );
			if( ! AFromStart )
				if( FLastOutput == AValue )
					return;

			OutputPin.SendValue( AValue ); // Use Send Value for String Compatibility!
			FLastOutput = AValue;
		}

	public:
		inline void SystemStart()
		{
			CalculateSendOutput( true );
		}

		inline void SystemLoopEnd( unsigned long currentMicros )
		{
			CalculateSendOutput( false );
		}

	};
//---------------------------------------------------------------------------
	template<int C_NUM_INPUTS> class CommonMux<Mitov::String, C_NUM_INPUTS>
	{
	public:
		OpenWire::SourcePin	OutputPin;

	public:
		uint8_t	InitialChannel = 0;

	protected:
		Mitov::String		FLastOutput;
		Mitov::String		FValues[ C_NUM_INPUTS ];

	public:
		inline void InputPins_o_Receive( int AIndex, void *_Data )
		{
			FValues[ AIndex ] = (char *)_Data;
		}

		void SelectInputPin_o_Receive( void *_Data )
		{
			uint32_t AChannel = *(uint32_t *)_Data;
			if( AChannel >= C_NUM_INPUTS )
				AChannel = C_NUM_INPUTS - 1;

			if( InitialChannel == AChannel )
				return;

//			Serial.println( AChannel );

			InitialChannel = AChannel;
			CalculateSendOutput( false );
		}

	protected:
		void CalculateSendOutput( bool AFromStart )
		{
//			Serial.println( "CalculateSendOutput+++" );
			Mitov::String AValue = FValues[ InitialChannel ];
//			Serial.println( AValue );
			if( ! AFromStart )
				if( FLastOutput == AValue )
					return;

			OutputPin.SendValue( AValue ); // Use Send Value for String Compatibility!
			FLastOutput = AValue;
		}

	public:
		inline void SystemStart()
		{
			CalculateSendOutput( true );
		}

		inline void SystemLoopEnd( unsigned long currentMicros )
		{
			CalculateSendOutput( false );
		}

	};
//---------------------------------------------------------------------------
	template<int C_NUM_INPUTS> class DigitalMux
	{
	public:
		OpenWire::SourcePin	OutputPin;

	public:
		uint8_t	InitialChannel = 0;

	protected:
		bool		FLastOutput;
		uint8_t		FValues[ ( C_NUM_INPUTS + 7 ) / 8 ] = { 0 };

	public:
		void InputPins_o_Receive( int AIndex, void *_Data )
		{
			bool AValue = *(bool *)_Data;
			if( AValue )
				FValues[ AIndex / 8 ] |= 1 << ( AIndex & 0b111 );  // same as % 8

			else
				FValues[ AIndex / 8 ] &= ~( 1 << ( AIndex & 0b111 ));  // same as % 8

		}

		void SelectInputPin_o_Receive( void *_Data )
		{
			uint32_t AChannel = *(uint32_t *)_Data;
			if( AChannel >= C_NUM_INPUTS )
				AChannel = C_NUM_INPUTS - 1;

			if( InitialChannel == AChannel )
				return;

//			Serial.println( AChannel );

			InitialChannel = AChannel;
			CalculateSendOutput( false );
		}

	protected:
		void CalculateSendOutput( bool AFromStart )
		{
//			Serial.println( "CalculateSendOutput+++" );
			bool AValue = (( FValues[ InitialChannel / 8 ] & ( 1 << ( InitialChannel & 0b111 ))) != 0 );
//			Serial.println( AValue );
			if( ! AFromStart )
				if( FLastOutput == AValue )
					return;

			OutputPin.SendValue( AValue ); // Use Send Value for String Compatibility!
			FLastOutput = AValue;
		}

	public:
		inline void SystemStart()
		{
			CalculateSendOutput( true );
		}

		inline void SystemLoopEnd( unsigned long currentMicros )
		{
			CalculateSendOutput( false );
		}

	};
//---------------------------------------------------------------------------
	template<int C_NUM_INPUTS> class ClockMux
	{
	public:
		OpenWire::SourcePin	OutputPin;

	public:
		uint8_t	InitialChannel = 0;

	public:
		void InputPins_o_Receive( int AIndex, void *_Data )
		{
			if( AIndex == InitialChannel )
				OutputPin.Notify( nullptr );
		}

		void SelectInputPin_o_Receive( void *_Data )
		{
			uint32_t AChannel = *(uint32_t *)_Data;
			if( AChannel >= C_NUM_INPUTS )
				AChannel = C_NUM_INPUTS - 1;

			InitialChannel = AChannel;
		}
	};
//---------------------------------------------------------------------------
}

#endif
