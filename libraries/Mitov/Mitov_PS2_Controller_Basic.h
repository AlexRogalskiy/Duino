////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2018 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_PS2_CONTROLLER_BASIC_h
#define _MITOV_PS2_CONTROLLER_BASIC_h

#include <Mitov.h>

namespace Mitov
{
	template<typename T_IMPLEMENTATION> class PS2BasicController : public T_IMPLEMENTATION
	{
	public:
		bool	Enabled = true;

	public:
		bool	ReadDigital( unsigned int AIndex )
		{
			return T_IMPLEMENTATION::ReadDigital( AIndex );
		}

		float	ReadAnalog( unsigned int AIndex )
		{
			return T_IMPLEMENTATION::ReadAnalog( AIndex );
		}

	public:
		class BaseSensor : public OpenWire::SourcePin
		{
		protected:
			uint8_t		FIndex;

		public:
			virtual	void	Process( PS2BasicController *AOwner ) = 0;
			virtual	void	StartProcess( PS2BasicController *AOwner ) = 0;

		public:
			BaseSensor( unsigned int AIndex ) :
				FIndex( AIndex )
			{
			}

		};

		class DigitalSensor : public BaseSensor
		{
			typedef BaseSensor inherited;

		protected:
			bool	FValue;

		public:
			virtual	void Process( PS2BasicController *AOwner ) override
			{
				bool AValue = AOwner->ReadDigital( inherited::FIndex );

				if( FValue == AValue )
					return;

				FValue = AValue;
				inherited::Notify( &FValue );
			}

			virtual	void StartProcess( PS2BasicController *AOwner ) override
			{
				bool AValue = AOwner->ReadDigital( inherited::FIndex );
				FValue = AValue;
				inherited::Notify( &FValue );
			}

		public:
			using inherited::inherited;

		};

		class AnalogSensor : public BaseSensor
		{
			typedef BaseSensor inherited;

		protected:
			float	FValue;

		public:
			virtual	void	Process( PS2BasicController *AOwner ) override
			{
				float	AValue = AOwner->ReadAnalog( inherited::FIndex );

				if( FValue == AValue )
					return;

				FValue = AValue;
				inherited::Notify( &FValue );				
			}

			virtual	void StartProcess( PS2BasicController *AOwner ) override
			{
				float	AValue = AOwner->ReadAnalog( inherited::FIndex );

				FValue = AValue;
				inherited::Notify( &FValue );				
			}

		public:
			using inherited::inherited;

		};

	public:
		Mitov::SimpleList<Mitov::PS2BasicController<T_IMPLEMENTATION>::BaseSensor *> Sensors;

	public:
		inline void SystemLoopBegin( unsigned long currentMicros )
		{
			ReadData();
		}

		inline void SystemStart()
		{
			ReadController();
			for( int i = 0; i < Sensors.size(); ++i )
				Sensors[ i ]->StartProcess( this );

		}

	public:
		inline void ClockInputPin_o_Receive( void *_Data )
		{
			ReadData();
		}

	protected:
		void ReadData()
		{
			if( ! Enabled )
				return;

			ReadController();

			for( int i = 0; i < Sensors.size(); ++i )
				Sensors[ i ]->Process( this );
		}

		void ReadController()
		{
			T_IMPLEMENTATION::ReadController();
		}

	public:
		using T_IMPLEMENTATION::T_IMPLEMENTATION;

/*
		virtual ~PS2BasicController()
		{
			for( int i = 0; i < Sensors.size(); ++i )
				delete Sensors[ i ];
		}
*/

	};
//---------------------------------------------------------------------------
}

#endif
