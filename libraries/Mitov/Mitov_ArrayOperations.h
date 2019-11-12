////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2018 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_ARRAY_OPERATIONS_h
#define _MITOV_ARRAY_OPERATIONS_h

#include <Mitov.h>

#ifdef __TEST_FOR_DEBUG_PRINTS__
#define Serial UNGUARDED DEBUG PRINT!!!
#endif

namespace Mitov
{
	template<typename T> class ValueArraySource
	{
	public:
		OpenWire::SourcePin	OutputPin;

	public:
		T *_Values;
		uint32_t _Count;

	protected:
		void SetValue()
		{
			if( ! _Count )
				return;

			TArray<T> ABuffer( _Count, _Values );
			OutputPin.Notify( &ABuffer );
		}

	public:
		inline void SystemStart()
		{
			SetValue();
		}

	public:
		inline void ClockInputPin_o_Receive( void *_Data )
		{
			SetValue();
		}

	};
//---------------------------------------------------------------------------
	template<typename T> class ArrayDeleteItems
	{
	public:
		OpenWire::SourcePin	OutputPin;

	public:
		uint8_t Index = 0;
		uint8_t Count = 1;

	public:
		void InputPin_o_Receive( void *_Data )
		{
			TArray<T> *AData = (TArray<T>*)_Data;
			if( Count == 0 || ( Index > AData->Size ))
			{
				OutputPin.Notify( _Data );
				return;
			}

			uint8_t ACount = MitovMin<int>( Count, int( AData->Size ) - int( Index ) );
			if( ACount == AData->Size )
				return;

			uint8_t AOutSize = AData->Size - ACount;
			T *AOutData = new T[ AOutSize ];
			for( int i = 0; i < Index; ++i )
				AOutData[ i ] = AData->Data[ i ];

			for( int i = 0; i < AOutSize; ++i )
				AOutData[ Index + i ] = AData->Data[ Index + ACount + i ];

			TArray<T> ABuffer( AOutSize, AOutData );
			OutputPin.Notify( &ABuffer );
			delete [] AOutData;
		}

	};
//---------------------------------------------------------------------------
	template<typename T> class ArrayInsertItems
	{
	public:
		OpenWire::SourcePin	OutputPin;

	public:
		uint8_t Index = 0;

	public:
		T *_Values;
		uint32_t _Count;

	public:
		void InputPin_o_Receive( void *_Data )
		{
			TArray<T> *AData = (TArray<T>*)_Data;

			if( _Count == 0 || ( Index > AData->Size ))
			{
				OutputPin.Notify( _Data );
				return;
			}

			uint8_t AOutSize = AData->Size + _Count;
			T *AOutData = new T[ AOutSize ];

			for( int i = 0; i < Index; ++i )
				AOutData[ i ] = AData->Data[ i ];

			for( int i = 0; i < _Count; ++i )
				AOutData[ i + Index ] = _Values[ i ];

			for( int i = Index; i < AData->Size; ++i )
				AOutData[ i + _Count ] = AData->Data[ i ];

			TArray<T> ABuffer( AOutSize, AOutData );
			OutputPin.Notify( &ABuffer );
			delete [] AOutData;
		}
	};
//---------------------------------------------------------------------------
	template<typename T> class ArrayReplaceItemsAtIndex
	{
	public:
		OpenWire::SourcePin	OutputPin;

	public:
		uint8_t Index = 0;

	public:
		T *_Values;
		uint32_t _Count;

	public:
		void InputPin_o_Receive( void *_Data )
		{
			TArray<T> *AData = (TArray<T>*)_Data;

			uint8_t AOutSize = AData->Size;
			if( _Count == 0 || ( Index > AOutSize ))
			{
				OutputPin.Notify( _Data );
				return;
			}

			uint8_t ACount = MitovMin<int>( _Count, int( AOutSize ) - int( Index ) );
			if( ACount == AData->Size )
				return;

			T *AOutData = new T[ AOutSize ];

			for( int i = 0; i < Index; ++i )
				AOutData[ i ] = AData->Data[ i ];

			for( int i = 0; i < ACount; ++i )
				AOutData[ i + Index ] = _Values[ i ];

			for( int i = Index + ACount; i < AOutSize; ++i )
				AOutData[ i ] = AData->Data[ i ];

//			for( int i = 0; i < Index; ++i )
//				AOutData[ i ] = AData->Data[ i ];

			TArray<T> ABuffer( AOutSize, AOutData );
			OutputPin.Notify( &ABuffer );
			delete [] AOutData;

//			OutputPin.Notify( &ABuffer );
//			uint8_t AOutSize = AData->Size;

/*
			if( _Count == 0 || ( Index > AData->Size ))
			{
				OutputPin.Notify( _Data );
				return;
			}

			uint8_t AOutSize = AData->Size + _Count;
			T *AOutData = new T[ AOutSize ];

			for( int i = 0; i < Index; ++i )
				AOutData[ i ] = AData->Data[ i ];

			for( int i = 0; i < _Count; ++i )
				AOutData[ i + Index ] = _Values[ i ];

			for( int i = Index; i < AData->Size; ++i )
				AOutData[ i + _Count ] = AData->Data[ i ];

			TArray<T> ABuffer( AOutSize, AOutData );
			OutputPin.Notify( &ABuffer );
			delete [] AOutData;
*/
		}
	};
//---------------------------------------------------------------------------
	template<typename T> class ArrayReplace
	{
	public:
		OpenWire::SourcePin	OutputPin;

	public:
		T *_FromValues;
		uint32_t _FromCount;

		T *_ToValues;
		uint32_t _ToCount;

	public:
		void InputPin_o_Receive( void *_Data )
		{
			if( _FromCount == 0 )
			{
				OutputPin.Notify( _Data );
				return;
			}

			TArray<T> &AData = *(TArray<T>*)_Data;
			TValueArray<T> AOutData;
			if( _FromCount > AData.Size )
			{
				OutputPin.Notify( _Data );
				return;
			}

			int AEndIndex = AData.Size;

//			Serial.println( AData.ToString() );

			for( int i = AData.Size - _FromCount + 1; i-- > 0; )
			{
//				for( int j = 0; j < _FromCount; ++j )
				bool AFound = true;
				for( int j = _FromCount; j--; )
				{
//					Serial.print( i ); Serial.print( " + " ); Serial.print( j ); Serial.print( ": " ); Serial.print( AData.Data[ i + j ] ); Serial.print( " " ); Serial.println( _FromValues[ j ] );
					if( AData.Data[ i + j ] != _FromValues[ j ] )
					{
						AFound = false;
						break;
					}
				}

				if( AFound )
				{
//					Serial.println( "Test" );
					uint32_t ASubSize = AEndIndex - i - _FromCount;
					AOutData.ResizeFront( AOutData.Size + ASubSize + _ToCount );

//					Serial.println( ASubSize );
//					delay( 1000 );
					AOutData.Copy( AData.Data + i + _FromCount, ASubSize, _ToCount );
//					Serial.println( AOutData.ToString() );
					AOutData.Copy( _ToValues, _ToCount, 0 );
//					Serial.println( AOutData.ToString() );
					AEndIndex = i;
					i -= _FromCount - 1;
				}
			}

			if( AEndIndex )
			{
				AOutData.ResizeFront( AOutData.Size + AEndIndex );
				AOutData.Copy( AData.Data, AEndIndex, 0 );
			}

			OutputPin.Notify( &AOutData );
		}
	};
//---------------------------------------------------------------------------
}

#ifdef __TEST_FOR_DEBUG_PRINTS__
#undef Serial
#endif

#endif
