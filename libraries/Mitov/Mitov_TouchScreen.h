////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2018 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_TOUCH_SCREEN_h
#define _MITOV_TOUCH_SCREEN_h

#include <Mitov.h>
#include <Mitov_Display_ILI9341.h>

namespace Mitov
{
//---------------------------------------------------------------------------
//	TArduinoTFTLCDShieldTouchCoordinateMode = ( cmScreen, cmNormalized, cmRaw );
//---------------------------------------------------------------------------
	class TArduinoTFTLCDShieldTouchCoordinate
	{
	public:
		bool		Raw = false; // : 1;
//		TArduinoTFTLCDShieldTouchCoordinateMode	Mode : 2;
		uint16_t	MinRaw;
		uint16_t	MaxRaw;

	};
//---------------------------------------------------------------------------
	template<typename T_OWNER, T_OWNER &C_OWNER> class TFTShieldTouchScreen
	{
	protected:
		static const int NUMSAMPLES = 5;

	public:
		OpenWire::SourcePin	XOutputPin;
		OpenWire::SourcePin	YOutputPin;
		OpenWire::SourcePin	PressureOutputPin;

	public:
		float MinPressure = 0.01;
		TArduinoTFTLCDShieldTouchCoordinate	XCoordinate;
		TArduinoTFTLCDShieldTouchCoordinate	YCoordinate;

	public:
		void ClockInputPin_o_Receive( void *_Data )
		{
			const int _yp = A1;  // must be an analog pin, use "An" notation!
			const int _xm = A2;  // must be an analog pin, use "An" notation!
			const int _ym = 7;   // can be a digital pin
			const int _xp = 6;   // can be a digital pin

			int32_t x, y;
			int samples[NUMSAMPLES];
			uint8_t i;

			bool	valid = true;

//			delay( 10 );

			pinMode(_yp, INPUT);
			pinMode(_ym, INPUT);

//			delay( 1 );

			digitalWrite(_yp, LOW);
			digitalWrite(_ym, LOW);

			pinMode(_xp, OUTPUT);
			pinMode(_xm, OUTPUT);

			digitalWrite(_xp, HIGH);
			digitalWrite(_xm, LOW);

			for (i=0; i<NUMSAMPLES; i++)
				samples[i] = analogRead(_yp);
  
			Func::insert_sort(samples, NUMSAMPLES);
			if( abs( samples[ 1 ] - samples[ 3 ] ) > 10 )
				valid = false;

			x = (1023-samples[NUMSAMPLES/2]);

			pinMode(_xp, INPUT);
			pinMode(_xm, INPUT);
//		   *portOutputRegister(xp_port) &= ~xp_pin;
			digitalWrite(_xp, LOW);
   
			pinMode(_yp, OUTPUT);
//		   *portOutputRegister(yp_port) |= yp_pin;
			digitalWrite(_yp, HIGH);
			pinMode(_ym, OUTPUT);
  
			for (i=0; i<NUMSAMPLES; i++)
				samples[i] = analogRead(_xm);
		   
			Func::insert_sort(samples, NUMSAMPLES);
			if( abs( samples[ 1 ] - samples[ 3 ] ) > 10 )
				valid = false;

			y = (1023-samples[NUMSAMPLES/2]);

			// Set X+ to ground
			pinMode(_xp, OUTPUT);
//			*portOutputRegister(xp_port) &= ~xp_pin;
			digitalWrite(_xp, LOW);
  
			// Set Y- to VCC
//			*portOutputRegister(ym_port) |= ym_pin;
			digitalWrite(_ym, HIGH); 
  
			// Hi-Z X- and Y+
//			*portOutputRegister(yp_port) &= ~yp_pin;
			digitalWrite(_yp, LOW);
			pinMode(_yp, INPUT);
  
/*
			for (i=0; i<NUMSAMPLES; i++)
				samples[i] = analogRead(_xm);

			int z1 = samples[NUMSAMPLES/2]; //analogRead(_xm); 

			for (i=0; i<NUMSAMPLES; i++)
				samples[i] = analogRead(_yp);

			int z2 = samples[NUMSAMPLES/2];
*/
			int z1 = analogRead(_xm); 
			int z2 = analogRead(_yp);

/*
			float _rxplate = 300;
			if (_rxplate != 0) 
			{
				// now read the x 
				float rtouch;
				rtouch = z2;
				rtouch /= z1;
				rtouch -= 1;
				rtouch *= x;
				rtouch *= _rxplate;
				rtouch /= 1024;
     
				z = rtouch;
			} 
			else
*/
//			z = (1023-(z2-z1));


//			Serial.println( z );
			float z;
			if (! valid)
				z = 0;

			else
			{
//				float ARange = 1023 - z2;
				z = (1023.0-(z2-z1)) / 1023.0;
			}

			if( z > MinPressure )
			{
//				Serial.println( y );
				if( ! XCoordinate.Raw )
				{
//					case cmNormalized:
//						x = MapRange( x, XCoordinate.MinRaw, XCoordinate.MaxRaw, 0.0, 1.0 ); break;

//					case cmScreen:
					{
						x = Func::MapRange<float>( MitovMin<float>( XCoordinate.MaxRaw, MitovMax<float>( XCoordinate.MinRaw, x )), YCoordinate.MinRaw, YCoordinate.MaxRaw, 0, DisplayILI9341::HEIGHT );
//						y = Func::MapRange<float>( MitovMin( YCoordinate.MaxRaw, MitovMax( YCoordinate.MinRaw, y )), YCoordinate.MinRaw, YCoordinate.MaxRaw, 0, FOwner.height() );
						switch( C_OWNER.Orientation )
						{
							case goDown :
							case goLeft :
								x = DisplayILI9341::HEIGHT - x;
						}
					}
				}

				if( ! YCoordinate.Raw )
				{
//					case cmNormalized:
//						y = MapRange( y, YCoordinate.MinRaw, YCoordinate.MaxRaw, 0.0, 1.0 ); break;

//					case cmScreen:
					{
						y = Func::MapRange<float>( MitovMin<float>( YCoordinate.MaxRaw, MitovMax<float>( YCoordinate.MinRaw, y )), XCoordinate.MinRaw, XCoordinate.MaxRaw, 0, DisplayILI9341::WIDTH );
						switch( C_OWNER.Orientation )
						{
							case goDown :
							case goLeft :
								y = DisplayILI9341::WIDTH - y;
						}
					}
				}

				switch( C_OWNER.Orientation )
				{
					case goLeft :
					case goRight :
						XOutputPin.Notify( &x );
						YOutputPin.Notify( &y );
						break;

					case goUp :
					case goDown :
						XOutputPin.Notify( &y );
						YOutputPin.Notify( &x );
						break;
				}
			}

			PressureOutputPin.Notify( &z );
			pinMode(_xp, OUTPUT);
			pinMode(_xm, OUTPUT);

			pinMode(_yp, OUTPUT);
			pinMode(_xm, OUTPUT);

//			delay( 10 );

//			Serial.println( z );
		}

	public:
		inline void SystemLoopBegin( unsigned long currentMicros )
		{
			ClockInputPin_o_Receive( nullptr );
		}

	public:
		TFTShieldTouchScreen()
		{
//			YCoordinate.MinRaw = 90;
//			YCoordinate.MaxRaw = 920;
			YCoordinate.MinRaw = 68; // 90 - 22;
			YCoordinate.MaxRaw = 898; // 920 - 22;

			XCoordinate.MinRaw = 130;
			XCoordinate.MaxRaw = 900;
		}
	};
}

#endif
