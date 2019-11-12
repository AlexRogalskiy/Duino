////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2018 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _OPENWIRE_h
#define _OPENWIRE_h

#include <Mitov_SimpleList.h>

#if defined( VISUINO_MAPLE_MINI ) || defined( VISUINO_NUCLEO_F303RE )
	#define VISUINO_STMDUINO
#endif


#if defined( VISUINO_CHIPKIT_DP32 ) || defined( VISUINO_CHIPKIT_LENNY )
	#ifndef digitalPinToInterrupt
		#ifndef NOT_AN_INTERRUPT
			#define NOT_AN_INTERRUPT -1
		#endif

		#define digitalPinToInterrupt(p) ((p) == PIN_INT0 ? 0 : ((p) == PIN_INT1 ? 1 : ((p) == PIN_INT2 ? 2 : ((p) == PIN_INT3 ? 3 : ((p) == PIN_INT4 ? 4 : NOT_AN_INTERRUPT)))))
	#endif
#endif

#if defined( VISUINO_CHIPKIT_MAX32 )
	#define NOT_AN_INTERRUPT -1
	#define digitalPinToInterrupt(p) ((p) == 2 ? 1 : ((p) == 7 ? 2 : ((p) == 21 ? 3 : ((p) == 20 ? 4 : ((p) == 3 ? 0 : NOT_AN_INTERRUPT)))))

#endif

#if ( defined( VISUINO_STMDUINO ) || defined( VISUINO_TRINKET ) || defined( VISUINO_LINKIT_ONE ) || defined( VISUINO_INTEL_EDISON ) )

	#ifdef VISUINO_NUCLEO_F303RE
		#undef digitalPinToInterrupt
	#endif

	#ifndef digitalPinToInterrupt
		#define digitalPinToInterrupt(p) (p)
	#endif

#endif

#if __cplusplus <= 199711L
	#define constexpr
#endif
//		#define digitalPinToInterrupt(p) (p)

#if defined( VISUINO_MAPLE_MINI )
//	char * ltoa(long l, char * buffer, int radix)
//	{
//		return itoa( l, buffer, radix );
//	}
#endif

#if ( defined( VISUINO_FREESOC2 ) || defined( VISUINO_M0 ) || defined( VISUINO_CHIPKIT ) || defined( VISUINO_INTEL_EDISON ) )
  #if ! defined( __CORE_CM3_PSOC5_H__ )
//    #define PIN_MODE uint32_t
  #endif

namespace Mitov
{
	char *dtostrf (double val, signed char width, unsigned char prec, char *sout) 
	{
		char fmt[20];
		sprintf(fmt, "%%%d.%df", width, prec);
		sprintf(sout, fmt, val);
		return sout;
	}
}
#endif

namespace Mitov
{
#ifdef VISUINO_INTEL_EDISON
	class String : public ::String
	{
		typedef ::String inherited;

	public:
		char *c_str() { return buffer; }

	public:
		String & operator += (const String &rhs)	{concat(rhs); return (*this);}
		String & operator += (const char *cstr)		{concat(cstr); return (*this);}
		String & operator += (char c)			{concat(c); return (*this);}
		String & operator += (unsigned char num)       	{concat(num); return (*this);}
		String & operator += (int num)			{concat(num); return (*this);}
		String & operator += (unsigned int num)		{concat(num); return (*this);}
		String & operator += (long num)			{concat(num); return (*this);}
		String & operator += (unsigned long num)	{concat(num); return (*this);}


		// Implement StringAdditionOperator per Arduino docs... String + __
		String operator + (const String &rhs)	{return String(*this) += rhs;}
		String operator + (const char *cstr)	{return String(*this) += cstr;}
		String operator + (char c)		{return String(*this) += c;}
		String operator + (unsigned char num)	{return String(*this) += num;}
		String operator + (int num)		{return String(*this) += num;}
		String operator + (unsigned int num)	{return String(*this) += num;}
		String operator + (long num)		{return String(*this) += num;}
		String operator + (unsigned long num)	{return String(*this) += num;}

	public:
		void remove(unsigned int index)
		{
			// Pass the biggest integer as the count. The remove method
			// below will take care of truncating it at the end of the
			// string.
			remove(index, (unsigned int)-1);
		}

		void remove(unsigned int index, unsigned int count)
		{
			if (index >= len)
				return;

			if (count <= 0) 
				return;

			if (count > len - index) 
				count = len - index;

			char *writeTo = buffer + index;
			len = len - count;
			strncpy(writeTo, buffer + index + count,len - index);
			buffer[len] = 0;
		}

	public:
		using inherited::inherited;

		String(const char *cstr = "")
		{
			init();
			if (cstr) 
				copy(cstr, strlen(cstr));
		}

		String(const ::String &str)
		{
			init();
			*this = str;
		}

		explicit String(float value, unsigned char decimalPlaces=2)
		{
			init();
			char buf[33];
			*this = dtostrf(value, (decimalPlaces + 2), decimalPlaces, buf);
		}

		explicit String(double value, unsigned char decimalPlaces=2)
		{
			init();
			char buf[33];
			*this = dtostrf(value, (decimalPlaces + 2), decimalPlaces, buf);
		}

	};

#else
	typedef ::String String;
#endif
//	class String : public ::String
//	{
//	};
};

namespace OpenWire
{
	class StreamPin;
//---------------------------------------------------------------------------	
	class Object
	{
	};
//---------------------------------------------------------------------------	
	typedef Object InterruptObject;
//---------------------------------------------------------------------------	
	typedef void (Object::*TOnInterrupt)( bool );
//---------------------------------------------------------------------------
	typedef void (Object::*TOnPinReceive) ( void *_Data );
//---------------------------------------------------------------------------
	typedef void (Object::*TOnPinIndexedReceive) ( int AIndex, void *_Data );
//---------------------------------------------------------------------------
	typedef void (*TPinCallback)( void *_Data );
//---------------------------------------------------------------------------
	class StreamPin : public Object
	{
	public:
		TPinCallback FCallback = nullptr;

	public:
		inline StreamPin & operator ()() { return *this; }

	public:
		template<typename T> void SendValue( T AValue )
		{
			Notify( &AValue );
		}

		inline void SetCallback( TPinCallback ACallback )
		{
			FCallback = ACallback;
		}

	public:
		inline bool IsConnected()
		{
			return ( FCallback != nullptr );
		}

	public:
		void Notify( void *_data )
		{
			if( FCallback )
				FCallback( _data );
			
		}
	};
//---------------------------------------------------------------------------
	template<> void StreamPin::SendValue<Mitov::String>( Mitov::String AValue )
	{
		Notify( (char *)AValue.c_str() );
	}
//---------------------------------------------------------------------------
	template<> inline void StreamPin::SendValue<const char *>( const char *AValue )
	{
		Notify( (char *)AValue );
	}
//---------------------------------------------------------------------------
	typedef StreamPin SourcePin;
//---------------------------------------------------------------------------
	template<typename T> class TypedSourcePin : public SourcePin
	{
		typedef SourcePin inherited;

	public:
		T	Value;

	public:
		void SetValue( T AValue, bool AChangeOnly )
		{
			if( AChangeOnly )
				if( AValue == Value )
					return;

			Value = AValue;
			Notify( &Value );
		}

	};
//---------------------------------------------------------------------------
	template<> class TypedSourcePin<Mitov::String> : public SourcePin
	{
		typedef SourcePin inherited;

	public:
		Mitov::String	Value;

	public:
		void SetValue( Mitov::String AValue, bool AChangeOnly )
		{
			if( AChangeOnly )
				if( AValue == Value )
					return;

			Value = AValue;
			SendValue( Value );
		}

	};
//---------------------------------------------------------------------------
	template<typename T> class TypedStartSourcePin : public SourcePin
	{
		typedef SourcePin inherited;

	public:
		T	Value;

	protected:
		bool	FStarted = false;

	public:
		void SetValue( T AValue, bool ASendAll = false )
		{
			if( ! FStarted )
				FStarted = true;

			else
			{
				if( ! ASendAll )
					if( AValue == Value )
						return;
			}

			Value = AValue;
			Notify( &Value );
		}

	};
//---------------------------------------------------------------------------
	class PinRaisingInterrupt
	{
	protected:
		struct THandlers
		{
			Object			*Instance;
			TOnInterrupt	Callback;

		public:
			THandlers()
			{
			}

			THandlers( Object *AInstance, TOnInterrupt ACallback ) :
				Instance( AInstance ),
				Callback( ACallback )
			{
			}
		};

	protected:
		Mitov::SimpleList<THandlers> FHandlersList;

	public:
		void InterruptHandler()
		{
			for( int i = 0; i < FHandlersList.size(); ++i )
				( FHandlersList[ i ].Instance->*FHandlersList[ i ].Callback )( true );
				
		}

	public:
		void SubscribeRising( Object *AInstance, TOnInterrupt ACallback )
		{
			FHandlersList.push_back( THandlers( AInstance, ACallback ));
		}

	public:
		PinRaisingInterrupt( int AInterruptPin, void (*AInterruptRoutine)() )
		{
			// Add by Adrien van den Bossche <vandenbo@univ-tlse2.fr> for Teensy
			// ARM M4 requires the below. else pin interrupt doesn't work properly.
			// On all other platforms, its innocuous, belt and braces
			pinMode( AInterruptPin, INPUT ); 

			int AInterruptNumber = digitalPinToInterrupt( AInterruptPin );
			attachInterrupt( AInterruptNumber, AInterruptRoutine, RISING );
		}
	};
//---------------------------------------------------------------------------
	class PinFallingInterrupt
	{
	protected:
		struct THandlers
		{
			Object			*Instance;
			TOnInterrupt	Callback;

		public:
			THandlers()
			{
			}

			THandlers( Object *AInstance, TOnInterrupt ACallback ) :
				Instance( AInstance ),
				Callback( ACallback )
			{
			}
		};

	protected:
		Mitov::SimpleList<THandlers> FHandlersList;

	public:
		void InterruptHandler()
		{
			for( int i = 0; i < FHandlersList.size(); ++i )
				( FHandlersList[ i ].Instance->*FHandlersList[ i ].Callback )( false );
				
		}

	public:
		void SubscribeFalling( Object *AInstance, TOnInterrupt ACallback )
		{
			FHandlersList.push_back( THandlers( AInstance, ACallback ));
		}

	public:
		PinFallingInterrupt( int AInterruptPin, void (*AInterruptRoutine)() )
		{
			// Add by Adrien van den Bossche <vandenbo@univ-tlse2.fr> for Teensy
			// ARM M4 requires the below. else pin interrupt doesn't work properly.
			// On all other platforms, its innocuous, belt and braces
			pinMode( AInterruptPin, INPUT ); 

			int AInterruptNumber = digitalPinToInterrupt( AInterruptPin );
			attachInterrupt( AInterruptNumber, AInterruptRoutine, FALLING );
		}
	};
//---------------------------------------------------------------------------
	class PinChangeInterrupt
	{
	protected:
		struct THandlers
		{
			Object			*Instance;
			TOnInterrupt	Callback;

		public:
			THandlers()
			{
			}

			THandlers( Object *AInstance, TOnInterrupt ACallback ) :
				Instance( AInstance ),
				Callback( ACallback )
			{
			}
		};

	protected:
		int FPinNumber;
		Mitov::SimpleList<THandlers> FRisingHandlersList;
		Mitov::SimpleList<THandlers> FFallingHandlersList;

	public:
		void InterruptHandler()
		{
			if( digitalRead( FPinNumber ) == HIGH )
			{
				for( int i = 0; i < FRisingHandlersList.size(); ++i )
					( FRisingHandlersList[ i ].Instance->*FRisingHandlersList[ i ].Callback )( true );
			}
			else
			{
				for( int i = 0; i < FFallingHandlersList.size(); ++i )
					( FFallingHandlersList[ i ].Instance->*FFallingHandlersList[ i ].Callback )( false );
			}
		}

	public:
		void SubscribeRising( Object *AInstance, TOnInterrupt ACallback )
		{
			FRisingHandlersList.push_back( THandlers( AInstance, ACallback ));
		}

		void SubscribeFalling( Object *AInstance, TOnInterrupt ACallback )
		{
			FFallingHandlersList.push_back( THandlers( AInstance, ACallback ));
		}

		void SubscribeChange( Object *AInstance, TOnInterrupt ACallback )
		{
			FRisingHandlersList.push_back( THandlers( AInstance, ACallback ));
			FFallingHandlersList.push_back( THandlers( AInstance, ACallback ));
		}

	public:
		PinChangeInterrupt( int AInterruptPin, void (*AInterruptRoutine)() ) :
			FPinNumber( AInterruptPin )
		{
			// Add by Adrien van den Bossche <vandenbo@univ-tlse2.fr> for Teensy
			// ARM M4 requires the below. else pin interrupt doesn't work properly.
			// On all other platforms, its innocuous, belt and braces
			pinMode( AInterruptPin, INPUT ); 

			int AInterruptNumber = digitalPinToInterrupt( AInterruptPin );
			attachInterrupt( AInterruptNumber, AInterruptRoutine, CHANGE );
		}
	};	 
//---------------------------------------------------------------------------
}

#endif
