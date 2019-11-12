////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2018 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_HEAT_INDEDX_h
#define _MITOV_HEAT_INDEDX_h

#include <Mitov.h>

namespace Mitov
{
	class HeatIndex
	{
	public:
		OpenWire::SourcePin	OutputPin;

    public:
        bool	Enabled : 1;
		bool	InFahrenheit : 1;
		float	InitialTemperature = 0.0f;
		float	InitialHumidity = 0.0f;

	public:
		void TemperatureInputPin_o_Receive( void *_Data )
		{
			float AValue = *(float*)_Data;
			if( InitialTemperature == AValue )
				return;

			InitialTemperature = AValue;
			CalculateOutput();
		}

		void HumidityInputPin_o_Receive( void *_Data )
		{
			float AValue = *(float*)_Data;
			if( InitialHumidity == AValue )
				return;

			InitialHumidity = AValue;
			CalculateOutput();
		}

	protected:
		void CalculateOutput()
		{
			// Using both Rothfusz and Steadman's equations
			// http://www.wpc.ncep.noaa.gov/html/heatindex_equation.shtml

			float temperature = InitialTemperature;
			if ( ! InFahrenheit )
				temperature = Func::ConvertCtoF(temperature);

			float hi = 0.5 * (temperature + 61.0 + ((temperature - 68.0) * 1.2) + (InitialHumidity * 0.094));

			if( hi > 79 )
			{
				hi = -42.379 +
							 2.04901523 * temperature +
							10.14333127 * InitialHumidity +
							-0.22475541 * temperature * InitialHumidity +
							-0.00683783 * pow(temperature, 2) +
							-0.05481717 * pow(InitialHumidity, 2) +
							 0.00122874 * pow(temperature, 2) * InitialHumidity +
							 0.00085282 * temperature * pow(InitialHumidity, 2) +
							-0.00000199 * pow(temperature, 2) * pow(InitialHumidity, 2);

				if((InitialHumidity < 13) && (temperature >= 80.0) && (temperature <= 112.0))
					hi -= ((13.0 - InitialHumidity) * 0.25) * sqrt((17.0 - abs(temperature - 95.0)) * 0.05882);

				else if((InitialHumidity > 85.0) && (temperature >= 80.0) && (temperature <= 87.0))
					hi += ((InitialHumidity - 85.0) * 0.1) * ((87.0 - temperature) * 0.2);
			}

			if( ! InFahrenheit )
				hi = Func::ConvertFtoC( hi );

			OutputPin.Notify( &hi );
		}

	public:
		inline void SystemStart() 
		{
			CalculateOutput();
		}

	public:
		HeatIndex() :
			Enabled( true ),
			InFahrenheit( false )
		{
		}

	};
}

#endif
