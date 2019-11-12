////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2018 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_MATH_h
#define _MITOV_MATH_h

#include <Mitov.h>

namespace Mitov
{
#define Min Min
#define Max Max
#define Abs Abs
//---------------------------------------------------------------------------
	#define MITOV_PI 3.14159265359
//---------------------------------------------------------------------------
    template<typename T> class AddValue
	{
	public:
		inline static T CalculateOutput( T AInValue, T AParamValue )
        {
			return AInValue + AParamValue;
        }

	};
//---------------------------------------------------------------------------
	template<typename T> class SubtractValue
	{
	public:
		inline static T CalculateOutput( T AInValue, T AParamValue )
        {
			return AInValue - AParamValue;
        }

	};
//---------------------------------------------------------------------------
	template<typename T> class SubtractFromValue
	{
	public:
		inline static T CalculateOutput( T AInValue, T AParamValue )
        {
			return AParamValue - AInValue;
        }

	};
//---------------------------------------------------------------------------
    template<typename T> class MultiplyByValue
	{
	public:
		inline static T CalculateOutput( T AInValue, T AParamValue )
        {
			return AInValue * AParamValue;
        }

	};
//---------------------------------------------------------------------------
    template<typename T> class DivideByValue
	{
	public:
		OpenWire::SourcePin	OutputPin;

    public:
        T   Value;

    public:
		void SetValue( T AValue )
		{
			if( AValue == 0 )
				Value = 1;

			else
				Value = AValue;
		}

    public:
		void InputPin_o_Receive( void *_Data )
        {
            T AOutValue = ( *(T*)_Data ) / Value;

			OutputPin.Notify( &AOutValue );
        }

    public:
		DivideByValue( T AValue = 1 ) : 
			Value( AValue )
		{
		}

	};
//---------------------------------------------------------------------------
    template<typename T> class DivideByValue_Enable
	{
	public:
		OpenWire::SourcePin	OutputPin;

    public:
        bool Enabled = true;

    public:
        T   Value;

    public:
		void SetValue( T AValue )
		{
			if( AValue == 0 )
				Value = 1;

			else
				Value = AValue;
		}

    public:
		void InputPin_o_Receive( void *_Data )
        {
            if( ! Enabled )
            {
			    OutputPin.Notify( _Data );
                return;
            }

            T AOutValue = ( *(T*)_Data ) / Value;

			OutputPin.Notify( &AOutValue );
        }

    public:
		DivideByValue_Enable( T AValue = 1 ) : 
			Value( AValue )
		{
		}

	};
//---------------------------------------------------------------------------
	template<typename T> class DivideValue
	{
	public:
		inline static T CalculateOutput( T AInValue, T AParamValue )
        {
			if( AInValue == 0 )
				AInValue = 0.0000000001;

            return AParamValue / AInValue;
        }

	};
//---------------------------------------------------------------------------
	template<typename T> class Inverse
	{
	public:
		inline static T CalculateOutput( T AInValue )
        {
			return -AInValue;
        }

	};
//---------------------------------------------------------------------------
	template<typename T> class Abs
	{
	public:
		inline static T CalculateOutput( T AInValue )
        {
			return abs( AInValue );
        }

	};
//---------------------------------------------------------------------------
	template<typename T> class MinLimit
	{
	public:
		inline static T CalculateOutput( T AInValue, T AParamValue )
        {
			return ( AInValue < AParamValue ) ? AParamValue : AInValue;
        }

	};
//---------------------------------------------------------------------------
	template<typename T> class MaxLimit
	{
	public:
		inline static T CalculateOutput( T AInValue, T AParamValue )
        {
			return ( AInValue > AParamValue ) ? AParamValue : AInValue;
        }

	};
//---------------------------------------------------------------------------
	template<typename T> class Limit
	{
	public:
		OpenWire::SourcePin	OutputPin;

    public:
		T	Min = 0.0;
		T	Max = 1.0;

    public:
		void InputPin_o_Receive( void *_Data )
        {
			float AInValue = *(float*)_Data;

			float AOutValue = ( AInValue > Max ) ? 
					Max : 
					( AInValue < Min ) ? Min : AInValue;


			OutputPin.Notify( &AOutValue );
        }

	public:
		Limit( T AMin, T AMax ) :
			Min( AMin ),
			Max( AMax )
		{
		}

	};
//---------------------------------------------------------------------------
	template<typename T> class Limit_Enable
	{
	public:
		OpenWire::SourcePin	OutputPin;

    public:
        bool	Enabled = true;
		T	Min = 0.0;
		T	Max = 1.0;

    public:
		void InputPin_o_Receive( void *_Data )
        {
            if( ! Enabled )
            {
			    OutputPin.Notify( _Data );
                return;
            }

			float AInValue = *(float*)_Data;

			float AOutValue = ( AInValue > Max ) ? 
					Max : 
					( AInValue < Min ) ? Min : AInValue;

			OutputPin.Notify( &AOutValue );
        }

	public:
		Limit_Enable( T AMin, T AMax ) :
			Min( AMin ),
			Max( AMax )
		{
		}

	};
//---------------------------------------------------------------------------
	enum CompareValueType { ctEqual, ctNotEqual, ctBigger, ctSmaller, ctBiggerOrEqual, ctSmallerOrEqual };
//---------------------------------------------------------------------------
	template<typename T> class CompareValue
	{
	public:
		OpenWire::SourcePin	OutputPin;

    public:
        T	Value;

	protected:
		T	FCurrentValue;

    public:
		CompareValueType	CompareType : 3;
        bool				Enabled : 1;
		bool				OnlyChanged : 1;
		bool				IgnoreCase : 1; // Added in root to save space!

	protected:
		bool	FOutputValue : 1;
		bool	FStarted : 1;
		bool	FProcessedOnce : 1;

    public:
		void SetValue( T AValue )
		{
			if( OnlyChanged )
				if( Value == AValue )
					return;

			Value = AValue;
			ProcessOutput();
		}

	protected:
		bool PerformCompare( T ACurrentValue, T AValue )
		{
			switch( CompareType )
			{
			default:
			case ctEqual:
				return ( ACurrentValue == AValue );

			case ctNotEqual:
				return ( ACurrentValue != AValue );

			case ctBigger:
				return ( ACurrentValue > AValue );

			case ctSmaller:
				return ( ACurrentValue < AValue );

			case ctBiggerOrEqual:
				return ( ACurrentValue >= AValue );

			case ctSmallerOrEqual:
				return ( ACurrentValue <= AValue );

			}
		}

	protected:
		void ProcessOutput()
		{
			if( ! Enabled )
				return;

//			Serial.print( FCurrentValue ); Serial.print( " ? " ); Serial.println( Value );

			bool ABoolValue = PerformCompare( FCurrentValue, Value );

			if( ! OnlyChanged )
				if( FOutputValue )
				{
					FOutputValue = false;
					OutputPin.SendValue( false );
				}

			if( ( !FStarted ) || ( FOutputValue != ABoolValue ) )
			{
				OutputPin.Notify( &ABoolValue );
				FOutputValue = ABoolValue;
				FStarted = true;
			}
		}

	public:
		void InputPin_o_Receive( void *_Data )
		{
			T AValue = *(T*)_Data;

			if( OnlyChanged )
				if( FProcessedOnce )
					if( AValue == FCurrentValue )
						return;

			FCurrentValue = AValue;
			FProcessedOnce = true;

			ProcessOutput();
		}

	public:
		inline void SystemStart()
		{
			if( ! FStarted )
				OutputPin.SendValue<bool>( false );

		}

	public:
		CompareValue( T AInitialValue ) : 
			Value( AInitialValue ),
			FCurrentValue( AInitialValue ),
			CompareType( ctEqual ),
			Enabled( true ),
			OnlyChanged( true ),
			IgnoreCase( false ),
			FOutputValue( false ),
			FStarted( false ),
			FProcessedOnce( false )
		{
		}

	};
//---------------------------------------------------------------------------
	template<> bool CompareValue<char>::PerformCompare( char ACurrentValue, char AValue )
	{
		if( IgnoreCase )
		{
			ACurrentValue = toupper( ACurrentValue );
			AValue = toupper( AValue );
		}

		switch( CompareType )
		{
			default:
			case ctEqual:
				return ( ACurrentValue == AValue );

			case ctNotEqual:
				return ( ACurrentValue != AValue );

			case ctBigger:
				return ( ACurrentValue > AValue );

			case ctSmaller:
				return ( ACurrentValue < AValue );

			case ctBiggerOrEqual:
				return ( ACurrentValue >= AValue );

			case ctSmallerOrEqual:
				return ( ACurrentValue <= AValue );

		}
	}
//---------------------------------------------------------------------------
	template<> bool CompareValue<Mitov::String>::PerformCompare( Mitov::String ACurrentValue, Mitov::String AValue )
	{
		if( IgnoreCase )
		{
			ACurrentValue.toUpperCase();
			AValue.toUpperCase();
		}

		switch( CompareType )
		{
			default:
			case ctEqual:
				return ( ACurrentValue == AValue );

			case ctNotEqual:
				return ( ACurrentValue != AValue );

			case ctBigger:
				return ( ACurrentValue > AValue );

			case ctSmaller:
				return ( ACurrentValue < AValue );

			case ctBiggerOrEqual:
				return ( ACurrentValue >= AValue );

			case ctSmallerOrEqual:
				return ( ACurrentValue <= AValue );

		}
	}
//---------------------------------------------------------------------------
	template<> void CompareValue<Mitov::String>::InputPin_o_Receive( void *_Data )
	{
		Mitov::String AValue = Mitov::String( (char*)_Data );

		if( OnlyChanged )
			if( FProcessedOnce )
				if( AValue == FCurrentValue )
					return;

		FCurrentValue = AValue;
		FProcessedOnce = true;

		ProcessOutput();
	}
//---------------------------------------------------------------------------
	class CompareDateTimeValue
	{
	public:
		OpenWire::SourcePin	OutputPin;

    public:
        Mitov::TDateTime	Value;

	protected:
		Mitov::TDateTime	FCurrentValue;

    public:
		TDateTimeCompareItems	Items;

    public:
		CompareValueType	CompareType : 3;
        bool				Enabled : 1;
		bool				OnlyChanged : 1;

	protected:
		bool	FOutputValue : 1;
		bool	FStarted : 1;
		bool	FProcessedOnce : 1;

    public:
		void SetValue( Mitov::TDateTime AValue )
		{
			if( OnlyChanged )
				if( Value == AValue )
					return;

			Value = AValue;
			ProcessOutput();
		}

	protected:
		bool PerformCompare( Mitov::TDateTime ACurrentValue, Mitov::TDateTime AValue )
		{
			switch( CompareType )
			{
			default:
			case ctEqual:
				return ( ACurrentValue.IsEqual( AValue, Items ) );

			case ctNotEqual:
				return ( ! ACurrentValue.IsEqual( AValue, Items ) );

			case ctBigger:
				return ( ACurrentValue.IsBigger( AValue, Items ) );

			case ctSmaller:
				return ( AValue.IsBigger( ACurrentValue, Items ) );

			case ctBiggerOrEqual:
				return ( ACurrentValue.IsBiggerOrEqual( AValue, Items ) );

			case ctSmallerOrEqual:
				return ( AValue.IsBiggerOrEqual( ACurrentValue, Items ) );

			}
		}

	protected:
		void ProcessOutput()
		{
			if( ! Enabled )
				return;

//			Serial.print( FCurrentValue ); Serial.print( " ? " ); Serial.println( Value );

			bool ABoolValue = PerformCompare( FCurrentValue, Value );

			if( ! OnlyChanged )
				if( FOutputValue )
				{
					FOutputValue = false;
					OutputPin.SendValue( false );
				}

			if( ( !FStarted ) || ( FOutputValue != ABoolValue ) )
			{
				OutputPin.Notify( &ABoolValue );
				FOutputValue = ABoolValue;
				FStarted = true;
			}
		}

	public:
		void InputPin_o_Receive( void *_Data )
		{
			Mitov::TDateTime AValue = *(Mitov::TDateTime*)_Data;

			if( OnlyChanged )
				if( FProcessedOnce )
					if( AValue == FCurrentValue )
						return;

			FCurrentValue = AValue;
			FProcessedOnce = true;

			ProcessOutput();
		}

	public:
		inline void SystemStart()
		{
			if( ! FStarted )
				OutputPin.SendValue<bool>( false );

		}

	public:
		CompareDateTimeValue() : 
			CompareType( ctEqual ),
			Enabled( true ),
			OnlyChanged( true ),
			FOutputValue( false ),
			FStarted( false ),
			FProcessedOnce( false )
		{
		}

	};
//---------------------------------------------------------------------------
	template<typename T> class CompareRange
	{
	public:
		OpenWire::SourcePin	OutputPin;

    public:
		class TValueRange
		{
		public:
			T	Min;
			T	Max;

		public:
			TValueRange( T AMin, T AMax ) :
				Min( AMin ),
				Max( AMax )
			{
			}
		};

    public:
        TValueRange	Range;

	protected:
		T	FCurrentValue;

    public:
        bool	Enabled : 1;
		bool	IsOutsideRange : 1;
		bool	IncludeLimits : 1;
		bool	OnlyChanged : 1;
		bool	IgnoreCase : 1; // Added in root to save space!

	protected:
		bool	FOutputValue : 1;
		bool	FStarted : 1;
		bool	FProcessedOnce : 1;

    public:
		void ProcessOutput()
		{
			if( ! Enabled )
				return;

			bool ABoolValue = PerformCompare( FCurrentValue );

			if( ! OnlyChanged )
				if( FOutputValue )
				{
					FOutputValue = false;
					OutputPin.SendValue( false );
				}

			if( ( !FStarted ) || ( FOutputValue != ABoolValue ) )
			{
				OutputPin.Notify( &ABoolValue );
				FOutputValue = ABoolValue;
				FStarted = true;
			}
		}

	protected:
		bool PerformCompare( T ACurrentValue )
		{
			if( IsOutsideRange )
			{
				if( IncludeLimits )
				{
					if( ACurrentValue >= Range.Max )
						return true;

					return ( ACurrentValue <= Range.Min );
				}

				if( ACurrentValue > Range.Max )
					return true;

				return ( ACurrentValue < Range.Min );
			}

			if( IncludeLimits )
			{
				if( ACurrentValue >= Range.Max )
					return false;

				return ! ( ACurrentValue <= Range.Min );
			}

			if( ACurrentValue > Range.Max )
				return false;

			return ! ( ACurrentValue < Range.Min );
		}

	public:
		void InputPin_o_Receive( void *_Data )
		{
			T AValue = *(T*)_Data;

			if( OnlyChanged )
				if( FProcessedOnce )
					if( AValue == FCurrentValue )
						return;

			FCurrentValue = AValue;
			FProcessedOnce = true;

			ProcessOutput();
		}

	public:
		inline void SystemStart()
		{
			if( ! FStarted )
				OutputPin.SendValue<bool>( false );

		}

	public:
		CompareRange( T AMin, T AMax ) : 
			Range( AMin, AMax ),
			FCurrentValue( 0 ),
			Enabled( true ),
			IsOutsideRange( false ),
			IncludeLimits( false ),
			OnlyChanged( true ),
			IgnoreCase( false ),
			FOutputValue( false ),
			FStarted( false ),
			FProcessedOnce( false )
		{
		}

	};
//---------------------------------------------------------------------------
	template<> bool CompareRange<char>::PerformCompare( char ACurrentValue )
	{
		char AMinValue = Range.Min;
		char AMaxValue = Range.Max;
		if( IgnoreCase )
		{
			ACurrentValue = toupper( ACurrentValue );
			AMinValue = toupper( AMinValue );
			AMaxValue = toupper( AMaxValue );
		}

		if( IsOutsideRange )
		{
			if( IncludeLimits )
			{
				if( ACurrentValue >= AMaxValue )
					return true;

				return ( ACurrentValue <= AMinValue );
			}

			if( ACurrentValue > AMaxValue )
				return true;

			return ( ACurrentValue < AMinValue );
		}

		if( IncludeLimits )
		{
			if( ACurrentValue >= AMaxValue )
				return false;

			return ! ( ACurrentValue <= AMinValue );
		}

		if( ACurrentValue > AMaxValue )
			return false;

		return ! ( ACurrentValue < AMinValue );
	}
//---------------------------------------------------------------------------
	class CompareDateTimeRange
	{
	public:
		OpenWire::SourcePin	OutputPin;

    public:
		class TValueRange
		{
		public:
			Mitov::TDateTime	Min;
			Mitov::TDateTime	Max = 100.0;

		};

    public:
        TValueRange	Range;

    public:
		TDateTimeCompareItems	Items;

	protected:
		Mitov::TDateTime	FCurrentValue;

    public:
        bool	Enabled : 1;
		bool	IsOutsideRange : 1;
		bool	IncludeLimits : 1;
		bool	OnlyChanged : 1;
		bool	IgnoreCase : 1; // Added in root to save space!

	protected:
		bool	FOutputValue : 1;
		bool	FStarted : 1;
		bool	FProcessedOnce : 1;

    public:
		void ProcessOutput()
		{
			if( ! Enabled )
				return;

			bool ABoolValue = PerformCompare( FCurrentValue );

			if( ! OnlyChanged )
				if( FOutputValue )
				{
					FOutputValue = false;
					OutputPin.SendValue( false );
				}

			if( ( !FStarted ) || ( FOutputValue != ABoolValue ) )
			{
				OutputPin.Notify( &ABoolValue );
				FOutputValue = ABoolValue;
				FStarted = true;
			}
		}

	protected:
		bool PerformCompare( Mitov::TDateTime ACurrentValue )
		{
			if( IsOutsideRange )
			{
				if( IncludeLimits )
				{
					if( ACurrentValue.IsBiggerOrEqual( Range.Max, Items ) )
						return true;

					return (  Range.Min.IsBigger( ACurrentValue, Items ) );
				}

				if( ACurrentValue.IsBigger( Range.Max, Items ) )
					return true;

				return ( Range.Min.IsBiggerOrEqual( ACurrentValue, Items )  );
			}

			if( IncludeLimits )
			{
				if( ACurrentValue.IsBiggerOrEqual( Range.Max, Items ) )
					return false;

				return ! ( Range.Min.IsBigger( ACurrentValue, Items ) );
			}

			if( ACurrentValue.IsBigger( Range.Max, Items ) )
				return false;

			return ! ( Range.Min.IsBiggerOrEqual( ACurrentValue, Items ) );
		}

	public:
		void InputPin_o_Receive( void *_Data )
		{
			Mitov::TDateTime AValue = *(Mitov::TDateTime*)_Data;

			if( OnlyChanged )
				if( FProcessedOnce )
					if( AValue == FCurrentValue )
						return;

			FCurrentValue = AValue;
			FProcessedOnce = true;

			ProcessOutput();
		}

	public:
		inline void SystemStart()
		{
			if( ! FStarted )
				OutputPin.SendValue<bool>( false );

		}

	public:
		CompareDateTimeRange() : 
			Enabled( true ),
			IsOutsideRange( false ),
			IncludeLimits( false ),
			OnlyChanged( true ),
			IgnoreCase( false ),
			FOutputValue( false ),
			FStarted( false ),
			FProcessedOnce( false )
		{
		}

	};
//---------------------------------------------------------------------------
	class AveragePeriod
	{
	public:
		OpenWire::SourcePin	OutputPin;

	public:
		unsigned long	Period = 1000;

	protected:
		float FSum = 0.0f;
		unsigned long	FCount = 0;
		unsigned long	FLastTime = 0;

	public:
		void InputPin_o_Receive( void *_Data )
		{
			FSum += *(float *)_Data;
			++FCount;
		}

	public:
		inline void SystemLoopBegin( unsigned long currentMicros )
		{
			if( currentMicros - FLastTime < Period )
				return;

			if( ! FCount )
				return;

			float AValue = FSum / FCount;

			FSum = 0.0;
			FCount = 0;
			FLastTime = currentMicros;

			OutputPin.Notify( &AValue );
		}

	};
//---------------------------------------------------------------------------
	template<typename T_IMPLEMENTATION_CLASS, typename T, int C_NUM_INPUTS> class MultiInputMathOperation
	{
	public:
		OpenWire::SourcePin	OutputPin;

    public:
        bool Enabled : 1;

	protected:
		bool	FModified : 1;
		bool	FPopulated : 1;
		T		FLastOutput;

		T		FValues[ C_NUM_INPUTS ] = { 0 };

	public:
		void InputPins_o_Receive( int AIndex, void *_Data )
		{
			FValues[ AIndex ] = *(T *)_Data;
			FModified = true;
		}

	public:
		inline void SystemStart()
		{
			CalculateSendOutput();
		}

		inline void SystemLoopEnd( unsigned long currentMicros )
		{
			CalculateSendOutput();
		}

	protected:
		void CalculateSendOutput()
		{
//			Serial.println( "CalculateSendOutput+++" );
			T AValue = T_IMPLEMENTATION_CLASS::CalculateOutput( FValues, C_NUM_INPUTS );
//			Serial.println( AValue );
			if( FModified )
				if( FLastOutput == AValue )
					return;

			FLastOutput = AValue;
			FPopulated = true;
			FModified = false;

			OutputPin.SendValue( AValue ); // Use Send Value for String Compatibility!

/*
			T_OUT AValue = CalculateOutput();
//			Serial.println( AValue );
			if( ! AFromStart )
				if( FLastOutput == AValue )
					return;

			OutputPin.SendValue( AValue ); // Use Send Value for String Compatibility!
			FLastOutput = AValue;

			FModified = false;
*/
		}

	public:
		void ClockInputPin_o_Receive( void *_Data )
		{
			T AValue = T_IMPLEMENTATION_CLASS::CalculateOutput( FValues, C_NUM_INPUTS );
//			if( FLastOutput == AValue )
//				return;

			OutputPin.SendValue( AValue ); // Use Send Value for String Compatibility!
			FLastOutput = AValue;

			FModified = false;
		}

	public:
		MultiInputMathOperation() :
			Enabled( true ),
			FModified( false ),
			FPopulated( false )
		{
		}

	};
//---------------------------------------------------------------------------
	template<typename T> class Add
	{
	public:
		static T CalculateOutput( T *AInputValues, uint16_t ANumInputs )
		{
			T AValue = 0;
			for( int i = 0; i < ANumInputs; ++i )
				AValue += AInputValues[ i ];

			return AValue;
		}

	};
//---------------------------------------------------------------------------
	template<typename T> class Multiply
	{
	public:
		static T CalculateOutput( T *AInputValues, uint16_t ANumInputs )
		{
			T AValue = 1;
			for( int i = 0; i < ANumInputs; ++i )
				AValue *= AInputValues[ i ];

			return AValue;
		}
	};
//---------------------------------------------------------------------------
	class ValueRange
	{
	public:
		float Min = 0.0f;
		float Max = 1.0f;

	};
//---------------------------------------------------------------------------
	class MapRange
	{
	public:
		OpenWire::SourcePin	OutputPin;

    public:
		ValueRange	InputRange;
		ValueRange	OutputRange;

	public:
		void InputPin_o_Receive( void *_Data )
		{
			float AValue = *(float*)_Data;

			float AOutValue = Func::MapRange( AValue, InputRange.Min, InputRange.Max, OutputRange.Min, OutputRange.Max );

			OutputPin.Notify( &AOutValue );
		}
	};
//---------------------------------------------------------------------------
	class MapRange_Enable
	{
	public:
		OpenWire::SourcePin	OutputPin;

    public:
        bool Enabled = true;

		ValueRange	InputRange;
		ValueRange	OutputRange;

	public:
		void InputPin_o_Receive( void *_Data )
		{
            if( ! Enabled )
            {
			    OutputPin.Notify( _Data );
                return;
            }

			float AValue = *(float*)_Data;

			float AOutValue = Func::MapRange( AValue, InputRange.Min, InputRange.Max, OutputRange.Min, OutputRange.Max );

			OutputPin.Notify( &AOutValue );
		}
	};
//---------------------------------------------------------------------------
	class AnalogDeadZone
	{
	public:
		OpenWire::SourcePin	OutputPin;

    public:
		float Value = 0.5f;
		float Span = 0.1f;

	public:
		void InputPin_o_Receive( void *_Data )
		{
			float AValue = *(float*)_Data;

			float AFalfSpan = Span / 2;

			if( AValue > Value + AFalfSpan )
				AValue -= AFalfSpan;

			else if( AValue < Value - AFalfSpan )
				AValue += AFalfSpan;

			else
				AValue = Value;

			OutputPin.Notify( &AValue );
		}

	public:
		inline void SystemStart()
		{
			OutputPin.Notify( &Value );
		}

	};
//---------------------------------------------------------------------------
	class AnalogDeadZone_Enable
	{
	public:
		OpenWire::SourcePin	OutputPin;

    public:
        bool Enabled = true;

		float Value = 0.5f;
		float Span = 0.1f;

	public:
		void InputPin_o_Receive( void *_Data )
		{
            if( ! Enabled )
            {
			    OutputPin.Notify( _Data );
                return;
            }

			float AValue = *(float*)_Data;

			float AFalfSpan = Span / 2;

			if( AValue > Value + AFalfSpan )
				AValue -= AFalfSpan;

			else if( AValue < Value - AFalfSpan )
				AValue += AFalfSpan;

			else
				AValue = Value;

			OutputPin.Notify( &AValue );
		}

	public:
		inline void SystemStart()
		{
			OutputPin.Notify( &Value );
		}

	};
//---------------------------------------------------------------------------
	class AnalogDeadZoneScaled
	{
	public:
		OpenWire::SourcePin	OutputPin;

    public:
		ValueRange	InputRange;
		ValueRange	OutputRange;
		float Value = 0.5f;
		float Span = 0.1f;

	public:
		void InputPin_o_Receive( void *_Data )
		{
			float AValue = *(float*)_Data;

			float AFalfSpan = Span / 2;

			if( AValue > Value + AFalfSpan )
				AValue -= AFalfSpan;

			else if( AValue < Value - AFalfSpan )
				AValue += AFalfSpan;

			else
				AValue = Value;

			float AOutValue = Func::MapRange( AValue, InputRange.Min + AFalfSpan, InputRange.Max - AFalfSpan, OutputRange.Min, OutputRange.Max );

			OutputPin.Notify( &AOutValue );
		}

	public:
		inline void SystemStart()
		{
			float AFalfSpan = Span / 2;

			float AOutValue = Func::MapRange( Value, InputRange.Min + AFalfSpan, InputRange.Max - AFalfSpan, OutputRange.Min, OutputRange.Max );
			OutputPin.Notify( &AOutValue );
		}

	};
//---------------------------------------------------------------------------
	class AnalogDeadZoneScaled_Enable
	{
	public:
		OpenWire::SourcePin	OutputPin;

    public:
        bool Enabled = true;

		ValueRange	InputRange;
		ValueRange	OutputRange;
		float Value = 0.5f;
		float Span = 0.1f;

	public:
		void InputPin_o_Receive( void *_Data )
		{
            if( ! Enabled )
            {
			    OutputPin.Notify( _Data );
                return;
            }

			float AValue = *(float*)_Data;

			float AFalfSpan = Span / 2;

			if( AValue > Value + AFalfSpan )
				AValue -= AFalfSpan;

			else if( AValue < Value - AFalfSpan )
				AValue += AFalfSpan;

			else
				AValue = Value;

			float AOutValue = Func::MapRange( AValue, InputRange.Min + AFalfSpan, InputRange.Max - AFalfSpan, OutputRange.Min, OutputRange.Max );

			OutputPin.Notify( &AOutValue );
		}

	public:
		inline void SystemStart()
		{
			float AFalfSpan = Span / 2;

			float AOutValue = Func::MapRange( Value, InputRange.Min + AFalfSpan, InputRange.Max - AFalfSpan, OutputRange.Min, OutputRange.Max );
			OutputPin.Notify( &AOutValue );
		}

	};
//---------------------------------------------------------------------------
    template<typename T> class RaiseToPower
	{
	public:
		inline static T CalculateOutput( T AInValue, T AParamValue )
        {
			return pow( AInValue, AParamValue );
        }

	};
//---------------------------------------------------------------------------
	class Sine
	{
	public:
		inline static float CalculateOutput( float AInValue )
        {
			return sin( AInValue );
        }

	};
//---------------------------------------------------------------------------
	class Cosine
	{
	public:
		inline static float CalculateOutput( float AInValue )
        {
			return cos( AInValue );
        }

	};
//---------------------------------------------------------------------------
	class RadToDegrees
	{
	public:
		inline static float CalculateOutput( float AInValue )
        {
			return AInValue * 180 / MITOV_PI;
        }

	};
//---------------------------------------------------------------------------
	class DegreesToRad
	{
	public:
		inline static float CalculateOutput( float AInValue )
        {
			return AInValue * MITOV_PI / 180;
        }

	};
//---------------------------------------------------------------------------
    template<typename T> class AndValue
	{
	public:
		inline static T CalculateOutput( T AInValue, T AParamValue )
        {
			return AInValue & AParamValue;
        }

	};
//---------------------------------------------------------------------------
    template<typename T> class OrValue
	{
	public:
		inline static T CalculateOutput( T AInValue, T AParamValue )
        {
			return AInValue | AParamValue;
        }

	};
//---------------------------------------------------------------------------
    template<typename T> class XorValue
	{
	public:
		inline static T CalculateOutput( T AInValue, T AParamValue )
        {
			return AInValue ^ AParamValue;
        }

	};
//---------------------------------------------------------------------------
    template<typename T> class NotValue
	{
	public:
		inline static T CalculateOutput( T AInValue )
        {
			return ~ AInValue;
        }

	};
//---------------------------------------------------------------------------
	template<typename T> class Accumulate
	{
	public:
		OpenWire::SourcePin	OutputPin;

    public:
        bool Enabled : 1;

	protected:
		bool			FHasTime : 1;
		unsigned long	FLastTime = 0;

	public:
		T	InitialValue = 0;

	public:
		void SetInitialValue( float AValue )
		{
			if( InitialValue == AValue )
				return;

			InitialValue = AValue;
			OutputPin.Notify( &InitialValue );
		}

	protected:
        T FilterValue( T AValue )
        {
			unsigned long ATime = micros();
			if( ! FHasTime )			
			{
				FHasTime = true;
				return InitialValue;
			}

			float dT = ( ATime - FLastTime ) / 1000000.0f;
			InitialValue += AValue * dT;
            return InitialValue;
        }

	public:
		void InputPin_o_Receive( void *_Data )
        {
            if( ! Enabled )
            {
			    OutputPin.Notify( _Data );
                return;
            }

            T AOutValue = FilterValue( *(T*)_Data );

			OutputPin.Notify( &AOutValue );
        }

	public:
		Accumulate() :
			Enabled( true ),
			FHasTime( false )
		{
		}

	};
//---------------------------------------------------------------------------
	class AccumulateAngle
	{
	public:
		TAngleUnits	Units = auDegree;

	public:
		OpenWire::SourcePin	OutputPin;

    public:
        bool Enabled : 1;

	protected:
		bool			FHasTime : 1;
		unsigned long	FLastTime = 0;

	public:
		float	InitialValue = 0.0;

	public:
		void SetInitialValue( float AValue )
		{
			if( InitialValue == AValue )
				return;

			InitialValue = AValue;
			OutputPin.Notify( &InitialValue );
		}

	protected:
        float FilterValue( float AValue )
        {
			if( ! FHasTime )			
			{
				FHasTime = true;
				return InitialValue;
			}

			unsigned long ATime = micros();
			float dT = ( ATime - FLastTime ) / 1000000.0f;
			InitialValue += AValue * dT;
			switch( Units )
			{
				case auDegree:
					InitialValue = posmod( InitialValue, 360.0f );
					break;

				case auRadians:
					InitialValue = posmod( InitialValue, 2 * PI );
					break;

				case auNormalized:
					InitialValue = posmod( InitialValue, 1.0f );
					break;

			}

            return InitialValue;
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
		AccumulateAngle() :
			Enabled( true ),
			FHasTime( false )
		{
		}

	};
//---------------------------------------------------------------------------
#undef Abs
#undef Min
#undef Max
}

#endif
