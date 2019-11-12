////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2018 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_ESP32_h
#define _MITOV_ESP32_h

#include <Mitov.h>

#ifdef __TEST_FOR_DEBUG_PRINTS__
#define Serial UNGUARDED DEBUG PRINT!!!
#endif

namespace Mitov
{
//---------------------------------------------------------------------------
	template<int C_PIN_NUMBER, bool T_INITIAL_VALUE> class ESP32_DAC_DigitalChannel
	{
	public:
		OpenWire::SourcePin	OutputPin;

	protected:
        bool FIsOutput : 1;
        bool FIsPullUp : 1;
        bool FIsPullDown : 1;
        bool FIsOpenDrain : 1;
        bool FIsAnalog : 1;
		bool FLastOutput : 1;
		bool FRawInput : 1;
		bool FCombinedInOut : 1;

//	public:
//		OpenWire::SinkPin	AnalogInputPin;

	protected:
        void PinDirectionsInit()
        {
			if( FRawInput )
				return;

            if( FIsOutput )
			{
				if( FIsAnalog )
					pinMode( C_PIN_NUMBER, ANALOG );

				else
					pinMode( C_PIN_NUMBER, OUTPUT );
			}

            else
				pinMode( C_PIN_NUMBER, FIsPullUp ? INPUT_PULLUP : ( FIsPullDown ? INPUT_PULLDOWN : INPUT ) );
        }

	public:
		inline void SystemInit()
		{
            PinDirectionsInit();
		}

		inline void SystemStart()
		{
			if( FRawInput )
				return;

			FLastOutput = ( digitalRead( C_PIN_NUMBER ) == HIGH );
	    	OutputPin.SendValue( FLastOutput );
		}

		inline void SystemLoopBegin( unsigned long currentMicros )
        {
            if( ! FIsOutput )
            {
    			bool AValue = ( digitalRead( C_PIN_NUMBER ) == HIGH );
				if( AValue == FLastOutput )
					return;

				FLastOutput = AValue;
//	Serial.println( AData.Value );
	    		OutputPin.Notify( &AValue );
            }
        }

	public:
		void AnalogInputPin_o_Receive( void *_Data )
        {
            if( FIsOutput && FIsAnalog )
            {
                float AValue = *(float*)_Data;
                dacWrite( C_PIN_NUMBER, ( AValue * 255 ) + 0.5 );
            }
        }

		void DigitalInputPin_o_Receive( void *_Data )
		{
			if( FRawInput )
			{
				*((int*)_Data) = C_PIN_NUMBER;
				return;
			}

            if( ( FCombinedInOut || FIsOutput ) && (! FIsAnalog ))
            {
				bool AValue = *(bool *)_Data;

				if( AValue )
				  if( FCombinedInOut )
					  pinMode( C_PIN_NUMBER, OUTPUT );

                digitalWrite( C_PIN_NUMBER, AValue ? HIGH : LOW );

				if( ! AValue )
					if( FCombinedInOut )
						pinMode( C_PIN_NUMBER, FIsPullUp ? INPUT_PULLUP : ( FIsPullDown ? INPUT_PULLDOWN : INPUT ) );

            }
		}

	public:
        void SetIsOutput( bool AValue )
        {
            if( FIsOutput == AValue )
                return;

            FIsOutput = AValue;
            PinDirectionsInit();
        }

        void SetIsPullUp( bool AValue )
        {
            if( FIsPullUp == AValue )
                return;

            FIsPullUp = AValue;
            PinDirectionsInit();
        }

        void SetIsAnalog( bool AValue )
        {
            if( FIsAnalog == AValue )
                return;

            FIsAnalog = AValue;
            PinDirectionsInit();
        }

        void SetIsOpenDrain( bool AValue )
        {
            if( FIsOpenDrain == AValue )
                return;

            FIsOpenDrain = AValue;
            PinDirectionsInit();
        }

	public:
		ESP32_DAC_DigitalChannel( bool AIsOutput, bool AIsPullUp, bool AIsPullDown, bool AIsOpenDrain, bool AIsAnalog, bool ACombinedInOut, bool ARawInput ) :
            FIsOutput( AIsOutput ),
            FIsPullUp( AIsPullUp ),
			FIsPullDown( AIsPullDown ),
			FIsOpenDrain( AIsOpenDrain ),
            FIsAnalog( AIsAnalog ),
			FRawInput( ARawInput ),
			FCombinedInOut( ACombinedInOut )
		{
		}

	};
//---------------------------------------------------------------------------
}

#ifdef __TEST_FOR_DEBUG_PRINTS__
#undef Serial
#endif

#endif
