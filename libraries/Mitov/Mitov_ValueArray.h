////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2018 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_VALUE_ARRAY_h
#define _MITOV_VALUE_ARRAY_h

#include <Mitov.h>

namespace Mitov
{
//---------------------------------------------------------------------------
	template<typename T_CONST, typename T> class ValueArray
	{
	public:
		OpenWire::SourcePin	OutputPin;

	public:
		uint32_t InitialIndex = 0;

	public:
		T *_Values;
		uint32_t _MaxIndex;

	public:
		bool	ClockInputPin_o_IsConnected = false;

	public:
		operator T *() { return _Values; }

	public:
		inline void SystemStart()
		{
			if( InitialIndex > _MaxIndex )
				InitialIndex = _MaxIndex;

			UpdateArrayValue();
		}

	public:
		inline void ClockInputPin_o_Receive( void *_Data )
		{
			OutputPin.SendValue( _Values[ InitialIndex ] );
		}
		
		void IndexInputPin_o_Receive( void *_Data )
		{
			uint32_t AIndex = *(uint32_t *)_Data;
			if( AIndex > _MaxIndex )
				AIndex = _MaxIndex;

			if( InitialIndex == AIndex )
				return;

			InitialIndex = AIndex;

			UpdateArrayValue();
		}

	public:
		void UpdateArrayValue()
		{
			if( ! ClockInputPin_o_IsConnected )
				OutputPin.SendValue( _Values[ InitialIndex ] );
		}

	};
//---------------------------------------------------------------------------
	template<> class ValueArray<uint8_t, bool>
	{
	public:
		OpenWire::SourcePin	OutputPin;

	public:
		uint32_t InitialIndex = 0;

	public:
		uint8_t *_Values;
		uint32_t _MaxIndex;

	public:
		bool	ClockInputPin_o_IsConnected : 1;

	protected:
		bool	FOutValue : 1;

	public:
		class _ByteAccess
		{
		protected:
			ValueArray<uint8_t, bool>	*FOwner;
			int					FIndex;

		public:
			void operator = ( bool AValue ) 
			{ 
				if( AValue )
					FOwner->_Values[ FIndex / 8 ] |= 1 << ( FIndex & 0b111 );

				else
					FOwner->_Values[ FIndex / 8 ] &= ~( 1 << ( FIndex & 0b111 ));

			}

			bool operator == ( bool AValue ) 
			{
				return ( FOwner->_Values[ FIndex / 8 ] & ( 1 << ( FIndex & 0b111 )) == AValue );
			}

			bool operator != ( bool AValue ) 
			{
				return ( FOwner->_Values[ FIndex / 8 ] & ( 1 << ( FIndex & 0b111 )) != AValue );
			}

		public:
			_ByteAccess( ValueArray<uint8_t, bool> *AOwner, int AIndex ) :
				FOwner( AOwner ),
				FIndex( AIndex )
			{
			}
		};

		_ByteAccess operator [](int AIndex) { return _ByteAccess( this, AIndex ); }

	public:
		inline void SystemStart()
		{
			if( InitialIndex > _MaxIndex )
				InitialIndex = _MaxIndex;

			UpdateArrayValue();
		}

	public:
		inline void ClockInputPin_o_Receive( void *_Data )
		{
			OutputPin.SendValue( _Values[ InitialIndex / 8 ] & ( 1 << ( InitialIndex & 0b111 )) );
		}

		void IndexInputPin_o_Receive( void *_Data )
		{
			uint32_t AIndex = *(uint32_t *)_Data;
			if( AIndex > _MaxIndex )
				AIndex = _MaxIndex;

			if( InitialIndex == AIndex )
				return;

			InitialIndex = AIndex;
			UpdateArrayValue();
		}

	public:
		void UpdateArrayValue()
		{
			if( ! ClockInputPin_o_IsConnected )
				OutputPin.SendValue( _Values[ InitialIndex / 8 ] & ( 1 << ( InitialIndex & 0b111 )) );
		}


	public:
		ValueArray() :
			ClockInputPin_o_IsConnected( false )
		{
		}
	};
//---------------------------------------------------------------------------
/*
	class ValueConstFloatArray : public ValueArray_Const<float, float>
	{
	public:
		inline float GetValue()
		{
			return pgm_read_float( _Values + InitialIndex );
		}

	};
*/
//---------------------------------------------------------------------------
	template<typename CONST_T, typename T> class ValueArray_Const
	{
	public:
		OpenWire::TypedSourcePin<T>	OutputPin;

	public:
		uint32_t InitialIndex = 0;

	public:
		const /*PROGMEM*/ CONST_T *_Values;
		uint32_t _MaxIndex;

	public:
		bool	ClockInputPin_o_IsConnected = false;

	protected:
		inline void SetValue( bool AChangeOnly )
		{
			OutputPin.SetValue( GetValue(), AChangeOnly );
		}

		T GetValue() {}

	public:
		inline void SystemStart()
		{
			if( InitialIndex > _MaxIndex )
				InitialIndex = _MaxIndex;
			
			SetValue( true );
		}

	public:
		inline void ClockInputPin_o_Receive( void *_Data )
		{
			SetValue( false );
		}

		void IndexInputPin_o_Receive( void *_Data )
		{
			uint32_t AIndex = *(uint32_t *)_Data;
			if( AIndex > _MaxIndex )
				AIndex = _MaxIndex;

			if( InitialIndex == AIndex )
				return;

			InitialIndex = AIndex;

			if( ! ClockInputPin_o_IsConnected )
				SetValue( false );
		}

	};
//---------------------------------------------------------------------------
	template<> float ValueArray_Const<float, float>::GetValue()
	{
		return pgm_read_float( _Values + InitialIndex );
	}
//---------------------------------------------------------------------------
	template<> Mitov::String ValueArray_Const<const char *, Mitov::String>::GetValue()
	{
		char *APtr = (char *)pgm_read_dword( _Values + InitialIndex );
		return APtr;
	}
//---------------------------------------------------------------------------
	template<> bool ValueArray_Const<uint8_t, bool>::GetValue()
	{
		return pgm_read_byte( _Values + ( InitialIndex / 8 )) & ( 1 << ( InitialIndex & 0b111 ));
	}
//---------------------------------------------------------------------------
	template<> int32_t ValueArray_Const<int32_t, int32_t>::GetValue()
	{
		uint32_t AValue = pgm_read_dword( _Values + InitialIndex );
		return *(int32_t *)&AValue;
	}
//---------------------------------------------------------------------------
	template<> uint32_t ValueArray_Const<uint32_t, uint32_t>::GetValue()
	{
		return pgm_read_dword( _Values + InitialIndex );
	}
//---------------------------------------------------------------------------
	template<> Mitov::TColor ValueArray_Const<uint32_t, Mitov::TColor>::GetValue()
	{
		uint32_t AValue = pgm_read_dword( _Values + InitialIndex );
		return *(Mitov::TColor *)&AValue;
	}
//---------------------------------------------------------------------------
	template<> Mitov::TRGBWColor ValueArray_Const<uint32_t, Mitov::TRGBWColor>::GetValue()
	{
		uint32_t AValue = pgm_read_dword( _Values + InitialIndex );
		return *(Mitov::TRGBWColor *)&AValue;
	}
//---------------------------------------------------------------------------
/*
	class ValueConstDateTimeArray : public ValueArray_Const<Mitov::TDateTime, Mitov::TDateTime>
	{
	protected:
		virtual void SetValue( bool AChangeOnly )
		{
			uint32_t ADate = pgm_read_dword( _Values + InitialIndex );
			uint32_t ATime = pgm_read_dword( ((byte *)_Values ) + InitialIndex * sizeof(TDateTime) + sizeof(uint32_t) );
			OutputPin.SetValue( TDateTime( ADate, ATime ), AChangeOnly );
		}

	};
*/
//---------------------------------------------------------------------------
}

#endif
