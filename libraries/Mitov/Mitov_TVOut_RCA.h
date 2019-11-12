////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2018 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_TVOUT_RCA_h
#define _MITOV_TVOUT_RCA_h

#include <Mitov.h>
#include <TVout.h>

namespace Mitov
{
//---------------------------------------------------------------------------
	template<typename T_OWNER> class TVOutRCAElementBasic
	{
	public:
		virtual void Render() {}
		virtual void TVBegin() {}

	public:
		TVOutRCAElementBasic( T_OWNER &AOwner )
		{
			AOwner.RegisterRender( this );
		}

	};
//---------------------------------------------------------------------------
/*
	class TVOut_RCA_Intf
	{
	public:
		virtual void RegisterRender( TVOutRCAElementBasic *AItem ) {}
		virtual bool IsEnabled() = 0;
		virtual TVout &GetTV() = 0;

		virtual void TV_shift( bool _InVertical, int32_t _Distance ) = 0;
		virtual void TV_draw_line(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, char c) = 0;
		virtual void TV_bitmap(uint8_t x, uint8_t y, const unsigned char * bmp, uint16_t i = 0, uint8_t width = 0, uint8_t lines = 0) = 0;
		virtual void TV_set_cursor(uint8_t, uint8_t) = 0;
		virtual void TV_draw_circle(uint8_t x0, uint8_t y0, uint8_t radius, char c, char fc = -1) = 0;
		virtual void TV_draw_rect(uint8_t x0, uint8_t y0, uint8_t w, uint8_t h, char c, char fc = -1) = 0;
		virtual void TV_set_pixel(uint8_t x, uint8_t y, char c) = 0;
		virtual unsigned char TV_get_pixel(uint8_t x, uint8_t y) = 0;

	};
*/
//---------------------------------------------------------------------------
	enum TArduinoTVOutRCAElementColor { tvcBlack, tvcWhite, tvcInvert, tvcNone = -1 };
//---------------------------------------------------------------------------
	template<typename T_OWNER, T_OWNER &C_OWNER> class TVOutRCAElementClocked : public Mitov::TVOutRCAElementBasic<T_OWNER>
	{
		typedef Mitov::TVOutRCAElementBasic<T_OWNER> inherited;

	public:
		bool	ClockInputPin_o_IsConnected = false;

	public:
		inline void ClockInputPin_o_Receive( void *_Data )
		{
			inherited::Render();
		}

	public:
		virtual void TVBegin() override
		{
			if( ! ClockInputPin_o_IsConnected )
				inherited::Render();
		}

	public:
		TVOutRCAElementClocked() :
			inherited( C_OWNER )
		{
		}

	};
//---------------------------------------------------------------------------
	class TVOut_RCA
	{
    public:
        bool	Enabled : 1;
		bool	IsPAL : 1;
		int32_t	Width = 128;
		int32_t	Height = 96;

	public:
		void SetEnabled( bool AVelue )
		{
			if( Enabled == AVelue )
				return;

			Enabled = AVelue;

			if( Enabled )
				TVBegin();

			else
				FTV.end();
		}

	public:
		TVout	FTV;

	protected:
		Mitov::SimpleList<TVOutRCAElementBasic<TVOut_RCA> *>	FElements;

	public:
		void RegisterRender( TVOutRCAElementBasic<TVOut_RCA> *AItem )
		{
			FElements.push_back( AItem );
		}

		void TV_shift( bool _InVertical, int32_t _Distance )
		{
			if( ! Enabled )
				return;

			uint8_t ADistance = abs( _Distance );
			uint8_t ADirection;
			if( _InVertical )
			{
				if( _Distance < 0 )
					ADirection = UP;

				else
					ADirection = DOWN;

			}
			else
			{
				if( _Distance < 0 )
					ADirection = LEFT;

				else
					ADirection = RIGHT;

			}

			FTV.shift( ADistance, ADirection );
		}

		void TV_draw_line(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, char c)
		{
			if( Enabled )
				FTV.draw_line( x0, y0, x1, y1, c );
		}

		void TV_draw_circle(uint8_t x0, uint8_t y0, uint8_t radius, char c, char fc = -1)
		{
			if( Enabled )
				FTV.draw_circle( x0, y0, radius, c, fc );
		}

		void TV_draw_rect(uint8_t x0, uint8_t y0, uint8_t w, uint8_t h, char c, char fc = -1)
		{
			if( Enabled )
				FTV.draw_rect( x0, y0, w, h, c, fc );
		}

		void TV_set_pixel(uint8_t x, uint8_t y, char c)
		{
			if( Enabled )
				FTV.set_pixel( x, y, c );
		}

		unsigned char TV_get_pixel(uint8_t x, uint8_t y)
		{
			return FTV.get_pixel( x, y );
		}

		void TV_bitmap(uint8_t x, uint8_t y, const unsigned char * bmp, uint16_t i = 0, uint8_t width = 0, uint8_t lines = 0)
		{
			if( Enabled )
				FTV.bitmap( x, y, bmp, i, width, lines );
		}

		void TV_set_cursor( uint8_t x, uint8_t y )
		{
			if( Enabled )
				FTV.set_cursor( x, y );
		}

	public:
		inline bool IsEnabled()
		{
			return Enabled;
		}

		inline TVout &GetTV()
		{
			return FTV;
		}

	protected:
		void TVBegin()
		{
			FTV.begin( IsPAL ? PAL : NTSC, Width, Height );

			for( int i = 0; i < FElements.size(); i ++ )
				FElements[ i ]->TVBegin();

//			FTV.select_font( font4x6 );
		}

	public:
		inline void SystemInit()
		{
			if( Enabled )
				FTV.begin( IsPAL ? PAL : NTSC, Width, Height );

		}

		inline void SystemStart()
		{
			if( Enabled )
				for( int i = 0; i < FElements.size(); i ++ )
					FElements[ i ]->TVBegin();

		}

	public:
		TVOut_RCA() :
			IsPAL( false ),
			Enabled( true )
		{
		}

	};
//---------------------------------------------------------------------------
	template<typename T_OWNER, T_OWNER &C_OWNER> class TVOutRCAElementFillScreen : public TVOutRCAElementClocked<T_OWNER, C_OWNER>
	{
		typedef Mitov::TVOutRCAElementClocked<T_OWNER, C_OWNER> inherited;

	public:
		TArduinoTVOutRCAElementColor	Color = tvcBlack;

	public:
		virtual void Render() override
		{
			if( C_OWNER.IsEnabled() )
				C_OWNER.GetTV().fill( (int)Color );
		}

	public:
		using inherited::inherited;

	};
//---------------------------------------------------------------------------
	template<typename T_OWNER, T_OWNER &C_OWNER> class TVOutRCAElementPlayTone : public TVOutRCAElementClocked<T_OWNER, C_OWNER>
	{
		typedef Mitov::TVOutRCAElementClocked<T_OWNER, C_OWNER> inherited;

	public:
		unsigned int	Frequency = 1760;
		unsigned long	Duration = 1000;

	public:
		virtual void Render() override
		{
			if( C_OWNER.IsEnabled() )
			{
				if( Frequency == 0 )
					C_OWNER.GetTV().noTone();

				else
					C_OWNER.GetTV().tone( Frequency, Duration );
			}
		}

	public:
		using inherited::inherited;

	};
//---------------------------------------------------------------------------
	template<typename T_OWNER, T_OWNER &C_OWNER> class TVOutRCAElementDrawCircle : public TVOutRCAElementClocked<T_OWNER, C_OWNER>
	{
		typedef Mitov::TVOutRCAElementClocked<T_OWNER, C_OWNER> inherited;

	public:
		TArduinoTVOutRCAElementColor	BorderColor = tvcWhite;
		TArduinoTVOutRCAElementColor	FillColor = tvcBlack;

		int32_t	X = 10;
		int32_t	Y = 10;
		int32_t	Radius = 10;

	public:
		virtual void Render() override
		{
			C_OWNER.TV_draw_circle( X, Y, Radius, (int)BorderColor, (int)FillColor );
		}

	public:
		using inherited::inherited;

	};
//---------------------------------------------------------------------------
	template<typename T_OWNER, T_OWNER &C_OWNER> class TVOutRCAElementDrawRectangle : public TVOutRCAElementClocked<T_OWNER, C_OWNER>
	{
		typedef Mitov::TVOutRCAElementClocked<T_OWNER, C_OWNER> inherited;

	public:
		TArduinoTVOutRCAElementColor	BorderColor = tvcWhite;
		TArduinoTVOutRCAElementColor	FillColor = tvcBlack;

		int32_t	X = 0;
		int32_t	Y = 0;
		int32_t	Width = 10;
		int32_t	Height = 10;

	public:
		virtual void Render() override
		{
			C_OWNER.TV_draw_rect( X, Y, Width, Height, (int)BorderColor, (int)FillColor );
		}

	public:
		using inherited::inherited;

	};
//---------------------------------------------------------------------------
	template<typename T_OWNER, T_OWNER &C_OWNER> class TVOutRCAElementDrawLine : public TVOutRCAElementClocked<T_OWNER, C_OWNER>
	{
		typedef Mitov::TVOutRCAElementClocked<T_OWNER, C_OWNER> inherited;

	public:
		TArduinoTVOutRCAElementColor	Color = tvcWhite;

	public:
		int32_t	X1 = 0;
		int32_t	Y1 = 0;
		int32_t	X2 = 10;
		int32_t	Y2 = 10;

	public:
		virtual void Render() override
		{
			C_OWNER.TV_draw_line( X1, Y1, X2, Y2, (int)Color );
		}

	public:
		using inherited::inherited;

	};
//---------------------------------------------------------------------------
	template<typename T_OWNER, T_OWNER &C_OWNER> class TVOutRCAElementDrawPixel : public TVOutRCAElementClocked<T_OWNER, C_OWNER>
	{
		typedef Mitov::TVOutRCAElementClocked<T_OWNER, C_OWNER> inherited;

	public:
		TArduinoTVOutRCAElementColor	Color = tvcWhite;

	public:
		int32_t	X = 0;
		int32_t	Y = 0;

	public:
		virtual void Render() override
		{
			C_OWNER.TV_set_pixel( X, Y, (int)Color );
		}

	public:
		using inherited::inherited;

	};
//---------------------------------------------------------------------------
	template<typename T_OWNER, T_OWNER &C_OWNER> class TVOutRCAElementShiftScreen : public Mitov::TVOutRCAElementClocked<T_OWNER, C_OWNER>
	{
		typedef Mitov::TVOutRCAElementClocked<T_OWNER, C_OWNER> inherited;

	public:
		int32_t	Distance = 1;
		bool	InVertical = false;

	public:
		virtual void Render() override
		{
			C_OWNER.TV_shift( InVertical, Distance );
		}

	public:
		using inherited::inherited;

	};
//---------------------------------------------------------------------------
	template<typename T_OWNER, T_OWNER &C_OWNER> class TVOutRCAElementDrawScene : public Mitov::TVOutRCAElementClocked<T_OWNER, C_OWNER>
	{
		typedef Mitov::TVOutRCAElementClocked<T_OWNER, C_OWNER> inherited;

	public:
		int32_t	X = 0;
		int32_t	Y = 0;

	protected:
		Mitov::SimpleList<TVOutRCAElementBasic<TVOutRCAElementDrawScene> *>	FElements;

	public:
		inline void RegisterRender( TVOutRCAElementBasic<TVOutRCAElementDrawScene> *AItem )
		{
			FElements.push_back( AItem );
		}

		inline bool IsEnabled()
		{
			return C_OWNER.IsEnabled();
		}

		inline TVout &GetTV()
		{
			return C_OWNER.GetTV();
		}

		inline void TV_shift( bool _InVertical, int32_t _Distance )
		{
			C_OWNER.TV_shift( _InVertical, _Distance );
		}

		inline void TV_draw_line(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, char c)
		{
			C_OWNER.TV_draw_line( X + x0, Y + y0, X + x1, Y + y1, c );
		}

		inline void TV_bitmap(uint8_t x, uint8_t y, const unsigned char * bmp, uint16_t i = 0, uint8_t width = 0, uint8_t lines = 0)
		{
			C_OWNER.TV_bitmap( X + x , Y + y, bmp, i, width, lines );
		}

		inline void TV_set_cursor( uint8_t x, uint8_t y )
		{
			C_OWNER.TV_set_cursor( X + x , Y + y );
		}

		inline void TV_draw_circle(uint8_t x0, uint8_t y0, uint8_t radius, char c, char fc = -1)
		{
			C_OWNER.TV_draw_circle( X + x0, Y + y0, radius, c, fc );
		}

		inline void TV_draw_rect(uint8_t x0, uint8_t y0, uint8_t w, uint8_t h, char c, char fc = -1)
		{
			C_OWNER.TV_draw_rect( X + x0, Y + y0, w, h, c, fc );
		}

		inline void TV_set_pixel(uint8_t x, uint8_t y, char c)
		{
			C_OWNER.TV_set_pixel( X + x , Y + y, c );
		}

		inline unsigned char TV_get_pixel(uint8_t x, uint8_t y)
		{
			return C_OWNER.TV_get_pixel( X + x , Y + y );
		}

	public:
		virtual void Render() override
		{
			for( int i = 0; i < FElements.size(); ++ i )
				FElements[ i ]->Render();
		}

	public:
		using inherited::inherited;

	};
//---------------------------------------------------------------------------
	template<typename T_OWNER, T_OWNER &C_OWNER> class TVOutRCAElementCheckPixel : public Mitov::TVOutRCAElementClocked<T_OWNER, C_OWNER>
	{
		typedef Mitov::TVOutRCAElementClocked<T_OWNER, C_OWNER> inherited;

	public:
		int32_t	X = 0;
		int32_t	Y = 0;

    public:
		OpenWire::SourcePin	OutputPin;

	public:
		virtual void Render() override
		{
			if( C_OWNER.IsEnabled() )
				OutputPin.SendValue( C_OWNER.TV_get_pixel( X, Y ) > 0 );
		}

	public:
		using inherited::inherited;

	};
//---------------------------------------------------------------------------
	template<typename T_OWNER, T_OWNER &C_OWNER> class TVOutRCAElementDrawBitmap : public Mitov::TVOutRCAElementClocked<T_OWNER, C_OWNER>
	{
		typedef Mitov::TVOutRCAElementClocked<T_OWNER, C_OWNER> inherited;

	public:
		int32_t	X = 0;
		int32_t	Y = 0;

    public:
		uint8_t	Width;
		uint8_t	Height;

		const unsigned char *_Bytes;

	public:
		virtual void Render() override
		{
			C_OWNER.TV_bitmap( X, Y, _Bytes, 0, Width, Height );
		}

	public:
		using inherited::inherited;

	};
//---------------------------------------------------------------------------
	template<typename T_OWNER, T_OWNER &C_OWNER> class TVOutRCAElementSetCursor : public Mitov::TVOutRCAElementClocked<T_OWNER, C_OWNER>
	{
		typedef Mitov::TVOutRCAElementClocked<T_OWNER, C_OWNER> inherited;

	public:
		int32_t	X = 0;
		int32_t	Y = 0;

	public:
		virtual void Render() override
		{
			C_OWNER.TV_set_cursor( X, Y );
		}

	public:
		using inherited::inherited;

	};
//---------------------------------------------------------------------------
	template<typename T_OWNER, T_OWNER &C_OWNER> class TVOutRCAElementPrintText : public Mitov::TVOutRCAElementBasic<T_OWNER>
	{
		typedef Mitov::TVOutRCAElementBasic<T_OWNER> inherited;

	public:
		bool NewLine = true;

	protected:
		const unsigned char *FFont;

	public:
		template<typename T> void Print( T AValue )
		{
			if( C_OWNER.IsEnabled() )
			{
				C_OWNER.GetTV().select_font( FFont );
				if( NewLine )
					C_OWNER.GetTV().println( AValue );

				else
					C_OWNER.GetTV().print( AValue );
			}
		}

	public:
		TVOutRCAElementPrintText( const unsigned char * AFont ) :
			inherited( C_OWNER ),
			FFont( AFont )
		{
		}

	};
//---------------------------------------------------------------------------
	template<typename T_OWNER, T_OWNER &C_OWNER> class TVOutRCAElementTextAt : public Mitov::TVOutRCAElementClocked<T_OWNER, C_OWNER>
	{
		typedef Mitov::TVOutRCAElementClocked<T_OWNER, C_OWNER> inherited;

	public:
		int32_t X = 0;
		int32_t Y = 0;
		Mitov::String	InitialValue;

	protected:
		const unsigned char *FFont;

	public:
		template<typename T> void Print( T AValue )
		{
			 InitialValue = Mitov::String( AValue );
		}

	public:
		virtual void Render() override
		{
			if( C_OWNER.IsEnabled() )
			{
				C_OWNER.GetTV().select_font( FFont );
				C_OWNER.TV_set_cursor( X, Y );
				C_OWNER.GetTV().print( InitialValue.c_str() );
			}
		}

	public:
		TVOutRCAElementTextAt( const unsigned char * AFont ) :
			FFont( AFont )
		{
		}

	};
//---------------------------------------------------------------------------
}

#endif
