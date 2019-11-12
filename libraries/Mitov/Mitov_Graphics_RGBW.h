////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2018 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_GRAPHICS_RGBW_h
#define _MITOV_GRAPHICS_RGBW_h

#include <Mitov.h>
#include <Mitov_Graphics_Color.h>

#ifdef __TEST_FOR_DEBUG_PRINTS__
#define Serial UNGUARDED DEBUG PRINT!!!
#endif

namespace Mitov
{
//---------------------------------------------------------------------------
	template<typename T_OWNER, T_OWNER &C_OWNER> class GraphicsRGBWElementFillScreen
	{
	public:
		uint64_t	Color : 40;

	public:
		void Render()
		{
			T_OWNER::ClearScreen( C_OWNER, Color );
		}

	public:
		inline void ClockInputPin_o_Receive( void *_Data )
		{
			Render();
		}

	public:
		GraphicsRGBWElementFillScreen() :
			Color( 0xFF00000000 )
		{
		}

	};
//---------------------------------------------------------------------------
	template<typename T_OWNER, T_OWNER &C_OWNER> class GraphicsRGBWElementDrawPixel
	{
	public:
		uint64_t		Color : 40;
		TGraphicsPos	X = 0;
		TGraphicsPos	Y = 0;

	public:
		void Render()
		{
			if( !( Color & 0xFF00000000 ))
				return;

			TGraphicsPos AParentX, AParentY;
			C_OWNER.GetPosition( AParentX, AParentY );

			C_OWNER.drawPixel( AParentX + X, AParentY + Y, Color );
		}

	public:
		inline void ClockInputPin_o_Receive( void *_Data )
		{
			Render();
		}

	public:
		GraphicsRGBWElementDrawPixel() :
			Color( 0xFFFFFFFFFF )
		{
		}

	};
//---------------------------------------------------------------------------
	template<typename T_OWNER, T_OWNER &C_OWNER> class GraphicsRGBWElementDrawRectangle
	{
	public:
		uint64_t		Color : 40;
		uint64_t		FillColor : 40;
		TGraphicsPos	X = 0;
		TGraphicsPos	Y = 0;
		TGraphicsSize	Width = 40;
		TGraphicsSize	Height = 20;

	public:
		void Render()
		{
			if( !( ( FillColor & 0xFF00000000 ) || ( Color & 0xFF00000000 )))
				return;

			TGraphicsPos AParentX, AParentY;
			C_OWNER.GetPosition( AParentX, AParentY );

			if( FillColor & 0xFF00000000 )
				T_OWNER::fillRect( C_OWNER, AParentX + X + 1, AParentY + Y + 1, Width - 2, Height - 2, FillColor );

			if( Color & 0xFF00000000 )
				T_OWNER::drawRect( C_OWNER, AParentX + X, AParentY + Y, Width, Height, Color );
		}

	public:
		inline void ClockInputPin_o_Receive( void *_Data )
		{
			Render();
		}

	public:
		GraphicsRGBWElementDrawRectangle() :
			Color( 0xFFFFFFFFFF ),
			FillColor( 0 )
		{
		}

	};
//---------------------------------------------------------------------------
	template<typename T_OWNER, T_OWNER &C_OWNER> class GraphicsRGBWElementDrawRoundRectangle
	{
	public:
		uint64_t					Color : 40;
		uint64_t					FillColor : 40;
		TGraphicsSize				X = 0;
		TGraphicsSize				Y = 0;
		TGraphicsSize				Width = 40;
		TGraphicsSize				Height = 20;
		TArduinoGraphicsCornerSize	CornerSize;

	public:
		void Render()
		{
			if( !( ( FillColor & 0xFF00000000 ) || ( Color & 0xFF00000000 )))
				return;

			TGraphicsPos AParentX, AParentY;
			C_OWNER.GetPosition( AParentX, AParentY );

			if( FillColor & 0xFF00000000 )
				T_OWNER::fillRoundRect( C_OWNER, AParentX + X, AParentY + Y, Width, Height, CornerSize.X, CornerSize.Y, FillColor );

			if( Color & 0xFF00000000 )
				T_OWNER::drawRoundRect( C_OWNER, AParentX + X, AParentY + Y, Width, Height, CornerSize.X, CornerSize.Y, Color );
		}

	public:
		inline void ClockInputPin_o_Receive( void *_Data )
		{
			Render();
		}

	public:
		GraphicsRGBWElementDrawRoundRectangle() :
			Color( 0xFFFFFFFFFF ),
			FillColor( 0 )
		{
		}

	};
//---------------------------------------------------------------------------
	template<typename T_OWNER, T_OWNER &C_OWNER> class GraphicsRGBWElementDrawEllipse
	{
	public:
		uint64_t		Color : 40;
		uint64_t		FillColor : 40;
		TGraphicsPos	X = 0;
		TGraphicsPos	Y = 0;
		TGraphicsSize	Width = 40;
		TGraphicsSize	Height = 20;
		bool			Centered	= false;

	public:
		void Render()
		{
			if( !( ( FillColor & 0xFF00000000 ) || ( Color & 0xFF00000000 )))
				return;

			TGraphicsPos AParentX, AParentY;
			C_OWNER.GetPosition( AParentX, AParentY );

			TGraphicsPos	AX = X;
			TGraphicsPos	AY = Y;

			if( Centered )
			{
				AX -= Width / 2;
				AY -= Height / 2;
			}

			if( FillColor & 0xFF00000000 )
				T_OWNER::fillEllipseWH( C_OWNER, AParentX + AX, AParentY + AY, Width, Height, FillColor );

			if( Color & 0xFF00000000 )
				T_OWNER::drawEllipseWH( C_OWNER, AParentX + AX, AParentY + AY, Width, Height, Color );
		}

	public:
		inline void ClockInputPin_o_Receive( void *_Data )
		{
			Render();
		}

	public:
		GraphicsRGBWElementDrawEllipse() :
			Color( 0xFFFFFFFFFF ),
			FillColor( 0 )
		{
		}

	};
//---------------------------------------------------------------------------
	template<typename T_OWNER, T_OWNER &C_OWNER> class GraphicsRGBWElementDrawLine
	{
	public:
		uint64_t		Color : 40;
		TGraphicsPos	X = 0;
		TGraphicsPos	Y = 0;
		TGraphicsPos	Width = 40;
		TGraphicsPos	Height = 20;

	public:
		void Render()
		{
			if( ! ( Color & 0xFF00000000 ))
				return;

			TGraphicsPos AParentX, AParentY;
			C_OWNER.GetPosition( AParentX, AParentY );

			T_OWNER::drawLine( C_OWNER, AParentX + X, AParentY + Y, AParentX + X + Width, AParentY + Y + Height, Color );
		}

	public:
		inline void ClockInputPin_o_Receive( void *_Data )
		{
			Render();
		}

	public:
		GraphicsRGBWElementDrawLine() :
			Color( 0xFFFFFFFFFF )
		{
		}

	};
//---------------------------------------------------------------------------
	template<typename T_OWNER, T_OWNER &C_OWNER> class GraphicsRGBWElementDrawLines
	{
	public:
		uint64_t		Color : 40;
		TGraphicsPos	X = 0;
		TGraphicsPos	Y = 0;

	public:
		GraphicsPoint	*Points;
		uint32_t		_PointsCount = 0;

	public:
		void Render()
		{
			if( _PointsCount == 0 )
				return;

			if( !( Color & 0xFF00000000 ))
				return;

			TGraphicsPos AParentX, AParentY;
			C_OWNER.GetPosition( AParentX, AParentY );

			TGraphicsPos	AX1 = X;
			TGraphicsPos	AY1 = Y;

			for( uint32_t i = 0; i < _PointsCount; ++i )
			{
				TGraphicsPos	AX2 = Points[ i ].X + X;
				TGraphicsPos	AY2 = Points[ i ].Y + Y;

				T_OWNER::drawLine( C_OWNER, AParentX + AX1, AParentY + AY1, AParentX + AX2, AParentY + AY2, Color );

				AX1 = AX2;
				AY1 = AY2;
			}
		}

	public:
		inline void ClockInputPin_o_Receive( void *_Data )
		{
			Render();
		}

	public:
		GraphicsRGBWElementDrawLines() :
			Color( 0xFFFFFFFFFF )
		{
		}

	};
//---------------------------------------------------------------------------
	template<typename T_OWNER, T_OWNER &C_OWNER> class GraphicsRGBWElementDrawPolygon
	{
	public:
		uint64_t		Color : 40;
		uint64_t		FillColor : 40;
		TGraphicsPos	X = 0;
		TGraphicsPos	Y = 0;

	public:
		GraphicsPoint	*Points;
		uint32_t		_PointsCount = 0;

	public:
		void Render()
		{
			if( _PointsCount == 0 )
				return;

			if( !( ( FillColor & 0xFF00000000 ) || ( Color & 0xFF00000000 )))
				return;

			TGraphicsPos AParentX, AParentY;
			C_OWNER.GetPosition( AParentX, AParentY );

			TGraphicsPos	AStartX = AParentX + X;
			TGraphicsPos	AStartY = AParentY + Y;

			if( FillColor & 0xFF00000000 )
			{
				TGraphicsPos	AMinX = AStartX;
				TGraphicsPos	AMinY = AStartY;
				TGraphicsPos	AMaxX = AStartX;
				TGraphicsPos	AMaxY = AStartY;

				for( uint32_t i = 0; i < _PointsCount; ++i )
				{
					TGraphicsPos	AX2 = Points[ i ].X + AStartX;
					TGraphicsPos	AY2 = Points[ i ].Y + AStartY;

					if( AX2 < AMinX )
						AMinX = AX2;

					if( AX2 > AMaxX )
						AMaxX = AX2;

					if( AY2 < AMinY )
						AMinY = AY2;

					if( AY2 > AMaxY )
						AMaxY = AY2;

				}

				if( AMinX < 0 )
				 AMinX = 0;

				if( AMinY < 0 )
				 AMinY = 0;

				TGraphicsPos	AWidth = C_OWNER.width();
				TGraphicsPos	AHeight = C_OWNER.height();

				if( AMaxX > AWidth )
				 AMaxX = AWidth;

				if( AMaxY > AHeight )
				 AMaxY = AHeight;

				if( AMinX > AMaxX )
					return;

				if( AMinY > AMaxY )
					return;

				TGraphicsPos *nodeX = new TGraphicsPos[ _PointsCount ];
				for( int pixelY = AMinY; pixelY < AMaxY; ++ pixelY )
				{
					//  Build a list of nodes.
					int nodes = 0;
					int j = _PointsCount - 1;
					for( uint32_t i = 0; i < _PointsCount; i++ )
					{
						if ( (( Points[ i ].Y + AStartY ) < pixelY && ( Points[ j ].Y + AStartY ) >= pixelY ) || (( Points[ j ].Y + AStartY ) < pixelY && ( Points[ i ].Y + AStartY ) >= pixelY ))
							nodeX[nodes ++] = ( Points[i].X + AStartX ) + float( pixelY - ( Points[i].Y + AStartY ) ) / ( Points[j].Y - Points[i].Y ) * float( (Points[j].X - Points[i].X ) ) + 0.5;

						j = i;
					}

					//  Sort the nodes, via a simple “Bubble” sort.
					int i = 0;
					while( i < nodes - 1 )
					{
						if( nodeX[ i ] > nodeX[ i + 1 ] )
						{
							TGraphicsPos swap = nodeX[ i ];
							nodeX[ i ] = nodeX[ i + 1 ];
							nodeX[ i + 1 ] = swap;
							if( i )
								i--;
						}

						else
							i++;
						
					}

					//  Fill the pixels between node pairs.
					for( int i = 0; i < nodes; i += 2 ) 
					{
						TGraphicsPos ALeft = nodeX[ i ];
						TGraphicsPos ARight = nodeX[ i + 1 ];

						if( ALeft >= AWidth )
							break;

						if( ARight > 0 ) 
						{
							if( ALeft < 0 ) 
								ALeft = 0;

							if( ARight > AWidth )
								ARight = AWidth;

							T_OWNER::drawFastHLine( C_OWNER, ALeft, pixelY, ARight - ALeft, FillColor );

	//					for (pixelX=nodeX[i]; pixelX<nodeX[i+1]; pixelX++) fillPixel(pixelX,pixelY); 
						}
					}

				}

			}

			if( Color & 0xFF00000000 )
			{
				TGraphicsPos	AX1 = X;
				TGraphicsPos	AY1 = Y;

				for( uint32_t i = 1; i < _PointsCount; ++i )
				{
					TGraphicsPos	AX2 = Points[ i ].X + X;
					TGraphicsPos	AY2 = Points[ i ].Y + Y;

					T_OWNER::drawLine( C_OWNER, AParentX + AX1, AParentY + AY1, AParentX + AX2, AParentY + AY2, Color );

					AX1 = AX2;
					AY1 = AY2;
				}

				T_OWNER::drawLine( C_OWNER, AParentX + AX1, AParentY + AY1, AParentX + X, AParentY + Y, Color );
			}
		}

	public:
		inline void ClockInputPin_o_Receive( void *_Data )
		{
			Render();
		}

	public:
		GraphicsRGBWElementDrawPolygon() :
			Color( 0xFFFFFFFFFF ),
			FillColor( 0 )
		{
		}

	};
//---------------------------------------------------------------------------
	template<typename T_OWNER, T_OWNER &C_OWNER> class GraphicsRGBWElementDrawText : public GraphicsTextIntf, public GraphicsDelegate<GraphicsRGBWElementDrawText<T_OWNER, C_OWNER>, T_OWNER, C_OWNER>
	{
	public:
		uint64_t		Color : 40;
		uint64_t		FillColor : 40;
		TGraphicsPos	X = 0;
		TGraphicsPos	Y = 0;

		Mitov::String	Text;

		uint8_t			Size : 4;
		bool			Wrap : 1;

	public:
		void Render()
		{
			if( !( ( FillColor & 0xFF00000000 ) || ( Color & 0xFF00000000 )))
				return;

			TGraphicsPos AParentX, AParentY;
			C_OWNER.GetPosition( AParentX, AParentY );

			int16_t ACursorY; // = AGraphics->getCursorY();
			int16_t ACursorX; // = AGraphics->getCursorX();

			C_OWNER.getCursor( ACursorX, ACursorY );
			TUniColor AOldColor;
			TUniColor AOldBackgroundColor;

			uint8_t	AOldSize = C_OWNER.getTextSize();
			bool AOldWrap = C_OWNER.getTextWrap();

			EnterLeave( true );

			C_OWNER.setTextSize( Size );
			C_OWNER.setTextWrap( Wrap );			

			C_OWNER.getTextColor( AOldColor, AOldBackgroundColor );

			C_OWNER.setTextColor( Color, FillColor );

			C_OWNER.setCursor( X, Y );
			C_OWNER.print( Text );

			C_OWNER.setTextColor( AOldColor, AOldBackgroundColor );

			C_OWNER.setCursor( ACursorX, ACursorY );

			C_OWNER.setTextWrap( AOldWrap );
			C_OWNER.setTextSize( AOldSize );

			EnterLeave( false );
		}

	public:
		inline void ClockInputPin_o_Receive( void *_Data )
		{
			Render();
		}

	public:
		GraphicsRGBWElementDrawText() :
			Size( 1 ),
			Wrap( true ),
			Color( 0xFFFFFFFFFF ),
			FillColor( 0 )
		{
		}

	};
//---------------------------------------------------------------------------
	template<typename T_OWNER, T_OWNER &C_OWNER> class GraphicsRGBWElementTextField : public GraphicsTextIntf, public GraphicsDelegate<GraphicsRGBWElementTextField<T_OWNER, C_OWNER>, T_OWNER, C_OWNER>
	{
	public:
		bool	ClockInputPin_o_IsConnected	: 1;

	public:
		bool						AutoSize : 1;
		bool						Wrap : 1;
		uint8_t						Size : 4;
		TArduinoTextHorizontalAlign	HorizontalAlign : 2;
		TGraphicsPos				X = 0;
		TGraphicsPos				Y = 0;
		uint16_t					Width = 6;
		uint64_t					Color : 40;
		uint64_t					FillColor : 40;
		Mitov::String				InitialValue;

	protected:
		uint8_t	FOldSize = 0;

	public:
		void Render()
		{
			if( !( ( FillColor & 0xFF00000000 ) || ( Color & 0xFF00000000 )))
				return;

			TGraphicsPos AParentX, AParentY;
			C_OWNER.GetPosition( AParentX, AParentY );

			int16_t ACursorY; // = AGraphics->getCursorY();
			int16_t ACursorX; // = AGraphics->getCursorX();

			C_OWNER.getCursor( ACursorX, ACursorY );

			TUniColor AOldColor;
			TUniColor AOldBackgroundColor;

			EnterLeave( true );

			uint8_t	AOldSize = C_OWNER.getTextSize();
			bool AOldWrap = C_OWNER.getTextWrap();

			C_OWNER.setTextSize( Size );
			C_OWNER.setTextWrap( Wrap );			

			C_OWNER.getTextColor( AOldColor, AOldBackgroundColor );

			C_OWNER.setTextColor( Color, FillColor );

			C_OWNER.setCursor( X, Y );
			Mitov::String AText = InitialValue;
			if( AutoSize )
			{
				uint8_t ANewSize = AText.length();
				while( AText.length() < FOldSize )
					AText += " ";				

				FOldSize = ANewSize;
			}

			else
			{
				while( AText.length() < Width )
				{
					switch( HorizontalAlign )
					{
						case thaLeft :
							AText += " ";
							break;

						case thaRight :
							AText = " " + AText;
							break;

						case thaCenter :
							AText = " " + AText + " ";
							break;
					}
				}

				AText = AText.substring( 0, Width );
			}

			C_OWNER.print( AText );

			C_OWNER.setTextColor( AOldColor, AOldBackgroundColor );
			C_OWNER.setCursor( ACursorX, ACursorY );
			C_OWNER.setTextWrap( AOldWrap );			
			C_OWNER.setTextSize( AOldSize );

			EnterLeave( false );
		}

	public:
		inline void ClockInputPin_o_Receive( void *_Data )
		{
			Render();
		}

		void InputPin_o_Receive( void *_Data )
		{
			InitialValue = (char *)_Data;
			if( ! ClockInputPin_o_IsConnected )
				Render( true );
		}

	public:
		GraphicsRGBWElementTextField() :
			ClockInputPin_o_IsConnected( false ),
			HorizontalAlign( thaLeft ),
			AutoSize( true ),
			Wrap( true ),
			Size( 1 ),
			Color( 0xFFFFFFFFFF ),
			FillColor( 0xFF00000000 )
		{
		}

	};
//---------------------------------------------------------------------------
	template<typename T_OWNER, T_OWNER &C_OWNER> class GraphicsRGBWElementScroll
	{
	public:
		uint64_t		Color : 40;
		TGraphicsPos	X = 0;
		TGraphicsPos	Y = 0;

	public:
		inline void Render()
		{
			T_OWNER::Scroll( C_OWNER, X, Y, Color );
		}

	public:
		inline void ClockInputPin_o_Receive( void *_Data )
		{
			Render();
		}

	public:
		GraphicsRGBWElementScroll() :
			Color( 0xFF00000000 )
		{
		}

	};
//---------------------------------------------------------------------------
	template<typename T_OWNER, T_OWNER &C_OWNER> class GraphicsRGBWElementCheckPixel
	{
	public:
		OpenWire::SourcePin	OutputPin;

	public:
		uint32_t X = 0;
		uint32_t Y = 0;

	public:
		void Render()
		{
			uint32_t AValue = C_OWNER.GetPixelColor( X, Y );
			OutputPin.SendValue<uint64_t>( AValue );
		}

	public:
		inline void ClockInputPin_o_Receive( void *_Data )
		{
			Render( true );
		}

	};
//---------------------------------------------------------------------------
//#ifdef _GRAPHICS_DRAW_ALPHA_RGBW_COLOR_BITMAP_

	template<typename T_OWNER, T_OWNER &C_OWNER> class GraphicsRGBWElementDrawAlphaBitmap
	{
    public:
		uint8_t	Size = 1;

    public:
		TGraphicsPos	X = 0;
		TGraphicsPos	Y = 0;
		uint16_t		Width;
		uint16_t		Height;

		const uint8_t *_Data;

	public:
		void Render()
		{
			TGraphicsPos AParentX, AParentY;
			C_OWNER.GetPosition( AParentX, AParentY );

			T_OWNER::drawAlphaRGBWColorBitmap( C_OWNER, AParentX + X, AParentY + Y, _Data, Width, Height, Size );
		}

	public:
		inline void ClockInputPin_o_Receive( void *_Data )
		{
			Render();
		}

	};

//#endif // _GRAPHICS_DRAW_ALPHA_RGBW_COLOR_BITMAP_
//---------------------------------------------------------------------------	
//#ifdef _GRAPHICS_DRAW_REPEAT_ALPHA_RGBW_COLOR_BITMAP_

	template<typename T_OWNER, T_OWNER &C_OWNER> class GraphicsRGBWRepeatElementDrawAlphaBitmap
	{
    public:
		uint8_t	Size = 1;

    public:
		TGraphicsPos	X = 0;
		TGraphicsPos	Y = 0;
		uint16_t		Width;
		uint16_t		Height;
		uint16_t		Count;

		const uint8_t *_Data;

	public:
		void Render()
		{
			TGraphicsPos AParentX, AParentY;
			C_OWNER.GetPosition( AParentX, AParentY );

			T_OWNER::drawRepeatAlphaRGBWColorBitmap( C_OWNER, AParentX + X, AParentY + Y, _Data, Count, Width, Height, Size );
		}

	public:
		inline void ClockInputPin_o_Receive( void *_Data )
		{
			Render();
		}

	};

//#endif // _GRAPHICS_DRAW_REPEAT_ALPHA_RGBW_COLOR_BITMAP_
//---------------------------------------------------------------------------	
//#ifdef _GRAPHICS_DRAW_RGBW_COLOR_BITMAP_

	template<typename T_OWNER, T_OWNER &C_OWNER> class GraphicsRGBWElementDrawBitmap
	{
    public:
		uint8_t	Size = 1;

    public:
		TGraphicsPos	X = 0;
		TGraphicsPos	Y = 0;
		uint16_t		Width;
		uint16_t		Height;

		const uint8_t *_Bytes;

	public:
		void Render()
		{
			TGraphicsPos AParentX, AParentY;
			C_OWNER.GetPosition( AParentX, AParentY );

			T_OWNER::drawRGBWColorBitmap( C_OWNER, AParentX + X, AParentY + Y, _Bytes, Width, Height, Size );
		}

	public:
		inline void ClockInputPin_o_Receive( void *_Data )
		{
			Render();
		}

	};

//#endif // _GRAPHICS_DRAW_RGBW_COLOR_BITMAP_
//---------------------------------------------------------------------------	
//#ifdef _GRAPHICS_DRAW_REPEAT_RGBW_COLOR_BITMAP_

	template<typename T_OWNER, T_OWNER &C_OWNER> class GraphicsRGBWRepeatElementDrawBitmap
	{
    public:
		uint8_t	Size = 1;

    public:
		TGraphicsPos	X = 0;
		TGraphicsPos	Y = 0;
		uint16_t		Width;
		uint16_t		Height;
		uint16_t		Count;

		const uint8_t *_Data;

	public:
		void Render()
		{
			TGraphicsPos AParentX, AParentY;
			C_OWNER.GetPosition( AParentX, AParentY );

			T_OWNER::drawRepeatRGBWColorBitmap( C_OWNER, AParentX + X, AParentY + Y, _Data, Count, Width, Height, Size );
		}

	public:
		inline void ClockInputPin_o_Receive( void *_Data )
		{
			Render();
		}

	};

//#endif // _GRAPHICS_DRAW_REPEAT_RGBW_COLOR_BITMAP_
//---------------------------------------------------------------------------	
	template<typename T_OWNER, T_OWNER &C_OWNER> class GraphicsElementRGBWMonohromeDrawScene : public GraphicsIntf<T_OWNER>, public Graphics<GraphicsElementRGBWMonohromeDrawScene<T_OWNER,C_OWNER>>
	{
		typedef GraphicsIntf<T_OWNER> inherited;
		typedef GraphicsElementRGBWMonohromeDrawScene<T_OWNER,C_OWNER>	self;
		typedef Graphics<GraphicsElementRGBWMonohromeDrawScene<T_OWNER,C_OWNER>>	graphics;

    public:
		uint64_t		WhiteColor : 40;
		uint64_t		BlackColor : 40;
		uint64_t		BackgroundColor : 40;
		TGraphicsPos	X = 0;
		TGraphicsPos	Y = 0;

	public:
		void GetPosition( TGraphicsPos &AX, TGraphicsPos &AY )
		{ 
			TGraphicsPos AParentX, AParentY;
			C_OWNER.GetPosition( AParentX, AParentY );

			AX = AParentX + X;
			AY = AParentY + Y;
		}

	public:
		inline TUniColor	GetBackgroundColor()
		{
			return BackgroundColor;
		}

		inline int16_t width(void) const
		{
			return C_OWNER.width() - X;
		}

		inline int16_t height(void) const
		{
			return C_OWNER.height() - Y;
		}

		void drawPixel(int16_t x, int16_t y, TUniColor color)
		{
			if( color == tmcNone )
				return;

			switch( color )
			{
				case tmcWhite:	color =	WhiteColor; break;
				case tmcBlack:	color =	BlackColor; break;
				case tmcInvert:	color =	( C_OWNER.GetPixelColor( x, y ) > 0xFF00000000 ) ? WhiteColor : BlackColor; break;
			}

//			color = WhiteColor; //0xFF0000ff00;
//			color = 0xFFFFFF0000;
			//color = 0xFF000000FF;
			C_OWNER.drawPixel( x, y, color );
		}

		inline TUniColor GetPixelColor( int16_t x, int16_t y )
		{
			return ( C_OWNER.GetPixelColor( x, y ) > 0xFF00000000 ) ? tmcWhite : tmcBlack;
		}

	public:
		inline void Render()
		{
			inherited::RenderElements();
//			for( int i = 0; i < GraphicsIntf::FElements.size(); ++ i )
//				GraphicsIntf::FElements[ i ]->Render( AForce );
		}

	public:
		inline void ClockInputPin_o_Receive( void *_Data )
		{
			Render();
		}

	public:
		inline static size_t write( GraphicsElementRGBWMonohromeDrawScene &AImplementation, uint8_t AValue )
		{
			return graphics::write( AImplementation, AValue );
		}

		template<typename T> void PrintChar( T AValue )
		{
			write( *this, AValue );
		}

	public:
		template<typename T> void Print( T AValue )
		{
			println( AValue );
		}

	public:
		template<typename T> size_t print( T AValue )
		{
			GraphicsPrint<self> APrint( this, (typename GraphicsPrint<self>::TCallback) &GraphicsElementRGBWMonohromeDrawScene::write );
			return APrint.print( AValue );
		}

		template<typename T> size_t println( T AValue )
		{
			GraphicsPrint<self> APrint( this, (typename GraphicsPrint<self>::TCallback) &GraphicsElementRGBWMonohromeDrawScene::write );
			return APrint.println( AValue );
		}

	public:
		GraphicsElementRGBWMonohromeDrawScene( const unsigned char *AFont ) :
			Graphics<GraphicsElementRGBWMonohromeDrawScene<T_OWNER,C_OWNER>>( AFont ),
			WhiteColor( 0xFFFFFFFFFF ),
			BlackColor( 0xFF00000000 ),
			BackgroundColor( 0 )
		{
		}

	};
//---------------------------------------------------------------------------
	template<typename T_OWNER, T_OWNER &C_OWNER> class GraphicsElementRGBWGrayscaleDrawScene : public GraphicsIntf<T_OWNER>, public Graphics<GraphicsElementRGBWGrayscaleDrawScene<T_OWNER,C_OWNER>>
	{
		typedef GraphicsIntf<T_OWNER> inherited;
		typedef GraphicsElementRGBWGrayscaleDrawScene<T_OWNER,C_OWNER>	self;
		typedef Graphics<GraphicsElementRGBWGrayscaleDrawScene<T_OWNER,C_OWNER>>	graphics;

    public:
		uint64_t		BackgroundColor : 40;
		TGraphicsPos	X = 0;
		TGraphicsPos	Y = 0;

	public:
		void GetPosition( TGraphicsPos &AX, TGraphicsPos &AY )
		{ 
			TGraphicsPos AParentX, AParentY;
			C_OWNER.GetPosition( AParentX, AParentY );

			AX = AParentX + X;
			AY = AParentY + Y;
		}

	public:
		inline TUniColor GetBackgroundColor() const
		{
			return BackgroundColor;
		}

		inline int16_t width(void) const
		{
			return C_OWNER.width() - X;
		}

		inline int16_t height(void) const
		{
			return C_OWNER.height() - Y;
		}

		static void drawPixel(int16_t x, int16_t y, TUniColor color)
		{
			if( !( color & 0xFF00 ))
				return;

			uint16_t AGray = color & 0xFF;
			color = 0xFF00000000 | ( color << 16 ) | ( AGray << 8 ) | AGray;

			C_OWNER.drawPixel( x, y, color );
		}

		static TUniColor GetPixelColor( int16_t x, int16_t y )
		{
			uint32_t	AColor = C_OWNER.GetPixelColor( x, y );
			uint8_t ARed = AColor >> 16;
			uint8_t AGreen = AColor >> 8;
			uint8_t ABlue = AColor >> 8;
			uint16_t AGray = 0.2989 * ARed + 0.5870 * AGreen + 0.1140 * ABlue + 0.5;
			AGray |= ( AColor >> 16 ) & 0xFF00;
			return AGray;
		}

/*
	protected:
		inline void RenderElements()
		{
			C_OWNER.fillRect( C_OWNER, 0, 0, width(), height(), BackgroundColor );
			inherited::RenderElements();
		}
*/
	public:
		inline void Render()
		{
			inherited::RenderElements();
//			for( int i = 0; i < GraphicsIntf::FElements.size(); ++ i )
//				GraphicsIntf::FElements[ i ]->Render( AForce );
		}

	public:
		inline void ClockInputPin_o_Receive( void *_Data )
		{
			Render();
		}

	public:
		inline static size_t write( GraphicsElementRGBWGrayscaleDrawScene &AImplementation, uint8_t AValue )
		{
			return graphics::write( AImplementation, AValue );
		}

		template<typename T> void PrintChar( T AValue )
		{
			write( *this, AValue );
		}

	public:
		template<typename T> void Print( T AValue )
		{
			println( AValue );
		}

	public:
		template<typename T> size_t print( T AValue )
		{
			GraphicsPrint<self> APrint( this, (typename GraphicsPrint<self>::TCallback) &GraphicsElementRGBWGrayscaleDrawScene::write );
			return APrint.print( AValue );
		}

		template<typename T> size_t println( T AValue )
		{
			GraphicsPrint<self> APrint( this, (typename GraphicsPrint<self>::TCallback) &GraphicsElementRGBWGrayscaleDrawScene::write );
			return APrint.println( AValue );
		}

	public:
		GraphicsElementRGBWGrayscaleDrawScene( const unsigned char *AFont ) :
			Graphics<GraphicsElementRGBWGrayscaleDrawScene<T_OWNER,C_OWNER>>( AFont ),
			BackgroundColor( 0 )
		{
		}

	};
//---------------------------------------------------------------------------
	template<typename T_OWNER, T_OWNER &C_OWNER> class GraphicsElementRGBWColorDrawScene : public GraphicsIntf<T_OWNER>, public Graphics<GraphicsElementRGBWColorDrawScene<T_OWNER,C_OWNER>>
	{
		typedef GraphicsIntf<T_OWNER> inherited;
		typedef GraphicsElementRGBWColorDrawScene<T_OWNER,C_OWNER>	self;
		typedef Graphics<GraphicsElementRGBWColorDrawScene<T_OWNER,C_OWNER>>	graphics;

    public:
		uint64_t		BackgroundColor : 40;
		TGraphicsPos	X = 0;
		TGraphicsPos	Y = 0;

	public:
		void GetPosition( TGraphicsPos &AX, TGraphicsPos &AY )
		{ 
			TGraphicsPos AParentX, AParentY;
			C_OWNER.GetPosition( AParentX, AParentY );

			AX = AParentX + X;
			AY = AParentY + Y;
		}

	public:
		inline TUniColor GetBackgroundColor() const
		{
			return BackgroundColor;
		}

		inline int16_t width(void) const
		{
			return C_OWNER.width() - X;
		}

		inline int16_t height(void) const
		{
			return C_OWNER.height() - Y;
		}

		static void drawPixel(int16_t x, int16_t y, TUniColor color)
		{
			if( !( color & 0xFF000000 ))
				return;

			color = (( color & 0xFF000000 ) << 8 ) | ( color & 0xFFFFFF );

//			uint16_t AGray = color & 0xFF;
//			color = ( color << 16 ) | ( AGray << 8 ) | AGray;

			C_OWNER.drawPixel( x, y, color );
		}

		static TUniColor GetPixelColor( int16_t x, int16_t y )
		{
			TUniColor	AColor = C_OWNER.GetPixelColor( x, y );

			AColor = (( AColor & 0xFF00000000 ) >> 8 ) | ( AColor & 0xFFFFFF );

			return AColor;
		}

	public:
		inline void Render()
		{
			inherited::RenderElements();
//			for( int i = 0; i < GraphicsIntf::FElements.size(); ++ i )
//				GraphicsIntf::FElements[ i ]->Render( AForce );
		}

	public:
		inline void ClockInputPin_o_Receive( void *_Data )
		{
			Render();
		}

	public:
		inline static size_t write( GraphicsElementRGBWColorDrawScene &AImplementation, uint8_t AValue )
		{
			return graphics::write( AImplementation, AValue );
		}

		template<typename T> void PrintChar( T AValue )
		{
			write( *this, AValue );
		}

	public:
		template<typename T> void Print( T AValue )
		{
			println( AValue );
		}

	public:
		template<typename T> size_t print( T AValue )
		{
			GraphicsPrint<self> APrint( this, (typename GraphicsPrint<self>::TCallback) &GraphicsElementRGBWColorDrawScene::write );
			return APrint.print( AValue );
		}

		template<typename T> size_t println( T AValue )
		{
			GraphicsPrint<self> APrint( this, (typename GraphicsPrint<self>::TCallback) &GraphicsElementRGBWColorDrawScene::write );
			return APrint.println( AValue );
		}

	public:
		GraphicsElementRGBWColorDrawScene( const unsigned char *AFont ) :
			Graphics<GraphicsElementRGBWColorDrawScene<T_OWNER,C_OWNER>>( AFont ),
			BackgroundColor( 0 )
		{
		}

	};
//---------------------------------------------------------------------------
	template<typename T_OWNER, T_OWNER &C_OWNER, typename T_CARD, T_CARD &C_CARD> class GraphicsRGBWElementDrawGraphics
	{
    public:
		TGraphicsPos	X = 0;
		TGraphicsPos	Y = 0;

	public:
		void Render()
		{
			TGraphicsPos AParentX, AParentY;
			C_OWNER.GetPosition( AParentX, AParentY );

			C_CARD.template RenderGraphics<GraphicsRGBWElementDrawGraphics<T_OWNER, C_OWNER, T_CARD, C_CARD>>( this, AParentX + X, AParentY + Y );

//			FOwner.GetGraphics()->drawRepeatColorBitmap( AParentX + X, AParentY + Y, _Data, Count, Width, Height, Size );
		}

	public:
		inline TUniColor MakeColorColor( uint8_t r, uint8_t g, uint8_t b )
		{
			return 0xFF00000000 | ( uint32_t( r ) << 16 ) | ( uint32_t( g ) << 8 ) | b;
		}

	public:
		inline void ClockInputPin_o_Receive( void *_Data )
		{
			Render();
		}

	};
//---------------------------------------------------------------------------	
	class TGraphicsRGBWTextSettings
	{
	public:
		uint8_t		Size : 4;
		bool		Wrap : 1;
		uint64_t	Color : 40;
		uint64_t	BackgroundColor : 40;

	public:
		TGraphicsRGBWTextSettings() :
			Size( 1 ),
			Wrap( true ),
			Color( 0xFFFFFFFFFF ),
			BackgroundColor( 0xFF00000000 )
		{
		}
	};
//---------------------------------------------------------------------------
}

#ifdef __TEST_FOR_DEBUG_PRINTS__
#undef Serial
#endif

#endif
