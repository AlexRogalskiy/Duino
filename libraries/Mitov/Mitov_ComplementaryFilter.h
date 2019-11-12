////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2018 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_COMPLEMENTARYFILTER_h
#define _MITOV_COMPLEMENTARYFILTER_h

#include <Mitov.h>

namespace Mitov
{
//---------------------------------------------------------------------------
	class ComplementaryAnalog
	{
	public:
		OpenWire::TypedStartSourcePin<float>	OutputPin;

	public:
		float	TimeConstant = 0.075f;
		bool	Enabled : 1;

	protected:
		bool			FHasTime : 1;
		unsigned long	FLastTime = 0;
		float			FValue = 0;
		float			FRateValue = 0;

	public:
		void InputPin_o_Receive( void *_Data )
		{
			FValue = *(float *)_Data;
			if( ! Enabled )
				OutputPin.SetValue( *(float *)_Data );

		}

		inline void RateInputPin_o_Receive( void *_Data )
		{
			FRateValue = *(float *)_Data;
		}

	public:
		inline void SystemLoopEnd( unsigned long currentMicros ) 
		{
			unsigned long ATime = micros();
			if( ! FHasTime )			
			{
				FHasTime = true;
				FLastTime = ATime;
				OutputPin.SetValue( FValue );
				return;
			}

			if( ! Enabled )
				return;

			float dT = ( ATime - FLastTime ) / 1000000.0f;
			FLastTime = ATime;

			float a = TimeConstant / ( TimeConstant + dT );

			float AAngle = a * ( OutputPin.Value + FRateValue * dT) + (1-a) * FValue;

			OutputPin.SetValue( AAngle );
		}

	public:
		ComplementaryAnalog() :
			FHasTime( false ),
			Enabled( true )
		{
		}

	};
//---------------------------------------------------------------------------
	class ComplementaryAngle
	{
	public:
		OpenWire::TypedStartSourcePin<float>	OutputPin;

	public:
		float	TimeConstant = 0.075f;
		Mitov::TAngleUnits	Units : 2;
		bool	Enabled : 1;

	protected:
		bool			FHasTime : 1;
		unsigned long	FLastTime = 0;
		float			FValue = 0;
		float			FRateValue = 0;

	public:
		void InputPin_o_Receive( void *_Data )
		{
			FValue = *(float *)_Data;
			if( ! Enabled )
				OutputPin.SetValue( *(float *)_Data );

		}

		inline void RateInputPin_o_Receive( void *_Data )
		{
			FRateValue = *(float *)_Data;
		}

	public:
		inline void SystemLoopEnd( unsigned long currentMicros ) 
		{
			unsigned long ATime = micros();
			if( ! FHasTime )			
			{
				FHasTime = true;
				FLastTime = ATime;
				OutputPin.SetValue( FValue );
				return;
			}

			if( ! Enabled )
				return;

			float dT = ( ATime - FLastTime ) / 1000000.0f;
			FLastTime = ATime;

			float a = TimeConstant / ( TimeConstant + dT );

			float AAngle = a * ( OutputPin.Value + FRateValue * dT ) + (1-a) * FValue;

			switch( Units )
			{
				case auDegree:
					AAngle = posmod( AAngle, 360.0f );
					break;

				case auRadians:
					AAngle = posmod( AAngle, 2 * PI );
					break;

				case auNormalized:
					AAngle = posmod( AAngle, 1.0f );
					break;

			}

			OutputPin.SetValue( AAngle );
		}

	public:
		ComplementaryAngle() :
			Units( auDegree ),
			FHasTime( false ),
			Enabled( true )
		{
		}
	};
//---------------------------------------------------------------------------
	class Complementary2
	{
	public:
		OpenWire::TypedStartSourcePin<float>	OutputPin;

	public:
		float	Coefficient = 10.0f;
		bool	Enabled : 1;

	protected:
		bool			FHasTime : 1;
		unsigned long	FLastTime = 0;
		float			FValue = 0;
		float			FRateValue = 0;

	public:
		void InputPin_o_Receive( void *_Data )
		{
			FValue = *(float *)_Data;
			if( ! Enabled )
				OutputPin.SetValue( *(float *)_Data );

		}

		inline void RateInputPin_o_Receive( void *_Data )
		{
			FRateValue = *(float *)_Data;
		}

	public:
		inline void SystemLoopEnd( unsigned long currentMicros ) 
		{
			unsigned long ATime = micros();
			if( ! FHasTime )			
			{
				FHasTime = true;
				FLastTime = ATime;
				OutputPin.SetValue( FValue );
				return;
			}

			if( ! Enabled )
				return;

			float dT = ( ATime - FLastTime ) / 1000000.0f;
			FLastTime = ATime;

			float x_angle2C = OutputPin.Value;
			float newAngle = FValue;

			float x1 = (newAngle -   x_angle2C) * Coefficient * Coefficient;
			float y1 = dT * x1 + y1;
			float x2 = y1 + (newAngle -   x_angle2C) * 2 * Coefficient + FRateValue;
			x_angle2C = dT * x2 + x_angle2C;

			OutputPin.SetValue( x_angle2C );
		}

	public:
		Complementary2() :
			FHasTime( false ),
			Enabled( true )
		{
		}

	};
//---------------------------------------------------------------------------
	class ComplementaryAngle2
	{
	public:
		OpenWire::TypedStartSourcePin<float>	OutputPin;

	public:
		float	Coefficient = 10.0f;
		Mitov::TAngleUnits	Units : 2;
		bool	Enabled : 1;

	protected:
		bool			FHasTime : 1;
		unsigned long	FLastTime = 0;
		float			FValue = 0;
		float			FRateValue = 0;

	public:
		void InputPin_o_Receive( void *_Data )
		{
			FValue = *(float *)_Data;
			if( ! Enabled )
				OutputPin.SetValue( *(float *)_Data );

		}

		inline void RateInputPin_o_Receive( void *_Data )
		{
			FRateValue = *(float *)_Data;
		}

	public:
		inline void SystemLoopEnd( unsigned long currentMicros ) 
		{
			unsigned long ATime = micros();
			if( ! FHasTime )			
			{
				FHasTime = true;
				FLastTime = ATime;
				OutputPin.SetValue( FValue );
				return;
			}

			if( ! Enabled )
				return;

			float dT = ( ATime - FLastTime ) / 1000000.0f;
			FLastTime = ATime;

			float x_angle2C = OutputPin.Value;
			float newAngle = FValue;

			float x1 = (newAngle -   x_angle2C) * Coefficient * Coefficient;
			float y1 = dT * x1 + y1;
			float x2 = y1 + (newAngle -   x_angle2C) * 2 * Coefficient + FRateValue;
			x_angle2C = dT * x2 + x_angle2C;

			switch( Units )
			{
				case auDegree:
					x_angle2C = posmod( x_angle2C, 360.0f );
					break;

				case auRadians:
					x_angle2C = posmod( x_angle2C, 2 * PI );
					break;

				case auNormalized:
					x_angle2C = posmod( x_angle2C, 1.0f );
					break;

			}

			OutputPin.SetValue( x_angle2C );
		}

	public:
		ComplementaryAngle2() :
			Units( auDegree ),
			FHasTime( false ),
			Enabled( true )
		{
		}

	};
//---------------------------------------------------------------------------
}

#endif
