////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2018 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_PACKET_h
#define _MITOV_PACKET_h

#ifdef __TEST_FOR_DEBUG_PRINTS__
#define Serial UNGUARDED DEBUG PRINT!!!
#endif

#include <Mitov.h>

namespace Mitov
{
//---------------------------------------------------------------------------
	class TPacketAccumulator : public Mitov::SimpleList<unsigned char, uint16_t>
	{
		typedef Mitov::SimpleList<unsigned char, uint16_t> inherited;

	public:
		TPacketAccumulator & operator = ( const TPacketAccumulator &other )
		{
			assign( other );
			return *this;
		}

		void assign( const TPacketAccumulator &other )
		{
			inherited::assign( other );
		}

	public:
		bool SkipBytes( int ACount )
		{
			if( size() < ACount )
				return false;

//			Serial.print( "Size: " ); Serial.println( size() );
//			Serial.print( "SkipBytes: " ); Serial.println( ACount );
			pop_front( ACount );
			return true;
		}

		bool FindBytes( int ASize, unsigned char *ABytesPtr, int AStartPos, int ASearchSize, int &APosition )
		{
			for( APosition = AStartPos; APosition < MitovMin( (int)( _size - ASize ), ASearchSize ); ++APosition )
				if( memcmp( ABytesPtr, _list + APosition, ASize ) == 0 )
				{
					APosition += ASize;
					return true;
				}

			return false;
		}

		bool FindBytes( int ASize, unsigned char *ABytesPtr, int &APosition )
		{
			return FindBytes( ASize, ABytesPtr, 0, _size, APosition );
		}

		bool ExtractBytes( int ASize, void *ABytesPtr )
		{
			if( size() < ASize )
				return false;

//			Serial.print( "Size: " ); Serial.println( size() );
//			Serial.print( "ExtractBytes: " ); Serial.println( ASize );

			memcpy( ABytesPtr, _list, ASize );
			pop_front( ASize );
			return true;
		}

		bool PeekBytes( int ASize, void *ABytesPtr )
		{
			if( size() < ASize )
				return false;

			memcpy( ABytesPtr, _list, ASize );
			return true;
		}

	};
//---------------------------------------------------------------------------
	class BasicPacketElement
	{
	public:
		bool	Enabled : 1; // In base class to save memory
		bool	FBoolValue : 1; // In base class to save memory

	public:
		bool	FPopulated : 1;
		bool	FModified : 1;


	public:
		virtual	int	 GetSize( bool &Alligned ) { Alligned = true; return 0; }
		virtual	void GetData( unsigned char *ADataStart, unsigned char *&AData, unsigned char &AOffset ) {}
		virtual	bool Expand( unsigned char *AInBuffer, unsigned char *AOutBuffer, int &ASize ) { return false; }

	public:
		BasicPacketElement( bool APopulated ) :
			Enabled( true ),
			FBoolValue( false ),
			FPopulated( APopulated ),
			FModified( false )
		{
		}

	};
//---------------------------------------------------------------------------
	class BasicUnpacketElement
	{
	public:
		virtual int  Start( bool &AAllign ) { AAllign = false; return 0; }
		virtual bool ExpandBuffers( TPacketAccumulator *AInAccumulator, TPacketAccumulator *AOutAccumulator, int &ASize, int &APosition, bool &AIsValid, bool &AIsCorruptedPacket ) { return false; }
		virtual bool Process( TPacketAccumulator *AAccumulator, unsigned char &AOffset ) { return true; }

	};
//---------------------------------------------------------------------------
	template<typename T_PIN,typename T> class BasicTypedPacketSinkElement : public BasicPacketElement
	{
		typedef BasicPacketElement inherited;

	protected:
		T	FValue;

	public:
		void InputPin_o_Receive( void *_Data )
		{
			T AValue = *(T_PIN*)_Data;
			if( FPopulated )
				if( AValue == FValue )
					return;

//			Serial.println( "..." );
			FPopulated = true;
			FValue = AValue;
			FModified = true;
		}

	public:
		virtual	int	 GetSize( bool &Alligned ) override 
		{ 
			Alligned = true;
//			Serial.println( sizeof( double ) );
			return sizeof( T ) * 8; 
		}

		virtual	void GetData( unsigned char *ADataStart, unsigned char *&AData, unsigned char &AOffset ) override
		{ 
			if( AOffset )
			{
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
		}

	public:
		using inherited::inherited;

	};
//---------------------------------------------------------------------------
	template<typename T_PIN,typename T> class BasicTypedUnpacketSourceElement : public BasicUnpacketElement
	{
	public:
		OpenWire::SourcePin	OutputPin;

	public:
		virtual int  Start( bool &AAllign ) override
		{ 
			AAllign = true; 
			return sizeof( T ) * 8; 
		}

		virtual bool Process( TPacketAccumulator *AAccumulator, unsigned char &AOffset ) override
		{ 
			if( AOffset )
			{
				if( ! AAccumulator->SkipBytes( 1 ))
					return false;

				AOffset = 0;
			}

			T_PIN AValue;
			if( ! AAccumulator->ExtractBytes( sizeof( T ), &AValue ))
				return false;

			OutputPin.Notify( &AValue );

			return true;
		}

		virtual bool ExpandBuffers( TPacketAccumulator *AInAccumulator, TPacketAccumulator *AOutAccumulator, int &ASize, int &APosition, bool &AIsValid, bool &AIsCorruptedPacket ) override
		{ 
			APosition = ( APosition + 7 ) / 8;
			APosition *= 8;
			APosition += sizeof( T ) * 8;
			return false;
		}
	};
//---------------------------------------------------------------------------
	class UnpacketDigitalBinaryElement : public BasicTypedUnpacketSourceElement<bool,bool>
	{
	public:
		virtual int  Start( bool &AAllign ) override
		{ 
			AAllign = false;
			return 1; 
		}

		virtual bool Process( TPacketAccumulator *AAccumulator, unsigned char &AOffset ) override
		{
			unsigned char AValue;
			if( ! AAccumulator->PeekBytes( 1, &AValue ))
				return false;

			bool ABoolValue = (( AValue & ( 1 << AOffset )) != 0 );
			++AOffset;

			if( AOffset >= 8 )
			{
				AOffset = 0;
				if( ! AAccumulator->SkipBytes( 1 ))
					return false;

			}

			OutputPin.Notify( &ABoolValue );

			return true;
		}

		virtual bool ExpandBuffers( TPacketAccumulator *AInAccumulator, TPacketAccumulator *AOutAccumulator, int &ASize, int &APosition, bool &AIsValid, bool &AIsCorruptedPacket ) override
		{ 
			++ APosition;
			return false;
		}
	};
//---------------------------------------------------------------------------
	class PacketBasicMarkerBinaryElement : public BasicPacketElement
	{
		typedef BasicPacketElement inherited;

	public:
		Mitov::Bytes	Bytes;

	public:
		PacketBasicMarkerBinaryElement() :
			inherited( true )
		{
		}

	};
//---------------------------------------------------------------------------
	class UnpacketBasicMarkerBinaryElement : public BasicUnpacketElement
	{
	public:
		Mitov::Bytes	Bytes;

	public:
		virtual int  Start( bool &AAllign ) override
		{ 
			AAllign = true; 
			return Bytes._BytesSize * 2 * 8; 
		}

/*
		virtual bool ExpandBuffers( TPacketAccumulator *AInAccumulator, TPacketAccumulator *AOutAccumulator, int &ASize, int &APosition, bool &AIsValid, bool &AIsCorruptedPacket ) override
		{ 
			return false; 
		}

		virtual bool Process( TPacketAccumulator *AAccumulator, unsigned char &AOffset ) override { return true; }
*/

	};
//---------------------------------------------------------------------------
	class UnpacketMarkerBinaryElement : public UnpacketBasicMarkerBinaryElement
	{
	public:
		virtual bool ExpandBuffers( TPacketAccumulator *AInAccumulator, TPacketAccumulator *AOutAccumulator, int &ASize, int &APosition, bool &AIsValid, bool &AIsCorruptedPacket ) override
		{ 
			APosition = ( APosition + 7 ) / 8;
			APosition *= 8;

			int ABytesSize = Bytes._BytesSize;
			int ASearchPos;
			if( ! AInAccumulator->FindBytes( ABytesSize, Bytes._Bytes, APosition / 8, Bytes._BytesSize, ASearchPos ))
				AIsCorruptedPacket = true;

			else if( ASearchPos != APosition / 8 + ABytesSize )
				AIsCorruptedPacket = true;

			APosition += ABytesSize * 8;
			return false; 
		}

		virtual bool Process( TPacketAccumulator *AAccumulator, unsigned char &AOffset ) override
		{ 
			if( AOffset != 0 )
			{
				if( !AAccumulator->SkipBytes( 1 ))
					return false;

				AOffset = 0;
			}

			if( ! AAccumulator->SkipBytes( Bytes._BytesSize ))
				return false;

			return true; 
		}
	};
//---------------------------------------------------------------------------
	class UnpacketChecksumElement : public BasicUnpacketElement
	{
	public:
		bool Enabled = true;

	public:
		virtual int  Start( bool &AAllign ) override
        {
			if( ! Enabled )
				return 0;

            AAllign = true;
            return 8;
        }

		virtual bool ExpandBuffers( TPacketAccumulator *AInAccumulator, TPacketAccumulator *AOutAccumulator, int &ASize, int &APosition, bool &AIsValid, bool &AIsCorruptedPacket ) override
        {
			if( ! Enabled )
                return false;

			APosition = ( APosition + 7 ) / 8;
			APosition *= 8;

            unsigned char *ABytes = new unsigned char[ APosition / 8 + 1 ];
//            SetLength( ABytes, APosition + 1 );

            if( ! AInAccumulator->PeekBytes( APosition / 8 + 1, ABytes ))
            {
                AIsValid = false;
                delete [] ABytes;
                return false;
            }

            unsigned char AChecksum = 0;

//            for AByte in ABytes do
//                AChecksum ^= AByte;
            for( int i = 0; i < APosition / 8 + 1; ++i )
                AChecksum ^= ABytes[ i ];

            delete [] ABytes;

            if( AChecksum )
            {
                AIsCorruptedPacket = true;
                return false;
            }

            APosition += 8;
            return false;
        }

		virtual bool Process( TPacketAccumulator *AAccumulator, unsigned char &AOffset ) override
        {
			if( ! Enabled )
                return true;

            if( AOffset )
            {
                if( ! AAccumulator->SkipBytes( 1 ))
                  return false;

                AOffset = 0;
            }

            AAccumulator->SkipBytes( 1 );
            return true;
        }
	};
//---------------------------------------------------------------------------
	class PacketMarkerBinaryElement : public PacketBasicMarkerBinaryElement
	{
	public:
		virtual	int	 GetSize( bool &Alligned ) override
		{ 
			Alligned = true;
			return Bytes._BytesSize * 8;
		}

		virtual	void GetData( unsigned char *ADataStart, unsigned char *&AData, unsigned char &AOffset ) override
		{
			if( Bytes._BytesSize == 0 )
				return;

			if( AOffset )
			{
				++AData;
				AOffset = 0;
			}

			memcpy( AData, Bytes._Bytes, Bytes._BytesSize );
			AData += Bytes._BytesSize;
		}

	};
//---------------------------------------------------------------------------
	class PacketChecksumElement : public BasicPacketElement
	{
		typedef BasicPacketElement inherited;

	public:
		virtual	int	 GetSize( bool &Alligned ) override
		{ 
			if( Enabled )
			{
				Alligned = true;
				return 8;
			}

			return 0;
		}

		virtual	void GetData( unsigned char *ADataStart, unsigned char *&AData, unsigned char &AOffset ) override
		{
			if( !Enabled )
				return;

			if( AOffset )
			{
				++AData;
				AOffset = 0;
			}

			unsigned char	AChecksum = 0;
			for( ;ADataStart < AData; ++ADataStart )
				AChecksum ^= *ADataStart;

			*AData = AChecksum;
			++AData;
		}

	public:
		PacketChecksumElement() :
			inherited( true )
		{
		}

	};
//---------------------------------------------------------------------------
	class UnpacketHeadMarkerBinaryElement : public UnpacketBasicMarkerBinaryElement
	{
	protected:
		Mitov::SimpleList<unsigned char>	FDoubledBytes;

	public:
		virtual int  Start( bool &AAllign ) override
		{ 
			for( unsigned int i = 0; i < Bytes._BytesSize; ++i )
				FDoubledBytes.push_back( Bytes._Bytes[ i ] );

			for( unsigned int i = 0; i < Bytes._BytesSize; ++i )
				FDoubledBytes.push_back( Bytes._Bytes[ i ] );

			AAllign = true; 
			return Bytes._BytesSize * 2 * 8; 
		}

		virtual bool Process( TPacketAccumulator *AAccumulator, unsigned char &AOffset ) override
		{
			if( AOffset )
			{
				if( ! AAccumulator->SkipBytes( 1 ))
					return false;

				AOffset = 0;
			}

			//  AOldDebugBuf := AAccumulator.GetAsBuffer();
			AAccumulator->SkipBytes( FDoubledBytes.size() );
			return true;
		}

		virtual bool ExpandBuffers( TPacketAccumulator *AInAccumulator, TPacketAccumulator *AOutAccumulator, int &ASize, int &APosition, bool &AIsValid, bool &AIsCorruptedPacket ) override
		{ 
			APosition = ( APosition + 7 ) / 8;
			APosition *= 8;

			int ABytesSize = Bytes._BytesSize;
			if( !ABytesSize )
				return false;

			unsigned char *ABytes = Bytes._Bytes;
			//  if( not AInAccumulator.FindSkipBytes( ABytesSize * 2, @FDoubledBytes[ 0 ], ASkipped )) then
			//    Exit( False );

			int AFindPosition;

			if( !AInAccumulator->FindBytes( ABytesSize * 2, FDoubledBytes, AFindPosition ))
			{
				AIsValid = false;
				return false;
			}

			AFindPosition -= ABytesSize * 2;
			AInAccumulator->SkipBytes( AFindPosition );

			APosition += ABytesSize * 2 * 8;

			//  Dec( ASize, AFindPosition );

			//  if( ASize <= ABytesSize * 2 ) then
			//    Exit( False );

			//  ANewDebugBuf := AInAccumulator.GetAsBuffer();

			if( !AInAccumulator->FindBytes( ABytesSize, ABytes, ABytesSize * 2, ASize - ABytesSize * 2, AFindPosition ))
				return false;

			//  ANewDebugBuf := AInAccumulator.GetAsBuffer();
//			AMarkerBuffer := TSLBlockBuffer.CreateData( ABytes, ABytesSize );

			//  if( not AInAccumulator.FindBytes( ABytesSize, ABytes, ASize - ABytesSize * 2, AFindPosition )) then
			//    Exit( False );

			AOutAccumulator->append( ABytes, ABytesSize );
			AOutAccumulator->append( ABytes, ABytesSize );

			for(;;)
			{
				Mitov::SimpleList<unsigned char>	ABuffer;
				ABuffer.AddCount( AFindPosition );
//				ABuffer := TSLBlockBuffer.CreateSize( AFindPosition );
				AInAccumulator->ExtractBytes( AFindPosition, ABuffer );
				AOutAccumulator->append( ABuffer, AFindPosition );

				unsigned char ACount;
				AInAccumulator->ExtractBytes( 1, &ACount );

				for( int i = 0; i <= ACount; ++i )
					AOutAccumulator->append( ABytes, ABytesSize );

				ASize = ABytesSize * ACount + 1;

				if( ! AInAccumulator->FindBytes( ABytesSize, ABytes, AFindPosition, ASize - AFindPosition, AFindPosition ))
					return true;

			}

			return true;
		}

	public:
		void SkipHeader( TPacketAccumulator *AAccumulator ) 
		{
			AAccumulator->SkipBytes( Bytes._BytesSize * 2 );
		}

	};
//---------------------------------------------------------------------------
	class PacketHeadMarkerBinaryElement : public PacketMarkerBinaryElement
	{
	public:
		virtual	int	 GetSize( bool &Alligned ) override
		{ 
			Alligned = true;
			return Bytes._BytesSize * 2 * 8; 
		}

		virtual	void GetData( unsigned char *ADataStart, unsigned char *&AData, unsigned char &AOffset ) override
		{
			if( Bytes._BytesSize == 0 )
				return;

			if( AOffset )
			{
				++AData;
				AOffset = 0;
			}

			memcpy( AData, Bytes._Bytes, Bytes._BytesSize );
			AData += Bytes._BytesSize;

			memcpy( AData, Bytes._Bytes, Bytes._BytesSize );
			AData += Bytes._BytesSize;
		}

		virtual	bool Expand( unsigned char *AInBuffer, unsigned char *AOutBuffer, int &ASize ) override
		{
			if( Bytes._BytesSize == 0 )
				return false;

//			memcpy( AOutBuffer, AInBuffer, ASize );
//			return true;

			unsigned char *ABytes = Bytes._Bytes;
			int ABytesSize = Bytes._BytesSize;

			int AOutSize = ABytesSize * 2;

			memcpy( AOutBuffer, AInBuffer, AOutSize );
			AOutBuffer += AOutSize;
			AInBuffer += AOutSize;

			int i = AOutSize;
			for( ; i < ASize - ABytesSize * 2 + 1; ++i )
			{
				if( memcmp( AInBuffer, ABytes, ABytesSize ) == 0 )
					if( memcmp( AInBuffer + ABytesSize, ABytes, ABytesSize ) == 0 )
					{
						memcpy( AOutBuffer, AInBuffer, ABytesSize );
						AInBuffer += ABytesSize * 2;

//						Serial.println( "START" );
						AOutBuffer += ABytesSize;
						unsigned char ACount = 0;
						i += ABytesSize;
						for( ; i < ASize - ABytesSize + 1; i += ABytesSize )
						{
							if( memcmp( AInBuffer, ABytes, ABytesSize ) != 0 )
								break;

//							Serial.println( "INC" );
							++ACount;
							if( ACount == 255 )
								break;

							AInBuffer += ABytesSize;
						}

						if( ACount == *ABytes )
						{
							if( ACount > 0 )
							{
//								Serial.println( "DEC" );
								--ACount;
								AInBuffer -= ABytesSize;
								i -= ABytesSize;
							}
						}

						*AOutBuffer++ = ACount;
						AOutSize += ABytesSize + 1;
						continue;
					}

				*AOutBuffer++ = *AInBuffer++;
//Serial.print( "++AOutSize" );
				++AOutSize;
			}

			int ACorrection = ASize - i;
			memcpy( AOutBuffer, AInBuffer, ACorrection );
//			*AOutBuffer = 5;

//			Serial.print( "ADebugCount : " );
//			Serial.println( ADebugCount );

//			Serial.print( "AOutSize : " );
//			Serial.println( AOutSize );

//			Serial.print( "TEST : " );
//			Serial.println( ACorrection );
//			Serial.println( i );

			ASize = AOutSize + ACorrection;
			return true; 
		}

	};
//---------------------------------------------------------------------------
	class PacketDigitalBinaryElement : public BasicPacketElement
	{
		typedef BasicPacketElement inherited;

	public:
		void InputPin_o_Receive( void *_Data )
		{
			bool AValue = *(bool*)_Data;
			if( FPopulated )
				if( AValue == FBoolValue )
					return;

			FPopulated = true;
			FBoolValue = AValue;
			FModified = true;
		}

	public:
		virtual	int	 GetSize( bool &Alligned ) override
		{ 
			Alligned = false;
			return 1; 
		}

		virtual	void GetData( unsigned char *ADataStart, unsigned char *&AData, unsigned char &AOffset ) override
		{
			*AData &= 0xFF >> ( 8 - AOffset ); // Zero the upper bits
			if( FBoolValue )
				*AData |= 1 << AOffset;	// Set the bit

			++AOffset;
			if( AOffset >= 8 )
			{
				AOffset = 0;
				++AData;
			}
		}

	public:
		using inherited::inherited;

	};
//---------------------------------------------------------------------------
	class Packet
	{
	public:
		OpenWire::SourcePin	OutputPin;

	public:
		Mitov::SimpleObjectList<BasicPacketElement*>	Elements;

		bool	OnlyModified = false;
		float	SizeReserve	= 2.0;

		PacketHeadMarkerBinaryElement	HeadMarker;
		PacketChecksumElement			Checksum;

	protected:
		int	FBufferSize;

		unsigned char *FBuffers[ 2 ];
		Mitov::SimpleList<BasicPacketElement*>	FAllElements;

	public:
		void ClockInputPin_o_Receive( void *_Data )
		{
//			Serial.println( "++++++" );
			int AElementCount = FAllElements.size();
			if( OnlyModified )
			{
				bool AModified = false;
				for( int i = 0; i < AElementCount; ++i )
					if( FAllElements[ i ]->FModified )
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
					if( ! FAllElements[ i ]->FPopulated )
						return;

			}

			unsigned char *ADataPtr = FBuffers[ 0 ];
			unsigned char AOffset = 0;
			for( int i = 0; i < AElementCount; ++i )
			{
				FAllElements[ i ]->GetData( FBuffers[ 0 ], ADataPtr, AOffset );
//				while( AElementSize-- )
//					OutputPin.Notify( AData++ );


			}

			int ASize = FBufferSize;
//			Serial.println( ASize );
			int ABufferIndex = 0;
//			Serial.println( "--------" );
			for( int i = 0; i < AElementCount; ++i )
			{
//				Serial.println( ABufferIndex & 1 );
//				Serial.println( 1 ^ ( ABufferIndex & 1 ) );
				unsigned char *AInBuffer = FBuffers[ ABufferIndex & 1 ];
				unsigned char *AOutBuffer = FBuffers[ 1 ^ ( ABufferIndex & 1 )];
				if( FAllElements[ i ]->Expand( AInBuffer, AOutBuffer, ASize ))
					++ ABufferIndex;
			}

			ADataPtr = FBuffers[ ABufferIndex & 1 ];
			OutputPin.SendValue( Mitov::TDataBlock( ASize, ADataPtr ));

//			Serial.println( ASize );
//			while( ASize-- )
//				OutputPin.Notify( ADataPtr++ );

		}

	public:
		inline void SystemLoopBegin( unsigned long currentMicros )
		{
			ClockInputPin_o_Receive( nullptr );
		}

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
			FBuffers[ 0 ] = new unsigned char[ int( FBufferSize * SizeReserve ) ];
			FBuffers[ 1 ] = new unsigned char[ int( FBufferSize * SizeReserve ) ];
		}

/*
		virtual ~Packet()
		{
			delete []FBuffers[ 1 ];
			delete []FBuffers[ 0 ];
		}
*/
	};
//---------------------------------------------------------------------------
	class Unpacket
	{
	public:
		Mitov::SimpleObjectList<BasicUnpacketElement*>	Elements;

		UnpacketHeadMarkerBinaryElement	HeadMarker;
		UnpacketChecksumElement			Checksum;

	protected:
		TPacketAccumulator	FAccumulator;
		int					FMinSize = 0;
		Mitov::SimpleList<BasicUnpacketElement*>	FAllElements;

	public:
		void InputPin_o_Receive( void *_Data )
		{
			if( FMinSize == 0 )
				return;

//			Serial.println( "FMinSize" );
//			Serial.println( FMinSize );
//			Serial.println( "T0:" );
//			Serial.println( FAccumulator.size() );

			Mitov::TDataBlock ABlock = *(Mitov::TDataBlock *)_Data;

//			Serial.println( ABlock.Size );

			while( ABlock.Size-- )
			{
//				Serial.print( "+" );
				FAccumulator.push_back( *ABlock.Data++ );
//				Serial.print( "-" );
			}

//			Serial.println( "" );
//			Serial.println( FAccumulator.size() );

//			Serial.println( "T1" );

			TPacketAccumulator	AOldAccumulator;
			TPacketAccumulator	*AInAccumulator;
			TPacketAccumulator	ARealOutAccumulator;
			TPacketAccumulator	*AOutAccumulator = &ARealOutAccumulator;
			TPacketAccumulator	*AAccumulator;

			while( FAccumulator.size() >= FMinSize )
			{
//			Serial.println( "FMinSize" );
//			Serial.println( FMinSize );
//				Serial.println( "T2" );
//AOldAccumulator.clear();
//AOldAccumulator.append( FAccumulator, FAccumulator.size() );
//return;
				AOldAccumulator.assign( FAccumulator );
//return;
				AInAccumulator = &FAccumulator;

				AOutAccumulator->clear();
				AAccumulator = &FAccumulator;
				int ASize = FMinSize;
				bool AIsValid = true;

				bool AIsCorruptedPacket = false;
				int APosition = 0;
//return;
				for( int i = 0; i < FAllElements.size(); ++i )
				{
//					Serial.println( "enter" );
//					Serial.println( FAccumulator.size() );

					if( FAllElements[ i ]->ExpandBuffers( AInAccumulator, AOutAccumulator, ASize, APosition, AIsValid, AIsCorruptedPacket ))
					{
//						Serial.println( "ExpandBuffers" );
//return;
						if( AIsCorruptedPacket )
						{
							FAccumulator.assign( AOldAccumulator );
							HeadMarker.SkipHeader( &FAccumulator );
							break;
						}

						AAccumulator = AOutAccumulator;
						if( AAccumulator == &FAccumulator )
						{
//							Serial.println( "AAccumulator == &FAccumulator" );
							AInAccumulator = AOutAccumulator;
							AOutAccumulator->clear();
						}

						else
							swap( AInAccumulator, AOutAccumulator );

					}

//					Serial.println( "Step1" );
					if( AIsCorruptedPacket )
					{
//						Serial.println( "AIsCorruptedPacket" );
						FAccumulator.assign( AOldAccumulator );
						HeadMarker.SkipHeader( &FAccumulator );
						break;
					}

					if( ! AIsValid )
					{
						FAccumulator.assign( AOldAccumulator );
//						Serial.println( "Non Valid" );
						return;
					}

				}

//				Serial.println( "ExpandBuffers OUT" );
				if( AIsCorruptedPacket )
				{
//					Serial.println( "AIsCorruptedPacket SKIPPING" );
//return;
//					if( ! HeadMarker.Bytes._BytesSize )
						FAccumulator.SkipBytes( 1 );

					continue;
				}

				if( AAccumulator->size() < FMinSize )
				{
					FAccumulator.assign( AOldAccumulator );
					return;
				}

				if( ASize < FMinSize )
		//      for AItem in FAllElements do
		//        if( not AItem.CanProcess( AAccumulator )) then
				{
					FAccumulator.assign( AOldAccumulator );
					return;
				}

//				Serial.println( "PROCESS" );
				unsigned char AOffset = 0;
				for( int i = 0; i < FAllElements.size(); ++i )
				{
					if( ! FAllElements[ i ]->Process( AAccumulator, AOffset ))
						break;

//break;
				}


//				Serial.println( "exit" );
//				Serial.println( FAccumulator.size() );
//				return;

			}

//			Serial.println( "DoReceive exit" );
		}

	public:
		inline void SystemStart()
		{
//			Serial.println( "FMinSize" );
			FAllElements.push_back( &HeadMarker );

			for( int i = 0; i < Elements.size(); ++i )
				FAllElements.push_back( Elements[ i ] );

			FAllElements.push_back( &Checksum );

			FMinSize = 0;
			for( int i = 0; i < FAllElements.size(); ++i )
			{
				  bool AAlligned = false;
				  FMinSize += FAllElements[ i ]->Start( AAlligned );
				  if( AAlligned )
				  {
					  FMinSize = ( FMinSize + 7 ) / 8;
					  FMinSize = FMinSize * 8;
				  }
			}

			FMinSize = ( FMinSize + 7 ) / 8;

//			Serial.println( "FMinSize" );
//			Serial.println( FMinSize );
		}

	};
//---------------------------------------------------------------------------
}

#ifdef __TEST_FOR_DEBUG_PRINTS__
#undef Serial
#endif

#endif