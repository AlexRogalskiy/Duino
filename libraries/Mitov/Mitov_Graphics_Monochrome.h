////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2018 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_GRAPHICS_MONOCHROME_h
#define _MITOV_GRAPHICS_MONOCHROME_h

#include <Mitov.h>
#include <Mitov_Graphics.h>

namespace Mitov
{
//---------------------------------------------------------------------------
	template<typename T_OWNER, T_OWNER &C_OWNER> class GraphicsMonochromeElementFillScreen
	{
	public:
		TArduinoMonochromeColor	Color = tmcBlack;

	public:
		inline void Render()
		{
			T_OWNER::ClearScreen( C_OWNER, Color );
		}

	public:
		inline void ClockInputPin_o_Receive( void *_Data )
		{
			Render();
		}

	};
//---------------------------------------------------------------------------
	template<typename T_OWNER, T_OWNER &C_OWNER> class GraphicsMonochromeElementDrawRectangle
	{
	public:
		TArduinoMonochromeColor	Color : 2;
		TArduinoMonochromeColor	FillColor : 2;
		TGraphicsPos			X = 0;
		TGraphicsPos			Y = 0;
		uint32_t				Width = 40;
		uint32_t				Height = 20;

	public:
		void Render()
		{
			if( FillColor == tmcNone )
				if( Color == tmcNone )
					return;

			TGraphicsPos AParentX, AParentY;
			C_OWNER.GetPosition( AParentX, AParentY );

			if( FillColor != tmcNone )
				T_OWNER::fillRect( C_OWNER, AParentX + X + 1, AParentY + Y + 1, Width - 2, Height - 2, FillColor );

			if( Color != tmcNone )
				T_OWNER::drawRect( C_OWNER, AParentX + X, AParentY + Y, Width, Height, Color );
		}

	public:
		inline void ClockInputPin_o_Receive( void *_Data )
		{
			Render();
		}

	public:
		GraphicsMonochromeElementDrawRectangle() :
			Color( tmcWhite ),
			FillColor( tmcNone )
		{
		}

	};
//---------------------------------------------------------------------------
	template<typename T_OWNER, T_OWNER &C_OWNER> class GraphicsMonochromeElementDrawRoundRectangle
	{
	public:
		TArduinoMonochromeColor		Color : 2;
		TArduinoMonochromeColor		FillColor : 2;
		TGraphicsPos				X = 0;
		TGraphicsPos				Y = 0;
		uint32_t					Width = 40;
		uint32_t					Height = 20;
		TArduinoGraphicsCornerSize	CornerSize;

	public:
		void Render()
		{
			if( FillColor == tmcNone )
				if( Color == tmcNone )
					return;

			TGraphicsPos AParentX, AParentY;
			C_OWNER.GetPosition( AParentX, AParentY );

			if( FillColor != tmcNone )
				T_OWNER::fillRoundRect( C_OWNER, AParentX + X, AParentY + Y, Width, Height, CornerSize.X, CornerSize.Y, FillColor );

			if( Color != tmcNone )
				T_OWNER::drawRoundRect( C_OWNER, AParentX + X, AParentY + Y, Width, Height, CornerSize.X, CornerSize.Y, Color );
		}

	public:
		inline void ClockInputPin_o_Receive( void *_Data )
		{
			Render( true );
		}

	public:
		GraphicsMonochromeElementDrawRoundRectangle() :
			Color( tmcWhite ),
			FillColor( tmcNone )
		{
		}

	};
//---------------------------------------------------------------------------
	template<typename T_OWNER, T_OWNER &C_OWNER> class GraphicsMonochromeElementDrawEllipse
	{
	public:
		TArduinoMonochromeColor	Color : 2;
		TArduinoMonochromeColor	FillColor : 2;
		bool					Centered	: 1;
		TGraphicsPos			X = 0;
		TGraphicsPos			Y = 0;
		uint32_t				Width = 40;
		uint32_t				Height = 20;

	public:
		void Render()
		{
			if( FillColor == tmcNone )
				if( Color == tmcNone )
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

			if( FillColor != tmcNone )
				T_OWNER::fillEllipseWH( C_OWNER, AParentX + AX, AParentY + AY, Width, Height, FillColor );

			if( Color != tmcNone )
				T_OWNER::drawEllipseWH( C_OWNER, AParentX + AX, AParentY + AY, Width, Height, Color );
		}

	public:
		inline void ClockInputPin_o_Receive( void *_Data )
		{
			Render();
		}

	public:
		GraphicsMonochromeElementDrawEllipse() :
			Color( tmcWhite ),
			FillColor( tmcNone ),
			Centered( false )
		{
		}

	};
//---------------------------------------------------------------------------
	template<typename T_OWNER, T_OWNER &C_OWNER> class GraphicsMonochromeElementDrawPixel
	{
	public:
		TArduinoMonochromeColor	Color = tmcWhite;
		TGraphicsPos			X = 0;
		TGraphicsPos			Y = 0;

	public:
		void Render()
		{
			if( Color == tmcNone )
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

	};
//---------------------------------------------------------------------------
	template<typename T_OWNER, T_OWNER &C_OWNER> class GraphicsMonochromeElementDrawLine
	{
	public:
		TArduinoMonochromeColor	Color = tmcWhite;
		TGraphicsPos			X = 0;
		TGraphicsPos			Y = 0;
		int32_t					Width = 40;
		int32_t					Height = 20;

	public:
		void Render()
		{
			if( Color == tmcNone )
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

	};
//---------------------------------------------------------------------------
//#ifdef _GRAPHICS_DRAW_BLACK_WHITE_BITMAP_

	template<typename T_OWNER, T_OWNER &C_OWNER> class GraphicsMonochromeElementDrawBitmap
	{
    public:
		uint8_t	Size : 4;
		TArduinoMonochromeColor	Color : 2;
		TArduinoMonochromeColor	FillColor : 2;

    public:
		TGraphicsPos	X = 0;
		TGraphicsPos	Y = 0;
		uint8_t			Width;
		uint8_t			Height;

		const unsigned char *_Bytes;

	public:
		void Render()
		{
			if(( FillColor == tmcNone ) && ( Color == tmcNone ))
				return;

			TGraphicsPos AParentX, AParentY;
			C_OWNER.GetPosition( AParentX, AParentY );

			T_OWNER::drawBWBitmap( C_OWNER, AParentX + X, AParentY + Y, _Bytes, Width, Height, Color, FillColor, Size );
		}

	public:
		inline void ClockInputPin_o_Receive( void *_Data )
		{
			Render();
		}

	public:
		GraphicsMonochromeElementDrawBitmap() :
			Size( 1 ),
			Color( tmcWhite ),
			FillColor( tmcNone )
		{
		}

	};

//#endif // _GRAPHICS_DRAW_BLACK_WHITE_BITMAP_
//---------------------------------------------------------------------------
	template<typename T_OWNER, T_OWNER &C_OWNER, typename T_CARD, T_CARD &C_CARD> class GraphicsMonochromeElementDrawGraphics : public GraphicsDelegate<GraphicsMonochromeElementDrawGraphics<T_OWNER, C_OWNER, T_CARD, C_CARD>, T_OWNER, C_OWNER>
	{
    public:
		TGraphicsPos	X = 0;
		TGraphicsPos	Y = 0;
		float			Threshold = 0.5;

	public:
		void Render()
		{
			TGraphicsPos AParentX, AParentY;
			C_OWNER.GetPosition( AParentX, AParentY );

			C_CARD.template RenderGraphics<GraphicsMonochromeElementDrawGraphics<T_OWNER, C_OWNER, T_CARD, C_CARD>>( this, AParentX + X, AParentY + Y );

//			FOwner.GetGraphics()->drawRepeatColorBitmap( AParentX + X, AParentY + Y, _Data, Count, Width, Height, Size );
		}

	public:
		inline TUniColor MakeColorColor( uint8_t r, uint8_t g, uint8_t b )
		{
			float AValue = 0.21 * ( float( r ) / 255 ) + 0.72 * ( float( g ) / 255 ) + 0.07 * ( float( b ) / 255 );
//					Serial.println( AValue );
				
			//pushColor(tft.Color565(r,g,b));
			if( AValue > Threshold )
				return tmcWhite;

			return tmcBlack;
		}


	public:
		inline void ClockInputPin_o_Receive( void *_Data )
		{
			Render();
		}

	};
//---------------------------------------------------------------------------	
	template<typename T_OWNER, T_OWNER &C_OWNER> class GraphicsElementDrawTransparencyBitmap
	{
    public:
		uint8_t	Size = 1;

    public:
		TGraphicsPos	X = 0;
		TGraphicsPos	Y = 0;
		uint8_t			Width;
		uint8_t			Height;

		const unsigned char *_Bytes;

	public:
		void Render()
		{
			TGraphicsPos AParentX, AParentY;
			C_OWNER.GetPosition( AParentX, AParentY );

			T_OWNER::drawTransparencyBitmap( C_OWNER, AParentX + X, AParentY + Y, _Bytes, Width, Height, Size );
		}

	public:
		inline void ClockInputPin_o_Receive( void *_Data )
		{
			Render();
		}

	};
//---------------------------------------------------------------------------
	template<typename T_OWNER, T_OWNER &C_OWNER> class GraphicsMonochromeElementDrawText : public GraphicsTextIntf, public GraphicsDelegate<GraphicsMonochromeElementDrawText<T_OWNER, C_OWNER>, T_OWNER, C_OWNER>
	{
	public:
		TArduinoMonochromeColor	Color : 2;
		TArduinoMonochromeColor	FillColor : 2;
		uint8_t					Size : 4;
		bool					Wrap : 1;
		TGraphicsPos			X = 0;
		TGraphicsPos			Y = 0;

		Mitov::String			Text;

	public:
		void Render()
		{
			if(( FillColor == tmcNone ) && ( Color == tmcNone ))
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
		GraphicsMonochromeElementDrawText() :
			Color( tmcWhite ),
			FillColor( tmcNone ),
			Size( 1 ),
			Wrap( true )
		{
		}

	};
//---------------------------------------------------------------------------
	template<typename T_OWNER, T_OWNER &C_OWNER> class GraphicsMonochromeElementTextField : public GraphicsTextIntf, public GraphicsDelegate<GraphicsMonochromeElementTextField<T_OWNER, C_OWNER>, T_OWNER, C_OWNER>
	{
	public:
		bool	ClockInputPin_o_IsConnected	: 1;

	public:
		TArduinoMonochromeColor	Color : 2;
		TArduinoMonochromeColor	FillColor : 2;
		TArduinoTextHorizontalAlign	HorizontalAlign : 2;
		bool AutoSize : 1;
		bool Wrap : 1;
		uint8_t	Size : 4;

		TGraphicsPos	X = 0;
		TGraphicsPos	Y = 0;
		uint16_t Width = 6;

		Mitov::String	InitialValue;

	protected:
		uint8_t	FOldSize = 0;

	public:
		void Render()
		{
			if(( FillColor == tmcNone ) && ( Color == tmcNone ))
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
		void InputPin_o_Receive( void *_Data )
		{
			InitialValue = (char *)_Data;
			if( ! ClockInputPin_o_IsConnected )
				Render();
		}

		inline void ClockInputPin_o_Receive( void *_Data )
		{
			Render();
		}

	public:
		GraphicsMonochromeElementTextField() :
			ClockInputPin_o_IsConnected( false ),
			Color( tmcWhite ),
			FillColor( tmcBlack ),
			HorizontalAlign( thaLeft ),
			AutoSize( true ),
			Wrap( true ),
			Size( 1 )
		{
		}

	};
//---------------------------------------------------------------------------
	template<typename T_OWNER, T_OWNER &C_OWNER> class GraphicsMonochromeElementDrawLines
	{
	public:
		TArduinoMonochromeColor	Color = tmcWhite;
		TGraphicsPos			X = 0;
		TGraphicsPos			Y = 0;

	public:
		GraphicsPoint	*Points;
		uint32_t		_PointsCount = 0;

	public:
		void Render()
		{
			if( _PointsCount == 0 )
				return;

			if( Color == tmcNone )
				return;

			TGraphicsPos AParentX, AParentY;
			C_OWNER.GetPosition( AParentX, AParentY );

			int32_t	AX1 = X;
			int32_t	AY1 = Y;

			for( uint32_t i = 0; i < _PointsCount; ++i )
			{
				int32_t	AX2 = Points[ i ].X + X;
				int32_t	AY2 = Points[ i ].Y + Y;

				T_OWNER::drawLine( C_OWNER, AParentX + AX1, AParentY + AY1, AParentX + AX2, AParentY + AY2, Color );

				AX1 = AX2;
				AY1 = AY2;
			}
		}

	public:
		inline void ClockInputPin_o_Receive( void *_Data )
		{
			Render( true );
		}

	};
//---------------------------------------------------------------------------
	template<typename T_OWNER, T_OWNER &C_OWNER> class GraphicsMonochromeElementDrawPolygon
	{
	public:
		TArduinoMonochromeColor	Color : 2;
		TArduinoMonochromeColor	FillColor : 2;
		TGraphicsPos			X = 0;
		TGraphicsPos			Y = 0;

	public:
		GraphicsPoint	*Points;
		uint32_t		_PointsCount = 0;

	public:
		void Render()
		{
			if( _PointsCount == 0 )
				return;

			if(( FillColor == tmcNone ) && ( Color == tmcNone ))
				return;

			TGraphicsPos AParentX, AParentY;
			C_OWNER.GetPosition( AParentX, AParentY );

			int32_t	AStartX = AParentX + X;
			int32_t	AStartY = AParentY + Y;

			if( FillColor != tmcNone )
			{
				int32_t	AMinX = AStartX;
				int32_t	AMinY = AStartY;
				int32_t	AMaxX = AStartX;
				int32_t	AMaxY = AStartY;

				for( uint32_t i = 0; i < _PointsCount; ++i )
				{
					int32_t	AX2 = Points[ i ].X + AStartX;
					int32_t	AY2 = Points[ i ].Y + AStartY;

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

				int32_t	AWidth = C_OWNER.width();
				int32_t	AHeight = C_OWNER.height();

				if( AMaxX > AWidth )
				 AMaxX = AWidth;

				if( AMaxY > AHeight )
				 AMaxY = AHeight;

				if( AMinX > AMaxX )
					return;

				if( AMinY > AMaxY )
					return;

				int32_t *nodeX = new int32_t[ _PointsCount ];
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
							int32_t swap = nodeX[ i ];
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
						int32_t ALeft = nodeX[ i ];
						int32_t ARight = nodeX[ i + 1 ];

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

			if( Color != tmcNone )
			{
				int32_t	AX1 = X;
				int32_t	AY1 = Y;

				for( uint32_t i = 1; i < _PointsCount; ++i )
				{
					int32_t	AX2 = Points[ i ].X + X;
					int32_t	AY2 = Points[ i ].Y + Y;

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
		GraphicsMonochromeElementDrawPolygon() :
			Color( tmcWhite ),
			FillColor( tmcNone )
		{
		}

	};
//---------------------------------------------------------------------------
	template<typename T_OWNER, T_OWNER &C_OWNER> class GraphicsMonochromeElementCheckPixel
	{
	public:
		OpenWire::SourcePin	OutputPin;

	public:
		uint32_t X = 0;
		uint32_t Y = 0;

	public:
		void Render()
		{
			TUniColor AValue = C_OWNER.GetPixelColor( X, Y );
			OutputPin.SendValue<bool>( AValue );
		}

	public:
		inline void ClockInputPin_o_Receive( void *_Data )
		{
			Render();
		}

	};
//---------------------------------------------------------------------------
	template<typename T_OWNER, T_OWNER &C_OWNER> class GraphicsMonochromeElementScroll
	{
	public:
		TArduinoMonochromeColor	Color = tmcBlack;
		int32_t					X = 0;
		int32_t					Y = 0;

	public:
		void Render()
		{
			T_OWNER::Scroll( C_OWNER, X, Y, Color );
		}

	public:
		inline void ClockInputPin_o_Receive( void *_Data )
		{
			Render();
		}

	};
//---------------------------------------------------------------------------
	class TGraphicsMonochromeTextSettings
	{
	public:
		uint8_t					Size : 4;
		bool					Wrap : 1;
		TArduinoMonochromeColor	Color : 2;
		TArduinoMonochromeColor	BackgroundColor : 2;

	public:
		TGraphicsMonochromeTextSettings() :
			Size( 1 ),
			Wrap( true ),
			Color( tmcWhite ),
			BackgroundColor( tmcNone )
		{
		}
	};
//---------------------------------------------------------------------------
}

#endif
