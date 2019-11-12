////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2018 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_L298N_DUAL_MOTOR_h
#define _MITOV_L298N_DUAL_MOTOR_h

#include <Mitov.h>

#ifdef __TEST_FOR_DEBUG_PRINTS__
#define Serial UNGUARDED DEBUG PRINT!!!
#endif

namespace Mitov
{
	template<typename T_FORWARD_IMPLEMENTATION, typename T_BACK_IMPLEMENTATION, typename T_SPEED_IMPLEMENTATION> class L298NDualMotorDriverChannel : public T_FORWARD_IMPLEMENTATION, public T_BACK_IMPLEMENTATION, public T_SPEED_IMPLEMENTATION
	{
	public:
		inline OpenWire::SourcePin &ForwardOutputPin()
		{
			return T_FORWARD_IMPLEMENTATION::GetPinInstance();
		}

		inline OpenWire::SourcePin &ReverseOutputPin()
		{
			return T_BACK_IMPLEMENTATION::GetPinInstance();
		}

		inline OpenWire::SourcePin &SpeedOutputPin()
		{
			return T_SPEED_IMPLEMENTATION::GetPinInstance();
		}

	public:
		float	InitialValue = 0.5f;

	public:
		void InputPin_o_Receive( void *_Data )
		{
			float ASpeed = constrain( *(float *)_Data, 0.0, 1.0 );
			if( InitialValue == ASpeed )
				return;

			InitialValue = ASpeed;
			UpdateOutputs();
		}

	public:
		void UpdateOutputs()
		{
			float AOutSpeed = abs( InitialValue - 0.5 ) * 2;
			bool ADirection = InitialValue > 0.5;


//			ReverseOutputPin.Notify( &ADirection );
			T_BACK_IMPLEMENTATION::SetPinValue( ADirection );
			ADirection = !ADirection;
//			ForwardOutputPin.Notify( &ADirection );
			T_FORWARD_IMPLEMENTATION::SetPinValue( ADirection );

//			SpeedOutputPin.Notify( &AOutSpeed );
			T_SPEED_IMPLEMENTATION::SetPinValue( AOutSpeed );
		}

	public:
		inline void SystemStart()
		{
			UpdateOutputs();
		}

		inline void SystemLoopBegin( unsigned long currentMicros ) {} // Placeholder for *_Slopped compatibility

	};
//---------------------------------------------------------------------------
	template<typename T_FORWARD_IMPLEMENTATION, typename T_BACK_IMPLEMENTATION, typename T_SPEED_IMPLEMENTATION> class L298NDualMotorDriverChannel_Enable : public T_FORWARD_IMPLEMENTATION, public T_BACK_IMPLEMENTATION, public T_SPEED_IMPLEMENTATION
	{
	public:
		inline OpenWire::SourcePin &ForwardOutputPin()
		{
			return T_FORWARD_IMPLEMENTATION::GetPinInstance();
		}

		inline OpenWire::SourcePin &ReverseOutputPin()
		{
			return T_BACK_IMPLEMENTATION::GetPinInstance();
		}

		inline OpenWire::SourcePin &SpeedOutputPin()
		{
			return T_SPEED_IMPLEMENTATION::GetPinInstance();
		}

	public:
		bool	Enabled = true;
		float	InitialValue = 0.5f;

	public:
		void InputPin_o_Receive( void *_Data )
		{
			float ASpeed = constrain( *(float *)_Data, 0.0, 1.0 );
			if( InitialValue == ASpeed )
				return;

			InitialValue = ASpeed;
			UpdateOutputs();
		}

	public:
		void	UpdateOutputs()
		{
			if( Enabled )
			{
				float AOutSpeed = abs( InitialValue - 0.5 ) * 2;
				bool ADirection = InitialValue > 0.5;

//				ReverseOutputPin.Notify( &ADirection );
				T_BACK_IMPLEMENTATION::SetPinValue( ADirection );
				ADirection = !ADirection;
//				ForwardOutputPin.Notify( &ADirection );
				T_FORWARD_IMPLEMENTATION::SetPinValue( ADirection );

//				SpeedOutputPin.Notify( &AOutSpeed );
				T_SPEED_IMPLEMENTATION::SetPinValue( AOutSpeed );
			}

			else
			{
//				SpeedOutputPin.SendValue( 0.5 );
				T_SPEED_IMPLEMENTATION::SetPinValue( 0.5 );
//				ForwardOutputPin.SendValue( false );
				T_FORWARD_IMPLEMENTATION::SetPinValue( false );
//				ReverseOutputPin.SendValue( false );
				T_BACK_IMPLEMENTATION::SetPinValue( false );
			}
		}

	public:
		inline void SystemStart()
		{
			UpdateOutputs();
		}

		inline void SystemLoopBegin( unsigned long currentMicros ) {} // Placeholder for *_Slopped compatibility

	};
//---------------------------------------------------------------------------
	template<typename T_FORWARD_IMPLEMENTATION, typename T_BACK_IMPLEMENTATION, typename T_SPEED_IMPLEMENTATION> class L298NDualMotorDriverChannel_Slopped : public T_FORWARD_IMPLEMENTATION, public T_BACK_IMPLEMENTATION, public T_SPEED_IMPLEMENTATION
	{
	public:
		inline OpenWire::SourcePin &ForwardOutputPin()
		{
			return T_FORWARD_IMPLEMENTATION::GetPinInstance();
		}

		inline OpenWire::SourcePin &ReverseOutputPin()
		{
			return T_BACK_IMPLEMENTATION::GetPinInstance();
		}

		inline OpenWire::SourcePin &SpeedOutputPin()
		{
			return T_SPEED_IMPLEMENTATION::GetPinInstance();
		}

	public:
		float	SlopeRun = 0.0f;
		float	SlopeStop = 0.0f;
		float	InitialValue = 0.5f;

	protected:
		float	FCurrentValue = 0.0f;
		unsigned long	FLastTime = 0;

	public:
		void InputPin_o_Receive( void *_Data )
		{
			float ASpeed = constrain( *(float *)_Data, 0.0, 1.0 );
			if( InitialValue == ASpeed )
				return;

			InitialValue = ASpeed;
			UpdateOutputs();
		}

	public:
		void	UpdateOutputs()
		{
			if( FCurrentValue != 0.5 )
			{
				float AOutSpeed = abs( FCurrentValue - 0.5 ) * 2;
				bool ADirection = FCurrentValue > 0.5;

//				ReverseOutputPin.Notify( &ADirection );
				T_BACK_IMPLEMENTATION::SetPinValue( ADirection );
				ADirection = !ADirection;
//				ForwardOutputPin.Notify( &ADirection );
				T_FORWARD_IMPLEMENTATION::SetPinValue( ADirection );

//				SpeedOutputPin.Notify( &AOutSpeed );
				T_SPEED_IMPLEMENTATION::SetPinValue( AOutSpeed );
			}

			else
			{
//				SpeedOutputPin.SendValue( 0.5 );
				T_SPEED_IMPLEMENTATION::SetPinValue( 0.5 );
//				ForwardOutputPin.SendValue( false );
				T_FORWARD_IMPLEMENTATION::SetPinValue( false );
//				ReverseOutputPin.SendValue( false );
				T_BACK_IMPLEMENTATION::SetPinValue( false );
			}
		}

	public:
		inline void SystemStart()
		{
			FCurrentValue = InitialValue;

			UpdateOutputs();
		}

		inline void SystemLoopBegin( unsigned long currentMicros )
		{
			float ATargetValue = InitialValue;

			if( FCurrentValue != ATargetValue )
			{
				float Slope = SlopeRun;
				if( FCurrentValue > ATargetValue )
				{
					if( ATargetValue > 0.5 )
						Slope = SlopeStop;
				}

				else
				{
					if( ATargetValue < 0.5 )
						Slope = SlopeStop;
				}

				if( Slope == 0 )
					FCurrentValue = ATargetValue;

				else
				{
					float ARamp = abs( ( currentMicros - FLastTime ) * Slope / 1000000 );
					if( FCurrentValue < ATargetValue )
					{
						FCurrentValue += ARamp;
						if( FCurrentValue > ATargetValue )
							FCurrentValue = ATargetValue;

					}
					else
					{
						FCurrentValue -= ARamp;
						if( FCurrentValue < ATargetValue )
							FCurrentValue = ATargetValue;

					}
				}

				UpdateOutputs();
			}

			FLastTime = currentMicros;
		}

	};
//---------------------------------------------------------------------------
	template<typename T_FORWARD_IMPLEMENTATION, typename T_BACK_IMPLEMENTATION, typename T_SPEED_IMPLEMENTATION> class L298NDualMotorDriverChannel_Slopped_Enable : public T_FORWARD_IMPLEMENTATION, public T_BACK_IMPLEMENTATION, public T_SPEED_IMPLEMENTATION
	{
	public:
		inline OpenWire::SourcePin &ForwardOutputPin()
		{
			return T_FORWARD_IMPLEMENTATION::GetPinInstance();
		}

		inline OpenWire::SourcePin &ReverseOutputPin()
		{
			return T_BACK_IMPLEMENTATION::GetPinInstance();
		}

		inline OpenWire::SourcePin &SpeedOutputPin()
		{
			return T_SPEED_IMPLEMENTATION::GetPinInstance();
		}

	public:
		float	SlopeRun = 0.0f;
		float	SlopeStop = 0.0f;
		float	InitialValue = 0.5f;
		bool	Enabled = true;

	protected:
		float	FCurrentValue = 0.0f;
		unsigned long	FLastTime = 0;

	public:
		void InputPin_o_Receive( void *_Data )
		{
			float ASpeed = constrain( *(float *)_Data, 0.0, 1.0 );
			if( InitialValue == ASpeed )
				return;

			InitialValue = ASpeed;
			UpdateOutputs();
		}

	public:
		void	UpdateOutputs()
		{
			if( FCurrentValue != 0.5 )
			{
				float AOutSpeed = abs( FCurrentValue - 0.5 ) * 2;
				bool ADirection = FCurrentValue > 0.5;

//				ReverseOutputPin.Notify( &ADirection );
				T_BACK_IMPLEMENTATION::SetPinValue( ADirection );
				ADirection = !ADirection;
//				ForwardOutputPin.Notify( &ADirection );
				T_FORWARD_IMPLEMENTATION::SetPinValue( ADirection );

//				SpeedOutputPin.Notify( &AOutSpeed );
				T_SPEED_IMPLEMENTATION::SetPinValue( AOutSpeed );
			}

			else
			{
//				SpeedOutputPin.SendValue( 0.5 );
				T_SPEED_IMPLEMENTATION::SetPinValue( 0.5 );
//				ForwardOutputPin.SendValue( false );
				T_FORWARD_IMPLEMENTATION::SetPinValue( false );
//				ReverseOutputPin.SendValue( false );
				T_BACK_IMPLEMENTATION::SetPinValue( false );
			}
		}

	public:
		inline void SystemStart()
		{
			FCurrentValue = InitialValue;

			UpdateOutputs();
		}

		inline void SystemLoopBegin( unsigned long currentMicros )
		{
			float ATargetValue = (Enabled) ? InitialValue : 0.5;

			if( FCurrentValue != ATargetValue )
			{
				float Slope = SlopeRun;
				if( FCurrentValue > ATargetValue )
				{
					if( ATargetValue > 0.5 )
						Slope = SlopeStop;
				}

				else
				{
					if( ATargetValue < 0.5 )
						Slope = SlopeStop;
				}

				if( Slope == 0 )
					FCurrentValue = ATargetValue;

				else
				{
					float ARamp = abs( ( currentMicros - FLastTime ) * Slope / 1000000 );
					if( FCurrentValue < ATargetValue )
					{
						FCurrentValue += ARamp;
						if( FCurrentValue > ATargetValue )
							FCurrentValue = ATargetValue;

					}
					else
					{
						FCurrentValue -= ARamp;
						if( FCurrentValue < ATargetValue )
							FCurrentValue = ATargetValue;

					}
				}

				UpdateOutputs();
			}

			FLastTime = currentMicros;
		}

	};
//---------------------------------------------------------------------------
	template<int PIN_ENABLE_1, int PIN_FORWARD_1, int PIN_REVERSE_1, int PIN_ENABLE_2, int PIN_FORWARD_2, int PIN_REVERSE_2> class MotorShieldChannel
	{
	public:
		float	InitialValue = 0.5f;
		bool	Enabled : 1;

	protected:
		bool	FIsSecond : 1;

	public:
		void InputPin_o_Receive( void *_Data )
		{
			float ASpeed = constrain( *(float *)_Data, 0.0, 1.0 );
			if( InitialValue == ASpeed )
				return;

			InitialValue = ASpeed;
			UpdateOutputs();
		}

	public:
		void	UpdateOutputs()
		{
			bool AForward;
			bool AReverse;
			float AOutSpeed;
			if( Enabled && ( InitialValue != 0.5 ))
			{
				AOutSpeed = abs( InitialValue - 0.5 ) * 2;
				bool ADirection = InitialValue > 0.5;

				AReverse = ADirection;
				AForward = ! ADirection;
			}

			else
			{
				AOutSpeed = 0;
				AReverse = false;
				AForward = false;
			}

			if( FIsSecond )
			{
				digitalWrite( PIN_FORWARD_2, AForward );
				digitalWrite( PIN_REVERSE_2, AReverse );
				analogWrite( PIN_ENABLE_2, ( AOutSpeed * PWMRANGE ) + 0.5 );
//				analogWrite( 3, ( AOutSpeed * PWMRANGE ) + 0.5 );
			}
			else
			{
				digitalWrite( PIN_FORWARD_1, AForward );
				digitalWrite( PIN_REVERSE_1, AReverse );
				analogWrite( PIN_ENABLE_1, ( AOutSpeed * PWMRANGE ) + 0.5 );
			}

		}

	public:
		inline void SystemStart()
		{
			UpdateOutputs();
		}

		inline void SystemLoopBegin( unsigned long currentMicros ) {} // Placeholder for *_Slopped compatibility

	public:
		MotorShieldChannel( bool AIsSecond ) :
			Enabled( true ),
			FIsSecond( AIsSecond )
		{
			if( AIsSecond )
			{
				pinMode( PIN_ENABLE_2, OUTPUT );
				pinMode( PIN_FORWARD_2, OUTPUT );
				pinMode( PIN_REVERSE_2, OUTPUT );
			}
			else
			{
				pinMode( PIN_ENABLE_1, OUTPUT );
				pinMode( PIN_FORWARD_1, OUTPUT );
				pinMode( PIN_REVERSE_1, OUTPUT );
			}
		}
	};

//---------------------------------------------------------------------------
	template<int PIN_ENABLE_1, int PIN_FORWARD_1, int PIN_REVERSE_1, int PIN_ENABLE_2, int PIN_FORWARD_2, int PIN_REVERSE_2> class MotorShieldChannel_Enable : public MotorShieldChannel<PIN_ENABLE_1, PIN_FORWARD_1, PIN_REVERSE_1, PIN_ENABLE_2, PIN_FORWARD_2, PIN_REVERSE_2> {};
//---------------------------------------------------------------------------
	template<int PIN_ENABLE_1, int PIN_FORWARD_1, int PIN_REVERSE_1, int PIN_ENABLE_2, int PIN_FORWARD_2, int PIN_REVERSE_2> class MotorShieldChannel_Slopped
	{
	public:
		float	SlopeRun = 0.0f;
		float	SlopeStop = 0.0f;
		float	InitialValue = 0.5f;
		bool	Enabled : 1;

	protected:
		bool	FIsSecond : 1;
		float	FCurrentValue = 0.0f;
		unsigned long	FLastTime = 0;

	public:
		void InputPin_o_Receive( void *_Data )
		{
			float ASpeed = constrain( *(float *)_Data, 0.0, 1.0 );
			if( InitialValue == ASpeed )
				return;

			InitialValue = ASpeed;
			UpdateOutputs();
		}

	public:
		void	UpdateOutputs()
		{
			bool AForward;
			bool AReverse;
			float AOutSpeed;
			if( FCurrentValue != 0.5 )
			{
				AOutSpeed = abs( FCurrentValue - 0.5 ) * 2;
				bool ADirection = FCurrentValue > 0.5;

				AReverse = ADirection;
				AForward = ! ADirection;
			}

			else
			{
				AOutSpeed = 0;
				AReverse = false;
				AForward = false;
			}

			if( FIsSecond )
			{
				digitalWrite( PIN_FORWARD_2, AForward );
				digitalWrite( PIN_REVERSE_2, AReverse );
				analogWrite( PIN_ENABLE_2, ( AOutSpeed * PWMRANGE ) + 0.5 );
			}
			else
			{
				digitalWrite( PIN_FORWARD_1, AForward );
				digitalWrite( PIN_REVERSE_1, AReverse );
				analogWrite( PIN_ENABLE_1, ( AOutSpeed * PWMRANGE ) + 0.5 );
			}

		}

	public:
		inline void SystemStart()
		{
			FCurrentValue = InitialValue;

			UpdateOutputs();
		}

		inline void SystemLoopBegin( unsigned long currentMicros )
		{
			float ATargetValue = (Enabled) ? InitialValue : 0.5;
			if( FCurrentValue != ATargetValue )
			{
				float Slope = SlopeRun;
				if( FCurrentValue > ATargetValue )
				{
					if( ATargetValue > 0.5 )
						Slope = SlopeStop;
				}

				else
				{
					if( ATargetValue < 0.5 )
						Slope = SlopeStop;
				}

				if( Slope == 0 )
					FCurrentValue = ATargetValue;

				else
				{
					float ARamp = abs( ( currentMicros - FLastTime ) * Slope / 1000000 );
					if( FCurrentValue < ATargetValue )
					{
						FCurrentValue += ARamp;
						if( FCurrentValue > ATargetValue )
							FCurrentValue = ATargetValue;

					}
					else
					{
						FCurrentValue -= ARamp;
						if( FCurrentValue < ATargetValue )
							FCurrentValue = ATargetValue;

					}
				}

				UpdateOutputs();
			}

			FLastTime = currentMicros;
		}

	public:
		MotorShieldChannel_Slopped( bool AIsSecond ) :
			Enabled( true ),
			FIsSecond( AIsSecond )
		{
			if( AIsSecond )
			{
				pinMode( PIN_FORWARD_2, OUTPUT );
				pinMode( PIN_REVERSE_2, OUTPUT );
				pinMode( PIN_ENABLE_2, OUTPUT );
			}
			else
			{
				pinMode( PIN_FORWARD_1, OUTPUT );
				pinMode( PIN_REVERSE_1, OUTPUT );
				pinMode( PIN_ENABLE_1, OUTPUT );
			}
		}
	};
//---------------------------------------------------------------------------
	template<int PIN_ENABLE_1, int PIN_FORWARD_1, int PIN_REVERSE_1, int PIN_ENABLE_2, int PIN_FORWARD_2, int PIN_REVERSE_2> class MotorShieldChannel_Slopped_Enable : public MotorShieldChannel_Slopped<PIN_ENABLE_1, PIN_FORWARD_1, PIN_REVERSE_1, PIN_ENABLE_2, PIN_FORWARD_2, PIN_REVERSE_2> {};
//---------------------------------------------------------------------------
}

#ifdef __TEST_FOR_DEBUG_PRINTS__
#undef Serial
#endif

#endif
