////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2018 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_ACCELERATIONTOANGLE_h
#define _MITOV_ACCELERATIONTOANGLE_h

#include <Mitov.h>

namespace Mitov
{
//---------------------------------------------------------------------------
	class TArduinoAccelerationToAngleOffsets
	{
	public:
		float	X = 0;
		float	Y = 0;
		float	Z = 0;

	};
//---------------------------------------------------------------------------
	class AccelerationToAngle
	{
	public:
		OpenWire::TypedSourcePin<float>	OutputPins[ 3 ];

		OpenWire::TypedSourcePin<float>	ForceOutputPin;

	public:
#ifdef _MITOV_ACCELERATION_TO_ANGLE_OFFSETS_
		TArduinoAccelerationToAngleOffsets	Offsets;
#endif

		Mitov::TAngleUnits	Units : 2;

	protected:
		bool	FChangeOnly : 1;
		bool	FModified : 1;

		float	FValues[ 3 ] = { 0 };

	protected:
		float ComputeAngle( float a, float b
#ifdef _MITOV_ACCELERATION_TO_ANGLE_OFFSETS_
			, float AOffset
#endif //_MITOV_ACCELERATION_TO_ANGLE_OFFSETS_
			)
		{
			// Hold the module so that Z is pointing 'up' and you can measure the heading with x&y
			// Calculate heading when the magnetometer is level, then correct for signs of axis.
			float AHeading = atan2( a, b ); // Y , X

//			Serial.println( AHeading );

			// Once you have your heading, you must then add your 'Declination Angle', which is the 'Error' of the magnetic field in your location.
			// Find yours here: http://www.magnetic-declination.com/
			// If you cannot find your Declination, comment out these two lines, your compass will be slightly off.

			// Correct for when signs are reversed.
#ifdef _MITOV_ACCELERATION_TO_ANGLE_OFFSETS_
			switch( Units )
			{
				case auDegree:
					AHeading += AOffset * PI / 180.0f;
					break;

				case auNormalized:
					AHeading += AOffset * 2 * PI;
					break;

				default:
					AHeading += AOffset;
			}
//			if( Units == auDegree )
//				AHeading += AOffset * MITOV_PI / 180;

//			else

#endif //_MITOV_ACCELERATION_TO_ANGLE_OFFSETS_
			AHeading = posmod( AHeading, 2*PI );
/*
			if(AHeading < 0)
				AHeading += 2*PI;
    
			// Check for wrap due to addition of declination.
			if(AHeading > 2*PI)
				AHeading -= 2*PI;
*/
			switch( Units )
			{
				case auDegree:
					AHeading *= 180.0f / PI;
					break;

				case auNormalized:
					AHeading /= 2 * PI;
					break;

			}

			return AHeading;
		}

	public:
		inline void SystemLoopEnd( unsigned long currentMicros )
		{
			if( ! FModified )
				return;

			float AValue;

			AValue = ComputeAngle( FValues[ 2 ], FValues[ 1 ]
#ifdef _MITOV_ACCELERATION_TO_ANGLE_OFFSETS_
				, Offsets.X
#endif // _MITOV_ACCELERATION_TO_ANGLE_OFFSETS_
				);
			OutputPins[ 0 ].SetValue( AValue, FChangeOnly );

			AValue = ComputeAngle( FValues[ 2 ], FValues[ 0 ]
#ifdef _MITOV_ACCELERATION_TO_ANGLE_OFFSETS_
				, Offsets.Y
#endif // _MITOV_ACCELERATION_TO_ANGLE_OFFSETS_
				);
			OutputPins[ 1 ].SetValue( AValue, FChangeOnly );

			AValue = ComputeAngle( FValues[ 1 ], FValues[ 0 ]
#ifdef _MITOV_ACCELERATION_TO_ANGLE_OFFSETS_
				, Offsets.Z
#endif // _MITOV_ACCELERATION_TO_ANGLE_OFFSETS_
				);
			OutputPins[ 2 ].SetValue( AValue, FChangeOnly );

			AValue = sqrt( FValues[ 0 ] * FValues[ 0 ] + FValues[ 1 ] * FValues[ 1 ] + FValues[ 2 ] * FValues[ 2 ] );
			ForceOutputPin.SetValue( AValue, FChangeOnly );

			FChangeOnly = true;
		}

	public:
		void InputPins_o_Receive( int AIndex, void *_Data )
		{
			FValues[ AIndex ] = *(float *)_Data;
			FModified = true;
		}

	public:
		AccelerationToAngle() :
			Units( auDegree ),
			FChangeOnly( false ),
			FModified( true )
		{
		}
	};
//---------------------------------------------------------------------------
}

#endif
