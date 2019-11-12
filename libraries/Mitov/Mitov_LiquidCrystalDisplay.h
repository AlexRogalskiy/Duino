////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2018 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_IQUID_CRYSTAL_DISPLAY_h
#define _MITOV_IQUID_CRYSTAL_DISPLAY_h

#include <Mitov.h>

#ifdef __TEST_FOR_DEBUG_PRINTS__
#define Serial UNGUARDED DEBUG PRINT!!!
#endif

namespace Mitov
{
//---------------------------------------------------------------------------
	namespace LCDConsts
	{
		const uint8_t LCD_8BITMODE				= 0x10;
//		const uint8_t LCD_4BITMODE				= 0x00;
		const uint8_t LCD_2LINE					= 0x08;
//		const uint8_t LCD_1LINE					= 0x00;
		const uint8_t LCD_5x10DOTS				= 0x04;
//		const uint8_t LCD_5x8DOTS				= 0x00;

		const uint8_t LCD_ENTRYLEFT				= 0x02;
		const uint8_t LCD_ENTRYSHIFTINCREMENT	= 0x01;

		const uint8_t LCD_DISPLAYON				= 0x04;
		const uint8_t LCD_CURSORON				= 0x02;
		const uint8_t LCD_BLINKON				= 0x01;

		const uint8_t LCD_DISPLAYMOVE			= 0x08;
		const uint8_t LCD_MOVELEFT				= 0x00;
		const uint8_t LCD_MOVERIGHT				= 0x04;

		const uint16_t HOME_CLEAR_EXEC			= 2000;

// LCD Commands
// ---------------------------------------------------------------------------
		const uint8_t LCD_CLEARDISPLAY      = 0x01;
		const uint8_t LCD_RETURNHOME        = 0x02;
		const uint8_t LCD_ENTRYMODESET      = 0x04;
		const uint8_t LCD_DISPLAYCONTROL    = 0x08;
		const uint8_t LCD_CURSORSHIFT       = 0x10;
		const uint8_t LCD_FUNCTIONSET       = 0x20;
		const uint8_t LCD_SETCGRAMADDR      = 0x40;
		const uint8_t LCD_SETDDRAMADDR		= 0x80;
	}
//---------------------------------------------------------------------------
	class LiquidCrystalDisplayBasic
	{
	public:
		bool Enabled : 1;
		bool AutoScroll : 1;
		bool RightToLeft : 1;
		bool ShowCursor : 1;
		bool Blink : 1;
		bool Hight10Pixels : 1;
		bool Backlight : 1; // = true; // In base class to save memory
		bool BacklightPositive : 1; // = true; // In base class to save memory

	public:
		LiquidCrystalDisplayBasic() :
			Enabled( true ),
			AutoScroll( false ),
			RightToLeft( false ),
			ShowCursor( false ),
			Blink( false ),
			Hight10Pixels( false ),
			Backlight( true ),
			BacklightPositive( true )
		{
		}
	};
//---------------------------------------------------------------------------
	// TODO: Implement setCursor() and createChar()
	template<typename T_BASE> class LiquidCrystalDisplay : public T_BASE
	{
		typedef	T_BASE	inherited;

	public: // Must be public for the implementation
		uint8_t	FWidth;
		uint8_t	FHeight;

	protected:
		uint8_t FCursorLine = 0;
		uint8_t FCursorPos = 0;

	public:
		void SetCursor(uint8_t col, uint8_t row)
		{
		   const byte row_offsetsDef[]   = { 0x00, 0x40, 0x14, 0x54 }; // For regular LCDs
		   const byte row_offsetsLarge[] = { 0x00, 0x40, 0x10, 0x50 }; // For 16x4 LCDs
   
		   if ( row >= FHeight ) 
			  row = FHeight - 1;    // rows start at 0
   
		   // 16x4 LCDs have special memory map layout
		   // ----------------------------------------
		   if ( FWidth == 16 && FHeight == 4 )
			  Command( LCDConsts::LCD_SETDDRAMADDR | (col + row_offsetsLarge[row]));

		   else 
			  Command( LCDConsts::LCD_SETDDRAMADDR | (col + row_offsetsDef[row]));
   
		}

		size_t Print( const Mitov::String AValue )
		{
			const char *APtr = AValue.c_str();
			size_t ACount = 0;
			while( *APtr )
			{
				Write( *APtr++ );
				++ ACount;
			}

			return ACount;
		}

		size_t Print( float AValue )
		{
			char AText[ 16 ];
			dtostrf( AValue,  1, 2, AText );
			return Print( Mitov::String( AText ));
		}

		size_t Print( int32_t AValue )
		{
			char AText[ 16 ];
			ltoa( AValue, AText, 10 );
			return Print( Mitov::String( AText ));
		}

		size_t Print( uint32_t AValue )
		{
			char AText[ 16 ];
			ltoa( AValue, AText, 10 );
			return Print( Mitov::String( AText ));
		}

		inline size_t Print( char AValue )
		{
			Write( (uint8_t)AValue );
			return 1;
		}

		inline size_t Print( byte AValue )
		{
			Write( AValue );
			return 1;
		}

	public:
		inline void Command(uint8_t AValue)
		{
			inherited::Send( AValue, true, false );
		}

		void Write(uint8_t AValue)
		{
			inherited::Send( AValue, false, false );
		}

	public:
		void ClearLine()
		{
			SetCursor( 0, FCursorLine );
			FCursorPos = 0;
//			for( int i = 0; i < FCols; ++ i )
//				Interface::FLcd->print( ' ' );

//			Interface::FLcd->setCursor( 0, FCursorLine );
		}

		void NewLine( int AClearSize )
		{
			for( int i = 0; i < FWidth - FCursorPos - AClearSize; ++ i )
				Print( ' ' );

			++FCursorLine;
			FCursorPos = 0;
			if( FCursorLine >= FHeight )
				FCursorLine = 0;

//			Interface::FLcd->setCursor( 0, FCursorLine );
		}

		inline void MoveChars( int ACount )
		{
			++FCursorPos;
		}

	public:
		void UpdateDisplayControl()
		{
			uint8_t AValue = 0;
			if( inherited::Enabled )
				AValue |= LCDConsts::LCD_DISPLAYON;

			if( inherited::ShowCursor )
				AValue |= LCDConsts::LCD_CURSORON;

			if( inherited::Blink )
				AValue |= LCDConsts::LCD_BLINKON;

			Command( LCDConsts::LCD_DISPLAYCONTROL | AValue );
		}

		void UpdateEntryModeSet()
		{
			uint8_t AValue = 0;

			if( ! inherited::RightToLeft )
				AValue |= LCDConsts::LCD_ENTRYLEFT;

			if( inherited::AutoScroll )
				AValue |= LCDConsts::LCD_ENTRYSHIFTINCREMENT;

			Command( LCDConsts::LCD_ENTRYMODESET | AValue );
		}

		void Clear()
		{
			Command( LCDConsts::LCD_CLEARDISPLAY );             // clear display, set cursor position to zero
			delayMicroseconds( LCDConsts::HOME_CLEAR_EXEC );    // this command is time consuming
		}

	public:
		inline void ScrollLeftInputPin_o_Receive( void * )
		{
			Command( LCDConsts::LCD_CURSORSHIFT | LCDConsts::LCD_DISPLAYMOVE | LCDConsts::LCD_MOVELEFT );
		}

		inline void ScrollRightInputPin_o_Receive( void * )
		{
			Command( LCDConsts::LCD_CURSORSHIFT | LCDConsts::LCD_DISPLAYMOVE | LCDConsts::LCD_MOVERIGHT );
		}

		inline void ClearInputPin_o_Receive( void * )
		{
			Clear();
		}

		void HomeInputPin_o_Receive( void * )
		{
			Command( LCDConsts::LCD_RETURNHOME );             // set cursor position to zero
			delayMicroseconds( LCDConsts::HOME_CLEAR_EXEC );  // This command is time consuming
		}

	public:
		inline void SystemInit()
		{
			T_BASE::DisplayInit( this );
		}

	public:
		LiquidCrystalDisplay( uint8_t AWidth, uint8_t AHeight ) :
			FWidth( AWidth ),
			FHeight( AHeight )
		{
		}

/*
		template<typename T> LiquidCrystalDisplay( uint8_t AWidth, uint8_t AHeight, T &AWire ) :
			T_BASE( AWire ),
			FWidth( AWidth ),
			FHeight( AHeight )
		{
		}
*/
	};
//---------------------------------------------------------------------------
	template<uint8_t C_PIN_COUNT> class LiquidCrystalDisplayParallel : public LiquidCrystalDisplayBasic
	{
	public:
		uint8_t *__Pins;

	protected:
		static const uint8_t	EXEC_TIME = 37;
		static const uint8_t	RS_PIN_INDEX = 0;
		static const uint8_t	RW_PIN_INDEX = 1;
		static const uint8_t	ENABLE_PIN_INDEX = 2;

	public:
		void Send( uint8_t value, bool ACommand, bool A4Bits )
		{
			digitalWrite( __Pins[ RS_PIN_INDEX ], ( ! ACommand ) );
   
			// if there is a RW pin indicated, set it low to Write
			// ---------------------------------------------------
			if( __Pins[ RW_PIN_INDEX ] != 255 ) 
				digitalWrite( __Pins[ RW_PIN_INDEX ], LOW );
   
			if( ! A4Bits )
				WriteNbits( value >> 4, 4 );

			WriteNbits( value, 4 );

			delayMicroseconds( EXEC_TIME ); // wait for the command to execute by the LCD
		}

		void WriteNbits(uint8_t value, uint8_t numBits) 
		{
			for (uint8_t i = 0; i < numBits; i++) 
				digitalWrite( __Pins[ i + 3 ], (value >> i) & 0x01);

			PulseEnable();
		}

		void PulseEnable(void) 
		{
		   // There is no need for the delays, since the digitalWrite operation
		   // takes longer.
		   digitalWrite( __Pins[ ENABLE_PIN_INDEX ], HIGH );   
		   delayMicroseconds( 1 );          // enable pulse must be > 450ns   
		   digitalWrite( __Pins[ ENABLE_PIN_INDEX ], LOW );
		}

	protected:
		template<typename T_IMPLEMENT> void InitRegisters( T_IMPLEMENT *AObject )
		{
			// 4 bit mode
			// this is according to the hitachi HD44780 datasheet
			// figure 24, pg 46

			// we start in 8bit mode, try to set 4 bit mode
			Send( 0x03, true, true );
//			delayMicroseconds(4500); // wait min 4.1ms
      
			// second try
			Send( 0x03, true, true );
//			delayMicroseconds(4500); // wait min 4.1ms
      
			// third go!
			Send( 0x03, true, true );
//			delayMicroseconds(150);
      
			// finally, set to 4-bit interface
			Send( 0x02, true, true ); 
		}

	protected:
		template<typename T_IMPLEMENT> void DisplayInit( T_IMPLEMENT *AObject )
		{
			for (uint8_t i = 0; i < C_PIN_COUNT + 3; i++)
				pinMode( __Pins[ i ], OUTPUT );

			// Now we pull both RS and R/W low to begin commands
			digitalWrite( __Pins[ RS_PIN_INDEX ], LOW);
			digitalWrite( __Pins[ ENABLE_PIN_INDEX ], LOW);
   
			if( __Pins[ RW_PIN_INDEX ] != 255 ) 
				digitalWrite( __Pins[ RW_PIN_INDEX ], LOW);

			// SEE PAGE 45/46 FOR INITIALIZATION SPECIFICATION!
			// according to datasheet, we need at least 40ms after power rises above 2.7V
			// before sending commands. Arduino can turn on way before 4.5V so we'll wait 
			// 50
			// ---------------------------------------------------------------------------
			delay( 100 ); // 100ms delay

//			Serial.println( "STEP1" );


			InitRegisters( AObject );
      
			// finally, set # lines, font size, etc.
			uint8_t ADisplayFunction = 0;
			if( C_PIN_COUNT == 8 )
				ADisplayFunction |= LCDConsts::LCD_8BITMODE;

			if( AObject->FHeight > 1 )
				ADisplayFunction |= LCDConsts::LCD_2LINE;

			if( AObject->Hight10Pixels && ( AObject->FHeight == 1 ))
				ADisplayFunction |= LCDConsts::LCD_5x10DOTS;

//			Serial.print( "ADisplayFunction: " );
//			Serial.println( ADisplayFunction, HEX );

			AObject->Command( LCDConsts::LCD_FUNCTIONSET | ADisplayFunction );  
   
//			Serial.println( "UpdateDisplayControl()" );
			AObject->UpdateDisplayControl();
   
			// clear the LCD
//			Serial.println( "Clear()" );
			AObject->Clear();

//			Serial.println( "UpdateEntryModeSet()" );
			AObject->UpdateEntryModeSet();
   
			// Initialize to default text direction (for romance languages)
//			_displaymode = LCD_ENTRYLEFT | LCD_ENTRYSHIFTDECREMENT;
			// set the entry mode
//			command(LCD_ENTRYMODESET | _displaymode);

//			UpdateBacklight();
		}

	};
//---------------------------------------------------------------------------
	template<> template<typename T_IMPLEMENT> void LiquidCrystalDisplayParallel<8>::InitRegisters( T_IMPLEMENT *AObject )
	{
		uint8_t ADisplayFunction = LCDConsts::LCD_8BITMODE;
		if( AObject->FHeight > 1 )
			ADisplayFunction |= LCDConsts::LCD_2LINE;

		if( AObject->Hight10Pixels && ( AObject->FHeight == 1 ))
			ADisplayFunction |= LCDConsts::LCD_5x10DOTS;

		// this is according to the hitachi HD44780 datasheet
		// page 45 figure 23
      
		// Send function set command sequence
		AObject->Command( LCDConsts::LCD_FUNCTIONSET | ADisplayFunction);
		delayMicroseconds(4500);  // wait more than 4.1ms
      
		// second try
		AObject->Command( LCDConsts::LCD_FUNCTIONSET | ADisplayFunction);
		delayMicroseconds(150);
      
		// third go
		AObject->Command( LCDConsts::LCD_FUNCTIONSET | ADisplayFunction);
	}
//---------------------------------------------------------------------------
	template<> void LiquidCrystalDisplayParallel<8>::Send( uint8_t value, bool ACommand, bool A4Bits )
	{
		digitalWrite( __Pins[ RS_PIN_INDEX ], ( ! ACommand ) );
   
		// if there is a RW pin indicated, set it low to Write
		// ---------------------------------------------------
		if( __Pins[ RW_PIN_INDEX ] != 255 ) 
			digitalWrite( __Pins[ RW_PIN_INDEX ], LOW );
   
		WriteNbits(value, 8); 

		delayMicroseconds( EXEC_TIME ); // wait for the command to execute by the LCD
	}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
	template<typename T_OWNER, T_OWNER &C_INSTANCE> class LiquidCrystalElementDefineCustomCharacter
	{
	public:
		const /*PROGMEM*/ uint8_t *_CharMap;

	protected:
		uint8_t	FIndex;

	public:
		void ClockInputPin_o_Receive( void *_Data )
		{
			C_INSTANCE.Command( LCDConsts::LCD_SETCGRAMADDR | (FIndex << 3));
			delayMicroseconds(30);
   
			for (int i=0; i<8; i++) 
			{
				C_INSTANCE.Write( pgm_read_byte( _CharMap + i ) );
				delayMicroseconds(40);
			}
		}

	public:
		inline void SystemInit()
		{
			ClockInputPin_o_Receive( nullptr );
		}

	public:
		LiquidCrystalElementDefineCustomCharacter( uint8_t AIndex ) :
			FIndex( AIndex )
		{
		}

	};
//---------------------------------------------------------------------------
	template<typename T_OWNER, T_OWNER &C_INSTANCE> class LiquidCrystalElementCustomCharacterField
	{
	public:
		uint8_t Column : 7;
		uint8_t Row : 7;

	public:
		uint8_t Index : 3;

	protected:
		bool	FModified : 1;

	public:
		void SetIndex( uint8_t	AValue )
		{
			if( AValue > 7 )
				AValue = 7;

			if( Index == AValue )
				return;

			Index = AValue;
			FModified = true;
		}

		void SetColumn( uint8_t AValue )
		{
			if( Column == AValue )
				return;

			Column = AValue;
			FModified = true;
		}

		void SetRow( uint8_t AValue )
		{
			if( Row == AValue )
				return;

			Row = AValue;
			FModified = true;
		}

	public:
		inline void ClockInputPin_o_Receive( void *_Data )
		{
			DisplayCharacter();
		}

	public:
		inline void SystemStart()
		{
			DisplayCharacter();
		}

		inline void SystemLoopEnd( unsigned long currentMicros )
		{
			if( FModified )
			{
				DisplayCharacter();
				FModified = false;
			}
		}

	protected:
		void DisplayCharacter()
		{
//			FLcd->setCursor( Column, Row );
//			FLcd->write( (uint8_t) Index );
			C_INSTANCE.SetCursor( Column, Row );
			C_INSTANCE.Write( (uint8_t) Index );
		}

	public:
		LiquidCrystalElementCustomCharacterField() :
			Column( 0 ),
			Row( 0 ),
			Index( 0 ),
			FModified( false )
		{
		}

	};
//---------------------------------------------------------------------------
	template<typename T_LCD, T_LCD &C_LCD, typename T> class LiquidCrystalDisplayInput
	{
	public:
		void InputPin_o_Receive( void *_Data )
		{
			C_LCD.ClearLine();
			int AClearSize = C_LCD.Print( *(T*)_Data );
			C_LCD.NewLine( AClearSize );
		}

	};
//---------------------------------------------------------------------------
	template<typename T_LCD, T_LCD &C_LCD> class LiquidCrystalDisplayInput<T_LCD, C_LCD, bool>
	{
	public:
		inline void InputPin_o_Receive( void *_Data )
		{
			C_LCD.ClearLine();
			int AClearSize = C_LCD.Print( *(bool*)_Data ? "true" : "false" );
			C_LCD.NewLine( AClearSize );
		}

	};
//---------------------------------------------------------------------------
	template<typename T_LCD, T_LCD &C_LCD> class LiquidCrystalDisplayCharInput
	{
	public:
		void InputPin_o_Receive( void *_Data )
		{
//			Serial.println( "DoReceive" );
//			Serial.print( (char*)_Data );
//			if( *(byte*)_Data < ' ' )
//				return;

			if( *(char*)_Data == '\r' )
				C_LCD.NewLine( 0 );

			else if( *(char*)_Data == '\n' )
				C_LCD.ClearLine();

			else if( *(char*)_Data >= ' ' )
			{
				int AClearSize = C_LCD.Print( *(char*)_Data );
				C_LCD.MoveChars( AClearSize );
			}
//			int AClearSize = C_LCD.FLcd->print( (char*)_Data );
//			C_LCD.NewLine( AClearSize );
		}

	};
//---------------------------------------------------------------------------
	template<typename T_LCD, T_LCD &C_LCD> class LiquidCrystalDisplayStringInput
	{
	public:
		void InputPin_o_Receive( void *_Data )
		{
			C_LCD.ClearLine();
			int AClearSize = C_LCD.Print( (char*)_Data );
			C_LCD.NewLine( AClearSize );
		}

	};
//---------------------------------------------------------------------------
	template<typename T_LCD, T_LCD &C_LCD> class LiquidCrystalDisplayClockInput
	{
	public:
		void InputPin_o_Receive( void *_Data )
		{
			C_LCD.ClearLine();
			int AClearSize = C_LCD.Print( "(Clock)" );
			C_LCD.NewLine( AClearSize );
		}

	};
//---------------------------------------------------------------------------
	template<typename T_LCD, T_LCD &C_LCD, typename T_OBJECT> class LiquidCrystalDisplayObjectInput
	{
	public:
		void InputPin_o_Receive( void *_Data )
		{
			C_LCD.ClearLine();
			int AClearSize = C_LCD.Print( ((T_OBJECT *)_Data)->ToString().c_str() );
			C_LCD.NewLine( AClearSize );
		}

	};
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
	template<typename T_OWNER, T_OWNER &C_INSTANCE> class LiquidCrystalElementBasicPositionedWidthField
	{
	public:
		uint8_t Column : 7;
		uint8_t Row : 7;

	public:
		uint8_t Width : 7;
		bool	AllignLeft : 1;
		char	FillCharacter = ' ';

	public:
		void ClearLine()
		{
			C_INSTANCE.SetCursor( Column, Row );
		}

		void NewLine( int AClearSize )
		{
			for( int i = 0; i < (int)Width - AClearSize; ++ i )
				C_INSTANCE.Print( FillCharacter );
		}

	public:
		LiquidCrystalElementBasicPositionedWidthField() :
			Column( 0 ),
			Row( 0 ),
			Width( 16 ),
			AllignLeft( false )
		{
		}

	};
//---------------------------------------------------------------------------
	template<typename T_OWNER, T_OWNER &C_INSTANCE> class LiquidCrystalElementTextField : public LiquidCrystalElementBasicPositionedWidthField<T_OWNER, C_INSTANCE>
	{
		typedef Mitov::LiquidCrystalElementBasicPositionedWidthField<T_OWNER, C_INSTANCE> inherited;

	public:
		Mitov::String	InitialValue;

	public:
/*
		template<typename T> inline size_t Print( T AValue )
		{
			C_INSTANCE.Print( AValue );
		}
*/
		size_t Print( const Mitov::String AValue )
		{
			const char *APtr = AValue.c_str();
			size_t ACount = 0;
			while( *APtr )
			{
				if( *APtr == '\r' )
				{
					inherited::ClearLine();
					ACount = 0;
					continue;
				}

				C_INSTANCE.Write( *APtr++ );
				++ ACount;
				if( ACount == inherited::Width )
					break;

			}

			return ACount;
		}

		size_t Print( float AValue )
		{
			char AText[ 16 ];
			dtostrf( AValue,  1, 2, AText );
			return Print( Mitov::String( AText ));
		}

		size_t Print( int32_t AValue )
		{
			char AText[ 16 ];
			ltoa( AValue, AText, 10 );
			return Print( Mitov::String( AText ));
		}

		size_t Print( uint32_t AValue )
		{
			char AText[ 16 ];
			ltoa( AValue, AText, 10 );
			return Print( Mitov::String( AText ));
		}

		inline size_t Print( char AValue )
		{
			C_INSTANCE.Write( (uint8_t)AValue );
			return 1;
		}

		inline size_t Print( byte AValue )
		{
			C_INSTANCE.Write( AValue );
			return 1;
		}

	public:
		inline void SystemStart()
		{
			inherited::ClearLine();
			int AClearSize = C_INSTANCE.Print( InitialValue );
			inherited::NewLine( AClearSize );
		}

	};
//---------------------------------------------------------------------------
	template<typename T_OWNER, T_OWNER &C_INSTANCE> class LiquidCrystalElementTextField_Clocked : public LiquidCrystalElementTextField<T_OWNER, C_INSTANCE>
	{
		typedef Mitov::LiquidCrystalElementTextField<T_OWNER, C_INSTANCE> inherited;

	public:
		inline void ClearLine()
		{
			inherited::InitialValue = "";
		}

		inline void NewLine( int AClearSize ) {}

		size_t Print( const Mitov::String AValue )
		{
			const char *APtr = AValue.c_str();
			size_t ACount = 0;
			while( *APtr )
			{
				if( *APtr == '\r' )
				{
					ClearLine();
					ACount = 0;
					continue;
				}

				Write( *APtr++ );
				++ ACount;
			}

			return ACount;
		}

		size_t Print( float AValue )
		{
			char AText[ 16 ];
			dtostrf( AValue,  1, 2, AText );
			inherited::InitialValue = AText;
			return inherited::InitialValue.length();
		}

		size_t Print( int32_t AValue )
		{
			char AText[ 16 ];
			ltoa( AValue, AText, 10 );
			inherited::InitialValue = AText;
			return inherited::InitialValue.length();
		}

		size_t Print( uint32_t AValue )
		{
			char AText[ 16 ];
			ltoa( AValue, AText, 10 );
			inherited::InitialValue = AText;
			return inherited::InitialValue.length();
		}

		inline size_t Print( char AValue )
		{
			inherited::InitialValue += AValue;
			return 1;
		}

		inline size_t Print( byte AValue )
		{
			inherited::InitialValue += *(char *)&AValue;
			return 1;
		}

		inline void Write(uint8_t AValue)
		{
			inherited::InitialValue += *(char *)&AValue;
		}

		void ClockInputPin_o_Receive( void *_Data )
		{
			inherited::ClearLine();
			int AClearSize = inherited::Print( inherited::InitialValue );
			inherited::NewLine( AClearSize );
		}

	};
//---------------------------------------------------------------------------
	class LiquidCrystalElementIntegerField
	{
	public:
		uint8_t	Base = 10;

	protected:
		template<typename T_LCD, typename T> int PrintValue( T_LCD &ADisplay, T *ACaller, int32_t AValue )
		{
			char AText[ 16 ];
			ltoa( AValue, AText, Base );
			if( ACaller->Width < 15 )
				AText[ ACaller->Width ] = '\0';

			if( ACaller->AllignLeft )
				return ADisplay.Print( AText );

			for( int i = 0; i < ACaller->Width - strlen( AText ); ++i )
				ADisplay.Print( ACaller->FillCharacter );

			ADisplay.Print( AText );
			return ACaller->Width;
		}

	};
//---------------------------------------------------------------------------
	class LiquidCrystalElementUnsignedField
	{
	public:
		uint8_t	Base = 10;

	protected:
		template<typename T_LCD, typename T> int PrintValue( T_LCD &ADisplay, T *ACaller, uint32_t AValue )
		{
			char AText[ 16 ];
			ltoa( AValue, AText, Base );
			if( ACaller->Width < 15 )
				AText[ ACaller->Width ] = '\0';

			return ADisplay.Print( AText );
		}

	};
//---------------------------------------------------------------------------
	class LiquidCrystalElementAnalogField
	{
	public:
		uint8_t	MinWidth = 1;
		uint8_t	Precision = 3;

	protected:
		template<typename T_LCD, typename T> int PrintValue( T_LCD &ADisplay, T *ACaller, float AValue )
		{
			char AText[ 16 ];
			dtostrf( AValue,  MinWidth, Precision, AText );
			if( ACaller->Width < 15 )
				AText[ ACaller->Width ] = '\0';

//			Serial.println( AText );
			return ADisplay.Print( AText );
		}

	};
//---------------------------------------------------------------------------
	class LiquidCrystalElementDigitalField
	{
	public:
		Mitov::String	TrueValue = "true";
		Mitov::String	FalseValue = "false";

	protected:
		template<typename T_LCD, typename T> int PrintValue( T_LCD &ADisplay, T *ACaller, bool AValue )
		{
			if( AValue )
				return ADisplay.Print( TrueValue );

			return ADisplay.Print( FalseValue );
		}

	};
//---------------------------------------------------------------------------
	template<typename T_OWNER, T_OWNER &C_INSTANCE, typename T_BASE_CLASS, typename T> class LiquidCrystalElementBasicPositionedWidthTypedField : public LiquidCrystalElementBasicPositionedWidthField<T_OWNER, C_INSTANCE>, public T_BASE_CLASS
	{
		typedef LiquidCrystalElementBasicPositionedWidthField<T_OWNER, C_INSTANCE> inherited;

	public:
		T	InitialValue;

	protected:
		void ClearPrintValue( T AValue )
		{
			InitialValue = AValue;
//			Serial.println( AValue );
			inherited::ClearLine();
			int AClearSize = T_BASE_CLASS::PrintValue( C_INSTANCE, this, AValue );
			inherited::NewLine( AClearSize );
		}

	public:
		inline void SystemStart()
		{
			ClearPrintValue( InitialValue );
		}

	public:
		inline void InputPin_o_Receive( void *_Data )
		{
//			Serial.println( "TEST" );
			ClearPrintValue( *(T*)_Data );
		}

	public:
		LiquidCrystalElementBasicPositionedWidthTypedField( T AInitialValue ) :
			InitialValue( AInitialValue )
		{
		}

	};
//---------------------------------------------------------------------------
	template<typename T_OWNER, T_OWNER &C_INSTANCE, typename T_BASE_CLASS, typename T> class LiquidCrystalElementBasicPositionedWidthTypedField_Clocked : public LiquidCrystalElementBasicPositionedWidthTypedField<T_OWNER, C_INSTANCE, T_BASE_CLASS, T>
	{
		typedef LiquidCrystalElementBasicPositionedWidthTypedField<T_OWNER, C_INSTANCE, T_BASE_CLASS, T> inherited;

	public:
		inline void InputPin_o_Receive( void *_Data )
		{
			inherited::InitialValue = *(T*)_Data;
		}

		inline void ClockInputPin_o_Receive( void *_Data )
		{
			inherited::ClearPrintValue( inherited::InitialValue );
		}

	public:
		using inherited::inherited;

	};
//---------------------------------------------------------------------------
}


#ifdef __TEST_FOR_DEBUG_PRINTS__
#undef Serial
#endif

#endif
