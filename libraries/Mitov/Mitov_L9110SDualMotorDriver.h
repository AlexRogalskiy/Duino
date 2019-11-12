////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2018 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_L9110S_DUAL_MOTOR_h
#define _MITOV_L9110S_DUAL_MOTOR_h

#include <Mitov.h>

namespace Mitov
{
//---------------------------------------------------------------------------
	template<typename T_DIRECTION_IMPLEMENTATION, typename T_SPEED_IMPLEMENTATION> class L9110SDualMotorDriverChannel : public T_DIRECTION_IMPLEMENTATION, public T_SPEED_IMPLEMENTATION
	{
	public:
		inline OpenWire::SourcePin &DirectionOutputPin()
		{
			return T_DIRECTION_IMPLEMENTATION::GetPinInstance();
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

			if( ADirection )
				AOutSpeed = 1 - AOutSpeed;

//			SpeedOutputPin.Notify( &AOutSpeed );
//			DirectionOutputPin.Notify( &ADirection );

			T_SPEED_IMPLEMENTATION::SetPinValue( AOutSpeed );
			T_DIRECTION_IMPLEMENTATION::SetPinValue( ADirection );
		}

	public:
		inline void SystemStart()
		{
			UpdateOutputs();
		}

		inline void SystemLoopBegin( unsigned long currentMicros ) {} // Placeholder for L9110SDualMotorDriverChannel_Slopped compatibility
	};
//---------------------------------------------------------------------------
	template<typename T_DIRECTION_IMPLEMENTATION, typename T_SPEED_IMPLEMENTATION> class L9110SDualMotorDriverChannel_Enable : public T_DIRECTION_IMPLEMENTATION, public T_SPEED_IMPLEMENTATION
	{
	public:
		inline OpenWire::SourcePin &DirectionOutputPin()
		{
			return T_DIRECTION_IMPLEMENTATION::GetPinInstance();
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

				if( ADirection )
					AOutSpeed = 1 - AOutSpeed;

//				SpeedOutputPin.Notify( &AOutSpeed );
//				DirectionOutputPin.Notify( &ADirection );

				T_SPEED_IMPLEMENTATION::SetPinValue( AOutSpeed );
				T_DIRECTION_IMPLEMENTATION::SetPinValue( ADirection );
			}

			else
			{
//				SpeedOutputPin.SendValue( 0.5 );
//				DirectionOutputPin.SendValue( false );

				T_SPEED_IMPLEMENTATION::SetPinValue( 0.5 );
				T_DIRECTION_IMPLEMENTATION::SetPinValue( false );
			}
		}

	public:
		inline void SystemStart()
		{
			UpdateOutputs();
		}

		inline void SystemLoopBegin( unsigned long currentMicros ) {} // Placeholder for L9110SDualMotorDriverChannel_Slopped compatibility
	};
//---------------------------------------------------------------------------
	template<typename T_DIRECTION_IMPLEMENTATION, typename T_SPEED_IMPLEMENTATION> class L9110SDualMotorDriverChannel_Slopped : public T_DIRECTION_IMPLEMENTATION, public T_SPEED_IMPLEMENTATION
	{
	public:
		inline OpenWire::SourcePin &DirectionOutputPin()
		{
			return T_DIRECTION_IMPLEMENTATION::GetPinInstance();
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

				if( ADirection )
					AOutSpeed = 1 - AOutSpeed;

//				SpeedOutputPin.Notify( &AOutSpeed );
//				DirectionOutputPin.Notify( &ADirection );

				T_SPEED_IMPLEMENTATION::SetPinValue( AOutSpeed );
				T_DIRECTION_IMPLEMENTATION::SetPinValue( ADirection );
			}

			else
			{
//				SpeedOutputPin.SendValue( 0.5 );
//				DirectionOutputPin.SendValue( false );

				T_SPEED_IMPLEMENTATION::SetPinValue( 0.5 );
				T_DIRECTION_IMPLEMENTATION::SetPinValue( false );
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
	template<typename T_DIRECTION_IMPLEMENTATION, typename T_SPEED_IMPLEMENTATION> class L9110SDualMotorDriverChannel_Slopped_Enable : public T_DIRECTION_IMPLEMENTATION, public T_SPEED_IMPLEMENTATION
	{
	public:
		inline OpenWire::SourcePin &DirectionOutputPin()
		{
			return T_DIRECTION_IMPLEMENTATION::GetPinInstance();
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

				if( ADirection )
					AOutSpeed = 1 - AOutSpeed;

//				SpeedOutputPin.Notify( &AOutSpeed );
//				DirectionOutputPin.Notify( &ADirection );

				T_SPEED_IMPLEMENTATION::SetPinValue( AOutSpeed );
				T_DIRECTION_IMPLEMENTATION::SetPinValue( ADirection );
			}

			else
			{
//				SpeedOutputPin.SendValue( 0.5 );
//				DirectionOutputPin.SendValue( false );

				T_SPEED_IMPLEMENTATION::SetPinValue( 0.5 );
				T_DIRECTION_IMPLEMENTATION::SetPinValue( false );
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
	template<typename T_FORWARD_IMPLEMENTATION, typename T_BACK_IMPLEMENTATION> class L298NDualMotorDriverPWMChannel : public T_FORWARD_IMPLEMENTATION, public T_BACK_IMPLEMENTATION
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
		void	UpdateOutputs()
		{
			float AOutSpeed = abs( InitialValue - 0.5 ) * 2;
			if( InitialValue > 0.5 )
			{
				T_BACK_IMPLEMENTATION::SetPinValue( 0 );
				T_FORWARD_IMPLEMENTATION::SetPinValue( AOutSpeed );
			}
			else
			{
				T_FORWARD_IMPLEMENTATION::SetPinValue( 0 );
				T_BACK_IMPLEMENTATION::SetPinValue( AOutSpeed );
			}
		}

	public:
		inline void SystemStart()
		{
			UpdateOutputs();
		}

		inline void SystemLoopBegin( unsigned long currentMicros ) {} // Placeholder for L298NDualMotorDriverPWMChannel_Slopped compatibility

	};
//---------------------------------------------------------------------------
	template<typename T_FORWARD_IMPLEMENTATION, typename T_BACK_IMPLEMENTATION> class L298NDualMotorDriverPWMChannel_Enable : public T_FORWARD_IMPLEMENTATION, public T_BACK_IMPLEMENTATION
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
				if( InitialValue > 0.5 )
				{
					T_BACK_IMPLEMENTATION::SetPinValue( 0 );
					T_FORWARD_IMPLEMENTATION::SetPinValue( AOutSpeed );
				}
				else
				{
					T_FORWARD_IMPLEMENTATION::SetPinValue( 0 );
					T_BACK_IMPLEMENTATION::SetPinValue( AOutSpeed );
				}
			}

			else
			{
				T_FORWARD_IMPLEMENTATION::SetPinValue( 0 );
				T_BACK_IMPLEMENTATION::SetPinValue( 0 );
			}
		}

	public:
		inline void SystemStart()
		{
			UpdateOutputs();
		}

		inline void SystemLoopBegin( unsigned long currentMicros ) {} // Placeholder for L298NDualMotorDriverPWMChannel_Slopped compatibility

	};
//---------------------------------------------------------------------------
	template<typename T_FORWARD_IMPLEMENTATION, typename T_BACK_IMPLEMENTATION> class L298NDualMotorDriverPWMChannel_Slopped : public T_FORWARD_IMPLEMENTATION, public T_BACK_IMPLEMENTATION
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
				if( FCurrentValue > 0.5 )
				{
					T_BACK_IMPLEMENTATION::SetPinValue( 0 );
					T_FORWARD_IMPLEMENTATION::SetPinValue( AOutSpeed );
				}
				else
				{
					T_FORWARD_IMPLEMENTATION::SetPinValue( 0 );
					T_BACK_IMPLEMENTATION::SetPinValue( AOutSpeed );
				}
			}

			else
			{
				T_FORWARD_IMPLEMENTATION::SetPinValue( 0 );
				T_BACK_IMPLEMENTATION::SetPinValue( 0 );
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
	template<typename T_FORWARD_IMPLEMENTATION, typename T_BACK_IMPLEMENTATION> class L298NDualMotorDriverPWMChannel_Slopped_Enable : public T_FORWARD_IMPLEMENTATION, public T_BACK_IMPLEMENTATION
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
				if( FCurrentValue > 0.5 )
				{
					T_BACK_IMPLEMENTATION::SetPinValue( 0 );
					T_FORWARD_IMPLEMENTATION::SetPinValue( AOutSpeed );
				}
				else
				{
					T_FORWARD_IMPLEMENTATION::SetPinValue( 0 );
					T_BACK_IMPLEMENTATION::SetPinValue( AOutSpeed );
				}
			}

			else
			{
				T_FORWARD_IMPLEMENTATION::SetPinValue( 0 );
				T_BACK_IMPLEMENTATION::SetPinValue( 0 );
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
}

#endif
