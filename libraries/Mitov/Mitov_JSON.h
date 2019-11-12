////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2018 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_JSON_h
#define _MITOV_JSON_h

#include <Mitov.h>

#ifdef __TEST_FOR_DEBUG_PRINTS__
#define Serial UNGUARDED DEBUG PRINT!!!
#endif

namespace Mitov
{
	class MakeJSONBasicCommonElement;
	class SplitJSONBasicCommonElement;
//---------------------------------------------------------------------------
	class JSONMakeIntf
	{
	public:
		Mitov::SimpleObjectList<MakeJSONBasicCommonElement*>	Elements;
	};
//---------------------------------------------------------------------------
	class JSONSplitIntf
	{
	public:
		Mitov::SimpleObjectList<SplitJSONBasicCommonElement*>	Elements;

	protected:
		static bool ParseHex( Mitov::String &AText, int &AIndex, Mitov::String &AName )
		{
			++AIndex;
//			Mitov::String AResult;
			int AStartIndex = AIndex;

			if( ! IsHexChar( AText[ AIndex ] ))
				return false;

//			while( IsHexChar( AText[ AIndex ] ))
//				++ AIndex;
//				AResult += AText[ AIndex ++ ];

			const char *ACharText = AText.c_str() + AStartIndex;
			char *AEnd;

			int AValue = strtol( ACharText, &AEnd, 16 );

			AIndex += ( AEnd - ACharText );

			AName += char( AValue );
		}

		static bool IsHexChar( char AChar )
		{
			if( AChar >= '0' && AChar <= '9' )
				return true;

			if( AChar >= 'a' && AChar <= 'f' )
				return true;

			if( AChar >= 'A' && AChar <= 'F' )
				return true;

			return false;
		}

	public:
		static bool ExtractString( Mitov::String &AText, Mitov::String &AName )
		{
			if( AText[ 0 ] != '"' )
				return false;

			bool AInEscape = false;			

			for( int i = 1; i < AText.length(); ++ i )
			{
				char AChar = AText[ i ];
				if( AInEscape )
				{
					switch( AChar )
					{
						case '"' :
						case '\\' :
						case '/' :
							AName += AChar;
							break;

						case 'b' :	AName += char( 8 );		break;
						case 'f' :	AName += char( 12 );	break;
						case 'n' :	AName += char( 10 );	break;
						case 'r' :	AName += char( 13 );	break;
						case 't' :	AName += char( 9 );	break;
						case 'u' :	
							if( ! ParseHex( AText, i, AName ))
								return false;

							break;

						default:
							return false;
					}
				}

				else
				{
					if( AChar == '"' )
					{
						AText.remove( 0, i + 1 );
						return true;
					}

					if( AChar == '\\' )
						AInEscape = true;

					else
						AName += AChar;
				}
			}

			return false;
		}

		static bool ExtractObject( Mitov::String &AText )
		{
//			Serial.println( "OBJECT" );

			AText.remove( 0, 1 );
			AText.trim();
			while( AText.length() )
			{
				if( AText.startsWith( "}" ) )
				{
					AText.remove( 0, 1 );
					AText.trim();
					break;
				}

				Mitov::String AValue;
				if( ! ExtractString( AText, AValue ))
					return false;

				AText.trim();
				if( ! AText.startsWith( ":" ) )
					return false;

				AText.remove( 0, 1 );
				AText.trim();

				if( ! ExtractElement( AText ))
					return false;

				if( AText.startsWith( "," ) )
				{
					AText.remove( 0, 1 );
					AText.trim();
				}
			}

			return true;
		}

		static bool ExtractArray( Mitov::String &AText )
		{
			AText.remove( 0, 1 );
			AText.trim();
			while( AText.length() )
			{
				if( AText.startsWith( "]" ) )
				{
					AText.remove( 0, 1 );
					AText.trim();
					break;
				}

				if( ! ExtractElement( AText ))
					return false;

				if( AText.startsWith( "," ) )
				{
					AText.remove( 0, 1 );
					AText.trim();
				}
			}
		}

		static bool IsNumberChar( char AChar )
		{
			static const char numberChars[] = { '-', '+', '.', 'e', 'E' };

			if( ( AChar >= '0' ) && ( AChar <= '9' ) )
				return true;

			for( int i = 0; i < MITOV_ARRAY_SIZE( numberChars ); ++ i )
				if( AChar == numberChars[ i ] )
					return true;

			return false;
		}

		static bool ExtractNumber( Mitov::String &AText, Mitov::String &AValue )
		{
			for( int i = 0; i < AText.length(); ++ i )
			{
				if( ! IsNumberChar( AText[ i ] ))
				{
					if( i == 0 )
						return false;

					AValue = AText.substring( 0, i );

//					Serial.println( AValue );

					AText.remove( 0, i );
					return true;
				}
			}

			return true;
		}

		static bool ExtractElement( Mitov::String &AText )
		{
//			Serial.println( "ExtractElement" );
//			Serial.println( AText );

			Mitov::String AValue;
			if( AText.startsWith( "\"" ))
				return ExtractString( AText, AValue );

			if( AText.startsWith( "true" ))
			{
				AText.remove( 0, 4 );
				return true;
			}

			if( AText.startsWith( "false" ))
			{
				AText.remove( 0, 5 );
				return true;
			}

			if( AText.startsWith( "null" ))
			{
//				Serial.println( "NULL" );
				AText.remove( 0, 4 );
				return true;
			}

			if( AText.startsWith( "{" ))
				return ExtractObject( AText );

			if( AText.startsWith( "[" ))
				return ExtractArray( AText );

			return ExtractNumber( AText, AValue );
		}

	protected:
		bool ProcessObject( Mitov::String &AText );
		bool ProcessArray( Mitov::String &AText );

	};
//---------------------------------------------------------------------------
	class MakeJSONBasicCommonElement
	{
	protected:
		bool FModified = false;

	public:
		virtual bool GetIsModified() { return FModified; }
		virtual Mitov::String GetText() = 0;

	public:
		MakeJSONBasicCommonElement( JSONMakeIntf &AOwner )
		{
			AOwner.Elements.push_back( this );
		}
	};
//---------------------------------------------------------------------------
	class SplitJSONBasicCommonElement
	{
	public:
		virtual bool Extract( Mitov::String &AText ) = 0;
		virtual void SendDefault() = 0;

	public:
		SplitJSONBasicCommonElement( JSONSplitIntf &AOwner )
		{
			AOwner.Elements.push_back( this );
		}
	};
//---------------------------------------------------------------------------
	class SplitJSONBasicObjectElement : public SplitJSONBasicCommonElement
	{
		typedef SplitJSONBasicCommonElement inherited;

	public:
		const char *FName;

	public:
		SplitJSONBasicObjectElement( JSONSplitIntf &AOwner, const char *AName ) :
			inherited( AOwner ),
			FName( AName )
		{
		}

	};
//---------------------------------------------------------------------------
	class MakeJSON : public JSONMakeIntf
	{
	public:
		OpenWire::SourcePin	OutputPin;

	public:
		bool	OnlyModified = false;

	public:
		void ClockInputPin_o_Receive( void *_Data )
		{
//			Serial.println( "++++++" );
			int AElementCount = Elements.size();
			if( OnlyModified )
			{
				bool AModified = false;
				for( int i = 0; i < AElementCount; ++i )
					if( Elements[ i ]->GetIsModified() )
					{
						AModified = true;
						break;
					}

				if( ! AModified )
					return;
			}

/*
			else
			{
				for( int i = 0; i < AElementCount; ++i )
					if( ! Elements[ i ]->GetIsPopulated() )
						return;

			}
*/
			Mitov::String AResult = "{";

			for( int i = 0; i < AElementCount; ++i )
			{
				AResult += Elements[ i ]->GetText();
				if( i < AElementCount - 1 )
					AResult += ",";

			}

			AResult += "}";

			OutputPin.Notify( (void *)AResult.c_str() );
		}

	public:
		inline void SystemLoopBegin( unsigned long currentMicros )
		{
			ClockInputPin_o_Receive( nullptr );
		}

	};
//---------------------------------------------------------------------------
	class MakeJSONBasicObjectElement : public MakeJSONBasicCommonElement
	{
		typedef MakeJSONBasicCommonElement inherited;

	protected:
		const char *FName;

	protected:
		virtual	Mitov::String GetValueText() = 0;

	public:
		virtual Mitov::String GetText() override
		{
			return Mitov::String( "\"" ) + FName + "\":" + GetValueText();
		}

	public:
		MakeJSONBasicObjectElement( JSONMakeIntf &AOwner, const char *AName ) :
			inherited( AOwner ),
			FName( AName )
		{
		}

	};
//---------------------------------------------------------------------------
	class SplitJSON : public JSONSplitIntf
	{
	public:
		OpenWire::SourcePin	ErrorOutputPin;

	public:
		void InputPin_o_Receive( void *_Data )
		{
			Mitov::String AText = (char *)_Data;

			if( ! ProcessObject( AText ))
			{
				ErrorOutputPin.Notify( nullptr );
				return;
			}

			if( AText.length() )
			{
				ErrorOutputPin.Notify( nullptr );
				return;
			}

			for( int i = 0; i < Elements.size(); ++i )
				Elements[ i ]->SendDefault();
		}

	};
//---------------------------------------------------------------------------
	class MakeJSONTextElement : public MakeJSONBasicObjectElement
	{
		typedef MakeJSONBasicObjectElement inherited;

	public:
		Mitov::String InitialValue;

	protected:
		virtual	Mitov::String GetValueText() override
		{
			FModified = false;
			return "\"" + InitialValue + "\"";
		}

	public:
		void InputPin_o_Receive( void *_Data )
		{
			InitialValue = (char *)_Data;
			FModified = true;
		}

	public:
		using inherited::inherited;

	};
//---------------------------------------------------------------------------
	class MakeJSONDigitalElement : public MakeJSONBasicObjectElement
	{
		typedef MakeJSONBasicObjectElement inherited;

	public:
		bool InitialValue = false;

	protected:
		virtual	Mitov::String GetValueText() override
		{
			FModified = false;
			if( InitialValue )
				return "true";

			return "false";
		}

	public:
		void InputPin_o_Receive( void *_Data )
		{
			InitialValue = *(bool *)_Data;
			FModified = true;
		}

	public:
		using inherited::inherited;

	};
//---------------------------------------------------------------------------
	class MakeJSONIntegerElement : public MakeJSONBasicObjectElement
	{
		typedef MakeJSONBasicObjectElement inherited;

	public:
		int32_t InitialValue = 0;

	protected:
		virtual	Mitov::String GetValueText() override
		{
			FModified = false;
			char AText[ 16 ];
			ltoa( InitialValue, AText, 10 );
			return AText;
		}

	public:
		void InputPin_o_Receive( void *_Data )
		{
			InitialValue = *(int32_t *)_Data;
			FModified = true;
		}

	public:
		using inherited::inherited;

	};
//---------------------------------------------------------------------------
	class MakeJSONUnsignedElement : public MakeJSONBasicObjectElement
	{
		typedef MakeJSONBasicObjectElement inherited;

	public:
		uint32_t InitialValue = 0;

	protected:
		virtual	Mitov::String GetValueText() override
		{
			FModified = false;
			char AText[ 16 ];
			ltoa( InitialValue, AText, 10 );
			return AText;
		}

	public:
		void InputPin_o_Receive( void *_Data )
		{
			InitialValue = *(uint32_t *)_Data;
			FModified = true;
		}

	public:
		using inherited::inherited;

	};
//---------------------------------------------------------------------------
	class MakeJSONAnalogElement : public MakeJSONBasicObjectElement
	{
		typedef MakeJSONBasicObjectElement inherited;

	public:
		float InitialValue = 0;
		uint8_t	MinWidth : 5;
		uint8_t	Precision : 5;

	protected:
		virtual	Mitov::String GetValueText() override
		{
			FModified = false;
			char AText[ 16 ];
			dtostrf( InitialValue,  MinWidth, Precision, AText );
			return AText;
		}

	public:
		void InputPin_o_Receive( void *_Data )
		{
			InitialValue = *(float *)_Data;
			FModified = true;
		}

	public:
		MakeJSONAnalogElement( JSONMakeIntf &AOwner, const char *AName ) :
			inherited( AOwner, AName ),
			MinWidth( 1 ),
			Precision( 3 )
		{
		}

	};
//---------------------------------------------------------------------------
	class MakeJSONNullElement : public MakeJSONBasicObjectElement
	{
		typedef MakeJSONBasicObjectElement inherited;

	protected:
		virtual	Mitov::String GetValueText() override
		{
			return "null";
		}

	public:
		using inherited::inherited;

	};
//---------------------------------------------------------------------------
	class MakeJSONObjectElement : public MakeJSONBasicObjectElement, public JSONMakeIntf
	{
		typedef MakeJSONBasicObjectElement inherited;

	public:
		virtual bool GetIsModified() 
		{ 
			for( int i = 0; i < Elements.size(); ++i )
				if( Elements[ i ]->GetIsModified() )
					return true;

			return false;
		}

	protected:
		virtual	Mitov::String GetValueText() override
		{
			int AElementCount = Elements.size();

			Mitov::String AResult = "{";

			for( int i = 0; i < AElementCount; ++i )
			{
				AResult += Elements[ i ]->GetText();
				if( i < AElementCount - 1 )
					AResult += ",";

			}

			return AResult + "}";
		}

	public:
		using inherited::inherited;

	};
//---------------------------------------------------------------------------
	class MakeJSONArrayElement : public MakeJSONObjectElement
	{
		typedef MakeJSONObjectElement inherited;

	protected:
		virtual	Mitov::String GetValueText() override
		{
			int AElementCount = Elements.size();

			Mitov::String AResult = "[";

			for( int i = 0; i < AElementCount; ++i )
			{
				AResult += Elements[ i ]->GetText();
				if( i < AElementCount - 1 )
					AResult += ",";

			}

			return AResult + "]";
		}

	public:
		using inherited::inherited;

	};
//---------------------------------------------------------------------------
	class MakeJSONTextArrayElement : public MakeJSONBasicCommonElement
	{
		typedef MakeJSONBasicCommonElement inherited;

	public:
		Mitov::String InitialValue;

	protected:
		virtual	Mitov::String GetText() override
		{
			FModified = false;
			return "\"" + InitialValue + "\"";
		}

	public:
		void InputPin_o_Receive( void *_Data )
		{
			InitialValue = (char *)_Data;
			FModified = true;
		}

	public:
		using inherited::inherited;

	};
//---------------------------------------------------------------------------
	class MakeJSONDigitalArrayElement : public MakeJSONBasicCommonElement
	{
		typedef MakeJSONBasicCommonElement inherited;

	public:
		bool InitialValue = false;

	protected:
		virtual	Mitov::String GetText() override
		{
			FModified = false;
			if( InitialValue )
				return "true";

			return "false";
		}

	public:
		void InputPin_o_Receive( void *_Data )
		{
			InitialValue = *(bool *)_Data;
			FModified = true;
		}

	public:
		using inherited::inherited;

	};
//---------------------------------------------------------------------------
	class MakeJSONIntegerArrayElement : public MakeJSONBasicCommonElement
	{
		typedef MakeJSONBasicCommonElement inherited;

	public:
		int32_t InitialValue = 0;

	protected:
		virtual	Mitov::String GetText() override
		{
			FModified = false;
			char AText[ 16 ];
			ltoa( InitialValue, AText, 10 );
			return AText;
		}

	public:
		void InputPin_o_Receive( void *_Data )
		{
			InitialValue = *(int32_t *)_Data;
			FModified = true;
		}

	public:
		using inherited::inherited;

	};
//---------------------------------------------------------------------------
	class MakeJSONUnsignedArrayElement : public MakeJSONBasicCommonElement
	{
		typedef MakeJSONBasicCommonElement inherited;

	public:
		uint32_t InitialValue = 0;

	protected:
		virtual	Mitov::String GetText() override
		{
			FModified = false;
			char AText[ 16 ];
			ltoa( InitialValue, AText, 10 );
			return AText;
		}

	public:
		void InputPin_o_Receive( void *_Data )
		{
			InitialValue = *(uint32_t *)_Data;
			FModified = true;
		}

	public:
		using inherited::inherited;

	};
//---------------------------------------------------------------------------
	class MakeJSONAnalogArrayElement : public MakeJSONBasicCommonElement
	{
		typedef MakeJSONBasicCommonElement inherited;

	public:
		float InitialValue = 0;
		uint8_t	MinWidth : 5;
		uint8_t	Precision : 5;

	protected:
		virtual	Mitov::String GetText() override
		{
			FModified = false;
			char AText[ 16 ];
			dtostrf( InitialValue,  MinWidth, Precision, AText );
			return AText;
		}

	public:
		void InputPin_o_Receive( void *_Data )
		{
			InitialValue = *(float *)_Data;
			FModified = true;
		}

	public:
		MakeJSONAnalogArrayElement( JSONMakeIntf &AOwner ) :
			inherited( AOwner ),
			MinWidth( 1 ),
			Precision( 3 )
		{
		}

	};
//---------------------------------------------------------------------------
	class MakeJSONNullArrayElement : public MakeJSONBasicCommonElement
	{
		typedef MakeJSONBasicCommonElement inherited;

	protected:
		virtual	Mitov::String GetText() override
		{
			return "null";
		}

	public:
		using inherited::inherited;

	};
//---------------------------------------------------------------------------
	class MakeJSONObjectArrayElement : public MakeJSONBasicCommonElement, public JSONMakeIntf
	{
		typedef MakeJSONBasicCommonElement inherited;

	public:
		virtual bool GetIsModified() 
		{ 
			for( int i = 0; i < Elements.size(); ++i )
				if( Elements[ i ]->GetIsModified() )
					return true;

			return false;
		}

	protected:
		virtual	Mitov::String GetText() override
		{
			int AElementCount = Elements.size();

			Mitov::String AResult = "{";

			for( int i = 0; i < AElementCount; ++i )
			{
				AResult += Elements[ i ]->GetText();
				if( i < AElementCount - 1 )
					AResult += ",";

			}

			return AResult + "}";
		}

	public:
		using inherited::inherited;

	};
//---------------------------------------------------------------------------
	class MakeJSONArrayArrayElement : public MakeJSONObjectArrayElement
	{
		typedef MakeJSONObjectArrayElement inherited;

	protected:
		virtual	Mitov::String GetText() override
		{
			int AElementCount = Elements.size();

			Mitov::String AResult = "[";

			for( int i = 0; i < AElementCount; ++i )
			{
				AResult += Elements[ i ]->GetText();
				if( i < AElementCount - 1 )
					AResult += ",";

			}

			return AResult + "]";
		}

	public:
		using inherited::inherited;

	};
//---------------------------------------------------------------------------
	class SplitJSONTextElement : public SplitJSONBasicObjectElement
	{
		typedef SplitJSONBasicObjectElement inherited;

	public:
		OpenWire::SourcePin	OutputPin;

	public:
		Mitov::String	InitialValue;

	public:
		virtual bool Extract( Mitov::String &AText ) override
		{
			Mitov::String AValue;
			if( ! JSONSplitIntf::ExtractString( AText, AValue ))
				return false;

//			Serial.println( AValue );

			InitialValue = AValue;
			return true;
		}

		virtual void SendDefault() override
		{
			OutputPin.Notify( (void *)InitialValue.c_str() );
		}

	public:
		using inherited::inherited;

	};
//---------------------------------------------------------------------------
	class SplitJSONDigitalElement : public SplitJSONBasicObjectElement
	{
		typedef SplitJSONBasicObjectElement inherited;

	public:
		OpenWire::SourcePin	OutputPin;

	public:
		bool	InitialValue = false;

	public:
		virtual bool Extract( Mitov::String &AText ) override
		{
//			Serial.println( AText );

			if( AText.startsWith( "true" ))
			{
				AText.remove( 0, 4 );
//				Serial.println( "TRUE" );
				InitialValue = true;
				return true;
			}

			if( AText.startsWith( "false" ))
			{
				AText.remove( 0, 5 );
//				Serial.println( "FALSE" );
				InitialValue = false;
				return true;
			}

			return false;
		}

		virtual void SendDefault() override
		{
			OutputPin.Notify( &InitialValue );
		}

	public:
		using inherited::inherited;

	};
//---------------------------------------------------------------------------
	class SplitJSONIntegerElement : public SplitJSONBasicObjectElement
	{
		typedef SplitJSONBasicObjectElement inherited;

	public:
		OpenWire::SourcePin	OutputPin;

	public:
		int32_t	InitialValue = 0;

	public:
		virtual bool Extract( Mitov::String &AText ) override
		{
			Mitov::String AValue;
			if( ! JSONSplitIntf::ExtractNumber( AText, AValue ))
				return false;

//			Serial.println( "INTEGER" );
//			Serial.println( AValue );
			InitialValue = atol( AValue.c_str() );
			return true;
		}

		virtual void SendDefault() override
		{
			OutputPin.Notify( &InitialValue );
		}

	public:
		using inherited::inherited;

	};
//---------------------------------------------------------------------------
	class SplitJSONUnsignedElement : public SplitJSONBasicObjectElement
	{
		typedef SplitJSONBasicObjectElement inherited;

	public:
		OpenWire::SourcePin	OutputPin;

	public:
		uint32_t	InitialValue = 0;

	public:
		virtual bool Extract( Mitov::String &AText ) override
		{
			Mitov::String AValue;
			if( ! JSONSplitIntf::ExtractNumber( AText, AValue ))
				return false;

//			Serial.println( "UNSIGNED" );
//			Serial.println( AValue );
			InitialValue = atol( AValue.c_str() );
			return true;
		}

		virtual void SendDefault() override
		{
			OutputPin.Notify( &InitialValue );
		}

	public:
		using inherited::inherited;

	};
//---------------------------------------------------------------------------
	class SplitJSONAnalogElement : public SplitJSONBasicObjectElement
	{
		typedef SplitJSONBasicObjectElement inherited;

	public:
		OpenWire::SourcePin	OutputPin;

	public:
		float	InitialValue = 0;

	public:
		virtual bool Extract( Mitov::String &AText ) override
		{
			Mitov::String AValue;
			if( ! JSONSplitIntf::ExtractNumber( AText, AValue ))
				return false;

//			Serial.println( "ANALOG" );
//			Serial.println( AValue );
			InitialValue = strtod( AValue.c_str(), nullptr );
			return true;
		}

		virtual void SendDefault() override
		{
			OutputPin.Notify( &InitialValue );
		}

	public:
		using inherited::inherited;

	};
//---------------------------------------------------------------------------
	class SplitJSONObjectElement : public SplitJSONBasicObjectElement, public JSONSplitIntf
	{
		typedef SplitJSONBasicObjectElement inherited;

	public:
		virtual bool Extract( Mitov::String &AText ) override
		{
			return ProcessObject( AText );
//			if( !AText.startsWith( "{" ))
//				return false;
		}

		virtual void SendDefault() override
		{
			for( int i = 0; i < Elements.size(); ++i )
				Elements[ i ]->SendDefault();
		}

	public:
		using inherited::inherited;

	};
//---------------------------------------------------------------------------
	class SplitJSONArrayElement : public SplitJSONBasicObjectElement, public JSONSplitIntf
	{
		typedef SplitJSONBasicObjectElement inherited;

	public:
		virtual bool Extract( Mitov::String &AText ) override
		{
			return ProcessArray( AText );
//			if( !AText.startsWith( "{" ))
//				return false;
		}

		virtual void SendDefault() override
		{
			for( int i = 0; i < Elements.size(); ++i )
				Elements[ i ]->SendDefault();
		}

	public:
		using inherited::inherited;

	};
//---------------------------------------------------------------------------
	class SplitJSONTextArrayElement : public SplitJSONBasicCommonElement
	{
		typedef SplitJSONBasicCommonElement inherited;

	public:
		OpenWire::SourcePin	OutputPin;

	public:
		Mitov::String	InitialValue;

	public:
		virtual bool Extract( Mitov::String &AText ) override
		{
			Mitov::String AValue;
			if( ! JSONSplitIntf::ExtractString( AText, AValue ))
				return false;

//			Serial.println( AValue );

			InitialValue = AValue;
			return true;
		}

		virtual void SendDefault() override
		{
			OutputPin.Notify( (void *)InitialValue.c_str() );
		}

	public:
		using inherited::inherited;

	};
//---------------------------------------------------------------------------
	class SplitJSONDigitalArrayElement : public SplitJSONBasicCommonElement
	{
		typedef SplitJSONBasicCommonElement inherited;

	public:
		OpenWire::SourcePin	OutputPin;

	public:
		bool	InitialValue = false;

	public:
		virtual bool Extract( Mitov::String &AText ) override
		{
//			Serial.println( AText );

			if( AText.startsWith( "true" ))
			{
				AText.remove( 0, 4 );
//				Serial.println( "TRUE" );
				InitialValue = true;
				return true;
			}

			if( AText.startsWith( "false" ))
			{
				AText.remove( 0, 5 );
//				Serial.println( "FALSE" );
				InitialValue = false;
				return true;
			}

			return false;
		}

		virtual void SendDefault() override
		{
			OutputPin.Notify( &InitialValue );
		}

	public:
		using inherited::inherited;

	};
//---------------------------------------------------------------------------
	class SplitJSONIntegerArrayElement : public SplitJSONBasicCommonElement
	{
		typedef SplitJSONBasicCommonElement inherited;

	public:
		OpenWire::SourcePin	OutputPin;

	public:
		int32_t	InitialValue = 0;

	public:
		virtual bool Extract( Mitov::String &AText ) override
		{
			Mitov::String AValue;
			if( ! JSONSplitIntf::ExtractNumber( AText, AValue ))
				return false;

//			Serial.println( "INTEGER" );
//			Serial.println( AValue );
			InitialValue = atol( AValue.c_str() );
			return true;
		}

		virtual void SendDefault() override
		{
			OutputPin.Notify( &InitialValue );
		}

	public:
		using inherited::inherited;

	};
//---------------------------------------------------------------------------
	class SplitJSONUnsignedArrayElement : public SplitJSONBasicCommonElement
	{
		typedef SplitJSONBasicCommonElement inherited;

	public:
		OpenWire::SourcePin	OutputPin;

	public:
		uint32_t	InitialValue = 0;

	public:
		virtual bool Extract( Mitov::String &AText ) override
		{
			Mitov::String AValue;
			if( ! JSONSplitIntf::ExtractNumber( AText, AValue ))
				return false;

//			Serial.println( "UNSIGNED" );
//			Serial.println( AValue );
			InitialValue = atol( AValue.c_str() );
			return true;
		}

		virtual void SendDefault() override
		{
			OutputPin.Notify( &InitialValue );
		}

	public:
		using inherited::inherited;

	};
//---------------------------------------------------------------------------
	class SplitJSONAnalogArrayElement : public SplitJSONBasicCommonElement
	{
		typedef SplitJSONBasicCommonElement inherited;

	public:
		OpenWire::SourcePin	OutputPin;

	public:
		float	InitialValue = 0;

	public:
		virtual bool Extract( Mitov::String &AText ) override
		{
			Mitov::String AValue;
			if( ! JSONSplitIntf::ExtractNumber( AText, AValue ))
				return false;

//			Serial.println( "ANALOG" );
//			Serial.println( AValue );
			InitialValue = strtod( AValue.c_str(), nullptr );
			return true;
		}

		virtual void SendDefault() override
		{
			OutputPin.Notify( &InitialValue );
		}

	public:
		using inherited::inherited;

	};
//---------------------------------------------------------------------------
	class SplitJSONObjectArrayElement : public SplitJSONBasicCommonElement, public JSONSplitIntf
	{
		typedef SplitJSONBasicCommonElement inherited;

	public:
		virtual bool Extract( Mitov::String &AText ) override
		{
			return ProcessObject( AText );
//			if( !AText.startsWith( "{" ))
//				return false;
		}

		virtual void SendDefault() override
		{
			for( int i = 0; i < Elements.size(); ++i )
				Elements[ i ]->SendDefault();
		}

	public:
		using inherited::inherited;

	};
//---------------------------------------------------------------------------
	class SplitJSONArrayArrayElement : public SplitJSONBasicCommonElement, public JSONSplitIntf
	{
		typedef SplitJSONBasicCommonElement inherited;

	public:
		virtual bool Extract( Mitov::String &AText ) override
		{
			return ProcessArray( AText );
//			if( !AText.startsWith( "{" ))
//				return false;
		}

		virtual void SendDefault() override
		{
			for( int i = 0; i < Elements.size(); ++i )
				Elements[ i ]->SendDefault();
		}

	public:
		using inherited::inherited;

	};
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
	bool JSONSplitIntf::ProcessObject( Mitov::String &AText )			
	{
		AText.trim();

//			Serial.println( AText );

		if( ! AText.startsWith( "{" ))
			return false;

		AText.remove( 0, 1 );
		AText.trim();

		if( AText.startsWith( "}" ))
		{
			AText.remove( 0, 1 );
			AText.trim();
			return true;
		}

		for(;;)
		{

//				Serial.println( AText );

			Mitov::String AName;

			if( ! ExtractString( AText, AName ) )
				return false;

//			Serial.println( AName );
//			Serial.println( AText );

			AText.trim();
			if( ! AText.startsWith( ":" ))
				return false;

			AText.remove( 0, 1 );
			AText.trim();

//			Serial.println( AText );

			bool AParsed = false;
			for( int i = 0; i < Elements.size(); ++i )
				if( AName == ((SplitJSONBasicObjectElement *) Elements[ i ] )->FName )
				{
//					Serial.println( "MATCH" );
//					Serial.println( AName );
//					Serial.println( ((SplitJSONBasicObjectElement *) Elements[ i ] )->FName );
					if( ! Elements[ i ]->Extract( AText ))
						return false;

//					Serial.println( "SUCCESS" );

					AParsed = true;
					break;
				}

//			Serial.println( AParsed );

			if( ! AParsed )
				if( ! ExtractElement( AText ))
					return false;

//			Serial.println( "SUCCESS1" );
//			Serial.println( AText );
			AText.trim();
			if( AText.startsWith( "}" ))
			{
				AText.remove( 0, 1 );
				AText.trim();
				break;
			}

//			Serial.println( AText );

			AText.trim();
			if( !AText.startsWith( "," ))
				break;

			AText.remove( 0, 1 );
			AText.trim();

//			Serial.println( "SUCCESS2" );
//			Serial.println( AText );
		}

		return true;
	}
//---------------------------------------------------------------------------
	bool JSONSplitIntf::ProcessArray( Mitov::String &AText )
	{
		AText.trim();

//		Serial.println( "->> ProcessArray" );

//			Serial.println( AText );

		if( ! AText.startsWith( "[" ))
			return false;

		AText.remove( 0, 1 );
		AText.trim();

		if( AText.startsWith( "]" ))
		{
			AText.remove( 0, 1 );
			AText.trim();
			return true;
		}

		int AElementIndex = 0;

		for(;;)
		{

//				Serial.println( AText );

			if( AElementIndex < Elements.size() )
			{
				if( ! Elements[ AElementIndex ++ ]->Extract( AText ))
					return false;

			}

			else if( ! ExtractElement( AText ))
				return false;

			AText.trim();
			if( AText.startsWith( "]" ))
			{
				AText.remove( 0, 1 );
				AText.trim();
				break;
			}

//			Serial.println( AText );

			AText.trim();
			if( !AText.startsWith( "," ))
				break;

			AText.remove( 0, 1 );
			AText.trim();

//			Serial.println( AText );
		}
	}
//---------------------------------------------------------------------------
}

#ifdef __TEST_FOR_DEBUG_PRINTS__
#undef Serial
#endif

#endif