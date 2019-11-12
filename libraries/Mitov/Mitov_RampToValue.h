////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2018 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_RAMP_TO_VALUE_h
#define _MITOV_RAMP_TO_VALUE_h

#include <Mitov.h>

#ifdef __TEST_FOR_DEBUG_PRINTS__
#define Serial UNGUARDED DEBUG PRINT!!!
#endif

namespace Mitov
{
	class RampToValue
	{
	public:
		OpenWire::SourcePin	OutputPin;

    public:
        bool	Enabled = true;
		float	InitialValue = 0.0f;
		float	Slope = 1.0f;

	public:
		void SetInitialValue( float AValue )
		{
			InitialValue = AValue;
			FCurrentValue = InitialValue;
		}

	protected:
		unsigned long	FLastTime = 0;
		float			FCurrentValue = 0.0f;
		float			FTargetValue = 0.0f;

	public:
		void InputPin_o_Receive( void *_Data )
		{
			float AValue = *(float *)_Data;
			FTargetValue = AValue;
		}

		inline void ClockInputPin_o_Receive( void *_Data )
		{
			Generate( micros(), true );
		}

	protected:
		void Generate( unsigned long currentMicros, bool FromClock )
		{
			if( FCurrentValue != FTargetValue )
			{
				if( ! Enabled )
					FCurrentValue = FTargetValue;

				else
				{
					float ARamp = abs( ( currentMicros - FLastTime ) * Slope / 1000000 );
					if( FCurrentValue < FTargetValue )
					{
						FCurrentValue += ARamp;
						if( FCurrentValue > FTargetValue )
							FCurrentValue = FTargetValue;

					}
					else
					{
						FCurrentValue -= ARamp;
						if( FCurrentValue < FTargetValue )
							FCurrentValue = FTargetValue;

					}
				}

				OutputPin.Notify( &FCurrentValue );
			}

			else if( FromClock )
				OutputPin.Notify( &FCurrentValue );

			FLastTime = currentMicros;
//			inherited::SendOutput();
		}

	public:
		inline void SystemStart()
		{
			FCurrentValue = InitialValue;
			FTargetValue = InitialValue;

			OutputPin.Notify( &FCurrentValue );
		}

		inline void SystemLoopBegin( unsigned long currentMicros )
		{
			Generate( currentMicros, false );
		}

	};
//---------------------------------------------------------------------------
	class RampToColorValue
	{
	public:
		OpenWire::SourcePin	OutputPin;

    public:
        bool	Enabled = true;
		TColor	InitialValue;
		float	Slope = 1.0f;

	public:
		void SetInitialValue( float AValue )
		{
			InitialValue = AValue;
			FCurrentValue = InitialValue;
		}

	protected:
		unsigned long	FLastTimeRed = 0;
		unsigned long	FLastTimeGreen = 0;
		unsigned long	FLastTimeBlue = 0;
		TColor			FSlopeValues;
		TColor			FCurrentValue;
		TColor			FTargetValue;

	public:
		static void ProcessColor( uint8_t &ACurrentColor, uint8_t &ATargetColor, int ARamp )
		{
			int ANewColor = ACurrentColor;
			if( ACurrentColor < ATargetColor )
			{
				ANewColor += ARamp;
				if( ANewColor > ATargetColor )
					ANewColor = ATargetColor;

			}
			else
			{
				ANewColor -= ARamp;
				if( ANewColor < ATargetColor )
					ANewColor = ATargetColor;

			}

			ACurrentColor = ANewColor;
		}

	public:
		void InputPin_o_Receive( void *_Data )
		{
			TColor AValue = *(TColor *)_Data;
			FTargetValue = AValue;
			FSlopeValues.Red = abs( int( FTargetValue.Red ) - int( FCurrentValue.Red ) );
			FSlopeValues.Green = abs( int( FTargetValue.Green ) - int( FCurrentValue.Green ) );
			FSlopeValues.Blue = abs( int( FTargetValue.Blue ) - int( FCurrentValue.Blue ) );

			unsigned long currentMicros = micros();
			FLastTimeRed = currentMicros;
			FLastTimeGreen = currentMicros;
			FLastTimeBlue = currentMicros;
		}

		inline void ClockInputPin_o_Receive( void *_Data )
		{
			Generate( micros(), true );
		}

	protected:
		void Generate( unsigned long currentMicros, bool FromClock )
		{
			if( FCurrentValue != FTargetValue )
			{
				if( ! Enabled )
					FCurrentValue = FTargetValue;

				else
				{
					float ACoef =  Slope / 1000000;
					int ARamp = abs( ACoef * float( currentMicros - FLastTimeRed ) * FSlopeValues.Red ) + 0.5;
					if( ARamp )
					{
						ProcessColor( FCurrentValue.Red, FTargetValue.Red, ARamp );
						FLastTimeRed = currentMicros;
					}

					ARamp = abs( ACoef * float( currentMicros - FLastTimeGreen ) * FSlopeValues.Green ) + 0.5;
					if( ARamp )
					{
						ProcessColor( FCurrentValue.Green, FTargetValue.Green, ARamp );
						FLastTimeGreen = currentMicros;
					}

					ARamp = abs( ACoef * float( currentMicros - FLastTimeBlue ) * FSlopeValues.Blue ) + 0.5;
					if( ARamp )
					{
						ProcessColor( FCurrentValue.Blue, FTargetValue.Blue, ARamp );
						FLastTimeBlue = currentMicros;
					}
				}

				OutputPin.Notify( &FCurrentValue );
			}

			else if( FromClock )
			{
				OutputPin.Notify( &FCurrentValue );
				FLastTimeRed = currentMicros;
				FLastTimeGreen = currentMicros;
				FLastTimeBlue = currentMicros;
			}
//			inherited::SendOutput();
		}

	public:
		inline void SystemStart()
		{
			FCurrentValue = InitialValue;
			FTargetValue = InitialValue;

			OutputPin.Notify( &FCurrentValue );
		}

		inline void SystemLoopBegin( unsigned long currentMicros )
		{
			Generate( currentMicros, false );
		}

	};
//---------------------------------------------------------------------------
	class RampToRGBWColorValue
	{
	public:
		OpenWire::SourcePin	OutputPin;

    public:
        bool		Enabled = true;
		TRGBWColor	InitialValue;
		float		Slope = 1.0f;

	public:
		void SetInitialValue( float AValue )
		{
			InitialValue = AValue;
			FCurrentValue = InitialValue;
		}

	protected:
		unsigned long	FLastTimeRed = 0;
		unsigned long	FLastTimeGreen = 0;
		unsigned long	FLastTimeBlue = 0;
		unsigned long	FLastTimeWhite = 0;
		TRGBWColor		FSlopeValues;
		TRGBWColor		FCurrentValue;
		TRGBWColor		FTargetValue;

	public:
		void InputPin_o_Receive( void *_Data )
		{
			TRGBWColor AValue = *(TRGBWColor *)_Data;
			FTargetValue = AValue;
			FSlopeValues.Red = abs( int( FTargetValue.Red ) - int( FCurrentValue.Red ) );
			FSlopeValues.Green = abs( int( FTargetValue.Green ) - int( FCurrentValue.Green ) );
			FSlopeValues.Blue = abs( int( FTargetValue.Blue ) - int( FCurrentValue.Blue ) );
			FSlopeValues.White = abs( int( FTargetValue.White ) - int( FCurrentValue.White ) );

			unsigned long currentMicros = micros();
			FLastTimeRed = currentMicros;
			FLastTimeGreen = currentMicros;
			FLastTimeBlue = currentMicros;
			FLastTimeWhite = currentMicros;
		}

		inline void ClockInputPin_o_Receive( void *_Data )
		{
			Generate( micros(), true );
		}

	protected:
		void Generate( unsigned long currentMicros, bool FromClock )
		{
			if( FCurrentValue != FTargetValue )
			{
				if( ! Enabled )
					FCurrentValue = FTargetValue;

				else
				{
					float ACoef =  Slope / 1000000;
					int ARamp = abs( ACoef * float( currentMicros - FLastTimeRed ) * FSlopeValues.Red ) + 0.5;
					if( ARamp )
					{
						RampToColorValue::ProcessColor( FCurrentValue.Red, FTargetValue.Red, ARamp );
						FLastTimeRed = currentMicros;
					}

					ARamp = abs( ACoef * float( currentMicros - FLastTimeGreen ) * FSlopeValues.Green ) + 0.5;
					if( ARamp )
					{
						RampToColorValue::ProcessColor( FCurrentValue.Green, FTargetValue.Green, ARamp );
						FLastTimeGreen = currentMicros;
					}

					ARamp = abs( ACoef * float( currentMicros - FLastTimeBlue ) * FSlopeValues.Blue ) + 0.5;
					if( ARamp )
					{
						RampToColorValue::ProcessColor( FCurrentValue.Blue, FTargetValue.Blue, ARamp );
						FLastTimeBlue = currentMicros;
					}

					ARamp = abs( ACoef * float( currentMicros - FLastTimeWhite ) * FSlopeValues.White ) + 0.5;
					if( ARamp )
					{
						RampToColorValue::ProcessColor( FCurrentValue.White, FTargetValue.White, ARamp );
						FLastTimeWhite = currentMicros;
					}
				}

				OutputPin.Notify( &FCurrentValue );
			}

			else if( FromClock )
			{
				OutputPin.Notify( &FCurrentValue );
				FLastTimeRed = currentMicros;
				FLastTimeGreen = currentMicros;
				FLastTimeBlue = currentMicros;
				FLastTimeWhite = currentMicros;
			}
//			inherited::SendOutput();
		}

	public:
		inline void SystemStart()
		{
			FCurrentValue = InitialValue;
			FTargetValue = InitialValue;

			OutputPin.Notify( &FCurrentValue );
		}

		inline void SystemLoopBegin( unsigned long currentMicros )
		{
			Generate( currentMicros, false );
		}

	};
//---------------------------------------------------------------------------
}

#ifdef __TEST_FOR_DEBUG_PRINTS__
#undef Serial
#endif

#endif
