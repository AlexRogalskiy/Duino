////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2018 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_STRUCTURE_h
#define _MITOV_STRUCTURE_h

#include <Mitov.h>

namespace Mitov
{
//---------------------------------------------------------------------------
	class BasicMakeStructureElement
	{
	public:
		bool	FPopulated : 1;
		bool	FModified : 1;

	public:
		virtual	void GetData( uint8_t *&AData, uint8_t &AOffset ) {}

	public:
		BasicMakeStructureElement() :
			FPopulated( false ),
			FModified( false )
		{
		}

	};
//---------------------------------------------------------------------------
	class BasicSplitStructureElement
	{
	public:
		virtual void Extract( uint8_t *&AData, uint8_t &AOffset ) {}

	};
//---------------------------------------------------------------------------
	template<typename T_PIN,typename T> class BasicTypedStructureSinkElement : public BasicMakeStructureElement
	{
	protected:
		T	FValue;

	public:
		void InputPin_o_Receive( void *_Data )
		{
			T AValue = *(T_PIN*)_Data;
			if( FPopulated )
				if( AValue == FValue )
					return;

			FPopulated = true;
			FValue = AValue;
			FModified = true;
		}

	public:
		virtual	void GetData( uint8_t *&AData, uint8_t &AOffset ) override
		{ 
			if( AOffset )
			{
//				Serial.println( "TTT" );
				++AData;
				AOffset = 0;
			}

#ifdef VISUINO_MEMORY_ALLIGN
			memcpy( AData, &FValue, sizeof( T ));
#else
			*(T*)AData = FValue;
#endif
//			memcpy( AData, &FValue, sizeof( T ));
			AData += sizeof( T );
			FModified = false;
		}

	};
//---------------------------------------------------------------------------
	template<typename T_PIN,typename T> class BasicTypedStructureSourceElement : public BasicSplitStructureElement
	{
	public:
		OpenWire::SourcePin	OutputPin;

	public:
		virtual void Extract( uint8_t *&AData, uint8_t &AOffset ) override
		{ 
			if( AOffset )
			{
				++ AData;
				AOffset = 0;
			}

			T_PIN AValue;
			AValue = *((T *)AData );
			OutputPin.Notify( &AValue );

			AData += sizeof( T );
		}
	};
//---------------------------------------------------------------------------
	class DigitalStructureSourceElement : public BasicTypedStructureSourceElement<bool,bool>
	{
	public:
/*
		virtual int  Start( bool &AAllign ) override
		{ 
			AAllign = false;
			return 1; 
		}
*/
		virtual void Extract( uint8_t *&AData, uint8_t &AOffset ) override
		{
			uint8_t AValue = *AData;
			bool ABoolValue = (( AValue & ( 1 << AOffset )) != 0 );
			++AOffset;

			if( AOffset >= 8 )
			{
				AOffset = 0;
				++AData;
			}

			OutputPin.Notify( &ABoolValue );
		}
	};
//---------------------------------------------------------------------------
	class DigitalStructureSinkElement : public BasicTypedStructureSinkElement<bool,bool>
	{
	public:
		virtual	void GetData( uint8_t *&AData, uint8_t &AOffset ) override
		{
			if( AOffset == 0 )
				*AData = 0;

			*AData &= 0xFF >> ( 8 - AOffset ); // Zero the upper bits
			if( FValue )
				*AData |= 1 << AOffset;	// Set the bit

			++AOffset;
			if( AOffset >= 8 )
			{
				AOffset = 0;
				++AData;
			}
		}
	};
//---------------------------------------------------------------------------
	template<int T_BUFFER_SIZE> class MakeStructure
	{
	public:
		OpenWire::SourcePin	OutputPin;

	public:
		Mitov::SimpleObjectList<BasicMakeStructureElement*>	Elements;

		bool	OnlyModified = false;

	protected:
		byte FBuffer[ T_BUFFER_SIZE ];

	public:
		void ClockInputPin_o_Receive( void *_Data )
		{
//			Serial.println( "++++++" );
			int AElementCount = Elements.size();
			if( OnlyModified )
			{
				bool AModified = false;
				for( int i = 0; i < AElementCount; ++i )
					if( Elements[ i ]->FModified )
					{
						AModified = true;
						break;
					}

				if( !AModified )
					return;
			}

			else
			{
				for( int i = 0; i < AElementCount; ++i )
					if( ! Elements[ i ]->FPopulated )
						return;

			}

			byte *ADataPtr = FBuffer;
			uint8_t AOffset = 0;
			for( int i = 0; i < AElementCount; ++i )
				Elements[ i ]->GetData( ADataPtr, AOffset );

//			Serial.println( ASize );

			
			OutputPin.SendValue( Mitov::TDataBlock( T_BUFFER_SIZE, FBuffer ));

/*
			ADataPtr = FBuffer;
			int ASize = T_BUFFER_SIZE;

			while( ASize-- )
				OutputPin.Notify( ADataPtr++ );
*/
		}

	public:
		inline void SystemLoopBegin( unsigned long currentMicros )
		{
			ClockInputPin_o_Receive( nullptr );
		}
/*
		inline void SystemStart()
		{
			FBufferSize = 0;
			FAllElements.push_back( &HeadMarker );
			for( int i = 0; i < Elements.size(); ++i )
				FAllElements.push_back( Elements[ i ] );

			FAllElements.push_back( &Checksum );

			for( int i = 0; i < FAllElements.size(); ++i )
			{
				bool AAlligned = false;
				FBufferSize += FAllElements[ i ]->GetSize( AAlligned );
//				Serial.println( FBufferSize );
				if( AAlligned )
				{
					FBufferSize = ( FBufferSize + 7 ) / 8;
					FBufferSize *= 8;
				}

			}

//			Serial.println( FBufferSize );

			FBufferSize = ( FBufferSize + 7 ) / 8;
			FBuffers[ 0 ] = new uint8_t[ FBufferSize * 2 ];
			FBuffers[ 1 ] = new uint8_t[ FBufferSize * 2 ];
		}
*/

	};
//---------------------------------------------------------------------------
	template<int T_BUFFER_SIZE> class SplitStructure
	{
	public:
		Mitov::SimpleObjectList<BasicSplitStructureElement*>	Elements;

	protected:
		byte FBuffer[ T_BUFFER_SIZE ];
		byte *FDataPtr = FBuffer;

	public:
		void InputPin_o_Receive( void *_Data )
		{
			Mitov::TDataBlock ABlock = *(Mitov::TDataBlock *)_Data;
			while( ABlock.Size -- )
			{
			    *FDataPtr ++ = *ABlock.Data ++;

				if( ( FDataPtr - FBuffer ) >= T_BUFFER_SIZE )
				{
					uint8_t AOffset = 0;
					byte *ADataPtr = FBuffer;
					for( int i = 0; i < Elements.size(); ++i )
						Elements[ i ]->Extract( ADataPtr, AOffset );

					FDataPtr = FBuffer;
				}
			}
		}

/*
		void SystemStart()
		{
			FDataPtr = FBuffer;
		}
*/
	};
//---------------------------------------------------------------------------

}

#endif