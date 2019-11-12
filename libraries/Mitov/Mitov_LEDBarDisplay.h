////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2018 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_LED_BAR_DISPLAY_h
#define _MITOV_LED_BAR_DISPLAY_h

#include <Mitov.h>

namespace Mitov
{
	template<typename T> class LEDBarDisplay
	{
	public:
		Mitov::SimpleList<OpenWire::SourcePin>	BarsOutputPins;

	protected:
		T		FValue;

	public:
		T		InitialValue = 0;
		bool	Enabled : 1;

	public:
		bool InvertedBars : 1;
		bool SingleBar : 1;

	public:
		void InputPin_o_Receive( void *_Data )
		{
			FValue = *(T*)_Data;
			UpdateDisplay();
		}

	protected:
		void UpdateDisplay()
		{
			int32_t AValue = (Enabled) ? FValue : 0;
			DisplayValue( AValue );
		}

	protected:
		void DisplayValue( long AValue )
		{
			for( int i = 0; i < BarsOutputPins.size(); ++i )
			{
				bool ABitValue;
				if( SingleBar )
					ABitValue = ( AValue == 0 ) ^ InvertedBars;

				else
					ABitValue = ( AValue > 0 ) ^ InvertedBars;

				BarsOutputPins[ i ].Notify( &ABitValue );
				--AValue;
			}
		}

	public:
		inline void SystemInit()
		{
			FValue = InitialValue;
			UpdateDisplay();
		}

	public:
		LEDBarDisplay() :
			Enabled( true ),
			InvertedBars( false ),
			SingleBar( false )
		{
		}

	};
//---------------------------------------------------------------------------
	template<> void LEDBarDisplay<float>::UpdateDisplay()
	{
		int AValue;
		if( Enabled )
		{
			if( SingleBar )
				AValue = FValue * BarsOutputPins.size();

			else
				AValue = FValue * BarsOutputPins.size() + 0.5;
		}

		else
			AValue = 0;

		DisplayValue( AValue );
	}
//---------------------------------------------------------------------------
}

#endif
