////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2018 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_h
#define _MITOV_h

#include <OpenWire.h>

#define _VISUINO_MAX_PIN_NO_ 0xFFFF

#ifdef _MITOV_SOFTWARE_SERIAL_SENDING_PIN_
	#define _MITOV_SERIAL_SENDING_PIN_
#endif

#ifdef FSPI
  #undef FSPI
#endif

#if defined(__AVR_ATmega168__) ||defined(__AVR_ATmega168P__) ||defined(__AVR_ATmega328P__)
  #define Mitov_ATmega_168_168P_328P
#endif

#ifdef VISUINO_ESP32
	typedef const char *MITOV_FILE_MODE;
	#define __VISUINO_USE_NEO_PIXELS_BUS_LIBRARY__
#else
	typedef uint8_t MITOV_FILE_MODE;
#endif


#ifndef INPUT_PULLDOWN
	#ifndef VISUINO_ESP32
		#define INPUT_PULLDOWN INPUT
	#endif
#endif

#if defined( VISUINO_CHIPKIT_MAX32 ) || defined( VISUINO_CHIPKIT_UC32 ) || defined( VISUINO_CHIPKIT_DP32 ) || defined( VISUINO_CHIPKIT_LENNY )
	#define VISUINO_CHIPKIT

#endif

#if defined( VISUINO_FEATHER_M0_BASIC ) || defined( VISUINO_FEATHER_M0_RFM9X ) || defined( VISUINO_FEATHER_M0_RFM69 ) || defined( VISUINO_TRINKET_M0 )
	#define VISUINO_M0
	#define VISUINO_MEMORY_ALLIGN
	#define VISUINO_FEATHER_M0
#endif

#if defined( VISUINO_WEMOS_D1_R1 ) || defined( VISUINO_WEMOS_D1_R2 ) || defined( VISUINO_WEMOS_D1_MINI ) || defined( VISUINO_FEATHER_HUZZAH_ESP8266 )
	#define VISUINO_ESP8266
#endif

#ifdef VISUINO_ESP8266
	#define VISUINO_MEMORY_ALLIGN
#endif

#if defined( VISUINO_STMDUINO )

#if defined( _WIRISH_WPROGRAM_H_ ) // For the olr Mapple compatibility
  #define PIN_MODE WiringPinMode
#endif

  #ifndef OUTPUT_OPEN_DRAIN
	#define OUTPUT_OPEN_DRAIN	OUTPUT
  #endif // OUTPUT_OPEN_DRAIN

#else
  #ifndef OUTPUT_OPEN_DRAIN
    #define OUTPUT_OPEN_DRAIN	OUTPUT
  #endif // OUTPUT_OPEN_DRAIN

#endif

#ifndef PWMRANGE
  #ifdef VISUINO_STMDUINO
    #define PWMRANGE 0xFFFF
 #else // VISUINO_STMDUINO
    #define PWMRANGE 0xFF
  #endif // VISUINO_STMDUINO

#endif // PWMRANGE

//#ifdef VISUINO_ESP32
//	inline void analogWrite( uint8_t APin, uint16_t AValue ) {}
//#endif

#ifdef VISUINO_ESP32
	#define VISUINO_ANALOG_IN_RANGE 4096
	#define VISUINO_ANALOG_IN_THRESHOLD 2000
#else
	#define VISUINO_ANALOG_IN_RANGE 1023
	#define VISUINO_ANALOG_IN_THRESHOLD 409
#endif

#ifdef VISUINO_ESP32
  #define PWMRANGE 255

namespace Mitov
{
	HardwareSerial Serial1(1);
	HardwareSerial Serial2(2);
}

namespace Mitov
{
	uint8_t	CESP32PWMTimers[ 2 ] = {0};
	uint8_t	CESP32PWMPinMap[ 40 ] = {0};

	void ServoAttach( uint8_t APin )
	{
		if( Mitov::CESP32PWMPinMap[ APin ] == 0 )
		{
			uint8_t AChannel = 0;
			for( AChannel = 0; AChannel < 16; ++AChannel )
				if( ! ( Mitov::CESP32PWMTimers[ AChannel >> 3 ] & ( 1 << ( AChannel & 0b111 ) ) ))
					break;

			if( AChannel == 16 )
				return; // Can't allocate channel

			Mitov::CESP32PWMPinMap[ APin ] = AChannel + 1;
			Mitov::CESP32PWMTimers[ AChannel >> 3 ] |= ( 1 << ( AChannel & 0b111 ) );
			ledcSetup( AChannel, 50, 16 ); // 50Hz 16 bit
//			ledcWrite( AChannel, AValue );
			ledcAttachPin( APin, AChannel );
		}
	}

	void ServoDetach( uint8_t APin )
	{
		if( Mitov::CESP32PWMPinMap[ APin ] != 0 )
		{
			ledcDetachPin( APin );
			uint8_t AChannel = Mitov::CESP32PWMPinMap[ APin ] - 1;
			Mitov::CESP32PWMTimers[ AChannel >> 3 ] &= ~ ( 1 << ( AChannel & 0b111 ) );
			Mitov::CESP32PWMPinMap[ APin ] = 0;
		}
	}

	void ServoWrite( uint8_t APin, float AValue, uint16_t APulseMin, uint16_t APulseMax )
	{
		if( Mitov::CESP32PWMPinMap[ APin ] != 0 )
		{
			APulseMin *= 0xFFFF / 20000;
			APulseMax *= 0xFFFF / 20000;
			APulseMin += 512; //255;
			APulseMax += 512; //255;
			uint16_t AIntValue = ( AValue / 180 ) * ( APulseMax - APulseMin ) + 0.5;
			AIntValue += APulseMin;
			ledcWrite( Mitov::CESP32PWMPinMap[ APin ] - 1, AIntValue );
		}
	}

	void ToneAttach( uint8_t APin )
	{
		if( Mitov::CESP32PWMPinMap[ APin ] == 0 )
		{
			uint8_t AChannel = 0;
			for( AChannel = 0; AChannel < 16; ++AChannel )
				if( ! ( Mitov::CESP32PWMTimers[ AChannel >> 3 ] & ( 1 << ( AChannel & 0b111 ) ) ))
					break;

			if( AChannel == 16 )
				return; // Can't allocate channel

			Mitov::CESP32PWMPinMap[ APin ] = AChannel + 1;
			Mitov::CESP32PWMTimers[ AChannel >> 3 ] |= ( 1 << ( AChannel & 0b111 ) );
			ledcSetup( AChannel, 50, 16 ); // 50Hz 16 bit
//			ledcWrite( AChannel, AValue );
			ledcAttachPin( APin, AChannel );
		}
	}

	void ToneDetach( uint8_t APin )
	{
		if( Mitov::CESP32PWMPinMap[ APin ] != 0 )
		{
			ledcDetachPin( APin );
			uint8_t AChannel = Mitov::CESP32PWMPinMap[ APin ] - 1;
			Mitov::CESP32PWMTimers[ AChannel >> 3 ] &= ~ ( 1 << ( AChannel & 0b111 ) );
			Mitov::CESP32PWMPinMap[ APin ] = 0;
		}
	}

	void PlayTone( uint8_t APin, double freq )
	{
		if( Mitov::CESP32PWMPinMap[ APin ] != 0 )
		{
			ledcWriteTone( Mitov::CESP32PWMPinMap[ APin ] - 1, freq );
		}
	}
}
	void analogWrite( uint8_t APin, uint16_t AValue )
	{
//		Serial.println( "analogWrite" );
//		Serial.println( AValue );
		if( AValue == 0 || AValue == 255 )
		{
			if( Mitov::CESP32PWMPinMap[ APin ] != 0 )
			{
				ledcWrite( Mitov::CESP32PWMPinMap[ APin ] - 1, AValue );
//				Serial.println( "ledcDetachPin" );
				digitalWrite( APin, (AValue == 255 ) ? HIGH : LOW );
				ledcDetachPin( APin );
				uint8_t AChannel = Mitov::CESP32PWMPinMap[ APin ] - 1;
				Mitov::CESP32PWMTimers[ AChannel >> 3 ] &= ~ ( 1 << ( AChannel & 0b111 ) );
				Mitov::CESP32PWMPinMap[ APin ] = 0;
			}
			
			digitalWrite( APin, (AValue == 255 ) ? HIGH : LOW );
			return;
		}

		if( Mitov::CESP32PWMPinMap[ APin ] == 0 )
		{
			uint8_t AChannel = 0;
			for( AChannel = 0; AChannel < 16; ++AChannel )
				if( ! ( Mitov::CESP32PWMTimers[ AChannel >> 3 ] & ( 1 << ( AChannel & 0b111 ) ) ))
					break;

			if( AChannel == 16 )
				return; // Can't allocate channel

			Mitov::CESP32PWMPinMap[ APin ] = AChannel + 1;
			Mitov::CESP32PWMTimers[ AChannel >> 3 ] |= ( 1 << ( AChannel & 0b111 ) );
			ledcSetup( AChannel, 1000, 8 ); // 1KHz 8 bit
			ledcWrite( AChannel, AValue );
			ledcAttachPin( APin, AChannel );
//			Serial.print( "ledcAttachPin: " ); Serial.print( APin ); Serial.print( " " ); Serial.println( AChannel );
			return;
		}

		ledcWrite( Mitov::CESP32PWMPinMap[ APin ] - 1, AValue );
//		Serial.print( "ledcWrite: " ); Serial.print( Mitov::CESP32PWMPinMap[ APin ] - 1 ); Serial.print( " " ); Serial.println( AValue );
	}

#endif

#if defined( VISUINO_FREESOC2 )
  #include <itoa.h>
#endif

#if defined( VISUINO_ARDUINO_DUE ) || defined( VISUINO_NRF52832 ) || defined( VISUINO_BBC_MICRO_BIT ) || defined( VISUINO_FEATHER_M0 )
  #include <avr/dtostrf.h> 
#endif

#if defined( VISUINO_TEENSY_3_0 ) || defined( VISUINO_TEENSY_3_1 ) || defined( VISUINO_TEENSY_LC )
  #define VISUINO_TEENSY
#endif

#if defined( VISUINO_ARDUINO_DUE ) || defined( VISUINO_LINKIT_ONE ) || defined( VISUINO_TEENSY ) || defined( VISUINO_TEENSY_2_0 ) || defined( VISUINO_TEENSY_2_0_PP )
  #define NO_SERIAL_FORMAT
#endif

#if defined( VISUINO_ESP8266 )
  #define V_FMOD(a,b) (a - b * floor(a / b))
  #define sscanf os_sprintf
#else
  #define V_FMOD(a,b) fmod(a,b)
#endif


float posmod( float a, float b )
{
	a = V_FMOD(a,b);
	if( a < 0 )
		a += b;

	return a;
}

#define MAKE_CALLBACK_3_1(T1,T2,A,P) (T2)( void (T1::*) ( P ) )&A
#define MAKE_CALLBACK_3(T1,T2,A,P) (T1 *)this, (T2)( void (T1::*) ( P ) )&A
#define MAKE_CALLBACK_2(T1,T2,A) (T1 *)this, (T2)( void (T1::*) ( void *_Data ) )&A

#define MITOV_ARRAY_SIZE(A) ( sizeof(A) / sizeof(A[0]) )
#define ARRAY_PARAM(A) A,MITOV_ARRAY_SIZE(A)

namespace Mitov
{
/*
	template<typename T> T log2( T log2_input) 
	{
		return log(float( log2_input)) / log(float( 2 ));  
	}
*/
	uint8_t MitovIlog2( uint32_t AValue )
	{
		for( int i = 0; i < 32; ++i )
		{
			if( !( AValue >> i ))
				return i;
		}

		return 32;
	}

	template<typename T> T Constrain16( uint32_t AValue )
	{
		uint32_t AValue1 = MitovMin( AValue, uint32_t( 0xFFFF ));
		return AValue1;
	}

	template<> int16_t Constrain16<int16_t>( uint32_t AValue )
	{
		int32_t AValue1 = constrain( (*(int32_t *)&AValue ), -0x8000l, 0x7FFFl );
		return AValue1;
	}
//---------------------------------------------------------------------------
	enum TAngleUnits { auDegree, auRadians, auNormalized };
//---------------------------------------------------------------------------
	template <typename T> struct TArray
	{
	public:
		uint32_t	Size;
		T *Data;

	public:
		Mitov::String ToString()
		{
//			Serial.println( "ToString" );
			Mitov::String AResult = "(";
			for( int i = 0; i < MitovMin( Size, uint32_t( 32 ) ); ++i )
			{
				if( i < Size - 1 )
					AResult += Mitov::String( Data[ i ] ) + ",";

				else
					AResult += Mitov::String( Data[ i ] );

			}

			if( Size > 32 )
				AResult += "...";

			return	AResult + ")";
		}

	public:
		TArray( uint32_t ASize, const T *AData ) :
			Size( ASize), 
			Data( (T *)AData )
		{
		}

		TArray( const T &AData ) :
			Size( 1 ), 
			Data( (T *)&AData )
		{
		}
	};
//---------------------------------------------------------------------------
	struct TDataBlock : public TArray<byte>
	{
		typedef TArray inherited;

	public:
		TDataBlock( uint32_t ASize, const void *AData ) :
			inherited( ASize, (byte *)AData )
		{
		}

		TDataBlock() :
			inherited( 0, nullptr )
		{
		}

	};
//---------------------------------------------------------------------------
	template <typename T> struct TValueArray : public TArray<T>
	{
		typedef TArray<T> inherited;

	public:
		TValueArray<T>& operator=(const TArray<T>& other)
		{
			if( inherited::Data )
				delete [] inherited::Data;

			inherited::Size = other.Size;
			inherited::Data = new T[ inherited::Size ];

			for( int i = 0; i < inherited::Size; ++ i )
				inherited::Data[ i ] = other.Data[ i ];

			return *this;
		}

	public:
		void Copy( T *AData, uint32_t ASize, uint32_t AOffset )
		{
			for( int i = 0; i < MitovMin( ASize, inherited::Size - AOffset ); ++i )
				inherited::Data[ i + AOffset ] = AData[ i ];

		}

		void ResizeFront( uint32_t ASize )
		{
			if( ASize == inherited::Size )
				return;

			T *AOldData = inherited::Data;
			inherited::Data = new T[ ASize ];

			if( ASize > inherited::Size )
			{
				uint32_t AOffset = ASize - inherited::Size;
				for( int i = 0; i < inherited::Size; ++i )
					inherited::Data[ i + AOffset ] = AOldData[ i ];
			}

			else
			{
				uint32_t AOffset = inherited::Size - ASize;
				for( int i = 0; i < inherited::Size; ++i )
					inherited::Data[ i ] = AOldData[ i + AOffset ];
			}

			delete [] AOldData;

			inherited::Size = ASize;
		}

		void ResizeBack( uint32_t ASize, bool APreserveData )
		{
			if( ASize == inherited::Size )
				return;

			T *AOldData = inherited::Data;
			inherited::Data = new T[ ASize ];

			for( int i = 0; i < MitovMin( ASize, inherited::Size ); ++i )
				inherited::Data[ i ] = AOldData[ i ];

			delete [] AOldData;

			inherited::Size = ASize;
		}

		void Resize( uint32_t ASize )
		{
			if( ASize == inherited::Size )
				return;

			delete [] inherited::Data;
			inherited::Data = new T[ ASize ];

			inherited::Size = ASize;
		}

	public:
		TValueArray( T AData ) :
			inherited( 1, new T[ 1 ] )

		{
			*inherited::Data = AData;
		}

		TValueArray( uint32_t ASize, const T *AData ) :
			inherited( ASize, new T[ ASize ] )
		{
			for( int i = 0; i < ASize; ++ i )
				inherited::Data[ i ] = AData[ i ];
		}

		TValueArray( const TArray<T> &AOther ) :
			TValueArray( AOther.Size, AOther.Data )
		{
		}

		TValueArray() :
			inherited( 0, nullptr )
		{
		}

		~TValueArray()
		{
			if( inherited::Data )
				delete [] inherited::Data;
		}
	};
//---------------------------------------------------------------------------
	#pragma pack(push, 1)
	struct TColor
	{
	public:
		uint8_t Red;
		uint8_t Green;
		uint8_t Blue;
		uint8_t Alpha = 0;

	public:
		void SetValue( const int32_t AValue )
		{
#if defined( VISUINO_MEMORY_ALLIGN )
			Red = ( AValue >> 16 ) & 0xFF;
			Green = ( AValue >> 8 ) & 0xFF;
			Blue = AValue & 0xFF;
#else
			*((int32_t *)this) = AValue & 0xFFFFFF;
#endif

//			Blue = ( AValue >> 16 ) & 0xFF;
//			Green = ( AValue >> 8 ) & 0xFF;
//			Red = AValue & 0xFF;
//			Alpha = 0;
		}

		void operator = ( const long AValue )
		{
			SetValue( AValue );
		}

		bool operator == ( const TColor other )
		{
#if defined( VISUINO_MEMORY_ALLIGN )
			return (( Red == other.Red ) && ( Green == other.Green ) && ( Blue == other.Blue ) && ( Alpha == other.Alpha ));
#else
			return *((long *)this) == *((long *)&other);
#endif
		}

		operator long()
		{
			return *((long *)this);
		}

	public:
		Mitov::String ToString()
		{
			return	Mitov::String( "(R:" ) + 
					Mitov::String((int)Red ) + ",G:" +
					Mitov::String((int)Green ) + ",B:" +
					Mitov::String((int)Blue ) + ")";
		}

		uint16_t To565Color()
		{
			return (((31*(Red+4))/255)<<11) | 
               (((63*(Green+2))/255)<<5) | 
               ((31*(Blue+4))/255);
		}
/*
		uint32_t GetGBR()
		{
			return ( uint32_t( Red ) << 16 ) | ( uint32_t( Green ) << 8 ) | Blue;
		}
*/
	public:
		TColor( long AValue = 0 )
		{
			SetValue( AValue );
		}

		TColor( unsigned char ARed, unsigned char AGreen, unsigned char ABlue ) :
			Red( ARed ),
			Green( AGreen ),
			Blue( ABlue )
		{
		}

		TColor( long AValue, bool ) // For Windows BGR support
		{
			Red = ( AValue >> 16 ) & 0xFF;
			Green = ( AValue >> 8 ) & 0xFF;
			Blue = AValue & 0xFF;
		}
	};
	#pragma pack(pop)
//---------------------------------------------------------------------------
	#pragma pack(push, 1)
	struct TRGBWColor
	{
	public:
		uint8_t Red;
		uint8_t Green;
		uint8_t Blue;
		uint8_t White;

	public:
		void SetValue( const uint32_t AValue )
		{
//			*((uint32_t *)this) = AValue;
			White = ( AValue >> 24 ) & 0xFF;
			Red = ( AValue >> 16 ) & 0xFF;
			Green = ( AValue >> 8 ) & 0xFF;
			Blue = AValue & 0xFF;
		}

		void operator =( const long AValue )
		{
			SetValue( AValue );
		}

		bool operator ==( const TRGBWColor other )
		{
			return *((uint32_t *)this) == *((uint32_t *)&other);
		}

		operator long()
		{
			return *((long *)this);
		}

	public:
		Mitov::String ToString()
		{
			return	Mitov::String( "(R:" ) + 
					Mitov::String((int)Red ) + ",G:" +
					Mitov::String((int)Green ) + ",B:" +
					Mitov::String((int)Blue ) + ",W:" +
					Mitov::String((int)White ) + ")";
		}

		uint16_t To565Color()
		{
			return (((31*(Red+4))/255)<<11) | 
               (((63*(Green+2))/255)<<5) | 
               ((31*(Blue+4))/255);
		}

	public:
		TRGBWColor( uint32_t AValue = 0 )
		{
			SetValue( AValue );
		}

		TRGBWColor( TColor &AColor )
		{
			Red = AColor.Red;
			Green = AColor.Green;
			Blue = AColor.Blue;
			White = 0;
		}

		TRGBWColor( unsigned char ARed, unsigned char AGreen, unsigned char ABlue, unsigned char AWhite ) :
			Red( ARed ),
			Green( AGreen ),
			Blue( ABlue ),
			White( AWhite )
		{
		}

		TRGBWColor( uint32_t AValue, bool ) // For Windows BGR support
		{
			White = ( AValue >> 24 ) & 0xFF;
			Red = ( AValue >> 16 ) & 0xFF;
			Green = ( AValue >> 8 ) & 0xFF;
			Blue = AValue & 0xFF;
		}
	};
	#pragma pack(pop)
//---------------------------------------------------------------------------
	typedef TRGBWColor TAlphaColor;
//---------------------------------------------------------------------------
	static const double HoursPerDay			= 24;
	static const double MinsPerHour			= 60;
	static const double SecsPerMin			= 60;
	static const double MSecsPerSec			= 1000;
	static const double MinsPerDay			= HoursPerDay * MinsPerHour;
	static const double SecsPerDay			= MinsPerDay * SecsPerMin;
	static const double SecsPerHour			= SecsPerMin * MinsPerHour;
	static const double MSecsPerDay			= SecsPerDay * MSecsPerSec;
	static const int32_t	IMSecsPerDay	= MSecsPerDay;
// Days between 1/1/0001 and 12/31/1899
//	const double DateDelta			= 693594;
//---------------------------------------------------------------------------
	const uint16_t MonthDays[2][12] =
	{
		{31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31},
		{31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31}
	};
//---------------------------------------------------------------------------
	class TDateTimeCompareItems
	{
	public:
		bool Years : 1;
		bool Months : 1;
		bool Days : 1;
		bool WeekDays : 1;
		bool Hours : 1;
		bool Minutes : 1;
		bool Seconds : 1;
		bool MilliSeconds : 1;

	public:
		bool AllTrue() const
		{ 
			return Years && Months && Days && Hours && Minutes && Seconds && MilliSeconds;
		}

		bool AllDateTrue() const
		{ 
			return Years && Months && Days;
		}

		bool AllDateFalse() const
		{ 
			return ( ! Years ) && ( ! Months ) && ( ! Days );
		}

		bool AllTimeTrue() const
		{ 
			return Hours && Minutes && Seconds && MilliSeconds;
		}

		bool AllTimeFalse() const
		{ 
			return ( ! Hours ) && ( ! Minutes ) && ( ! Seconds ) && ( ! MilliSeconds );
		}

		bool CompareWeekDays() const
		{
			return AllDateFalse() && WeekDays; 
		}

	public:
		TDateTimeCompareItems() :
			Years( true ),
			Months( true ),
			Days( true ),
			WeekDays( true ),
			Hours( true ),
			Minutes( true ),
			Seconds( true ),
			MilliSeconds( true )
		{
		}

	};
//---------------------------------------------------------------------------
	class TDateTime
	{
	public:
		// Do not change the order! Date Must be after Time for pcket communication!
		int32_t Time; // Number of milliseconds since midnight
		int32_t Date; // One plus number of days since 1/1/0001

	protected:
		inline void DivMod( int32_t Dividend, uint16_t Divisor, uint16_t &Result, uint16_t &Remainder ) const
		{
			Result = Dividend / Divisor;
			Remainder = Dividend % Divisor;
		}

	public:
		void DecodeTime( uint16_t &Hour, uint16_t &AMin, uint16_t &Sec, uint16_t &MSec ) const
		{
			uint16_t	MinCount, MSecCount;

			DivMod( Time, SecsPerMin * MSecsPerSec, MinCount, MSecCount );
			DivMod( MinCount, MinsPerHour, Hour, AMin );
			DivMod( MSecCount, MSecsPerSec, Sec, MSec );
		}

		bool IsLeapYear( uint16_t Year ) const
		{
			return (Year % 4 == 0) && ((Year % 100 != 0) || (Year % 400 == 0));
		}

		bool DecodeDateFully( uint16_t &Year, uint16_t &Month, uint16_t &Day, uint16_t &DOW ) const
		{
			const uint32_t TD1 = 365;
			const uint32_t TD4 = TD1 * 4 + 1;
			const uint32_t D100 = TD4 * 25 - 1;
			const uint32_t D400 = D100 * 4 + 1;

			int32_t T = Date;
			if( T <= 0 )
			{
				Year = 0;
				Month = 0;
				Day = 0;
				DOW = 0;
				return( false );
			}

			else
			{
				DOW = T % 7 + 1;
				--T;
				uint16_t Y = 1;
				while( T >= (int32_t)D400 )
				{
					T -= D400;
					Y += 400;
				}

				uint16_t D, I;
				DivMod( T, D100, I, D );
				if( I == 4 )
				{
					-- I;
					D -= D100;
				}

				Y += I * 100;
				DivMod(D, TD4, I, D);

				Y += I * 4;
				DivMod(D, TD1, I, D);

				if( I == 4 )
				{
					--I;
					D += TD1;
				}

				Y += I;
				bool Result = IsLeapYear(Y);

				const uint16_t *DayTable = MonthDays[Result];
				uint16_t M = 0; // The C++ Day table is zero indexed!
				for(;;)
				{
					I = DayTable[M];
					if( D < I )
						break;

					D -= I;
					++ M;
				}

				Year = Y;
				Month = M + 1;
				Day = D + 1;

                return Result;
			}
		}

		bool TryEncodeDate( uint16_t Year, uint16_t Month, uint16_t Day )
		{
			const uint16_t *DayTable = MonthDays[IsLeapYear(Year)];
			if( (Year >= 1) && (Year <= 9999) && (Month >= 1) && (Month <= 12) && (Day >= 1))
				if( Day <= DayTable[Month - 1] )
				{

					for( int i = 1; i < Month; i ++ )
						Day += DayTable[ i - 1 ];

					int I = Year - 1;
					Date = ((uint32_t)I) * 365 + (int)( I / 4 ) - (int)( I / 100 ) + (int)( I / 400 ) + (uint32_t)Day;
					return true;
				}

			return false;
		}

		bool TryEncodeTime( uint16_t Hour, uint16_t AMin, uint16_t Sec, uint16_t MSec )
		{
			if ((Hour < HoursPerDay) && (AMin < MinsPerHour) && (Sec < SecsPerMin) && (MSec < MSecsPerSec))
			{
				Time =  (Hour * (MinsPerHour * SecsPerMin * MSecsPerSec))
					  + (AMin * SecsPerMin * MSecsPerSec)
					  + (Sec * MSecsPerSec)
					  +  MSec;
//				Date = DateDelta; // This is the "zero" day for a TTimeStamp, days between 1/1/0001 and 12/30/1899 including the latter date
				Date = 0; // This is the "zero" day for a TTimeStamp, days between 1/1/0001 and 12/30/1899 including the latter date
				return true;
			}

			return false;
		}

		bool TryEncodeDateTime( uint16_t AYear, uint16_t AMonth, uint16_t ADay, uint16_t AHour, uint16_t AMinute, uint16_t ASecond, uint16_t AMilliSecond )
		{
			bool Result = TryEncodeDate( AYear, AMonth, ADay );
			if( Result )
			{
				TDateTime LTime;
				Result = LTime.TryEncodeTime( AHour, AMinute, ASecond, AMilliSecond );
				if( Result )
					Time = LTime.Time; 

			}

            return Result;
		}

		void DecodeDateTime( uint16_t &AYear, uint16_t &AMonth, uint16_t &ADay, uint16_t &AHour, uint16_t &AMinute, uint16_t &ASecond, uint16_t &AMilliSecond ) const
		{
			uint16_t AWeekDay;
			DecodeDateTime( AYear, AMonth, ADay, AWeekDay, AHour, AMinute, ASecond, AMilliSecond );
		}

		void DecodeDateTime( uint16_t &AYear, uint16_t &AMonth, uint16_t &ADay, uint16_t &AWeekDay, uint16_t &AHour, uint16_t &AMinute, uint16_t &ASecond, uint16_t &AMilliSecond ) const
		{
			DecodeDateFully( AYear, AMonth, ADay, AWeekDay );
			DecodeTime( AHour, AMinute, ASecond, AMilliSecond );
		}

		Mitov::String ToString() const
		{
			uint16_t AYear, AMonth, ADay, AHour, AMinute, ASecond, AMilliSecond;
			DecodeDateTime( AYear, AMonth, ADay, AHour, AMinute, ASecond, AMilliSecond );
			char ABuffer[ 6 + 4 + 5 * 2 + 3 + 2 + 1 + 5 ];
			sprintf( ABuffer, "(%04d.%02d.%02d %02d:%02d:%02d.%03d)", AYear, AMonth, ADay, AHour, AMinute, ASecond, AMilliSecond );
			return ABuffer;
		}

		uint32_t JulianDate()
		{
			uint16_t AYear;
			uint16_t AMonth;
			uint16_t ADay;
			uint16_t DOW;

			DecodeDateFully( AYear, AMonth, ADay, DOW );
//			Serial.println( AYear );
			if( AMonth <= 2 ) 
			{
				AYear--; 
				AMonth += 12;
			}

			int A = AYear / 100; 
			int B = 2 - A + A / 4;
			return uint32_t((365.25*( AYear+4716))+(int)(30.6001*(AMonth+1))+ ADay+B-1524 );
		}

		void AddMilliSeconds( int64_t AValue )
		{
			AllignTime( AValue + Time );
//			int64_t
//			if( int64_t( AValue ) + >= MSecsPerDay )
//			int32_t AOldTime
		}

		void AddSeconds( int64_t AValue )
		{
//			Serial.println( uint32_t( AValue ));
			AllignTime( int64_t( AValue * MSecsPerSec ) + Time );
		}

		void AddMinutes( int64_t AValue )
		{
			AllignTime( int64_t( AValue * MSecsPerSec * SecsPerMin ) + Time );
		}

		void AddHours( int64_t AValue )
		{
			AllignTime( int64_t( AValue * MSecsPerSec * SecsPerMin * MinsPerHour ) + Time );
		}

		void AddDays( int32_t AValue )
		{
			Date += AValue;
		}

		void AddMonths( int32_t AValue )
		{
			uint16_t AYear;
			uint16_t AMonth;
			uint16_t ADay;
			uint16_t DOW;
			DecodeDateFully( AYear, AMonth, ADay, DOW );

			IncAMonth( AYear, AMonth, ADay, AValue );

			TryEncodeDate( AYear, AMonth, ADay );
//			Date += AValue;
		}

		void AddYears( int32_t AValue )
		{
			AddMonths( AValue * 12 );
		}

	protected:
		void AllignTime( int64_t AValue )
		{
//			Serial.println( "AllignTime" );
//			Serial.println( int32_t( AValue ));
//			Serial.println( Time );
			Date += AValue / IMSecsPerDay;
			Time = AValue % IMSecsPerDay;
			if( Time < 0 )
			{
				Time += IMSecsPerDay;
				Date --;
			}
				
//			Serial.println( Time );
		}

		void IncAMonth( uint16_t &AYear, uint16_t &AMonth, uint16_t &ADay, int32_t ANumberOfMonths )
		{
			int Sign;

			if( ANumberOfMonths >= 0 )
				Sign = 1;

			else 
				Sign = -1;

			AYear += ANumberOfMonths / 12;
			ANumberOfMonths %= 12;
			int64_t ABigMonth = AMonth;
			ABigMonth += ANumberOfMonths;
			if( uint64_t( ABigMonth - 1 ) > 11 )    // if Month <= 0, word(Month-1) > 11)
			{
				AYear += Sign;
				ABigMonth += -12 * Sign;
			}

			AMonth = ABigMonth;

			const uint16_t *DayTable = MonthDays[IsLeapYear(AYear)];
			if( ADay > DayTable[ AMonth - 1 ] )
				ADay = DayTable[ AMonth - 1 ];

		}


	public:
		bool IsEqual( const TDateTime &AOther, const TDateTimeCompareItems &AItems ) const
		{
			if( AItems.AllTrue() )
				return ( *this == AOther );

			if( AItems.AllDateTrue() && AItems.AllTimeFalse() )
				return ( Date == AOther.Date );

			if( AItems.AllTimeTrue() && AItems.AllDateFalse() )
				return ( Time == AOther.Time );

			uint16_t AYear;
			uint16_t AMonth;
			uint16_t ADay;
			uint16_t AWeekDay;
			uint16_t AHour;
			uint16_t AMinute;
			uint16_t ASecond;
			uint16_t AMilliSecond;

			DecodeDateTime( AYear, AMonth, ADay, AWeekDay, AHour, AMinute, ASecond, AMilliSecond );

			uint16_t AOtherYear;
			uint16_t AOtherMonth;
			uint16_t AOtherDay;
			uint16_t AOtherWeekDay;
			uint16_t AOtherHour;
			uint16_t AOtherMinute;
			uint16_t AOtherSecond;
			uint16_t AOtherMilliSecond;

			AOther.DecodeDateTime( AOtherYear, AOtherMonth, AOtherDay, AOtherWeekDay, AOtherHour, AOtherMinute, AOtherSecond, AOtherMilliSecond );

			if( AItems.CompareWeekDays() )
				if( AWeekDay != AOtherWeekDay ) 
					return false;

			if( AItems.Years )
				if( AYear != AOtherYear ) 
					return false;

			if( AItems.Months )
				if( AMonth != AOtherMonth ) 
					return false;

			if( AItems.Days )
				if( ADay != AOtherDay ) 
					return false;

			if( AItems.Hours )
				if( AHour != AOtherHour ) 
					return false;

			if( AItems.Minutes )
				if( AMinute != AOtherMinute ) 
					return false;

			if( AItems.Seconds )
				if( ASecond != AOtherSecond ) 
					return false;

			if( AItems.MilliSeconds )
				if( AMilliSecond != AOtherMilliSecond ) 
					return false;

			return true;
		}

		bool IsBigger( const TDateTime &AOther, const TDateTimeCompareItems &AItems ) const
		{
			if( AItems.AllTrue() )
				return ( *this > AOther );

			if( AItems.AllDateTrue() && AItems.AllTimeFalse() )
				return ( Date > AOther.Date );

			if( AItems.AllTimeTrue() && AItems.AllDateFalse() )
				return ( Time > AOther.Time );

			uint16_t AYear;
			uint16_t AMonth;
			uint16_t ADay;
			uint16_t AWeekDay;
			uint16_t AHour;
			uint16_t AMinute;
			uint16_t ASecond;
			uint16_t AMilliSecond;

			DecodeDateTime( AYear, AMonth, ADay, AWeekDay, AHour, AMinute, ASecond, AMilliSecond );

			uint16_t AOtherYear;
			uint16_t AOtherMonth;
			uint16_t AOtherDay;
			uint16_t AOtherWeekDay;
			uint16_t AOtherHour;
			uint16_t AOtherMinute;
			uint16_t AOtherSecond;
			uint16_t AOtherMilliSecond;

			AOther.DecodeDateTime( AOtherYear, AOtherMonth, AOtherDay, AOtherWeekDay, AOtherHour, AOtherMinute, AOtherSecond, AOtherMilliSecond );

			if( AItems.CompareWeekDays() )
			{
				if( AWeekDay < AOtherWeekDay ) 
					return false;

				if( AWeekDay > AOtherWeekDay ) 
					return true;
			}

			if( AItems.Years )
			{
				if( AYear < AOtherYear ) 
					return false;

				if( AYear > AOtherYear ) 
					return true;
			}

			if( AItems.Months )
			{
				if( AMonth < AOtherMonth ) 
					return false;

				if( AMonth > AOtherMonth ) 
					return true;
			}

			if( AItems.Days )
			{
				if( ADay < AOtherDay ) 
					return false;

				if( ADay > AOtherDay ) 
					return true;
			}

			if( AItems.Hours )
			{
				if( AHour < AOtherHour ) 
					return false;

				if( AHour > AOtherHour ) 
					return true;
			}

			if( AItems.Minutes )
			{
				if( AMinute < AOtherMinute ) 
					return false;

				if( AMinute > AOtherMinute ) 
					return true;
			}

			if( AItems.Seconds )
			{
				if( ASecond < AOtherSecond ) 
					return false;

				if( ASecond > AOtherSecond ) 
					return true;
			}

			if( AItems.MilliSeconds )
			{
				if( AMilliSecond < AOtherMilliSecond ) 
					return false;

				if( AMilliSecond > AOtherMilliSecond ) 
					return true;
			}

			return true;
		}

		bool IsBiggerOrEqual( const TDateTime &AOther, const TDateTimeCompareItems &AItems ) const
		{
			if( AItems.AllTrue() )
				return ( *this >= AOther );

			return( IsEqual( AOther, AItems ) || IsBigger( AOther, AItems ));
		}

	public:
		bool operator == ( const TDateTime &AOther ) const
		{
			if( Date != AOther.Date )
				return false;

			return ( Time == AOther.Time );
		}

		bool operator != ( const TDateTime &AOther ) const
		{
			if( Date != AOther.Date )
				return true;

			return ( Time != AOther.Time );
		}

		bool operator <= ( const TDateTime &AOther ) const
		{
			if( Date > AOther.Date )
				return false;

			if( Date < AOther.Date )
				return true;

			return ( Time <= AOther.Time );
		}

		bool operator >= ( const TDateTime &AOther ) const
		{
			if( Date > AOther.Date )
				return true;

			if( Date < AOther.Date )
				return false;

			return ( Time >= AOther.Time );
		}

		bool operator < ( const TDateTime &AOther ) const
		{
			if( Date > AOther.Date )
				return false;

			if( Date < AOther.Date )
				return true;

			return ( Time < AOther.Time );
		}

		bool operator > ( const TDateTime &AOther ) const
		{
			if( Date > AOther.Date )
				return true;

			if( Date < AOther.Date )
				return false;

			return ( Time > AOther.Time );
		}

	public:
		TDateTime() :
			Time( 0 ),
			Date( 693594 )
		{
		}

		TDateTime( int32_t ADate, int32_t ATime ) :
			Time( ATime ),
			Date( ADate )
		{
		}

		TDateTime( uint32_t ADummy ) :
			Time( 0 ),
			Date( 693594 )
		{
		}

	};
//---------------------------------------------------------------------------
	class TComplex
	{
	public:
		float Real;
		float Imaginary;

	public:
		operator Mitov::String()
		{
			return "[" + Mitov::String( Real ) + "," + Mitov::String( Imaginary ) + "]";
		}

	public:
		TComplex() :
			Real( 0 ),
			Imaginary( 0 )
		{
		}

		TComplex( float AReal ) :
			Real( AReal ),
			Imaginary( 0 )
		{
		}
	};
//---------------------------------------------------------------------------
	template<typename T> inline T GetDefaultValue() { return 0; }
	template<> inline bool GetDefaultValue() { return false; }
	template<> inline Mitov::String GetDefaultValue() { return ""; }
	template<> inline TDateTime GetDefaultValue() { return TDateTime(); }
	template<> inline TColor GetDefaultValue() { return TColor(); }
	template<> inline TRGBWColor GetDefaultValue() { return TRGBWColor(); }
//---------------------------------------------------------------------------
	template<typename T> inline T ExtractValue( void *_Data ) { return *( (T *)_Data ); }
	template<> inline Mitov::String ExtractValue( void *_Data ) { return ( (char *)_Data ); }
//---------------------------------------------------------------------------
	template <typename T> void swap ( T& a, T& b )
	{
		T c(a); a=b; b=c;
	}
//---------------------------------------------------------------------------
	class Bytes
	{
	public:
		uint8_t *_Bytes;
		unsigned int  _BytesSize = 0;

	public:
		bool operator == ( const Bytes &AOther )
		{
			if( _BytesSize != AOther._BytesSize )
				return false;

			return ( memcmp( _Bytes, AOther._Bytes, _BytesSize ) == 0 );
		}
	};
//---------------------------------------------------------------------------
	class ConstBytes
	{
	public:
		uint8_t const * _Bytes;
		unsigned int  _BytesSize = 0;

	};
//---------------------------------------------------------------------------
	class RampToValueElementIntf
	{
//	public:
//		virtual void UpdateRamp( float ACurrentValue, unsigned long AOldTime, unsigned long ANewTime ) = 0;

	};
//---------------------------------------------------------------------------
	template<typename T> class ValueSource
	{
	public:
		OpenWire::SourcePin	OutputPin;

	public:
		T Value;

	public:
		inline T GetCurrentValue() { return Value; }

	public:
		void ExternalSetValue( RampToValueElementIntf * ARampSender, T AValue )
		{
			if( Value == AValue )
				return;

			Value = AValue;
			OutputPin.Notify( &Value );
		}

	public:
		inline void SystemInit() {} // Placeholder

		inline void SystemStart()
		{
			OutputPin.Notify( &Value );
		}

		inline void SystemLoopBegin( unsigned long currentMicros ) // Placeholder for inherited classes
		{
		}

		inline void SystemLoopEnd( unsigned long currentMicros ) // Placeholder for inherited classes
		{
		}

	public:
		inline void ClockInputPin_o_Receive( void *_Data )
		{
			OutputPin.Notify( &Value );
		}

	public:
		ValueSource( T AValue ) :
			Value( AValue )
		{
		}

	};
//---------------------------------------------------------------------------
	template<typename T> class BindableValueSource : public Mitov::ValueSource<T>
	{
		typedef Mitov::ValueSource<T> inherited;

	protected:
		T OldValue;

	public:
		void ExternalSetValue( RampToValueElementIntf * ARampSender, T AValue )
		{
			inherited::ExternalSetValue( ARampSender, AValue );
			OldValue = inherited::Value;
		}

	public:
		inline void SystemInit()
		{
			inherited::SystemInit();
			OldValue = inherited::Value;
		}

		inline void SystemLoopBegin( unsigned long currentMicros )
		{
			if( inherited::Value == OldValue )
				return;

			OldValue = inherited::Value;
			inherited::OutputPin.Notify( &OldValue );
		}

	public:
		BindableValueSource( T AValue ) :
			inherited( AValue ),
			OldValue( AValue )
		{
		}

	};
//---------------------------------------------------------------------------
	template<typename T> class ValueRampSource : public Mitov::ValueSource<T>
	{
		typedef Mitov::ValueSource<T> inherited;

	public:
		RampToValueElementIntf	*FActiveElement = nullptr;

	public:
		void ExternalSetValue( RampToValueElementIntf * ARampSender, T AValue )
		{
			if( ARampSender != FActiveElement )
				FActiveElement = nullptr;

			inherited::ExternalSetValue( ARampSender, AValue );
		}

	public:
		unsigned long	FLastTime = 0;

	public:
		inline void SystemLoopEnd( unsigned long currentMicros )
		{
			FLastTime = currentMicros;
		}

	public:
		void	RegisterForTimeUpdate( RampToValueElementIntf *AElement, bool ARegister )
		{
			if( ARegister )
				FActiveElement = AElement;

			else if( FActiveElement == AElement )
				FActiveElement = nullptr;

		}

	public:
		using inherited::inherited;

	};
//---------------------------------------------------------------------------
	template<typename T> class BindableValueRampSource : public ValueRampSource<T>
	{
		typedef Mitov::ValueRampSource<T> inherited;

	protected:
		T OldValue;

	public:
		void ExternalSetValue( RampToValueElementIntf * ARampSender, T AValue )
		{
			inherited::ExternalSetValue( ARampSender, AValue );
			OldValue = inherited::Value;
		}

	public:
		inline void SystemInit()
		{
			inherited::SystemInit();
			OldValue = inherited::Value;
		}

		inline void SystemLoopBegin( unsigned long currentMicros )
		{
			if( inherited::Value == OldValue )
				return;

			OldValue = inherited::Value;
			inherited::OutputPin.Notify( &OldValue );
		}

	public:
		BindableValueRampSource( T AValue ) :
			inherited( AValue ),
			OldValue( AValue )
		{
		}

	};
//---------------------------------------------------------------------------
	class BasicValueStateElement
	{
	public:
		virtual bool TryActive() = 0;

	};
//---------------------------------------------------------------------------
	template<typename T> class ValueStateSource : public Mitov::ValueSource<T>
	{
		typedef Mitov::ValueSource<T> inherited;

	protected:
		T FCurrentValue;

	public:
		inline T GetCurrentValue() { return FCurrentValue; }

	protected:
		Mitov::SimpleList<BasicValueStateElement *>	FElements;

	public:
		void ExternalSetValue( RampToValueElementIntf * ARampSender, T AValue )
		{
			if( FCurrentValue == AValue )
				return;

			FCurrentValue = AValue;
			inherited::OutputPin.Notify( &FCurrentValue );
		}

		void ResetValue()
		{
			for( int i = FElements.size(); i -- ; )
				if( FElements[ i ]->TryActive() )
					return;

			FCurrentValue = inherited::Value;
			inherited::OutputPin.Notify( &FCurrentValue );
		}

	public:
		inline void RegisterElement( BasicValueStateElement *AValue )
		{
			FElements.push_back( AValue );
		}

	public:
		inline void SystemStart()
		{
			FCurrentValue = inherited::Value;
			inherited::SystemStart();
//			if( ! ClockInputPin.IsConnected() )
//				OutputPin.Notify( &FCurrentValue );

		}

	public:
		inline void ClockInputPin_o_Receive( void *_Data )
		{
			inherited::OutputPin.Notify( &FCurrentValue );
		}

	public:
		using inherited::inherited;

	};
//---------------------------------------------------------------------------
	template<typename T> class BindableValueStateSource : public Mitov::ValueStateSource<T>
	{
		typedef Mitov::ValueStateSource<T> inherited;

	protected:
		T OldValue;

	public:
		inline void SystemInit()
		{
			inherited::SystemInit();
			OldValue = inherited::Value;
		}

		inline void SystemLoopBegin( unsigned long currentMicros )
		{
			if( inherited::Value == OldValue )
				return;

			OldValue = inherited::Value;
			inherited::OutputPin.Notify( &OldValue );
		}

	public:
		BindableValueStateSource( T AValue ) :
			inherited( AValue ),
			OldValue( AValue )
		{
		}

	};
//---------------------------------------------------------------------------
	template<typename T> class ValueRampStateSource : public Mitov::ValueStateSource<T>
	{
		typedef Mitov::ValueStateSource<T> inherited;

	public:
		RampToValueElementIntf	*FActiveElement = nullptr;

	public:
		void ExternalSetValue( RampToValueElementIntf * ARampSender, T AValue )
		{
			if( ARampSender != FActiveElement )
				FActiveElement = nullptr;

			inherited::ExternalSetValue( ARampSender, AValue );
		}

	public:
		unsigned long	FLastTime = 0;

	public:
		inline void SystemLoopEnd( unsigned long currentMicros )
		{
			FLastTime = currentMicros;
		}

	public:
		void	RegisterForTimeUpdate( RampToValueElementIntf *AElement, bool ARegister )
		{
			if( ARegister )
				FActiveElement = AElement;

			else if( FActiveElement == AElement )
				FActiveElement = nullptr;

		}

	public:
		using inherited::inherited;

	};
//---------------------------------------------------------------------------
	template<typename T> class BindableValueRampStateSource : public ValueRampStateSource<T>
	{
		typedef Mitov::ValueRampStateSource<T> inherited;

	protected:
		T OldValue;

	public:
		void ExternalSetValue( RampToValueElementIntf * ARampSender, T AValue )
		{
			inherited::ExternalSetValue( ARampSender, AValue );
			OldValue = inherited::Value;
		}

	public:
		inline void SystemInit()
		{
			inherited::SystemInit();
			OldValue = inherited::Value;
		}

		inline void SystemLoopBegin( unsigned long currentMicros )
		{
			if( inherited::Value == OldValue )
				return;

			OldValue = inherited::Value;
			inherited::OutputPin.Notify( &OldValue );
		}

	public:
		BindableValueRampStateSource( T AValue ) :
			inherited( AValue ),
			OldValue( AValue )
		{
		}

	};
//---------------------------------------------------------------------------
	class BinaryValueSource
	{
	public:
		OpenWire::SourcePin	OutputPin;

	public:
		Mitov::Bytes	Value;

	public:
		inline Mitov::Bytes GetCurrentValue() { return Value; }

	public:
		void ExternalSetValue( Mitov::Bytes &AValue )
		{
			if( Value == AValue )
				return;

			Value = AValue;
			OutputPin.SendValue( Mitov::TDataBlock( Value._BytesSize, Value._Bytes ));
		}

	public:
		inline void SystemStart()
		{
			OutputPin.SendValue( Mitov::TDataBlock( Value._BytesSize, Value._Bytes ));
		}

	public:
		inline void ClockInputPin_o_Receive( void *_Data )
		{
			OutputPin.SendValue( Mitov::TDataBlock( Value._BytesSize, Value._Bytes ));
//			OutputPin.Notify( &Value );
		}

	};
//---------------------------------------------------------------------------
	template<typename T_OWNER> class ArduinoBinaryStateSetValueElementBasic
	{
	protected:
		bool	FInputValue = false;

	public:
		Mitov::Bytes	Value;

	public:
		inline Mitov::Bytes GetCurrentValue() { return Value; }

	public:
		bool TryActive( T_OWNER *AOwner )
		{
			if( FInputValue )
			{
				AOwner->ExternalSetValue( Value );
				return true;
			}

			return false;
		}

	};
//---------------------------------------------------------------------------
	template<typename T_OWNER, T_OWNER &C_OWNER> class ArduinoBinaryStateSetValueElement : public ArduinoBinaryStateSetValueElementBasic<T_OWNER>
	{
		typedef ArduinoBinaryStateSetValueElementBasic<T_OWNER> inherited;

	public:
		void InputPin_o_Receive( void *_Data )
		{
			if( inherited::FInputValue == *(bool *)_Data )
				return;

			inherited::FInputValue = *(bool *)_Data;
			C_OWNER.ResetValue();
		}

	public:
		ArduinoBinaryStateSetValueElement()
		{
			C_OWNER.RegisterElement( this );
		}

	};
//---------------------------------------------------------------------------
    class BinaryStateValueSource
	{
	public:
		OpenWire::SourcePin	OutputPin;

	public:
		Mitov::Bytes	Value;

	protected:
		Mitov::Bytes	*FCurrentValue = &Value;

	public:
		inline Mitov::Bytes *GetCurrentValue() { return FCurrentValue; }

	protected:
		Mitov::SimpleList<ArduinoBinaryStateSetValueElementBasic<BinaryStateValueSource> *>	FElements;

	public:
		void ExternalSetValue( Mitov::Bytes &AValue )
		{
			if( *FCurrentValue == AValue )
				return;

			FCurrentValue = &AValue;
			OutputPin.SendValue( Mitov::TDataBlock( FCurrentValue->_BytesSize, FCurrentValue->_Bytes ));
		}

		void ResetValue()
		{
			for( int i = FElements.size(); i -- ; )
				if( FElements[ i ]->TryActive( this ) )
					return;

			FCurrentValue = &Value;
			OutputPin.SendValue( Mitov::TDataBlock( FCurrentValue->_BytesSize, FCurrentValue->_Bytes ));
		}

		void RegisterElement( ArduinoBinaryStateSetValueElementBasic<BinaryStateValueSource> *AValue )
		{
			FElements.push_back( AValue );
		}

	public:
		inline void SystemStart()
		{
			OutputPin.SendValue( Mitov::TDataBlock( FCurrentValue->_BytesSize, FCurrentValue->_Bytes ));
		}

	public:
		inline void ClockInputPin_o_Receive( void *_Data )
		{
			OutputPin.SendValue( Mitov::TDataBlock( FCurrentValue->_BytesSize, FCurrentValue->_Bytes ));
		}

	};
//---------------------------------------------------------------------------
	typedef void (OpenWire::Object::*TNotifyEvent) ();
//---------------------------------------------------------------------------
	template<int C_PIN_NUMBER> class DigitalInput
	{
	public:
		OpenWire::SourcePin	OutputPin;

	public:
		inline void SystemStart() {} // Placeholder

		inline void SystemInit()
		{
			pinMode(C_PIN_NUMBER, INPUT);
		}

		inline void SystemLoopBegin( unsigned long currentMicros )
		{
			bool AValue = digitalRead( C_PIN_NUMBER );
			OutputPin.Notify( &AValue );
		}

		inline void SystemLoopUpdateHardware() {} // Placeholder

	public:
		inline void SetMode( int AMode )
		{
#if defined( VISUINO_FREESOC2 ) || defined( VISUINO_STMDUINO )
			pinMode( C_PIN_NUMBER, (PIN_MODE)AMode );
#else
			pinMode( C_PIN_NUMBER, AMode );
#endif
		}

		inline bool DigitalRead()
		{
			return digitalRead( C_PIN_NUMBER );
		}

		inline void DigitalWrite( bool AValue )
		{
			digitalWrite( C_PIN_NUMBER, AValue );
		}

	};
//---------------------------------------------------------------------------
	template<int C_PIN_NUMBER> class DigitalPullUpInput
	{
	public:
		OpenWire::SourcePin	OutputPin;

	public:
		inline void SystemInit()
		{
			pinMode( C_PIN_NUMBER, INPUT_PULLUP );
		}

		inline void SystemStart()
		{
			bool AValue = digitalRead( C_PIN_NUMBER );
			OutputPin.Notify( &AValue );
		}

		inline void SystemLoopBegin( unsigned long currentMicros )
		{
			SystemStart();
		}

	public:
		void SetMode( int AMode )
		{
#if defined( VISUINO_FREESOC2 ) || defined( VISUINO_STMDUINO )
			pinMode( C_PIN_NUMBER, (PIN_MODE)AMode );
#else
			pinMode( C_PIN_NUMBER, AMode );
#endif
		}

		inline bool DigitalRead()
		{
			return digitalRead( C_PIN_NUMBER );
		}

		inline void DigitalWrite( bool AValue )
		{
			digitalWrite( C_PIN_NUMBER, AValue );
		}

	};
//---------------------------------------------------------------------------
	template<int C_PIN_NUMBER> class InvertedDigitalPullUpInput
	{
	public:
		OpenWire::SourcePin	OutputPin;

	public:
		inline void SystemInit()
		{
			pinMode( C_PIN_NUMBER, INPUT_PULLUP );
		}

		inline void SystemStart()
		{
			bool AValue = ! digitalRead( C_PIN_NUMBER );
			OutputPin.Notify( &AValue );
		}

		inline void SystemLoopBegin( unsigned long currentMicros )
		{
			SystemStart();
		}

	public:
		void SetMode( int AMode )
		{
#if defined( VISUINO_FREESOC2 ) || defined( VISUINO_STMDUINO )
			pinMode( C_PIN_NUMBER, (PIN_MODE)AMode );
#else
			pinMode( C_PIN_NUMBER, AMode );
#endif
		}

		inline bool DigitalRead()
		{
			return digitalRead( C_PIN_NUMBER );
		}

		inline void DigitalWrite( bool AValue )
		{
			digitalWrite( C_PIN_NUMBER, AValue );
		}

	};
//---------------------------------------------------------------------------
	template<int C_PIN_NUMBER> class AnalogAsDigitalInput
	{
	public:
		OpenWire::SourcePin	OutputPin;

	public:
		void SetMode( int AMode ) {} // Placeholder

		bool DigitalRead()
		{
			int AValue = analogRead( C_PIN_NUMBER );
			return ( AValue > VISUINO_ANALOG_IN_THRESHOLD ); // > 2.0 V High < 0.8 V Low
		}

	public:
		inline void SystemLoopBegin( unsigned long currentMicros )
		{
			bool AValue = DigitalRead();
			OutputPin.Notify( &AValue );
		}

	};
//---------------------------------------------------------------------------
	template<int C_PIN_NUMBER> class ArduinoAnalogInputChannel
	{
	public:
		OpenWire::SourcePin	OutputPin;

	protected:
		float			FLastOutput = 0.0f;

#ifdef VISUINO_ESP8266
		unsigned int	FLastTime;
#endif // VISUINO_ESP8266

#ifdef VISUINO_TEENSY
	protected:
		unsigned int	FResolution = 0;
		float			FDivider = 1023.0;

	public:
		void SetResolution( unsigned int AResolution )
		{
			if( AResolution == FResolution )
				return;

			FResolution = AResolution;
			analogReadRes( FResolution );
			CalculateMultiplier();
		}

	protected:
		void CalculateMultiplier()
		{
			FDivider = pow( 2, FResolution ) - 1;
		}

#endif // VISUINO_TEENSY

	public:
		inline void SystemInit() {} // Placeholder

		inline void SystemLoopUpdateHardware() {} // Placeholder

		inline void SystemStart()
		{
#ifdef VISUINO_TEENSY
			analogReadRes( FResolution );
			FLastOutput = analogRead( C_PIN_NUMBER ) / float( FDivider );
#else // VISUINO_TEENSY
			FLastOutput = analogRead( C_PIN_NUMBER ) / float( VISUINO_ANALOG_IN_RANGE );
#endif // VISUINO_TEENSY

#ifdef VISUINO_ESP8266
			FLastTime = micros();
#endif // VISUINO_ESP8266
	    	OutputPin.Notify( &FLastOutput );
		}

		inline void SystemLoopBegin( unsigned long currentMicros )
		{
#ifdef VISUINO_ESP8266
			if( currentMicros - FLastTime < 10000 )
				return;

			FLastTime = currentMicros;
#endif // VISUINO_ESP8266
#ifdef VISUINO_TEENSY
			float AValue = analogRead( C_PIN_NUMBER ) / float( FDivider );
#else // VISUINO_TEENSY
			float AValue = analogRead( C_PIN_NUMBER ) / float( VISUINO_ANALOG_IN_RANGE );
#endif // VISUINO_TEENSY
			if( AValue == FLastOutput )
				return;

			FLastOutput = AValue;
			OutputPin.Notify( &AValue );
		}

	public:
		ArduinoAnalogInputChannel()
#ifdef VISUINO_TEENSY
			: FResolution( 1023 )
#endif // VISUINO_TEENSY
		{
		}

#ifdef VISUINO_TEENSY
		ArduinoAnalogInputChannel( unsigned int AResolution ) :
			FResolution( AResolution )
		{
			CalculateMultiplier();
		}

#endif // VISUINO_TEENSY
	};
//---------------------------------------------------------------------------
#if ( defined( VISUINO_TEENSY ) || defined( VISUINO_ESP32 ))
//---------------------------------------------------------------------------
	template<int C_PIN_NUMBER> class ArduinoTouchAnalogInputChannel
	{
	public:
		OpenWire::SourcePin	OutputPin;

	protected:
		float	FLastOutput = 0.0f;

	public:
		inline void SystemStart()
		{
			FLastOutput = touchRead( C_PIN_NUMBER );
	    	OutputPin.Notify( &FLastOutput );
		}

		inline void SystemLoopBegin( unsigned long currentMicros )
		{
			float AValue = touchRead( C_PIN_NUMBER );
			if( AValue == FLastOutput )
				return;

			FLastOutput = AValue;
			OutputPin.Notify( &AValue );
		}

	};
//---------------------------------------------------------------------------
/*
	template <int V_PIN> void SetArduinoAnalogInputResolutionChannel( unsigned int AValue )
	{
		analogReadRes( AValue );
	}
*/
//---------------------------------------------------------------------------
#endif // VISUINO_TEENSY
//---------------------------------------------------------------------------
	template<int C_PIN_NUMBER> class AnalogOutput
	{
	public:
		void InputPin_o_Receive( void *_Data )
		{
			float AValue = *(float*)_Data;
			analogWrite( C_PIN_NUMBER, ( AValue * PWMRANGE ) + 0.5 );
		}

	public:
		inline void SystemInit()
		{
			pinMode( C_PIN_NUMBER, OUTPUT);
		}

	};
//---------------------------------------------------------------------------
    template<int C_PIN_NUMBER, bool T_INITIAL_VALUE> class ArduinoDigitalInputChannel
	{
/*
	public:
		class DigitalPin : public OpenWire::ReceivePin
		{
//		public:
//			uint8_t FPinNumber;

		protected:
			virtual void Receive( void *_Data ) override
			{
				bool AValue = *(bool *)_Data;
				digitalWrite( C_PIN_NUMBER, AValue ? HIGH : LOW );
			}
		};

	public:
		DigitalPin	DigitalInputPin;
*/
/*
	protected:
		virtual void SystemInit() override
		{
//			inherited::SystemInit();
			pinMode( DigitalInputPin.FPinNumber, OUTPUT );
		}
*/
	public:
		void DigitalInputPin_o_Receive( void *_Data )
		{
				bool AValue = *(bool *)_Data;
				digitalWrite( C_PIN_NUMBER, AValue ? HIGH : LOW );
		}

	public:
		ArduinoDigitalInputChannel( bool AIsOpenDrain )
		{
			pinMode( C_PIN_NUMBER, ( AIsOpenDrain ) ? OUTPUT_OPEN_DRAIN : OUTPUT );
			digitalWrite( C_PIN_NUMBER, T_INITIAL_VALUE ? HIGH : LOW );
		}
	};
//---------------------------------------------------------------------------
    template<int C_PIN_NUMBER> class ArduinoDigitalOutputChannel // : public BasicPinRead
	{
	public:
		OpenWire::SourcePin	OutputPin;

	protected:
        bool FIsPullUp : 1;
		bool FIsPullDown : 1;
		bool FLastOutput : 1;

	public:
		inline void SystemInit()
		{
			pinMode( C_PIN_NUMBER, FIsPullUp ? INPUT_PULLUP : ( FIsPullDown ? INPUT_PULLDOWN : INPUT ) );
		}

		inline void SystemStart()
		{
			FLastOutput = ( digitalRead( C_PIN_NUMBER ) == HIGH );
	    	OutputPin.SendValue( FLastOutput );
		}

		inline void SystemLoopBegin( unsigned long currentMicros )
        {
    		bool AValue = ( digitalRead( C_PIN_NUMBER ) == HIGH );
			if( AValue == FLastOutput )
				return;

			FLastOutput = AValue;
	    	OutputPin.Notify( &AValue );
        }

	public:
		void SetMode( int AMode )
		{
#if defined( VISUINO_FREESOC2 ) || defined( VISUINO_STMDUINO )
			pinMode( C_PIN_NUMBER, (PIN_MODE)AMode );
#else
			pinMode( C_PIN_NUMBER, AMode );
#endif
		}

		bool DigitalRead()
		{
			return digitalRead( C_PIN_NUMBER );
		}

		inline void DigitalWrite( bool AValue )
		{
			digitalWrite( C_PIN_NUMBER, AValue );
		}

	public:
		ArduinoDigitalOutputChannel( bool AIsPullUp, bool AIsPullDown ) :
            FIsPullUp( AIsPullUp ),
			FIsPullDown( AIsPullDown )
		{
		}

	};
//---------------------------------------------------------------------------
    template<int C_PIN_NUMBER, bool T_INITIAL_VALUE> class ArduinoDigitalChannel
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
				pinMode( C_PIN_NUMBER, ( FIsOpenDrain ) ? OUTPUT_OPEN_DRAIN : OUTPUT );
				if( ! FIsAnalog )
					digitalWrite( C_PIN_NUMBER, T_INITIAL_VALUE ? HIGH : LOW );
			}

            else
				pinMode( C_PIN_NUMBER, ( FIsPullUp ) ? INPUT_PULLUP : ( FIsPullDown ? INPUT_PULLDOWN : INPUT ) );
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
                analogWrite( C_PIN_NUMBER, ( AValue * PWMRANGE ) + 0.5 );
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
		ArduinoDigitalChannel( bool AIsOutput, bool AIsPullUp, bool AIsPullDown, bool AIsOpenDrain, bool AIsAnalog, bool ACombinedInOut, bool ARawInput ) :
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
    template<int C_PIN_NUMBER> class ArduinoAnalogOutputChannel
    {
	public:
		void InputPin_o_Receive( void *_Data )
		{
            float AValue = *(float*)_Data;
            analogWrite( C_PIN_NUMBER, ( AValue * PWMRANGE ) + 0.5 );
		}

	};
//---------------------------------------------------------------------------
	template<int PIN_NUMBER> class DirectOuputPin
	{
	public:
		inline void InputPin_o_Receive( void *_Data )
		{
			digitalWrite( PIN_NUMBER, ( *(bool*)_Data ) ? HIGH : LOW );
		}

	public:
		DirectOuputPin( bool AInitialValue = false )
		{
			pinMode( PIN_NUMBER, OUTPUT );
			digitalWrite( PIN_NUMBER, ( AInitialValue ) ? HIGH : LOW );
		}
	};
//---------------------------------------------------------------------------
	template<int PIN_1, int PIN_2> class DigtitalInputPins2
	{
	public:
		OpenWire::TypedSourcePin<bool>	OutputPins[ 2 ];

	protected:
		bool	FChangeOnly = false;

	public:
		inline void SystemInit()
		{
//			pinMode( PIN_1, INPUT );
//			pinMode( PIN_2, INPUT );
		}

		inline void SystemLoopBegin( unsigned long currentMicros )
		{
			OutputPins[ 0 ].SetValue( digitalRead( PIN_1 ) == HIGH , FChangeOnly );
			OutputPins[ 1 ].SetValue( digitalRead( PIN_2 ) == HIGH , FChangeOnly );

			FChangeOnly = true;
		}
	};
//---------------------------------------------------------------------------
	template<int PIN_1, int PIN_2> class InvertedDigtitalInputPins2
	{
	public:
		OpenWire::TypedSourcePin<bool>	OutputPins[ 2 ];

	protected:
		bool	FChangeOnly = false;

	public:
		inline void SystemInit()
		{
//			pinMode( PIN_1, INPUT );
//			pinMode( PIN_2, INPUT );
		}

		inline void SystemLoopBegin( unsigned long currentMicros )
		{
			OutputPins[ 0 ].SetValue( digitalRead( PIN_1 ) != HIGH , FChangeOnly );
			OutputPins[ 1 ].SetValue( digitalRead( PIN_2 ) != HIGH , FChangeOnly );

			FChangeOnly = true;
		}
	};
//---------------------------------------------------------------------------
	template<int C_PIN_NUMBER>	class TPinAccess
	{
	public:
		inline bool DigitalRead()
		{
			return digitalRead( C_PIN_NUMBER );
		}

	};
//---------------------------------------------------------------------------
	template<int PIN_1, int PIN_2, typename T_OUT_PIN_0, typename T_OUT_PIN_1> class DigtitalInputChangeOnlyPins2 : public T_OUT_PIN_0, public T_OUT_PIN_1 //, public TPinAccess<PIN_1>, public TPinAccess<PIN_2>
	{
	public:
		typedef TPinAccess<PIN_1> T_PIN_TYPE_0;
		typedef TPinAccess<PIN_2> T_PIN_TYPE_1;

	public:
		static T_PIN_TYPE_0 PinObject0;
		static T_PIN_TYPE_1 PinObject1;

	public:
		OpenWire::SourcePin &OutputPins0() { return T_OUT_PIN_0::GetPinInstance(); }
		OpenWire::SourcePin &OutputPins1() { return T_OUT_PIN_1::GetPinInstance(); }

	public:
		inline void SystemInit()
		{
//			pinMode( PIN_1, INPUT );
//			pinMode( PIN_2, INPUT );
		}

		inline void SystemStart()
		{
			T_OUT_PIN_0::SetPinValue( digitalRead( PIN_1 ) != LOW, false );
			T_OUT_PIN_1::SetPinValue( digitalRead( PIN_2 ) != LOW, false );
		}

		inline void SystemLoopBegin( unsigned long currentMicros )
		{
			T_OUT_PIN_0::SetPinValue( digitalRead( PIN_1 ) != LOW, true );
			T_OUT_PIN_1::SetPinValue( digitalRead( PIN_2 ) != LOW, true );
		}
	};
//---------------------------------------------------------------------------
	template<int PIN_1, int PIN_2, typename T_OUT_PIN_0, typename T_OUT_PIN_1> class InvertedDigtitalInputChangeOnlyPins2 : public T_OUT_PIN_0, public T_OUT_PIN_1 //, public TPinAccess<PIN_1>, public TPinAccess<PIN_2>
	{
	public:
		typedef TPinAccess<PIN_1> T_PIN_TYPE_0;
		typedef TPinAccess<PIN_2> T_PIN_TYPE_1;

	public:
		static T_PIN_TYPE_0 PinObject0;
		static T_PIN_TYPE_1 PinObject1;

	public:
		OpenWire::SourcePin &OutputPins0() { return T_OUT_PIN_0::GetPinInstance(); }
		OpenWire::SourcePin &OutputPins1() { return T_OUT_PIN_1::GetPinInstance(); }

	public:
		inline void SystemInit()
		{
//			pinMode( PIN_1, INPUT );
//			pinMode( PIN_2, INPUT );
		}

		inline void SystemStart()
		{
			T_OUT_PIN_0::SetPinValue( digitalRead( PIN_1 ) != HIGH, false );
			T_OUT_PIN_1::SetPinValue( digitalRead( PIN_2 ) != HIGH, false );
		}

		inline void SystemLoopBegin( unsigned long currentMicros )
		{
			T_OUT_PIN_0::SetPinValue( digitalRead( PIN_1 ) != HIGH, true );
			T_OUT_PIN_1::SetPinValue( digitalRead( PIN_2 ) != HIGH, true );
		}
	};
//---------------------------------------------------------------------------
//	template<int PIN_1, int PIN_2, typename T_OUT_PIN_0, typename T_OUT_PIN_1> InvertedDigtitalInputChangeOnlyPins2<PIN_1, PIN_2, T_OUT_PIN_0, T_OUT_PIN_1>::T_PIN_TYPE_0	InvertedDigtitalInputChangeOnlyPins2<PIN_1, PIN_2, T_OUT_PIN_0, T_OUT_PIN_1>::::PinAccess0;
//---------------------------------------------------------------------------
	template<int PIN_1, int PIN_2, typename T_OUT_PIN_0, typename T_OUT_PIN_1> class InvertedDigtitalInputPullUpChangeOnlyPins2 : public InvertedDigtitalInputChangeOnlyPins2<PIN_1, PIN_2, T_OUT_PIN_0, T_OUT_PIN_1>
	{
	public:
		inline void SystemInit()
		{
			pinMode( PIN_1, INPUT_PULLUP );
			pinMode( PIN_2, INPUT_PULLUP );
		}

	};
//---------------------------------------------------------------------------
	template<int PIN_1, int PIN_2, typename T_OUT_PIN_0, typename T_OUT_PIN_1> class AnalogInputPins2 : public T_OUT_PIN_0, public T_OUT_PIN_1
	{
	public:
		OpenWire::SourcePin &OutputPins0() { return T_OUT_PIN_0::GetPinInstance(); }
		OpenWire::SourcePin &OutputPins1() { return T_OUT_PIN_1::GetPinInstance(); }

	protected:
		inline void ReadValues()
		{
			T_OUT_PIN_0::SetPinValue( analogRead( PIN_1 ) / float( VISUINO_ANALOG_IN_RANGE ) );
			T_OUT_PIN_1::SetPinValue( analogRead( PIN_2 ) / float( VISUINO_ANALOG_IN_RANGE ) );
		}

	public:
		inline void ClockInputPin_o_Receive( void *_Data )
		{
			ReadValues();
		}

	public:
		inline void SystemInit()
		{
//			pinMode( PIN_1, INPUT );
//			pinMode( PIN_2, INPUT );
		}

		inline void SystemStart()
		{
			ReadValues();
		}

		inline void SystemLoopBegin( unsigned long currentMicros )
		{
			ReadValues();
		}

	};
//---------------------------------------------------------------------------
/*
	template<int C_PIN_NUMBER> class AnalogInitialValuePin
	{
	public:
		float InitialValue = 0.5;

	public:
		inline void InputPin_o_Receive( void *_Data )
		{
			InitialValue = *(float*)_Data;
			SystemStart();
		}

	public:
		inline void SystemStart()
		{
			analogWrite( C_PIN_NUMBER, ( InitialValue * PWMRANGE ) + 0.5 );
		}
	};
*/
//---------------------------------------------------------------------------
#if defined( VISUINO_NRF52832 ) || defined( VISUINO_BBC_MICRO_BIT )
	#define SERIAL_TYPE Uart

#else // VISUINO_NRF52832
	#ifdef VISUINO_FREESOC2
		#define SERIAL_TYPE UARTClass

	#else // VISUINO_FREESOC2
		#ifdef VISUINO_FEATHER_M0
			#define SERIAL_TYPE Serial_

		#else // VISUINO_FEATHER_M0
			#ifdef VISUINO_FEMTO_USB
				#define SERIAL_TYPE Serial_
				#define Serial SerialUSB

			#else // VISUINO_FEMTO_USB
				#ifdef HAVE_CDCSERIAL
					#define SERIAL_TYPE Serial_
				#else // HAVE_CDCSERIAL
					#ifdef VISUINO_ARDUINO_101
						#define SERIAL_TYPE CDCSerialClass
					#else // VISUINO_ARDUINO_101
						#ifdef VISUINO_INTEL_EDISON
							#define SERIAL_TYPE TTYUARTClass
						#else // VISUINO_INTEL_EDISON
							#define SERIAL_TYPE HardwareSerial
						#endif

					#endif // VISUINO_ARDUINO_101
				#endif // HAVE_CDCSERIAL
			#endif // VISUINO_FEMTO_USB
		#endif // VISUINO_FEATHER_M0
	#endif // VISUINO_FREESOC2
#endif // VISUINO_NRF52832
//---------------------------------------------------------------------------
	enum TArduinoSerialParity { spNone, spEven, slOdd, slMark, slSpace };
//---------------------------------------------------------------------------
	template<typename T_PARENT, typename T_SERIAL_TYPE, T_SERIAL_TYPE &C_SERIAL> class TypedSerialPort : public T_PARENT
	{
#ifdef _MITOV_SERIAL_SENDING_PIN_
	public:
		OpenWire::SourcePin	SendingOutputPin;
#endif

	public:
		bool	Enabled : 1;

#ifdef _MITOV_SERIAL_SENDING_PIN_
	protected:
		bool	FSending : 1;

	public:
		uint32_t	AfterSendingDelay = 0;
#endif

	public:
		inline Stream &GetStream()
		{
			return C_SERIAL;
		}

	public:
		void ResetSerial()
		{
			C_SERIAL.end();
			T_PARENT::StartPort();
		}


/*
#ifdef _MITOV_SERIAL_SENDING_PIN_
		virtual uint32_t GetByteSpeed()
		{ 
			return 9600 / 11;
		}
#endif
*/
	protected:
#ifdef _MITOV_SERIAL_SENDING_PIN_
		uint32_t	FEndTime;
#endif

#ifdef _MITOV_SERIAL_SENDING_PIN_
	protected:
//	#if ( defined( VISUINO_CONTROLLINO_MAXI ) || defined( VISUINO_CONTROLLINO_MEGA ) || defined( VISUINO_M_DUINO_ETHERNET_58 ))
//		virtual
//	#endif
		void UpdateSending( bool ASending )
		{
			T_PARENT::UpdateSending( ASending );
			SendingOutputPin.SendValue( ASending );
		}

		void SendingCount( uint32_t ACount )
		{
//			Serial.println( ACount );

			ACount = 1; // Most conrtrollers can send only one character at a time.
#ifdef __AVR
			ACount = 1; // The AVR can send only one character at a time.
#endif
			if( ! FSending )
			{
				FSending = true;
				UpdateSending( FSending );
				uint32_t ATime = micros();
				FEndTime = ATime + AfterSendingDelay + ACount * ( 10000000 / T_PARENT::GetByteSpeed() );
//				Serial.println( 10000000 / GetByteSpeed() );
//				Serial.println( int32_t( FEndTime - ATime ));
			}

			else
			{
				UpdateSending( FSending );
				uint32_t ATime = micros();
				if( int32_t( FEndTime - ATime ) >= AfterSendingDelay )
					FEndTime += ACount * ( 10000000 / T_PARENT::GetByteSpeed() );

				else
					FEndTime = ATime + AfterSendingDelay + ACount * ( 10000000 / T_PARENT::GetByteSpeed() );

//					Serial.println( 10000000 / GetByteSpeed() );
//					Serial.println( int32_t( FEndTime - ATime ));
			}

//			Serial.println( 10000000 / GetByteSpeed() );
//			Serial.println( int32_t( FEndTime - ATime ));
		}
#endif

	public:
		template<typename T> void Print( T AValue )
		{
			if( Enabled )
			{
#ifdef _MITOV_SERIAL_SENDING_PIN_
				SendingCount( 1 );
#endif
				C_SERIAL.println( AValue );
#ifdef __AVR
//				SendingCount( 1 );
#endif
			}
		}

#ifdef _MITOV_SERIAL_SENDING_PIN_
		void Print( Mitov::String AValue )
		{
			if( Enabled )
			{
#ifdef _MITOV_SERIAL_SENDING_PIN_
				SendingCount( AValue.length() );
#endif
				C_SERIAL.println( AValue );
#ifdef _MITOV_SERIAL_SENDING_PIN_
#ifdef __AVR
//				SendingCount( 1 );
#endif
#endif
			}
		}

		void Print( char *AValue )
		{
			if( Enabled )
			{
#ifdef _MITOV_SERIAL_SENDING_PIN_
				SendingCount( strlen( AValue ) );
#endif
				C_SERIAL.println( AValue );
#ifdef _MITOV_SERIAL_SENDING_PIN_
#ifdef __AVR
//				SendingCount( 1 );
#endif
#endif
			}
		}
#endif

		void PrintChar( char AValue )
		{
			if( Enabled )
			{
#ifdef _MITOV_SERIAL_SENDING_PIN_
				SendingCount( 1 );
#endif
				C_SERIAL.print( AValue );
			}
		}

		void Write( uint8_t *AData, uint32_t ASize )
		{
#ifdef _MITOV_SERIAL_SENDING_PIN_
			SendingCount( ASize );
#endif
			C_SERIAL.write( AData, ASize );
#ifdef _MITOV_SERIAL_SENDING_PIN_
#ifdef __AVR
//			SendingCount( 1 );
#endif
#endif
		}

	public:
		inline void SystemLoopBegin( unsigned long currentMicros )
		{
#ifdef _MITOV_SERIAL_SENDING_PIN_
			if( FSending )
			{
				if( int32_t( FEndTime - currentMicros ) < 0 )
				{
//					Serial.print( "STOP: " );
//					Serial.println( int32_t( FEndTime - currentMicros ) );
					FSending = false;
					UpdateSending( FSending );
				}
			}

#endif
		}

		inline void SystemInit()
		{
			if( Enabled )
				T_PARENT::StartPort();

		}

	public:
        void RestartPort() // Used by setters
		{
			if( ! Enabled )
				return;

			C_SERIAL.end();
			T_PARENT::StartPort();
		}

	public:
		void SetEnabled( bool AValue )
		{
            if( Enabled == AValue )
                return;

            Enabled = AValue;
			if( Enabled )
				T_PARENT::StartPort();

			else
			{
				C_SERIAL.end();
				T_PARENT::SerialEnd();
			}
		}

	public:
		TypedSerialPort() :
			Enabled( true )
#ifdef _MITOV_SERIAL_SENDING_PIN_
			,
			FSending( false )
#endif
		{
		}

	};
//---------------------------------------------------------------------------
	template<typename T_SERIAL_TYPE, T_SERIAL_TYPE &C_SERIAL> class SpeedSerialPort
	{
	public:
		uint32_t	Speed = 9600;

	protected:
#ifdef _MITOV_SERIAL_SENDING_PIN_
		uint32_t GetByteSpeed()
		{ 
			return Speed * 11; 
		}
#endif

		inline void SerialEnd() {} // Placeholder

		inline void StartPort()
		{
			C_SERIAL.begin( Speed );
		}

	};
//---------------------------------------------------------------------------
	template<typename T_INSTANCE, T_INSTANCE &C_SERIAL, typename T> class ArduinoSerialInput
	{
	public:
		inline void InputPin_o_Receive( void *_Data )
		{
			C_SERIAL.Print( *(T*)_Data );
		}

	};
//---------------------------------------------------------------------------
	template<typename T_INSTANCE, T_INSTANCE &C_SERIAL> class ArduinoSerialInput<T_INSTANCE, C_SERIAL, bool>
	{
	public:
		inline void InputPin_o_Receive( void *_Data )
		{
			C_SERIAL.Print( *(bool*)_Data ? "true" : "false" );
		}

	};
//---------------------------------------------------------------------------
	template<typename T_INSTANCE, T_INSTANCE &C_SERIAL> class ArduinoSerialInput<T_INSTANCE, C_SERIAL, char>
	{
	public:
		void InputPin_o_Receive( void *_Data )
		{
			if( *(char*)_Data == '\n' )
				return;

			if( *(char*)_Data == '\r' )
				C_SERIAL.Print( "" );

			else
				C_SERIAL.PrintChar( *(char*)_Data );
		}

	};
//---------------------------------------------------------------------------
	template<typename T_INSTANCE, T_INSTANCE &C_SERIAL> class ArduinoSerialBinaryInput
	{
	public:
		void InputPin_o_Receive( void *_Data )
		{
			if( C_SERIAL.Enabled )
			{
				Mitov::TDataBlock ADataBlock = *(Mitov::TDataBlock*)_Data;
				C_SERIAL.Write((uint8_t *) ADataBlock.Data, ADataBlock.Size );
//				C_SERIAL.GetStream().write( *(unsigned char*)_Data );
			}
		}

	};
//---------------------------------------------------------------------------	
	template<typename T_INSTANCE, T_INSTANCE &C_SERIAL> class ArduinoEnabledSerialBinaryInput
	{
	public:
		void InputPin_o_Receive( void *_Data )
		{
			Mitov::TDataBlock ADataBlock = *(Mitov::TDataBlock*)_Data;
			C_SERIAL.Write((uint8_t *) ADataBlock.Data, ADataBlock.Size );
//				C_SERIAL.GetStream().write( *(unsigned char*)_Data );
		}

	};
//---------------------------------------------------------------------------
	template<typename T_INSTANCE, T_INSTANCE &C_SERIAL> class ArduinoSerialStringInput
	{
	public:
		void InputPin_o_Receive( void *_Data )
		{
			C_SERIAL.Print( (char*)_Data );
		}

	};
//---------------------------------------------------------------------------
	template<typename T_INSTANCE, T_INSTANCE &C_SERIAL> class ArduinoSerialClockInput
	{
	public:
		void InputPin_o_Receive( void *_Data )
		{
			C_SERIAL.Print( "(Clock)" );
		}

	};
//---------------------------------------------------------------------------
	template<typename T_INSTANCE, T_INSTANCE &C_SERIAL, typename T_OBJECT> class ArduinoSerialObjectInput
	{
	public:
		void InputPin_o_Receive( void *_Data )
		{
			C_SERIAL.Print( ((T_OBJECT *)_Data)->ToString().c_str() );
		}

	};
//---------------------------------------------------------------------------
/*
	template<typename T_INSTANCE, T_INSTANCE &C_SERIAL> class ArduinoDirectSerialInput : public OpenWire::Object
	{
		typedef OpenWire::Object	inherited;

	public:
		OpenWire::SinkPin	InputPin;

	public:
		virtual Stream &GetStream()
		{
			return C_SERIAL.GetStream();
		}

	protected:
		void DoReceive( void *_Data )
		{
//			if( C_SERIAL.Enabled )
//				C_SERIAL.write( *(unsigned char*)_Data );
		}

	public:
		ArduinoDirectSerialInput()
		{
			InputPin.SetCallback( this, (OpenWire::TOnPinReceive)&ArduinoDirectSerialInput::DoReceive );
		}
	};
*/
//---------------------------------------------------------------------------
	template<typename T_INSTANCE, T_INSTANCE &C_SERIAL> class ArduinoSerialOutput
	{
	public:
		OpenWire::SourcePin	OutputPin;

	public:
		inline void SystemLoopBegin( unsigned long currentMicros )
		{
			if( ! C_SERIAL.Enabled )
				return;

			uint8_t ABuffer[ 20 ];

			int i = 0;
			for( ; i < 20; ++i )
			{
				int AData = C_SERIAL.GetStream().read();
				if( AData < 0 )
					break;

				ABuffer[ i ] = AData;
			}

			if( i > 0 )
				OutputPin.SendValue( Mitov::TDataBlock( i, ABuffer ));

		}

	};
//---------------------------------------------------------------------------
	class GPRSAccessPoint
	{
	public:
		bool	Enabled = true;

		Mitov::String	AccessPointName;
		Mitov::String	UserName;
		Mitov::String	Password;

		int32_t		NumberRetries = 5;
		uint32_t	RetryInterval = 30000;

	public:
		int32_t	FRetryCount = 0;
		unsigned long FLastTime = 0;

	public:
		void ResetRetry()
		{
			FRetryCount = 0;
		}

	public:
		bool CanRetry( unsigned long currentMicros, bool &ACanRetry )
		{
			ACanRetry = ( FRetryCount <= NumberRetries );
//			Serial.print( "ACanRetry" ); Serial.println( ACanRetry );
			if( FRetryCount == 0 )
				return true;

			if( ! ACanRetry )
				return false;

			if(( currentMicros - FLastTime ) < RetryInterval * 1000 )
				return false;

			return true;
		}

	};
//---------------------------------------------------------------------------
	class Convert_AnalogToComplexArray
	{
	public:
		inline static void Convert( void *_Data, OpenWire::TPinCallback ACallback )
		{
			TComplex	AComplex( *(float *)_Data );
			TArray<Mitov::TComplex> AComplexBuffer( 1, &AComplex );
			ACallback( &AComplexBuffer );
		}

	};
//---------------------------------------------------------------------------
	class Convert_BinaryBlockToChar
	{
	public:
		static void Convert( void *_Data, OpenWire::TPinCallback ACallback )
		{
//			Serial.println( "TEST" );
			Mitov::TDataBlock ABlock = *(Mitov::TDataBlock *)_Data;
			char *APtr = (char *)ABlock.Data;
			int ASize = ABlock.Size;
//			Serial.println( ASize );
			while( ASize-- )
			{
//  				Serial.println( *APtr );
				ACallback( APtr++ );
//				if( OutputPin.IsConnected() )
//					Serial.println( "TEST2" );
			}
		}

	};
//---------------------------------------------------------------------------
	class Convert_BinaryBlockToText
	{
	public:
		static void Convert( void *_Data, OpenWire::TPinCallback ACallback )
		{
//			Serial.println( "TEST" );
			Mitov::TDataBlock ABlock = *(Mitov::TDataBlock *)_Data;
			char *APtr = (char *)ABlock.Data;
			int ASize = ABlock.Size;
			Mitov::String AText;
			AText.reserve( ASize );
//			Serial.println( ASize );
			while( ASize-- )
				AText += *APtr++;

			ACallback( (char *)AText.c_str() );
		}

	};	
//---------------------------------------------------------------------------
	class Convert_BinaryToClock
	{
	protected:
		bool	FLastValue = false;

	public:
		inline void Convert( void *_Data, OpenWire::TPinCallback ACallback )
		{
			bool AValue = *(bool *)_Data;
			if( AValue == FLastValue )
				return;

			FLastValue = AValue;
			if( AValue )
				ACallback( nullptr );

		}

	};
//---------------------------------------------------------------------------
	template<typename T_FROM, typename T_TO> class TypedConvert
	{
	public:
		inline static void Convert( void *_Data, OpenWire::TPinCallback ACallback )
		{
			T_TO AValue = T_TO( *(T_FROM *)_Data );
			ACallback( &AValue );
		}

	};
//---------------------------------------------------------------------------
	template<typename T_FROM> class TypedConvert<T_FROM, Mitov::String>
	{
	public:
		inline static void Convert( void *_Data, OpenWire::TPinCallback ACallback )
		{
			Mitov::String AValue = Mitov::String( *(T_FROM *)_Data );
			ACallback( (void *)AValue.c_str() );
		}

	};
//---------------------------------------------------------------------------
	class Convert_DigitalToText
	{
	public:
		inline static void Convert( void *_Data, OpenWire::TPinCallback ACallback )
		{
			if(*(bool*)_Data )
				ACallback( (void *)"true" );

			else
				ACallback( (void *)"false" );

		}

	};
//---------------------------------------------------------------------------
	class Convert_DateTimeToText
	{
	public:
		inline static void Convert( void *_Data, OpenWire::TPinCallback ACallback )
		{
			TDateTime &ADate = *(TDateTime *)_Data;
			ACallback( (void *)ADate.ToString().c_str());
		}

	};
//---------------------------------------------------------------------------
	class Convert_ColorToText
	{
	public:
		inline static void Convert( void *_Data, OpenWire::TPinCallback ACallback )
		{
			TColor &AColor= *(TColor *)_Data;
			ACallback( (void *)AColor.ToString().c_str());
		}

	};
//---------------------------------------------------------------------------
	class Convert_RGBWColorToText
	{
	public:
		inline static void Convert( void *_Data, OpenWire::TPinCallback ACallback )
		{
			TRGBWColor &AColor= *(TRGBWColor *)_Data;
			ACallback( (void *)AColor.ToString().c_str());
		}

	};
//---------------------------------------------------------------------------
	typedef Convert_RGBWColorToText Convert_AlphaColorToText;
//---------------------------------------------------------------------------
	namespace Func
	{
		const char PROGMEM b64_alphabet[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
		"abcdefghijklmnopqrstuvwxyz"
		"0123456789+/";

		inline void a3_to_a4(unsigned char * a4, unsigned char * a3) 
		{
			a4[0] = (a3[0] & 0xfc) >> 2;
			a4[1] = ((a3[0] & 0x03) << 4) + ((a3[1] & 0xf0) >> 4);
			a4[2] = ((a3[1] & 0x0f) << 2) + ((a3[2] & 0xc0) >> 6);
			a4[3] = (a3[2] & 0x3f);
		}

		inline void a4_to_a3(unsigned char * a3, unsigned char * a4) 
		{
			a3[0] = (a4[0] << 2) + ((a4[1] & 0x30) >> 4);
			a3[1] = ((a4[1] & 0xf) << 4) + ((a4[2] & 0x3c) >> 2);
			a3[2] = ((a4[2] & 0x3) << 6) + a4[3];
		}

		inline unsigned char b64_lookup(char c) 
		{
			if(c >='A' && c <='Z') 
				return c - 'A';

			if(c >='a' && c <='z') 
				return c - 71;

			if(c >='0' && c <='9') 
				return c + 4;

			if(c == '+') 
				return 62;

			if(c == '/') 
				return 63;

			return -1;
		}

		int Base64Encode(char *output, const char *input, int inputLen) 
		{
			int i = 0, j = 0;
			int encLen = 0;
			unsigned char a3[3];
			unsigned char a4[4];

			while(inputLen--) 
			{
				a3[i++] = *(input++);
				if(i == 3) 
				{
					a3_to_a4(a4, a3);

					for(i = 0; i < 4; i++)
						output[encLen++] = pgm_read_byte(&b64_alphabet[a4[i]]);

					i = 0;
				}
			}

			if(i) 
			{
				for(j = i; j < 3; j++)
					a3[j] = '\0';

				a3_to_a4(a4, a3);

				for(j = 0; j < i + 1; j++)
					output[encLen++] = pgm_read_byte(&b64_alphabet[a4[j]]);

				while((i++ < 3))
					output[encLen++] = '=';
				
			}

			output[encLen] = '\0';
			return encLen;
		}

		int Base64Decode(char * output, const char * input, int inputLen) 
		{
			int i = 0, j = 0;
			int decLen = 0;
			unsigned char a3[3];
			unsigned char a4[4];


			while (inputLen--) 
			{
				if(*input == '=')
					break;

				a4[i++] = *(input++);
				if (i == 4) 
				{
					for (i = 0; i <4; i++)
						a4[i] = b64_lookup(a4[i]);

					a4_to_a3(a3,a4);

					for (i = 0; i < 3; i++)
						output[decLen++] = a3[i];
					
					i = 0;
				}
			}

			if (i) 
			{
				for (j = i; j < 4; j++)
					a4[j] = '\0';				

				for (j = 0; j <4; j++)
					a4[j] = b64_lookup(a4[j]);

				a4_to_a3(a3,a4);

				for (j = 0; j < i - 1; j++)
					output[decLen++] = a3[j];
				
			}

			output[decLen] = '\0';
			return decLen;
		}

		inline int Base64EncodeLength(int plainLen) 
		{
			int n = plainLen;
			return (n + 2 - ((n + 2) % 3)) / 3 * 4;
		}

		int Base64DecodeLength(char * input, int inputLen)
		{
			int i = 0;
			int numEq = 0;
			for(i = inputLen - 1; input[i] == '='; i--) 
				numEq++;

			return ((6 * inputLen) / 8) - numEq;
		}

		Mitov::String Base64Encode( Mitov::String input ) 
		{
			int ALength = Base64EncodeLength( input.length() );
			Mitov::String AResult;
			AResult.reserve( ALength );
			for( int i = 0; i < ALength; ++i )
				AResult += " ";

			Base64Encode( (char *)AResult.c_str(), input.c_str(), input.length() );

			return AResult;
		}

		Mitov::String ByteListToString( Mitov::SimpleList<uint8_t, uint16_t> &AValue )
		{
			Mitov::String Result;
			Result.reserve( AValue.size());
			for( int i = 0; i < AValue.size(); ++i )
				Result += (char)AValue[ i ];

			return Result;
		}

// Pass 8-bit (each) R,G,B, get back 16-bit packed color
		inline uint16_t Color565(uint8_t r, uint8_t g, uint8_t b) 
		{
			return (uint16_t(r & 0xF8) << 8) | (uint16_t(g & 0xFC) << 3) | (b >> 3);
		}		
//---------------------------------------------------------------------------
		inline uint16_t Color565(uint32_t AColor ) 
		{
			uint8_t	r = AColor >> 16;
			uint8_t	g = AColor >> 8;
			uint8_t	b = AColor;
			return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
		}		
//---------------------------------------------------------------------------
		inline uint32_t ColorFrom565( uint16_t AColor ) 
		{
			return	0b000001110000001100000111 |
					(( uint32_t( AColor ) & 0xf800 ) << 8 ) |
					(( uint32_t( AColor ) & 0x07e0 ) << 5 ) |
					(( uint32_t( AColor ) & 0x001f ) << 3 );

		}		
//---------------------------------------------------------------------------
		void insert_sort(int array[], uint8_t size) 
		{
			uint8_t j;
			int save;
  
			for (int i = 1; i < size; i++) 
			{
				save = array[i];
				for (j = i; j >= 1 && save < array[j - 1]; j--)
					array[j] = array[j - 1];

				array[j] = save; 
			}
		}
//---------------------------------------------------------------------------
		inline uint8_t FromBcdToDec(const uint8_t bcd)
		{
			return (10 * ((bcd & 0xF0) >> 4) + (bcd & 0x0F));
		}
//---------------------------------------------------------------------------
		inline uint8_t FromDecToBcd(const uint8_t dec) 
		{
			const uint8_t tens = dec / 10;
			const uint8_t ones = dec % 10;
			return (tens << 4) | ones;
		}
//---------------------------------------------------------------------------
		uint8_t FromCharsToInt8( char *AValues, uint8_t ALength )
		{
			uint8_t AResult = 0;
			for( int i = 0; i < ALength; ++i )
				AResult = AResult * 10 + ( AValues[ i ] - '0' );

			return AResult;
		}
//---------------------------------------------------------------------------
		uint16_t FromCharsToInt16( char *AValues, uint8_t ALength )
		{
			uint16_t AResult = 0;
			for( int i = 0; i < ALength; ++i )
				AResult = AResult * 10 + ( AValues[ i ] - '0' );

			return AResult;
		}
//---------------------------------------------------------------------------
		inline float ConvertCtoF(float c) 
		{
			return c * 1.8 + 32;
		}
//---------------------------------------------------------------------------
		inline float ConvertFtoC(float f)
		{
			return (f - 32) * 0.55555;
		}
//---------------------------------------------------------------------------
		template<typename T>T MapRange( T x, T in_min, T in_max, T out_min, T out_max)
		{
			return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
		}
//---------------------------------------------------------------------------
		bool ExtractOptionallyQuotedCommaText( Mitov::String &AText, Mitov::String &AResult )
		{
			if( AText.length() == 0 )
				return false;

			bool	AStartsWithQuote;
//			int		AStartIndex;

			if( AText[ 0 ] == '"' )
			{
				AStartsWithQuote = true;
//				AStartIndex = 1;
			}

			else
			{
				AStartsWithQuote = false;
//				AStartIndex = 0;
			}

			if( ! AStartsWithQuote )
			{
				int APos = AText.indexOf( "," );
				if( APos < 0 )
				{
					AResult = AText;
					AText = "";
					return true;
				}

				AResult = AText.substring( 0, APos );
				AText.remove( 0, APos + 1 );
				return true;
			}

			AResult = "";
			int ASubStartsWithQuote = 0;
			uint32_t ALength = 0;
			while( ALength < AText.length() )
			{
				if( AText[ ASubStartsWithQuote ] == ',' )
					break;

				if( AText[ ASubStartsWithQuote ] != '"' )
					return false;

				++ASubStartsWithQuote;

	//			Serial.print( "ASubStartsWithQuote: " ); Serial.println( ASubStartsWithQuote );
				int APos = AText.indexOf( "\"", ASubStartsWithQuote );
				if( APos < 0 )
					return false;

	//			Serial.println( APos );
				AResult += AText.substring( ASubStartsWithQuote, APos );

				ASubStartsWithQuote = APos + 1;
				ALength = APos + 2;
			}

			AText.remove( 0, ALength );

			return true;
		}
//---------------------------------------------------------------------------
		bool ContainsTextAtPos( Mitov::String ALine, int APos, Mitov::String ASubText )
		{
			if( ALine.length() < APos + ASubText.length() )
				return false;

			for( int i = 0; i < int( ASubText.length()); ++i )
				if( ALine[ APos + i ] != ASubText[ i ] )
					return false;

			return true;
		}
//---------------------------------------------------------------------------
/*
		uint8_t Exponent(uint16_t value)
		{
			// Calculates the base 2 logarithm of a value
			uint8_t result = 0;
			while (((value >> result) & 1) != 1) 
				result++;
			return(result);
		}
*/
//---------------------------------------------------------------------------
		bool SetBitField( uint8_t *AValuesArray, int AIndex, bool AValue )
		{
			uint8_t AMask = 1 << ( AIndex & 0b111 ); // same as % 8
			uint8_t	AArrayIndex = AIndex / 8;
			if( AValue == (( AValuesArray[ AArrayIndex ] & AMask ) != 0 ))
				return false;

			if( AValue )
				AValuesArray[ AArrayIndex ] |= AMask;

			else
				AValuesArray[ AArrayIndex ] &= ~AMask;

			return true;
		}
//---------------------------------------------------------------------------
		inline uint32_t SwapRB( uint32_t AColor )
		{
			return ( ( AColor & 0xFF ) << 16 ) | ( AColor & 0x00FF00 ) | ( ( AColor >> 16 ) & 0xFF );
		}
	}
//---------------------------------------------------------------------------
	template <typename T> T ConstrainToList( T AValue, const T AList[], int ASize )
	{
		int AIndex = -1;
		for( int i = 0; i < ASize; ++i )
			if( AValue <= AList[ i ] )
			{
				AIndex = i;
				break;
			}

		if( AIndex == -1 )
			return AList[ ASize - 1 ];

		return AList[ AIndex ];
	}
//---------------------------------------------------------------------------
	class InterruptLock 
	{
	public:
		InterruptLock()
		{
#ifndef VISUINO_FREESOC2
			noInterrupts();
#endif
		}

		~InterruptLock() 
		{
#ifndef VISUINO_FREESOC2
			interrupts();
#endif
		}
	};
//---------------------------------------------------------------------------
	class StringEx : public Mitov::String
	{
		typedef	Mitov::String	inherited;

	public:
		using inherited::inherited;
		StringEx( const char *AStr, int ALength )
		{
			copy( AStr, ALength );
		}

	};
//---------------------------------------------------------------------------
	template<typename T_IMPLEMENTATION_CLASS, typename T> class UnaryOperation
	{
	public:
		OpenWire::SourcePin	OutputPin;

    public:
		inline void InputPin_o_Receive( void *_Data )
        {
            OutputPin.SendValue( T_IMPLEMENTATION_CLASS::CalculateOutput( *(T*)_Data ) ); // Do not use Notify for String compatibility!
        }

	};
//---------------------------------------------------------------------------
	template<typename T_IMPLEMENTATION_CLASS, typename T> class UnaryOperation_Enable
	{
	public:
		OpenWire::SourcePin	OutputPin;

    public:
        bool Enabled = true;

    public:
		void InputPin_o_Receive( void *_Data )
        {
            if( ! Enabled )
            {
			    OutputPin.Notify( _Data );
                return;
            }

            OutputPin.SendValue( T_IMPLEMENTATION_CLASS::CalculateOutput( *(T*)_Data ) ); // Do not use Notify for String compatibility!
        }

	};
//---------------------------------------------------------------------------
	template<typename T_IMPLEMENTATION_CLASS, typename T, typename T_PARAM1> class UnaryOperation_Param1
	{
	public:
		OpenWire::SourcePin	OutputPin;

    public:
        T   Value;

    public:
		inline void InputPin_o_Receive( void *_Data )
        {
            OutputPin.SendValue( T_IMPLEMENTATION_CLASS::CalculateOutput( *(T*)_Data, Value ) ); // Do not use Notify for String compatibility!
        }

    public:
        UnaryOperation_Param1( T AValue ) : 
			Value( AValue )
		{
		}

	};
//---------------------------------------------------------------------------
	template<typename T_IMPLEMENTATION_CLASS, typename T, typename T_PARAM1> class UnaryOperation_Param1_Enable
	{
	public:
		OpenWire::SourcePin	OutputPin;

    public:
        bool Enabled = true;

    public:
        T   Value;

    public:
		void InputPin_o_Receive( void *_Data )
        {
            if( ! Enabled )
            {
			    OutputPin.Notify( _Data );
                return;
            }

            OutputPin.SendValue( T_IMPLEMENTATION_CLASS::CalculateOutput( *(T*)_Data, Value ) ); // Do not use Notify for String compatibility!
        }

    public:
        UnaryOperation_Param1_Enable( T AValue ) : 
			Value( AValue )
		{
		}

	};
//---------------------------------------------------------------------------
	template <uint32_t C_UNIQUE> class ClockPin_NoImplementation
	{
	protected:
		inline void ClockPin()
		{
		}

		inline OpenWire::SourcePin & GetPinInstance()
		{
			return * (OpenWire::SourcePin *)nullptr;
		}
	};
//---------------------------------------------------------------------------
	template <uint32_t C_UNIQUE> class ClockPin_StandardPinImplementation
	{
	private:
		OpenWire::SourcePin	_GenericOutputPin;

	protected:
		inline void ClockPin()
		{
			_GenericOutputPin.Notify( nullptr );
		}

		inline OpenWire::SourcePin & GetPinInstance()
		{
			return _GenericOutputPin;
		}
	};
//---------------------------------------------------------------------------
	template <uint32_t C_UNIQUE, typename T_DATA> class TypedPin_NoImplementation
	{
	protected:
		inline void SetPinValue( T_DATA AValue, bool AChangeOnly = false )
		{
		}

		inline OpenWire::SourcePin & GetPinInstance()
		{
			return * (OpenWire::SourcePin *)nullptr;
		}
	};
//---------------------------------------------------------------------------
	template <uint32_t C_UNIQUE, typename T_DATA> class TypedPin_StandardPinImplementation
	{
	private:
		OpenWire::SourcePin	_GenericOutputPin;

	protected:
		inline void SetPinValue( T_DATA AValue, bool AChangeOnly = false )
		{
			_GenericOutputPin.template SendValue<T_DATA>( AValue );
		}

		inline OpenWire::SourcePin & GetPinInstance()
		{
			return _GenericOutputPin;
		}
	};
//---------------------------------------------------------------------------
	template <uint32_t C_UNIQUE> class DigitalPin_NoImplementation
	{
	protected:
		inline void SetPinValue( bool AValue, bool AChangeOnly = false )
		{
		}

		inline void SetPinValueHigh()
		{
		}

		inline void SetPinValueLow()
		{
		}

		inline bool GetPinValue()
		{
			return false;
		}

		inline OpenWire::SourcePin & GetPinInstance()
		{
			return * (OpenWire::SourcePin *)nullptr;
		}

	};
//---------------------------------------------------------------------------
	template <uint32_t C_UNIQUE> class DigitalPin_StandardPinImplementation
	{
	private:
		OpenWire::SourcePin	_GenericOutputPin;

	protected:
		inline void SetPinValue( bool AValue, bool AChangeOnly = false )
		{
			_GenericOutputPin.SendValue( AValue );
		}

		inline void SetPinValueHigh()
		{
			_GenericOutputPin.SendValue( true );
		}

		inline void SetPinValueLow()
		{
			_GenericOutputPin.SendValue( false );
		}

		inline bool GetPinValue()
		{
			return false;
		}

		inline OpenWire::SourcePin & GetPinInstance() 
		{
			return _GenericOutputPin;
		}

/*
	public:
		OpenWire::SourcePin	ChipSelectOutputPin;

	protected:
		inline void SetPinValue( bool AValue, bool AChangeOnly = false )
		{
			ChipSelectOutputPin.SendValue( AValue );
		}
*/
	};
//---------------------------------------------------------------------------
	template <uint32_t C_UNIQUE> class DigitalPin_StandardPinImplementation_ChangeOnly
	{
	private:
		OpenWire::TypedSourcePin<bool>	_GenericOutputPin;

	protected:
		inline void SetPinValue( bool AValue, bool AChangeOnly )
		{
			_GenericOutputPin.SetValue( AValue, AChangeOnly );
		}

		inline void SetPinValueHigh()
		{
			_GenericOutputPin.SendValue( true );
		}

		inline void SetPinValueLow()
		{
			_GenericOutputPin.SendValue( false );
		}

		inline bool GetPinValue()
		{
			return false;
		}

		inline OpenWire::SourcePin & GetPinInstance() 
		{
			return _GenericOutputPin;
		}

	};
//---------------------------------------------------------------------------
	template<typename T_PIN, T_PIN &C_PIN> class DigitalPin_DirectPinImplementation
	{
	protected:
		inline void SetPinValueHigh()
		{
			C_PIN.DigitalWrite( true );
		}

		inline void SetPinValueLow()
		{
			C_PIN.DigitalWrite( false );
		}

		inline void SetPinValue( bool AValue, bool AChangeOnly = false )
		{
			C_PIN.DigitalWrite( AValue );
		}

		inline bool GetPinValue()
		{			
			return C_PIN.DigitalRead();
		}

		inline OpenWire::SourcePin & GetPinInstance() 
		{
			return * (OpenWire::SourcePin *)nullptr;
		}

	public:
		DigitalPin_DirectPinImplementation()
		{
			C_PIN.SetMode( OUTPUT );
		}
	};
//---------------------------------------------------------------------------
	template<uint8_t C_PIN_NUMBER> class DigitalPin_DirectBoardPinImplementation
	{
	protected:
		inline void SetPinValueHigh()
		{
//			Serial.print( C_PIN_NUMBER ); Serial.println( " = True" );
			digitalWrite( C_PIN_NUMBER, HIGH );
		}

		inline void SetPinValueLow()
		{
//			Serial.print( C_PIN_NUMBER ); Serial.println( " = False" );
			digitalWrite( C_PIN_NUMBER, LOW );
		}

		inline void SetPinValue( bool AValue, bool AChangeOnly = false )
		{
//			Serial.print( C_PIN_NUMBER ); Serial.print( " = " ); Serial.println( AValue );
			digitalWrite( C_PIN_NUMBER, AValue ? HIGH : LOW );
		}

		inline bool GetPinValue()
		{			
			pinMode( C_PIN_NUMBER, INPUT );
			bool AValue = ( digitalRead( C_PIN_NUMBER ) != LOW );
			pinMode( C_PIN_NUMBER, OUTPUT );

			return AValue;
		}

		inline OpenWire::SourcePin & GetPinInstance()
		{
			return * (OpenWire::SourcePin *)nullptr;
		}

	public:
		DigitalPin_DirectBoardPinImplementation()
		{
			pinMode( C_PIN_NUMBER, OUTPUT );
		}
	};
//---------------------------------------------------------------------------
	template <uint32_t C_UNIQUE> class AnalogPin_NoImplementation
	{
	protected:
		inline void SetPinValue( float AValue, bool AChangeOnly = false )
		{
		}

		inline OpenWire::SourcePin & GetPinInstance() 
		{
			return * (OpenWire::SourcePin *)nullptr;
		}

	};
//---------------------------------------------------------------------------
	template <uint32_t C_UNIQUE> class AnalogPin_StandardPinImplementation
	{
	private:
		OpenWire::SourcePin	_GenericOutputPin;

	protected:
		inline void SetPinValue( float AValue, bool AChangeOnly = false )
		{
			_GenericOutputPin.SendValue( AValue );
		}

		inline OpenWire::SourcePin & GetPinInstance() 
		{
			return _GenericOutputPin;
		}
	};
//---------------------------------------------------------------------------
	template<typename T_PIN, T_PIN &C_PIN> class AnalogPin_DirectPinImplementation
	{
	protected:
		inline void SetPinValue( float AValue, bool AChangeOnly = false )
		{
			C_PIN.AnalogWrite( AValue );
		}

/*
	public:
		AnalogPin_DirectPinImplementation()
		{
			C_PIN.SetMode( OUTPUT );
		}
*/
	};
//---------------------------------------------------------------------------
	template<uint8_t C_PIN_NUMBER> class AnalogPin_DirectBoardPinImplementation
	{
	protected:
		inline void SetPinValue( float AValue, bool AChangeOnly = false )
		{
			analogWrite( C_PIN_NUMBER, ( AValue * PWMRANGE ) + 0.5 );
		}

	public:
		AnalogPin_DirectBoardPinImplementation()
		{
			pinMode( C_PIN_NUMBER, OUTPUT );
		}
	};
//---------------------------------------------------------------------------
	class ResetPinImplementation
	{
	public:
		template<typename T> inline static void ExecuteReset( T *AInstance, uint16_t ADelay )
		{
			AInstance->SetResetValueLow();
			delay( ADelay );
			AInstance->SetResetValueHigh();
		}

		template<typename T> inline static void ExecuteReset( T *AInstance, uint16_t ADelayHigh, uint16_t ADelayLow )
		{
			AInstance->SetResetValueHigh();
			delayMicroseconds( ADelayHigh );
			AInstance->SetResetValueLow();
			delay( ADelayLow );
			AInstance->SetResetValueHigh();
		}

		template<typename T> inline static void ExecuteReset( T *AInstance, uint16_t ADelayHigh, uint16_t ADelayLow, uint16_t APostDelay )
		{
			AInstance->SetResetValueHigh();
			delayMicroseconds( ADelayHigh );
			AInstance->SetResetValueLow();
			delay( ADelayLow );
			AInstance->SetResetValueHigh();
			delay( APostDelay );
		}

/*
	public:
		template<typename T> static void ExecuteMicrosReset( T *AInstance, uint16_t ADelayHigh, uint16_t ADelayLow )
		{
			delayMicroseconds( ADelayHigh );
			AInstance->SetResetValueLow();
			delayMicroseconds( ADelayLow );
			AInstance->SetResetValueHigh();
		}
*/
	};
//---------------------------------------------------------------------------
	class NoResetImplementation
	{
	public:
		template<typename T> inline static void ExecuteReset( T *AInstance, uint16_t ADelay )
		{
		}

		template<typename T> inline static void ExecuteReset( T *AInstance, uint16_t ADelayHigh, uint16_t ADelayLow )
		{
		}

		template<typename T> inline static void ExecuteReset( T *AInstance, uint16_t ADelayHigh, uint16_t ADelayLow, uint16_t APostDelay )
		{
		}

};
//---------------------------------------------------------------------------
} // Mitov

namespace ComponentsHardware
{
	extern void SystemUpdateHardware();
} // ComponentsHardware
//---------------------------------------------------------------------------
#endif

