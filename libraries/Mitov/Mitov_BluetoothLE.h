////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2018 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_BLUETOOTH_LE_h
#define _MITOV_BLUETOOTH_LE_h

#include <Mitov.h>

#ifdef VISUINO_ARDUINO_101
  #include <CurieBLE.h>
#else // VISUINO_ARDUINO_101
  #include <BLEPeripheral.h>
#endif // VISUINO_ARDUINO_101

namespace Mitov
{
//---------------------------------------------------------------------------
	enum TArduinoBluetoothSensorLocation { slNone, slOther, slChest, slWrist, slFinger, slHand, slEarLobe, slFoot };
//---------------------------------------------------------------------------
	class ArduinoBlutoothUpdateIntf
	{
	public:
		virtual void UpdateValues() = 0;
	};
//---------------------------------------------------------------------------
	class ArduinoBluetoothLE
	{
	public:
		OpenWire::SourcePin ConnectedOutputPin;
		OpenWire::SourcePin MACAddressOutputPin;

	public:
		Mitov::String	LocalName;
		Mitov::String	DeviceName;

	public:
		BLEPeripheral	FPeripheral;

	public:
		void RegisterUpdateElement( ArduinoBlutoothUpdateIntf *AElement )
		{
			FElements.push_back( AElement );
		}

	protected:
		Mitov::SimpleList<ArduinoBlutoothUpdateIntf *>	FElements;
		bool	FConnected = false;

	public:
		inline void SystemLoopBegin( unsigned long currentMicros )
		{
			BLECentral ACentral = FPeripheral.central();
			if( FConnected )
			{
				if( !ACentral )
				{
					FConnected = false;
					ConnectedOutputPin.Notify( &FConnected );
				}
			}
			else
			{
				if( ACentral )
				{
					FConnected = true;
					ConnectedOutputPin.Notify( &FConnected );
					Mitov::String AAddress = ACentral.address();
					MACAddressOutputPin.SendValue( AAddress );
				}
			}

			if( FConnected )
			{
				for( int i = 0; i < FElements.size(); ++ i )
					FElements[ i ]->UpdateValues();
			}
		}

		inline void SystemInit()
		{
			if( LocalName != "" )
				FPeripheral.setLocalName( LocalName.c_str() );

			if( DeviceName != "" )
				FPeripheral.setDeviceName( DeviceName.c_str() );
		}

		inline void SystemStart()
		{
			FPeripheral.begin();
		}
	};
//---------------------------------------------------------------------------
	class ArduinoBluetoothLEService
	{
	public:
		Mitov::String	UUID;
		bool	Advertise = false;

	public:
		ArduinoBluetoothLE &FOwner;

	protected:
		BLEService	*FService;

	public:
		inline void SystemInit()
		{
			FService = new BLEService( UUID.c_str() );
			FOwner.FPeripheral.addAttribute( *FService );
			if( Advertise )
				FOwner.FPeripheral.setAdvertisedServiceUuid( FService->uuid());
		}

	public:
		ArduinoBluetoothLEService( ArduinoBluetoothLE &AOwner ) :
			FOwner( AOwner )
		{
		}
	};
//---------------------------------------------------------------------------
	template<typename T_CHARACT> class ArduinoBluetoothLEBasicCharacteristic
	{
	public:
		Mitov::String	UUID;

	protected:
		ArduinoBluetoothLEService &FOwner;
		T_CHARACT	*FCharacteristic;

	public:
		inline void SystemInit() {} // Placeholder for inherited classes

	public:
		ArduinoBluetoothLEBasicCharacteristic( ArduinoBluetoothLEService &AOwner ) :
			FOwner( AOwner )
		{
		}
	};
//---------------------------------------------------------------------------
	template<typename T, typename T_CHARACT> class ArduinoBluetoothLETypedBasicWriteCharacteristic : public ArduinoBluetoothLEBasicCharacteristic<T_CHARACT>, public ArduinoBlutoothUpdateIntf
	{
		typedef ArduinoBluetoothLEBasicCharacteristic<T_CHARACT> inherited;

	public:
		OpenWire::TypedSourcePin<T>	OutputPin;

	public:
		T		InitialValue;
		bool	BigEndian = false;

	public:
		virtual void UpdateValues()
		{
			if( inherited::FCharacteristic->written() )
				OutputPin.SetValue( inherited::FCharacteristic->value(), true );

		}

	public:
		ArduinoBluetoothLETypedBasicWriteCharacteristic( ArduinoBluetoothLEService &AOwner ) :
			inherited( AOwner )
		{
			AOwner.FOwner.RegisterUpdateElement( this );
		}

	};
//---------------------------------------------------------------------------
	template<typename T, typename T_CHARACT> class ArduinoBluetoothLETypedWriteCharacteristic : public ArduinoBluetoothLETypedBasicWriteCharacteristic<T, T_CHARACT>
	{
		typedef ArduinoBluetoothLETypedBasicWriteCharacteristic<T, T_CHARACT> inherited;

	public:
		inline void SystemInit()
		{
			inherited::SystemInit();
			inherited::FCharacteristic = new T_CHARACT( inherited::UUID.c_str(), BLEWrite );
			inherited::FOwner.FOwner.FPeripheral.addAttribute( *inherited::FCharacteristic );
			if( inherited::BigEndian )
				inherited::FCharacteristic->setValueBE( inherited::InitialValue );

			else
				inherited::FCharacteristic->setValueLE( inherited::InitialValue );

			inherited::OutputPin.SetValue( inherited::InitialValue, false );
		}

	public:
		using inherited::inherited;

	};
//---------------------------------------------------------------------------
	template<typename T, typename T_CHARACT> class ArduinoBluetoothLETypedReadCharacteristic : public ArduinoBluetoothLEBasicCharacteristic<T_CHARACT>
	{
		typedef ArduinoBluetoothLEBasicCharacteristic<T_CHARACT> inherited;

	public:
		T		InitialValue;
		bool	Notify : 1;
		bool	BigEndian : 1;

	public:
		inline void SystemInit()
		{
			inherited::SystemInit();
			inherited::FCharacteristic = new T_CHARACT( inherited::UUID.c_str(), Notify ? ( BLERead | BLENotify ) : BLERead );
			inherited::FOwner.FOwner.FPeripheral.addAttribute( *inherited::FCharacteristic );
			if( BigEndian )
				inherited::FCharacteristic->setValueBE( InitialValue );

			else
				inherited::FCharacteristic->setValueLE( InitialValue );

		}

	public:
		void InputPin_o_Receive( void *_Data )
		{
			T AValue = *(T*)_Data;
			if( AValue != inherited::FCharacteristic->value() )
			{
				if( BigEndian )
					inherited::FCharacteristic->setValueBE( AValue );

				else
					inherited::FCharacteristic->setValueLE( AValue );
			}
		}

	public:
		ArduinoBluetoothLETypedReadCharacteristic( ArduinoBluetoothLEService &AOwner ) :
			inherited( AOwner ),
			Notify( false ),
			BigEndian( false )
		{
		}

	};
//---------------------------------------------------------------------------
	template<typename T, typename T_CHARACT> class ArduinoBluetoothLETypedReadWriteCharacteristic : public ArduinoBluetoothLETypedBasicWriteCharacteristic<T, T_CHARACT>
	{
		typedef ArduinoBluetoothLETypedBasicWriteCharacteristic<T, T_CHARACT> inherited;

	protected:
		bool	Notify = false;

	public:
		void InputPin_o_Receive( void *_Data )
		{
			T AValue = *(T*)_Data;
			if( AValue != inherited::FCharacteristic->value() )
			{
				if( inherited::BigEndian )
					inherited::FCharacteristic->setValueBE( AValue );

				else
					inherited::FCharacteristic->setValueLE( AValue );
			}
		}

	public:
		inline void SystemInit()
		{
			inherited::SystemInit();
			inherited::FCharacteristic = new T_CHARACT( inherited::UUID.c_str(), Notify ? ( BLERead | BLENotify | BLEWrite ) : BLERead | BLEWrite );
			inherited::FOwner.FOwner.FPeripheral.addAttribute( *inherited::FCharacteristic );
			if( inherited::BigEndian )
				inherited::FCharacteristic->setValueBE( inherited::InitialValue );

			else
				inherited::FCharacteristic->setValueLE( inherited::InitialValue );

			inherited::OutputPin.SetValue( inherited::InitialValue, false );
		}

	public:
		using inherited::inherited;

	};
//---------------------------------------------------------------------------
	class ArduinoBluetoothLEBinaryBasicWriteCharacteristic : public ArduinoBluetoothLEBasicCharacteristic<BLECharacteristic>, public ArduinoBlutoothUpdateIntf
	{
		typedef ArduinoBluetoothLEBasicCharacteristic<BLECharacteristic> inherited;

	public:
		OpenWire::SourcePin	OutputPin;

	public:
		Mitov::Bytes	InitialValue;

	public:
		virtual void UpdateValues()
		{
			if( inherited::FCharacteristic->written() )
				OutputPin.SendValue( Mitov::TDataBlock( inherited::FCharacteristic->valueLength(), inherited::FCharacteristic->value() ) );

		}

	public:
		ArduinoBluetoothLEBinaryBasicWriteCharacteristic( ArduinoBluetoothLEService &AOwner ) :
			inherited( AOwner )
		{
			AOwner.FOwner.RegisterUpdateElement( this );
		}
	};
//---------------------------------------------------------------------------
	template<int C_MAX_SIZE> class ArduinoBluetoothLEBinaryWriteCharacteristic : public ArduinoBluetoothLEBinaryBasicWriteCharacteristic
	{
		typedef ArduinoBluetoothLEBinaryBasicWriteCharacteristic inherited;

	public:
		inline void SystemInit()
		{
			inherited::SystemInit();
			inherited::FCharacteristic = new BLECharacteristic( inherited::UUID.c_str(), BLEWrite, C_MAX_SIZE );
			inherited::FOwner.FOwner.FPeripheral.addAttribute( *inherited::FCharacteristic );
			inherited::FCharacteristic->setValue( inherited::InitialValue._Bytes, inherited::InitialValue._BytesSize );
//			inherited::OutputPin.SendValue( Mitov::TDataBlock( inherited::InitialValue._BytesSize, inherited::InitialValue._Bytes ) );
		}

	public:
		using inherited::inherited;

	};
//---------------------------------------------------------------------------
	template<int C_MAX_SIZE> class ArduinoBluetoothLEBinaryReadCharacteristic : public ArduinoBluetoothLEBasicCharacteristic<BLECharacteristic>
	{
		typedef ArduinoBluetoothLEBasicCharacteristic<BLECharacteristic> inherited;

	public:
		Mitov::Bytes	InitialValue;
		bool	Notify = false;

	public:
		inline void SystemInit()
		{
			inherited::SystemInit();
			inherited::FCharacteristic = new BLECharacteristic( inherited::UUID.c_str(), Notify ? ( BLERead | BLENotify ) : BLERead, C_MAX_SIZE );
			inherited::FOwner.FOwner.FPeripheral.addAttribute( *inherited::FCharacteristic );
			inherited::FCharacteristic->setValue( InitialValue._Bytes, InitialValue._BytesSize );
		}

	public:
		void InputPin_o_Receive( void *_Data )
		{
			Mitov::TDataBlock ABlock = *(Mitov::TDataBlock *)_Data;
			if( ABlock.Size == inherited::FCharacteristic->valueLength() )
				if( memcmp( ABlock.Data, inherited::FCharacteristic->value(), ABlock.Size ) == 0 )
					return;

			inherited::FCharacteristic->setValue( ABlock.Data, ABlock.Size );
		}

	public:
		using inherited::inherited;

	};
//---------------------------------------------------------------------------
	template<int C_MAX_SIZE> class ArduinoBluetoothLEBinaryReadWriteCharacteristic : public ArduinoBluetoothLEBinaryBasicWriteCharacteristic
	{
		typedef ArduinoBluetoothLEBinaryBasicWriteCharacteristic inherited;

	protected:
		bool	Notify = false;

	public:
		void InputPin_o_Receive( void *_Data )
		{
			Mitov::TDataBlock ABlock = *(Mitov::TDataBlock *)_Data;
			if( ABlock.Size == inherited::FCharacteristic->valueLength() )
				if( memcmp( ABlock.Data, inherited::FCharacteristic->value(), ABlock.Size ) == 0 )
					return;

			inherited::FCharacteristic->setValue( ABlock.Data, ABlock.Size );
		}

	public:
		inline void SystemInit()
		{
			inherited::SystemInit();
			inherited::FCharacteristic = new BLECharacteristic( inherited::UUID.c_str(), Notify ? ( BLERead | BLENotify | BLEWrite ) : BLERead | BLEWrite, C_MAX_SIZE );
			inherited::FOwner.FOwner.FPeripheral.addAttribute( *inherited::FCharacteristic );
			inherited::FCharacteristic->setValue( inherited::InitialValue._Bytes, inherited::InitialValue._BytesSize );
//			inherited::OutputPin.SetValue( inherited::InitialValue, false );
		}

	public:
		using inherited::inherited;

	};
//---------------------------------------------------------------------------
	class ArduinoBluetoothLETextBasicWriteCharacteristic : public ArduinoBluetoothLEBasicCharacteristic<BLECharacteristic>, public ArduinoBlutoothUpdateIntf
	{
		typedef ArduinoBluetoothLEBasicCharacteristic<BLECharacteristic> inherited;

	public:
		OpenWire::SourcePin	OutputPin;

	public:
		Mitov::String	InitialValue;

	public:
		virtual void UpdateValues()
		{
			if( inherited::FCharacteristic->written() )
			{
				OutputPin.SendValue<Mitov::String>( StringEx( (const char*)inherited::FCharacteristic->value(), inherited::FCharacteristic->valueLength() ) );
			}
		}

	public:
		ArduinoBluetoothLETextBasicWriteCharacteristic( ArduinoBluetoothLEService &AOwner ) :
			inherited( AOwner )
		{
			AOwner.FOwner.RegisterUpdateElement( this );
		}
	};
//---------------------------------------------------------------------------
	template<int C_MAX_SIZE> class ArduinoBluetoothLETextWriteCharacteristic : public ArduinoBluetoothLETextBasicWriteCharacteristic
	{
		typedef ArduinoBluetoothLETextBasicWriteCharacteristic inherited;

	public:
		inline void SystemInit()
		{
			inherited::SystemInit();
			inherited::FCharacteristic = new BLECharacteristic( inherited::UUID.c_str(), BLEWrite, C_MAX_SIZE );
			inherited::FOwner.FOwner.FPeripheral.addAttribute( *inherited::FCharacteristic );
			inherited::FCharacteristic->setValue( (unsigned char *)inherited::InitialValue.c_str(), inherited::InitialValue.length() );
			inherited::OutputPin.SendValue<Mitov::String>( StringEx( (const char*)inherited::FCharacteristic->value(), inherited::FCharacteristic->valueLength() ) );
		}

	public:
		using inherited::inherited;

	};
//---------------------------------------------------------------------------
	template<int C_MAX_SIZE> class ArduinoBluetoothLETextReadCharacteristic : public ArduinoBluetoothLEBasicCharacteristic<BLECharacteristic>
	{
		typedef ArduinoBluetoothLEBasicCharacteristic<BLECharacteristic> inherited;

	public:
		Mitov::String	InitialValue;
		bool	Notify = false;

	public:
		inline void SystemInit()
		{
			inherited::SystemInit();
			inherited::FCharacteristic = new BLECharacteristic( inherited::UUID.c_str(), Notify ? ( BLERead | BLENotify ) : BLERead, C_MAX_SIZE );
			inherited::FOwner.FOwner.FPeripheral.addAttribute( *inherited::FCharacteristic );
			inherited::FCharacteristic->setValue( (unsigned char *)InitialValue.c_str(), InitialValue.length() );
		}

	public:
		void InputPin_o_Receive( void *_Data )
		{
			Mitov::String AData = (char *)_Data;
			if( AData.length() == inherited::FCharacteristic->valueLength() )
				if( memcmp( AData.c_str(), inherited::FCharacteristic->value(), AData.length() ) == 0 )
					return;

			inherited::FCharacteristic->setValue( (unsigned char *)AData.c_str(), AData.length() );
		}

	public:
		using inherited::inherited;

	};
//---------------------------------------------------------------------------
	template<int C_MAX_SIZE> class ArduinoBluetoothLETextReadWriteCharacteristic : public ArduinoBluetoothLETextBasicWriteCharacteristic
	{
		typedef ArduinoBluetoothLETextBasicWriteCharacteristic inherited;

	protected:
		bool	Notify = false;

	public:
		void InputPin_o_Receive( void *_Data )
		{
			Mitov::String AData = (char *)_Data;
			if( AData.length() == inherited::FCharacteristic->valueLength() )
				if( memcmp( AData.c_str(), inherited::FCharacteristic->value(), AData.length() ) == 0 )
					return;

			inherited::FCharacteristic->setValue( (unsigned char *)AData.c_str(), AData.length() );
		}

	public:
		inline void SystemInit()
		{
			inherited::SystemInit();
			inherited::FCharacteristic = new BLECharacteristic( inherited::UUID.c_str(), Notify ? ( BLERead | BLENotify | BLEWrite ) : BLERead | BLEWrite, C_MAX_SIZE );
			inherited::FOwner.FOwner.FPeripheral.addAttribute( *inherited::FCharacteristic );
			inherited::FCharacteristic->setValue( (unsigned char *)inherited::InitialValue.c_str(), inherited::InitialValue.length() );
			inherited::OutputPin.SendValue<Mitov::String>( StringEx( (const char*)inherited::FCharacteristic->value(), inherited::FCharacteristic->valueLength() ) );
		}

	public:
		using inherited::inherited;

	};
//---------------------------------------------------------------------------
}

#endif
