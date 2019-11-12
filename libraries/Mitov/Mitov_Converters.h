////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2018 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_CONVERTERS_h
#define _MITOV_CONVERTERS_h

#include <Mitov.h>

#ifdef __TEST_FOR_DEBUG_PRINTS__
#define Serial UNGUARDED DEBUG PRINT!!!
#endif

namespace Mitov
{
//---------------------------------------------------------------------------
	template<typename T> class TypedToAnalog
	{
	public:
		OpenWire::SourcePin	OutputPin;

	public:
		float	Scale = 1.0;

	public:
		void InputPin_o_Receive( void *_Data )
		{
			float AFloatValue = ( *(T*)_Data ) * Scale;
			OutputPin.Notify( &AFloatValue );
		}

	};
//---------------------------------------------------------------------------
	class BinaryToText
	{
	public:
		OpenWire::SourcePin	OutputPin;

	public:
		void InputPin_o_Receive( void *_Data )
		{
			Mitov::TDataBlock ADataBlock = *(Mitov::TDataBlock*)_Data;
			OutputPin.SendValue( ADataBlock.ToString() );
		}

	};
//---------------------------------------------------------------------------
	class AnalogToInteger
	{
	public:
		OpenWire::SourcePin	OutputPin;

	public:
		bool	Round = true;
		float	Scale = 1.0;

	public:
		void InputPin_o_Receive( void *_Data )
		{
			float AFloatValue = *(float*)_Data * Scale;
			if( Round )
				AFloatValue += 0.5f;

			long AIntValue = AFloatValue;
			OutputPin.Notify( &AIntValue );
		}

	};
//---------------------------------------------------------------------------
	class AnalogToUnsigned
	{
	public:
		OpenWire::SourcePin	OutputPin;

	public:
		bool	Round : 1;
		bool	Constrain : 1;
		float	Scale = 1.0;

	public:
		void InputPin_o_Receive( void *_Data )
		{
			float AFloatValue = *(float*)_Data * Scale;
			if( Round )
				AFloatValue += 0.5f;

			long AIntValue = AFloatValue;
			if( Constrain )
				AIntValue &= 0x7FFFFFFF;

			OutputPin.Notify( &AIntValue );
		}

	public:
		AnalogToUnsigned() : 
			Round( true ),
			Constrain( true )
		{
		}
	};
//---------------------------------------------------------------------------
	class AnalogToText
	{
	public:
		OpenWire::SourcePin	OutputPin;

	public:
		uint8_t	MinWidth : 5;
		uint8_t	Precision : 5;

	public:
		void InputPin_o_Receive( void *_Data )
		{
			float AFloatValue = *(float*)_Data;
			char AText[ 16 ];
			dtostrf( AFloatValue,  MinWidth, Precision, AText );
			OutputPin.Notify( AText );
		}

	public:
		AnalogToText() :
			MinWidth( 1 ),
			Precision( 3 )
		{
		}
	};
//---------------------------------------------------------------------------
	template<typename T> class TypedToText
	{
	public:
		OpenWire::SourcePin	OutputPin;

	public:
		uint8_t	Base = 10;

	public:
		void InputPin_o_Receive( void *_Data )
		{
			T AValue = *(T*)_Data;
			char AText[ 16 ];
			ltoa( AValue, AText, Base );
			OutputPin.Notify( AText );
		}

	};
//---------------------------------------------------------------------------
	class DigitalToText
	{
	public:
		OpenWire::SourcePin	OutputPin;

	public:
		Mitov::String	TrueValue = "true";
		Mitov::String	FalseValue = "false";

	public:
		void InputPin_o_Receive( void *_Data )
		{
			bool AValue = *(bool *)_Data;
//			Serial.println( AValue );
			const char *AText;
			if( AValue )
				AText = TrueValue.c_str();

			else
				AText = FalseValue.c_str();

//			Serial.println( AText );
			OutputPin.Notify( (void*)AText );
		}

	};
//---------------------------------------------------------------------------
	template <typename T, T T_FALSE, T C_TRUE> class DigitalToType
	{
	public:
		OpenWire::SourcePin	OutputPin;

	public:
		T	TrueValue = C_TRUE;
		T	FalseValue = T_FALSE;

	public:
		void InputPin_o_Receive( void *_Data )
		{
			if( *(bool *)_Data )
				OutputPin.Notify( &TrueValue );

			else
				OutputPin.Notify( &FalseValue );

		}

	};
//---------------------------------------------------------------------------
	template <typename T> class DigitalToTypeParams
	{
	public:
		OpenWire::SourcePin	OutputPin;

	public:
		T	TrueValue;
		T	FalseValue;

	public:
		void InputPin_o_Receive( void *_Data )
		{
			if( *(bool *)_Data )
				OutputPin.Notify( &TrueValue );

			else
				OutputPin.Notify( &FalseValue );

		}

	public:
		DigitalToTypeParams( T ATrueValue, T AFalseValue ) :
			TrueValue( ATrueValue ),
			FalseValue( AFalseValue )
		{
		}
	};
//---------------------------------------------------------------------------
	class TextToAnalog
	{
	public:
		OpenWire::SourcePin	OutputPin;

	public:
		void InputPin_o_Receive( void *_Data )
		{
			char * AText = (char*)_Data;
			float AValue = strtod( AText, nullptr );
			OutputPin.Notify( &AValue );
		}

	};
//---------------------------------------------------------------------------
	class TextToInteger
	{
	public:
		uint8_t	Base = 10;

	public:
		OpenWire::SourcePin	OutputPin;

	public:
		void InputPin_o_Receive( void *_Data )
		{
			char * AText = (char*)_Data;
//			long AValue = atoi( AText );
			char *AEnd;
			long AValue = strtol( AText, &AEnd, Base );
			OutputPin.Notify( &AValue );
		}

	};
//---------------------------------------------------------------------------
	class TextToUnsigned
	{
	public:
		OpenWire::SourcePin	OutputPin;

	public:
		uint8_t	Base = 10;

	public:
		void InputPin_o_Receive( void *_Data )
		{
			char * AText = (char*)_Data;
//			unsigned long AValue = atoi( AText );
			char *AEnd;
			unsigned long AValue = strtol( AText, &AEnd, Base );
			OutputPin.Notify( &AValue );
		}

	};
//---------------------------------------------------------------------------
	class UnsignedToInteger
	{
	public:
		OpenWire::SourcePin	OutputPin;

	public:
		bool Constrain = true;

	public:
		void InputPin_o_Receive( void *_Data )
		{
			unsigned long AValue = *(unsigned long*)_Data;
			if( Constrain )
				AValue &= 0x7FFFFFFF;

			OutputPin.Notify( &AValue );
		}

	};
//---------------------------------------------------------------------------
	class IntegerToUnsigned
	{
	public:
		OpenWire::SourcePin	OutputPin;

	public:
		bool Constrain = true;

	public:
		void InputPin_o_Receive( void *_Data )
		{
			long AValue = *(long*)_Data;
			if( Constrain )
				AValue &= 0x7FFFFFFF;

			OutputPin.Notify( &AValue );
		}

	};
//---------------------------------------------------------------------------
	template<int C_NUM_INPUTS> class UnsignedToDigital
	{
	public:
		OpenWire::SourcePin OutputPins[ C_NUM_INPUTS ];

	public:
		uint32_t	InitialValue = 0;

	protected:
		uint32_t	FOldValue = 0;

	protected:
		void SetValue( uint32_t AValue, bool AUpdate )
		{
			for( long i = 0; i < C_NUM_INPUTS; ++i )
			{
				uint32_t ABit = uint32_t(1) << i;
				bool AOldBitValue = ( FOldValue & ABit );
				bool ANewBitValue = ( AValue & ABit );
				if( AUpdate || AOldBitValue != ANewBitValue )
					OutputPins[ i ].SendValue( ANewBitValue );

			}

			FOldValue = AValue;
		}

	public:
		void InputPin_o_Receive( void *_Data )
		{
			unsigned long AValue = *(unsigned long *)_Data;
			if( FOldValue == AValue )
				return;

			SetValue( AValue, false );
		}

	public:
		inline void SystemInit()
		{
			SetValue( InitialValue, true );
		}

	public:
		UnsignedToDigital()
		{
		}
	};
//---------------------------------------------------------------------------
	class CelsiusToFahrenheit
	{
	public:
		OpenWire::SourcePin	OutputPin;

    public:
        bool	Enabled : 1;
		bool	Reverse : 1;

	protected:
        float FilterValue( float AValue )
        {
			if( Reverse )
				return ( AValue - 32.0 ) / ( 9.0/5.0 );

			else
				return AValue * ( 9.0/5.0 ) + 32.0;
        }

	public:
		void InputPin_o_Receive( void *_Data )
        {
            if( ! Enabled )
            {
			    OutputPin.Notify( _Data );
                return;
            }

            float AOutValue = FilterValue( *(float*)_Data );

			OutputPin.Notify( &AOutValue );
        }

	public:
		CelsiusToFahrenheit() :
			Enabled( true ),
			Reverse( false )
		{
		}
	};
//---------------------------------------------------------------------------
	class CelsiusToKelvin
	{
	public:
		OpenWire::SourcePin	OutputPin;

    public:
        bool	Enabled : 1;
		bool	Reverse : 1;

	protected:
        float FilterValue( float AValue )
        {
			if( Reverse )
				return AValue - 273.15;

			else
				return AValue + 273.15;
        }

	public:
		void InputPin_o_Receive( void *_Data )
        {
            if( ! Enabled )
            {
			    OutputPin.Notify( _Data );
                return;
            }

			float AInValue = *(float*)_Data;

            float AOutValue = FilterValue( AInValue );

			OutputPin.Notify( &AOutValue );
        }

	public:
		CelsiusToKelvin() :
			Enabled( true ),
			Reverse( false )
		{
		}
	};
//---------------------------------------------------------------------------
	class MetresToFeet
	{
	public:
		OpenWire::SourcePin	OutputPin;

    public:
        bool	Enabled : 1;
		bool	Reverse : 1;

	protected:
        float FilterValue( float AValue )
        {
			if( Reverse )
				return AValue * 0.3048;

			else
				return AValue / 0.3048;
        }

	public:
		void InputPin_o_Receive( void *_Data )
        {
            if( ! Enabled )
            {
			    OutputPin.Notify( _Data );
                return;
            }

			float AInValue = *(float*)_Data;

            float AOutValue = FilterValue( AInValue );

			OutputPin.Notify( &AOutValue );
        }

	public:
		MetresToFeet() :
			Enabled( true ),
			Reverse( false )
		{
		}
	};
//---------------------------------------------------------------------------
	template<int T_SIZE> class CharToText
	{
	public:
		OpenWire::SourcePin	OutputPin;

    public:
        bool	Enabled : 1;
		bool	Truncate : 1;
		bool	UpdateOnEachChar : 1;
		uint32_t	MaxLength = 100;

	protected:
		byte	FBuffer[ T_SIZE + 1 ];
		uint32_t	FIndex = 0;

	protected:
		void SendBufferNoReset()
		{
			FBuffer[ FIndex ] = '\0';
			OutputPin.Notify( FBuffer );
		}

		void SendBuffer()
		{
			SendBufferNoReset();
			FIndex = 0;
		}

	public:
		void InputPin_o_Receive( void *_Data )
        {
            if( ! Enabled )
            {
			    OutputPin.Notify( _Data );
                return;
            }

			char AValue = *(char *)_Data;
			if( AValue == '\n' )
				return;

			if( AValue == '\r' )
			{
				SendBuffer();
				return;
			}
				
			if( FIndex >= T_SIZE )
			{
				if( Truncate )
					return;

				SendBuffer();
			}

			FBuffer[ FIndex++ ] = AValue;
			if( UpdateOnEachChar )
				SendBufferNoReset();

        }

		inline void ClearInputPin_o_Receive( void *_Data )
		{
			FIndex = 0;
		}

		inline void ClockInputPin_o_Receive( void *_Data )
		{
			SendBuffer();
		}

	public:
		CharToText() :
			Enabled( true ),
			Truncate( false ),
			UpdateOnEachChar( false )
		{
		}
	};
//---------------------------------------------------------------------------
	class TextToChar
	{
	public:
		OpenWire::SourcePin	OutputPin;

	public:
        bool	Enabled : 1;
		bool	AddReturn : 1;
		bool	AddNewLine : 1;

	public:
		void InputPin_o_Receive( void *_Data )
        {
            if( ! Enabled )
            {
			    OutputPin.Notify( _Data );
                return;
            }

			char *AValue = (char *)_Data;
			while( *AValue )
			{
				OutputPin.Notify( AValue );
				++AValue;
			}

			if( AddReturn )
				OutputPin.SendValue( '\r' );

			if( AddNewLine )
				OutputPin.SendValue( '\n' );
        }

	public:
		TextToChar() :
			Enabled( true ),
			AddReturn( true ),
			AddNewLine( true )
		{
		}
	};
//---------------------------------------------------------------------------
	class PressureToAltitude
	{
	public:
		OpenWire::SourcePin	OutputPin;

    public:
        bool Enabled = true;
		float BaseLinePressure = 0;

	protected:
        float FilterValue( float AValue )
        {
			return ( 44330.0 * ( 1 - pow( AValue / BaseLinePressure, 1 / 5.255 )));
        }

	public:
		void InputPin_o_Receive( void *_Data )
        {
            if( ! Enabled )
            {
			    OutputPin.Notify( _Data );
                return;
            }

			float AInValue = *(float*)_Data;

            float AOutValue = FilterValue( AInValue );

			OutputPin.Notify( &AOutValue );
        }

	};
//---------------------------------------------------------------------------
	class AltitudePressureToSeaLevelPressure
	{
	public:
		OpenWire::SourcePin	OutputPin;

    public:
        bool Enabled = true;
		float Altitude = 0;

	protected:
        float FilterValue( float AValue )
        {
			return ( AValue / pow( 1 - ( Altitude / 44330.0 ), 5.255 ));
        }

	public:
		void InputPin_o_Receive( void *_Data )
        {
            if( ! Enabled )
            {
			    OutputPin.Notify( _Data );
                return;
            }

			float AInValue = *(float*)_Data;

            float AOutValue = FilterValue( AInValue );

			OutputPin.Notify( &AOutValue );
        }
	};
//---------------------------------------------------------------------------
	template<int C_NUM_INPUTS> class AnalogToAnalogArray
	{
	public:
		OpenWire::SourcePin	OutputPin;

	public:
		bool	ClockInputPin_o_IsConnected : 1;

	protected:
		bool	FModified : 1;
		float	FBufferData[ C_NUM_INPUTS ];

	protected:
		void CalculateSendOutput( bool AFromStart )
		{
			TArray<float> ABuffer( C_NUM_INPUTS, FBufferData );
			OutputPin.Notify( &ABuffer );
		}

	public:
		void ClockInputPin_o_Receive( void * )
		{
			if( FModified )
				CalculateSendOutput( false );
		}

		void InputPins_o_Receive( int AIndex, void *_Data )
		{
			FBufferData[ AIndex ] = *(float *)_Data;
			FModified = true;
		}

	public:
		inline void SystemStart()
		{
			CalculateSendOutput( true );
		}

		inline void SystemLoopEnd( unsigned long currentMicros )
		{
			if( FModified )
				if( ! ClockInputPin_o_IsConnected )
					CalculateSendOutput( false );

		}

	public:
		AnalogToAnalogArray() :
			ClockInputPin_o_IsConnected( false ),
			FModified( false )
		{
		}

	};
//---------------------------------------------------------------------------
	template<int C_NUM_INPUTS> class AnalogArrayToAnalogArray  // TODO!!!!
	{
	};
//---------------------------------------------------------------------------
	class ReceivePin
	{
	public:
		virtual void Receive( void *_Data ) = 0;
	};
//---------------------------------------------------------------------------
	template<typename T> class ValueSinkPin : public ReceivePin
	{
	public:
		T Value = T( 0 );

	public:
		virtual void Receive( void *_Data ) override
		{
			Value = *(T*)_Data;
		}
	};
//---------------------------------------------------------------------------
	template<typename T> class SingleElementArrayPin : public ValueSinkPin<Mitov::TValueArray<T> >
	{
		typedef ValueSinkPin<Mitov::TValueArray<T> > inherited;

	public:
		virtual void Receive( void *_Data ) override
		{
/*
			Serial.print( "Pin: " ); Serial.println( (int)this );
			Serial.println( inherited::Value.Size );
			Serial.println( (int)inherited::Value.Data );
			delay( 200 );
*/
			T AValue = *(T*)_Data;
			*inherited::Value.Data = AValue;

//			OpenWire::SinkPin::Receive( _Data );
		}

	};
//---------------------------------------------------------------------------
	class FloatArraySinkPinsArray
	{
	protected:
		ValueSinkPin<Mitov::TValueArray<float> >	**FInputPins;

	public:
		ValueSinkPin<Mitov::TValueArray<float> >& operator[] ( int AIndex ) 
		{
          return *( FInputPins[ AIndex ] );
		}

	public:
		inline void SetPin( int AIndex, ValueSinkPin<Mitov::TValueArray<float> > *APin )
		{
			FInputPins[ AIndex ] = APin;
		}

	public:
		FloatArraySinkPinsArray( int ASize )
		{
			FInputPins = new ValueSinkPin<Mitov::TValueArray<float> >*[ ASize ];
		}
	};
//---------------------------------------------------------------------------
	template<int C_NUM_INPUTS> class MixedAnalogToAnalogArray
	{
	public:
		FloatArraySinkPinsArray		InputPins = FloatArraySinkPinsArray( C_NUM_INPUTS );
		OpenWire::SourcePin			OutputPin;

	public:
		bool	ClockInputPin_o_IsConnected : 1;

	protected:
		bool	FModified : 1;

	protected:
		void CalculateSendOutput( bool AFromStart )
		{
//			TArray<float> ABuffer( C_NUM_INPUTS, FBufferData );
//			float *AData = 
//			TArray<float> ABuffer( C_NUM_INPUTS, FBufferData );
			int ASize = 0;

			for( int i = 0; i < C_NUM_INPUTS; ++i )
				ASize += InputPins[ i ].Value.Size;

//			Serial.println( "Size" );
//			Serial.println( ASize );
//			delay( 1000 );

			float *AData = new float[ ASize ];
			float *APtr = AData;
			for( int i = 0; i < C_NUM_INPUTS; ++i )
			{
				int ABlockSize = InputPins[ i ].Value.Size;
				memcpy( APtr, InputPins[ i ].Value.Data, ABlockSize * sizeof( APtr[ 0 ] ));
				APtr += ABlockSize;
			}

			TArray<float> ABuffer( ASize, AData );
			OutputPin.Notify( &ABuffer );

			delete [] AData;
		}

	public:
		void InputPins_o_Receive( int AIndex, void *_Data )
		{
			InputPins[ AIndex ].Receive( _Data );
			FModified = true;
		}

		void ClockInputPin_o_Receive( void * )
		{
			if( FModified )
				CalculateSendOutput( false );
		}

	public:
		inline void SystemStart()
		{
			CalculateSendOutput( true );
		}

		inline void SystemLoopEnd( unsigned long currentMicros )
		{
			if( FModified )
				if( ! ClockInputPin_o_IsConnected )
					CalculateSendOutput( false );

		}

	public:
		MixedAnalogToAnalogArray() :
			ClockInputPin_o_IsConnected( false ),
			FModified( false )
		{
		}

	};
//---------------------------------------------------------------------------
	template<int C_NUM_INPUTS> class AnalogArrayToAnalog
	{
	public:
		OpenWire::TypedSourcePin<float>	OutputPins[ C_NUM_INPUTS ];

	protected:
		bool	FStarted = false;

	public:
		void InputPin_o_Receive( void *_Data )
		{
			int ASize = (( TArray<float> *)_Data )->Size;
			float *AData = (( TArray<float> *)_Data )->Data;

			for( int i = 0; i < C_NUM_INPUTS; ++i )
				OutputPins[ i ].SetValue( *AData++, FStarted );

			FStarted = true;
		}

	};
//---------------------------------------------------------------------------
}

#ifdef __TEST_FOR_DEBUG_PRINTS__
#undef Serial
#endif

#endif
