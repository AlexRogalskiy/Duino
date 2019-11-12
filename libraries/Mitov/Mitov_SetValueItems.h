////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2018 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_SETVALUEITEMS_h
#define _MITOV_SETVALUEITEMS_h

#include <Mitov.h>

namespace Mitov
{
//---------------------------------------------------------------------------
	template<typename T_OWNER, T_OWNER &C_OWNER, typename T> class ArduinoSetValueElement
	{
	public:
		T	Value = ::Mitov::GetDefaultValue<T>();

	public:
		inline void InputPin_o_Receive( void *_Data )
		{
			C_OWNER.ExternalSetValue( nullptr, Value );
		}

	};
//---------------------------------------------------------------------------
	template<typename T_OWNER, T_OWNER &C_OWNER, typename T> class ArduinoStateSetValueElement : public Mitov::BasicValueStateElement
	{
	protected:
		bool	FInputValue = false;

	public:
		T	Value = ::Mitov::GetDefaultValue<T>();

	public:
		virtual bool TryActive() override
		{
			if( FInputValue )
			{
				C_OWNER.ExternalSetValue( nullptr, Value );
				return true;
			}

			return false;
		}

	public:
		void InputPin_o_Receive( void *_Data )
		{
			if( FInputValue == *(bool *)_Data )
				return;

//			Serial.println( FInputValue );

			FInputValue = *(bool *)_Data;
//			if( FInputValue )
//				FOwner.ExternalSetValue( nullptr, Value );

//			else
			C_OWNER.ResetValue();
		}

	public:
		ArduinoStateSetValueElement()
		{
			C_OWNER.RegisterElement( this );
		}

	};
//---------------------------------------------------------------------------    
	template<typename T_OWNER, T_OWNER &C_OWNER> class ArduinoRampToValueAnalogElement : public RampToValueElementIntf
	{
	public:
		float	Value = 0.0f;
		float	Slope = 1.0f;

//	protected:
//		unsigned long	FLastTime = 0;

	public:
		inline void SystemLoopBegin( unsigned long currentMicros )
		{
			if( C_OWNER.FActiveElement != this )
				return;

//			Serial.println( "UpdateRamp" );
			float ACurrentValue = C_OWNER.GetCurrentValue();

			if( ACurrentValue == Value )
				C_OWNER.RegisterForTimeUpdate( this, false );

			else
			{
				float ARamp = abs( ( currentMicros - C_OWNER.FLastTime ) * Slope / 1000000 );
				if( ACurrentValue < Value )
				{
					ACurrentValue += ARamp;
//					Serial.println( "UpdateRamp 1" );
					if( ACurrentValue > Value )
						ACurrentValue = Value;

				}
				else
				{
//					Serial.println( "UpdateRamp 2" );
					ACurrentValue -= ARamp;
					if( ACurrentValue < Value )
						ACurrentValue = Value;

				}
				
				C_OWNER.ExternalSetValue( this, ACurrentValue );
			}
		}

	public:
		inline void InputPin_o_Receive( void *_Data )
		{
//			Serial.println( "DoReceived" );
			C_OWNER.RegisterForTimeUpdate( this, true );
//			C_OWNER.ExternalSetValue( this, Value );
		}

	};
//---------------------------------------------------------------------------
	template<typename T_OWNER, T_OWNER &C_OWNER> class ArduinoStateRampToValueAnalogElement : public Mitov::BasicValueStateElement, public RampToValueElementIntf
	{
		typedef Mitov::BasicValueStateElement inherited;

	protected:
		bool	FInputValue = false;

	public:
		float	Value = 0.0f;
		float	Slope = 1.0f;

	public:
		inline void SystemLoopBegin( unsigned long currentMicros )
		{
			if( C_OWNER.FActiveElement != this )
				return;

			float ACurrentValue = C_OWNER.GetCurrentValue();

//			Serial.println( "UpdateRamp" );
			if( ACurrentValue == Value )
				C_OWNER.RegisterForTimeUpdate( this, false );

			else
			{
				float ARamp = abs( ( currentMicros - C_OWNER.FLastTime ) * Slope / 1000000 );
				if( ACurrentValue < Value )
				{
					ACurrentValue += ARamp;
//					Serial.println( "UpdateRamp 1" );
					if( ACurrentValue > Value )
						ACurrentValue = Value;

				}
				else
				{
//					Serial.println( "UpdateRamp 2" );
					ACurrentValue -= ARamp;
					if( ACurrentValue < Value )
						ACurrentValue = Value;

				}
				
				C_OWNER.ExternalSetValue( this, ACurrentValue );
			}
		}

	public:
		virtual bool TryActive() override
		{
//			Serial.println( "TryActive" );
			if( FInputValue )
			{
				C_OWNER.RegisterForTimeUpdate( this, true );
				return true;
			}

			return false;
		}

	public:
		void InputPin_o_Receive( void *_Data )
		{
			if( FInputValue == *(bool *)_Data )
				return;

//			Serial.println( FInputValue );

			FInputValue = *(bool *)_Data;
			if( FInputValue )
				C_OWNER.RegisterForTimeUpdate( this, true );
//				C_OWNER.ExternalSetValue( nullptr, Value );

			else
			{
				C_OWNER.RegisterForTimeUpdate( this, false );
				C_OWNER.ResetValue();
			}
		}

	public:
		ArduinoStateRampToValueAnalogElement()
		{
			C_OWNER.RegisterElement( this );
		}

	};
//---------------------------------------------------------------------------
	template<typename T_OWNER, T_OWNER &C_OWNER> class ArduinoBinarySetValueElement
	{
	public:
		Mitov::Bytes	Value;

	public:
		inline void InputPin_o_Receive( void *_Data )
		{
			C_OWNER.ExternalSetValue( Value );
		}

	};
//---------------------------------------------------------------------------
}

#endif
