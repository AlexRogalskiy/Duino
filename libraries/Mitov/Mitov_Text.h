////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2018 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_TEXT_h
#define _MITOV_TEXT_h

#include <Mitov.h>

#ifdef __TEST_FOR_DEBUG_PRINTS__
#define Serial UNGUARDED DEBUG PRINT!!!
#endif

namespace Mitov
{
//---------------------------------------------------------------------------
	class TextStateValue;
//---------------------------------------------------------------------------
	class TextValue
	{
	public:
		OpenWire::SourcePin	OutputPin;

	public:
		Mitov::String Value;

	public:
		void ExternalSetValue( Mitov::String AValue )
		{
			if( Value == AValue )
				return;

			Value = AValue;
			OutputPin.Notify( (void *)Value.c_str() );
		}

	public:
		inline void SystemInit() // Placeholder for inherited classes
		{
		}

		inline void SystemStart()
		{
			OutputPin.Notify( (void *)Value.c_str() );
		}

		inline void SystemLoopBegin( unsigned long currentMicros ) // Placeholder for inherited classes
		{
		}

	public:
		inline void ClockInputPin_o_Receive( void *_Data )
		{
			OutputPin.Notify( (void *)Value.c_str() );
		}

	public:
		TextValue( char *AValue ) :
			Value( AValue )
		{
		}

	};
//---------------------------------------------------------------------------
	class BindableTextValue : public TextValue
	{
		typedef TextValue inherited;

	protected:
		Mitov::String OldValue = inherited::Value;

	public:
		inline void SystemInit()
		{
			OldValue = inherited::Value;
		}

		inline void SystemLoopBegin( unsigned long currentMicros )
		{
			if( inherited::Value == OldValue )
				return;

			OldValue = inherited::Value;
			inherited::OutputPin.Notify( (void *)OldValue.c_str() );
		}

	public:
		BindableTextValue( char *AValue ) :
			inherited( AValue ),
			OldValue( AValue )
		{
		}

	};
//---------------------------------------------------------------------------
	template<typename T_OWNER, T_OWNER &C_OWNER> class ArduinoSetTextValueElement
	{
	public:
		Mitov::String	Value;

	public:
		inline void InputPin_o_Receive( void *_Data )
		{
			C_OWNER.ExternalSetValue( Value );
		}

	};
//---------------------------------------------------------------------------
	template<typename T_OWNER> class ArduinoStateSetTextValueElementBasic
	{
	protected:
		bool	FInputValue = false;

	public:
		Mitov::String	Value;

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
	template<typename T_OWNER, T_OWNER &C_OWNER> class ArduinoStateSetTextValueElement : public ArduinoStateSetTextValueElementBasic<T_OWNER>
	{
		typedef ArduinoStateSetTextValueElementBasic<T_OWNER> inherited;

	public:
		void InputPin_o_Receive( void *_Data )
		{
			if( inherited::FInputValue == *(bool *)_Data )
				return;

			inherited::FInputValue = *(bool *)_Data;
			if( inherited::FInputValue )
				C_OWNER.ExternalSetValue( inherited::Value );

			else
				C_OWNER.ResetValue();
		}

	public:
		ArduinoStateSetTextValueElement()
		{
			C_OWNER.RegisterElement( this );
		}

	};
//---------------------------------------------------------------------------
	class TextStateValue : public TextValue
	{
		typedef Mitov::TextValue inherited;

	protected:
		Mitov::String FCurrentValue;

	protected:
		Mitov::SimpleList<ArduinoStateSetTextValueElementBasic<TextStateValue> *>	FElements;

	public:
		void ExternalSetValue( Mitov::String AValue )
		{
			if( FCurrentValue == AValue )
				return;

			FCurrentValue = AValue;
			OutputPin.Notify( (void *)FCurrentValue.c_str() );
		}

		void ResetValue()
		{
			for( int i = FElements.size(); i -- ; )
				if( FElements[ i ]->TryActive( this ) )
					return;

			FCurrentValue = Value;
			OutputPin.Notify( (void *)FCurrentValue.c_str() );
		}

	public:
		void RegisterElement( ArduinoStateSetTextValueElementBasic<TextStateValue> *AValue )
		{
			FElements.push_back( AValue );
		}

	public:
		inline void SystemStart()
		{
			FCurrentValue = Value;
			inherited::SystemStart();
//			if( ! ClockInputPin_o_IsConnected )
//				OutputPin.Notify( (void *)FCurrentValue.c_str() );

		}

	public:
		inline void ClockInputPin_o_Receive( void *_Data )
		{
			OutputPin.Notify( (void *)FCurrentValue.c_str() );
		}

	public:
		using inherited::inherited;

	};
//---------------------------------------------------------------------------
	class BindableTextStateValue : public TextStateValue
	{
		typedef TextValue inherited;

	protected:
		Mitov::String OldValue;

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
			ResetValue();
//			inherited::OutputPin.Notify( (void *)OldValue.c_str() );
		}

	public:
		using inherited::inherited;

	};
//---------------------------------------------------------------------------
//	class BasicFormattedText;
//---------------------------------------------------------------------------
	template<typename T_OWNER> class FormattedTextElementBasic
	{
	public:
		bool	FModified	: 1; //= false;
		bool	FPopulated	: 1; //= false;

	public:
		virtual Mitov::String GetText() = 0;

	public:
		FormattedTextElementBasic( T_OWNER &AOwner, uint16_t AIndex ) :
			FModified( false ),
			FPopulated( false )
		{
			AOwner.FElements[ AIndex ] = this;
		}

	};
//---------------------------------------------------------------------------
/*
	class BasicFormattedText
	{
	public:
		OpenWire::SourcePin	OutputPin;

	public:
		bool	ClockInputPin_o_IsConnected = false;

	public:
		Mitov::SimpleList<FormattedTextElementBasic *>	FElements;

	};
*/
//---------------------------------------------------------------------------
	template <typename T_INSTANCE, typename T_STRING, uint16_t C_COUNT> class BasicTypedFormattedText
	{
//		typedef Mitov::BasicFormattedText inherited;

	public:
		OpenWire::SourcePin	OutputPin;

	public:
		bool	ClockInputPin_o_IsConnected = false;

	public:
		FormattedTextElementBasic<T_INSTANCE> *	FElements[ C_COUNT ];

	protected:
		struct TStringItem
		{
			T_STRING	Text;
			FormattedTextElementBasic<T_INSTANCE> *Element;
		};

	protected:
		Mitov::SimpleList<TStringItem *>	FReadyElements;

	public:
		void AddReadyElement( T_STRING ATextItem, int AIndex )
		{
//			Serial.print( "AddElement: " ); Serial.print( ATextItem ); Serial.println( AIndex );

			TStringItem	*AItem = new TStringItem;
			AItem->Text = ATextItem;
//			Serial.print( "AddElement: " ); Serial.println( AItem->Text );
			if( AIndex < C_COUNT )
				AItem->Element = FElements[ AIndex ];

			else
				AItem->Element = nullptr;

			FReadyElements.push_back( AItem );

//			Serial.println( FReadyElements[ FReadyElements.size() - 1 ]->Text );
//			Serial.println( "DEBUG>>" );
//			for( Mitov::SimpleList<TStringItem *>::iterator Iter = FReadyElements.begin(); Iter != FReadyElements.end(); ++Iter )
//				Serial.println( ( *Iter )->Text );

//			Serial.println( "<<DEBUG" );
		}

	protected:
		void ProcessSendOutput()
		{
//			Serial.println( "ProcessSendOutput" );
			Mitov::String AText;
			for( int i = 0; i < FReadyElements.size(); ++i )
			{
				AText += FReadyElements[ i ]->Text;
				FormattedTextElementBasic<T_INSTANCE> *AElement = FReadyElements[ i ]->Element;
				if( AElement )
				{
//					Serial.println( AElement->GetText() );
					AText += AElement->GetText();
//					Serial.println( AText );
					AElement->FModified = false;
				}
			}

//			Serial.println( AText );
			OutputPin.Notify( (void *)AText.c_str() );
		}

	public:
		inline void SystemLoopEnd( unsigned long currentMicros )
		{
			bool AModified = false;
			for( int i = 0; i < FReadyElements.size(); ++i )
			{
				FormattedTextElementBasic<T_INSTANCE> *AElemen = FReadyElements[ i ]->Element;
				if( AElemen )
					if( AElemen->FModified )
						AModified= true;
			}

			if( AModified )
				if( ! ClockInputPin_o_IsConnected )
					ProcessSendOutput();

		}

		inline void SystemStart() // Placeholder for inherited classes
		{
		}

	public:
		inline void ClockInputPin_o_Receive( void *_Data )
		{
			ProcessSendOutput();
		}

	};
//---------------------------------------------------------------------------
	template<uint16_t C_COUNT> class FormattedText_Fixed : public Mitov::BasicTypedFormattedText<FormattedText_Fixed<C_COUNT>, char *, C_COUNT>
	{
		typedef Mitov::BasicTypedFormattedText<FormattedText_Fixed<C_COUNT>, char *, C_COUNT> inherited;

	public:
		void AddNullElement( const char *ATextItem )
		{
			typename inherited::TStringItem	*AItem = new typename inherited::TStringItem;

			AItem->Text = (char *)ATextItem;
			AItem->Element = nullptr;

			inherited::FReadyElements.push_back( AItem );
		}

	public:
		inline void SystemStart()
		{
			inherited::ProcessSendOutput();
		}

	};
//---------------------------------------------------------------------------
	template<uint16_t C_COUNT> class FormattedText : public Mitov::BasicTypedFormattedText<FormattedText<C_COUNT>, Mitov::String, C_COUNT>
	{
		typedef Mitov::BasicTypedFormattedText<FormattedText<C_COUNT>, Mitov::String, C_COUNT> inherited;

	public:
		Mitov::String Text;

	public:
		void InitElements()
		{
			inherited::FReadyElements.clear();
			Mitov::String	ATextItem;
			Mitov::String	AIndexText;
			bool	AInEscape = false;

//			Serial.println( "INIT" );
//			Serial.println( Text );
//			delay( 1000 );

			for( unsigned int i = 0; i < Text.length(); ++ i )
			{
				char AChar = Text[ i ];
				if( AInEscape )
				{
					if( AChar >= '0' && AChar <= '9' )
						AIndexText += AChar;

					else
					{
						if( AChar == '%' )
						{
							if( AIndexText.length() == 0 )
							{
								ATextItem += '%';
								AInEscape = false;
							}

							else
							{
								inherited::AddReadyElement( ATextItem, AIndexText.toInt() );
								ATextItem = "";
								AIndexText = "";
							}

						}

						else
						{
							if( AIndexText.length() == 0 )
								ATextItem += '%';

							else
							{
								inherited::AddReadyElement( ATextItem, AIndexText.toInt() );
								ATextItem = "";
							}

							ATextItem += AChar;
						}

						if( AChar != '%' )
							AInEscape = false;
					}
				}

				else
				{
					if( AChar == '%' )
					{
						AInEscape = true;
						AIndexText = "";
					}

					else
						ATextItem += AChar;

				}

			}

			if( AInEscape )
				inherited::AddReadyElement( ATextItem, AIndexText.toInt() );

			else if( ATextItem.length() )
			{
				typename inherited::TStringItem	*AItem = new typename inherited::TStringItem;

				AItem->Text = ATextItem;
				AItem->Element = nullptr;

				inherited::FReadyElements.push_back( AItem );
			}

//			Serial.println( "DEBUG>>" );
//			for( Mitov::SimpleList<TStringItem *>::iterator Iter = FReadyElements.begin(); Iter != FReadyElements.end(); ++Iter )
//				Serial.println( ( *Iter )->Text );

//			Serial.println( "<<DEBUG" );
		}

	public:
		inline void SystemStart()
		{
			InitElements();
			inherited::ProcessSendOutput();
		}

	};
//---------------------------------------------------------------------------
	template<typename T_LCD, T_LCD &C_LCD, typename T> class TextFormatElementInput
	{
	public:
		inline void InputPin_o_Receive( void *_Data )
		{
			C_LCD.SetValue( Mitov::String( *(T*)_Data ));
		}
	};
//---------------------------------------------------------------------------
	template<typename T_LCD, T_LCD &C_LCD> class TextFormatElementStringInput
	{
	public:
		inline void InputPin_o_Receive( void *_Data )
		{
			C_LCD.SetValue( (char*)_Data );
		}

	};
//---------------------------------------------------------------------------
	template<typename T_LCD, T_LCD &C_LCD> class TextFormatElementClockInput
	{
	public:
		inline void InputPin_o_Receive( void *_Data )
		{
			C_LCD.SetValue( "(Clock)" );
		}

	};
//---------------------------------------------------------------------------
	template<typename T_LCD, T_LCD &C_LCD, typename T_OBJECT> class TextFormatElementObjectInput
	{
	public:
		inline void InputPin_o_Receive( void *_Data )
		{
			C_LCD.SetValue( ((T_OBJECT *)_Data)->ToString() );
		}

	};
//---------------------------------------------------------------------------
	template<typename T_OWNER, T_OWNER &C_OWNER> class FormattedTextElementText : public FormattedTextElementBasic<T_OWNER>
	{
		typedef Mitov::FormattedTextElementBasic<T_OWNER> inherited;

	public:
		Mitov::String	InitialValue;

	public:
		void SetValue( Mitov::String AValue )
		{
//			Serial.println( AValue );
			inherited::FModified = true;
			inherited::FPopulated = true;
			InitialValue = AValue;
		}

	public:
		virtual Mitov::String GetText() override
		{
//			Serial.print( "TEST1: " );
//			Serial.println( FValue );
			return InitialValue;
		}

	public:
		inline void SystemStart()
		{
//			if( ! inherited::FPopulated )
//				FValue = InitialValue;

//			Serial.print( "TEST0: " );
//			Serial.println( InitialValue );
//			Serial.println( FValue );
		}

	public:
		FormattedTextElementText( uint16_t AIndex ) :
			inherited( C_OWNER, AIndex )
		{
		}

	};
//---------------------------------------------------------------------------
	template<typename T_OWNER, T_OWNER &C_OWNER, typename T> class FormattedTextElementTyped : public Mitov::FormattedTextElementBasic<T_OWNER>
	{
		typedef Mitov::FormattedTextElementBasic<T_OWNER> inherited;

	public:
		T	InitialValue;

	public:
		void InputPin_o_Receive( void *_Data )
		{
			T AValue = *(T*)_Data;
			if( AValue == InitialValue )
				return;

			InitialValue = AValue;
			inherited::FModified = true;
		}

	public:
		inline void SystemStart() // Placeholder for inherited classes
		{
		}

	public:
		FormattedTextElementTyped( uint16_t AIndex ) :
			inherited( C_OWNER, AIndex )
		{
		}

	};
//---------------------------------------------------------------------------
	template<typename T> class FormattedTextLengthElementTyped : public T
	{
		typedef T inherited;

	public:
		uint8_t	Length = 0;
		char	FillCharacter = ' ';

	public:
		virtual Mitov::String GetText() override
		{
			Mitov::String AText = inherited::GetText();
			for( int i = AText.length(); i < Length; ++i )
				AText = FillCharacter + AText;

			return AText;
		}

	public:
		using inherited::inherited;

	};
//---------------------------------------------------------------------------
	template<typename T_OWNER, T_OWNER &C_OWNER> class FormattedTextElementInteger : public Mitov::FormattedTextElementTyped<T_OWNER, C_OWNER, int32_t>
	{
		typedef Mitov::FormattedTextElementTyped<T_OWNER, C_OWNER, int32_t> inherited;

	public:
		uint8_t	Base = 10;

	public:
		virtual Mitov::String GetText() override
		{
			char AText[ 16 ];
			ltoa( inherited::InitialValue, AText, Base );

			return AText;
		}

	public:
		using inherited::inherited;

	};
//---------------------------------------------------------------------------
	template<typename T_OWNER, T_OWNER &C_OWNER> class FormattedTextElementUnsigned : public Mitov::FormattedTextElementTyped<T_OWNER, C_OWNER, uint32_t>
	{
		typedef Mitov::FormattedTextElementTyped<T_OWNER, C_OWNER, uint32_t> inherited;

	public:
		uint8_t	Base = 10;

	public:
		virtual Mitov::String GetText() override
		{
			char AText[ 16 ];
			itoa( inherited::InitialValue, AText, Base );

			return AText;
		}

	public:
		using inherited::inherited;

	};
//---------------------------------------------------------------------------
	template<typename T_OWNER, T_OWNER &C_OWNER> class FormattedTextElementAnalog : public Mitov::FormattedTextElementTyped<T_OWNER, C_OWNER, float>
	{
		typedef Mitov::FormattedTextElementTyped<T_OWNER, C_OWNER, float> inherited;

	public:
		uint8_t	MinWidth : 5;
		uint8_t	Precision : 5;

	public:
		virtual Mitov::String GetText() override
		{
			char AText[ 16 ];
			dtostrf( inherited::InitialValue,  MinWidth, Precision, AText );
//			Serial.println( AText );

			return AText;
		}

	public:
		FormattedTextElementAnalog( uint16_t AIndex ) :
			inherited( AIndex ),
			MinWidth( 1 ),
			Precision( 3 )
		{
		}

	};
//---------------------------------------------------------------------------
	template<typename T_OWNER, T_OWNER &C_OWNER> class FormattedTextElementDigital : public Mitov::FormattedTextElementTyped<T_OWNER, C_OWNER, bool>
	{
		typedef Mitov::FormattedTextElementTyped<T_OWNER, C_OWNER, bool> inherited;

	public:
		Mitov::String	TrueValue = "true";
		Mitov::String	FalseValue = "false";

	public:
		virtual Mitov::String GetText() override
		{
			if( inherited::InitialValue )
				return TrueValue;

			return FalseValue;
		}

	public:
		FormattedTextElementDigital( uint16_t AIndex ) : // Do not use "using inherited::inherited;" for ESP8266 compatibility!
			inherited( AIndex )
		{
		}
//		using inherited::inherited;

	};
//---------------------------------------------------------------------------
	class TextLength
	{
	public:
		OpenWire::SourcePin	OutputPin;

    public:
        bool Enabled = true;

	public:
		void InputPin_o_Receive( void *_Data )
        {
            if( ! Enabled )
                return;

			int32_t AValue = strlen( (char *)_Data );

			OutputPin.Notify( &AValue );
        }

	};
//---------------------------------------------------------------------------
	class FindTextPosition
	{
	public:
		OpenWire::SourcePin	OutputPin;

    public:
        bool	Enabled : 1;
		bool	IgnoreCase : 1;
		Mitov::String	Text;

	public:
		void InputPin_o_Receive( void *_Data )
        {
            if( ! Enabled )
                return;

			Mitov::String		AText = Text;
			Mitov::String		ATextValue = Mitov::String( (char *)_Data );

			if( IgnoreCase )
			{
				AText.toUpperCase();
				ATextValue.toUpperCase();
			}

			int32_t AValue = ATextValue.indexOf( AText );

			OutputPin.Notify( &AValue );
        }

	public:
		FindTextPosition() :
			Enabled( true ),
			IgnoreCase( false )
		{
		}

	};
//---------------------------------------------------------------------------
	class FindLastTextPosition
	{
	public:
		OpenWire::SourcePin	OutputPin;

    public:
        bool	Enabled : 1;
		bool	IgnoreCase : 1;
		Mitov::String	Text;

	public:
		void InputPin_o_Receive( void *_Data )
        {
            if( ! Enabled )
                return;

			Mitov::String		AText = Text;
			Mitov::String		ATextValue = Mitov::String( (char *)_Data );

			if( IgnoreCase )
			{
				AText.toUpperCase();
				ATextValue.toUpperCase();
			}

			int32_t AValue = ATextValue.lastIndexOf( AText );

			OutputPin.Notify( &AValue );
        }

	public:
		FindLastTextPosition() :
			Enabled( true ),
			IgnoreCase( false )
		{
		}

	};
//---------------------------------------------------------------------------
	class ContainsText
	{
	public:
		OpenWire::SourcePin	OutputPin;

    public:
        bool	Enabled : 1;
		bool	IgnoreCase : 1;
		Mitov::String	Text;

	public:
		void InputPin_o_Receive( void *_Data )
        {
            if( ! Enabled )
                return;

			Mitov::String		AText = Text;
			Mitov::String		ATextValue = Mitov::String( (char *)_Data );

			if( IgnoreCase )
			{
				AText.toUpperCase();
				ATextValue.toUpperCase();
			}

			bool AValue = ( ATextValue.indexOf( AText ) >= 0 );

			OutputPin.Notify( &AValue );
        }

	public:
		ContainsText() :
			Enabled( true ),
			IgnoreCase( false )
		{
		}

	};
//---------------------------------------------------------------------------
	class TextStartsWith
	{
	public:
		OpenWire::SourcePin	OutputPin;

    public:
        bool Enabled : 1;

	public:
		bool	IgnoreCase : 1;
		Mitov::String	Text;

	public:
		void InputPin_o_Receive( void *_Data )
        {
            if( ! Enabled )
                return;

			Mitov::String		AText = Text;
			Mitov::String		ATextValue = Mitov::String( (char *)_Data );

			if( IgnoreCase )
			{
				AText.toUpperCase();
				ATextValue.toUpperCase();
			}

			bool AValue = ATextValue.startsWith( AText );

			OutputPin.Notify( &AValue );
        }

	public:
		TextStartsWith() :
			Enabled( true ),
			IgnoreCase( false )
		{
		}

	};
//---------------------------------------------------------------------------
	class TextEndsWith
	{
	public:
		OpenWire::SourcePin	OutputPin;

    public:
        bool	Enabled : 1;
		bool	IgnoreCase : 1;
		Mitov::String	Text;

	public:
		void InputPin_o_Receive( void *_Data )
        {
            if( ! Enabled )
                return;

			Mitov::String		AText = Text;
			Mitov::String		ATextValue = Mitov::String( (char *)_Data );

			if( IgnoreCase )
			{
				AText.toUpperCase();
				ATextValue.toUpperCase();
			}

			bool AValue = ATextValue.endsWith( AText );

			OutputPin.Notify( &AValue );
        }

	public:
		TextEndsWith() :
			Enabled( true ),
			IgnoreCase( false )
		{
		}

	};
//---------------------------------------------------------------------------
	class ReplaceText
	{
	public:
		OpenWire::SourcePin	OutputPin;

    public:
        bool Enabled = true;

	public:
		Mitov::String	FromValue;
		Mitov::String	ToValue;

	public:
		void InputPin_o_Receive( void *_Data )
        {
            if( ! Enabled )
			{
				OutputPin.SendValue( _Data );
                return;
			}

			Mitov::String AValue = (char *)_Data;
			AValue.replace( FromValue, ToValue );

			OutputPin.SendValue( AValue );
        }
	};
//---------------------------------------------------------------------------
	class SubText
	{
	public:
		OpenWire::SourcePin	OutputPin;

    public:
        bool Enabled = true;

	public:
		uint32_t	Length = 1;
		uint32_t	Position = 0;

	public:
		void InputPin_o_Receive( void *_Data )
        {
            if( ! Enabled )
			{
				OutputPin.SendValue( _Data );
                return;
			}

			Mitov::String AValue = Mitov::String( (char *)_Data ).substring( Position, Position + Length );

			OutputPin.SendValue( AValue );
        }

	};
//---------------------------------------------------------------------------
	class LeftSubText
	{
	public:
		OpenWire::SourcePin	OutputPin;

    public:
        bool Enabled = true;

	public:
		uint32_t	Length = 1;

	public:
		void InputPin_o_Receive( void *_Data )
        {
            if( ! Enabled )
			{
				OutputPin.SendValue( _Data );
                return;
			}

			Mitov::String AValue = Mitov::String( (char *)_Data ).substring( 0, Length );

			OutputPin.SendValue( AValue );
        }

	};
//---------------------------------------------------------------------------
	class RightSubText
	{
	public:
		OpenWire::SourcePin	OutputPin;

    public:
        bool Enabled = true;

	public:
		uint32_t	Length = 1;

	public:
		void InputPin_o_Receive( void *_Data )
        {
            if( ! Enabled )
			{
				OutputPin.SendValue( _Data );
                return;
			}

			Mitov::String AValue = Mitov::String( (char *)_Data );
			AValue = AValue.substring( AValue.length() - Length );

			OutputPin.SendValue( AValue );
        }

	};
//---------------------------------------------------------------------------
	class DeleteSubText
	{
	public:
		OpenWire::SourcePin	OutputPin;

    public:
        bool Enabled = true;

	public:
		uint32_t	Length = 1;
		uint32_t	Position = 0;

	public:
		void InputPin_o_Receive( void *_Data )
        {
            if( ! Enabled )
			{
				OutputPin.SendValue( _Data );
                return;
			}

			Mitov::String AValue = Mitov::String( (char *)_Data );
			AValue.remove( Position, Length );

			OutputPin.SendValue( AValue );
        }

	};
//---------------------------------------------------------------------------
	class DeleteLeftSubText
	{
	public:
		OpenWire::SourcePin	OutputPin;

    public:
        bool Enabled = true;

	public:
		uint32_t	Length = 1;

	public:
		void InputPin_o_Receive( void *_Data )
        {
            if( ! Enabled )
			{
				OutputPin.SendValue( _Data );
                return;
			}

			Mitov::String AValue = Mitov::String( (char *)_Data );
			AValue.remove( 0, Length );

			OutputPin.SendValue( AValue );
        }

	};
//---------------------------------------------------------------------------
	class DeleteRightSubText
	{
	public:
		OpenWire::SourcePin	OutputPin;

    public:
        bool Enabled = true;

	public:
		uint32_t	Length = 1;

	public:
		void InputPin_o_Receive( void *_Data )
        {
            if( ! Enabled )
			{
				OutputPin.SendValue( _Data );
                return;
			}

			Mitov::String AValue = Mitov::String( (char *)_Data );
			AValue.remove( AValue.length() - Length );

			OutputPin.SendValue( AValue );
        }

	};
//---------------------------------------------------------------------------
	class ToUpperCase
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

			Mitov::String AValue = Mitov::String( (char *)_Data );
			AValue.toUpperCase();

			OutputPin.SendValue( AValue );
        }
	};
//---------------------------------------------------------------------------
	class ToLowerCase
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

			Mitov::String AValue = Mitov::String( (char *)_Data );
			AValue.toLowerCase();

			OutputPin.SendValue( AValue );
        }
	};
//---------------------------------------------------------------------------
	class TrimText
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

			Mitov::String AValue = Mitov::String( (char *)_Data );
			AValue.trim();

			OutputPin.SendValue( AValue );
        }

	};
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
}

#ifdef __TEST_FOR_DEBUG_PRINTS__
#undef Serial
#endif

#endif
