////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2018 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_MICRO_SD_CARD_h
#define _MITOV_MICRO_SD_CARD_h

#include <Mitov.h>

#include <SPI.h>
#include <SD.h>

#ifdef __TEST_FOR_DEBUG_PRINTS__
#define Serial UNGUARDED DEBUG PRINT!!!
#endif

namespace Mitov
{
#ifdef VISUINO_ESP32
	typedef SDFS SDClass;
#endif
//---------------------------------------------------------------------------
	class MicroSDCardElementOperation
	{
	public:
		OpenWire::SourcePin	OutputPin;

	};
//---------------------------------------------------------------------------
	template<typename T_OWNER, T_OWNER &C_OWNER> class MicroSDCardElementDirectory
	{
	public:
		Mitov::String	PathName;

	public:
		MicroSDCardElementOperation	CheckExists;
		MicroSDCardElementOperation	Create;
		MicroSDCardElementOperation	Remove;

	public:
		inline bool DoExists( Mitov::String APathName )
		{
			return C_OWNER.DoExists( JoinPath( APathName ));
		}

		inline bool DoCreate( Mitov::String APathName )
		{
			return C_OWNER.DoCreate( JoinPath( APathName ));
		}

		inline bool DoRemove( Mitov::String APathName )
		{
			return C_OWNER.DoRemove( JoinPath( APathName ));
		}

		inline bool DoOpen( Mitov::String APathName, MITOV_FILE_MODE mode, File &AFile )
		{
			return C_OWNER.DoOpen( JoinPath( APathName ), mode, AFile );
		}

	protected:
		Mitov::String JoinPath( Mitov::String AValue )
		{
			Mitov::String APath = PathName;
			if( ! APath.endsWith( "/" ) )
				APath = APath + "/";

			if( AValue.startsWith( "/" ) )
				AValue.remove( 0, 1 );

			return APath + AValue;
		}

	public:
		void CheckExists_ClockInputPin_o_Receive( void *_Data )
		{
			CheckExists.OutputPin.SendValue( C_OWNER.DoExists( PathName ));
		}

		void Create_ClockInputPin_o_Receive( void *_Data )
		{
			Create.OutputPin.SendValue( C_OWNER.DoCreate( PathName ));
		}

		void Remove_ClockInputPin_o_Receive( void *_Data )
		{
			Remove.OutputPin.SendValue( C_OWNER.DoRemove( PathName ));
		}

	};
//---------------------------------------------------------------------------
	template<typename T_OWNER, T_OWNER &C_OWNER> class MicroSDCardElementList
	{
	public:
		OpenWire::SourcePin	OutputPin;
#ifdef MICROSD_LIST_USE_FAILED_OUTPUT_PIN
		OpenWire::SourcePin	FailedOutputPin;
#endif

	public:
		Mitov::String	PathName;

	public:
		bool	Recursive = false;

	protected:
		void ProcessLevel( Mitov::String APath, Mitov::String APrefix )
		{
			File AFile;
			if( ! C_OWNER.DoOpen( APath, FILE_READ, AFile ))
			{
#ifdef MICROSD_LIST_USE_FAILED_OUTPUT_PIN
				FailedOutputPin.Notify( nullptr );
#endif
#ifdef MICROSD_USE_ERROR_INFO_PIN
				C_OWNER.ErrorInfoOutputPin.SendValue( "Missing Path" );
#endif
				return;
			}

			if( APath.endsWith( "/" ))
				APath = APath.substring( 0, APath.length() - 1 );

			while (true) 
			{
				File AEntry =  AFile.openNextFile();
				if (! AEntry) 
				  // no more files
				  break;

				Mitov::String AItemName = APrefix + AEntry.name(); 
				OutputPin.Notify( (char *)AItemName.c_str() );

				if( Recursive )
					if( AEntry.isDirectory())
						ProcessLevel( APath + '/' + AEntry.name(), AItemName + '/' );

				AEntry.close();
			  }

			AFile.close();
		}

	public:
		void ClockInputPin_o_Receive( void *_Data )
		{
//			Serial.println( "" );
//			Serial.println( PathName );
//			Serial.println( "" );
			if( PathName = "" )
				ProcessLevel( "/", "" );

			else
				ProcessLevel( PathName, "" );
/*
			File AFile;
			if( ! C_OWNER.DoOpen( PathName, FILE_READ, AFile ))
			{
				FailedOutputPin.Notify( nullptr );
				return;
			}

			while (true) 
			{
				File AEntry =  AFile.openNextFile();
				if (! AEntry) 
				  // no more files
				  break;

				OutputPin.Notify( AEntry.name() );
				AEntry.close();
			  }

			AFile.close();
*/
		}

	public:
		inline void SystemStart()
		{
			ClockInputPin_o_Receive( nullptr );
		}

	};
//---------------------------------------------------------------------------
	template<typename T_OWNER, T_OWNER &C_OWNER> class MicroSDCardElementFileSize
	{
	public:
		OpenWire::SourcePin	OutputPin;

	};
//---------------------------------------------------------------------------
	template<typename T_OWNER, T_OWNER &C_OWNER> class MicroSDCardElementFile
	{
	public:
#ifdef MicroSDCardElementFile_ReadPositionOutputPin
		OpenWire::SourcePin	ReadPositionOutputPin;
#endif

#ifdef MicroSDCardElementFile_WritePositionOutputPin
		OpenWire::SourcePin	WritePositionOutputPin;
#endif

	public:
		Mitov::String	PathName;

	public:
		bool Enabled : 1;
		bool NewLine : 1;
		bool KeepFlushed : 1;
		bool KeepClosed : 1;

	public:
		bool FHasWrite : 1;

	protected:
		bool        FReadMoved : 1;
		bool        FWriteMoved : 1;

		uint16_t	FWriteCount = 0;

		uint32_t	FWritePosition = 0;
		uint32_t	FReadPosition = 0;

	public:
		File FFile;

	public:
#ifdef MicroSDCardElementFile_Size
		MicroSDCardElementFileSize<T_OWNER, C_OWNER> Size;
#endif

	public:
		void PrintChar( char AValue )
		{
			if( ! TryOpen() )
				return;

			PrepareWrite();
			FFile.print( AValue );
			if( AValue == 10 )
				UpdateWrite();

		}

		template<typename T> void Print( T AValue )
		{
			if( ! TryOpen() )
				return;

			PrepareWrite();

//			Serial.println( "TEST2" );
//			Serial.println( AValue );
			if( NewLine )
				FFile.println( AValue );

			else
				FFile.print( AValue );

			UpdateWrite();
		}

		void Write( void *_Data, size_t ASize )
		{
			if( ! TryOpen() )
				return;

			PrepareWrite();
			FFile.write( (uint8_t *)_Data, ASize );
			FWriteCount += ASize;
			if( FWriteCount > 2000 )
			{
				FWriteCount = 0;
				UpdateWrite();
			}

		}

		void BeginRead()
		{
			if( ! FWriteMoved )
			{
				FWritePosition = FFile.position();
				FWriteMoved = true;
			}

			if( FReadMoved )
			{
				FFile.seek( FReadPosition );
				FReadMoved = false;
			}
		}

		inline void ResetReadPosition()
		{
			FFile.seek( FReadPosition );
		}

	protected:
		void PrepareWrite()
		{
			if( ! FReadMoved )
			{
				FReadPosition = FFile.position();
#ifdef MicroSDCardElementFile_ReadPositionOutputPin
				ReadPositionOutputPin.Notify( &FReadPosition );
#endif
				FReadMoved = true;
			}

			if( FWriteMoved )
			{
				FFile.seek( FWritePosition );
				FWriteMoved = false;
			}
		}

		void UpdateWrite()
		{
			if( KeepFlushed )
				FFile.flush();

			if( KeepClosed )
				FFile.close();
			
#ifdef MicroSDCardElementFile_WritePositionOutputPin
			if( WritePositionOutputPin.IsConnected() )
				WritePositionOutputPin.SendValue( FFile.position() );
#endif
		}

	public:
		bool TryOpen()
		{
			if( FFile )
				return true;

//			Serial.println( "OPEN1" );
			if( ! Enabled )
				return false;

			if( PathName == "" )
				return false;

//			Serial.println( PathName );
//			Serial.println( FHasWrite );
			if( FHasWrite )
				return C_OWNER.DoOpen( PathName, FILE_WRITE, FFile );

			else
				return C_OWNER.DoOpen( PathName, FILE_READ, FFile );
		}

	public:
		inline void CloseInputPin_o_Receive( void *_Data )
		{
			FFile.close();
		}

		inline void FlushInputPin_o_Receive( void *_Data )
		{
			FFile.flush();
		}

		inline void Size_ClockInputPin_o_Receive( void *_Data )
		{
#ifdef MicroSDCardElementFile_Size
			Size.OutputPin.SendValue( FFile.size() );
#endif
		}

	public:
		MicroSDCardElementFile( bool AHasWrite ) :
			Enabled( true ),
			NewLine( true ),
			KeepFlushed( true ),
			KeepClosed( true ),
			FHasWrite( AHasWrite ),
			FReadMoved( false ),
			FWriteMoved( false )
		{
		}

	};
//---------------------------------------------------------------------------
	template<typename T_OWNER, T_OWNER &C_OWNER> class MicroSDCardFileElementReadTextLine
	{
    public:
		OpenWire::SourcePin	OutputPin;

	protected:
		bool readStringLine( Mitov::String &AResult )
		{
//			AResult = "";
			int c = C_OWNER.FFile.read();
//			Serial.println( c );
			if( c < 0 )
				return false;

//			Serial.println( "R1" );
			while (c >= 0 && c != '\n' && c != '\r' )
			{
				AResult += (char)c;
				c = C_OWNER.FFile.read();
			}

			while ( c >= 0 )
			{
				c = C_OWNER.FFile.peek();
				if( c != '\n' && c != '\r' )
					break;

				C_OWNER.FFile.read();
			}
/*
			while ( c >= 0 && ( c == '\n' || c == '\r' ))
				c = C_OWNER.FFile.read();
*/
			return true;
		}

	public:
		void ClockInputPin_o_Receive( void *_Data )
		{
			if( ! C_OWNER.TryOpen() )
				return;

			Mitov::String ALine;			
			C_OWNER.BeginRead();
//			Serial.println( "READ1" );
			if( readStringLine( ALine ))
				OutputPin.Notify( (void *)ALine.c_str() );

		}

	};
//---------------------------------------------------------------------------
	template<typename T_OWNER, T_OWNER &C_OWNER, typename T> class MicroSDCardFileElementReadTyped
	{
    public:
		OpenWire::SourcePin	OutputPin;

    public:
		void ClockInputPin_o_Receive( void *_Data )
		{
			if( ! C_OWNER.TryOpen() )
				return;

			C_OWNER.BeginRead();

			T AData;
#ifdef VISUINO_ESP32
			size_t AReadSize = C_OWNER.FFile.readBytes( (char *)&AData, sizeof( AData ));
#else
			size_t AReadSize = C_OWNER.FFile.readBytes( (uint8_t *)&AData, sizeof( AData ));
#endif
			if( AReadSize == sizeof( AData ) )
				OutputPin.Notify( &AData );

//			else
//				C_OWNER.ResetReadPosition();
		}

	};
//---------------------------------------------------------------------------
	template<typename T_OWNER, T_OWNER &C_OWNER, typename T> class MicroSDCardFileElementWriteTyped
	{
    public:
		inline void InputPin_o_Receive( void *_Data )
		{
			C_OWNER.Write( _Data, sizeof( T ));
		}

	public:
		MicroSDCardFileElementWriteTyped()
		{
			C_OWNER.FHasWrite = true;
		}

	};
//---------------------------------------------------------------------------
	template<typename T_OWNER, T_OWNER &C_OWNER> class MicroSDCardElementExists
	{
	public:
		OpenWire::SourcePin	OutputPin;

	public:
		Mitov::String	PathName;

	public:
		inline void SystemStart()
		{
			ClockInputPin_o_Receive( nullptr );
		}

	public:
		inline void ClockInputPin_o_Receive( void *_Data )
		{
			OutputPin.SendValue( C_OWNER.DoExists( PathName ));
		}

	};
//---------------------------------------------------------------------------
	template<int PIN_NUMBER> class MicroSDCard
	{
	public:
#ifdef MICROSD_USE_FAILED_OUTPUT_PIN
		OpenWire::SourcePin	FailedOutputPin;
#endif
#ifdef MICROSD_USE_SUCCESS_OUTPUT_PIN
		OpenWire::SourcePin	SuccessOutputPin;
#endif
#ifdef MICROSD_USE_ERROR_INFO_PIN
		OpenWire::SourcePin	ErrorInfoOutputPin;
#endif
	public:
		bool	Enabled = true;

	public:
		bool DoExists( Mitov::String APathName )
		{
			if( !FCard )
				return false;

//			Serial.println( "COOL1" );
//			Serial.println( APathName );
			char *ATmp = new char[ APathName.length() + 1 ];
			memcpy( ATmp, APathName.c_str(), APathName.length() + 1 );

			bool AResult = FCard->exists( ATmp );

			delete [] ATmp;

			return AResult;
		}

		bool DoCreate( Mitov::String APathName )
		{
			if( !FCard )
				return false;

			char *ATmp = new char[ APathName.length() + 1 ];
			memcpy( ATmp, APathName.c_str(), APathName.length() + 1 );

			bool AResult = FCard->mkdir( ATmp );

			delete [] ATmp;

			return AResult;
		}

		bool DoRemove( Mitov::String APathName )
		{
			if( !FCard )
				return false;

			char *ATmp = new char[ APathName.length() + 1 ];
			memcpy( ATmp, APathName.c_str(), APathName.length() + 1 );

			bool AResult = FCard->rmdir( ATmp );

			delete [] ATmp;

			return AResult;
		}

		bool DoOpen( Mitov::String APathName, MITOV_FILE_MODE mode, File &AFile )
		{
//			Serial.print( "O1 :" );
			if( !FCard )
				return false;

			if( ! APathName.startsWith( "/" ))
				APathName = Mitov::String( "/" ) + APathName;

//			char *ATmp = new char[ APathName.length() + 1 ];
//			memcpy( ATmp, APathName.c_str(), APathName.length() + 1 );

//			Serial.print( "T1 :" );
//			Serial.println( ATmp );
			AFile = FCard->open( APathName.c_str(), mode );
//			AFile = FCard->open( "/LOG.TXT", mode );
//			delete [] ATmp;

//			Serial.println( AFile );

			return AFile;
		}

	public:
		SDClass	*FCard = nullptr;
		
	public:
		void UpdateEnabled()
		{
			if( Enabled )
			{
//				Serial.println( "TEST1" );
#if defined( VISUINO_ESP32 ) || defined( VISUINO_TEENSY_3_6 )
				FCard = &SD;
#else
				FCard = new SDClass;				
#endif
				if( FCard->begin( PIN_NUMBER ) )
#ifdef MICROSD_USE_SUCCESS_OUTPUT_PIN
					SuccessOutputPin.Notify( nullptr )
#endif
					;

				else
				{
//					Serial.println( "FAIL" );
#ifdef MICROSD_USE_FAILED_OUTPUT_PIN
					FailedOutputPin.Notify( nullptr );
#endif
#ifdef MICROSD_USE_ERROR_INFO_PIN
					ErrorInfoOutputPin.SendValue( "Fail to open microSD" );
#endif

					Enabled = false;
					UpdateEnabled();
				}
//				Serial.println( "COOL" );
			}
			else
			{
#if defined( VISUINO_ESP32 ) || defined( VISUINO_TEENSY_3_6 )
				delete FCard;
#endif
				FCard = nullptr;
			}
		}

	public:
		inline void SystemInit()
		{
			UpdateEnabled();
		}

/*
	public:
		virtual ~MicroSDCard()
		{
			if( FCard )
				delete FCard;
		}
*/
	};
//---------------------------------------------------------------------------
}

#ifdef __TEST_FOR_DEBUG_PRINTS__
#undef Serial
#endif

#endif
