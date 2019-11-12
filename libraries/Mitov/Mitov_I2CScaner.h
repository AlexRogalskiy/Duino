////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2018 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_I2C_SCANNER_h
#define _MITOV_I2C_SCANNER_h

#include <Mitov.h>
#include <Wire.h>
#include <Mitov_Basic_I2C.h>

namespace Mitov
{
	template<typename T_I2C, T_I2C &C_I2C> class I2CScaner
	{
	public:
		OpenWire::SourcePin	ScanningOutputPin;
		OpenWire::SourcePin	ScanningAddressOutputPin;
		OpenWire::SourcePin	AddressOutputPin;

	public:
		void ScanInputPin_o_Receive( void *_Data )
		{
			ScanningOutputPin.SendValue( true );
			for( uint8_t address = 1; address < 127; address++ ) 
			{
				ScanningAddressOutputPin.SendValue<uint32_t>( address );
				// The i2c_scanner uses the return value of
				// the Write.endTransmisstion to see if
				// a device did acknowledge to the address.
				C_I2C.beginTransmission(address);
				uint8_t error = C_I2C.endTransmission();

				if (error == 0)
					AddressOutputPin.SendValue<uint32_t>( address );
/*
				else if (error==4) 
				{
				}    
*/
			}

			ScanningOutputPin.SendValue( false );
		}

	public:
		inline void SystemStart()
		{
			ScanInputPin_o_Receive( nullptr );
		}

	};
}

#endif
