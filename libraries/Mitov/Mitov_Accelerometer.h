////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2018 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_ACCELEROMETER_h
#define _MITOV_ACCELEROMETER_h

#include <Mitov.h>

namespace Mitov
{
//---------------------------------------------------------------------------
	class Memsic2125Accelerometer
	{
	public:
		OpenWire::SourcePin	XOutputPin;
		OpenWire::SourcePin	YOutputPin;

	public:
		bool	Enabled = true;
		bool	FOldValueX = false;
		bool	FOldValueY = false;

	protected:
		class DataSet
		{
		protected:
			unsigned long	FStartTime = 0;

		public:
			void DoReceive( bool AValue, OpenWire::SourcePin *AOutputPin )
			{
				unsigned long ANow = micros();
				if( AValue )
				{
					FStartTime = ANow;
					return;
				}

				long APeriod = ANow - FStartTime;

				if( APeriod == 0 )
					APeriod = 1;

				float AAcceleration = (( APeriod / 10 ) - 500) * 8;

				AOutputPin->Notify( &AAcceleration );
			}

		};


	protected:
		DataSet	FXDataSet;
		DataSet	FYDataSet;

	public:
		void XInputPin_o_Receive( void *_Data )
		{
			if( ! Enabled )
				return;

			bool AValue = *(bool *)_Data;
			if( FOldValueX == AValue )
				return;

			FOldValueX = AValue;
			FXDataSet.DoReceive( AValue, &XOutputPin );
		}

		void YInputPin_o_Receive( void *_Data )
		{
			if( ! Enabled )
				return;

			bool AValue = *(bool *)_Data;
			if( FOldValueY == AValue )
				return;

			FOldValueY = AValue;
			FYDataSet.DoReceive( AValue, &YOutputPin );
		}

	public:
		Memsic2125Accelerometer() :
			Enabled( true ),
			FOldValueX( false ),
			FOldValueY( false )
		{
		}
	};
//---------------------------------------------------------------------------
	class AccelerometerADXL335
	{
	public:
		OpenWire::SourcePin	AccelerationOutputPins[ 3 ];

	public:
		bool	Enabled = true;

	public:
		void AnalogInputPins_o_Receive( int AIndex, void *_Data )
		{
			if( ! Enabled )
				return;

			// min 0.27 and max 0.4
			float AValue = *(float*)_Data;

			float AOutValue = Func::MapRange<float>( AValue, 0.27, 0.4, -1.0, 1.0 );

			AccelerationOutputPins[ AIndex ].Notify( &AOutValue );
		}

	};
//---------------------------------------------------------------------------
}

#endif
