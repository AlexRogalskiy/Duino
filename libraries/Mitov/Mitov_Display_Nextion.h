////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2018 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_DISPLAY_NEXTION_h
#define _MITOV_DISPLAY_NEXTION_h

#include <Mitov.h>
#include <Mitov_Graphics.h>

#ifdef __TEST_FOR_DEBUG_PRINTS__
#define Serial UNGUARDED DEBUG PRINT!!!
#endif

// #define __NEXTION__DEBUG__

namespace Mitov
{
#define Min Min
#define Max Max

	class ArduinoDisplayNextionElementBasic;
//---------------------------------------------------------------------------
	struct TDisplayNextionResponseElementBasic
	{
	public:
		virtual bool ProcessResponse( uint8_t *ABuffer ) { return false; }
		virtual bool ProcessTextResponse( Mitov::String AText ) { return false; }

	public:
		virtual ~TDisplayNextionResponseElementBasic() {}
	};
//---------------------------------------------------------------------------
/*
	struct TDisplayNextionResponseElementNull : public TDisplayNextionResponseElementBasic
	{
	public:
		virtual bool ProcessResponse( uint8_t *ABuffer ) override { return true; }
		virtual bool ProcessTextResponse( Mitov::String AText ) override { return true; }

	};
*/
//---------------------------------------------------------------------------
	class DisplayNextionEventsIntf
	{
	public:
		virtual	void ResetEvent() {}
		virtual	bool TryProcessEvent( uint8_t *ABuffer ) = 0;

	public:
		virtual ~DisplayNextionEventsIntf() {}

	};
//---------------------------------------------------------------------------
	class DisplayNextionIntf
	{
	public:
		virtual void GetPosition( TGraphicsPos &AX, TGraphicsPos &AY ) { AX = 0; AY = 0; }
		virtual void SendCommand( const char *ACommand ) = 0;
		virtual void RegisterRender( ArduinoDisplayNextionElementBasic *AItem ) = 0;
		virtual	void ActiveatePage( ArduinoDisplayNextionElementBasic *APage ) = 0;
		virtual	bool IsPageActive( ArduinoDisplayNextionElementBasic *APage ) = 0;
		virtual	void AddRequest( Mitov::String ARequest, int8_t APage, TDisplayNextionResponseElementBasic *AResponse ) = 0;
		virtual	void AddRequestFront( Mitov::String ARequest, int8_t APage, TDisplayNextionResponseElementBasic *AResponse ) = 0;
		virtual	void RegisterEvents( DisplayNextionEventsIntf *AEvents ) = 0;
		virtual	uint8_t GetActivePageNumber() = 0;
		virtual	void RetryRequests() = 0;
		virtual	Mitov::String GetPageNamePath() { return ""; }

	};
//---------------------------------------------------------------------------
	class ArduinoDisplayNextionElementBasic
	{
	public:
		DisplayNextionIntf	&FOwner;

	protected:
		struct TResponseSendMe : public TDisplayNextionResponseElementBasic
		{
		public:
			ArduinoDisplayNextionElementBasic *FOwner;

		public:
			virtual bool ProcessResponse( uint8_t *ABuffer ) override
			{
#ifdef __NEXTION__DEBUG__
				Serial.println( "Page Number TEST" );
#endif //__NEXTION__DEBUG__

				if( ABuffer[ 0 ] != 0x66 )
					return false;

#ifdef __NEXTION__DEBUG__
				Serial.print( "Page Number: " ); Serial.println( ABuffer[ 1 ] );
				Serial.println( FOwner->ElementName );
#endif //__NEXTION__DEBUG__
				FOwner->PageNumber = ABuffer[ 1 ];
				FOwner->PageIdentified = true;
				if( FOwner->FHasData )
					FOwner->Render();

				return true;
			}

		public:
			TResponseSendMe( ArduinoDisplayNextionElementBasic *AOwner ) :
				FOwner( AOwner )
			{
			}
		};

		struct TResponseElementGetID : public TDisplayNextionResponseElementBasic
		{
		public:
			ArduinoDisplayNextionElementBasic *FOwner;
			uint8_t	FExpectedCode;

		public:
			virtual bool ProcessResponse( uint8_t *ABuffer ) override
			{
#ifdef __NEXTION__DEBUG__
				Serial.println( "ID TEST" );
#endif //__NEXTION__DEBUG__
				if( ABuffer[ 0 ] != FExpectedCode )
					return false;

				if( FOwner->PageIdentified )
					if( FOwner->PageNumber != FOwner->FOwner.GetActivePageNumber() )
						return false;

#ifdef __NEXTION__DEBUG__
				Serial.print( "ID: " ); Serial.println( ABuffer[ 1 ] );
				Serial.println( FOwner->ElementName );
#endif //__NEXTION__DEBUG__
				FOwner->ID = ABuffer[ 1 ];
				FOwner->Identified = true;
				if( ! FOwner->PageIdentified )
					FOwner->FOwner.AddRequestFront( "sendme", FOwner->PageNumber, new TResponseSendMe( FOwner ));

				return true;
			}

		public:
			TResponseElementGetID( ArduinoDisplayNextionElementBasic *AOwner, uint8_t AExpectedCode ) :
				FOwner( AOwner ),
				FExpectedCode( AExpectedCode )
			{
			}
		};

	public:
		uint8_t			ID;
		uint8_t			PageNumber;
		Mitov::String	ElementName;
		bool			Identified : 1;
		bool			PageIdentified : 1;

	public:
		bool			FHasData : 1;

	protected:
		bool			FUpdatedOnce : 1;

//	public:
//		virtual bool RequestInfo() { return false; }

	public:
		virtual void Render() {}
		virtual void SendRequest() {}

		Mitov::String GetElementName() 
		{ 
			return FOwner.GetPageNamePath() + ElementName;
		}

		void ActivatePageNo( uint8_t APageNo ) 
		{
			if( PageNumber == APageNo )
				Render();
		}

	protected:
		void RequestElementID( Mitov::String AName )
		{
#ifdef __NEXTION__DEBUG__
			Serial.print( "RequestElementID: " ); Serial.println( AName );
#endif //__NEXTION__DEBUG__

			FOwner.AddRequest( Mitov::String( "get " ) + AName + ".id", PageNumber, new TResponseElementGetID( this, 0x71 ));
		}

	public:
		inline void ClockInputPin_o_Receive( void *_Data )
		{
			Render();
		}

	public:
		ArduinoDisplayNextionElementBasic( DisplayNextionIntf &AOwner, Mitov::String AElementName, int APageID, int AElementID ) :
			FOwner( AOwner ),
			ID( AElementID ),
			PageNumber( APageID ),
			ElementName( AElementName ),
			Identified( AElementID >= 0 ),
			PageIdentified( APageID >= 0 ),
			FUpdatedOnce( false ),
			FHasData( false )
		{
			FOwner.RegisterRender( this );
		}
	};
//---------------------------------------------------------------------------
	class DisplayNextionElementFillScreen : public ArduinoDisplayNextionElementBasic
	{
		typedef ArduinoDisplayNextionElementBasic inherited;

	public:
		TColor Color;

	public:
		virtual void Render() override
		{
			FOwner.SendCommand( ( Mitov::String( "cls " ) + Color.To565Color() ).c_str() );
		}

	public:
		DisplayNextionElementFillScreen( DisplayNextionIntf &AOwner ) :
			inherited( AOwner, "", -1, -1 )
		{
		}

	};
//---------------------------------------------------------------------------
	class DisplayNextionElementFillRectangle : public ArduinoDisplayNextionElementBasic
	{
		typedef ArduinoDisplayNextionElementBasic inherited;

	public:
		TColor Color = TColor( 255, 0, 0 );
		TGraphicsPos	X = 0;
		TGraphicsPos	Y = 0;
		TGraphicsSize	Width = 100;
		TGraphicsSize	Height = 100;

	public:
		virtual void Render() override
		{
			TGraphicsPos AParentX, AParentY;
			FOwner.GetPosition( AParentX, AParentY );
			FOwner.SendCommand( ( Mitov::String( "fill " ) + ( AParentX + X ) + "," + ( AParentY + Y ) + "," + Width + "," + Height + "," + Color.To565Color() ).c_str() );
		}

	public:
		DisplayNextionElementFillRectangle( DisplayNextionIntf &AOwner ) :
			inherited( AOwner, "", -1, -1 )
		{
		}

	};
//---------------------------------------------------------------------------
	class DisplayNextionElementDrawRectangle : public ArduinoDisplayNextionElementBasic
	{
		typedef ArduinoDisplayNextionElementBasic inherited;

	public:
		TColor Color = TColor( 255, 0, 0 );
		TGraphicsPos	X = 0;
		TGraphicsPos	Y = 0;
		TGraphicsSize	Width = 100;
		TGraphicsSize	Height = 100;

	public:
		virtual void Render() override
		{
			TGraphicsPos AParentX, AParentY;
			FOwner.GetPosition( AParentX, AParentY );
			FOwner.SendCommand( ( Mitov::String( "draw " ) + ( AParentX + X ) + "," + ( AParentY + Y ) + "," + ( AParentX + X + Width ) + "," + ( AParentY + Y + Height ) + "," + Color.To565Color() ).c_str() );
		}

	public:
		DisplayNextionElementDrawRectangle( DisplayNextionIntf &AOwner ) :
			inherited( AOwner, "", -1, -1 )
		{
		}

	};
//---------------------------------------------------------------------------	
	class DisplayNextionElementDrawPicture : public ArduinoDisplayNextionElementBasic
	{
		typedef ArduinoDisplayNextionElementBasic inherited;

	public:
		TGraphicsPos	X = 0;
		TGraphicsPos	Y = 0;
		TGraphicsSize	Width = 100;
		TGraphicsSize	Height = 100;
		bool			Crop = true;
		uint8_t			PictureIndex = 0;

	public:
		virtual void Render() override
		{
			TGraphicsPos AParentX, AParentY;
			FOwner.GetPosition( AParentX, AParentY );
			if( Crop )
				FOwner.SendCommand( ( Mitov::String( "picq " ) + ( AParentX + X ) + "," + ( AParentY + Y ) + "," + Width + "," + Height + "," + PictureIndex ).c_str() );

			else
				FOwner.SendCommand( ( Mitov::String( "pic " ) + ( AParentX + X ) + "," + ( AParentY + Y ) + "," + PictureIndex ).c_str() );

		}

	public:
		DisplayNextionElementDrawPicture( DisplayNextionIntf &AOwner ) :
			inherited( AOwner, "", -1, -1 )
		{
		}

	};
//---------------------------------------------------------------------------
	class DisplayNextionElementActivatePage : public ArduinoDisplayNextionElementBasic, public DisplayNextionIntf
	{
		typedef ArduinoDisplayNextionElementBasic inherited;

	public:
		virtual	void ActiveatePage( ArduinoDisplayNextionElementBasic *APage ) override
		{
			FOwner.ActiveatePage( APage );
		}

		virtual	bool IsPageActive( ArduinoDisplayNextionElementBasic *APage ) override
		{
			return FOwner.IsPageActive( APage );
		}

		virtual	void AddRequest( Mitov::String ARequest, int8_t APage, TDisplayNextionResponseElementBasic *AResponse ) override
		{
			FOwner.AddRequest( ARequest, APage, AResponse );
		}

		virtual	void AddRequestFront( Mitov::String ARequest, int8_t APage, TDisplayNextionResponseElementBasic *AResponse ) override
		{
			FOwner.AddRequestFront( ARequest, APage, AResponse );
		}

		virtual	void RegisterEvents( DisplayNextionEventsIntf *AEvents ) override
		{
			FOwner.RegisterEvents( AEvents );
		}

		virtual void SendCommand( const char *ACommand ) override
		{
			if( FOwner.IsPageActive( this ))
				FOwner.SendCommand( ACommand );

		}

		virtual	uint8_t GetActivePageNumber() override
		{
			return FOwner.GetActivePageNumber();
		}

//		virtual	uint8_t GetPageNumber() override
//		{
//			return ID;
//		}

		virtual	void RetryRequests() override
		{
			FOwner.RetryRequests();
		}

		virtual	Mitov::String GetPageNamePath() override
		{
			return ElementName + ".";
		}

		virtual void RegisterRender( ArduinoDisplayNextionElementBasic *AItem ) override
		{
			FOwner.RegisterRender( AItem );
		}

/*
		virtual void SendRequest() override
		{
			if( ! Identified )
				FOwner.AddRequest( "sendme", new TResponseElementGetID( this ));

			inherited::SendRequest();
		}
*/
	public:
		virtual void Render() override
		{
			FOwner.ActiveatePage( this );
//			TGraphicsPos AParentX, AParentY;
//			FOwner.GetPosition( AParentX, AParentY );
			FOwner.SendCommand( ( Mitov::String( "page " ) + ElementName ).c_str() );
			if( ! Identified )
			{
				PageIdentified = true;
				FOwner.AddRequest( "sendme", -1, new TResponseElementGetID( this, 0x66 ));
			}

//			FOwner.SendCommand( "sendme"
			FOwner.RetryRequests();
		}

/*
		virtual void SendRequest() override
		{
			if( ! Identified )
				sendme
//				RequestElementID( ElementName );

			inherited::SendRequest();
		}
*/
	public:
		DisplayNextionElementActivatePage( DisplayNextionIntf &AOwner, Mitov::String AElementName ) :
			inherited( AOwner, AElementName, -1, -1 )
		{
		}

	};
//---------------------------------------------------------------------------
	class DisplayNextionAnalogGaugeValue
	{
	public:
		float	Value;
		int32_t	Angle;

	public:
		DisplayNextionAnalogGaugeValue( float AValue, int32_t AAngle ) :
			Value( AValue ),
			Angle( AAngle )
		{
		}
	};
//---------------------------------------------------------------------------
	class DisplayNextionElementAnalogGauge : public ArduinoDisplayNextionElementBasic
	{
		typedef ArduinoDisplayNextionElementBasic inherited;

	public:
		DisplayNextionAnalogGaugeValue	Min;
		DisplayNextionAnalogGaugeValue	Max;

	protected:
		float	FValue	= 0.0f;

//	public:
//		virtual bool RequestInfo() override;

	public:
		virtual void Render() override
		{
//			Serial.println( "STEP1" );
			if( !Identified )
				return;

//			Serial.println( "STEP2" );
			if( !PageIdentified )
				return;

//			Serial.println( "STEP3" );
			if( PageNumber != FOwner.GetActivePageNumber() )
				return;

			int16_t AAngle = Func::MapRange( constrain( FValue, Min.Value, Max.Value ), Min.Value, Max.Value, float( Min.Angle ), float( Max.Angle ) ) + 0.5;
			if( AAngle < 0 )
				AAngle += 360;

//			Serial.println( AAngle );
			FOwner.SendCommand( ( GetElementName() + ".val=" + AAngle ).c_str() );
			FUpdatedOnce = true;
		}

	public:
		virtual void SendRequest() override
		{
//			Serial.println( "TEST5" );
//			Serial.println( ElementName );
//			Serial.println( GetElementName() );
			if( ! Identified )
				RequestElementID( GetElementName() );

			inherited::SendRequest();
		}

	public:
		void InputPin_o_Receive( void * _Data )
		{
			if( FUpdatedOnce )
				if( FValue == *(float*)_Data )
					return;

			FValue = *(float*)_Data;
			FHasData = true;
			Render();
		}

	public:
		DisplayNextionElementAnalogGauge( DisplayNextionIntf &AOwner, Mitov::String AElementName, int APageID, int AElementID ) :
			inherited( AOwner, AElementName, APageID, AElementID ),
			Min( 0.0, 0 ),
			Max( 1.0, 180 )
		{
		}

	};
//---------------------------------------------------------------------------
	class DisplayNextionElementAnalogProgressBar : public ArduinoDisplayNextionElementBasic
	{
		typedef ArduinoDisplayNextionElementBasic inherited;

	public:
		float	Min = 0.0f;
		float	Max = 1.0f;

	protected:
		float	FValue = 0.0f;

	public:
		virtual void Render() override
		{
			if( !Identified )
				return;

			if( !PageIdentified )
				return;

			if( PageNumber != FOwner.GetActivePageNumber() )
				return;

			int16_t AAngle = Func::MapRange( constrain( FValue, Min, Max ), Min, Max, 0.0f, 100.0f ) + 0.5;
			FOwner.SendCommand( ( GetElementName() + ".val=" + AAngle ).c_str() );
			FUpdatedOnce = true;
		}

	public:
		virtual void SendRequest() override
		{
			if( ! Identified )
				RequestElementID( GetElementName() );

			inherited::SendRequest();
		}

	public:
		void InputPin_o_Receive( void * _Data )
		{
			if( FUpdatedOnce )
				if( FValue == *(float*)_Data )
					return;

			FValue = *(float*)_Data;
			FHasData = true;
			Render();
		}

	public:
		using inherited::inherited;

	};
//---------------------------------------------------------------------------
	class DisplayNextionElementText : public ArduinoDisplayNextionElementBasic
	{
		typedef ArduinoDisplayNextionElementBasic inherited;

	protected:
		Mitov::String	FValue;

	public:
		virtual void Render() override
		{
			if( !Identified )
				return;

//			Serial.println( "STEP2" );
			if( !PageIdentified )
				return;

//			Serial.println( "STEP3" );
			if( PageNumber != FOwner.GetActivePageNumber() )
				return;

//			Serial.println( "RENDER"  );
			FOwner.SendCommand( ( GetElementName() + ".txt=\"" + FValue + "\"" ).c_str() );
			FUpdatedOnce = true;
		}

	public:
		virtual void SendRequest() override
		{
			if( ! Identified )
				RequestElementID( GetElementName() );

			inherited::SendRequest();
		}

	public:
		void InputPin_o_Receive( void * _Data )
		{
			Mitov::String AValue = (char *)_Data;
			if( FUpdatedOnce )
				if( FValue == AValue )
					return;

			FValue = AValue;
			FHasData = true;
			Render();
		}

	public:
		using inherited::inherited;

	};
//---------------------------------------------------------------------------
	class DisplayNextionElementNumber : public ArduinoDisplayNextionElementBasic
	{
		typedef ArduinoDisplayNextionElementBasic inherited;

	protected:
		uint32_t	FValue = 0;

	public:
		virtual void Render() override
		{
			if( ! Identified )
				return;

			if( ! PageIdentified )
				return;

			if( PageNumber != FOwner.GetActivePageNumber() )
				return;

			FOwner.SendCommand( ( GetElementName() + ".val=" + FValue ).c_str() );
			FUpdatedOnce = true;
		}

	public:
		virtual void SendRequest() override
		{
			if( ! Identified )
				RequestElementID( GetElementName() );

			inherited::SendRequest();
		}

	public:
		void InputPin_o_Receive( void * _Data )
		{
			if( FUpdatedOnce )
				if( FValue == *(uint32_t*)_Data )
					return;

			FValue = *(uint32_t*)_Data;
			FHasData = true;
			Render();
		}

	public:
		using inherited::inherited;

	};
//---------------------------------------------------------------------------
	class DisplayNextionElementIntegerVariable : public ArduinoDisplayNextionElementBasic
	{
		typedef ArduinoDisplayNextionElementBasic inherited;

	protected:
		int32_t	FValue = 0;

	public:
		virtual void Render() override
		{
			if( ! Identified )
				return;

			if( ! PageIdentified )
				return;

			if( PageNumber != FOwner.GetActivePageNumber() )
				return;

			FOwner.SendCommand( ( GetElementName() + ".val=" + FValue ).c_str() );
			FUpdatedOnce = true;
		}

	public:
		virtual void SendRequest() override
		{
			if( ! Identified )
				RequestElementID( GetElementName() );

			inherited::SendRequest();
		}

	public:
		void InputPin_o_Receive( void * _Data )
		{
			if( FUpdatedOnce )
				if( FValue == *(int32_t*)_Data )
					return;

			FValue = *(int32_t*)_Data;
			FHasData = true;
			Render();
		}

	public:
		using inherited::inherited;

	};
//---------------------------------------------------------------------------
	class DisplayNextionElementTimer : public ArduinoDisplayNextionElementBasic
	{
		typedef ArduinoDisplayNextionElementBasic inherited;

	protected:
		bool	FValue = true;

	public:
		virtual void Render() override
		{
			if( ! Identified )
				return;

			if( ! PageIdentified )
				return;

			if( PageNumber != FOwner.GetActivePageNumber() )
				return;

			FOwner.SendCommand( ( GetElementName() + ".en=" + (( FValue ) ? "1" : "0" ) ).c_str() );
		}

	public:
		virtual void SendRequest() override
		{
			if( ! Identified )
				RequestElementID( GetElementName() );

			inherited::SendRequest();
		}

	public:
		void EnabledInputPin_o_Receive( void * _Data )
		{
//			if( FValue == *(bool*)_Data )
//				return;

			FValue = *(bool*)_Data;
			Render();
		}

	public:
		using inherited::inherited;

	};
//---------------------------------------------------------------------------
	class DisplayNextionElementFillCircle : public ArduinoDisplayNextionElementBasic
	{
		typedef ArduinoDisplayNextionElementBasic inherited;

	public:
		TColor Color = TColor( 255, 0, 0 );
		uint32_t	X = 50;
		uint32_t	Y = 50;
		uint32_t	Radius = 50;

	public:
		virtual void Render() override
		{
			TGraphicsPos AParentX, AParentY;
			FOwner.GetPosition( AParentX, AParentY );
			FOwner.SendCommand( ( Mitov::String( "cirs " ) + ( AParentX + X ) + "," + ( AParentY + Y ) + "," + Radius + "," + Color.To565Color() ).c_str() );
		}

	public:
		DisplayNextionElementFillCircle( DisplayNextionIntf &AOwner ) :
			inherited( AOwner, "", -1, -1 )
		{
		}

	};
//---------------------------------------------------------------------------
	class DisplayNextionElementDrawCircle : public ArduinoDisplayNextionElementBasic
	{
		typedef ArduinoDisplayNextionElementBasic inherited;

	public:
		TColor Color = TColor( 255, 0, 0 );
		uint32_t	X = 50;
		uint32_t	Y = 50;
		uint32_t	Radius = 50;

	public:
		virtual void Render() override
		{
			TGraphicsPos AParentX, AParentY;
			FOwner.GetPosition( AParentX, AParentY );
			FOwner.SendCommand( ( Mitov::String( "cir " ) + ( AParentX + X ) + "," + ( AParentY + Y ) + "," + Radius + "," + Color.To565Color() ).c_str() );
		}

	public:
		DisplayNextionElementDrawCircle( DisplayNextionIntf &AOwner ) :
			inherited( AOwner, "", -1, -1 )
		{
		}

	};
//---------------------------------------------------------------------------
	class DisplayNextionElementDrawLine : public ArduinoDisplayNextionElementBasic
	{
		typedef ArduinoDisplayNextionElementBasic inherited;

	public:
		TColor Color = TColor( 255, 0, 0 );
		TGraphicsPos	X = 0;
		TGraphicsPos	Y = 0;
		TGraphicsPos	Width = 100;
		TGraphicsPos	Height = 100;

	public:
		virtual void Render() override
		{
			TGraphicsPos AParentX, AParentY;
			FOwner.GetPosition( AParentX, AParentY );
			FOwner.SendCommand( ( Mitov::String( "line " ) + ( AParentX + X ) + "," + ( AParentY + Y ) + "," + ( AParentX + X + Width ) + "," + ( AParentY + Y + Height ) + "," + Color.To565Color() ).c_str() );
		}

	public:
		DisplayNextionElementDrawLine( DisplayNextionIntf &AOwner ) :
			inherited( AOwner, "", -1, -1 )
		{
		}

	};
//---------------------------------------------------------------------------
	enum TArduinoDisplayNextionTextBackgroundMode { bmClear, bmColor, bmPictureCrop, bmPicture };
//---------------------------------------------------------------------------
	class DisplayNextionTextBackground
	{
	public:
		TColor		Color;
		uint32_t	PictureIndex = 0;
		TArduinoDisplayNextionTextBackgroundMode Mode = bmClear;

	};
//---------------------------------------------------------------------------
	class DisplayNextionElementDrawText : public ArduinoDisplayNextionElementBasic
	{
		typedef ArduinoDisplayNextionElementBasic inherited;

	public:
		TColor Color = TColor( 255, 0, 0 );
		DisplayNextionTextBackground	Background;
		TGraphicsPos	X = 0;
		TGraphicsPos	Y = 0;
		TGraphicsSize	Width = 100;
		TGraphicsSize	Height = 20;
		uint8_t			FontIndex = 0;
		TArduinoTextHorizontalAlign	HorizontalAlign : 2;
		TArduinoTextVerticalAlign	VerticalAlign : 2;
		bool	ClockInputPin_IsConnected : 1;
		Mitov::String	InitialValue;

	public:
		template<typename T> void Print( T AValue )
		{
			 InitialValue = Mitov::String( AValue );
			 if( ! ClockInputPin_IsConnected )
				 Render();
		}

	public:
		virtual void Render() override
		{
			TGraphicsPos AParentX, AParentY;
			FOwner.GetPosition( AParentX, AParentY );
			Mitov::String ABackgroundColorText;
			Mitov::String ABackgroundTypeText;
			switch( Background.Mode )
			{
				case bmClear:
					ABackgroundColorText = "NULL";
					ABackgroundTypeText = "1";
					break;

				case bmColor:
					ABackgroundColorText = Mitov::String( Background.Color.To565Color());
					ABackgroundTypeText = "1";
					break;

				case bmPictureCrop:
					ABackgroundColorText = Mitov::String( Background.PictureIndex );
					ABackgroundTypeText = "0";
					break;

				case bmPicture:
					ABackgroundColorText = Mitov::String( Background.PictureIndex );
					ABackgroundTypeText = "2";
					break;

			}

			FOwner.SendCommand( ( Mitov::String( "xstr " ) + ( AParentX + X ) + "," + ( AParentY + Y ) + "," + Width + "," + Height + "," + FontIndex + "," + Color.To565Color() + "," + ABackgroundColorText + "," + uint16_t( HorizontalAlign ) + "," + uint16_t( VerticalAlign ) + "," + ABackgroundTypeText + ",\"" + InitialValue + "\"" ).c_str() );
//			FOwner.SendCommand( ( Mitov::String( "xstr " ) + ( AParentX + X ) + "," + ( AParentY + Y ) + "," + Width + "," + Height + "," + FontIndex + "," + Color.To565Color() + ",NULL," + uint16_t( HorizontalAlign ) + "," + uint16_t( VerticalAlign ) + ",1,\"" + InitialValue + "\"" ).c_str() );
		}

	public:
		DisplayNextionElementDrawText( DisplayNextionIntf &AOwner ) :
			inherited( AOwner, "", -1, -1 ),
			HorizontalAlign( thaCenter ),
			VerticalAlign( tvaCenter ),
			ClockInputPin_IsConnected( false )
		{
		}

	};
//---------------------------------------------------------------------------
	class DisplayNextionElementDrawScene : public ArduinoDisplayNextionElementBasic, public DisplayNextionIntf
	{
		typedef Mitov::ArduinoDisplayNextionElementBasic inherited;

	public:
		uint32_t	X = 0;
		uint32_t	Y = 0;

	protected:
		Mitov::SimpleList<ArduinoDisplayNextionElementBasic *>	FElements;

	public:
		virtual	void ActiveatePage( ArduinoDisplayNextionElementBasic *APage ) override
		{
			FOwner.ActiveatePage( APage );
		}

		virtual	bool IsPageActive( ArduinoDisplayNextionElementBasic *APage ) override
		{
			return FOwner.IsPageActive( APage );
		}

		virtual void SendCommand( const char *ACommand ) override
		{
			FOwner.SendCommand( ACommand );
		}

		virtual	uint8_t GetActivePageNumber() override
		{
			return FOwner.GetActivePageNumber();
		}

		virtual	void RetryRequests() override
		{
			FOwner.RetryRequests();
		}

		virtual	void AddRequest( Mitov::String ARequest, int8_t APage, TDisplayNextionResponseElementBasic *AResponse ) override
		{
			FOwner.AddRequest( ARequest, APage, AResponse );
		}

		virtual	void AddRequestFront( Mitov::String ARequest, int8_t APage, TDisplayNextionResponseElementBasic *AResponse ) override
		{
			FOwner.AddRequestFront( ARequest, APage, AResponse );
		}

		virtual	void RegisterEvents( DisplayNextionEventsIntf *AEvents ) override
		{
			FOwner.RegisterEvents( AEvents );
		}

	public:
		virtual void GetPosition( TGraphicsPos &AX, TGraphicsPos &AY )
		{ 
			TGraphicsPos AParentX, AParentY;
			FOwner.GetPosition( AParentX, AParentY );

			AX = AParentX + X;
			AY = AParentY + Y;
		}

		virtual void RegisterRender( ArduinoDisplayNextionElementBasic *AItem ) override
		{
			FElements.push_back( AItem );
			FOwner.RegisterRender( AItem );
		}

	public:
		virtual void Render() override
		{
			for( int i = 0; i < FElements.size(); ++ i )
				FElements[ i ]->Render();
		}

	public:
		DisplayNextionElementDrawScene( DisplayNextionIntf &AOwner ) :
			inherited( AOwner, "", -1, -1 )
		{
		}

	};
//---------------------------------------------------------------------------
	template<typename T_SERIAL, T_SERIAL &C_SERIAL> class DisplayNextion : public DisplayNextionIntf
	{
	protected:
		ArduinoDisplayNextionElementBasic *FActivePage = nullptr;

	protected:
		struct TRequestElement
		{
		public:
			Mitov::String						Request;
			TDisplayNextionResponseElementBasic *Response;
			int8_t								Page;

		public:
			TRequestElement( Mitov::String ARequest, int8_t APage, TDisplayNextionResponseElementBasic *AResponse ) :
				Request( ARequest ),
				Page( APage ),
				Response( AResponse )
			{
			}
		};

		struct TResponseReset : public TDisplayNextionResponseElementBasic
		{
		public:
			DisplayNextion *FOwner;

		public:
			virtual bool ProcessResponse( uint8_t *ABuffer ) override
			{
				if( ABuffer[ 0 ] != 0x88 )
					return false;

#ifdef __NEXTION__DEBUG__
							Serial.println( "RESET Confirmed" );
#endif // __NEXTION__DEBUG__

//				FOwner->Restarted();
				return true;
			}

		public:
			TResponseReset( DisplayNextion *AOwner ) :
				FOwner( AOwner )
			{
			}
		};

	public:
		Mitov::SimpleObjectList<DisplayNextionEventsIntf *>	FEventElements;
		Mitov::SimpleObjectDeleteList<TRequestElement *>	FRequestElements;
		TDisplayNextionResponseElementBasic *FLastResponse = nullptr;
//		Mitov::SimpleObjectDeleteList<TDisplayNextionResponseElementBasic *>	FResponseHandlersQueue;

	protected:
		Mitov::SimpleList<ArduinoDisplayNextionElementBasic *>	FElements;

	protected:
		uint8_t			FBuffer[ 10 ];
		uint8_t			FIndex = 0;
		uint8_t			FLength = 0;
		unsigned long	FLastTime = 0;
		Mitov::String	FCurrentRequest;
		Mitov::String	FResultText;
		bool			FStarted = false;
		uint8_t			FActivePageNumber = 0;

	public:
		virtual	void ActiveatePage( ArduinoDisplayNextionElementBasic *APage ) override
		{
			FActivePage = APage;
			if( FActivePage )
				FActivePageNumber = FActivePage->ID;
		}

		virtual	bool IsPageActive( ArduinoDisplayNextionElementBasic *APage ) override
		{
			if( ! FActivePage )
//				if( APage )
//					if( APage->GetElementName() == "page0" )
						return true;

			return ( APage == FActivePage );
		}

		virtual void SendCommand( const char *ACommand ) override
		{
#ifdef __NEXTION__DEBUG__
			Serial.print( "ACommand: " ); Serial.println( ACommand );
#endif // __NEXTION__DEBUG__

			C_SERIAL.GetStream().print( ACommand );
			C_SERIAL.GetStream().print( "\xFF\xFF\xFF" );
			FLastTime = millis();
		}

		virtual	uint8_t GetActivePageNumber() override
		{
			return FActivePageNumber;
		}

		virtual	void RetryRequests() override
		{
#ifdef __NEXTION__DEBUG__
			Serial.print( "FElements: " ); Serial.println( FElements.size() );
#endif // __NEXTION__DEBUG__

			for( int i = 0; i < FElements.size(); ++i )
				FElements[ i ]->SendRequest();

//			FOwner.RetryRequests();
		}

		virtual void RegisterRender( ArduinoDisplayNextionElementBasic *AItem )
		{
			FElements.push_back( AItem );
		}

	protected:
		void ProcessRequestQueue()
		{
#ifdef __NEXTION__DEBUG__
			Serial.println( "ProcessRequestQueue" );
#endif // __NEXTION__DEBUG__

//			if( ! FStarted )
//				return;

//			Serial.println( FRequestElements.size() );
			if( FLastResponse )
				return;

#ifdef __NEXTION__DEBUG__
			Serial.println( FRequestElements.size() );
#endif // __NEXTION__DEBUG__
			uint32_t ASkipped = 0;
			while( FRequestElements.size() > ASkipped )
			{
				TDisplayNextionResponseElementBasic *AElement = FRequestElements[ 0 ]->Response;
				Mitov::String ARequest = FRequestElements[ 0 ]->Request;
				int8_t APage = FRequestElements[ 0 ]->Page;
				FRequestElements.pop_front();
				if( APage >= 0 )
					if( APage != FActivePageNumber )
					{
						++ ASkipped;
						FRequestElements.push_back( new TRequestElement( ARequest, APage, AElement ));
						continue;
					}

				FCurrentRequest = ARequest;
				SendCommand( FCurrentRequest.c_str() );

#ifdef __NEXTION__DEBUG__
				Serial.print( "REQUEST: " );
				Serial.print( FCurrentRequest );
				Serial.print( " PAGE: " );
				Serial.println( APage );
#endif // __NEXTION__DEBUG__

				FLastResponse = AElement;
#ifdef __NEXTION__DEBUG__
				Serial.print( "FLastResponse = " );
				Serial.println( uint32_t( FLastResponse ));
#endif // __NEXTION__DEBUG__
				break;
//				FResponseHandlersQueue.push_back( AElement );
//				if( AElement->RequestInfo())
//					break;

//				Mitov::String AText = FRequestElements[ 0 ].ID;
//				FResponseHandlersQueue.push_back( new TResponseElementGetID( FRequestElements[ 0 ] ));
//				FRequestElements.pop_front();
//				SendCommand( AText );
			}
		}

		void ProcessChar( uint8_t AChar )
		{
			if( ! FStarted )
			{
				if( AChar != 0x88 )
					return;

				FStarted = true;
			}
			
#ifdef __NEXTION__DEBUG__
			Serial.print( "FIndex: " ); Serial.print( FIndex ); Serial.print( " - "); Serial.println( AChar, HEX );
#endif // __NEXTION__DEBUG__

			if( FIndex == 0 )
			{
#ifdef __NEXTION__DEBUG__
				Serial.print( "RESULT CODE: " ); Serial.println( AChar, HEX );
//				Serial.print( "FIndex: " ); Serial.println( FIndex );

				Serial.print( "FLastResponse = " );
				Serial.println( uint32_t( FLastResponse ));

				Serial.println( FCurrentRequest );
#endif // __NEXTION__DEBUG__
				switch( AChar )
				{
					case 0x00: // Invalid instruction
					case 0x03: // Page ID invalid
					case 0x04: // Picture ID invalid
					case 0x05: // Font ID invalid
					case 0x1A: // Variable name invalid
					case 0x1B: // 	Variable operation invalid
						FLength = 4;
						break;

					case 0x65:
						FLength = 7;
						break;

					case 0x66:
						FLength = 5;
						break;

					case 0x67:
						FLength = 9;
						break;

					case 0x68:
						FLength = 9;
						break;

					case 0x70: // String variable data returns
						FLength = 4; // Skip the sring content
						break;

					case 0x71:
						FLength = 8;
//						Serial.println( "FLength = 8" );
						break;

					case 0x86:
						FLength = 4;
						break;

					case 0x87:
						FLength = 4;
						break;

					case 0x88:
						FLength = 4; // ?
						break;

					case 0x89:
						FLength = 4; // ?
						break;

					case 0xFE:
						FLength = 4; // ?
						break;

					default:
						return; // Unknown Command

				}
			}

			if( FIndex > 0 )
				if( FBuffer[ 0 ] == 0x70 ) // String variable data returns
					if( AChar != 0xFF )
					{
						FResultText += char( AChar );
						return;
					}

			FBuffer[ FIndex ++ ] = AChar;
			if( FIndex < FLength )
				return;

			FIndex = 0;
			if( ( FBuffer[ FLength - 1 ] != 0xFF ) && ( FBuffer[ FLength - 2 ] != 0xFF ) && ( FBuffer[ FLength - 3 ] != 0xFF ) )
			{
//				Serial.println( "Bad command ending" );
				// Bad command ending
				return;
			}

			switch( FBuffer[ 0 ] )
			{
					case 0x65: // Touch event return data
						for( int i = 0; i < FEventElements.size(); ++i )
							if( FEventElements[ i ]->TryProcessEvent( FBuffer ))
								break;

						break;

					case 0x1A:
#ifdef __NEXTION__DEBUG__
						Serial.println( "INVALID" );
#endif // __NEXTION__DEBUG__
//						FLastResponse->ProcessResponse( nullptr );
						delete FLastResponse;
						FLastResponse = nullptr;
						FCurrentRequest = "";
						ProcessRequestQueue();
						SendCommand( "sendme" );
						break;

					case 0x66:
						FActivePageNumber = FBuffer[ 1 ];
#ifdef __NEXTION__DEBUG__
						Serial.print( "ACTIVATE PAGE: " );
						Serial.println( FActivePageNumber );
#endif // __NEXTION__DEBUG__

						for( int i = 0; i < FElements.size(); ++i )
							FElements[ i ]->ActivatePageNo( FActivePageNumber );

						ProcessRequestQueue();
						break;

					case 0x88: // System Startup

					case 0x71: // Numeric variable data returns
#ifdef __NEXTION__DEBUG__
						Serial.println( "TEST" );
#endif // __NEXTION__DEBUG__
						if( FLastResponse )
						{
#ifdef __NEXTION__DEBUG__
							Serial.println( "TEST11" );
#endif // __NEXTION__DEBUG__
							if( FLastResponse->ProcessResponse( FBuffer ))
							{
#ifdef __NEXTION__DEBUG__
								Serial.println( "CLEAR RESPONSE" );
#endif // __NEXTION__DEBUG__
								delete FLastResponse;
								FLastResponse = nullptr;
								FCurrentRequest = "";
								ProcessRequestQueue();
							}
							else if( FBuffer[ 0 ] == 0x66 )
								RetryRequests();

						}
						else if( FBuffer[ 0 ] == 0x66 )
							RetryRequests();

						break;

					case 0x70: // String variable data returns
//						Serial.println( "TEST1" );
#ifdef __NEXTION__DEBUG__
						Serial.println( "TEST1" );
#endif // __NEXTION__DEBUG__
						if( FLastResponse )
						{
#ifdef __NEXTION__DEBUG__
							Serial.println( "TEST2" );
							Serial.println( FResultText );
#endif // __NEXTION__DEBUG__
							if( FLastResponse->ProcessTextResponse( FResultText ))
							{
								delete FLastResponse;
								FLastResponse = nullptr;
								FCurrentRequest = "";
								ProcessRequestQueue();
							}
						}
						FResultText = "";
						break;
			}

/*
			if( AChar != 10 )
			{
				FBuffer[ FIndex ++ ] = AChar;
				if( FIndex < 255 )
					return;
			}
*/
//			FBuffer[ FIndex ] = '\0';
//			FIndex = 0;

//			Mitov::String AString = FBuffer;

		}

	public:
		virtual void AddRequest( Mitov::String ARequest, int8_t APage, TDisplayNextionResponseElementBasic *AResponse ) override
		{
//			Serial.print( "REQUEST: " ); Serial.println( ARequest );

			FRequestElements.push_back( new TRequestElement( ARequest, APage, AResponse ));
			ProcessRequestQueue();
//			FResponseHandlersQueue.push_back( AResponse );
		}

		virtual	void AddRequestFront( Mitov::String ARequest, int8_t APage, TDisplayNextionResponseElementBasic *AResponse ) override
		{
			FRequestElements.push_front( new TRequestElement( ARequest, APage, AResponse ));
			ProcessRequestQueue();
		}

		virtual	void RegisterEvents( DisplayNextionEventsIntf *AEvents ) override
		{
			FEventElements.push_back( AEvents );
		}

	public:
		void _DirectPinReceive( void *_Data )
		{
			Mitov::TDataBlock ADataBlock = *(Mitov::TDataBlock*)_Data;
			for( int i = 0; i < ADataBlock.Size; ++i )
				ProcessChar( ADataBlock.Data[ i ] );
		}

	public:
		inline void SystemInit()
		{
//			FStarted = true;
			AddRequest( "rest", -1, new TResponseReset( this ) );
//			delay( 1000 );
			RetryRequests();
			ProcessRequestQueue();
			for( int i = 0; i < FEventElements.size(); ++i )
				FEventElements[ i ]->ResetEvent();

			FLastTime = millis();
		}

		inline void SystemLoopBegin( unsigned long currentMicros )
		{
//			ReadSerial();
			if( FCurrentRequest != "" )
				if( millis() - FLastTime > 1000 )
				{
					FLastTime = millis();
					SendCommand( FCurrentRequest.c_str() );
//					ProcessRequestQueue();
				}
		}
/*
		virtual void SystemLoopBegin( unsigned long currentMicros ) override
		{
//			Serial.println( "." );
//			if( FNeedsRead || ( ! ClockInputPin_IsConnected ))
				ReadSensor();

			inherited::SystemLoopBegin( currentMicros );
		}
*/
	};
//---------------------------------------------------------------------------
	class DisplayNextionElementScope : public ArduinoDisplayNextionElementBasic
	{
		typedef ArduinoDisplayNextionElementBasic inherited;

	public:
		uint8_t	ID = 1;
		uint8_t	Height = 255;
		bool	Ready = false;

	protected:
		struct TResponseElementGetValue : public TDisplayNextionResponseElementBasic
		{
		public:
			DisplayNextionElementScope *FOwner;

		public:
			virtual bool ProcessResponse( uint8_t *ABuffer ) override
			{
//				Serial.print( "Height: " ); Serial.println( ABuffer[ 1 ] );
				FOwner->Height = ABuffer[ 1 ];
				FOwner->Ready = true;
				return true;
			}

		public:
			TResponseElementGetValue( DisplayNextionElementScope *AOwner ) :
				FOwner( AOwner )
			{
			}
		};

/*
	public:
		virtual bool RequestInfo() override
		{
			Serial.println( ElementName );
			RequestElementID( ElementName );

			FOwner.AddRequest( Mitov::String( "get " ) + ElementName + ".h", new TResponseElementGetValue( this )); // Get the Height

//			FOwner.FResponseHandlersQueue.push_back( new TResponseElementGetValue( this ));
//			FOwner.SendCommand( ( Mitov::String( "get " ) + ElementName + ".h" ).c_str() ); // Get the Height
			return true;
		}
*/
	public:
		inline void ClearInputPin_o_Receive( void * )
		{
			FOwner.SendCommand( ( Mitov::String( "cle " ) + ID + ",255" ).c_str() );
		}

	public:
		virtual void SendRequest() override
		{
			if( ! Identified )
			{
				RequestElementID( GetElementName() );
				FOwner.AddRequest( Mitov::String( "get " ) + GetElementName() + ".h", PageNumber, new TResponseElementGetValue( this ) );
			}

			inherited::SendRequest();
		}

	public:
		using inherited::inherited;

	};
//---------------------------------------------------------------------------
	class DisplayNextionScopeChannel
	{
	protected:
		DisplayNextionElementScope &FOwner;
		uint8_t	FIndex;

	public:
		void InputPin_o_Receive( void * _Data )
		{
			if( ! FOwner.Ready )
				return;

			float AFloatValue = MitovMax( 0.0f, MitovMin( 1.0f, *(float *)_Data ));
//			Serial.println( AFloatValue );
			int AIntValue = int( AFloatValue * FOwner.Height + 0.5 );
			FOwner.FOwner.SendCommand( ( Mitov::String( "add " ) + FOwner.ID + "," + FIndex + "," + AIntValue ).c_str() );
		}

		inline void ClearInputPin_o_Receive( void * )
		{
			FOwner.FOwner.SendCommand( ( Mitov::String( "cle " ) + FOwner.ID + "," + FIndex ).c_str() );
		}

	public:
		DisplayNextionScopeChannel( DisplayNextionElementScope &AOwner, uint8_t AIndex ) :
			FOwner( AOwner ),
			FIndex( AIndex )
		{
		}
	};
//---------------------------------------------------------------------------
	enum TArduinoDisplayNextionElementButtonEvents { nbeBoth, nbeDown, nbeUp };
//---------------------------------------------------------------------------
	class DisplayNextionElementButton : public ArduinoDisplayNextionElementBasic, public DisplayNextionEventsIntf
	{
		typedef ArduinoDisplayNextionElementBasic inherited;

	public:
		OpenWire::SourcePin	OutputPin;

	public:
		TArduinoDisplayNextionElementButtonEvents	Events : 2;

	public:
		virtual	void ResetEvent() override
		{
			OutputPin.SendValue( false );
		}

		virtual	bool TryProcessEvent( uint8_t *ABuffer ) override
		{
			if( !Identified )
				return false;

			if( ABuffer[ 2 ] != ID )
				return false;

			if( ABuffer[ 1 ] != PageNumber ) //FOwner.GetPageNumber() )
				return false;

			if( ABuffer[ 3 ] )
			{
				if( Events == nbeBoth )
					OutputPin.SendValue( true );

				else if( Events == nbeDown )
				{
					OutputPin.SendValue( true );
					OutputPin.SendValue( false );
				}
			}

			else
			{
				if( Events == nbeBoth )
					OutputPin.SendValue( false );

				else if( Events == nbeUp )
				{
					OutputPin.SendValue( true );
					OutputPin.SendValue( false );
				}
			}

			return true;
		}

	public:
		virtual void SendRequest() override
		{
#ifdef __NEXTION__DEBUG__
			Serial.print( "BUTTON Identified: " ); Serial.println( Identified );
#endif //__NEXTION__DEBUG__
			if( ! Identified )
				RequestElementID( GetElementName() );

			inherited::SendRequest();
		}

	public:
		DisplayNextionElementButton( DisplayNextionIntf &AOwner, Mitov::String AElementName, int APageID, int AElementID ) :
			inherited( AOwner, AElementName, APageID, AElementID ),
			Events( nbeBoth )
		{
			FOwner.RegisterEvents( this );
		}
	};
//---------------------------------------------------------------------------
	class DisplayNextionElementCheckBox : public ArduinoDisplayNextionElementBasic, public DisplayNextionEventsIntf
	{
		typedef ArduinoDisplayNextionElementBasic inherited;

	public:
		OpenWire::TypedStartSourcePin<bool>	OutputPin;

	protected:
		struct TResponseElementGetValue : public TDisplayNextionResponseElementBasic
		{
		public:
			DisplayNextionElementCheckBox *FOwner;

		public:
			virtual bool ProcessResponse( uint8_t *ABuffer ) override
			{
//				Serial.print( "Height: " ); Serial.println( ABuffer[ 1 ] );
				FOwner->OutputPin.SetValue( ABuffer[ 1 ] != 0 );
				return true;
			}

		public:
			TResponseElementGetValue( DisplayNextionElementCheckBox *AOwner ) :
				FOwner( AOwner )
			{
			}
		};

	public:
		virtual	bool TryProcessEvent( uint8_t *ABuffer ) override
		{
			if( ! Identified )
				return false;

			if( ABuffer[ 2 ] != ID )
				return false;

			if( ABuffer[ 1 ] != PageNumber ) // FOwner.GetPageNumber() )
				return false;

			RequestState();
			return true;
		}

	protected:
		void RequestState()
		{
			FOwner.AddRequest( Mitov::String( "get " ) + GetElementName() + ".val", PageNumber, new TResponseElementGetValue( this ) );
		}

	public:
		virtual void SendRequest() override
		{
			if( ! Identified )
			{
				RequestElementID( GetElementName() );
				RequestState();
			}

			inherited::SendRequest();
		}

	public:
		DisplayNextionElementCheckBox( DisplayNextionIntf &AOwner, Mitov::String AElementName, int APageID, int AElementID ) :
			inherited( AOwner, AElementName, APageID, AElementID )
		{
			FOwner.RegisterEvents( this );
		}
	};
//---------------------------------------------------------------------------
	class DisplayNextionElementSlider : public ArduinoDisplayNextionElementBasic, public DisplayNextionEventsIntf
	{
		typedef ArduinoDisplayNextionElementBasic inherited;

	public:
		OpenWire::TypedStartSourcePin<float>	OutputPin;

	public:
		uint32_t	Min = 0;
		uint32_t	Max = 100;

#ifdef __NEXTION_SCAN_SLIDER__
		uint32_t	PressedScanPeriod = 0;

	protected:
		unsigned long	FLastTime = 0;
		bool			FPressed = false;
#endif

	protected:
		struct TResponseElementGetValue : public TDisplayNextionResponseElementBasic
		{
		public:
			DisplayNextionElementSlider *FOwner;

		public:
			virtual bool ProcessResponse( uint8_t *ABuffer ) override
			{
				uint32_t AValue = ABuffer[ 1 ] | ( int32_t( ABuffer[ 2 ] ) << 8 ) | ( int32_t( ABuffer[ 3 ] ) << 16 ) | ( int32_t( ABuffer[ 4 ] ) << 24 );
				float AFloatValue = float( AValue - FOwner->Min ) / ( FOwner->Max - FOwner->Min );
				FOwner->OutputPin.SetValue( AFloatValue );
//				FOwner->OutputPin.SendValue<int32_t>( ABuffer[ 1 ] | ( int32_t( ABuffer[ 2 ] ) << 8 ) | ( int32_t( ABuffer[ 3 ] ) << 16 ) | ( int32_t( ABuffer[ 4 ] ) << 24 ) );
//				Serial.print( "Height: " ); Serial.println( ABuffer[ 1 ] );
//				FOwner->OutputPin.SetValue( ABuffer[ 1 ] != 0 );
				return true;
			}

		public:
			TResponseElementGetValue( DisplayNextionElementSlider *AOwner ) :
				FOwner( AOwner )
			{
			}
		};

	public:
		inline void ClockInputPin_o_Receive( void *_Data )
		{
			RequestState();
		}

	public:
		virtual	bool TryProcessEvent( uint8_t *ABuffer ) override
		{
			if( ! Identified )
				return false;

			if( ABuffer[ 2 ] != ID )
				return false;

			if( ABuffer[ 1 ] != PageNumber ) //FOwner.GetPageNumber() )
				return false;

			RequestState();
#ifdef __NEXTION_SCAN_SLIDER__
			FPressed = ( ABuffer[ 3 ] != 0 );
			if ( FPressed )
				if( PressedScanPeriod )
					FLastTime = millis();
#endif
			return true;
		}

	protected:
		void RequestState()
		{
			FOwner.AddRequest( Mitov::String( "get " ) + GetElementName() + ".val", PageNumber, new TResponseElementGetValue( this ) );
		}

#ifdef __NEXTION_SCAN_SLIDER__
	public:
		inline void SystemLoopBegin( unsigned long currentMicros )
		{
			if( FPressed )
				if( PressedScanPeriod )
					if( millis() - FLastTime > PressedScanPeriod )
					{
						FLastTime = millis();
//						Serial.println( "SCAN" );
						RequestState();
					}

		}
#endif

	public:
		virtual void SendRequest() override
		{
			if( ! Identified )
			{
				RequestElementID( GetElementName() );
				RequestState();
			}

			inherited::SendRequest();
		}

	public:
		DisplayNextionElementSlider( DisplayNextionIntf &AOwner, Mitov::String AElementName, int APageID, int AElementID ) :
			inherited( AOwner, AElementName, APageID, AElementID )
		{
			FOwner.RegisterEvents( this );
		}
	};
//---------------------------------------------------------------------------
	class DisplayNextionElementModifierPropertySetterString
	{
	public:
		Mitov::String	PropertyName;
		Mitov::String	Value;

	protected:
		ArduinoDisplayNextionElementBasic &FOwner;

	public:
		inline void ClockInputPin_o_Receive( void *_Data )
		{
			FOwner.FOwner.SendCommand(( FOwner.GetElementName() + "." + PropertyName + "=\"" + Value + "\"" ).c_str() );
		}

	public:
		DisplayNextionElementModifierPropertySetterString( ArduinoDisplayNextionElementBasic &AOwner ) :
			FOwner( AOwner )
		{
		}

	};
//---------------------------------------------------------------------------
	class DisplayNextionElementModifierPropertySetterInteger
	{
	public:
		Mitov::String	PropertyName;
		int	Value = 0;

	protected:
		ArduinoDisplayNextionElementBasic &FOwner;

	public:
		inline void ClockInputPin_o_Receive( void *_Data )
		{
			FOwner.FOwner.SendCommand(( FOwner.GetElementName() + "." + PropertyName + "=" + Value ).c_str() );
		}

	public:
		DisplayNextionElementModifierPropertySetterInteger( ArduinoDisplayNextionElementBasic &AOwner ) :
			FOwner( AOwner )
		{
		}

	};
//---------------------------------------------------------------------------
	class DisplayNextionElementModifierPropertyGetterInteger
	{
	public:
		OpenWire::SourcePin	OutputPin;

	public:
		Mitov::String	PropertyName;

	protected:
		ArduinoDisplayNextionElementBasic &FOwner;

	public:
		bool	FRequested = false;

	protected:
		struct TResponseElementGetValue : public TDisplayNextionResponseElementBasic
		{
		public:
			DisplayNextionElementModifierPropertyGetterInteger *FOwner;

		public:
			virtual bool ProcessResponse( uint8_t *ABuffer ) override
			{
//				Serial.println( "RESPONSE" );
				FOwner->OutputPin.SendValue<int32_t>( ABuffer[ 1 ] | ( int32_t( ABuffer[ 2 ] ) << 8 ) | ( int32_t( ABuffer[ 3 ] ) << 16 ) | ( int32_t( ABuffer[ 4 ] ) << 24 ) );
				return true;
			}

		public:
			TResponseElementGetValue( DisplayNextionElementModifierPropertyGetterInteger *AOwner ) :
				FOwner( AOwner )
			{
				FOwner->FRequested = true;
			}

			virtual ~TResponseElementGetValue()
			{
				FOwner->FRequested = false;
			}
		};

	public:
		inline void ClockInputPin_o_Receive( void *_Data )
		{
//			Serial.println( "GET" );
			if( ! FRequested )
				FOwner.FOwner.AddRequest( Mitov::String( "get " ) + FOwner.GetElementName() + "." + PropertyName, -1, new TResponseElementGetValue( this ) );
		}

	public:
		DisplayNextionElementModifierPropertyGetterInteger( ArduinoDisplayNextionElementBasic &AOwner ) :
			FOwner( AOwner )
		{
		}

	};
//---------------------------------------------------------------------------
	class DisplayNextionElementModifierPropertyGetterString
	{
	public:
		OpenWire::SourcePin	OutputPin;

	public:
		Mitov::String	PropertyName;

	protected:
		ArduinoDisplayNextionElementBasic &FOwner;

	public:
		bool	FRequested = false;

	protected:
		struct TResponseElementGetValue : public TDisplayNextionResponseElementBasic
		{
		public:
			DisplayNextionElementModifierPropertyGetterString *FOwner;

		public:
			virtual bool ProcessTextResponse( Mitov::String AText ) override
			{
//				Serial.println( "RESPONSE" );
//				FOwner->OutputPin.SendValue<int32_t>( ABuffer[ 1 ] | ( int32_t( ABuffer[ 2 ] ) << 8 ) | ( int32_t( ABuffer[ 3 ] ) << 16 ) | ( int32_t( ABuffer[ 4 ] ) << 24 ) );
				FOwner->OutputPin.Notify( (void *)AText.c_str() );
				return true;
			}

		public:
			TResponseElementGetValue( DisplayNextionElementModifierPropertyGetterString *AOwner ) :
				FOwner( AOwner )
			{
				FOwner->FRequested = true;
			}

			virtual ~TResponseElementGetValue()
			{
				FOwner->FRequested = false;
			}
		};

	public:
		inline void ClockInputPin_o_Receive( void *_Data )
		{
//			Serial.println( "GET" );
			if( ! FRequested )
				FOwner.FOwner.AddRequest( Mitov::String( "get " ) + FOwner.GetElementName() + "." + PropertyName, -1, new TResponseElementGetValue( this ) );
		}

	public:
		DisplayNextionElementModifierPropertyGetterString( ArduinoDisplayNextionElementBasic &AOwner ) :
			FOwner( AOwner )
		{
		}

	};
//---------------------------------------------------------------------------
	class DisplayNextionElementModifierPressedState : public DisplayNextionEventsIntf
	{
	public:
		OpenWire::SourcePin	OutputPin;

	protected:
		ArduinoDisplayNextionElementBasic &FOwner;

	public:
		virtual	void ResetEvent() override
		{
			OutputPin.SendValue( false );
		}

		virtual	bool TryProcessEvent( uint8_t *ABuffer ) override
		{
			if( ! FOwner.Identified )
				return false;

			if( ABuffer[ 2 ] != FOwner.ID )
				return false;

			if( ABuffer[ 1 ] != FOwner.PageNumber ) // FOwner.FOwner.GetPageNumber() )
				return false;

			OutputPin.SendValue( ABuffer[ 3 ] != 0 );
			return true;
		}

	public:
		DisplayNextionElementModifierPressedState( ArduinoDisplayNextionElementBasic &AOwner ) :
			FOwner( AOwner )
		{
			FOwner.FOwner.RegisterEvents( this );
		}
	};
//---------------------------------------------------------------------------
	class DisplayNextionElementModifierPressEvent : public DisplayNextionEventsIntf
	{
	public:
		OpenWire::SourcePin	OutputPin;

	protected:
		ArduinoDisplayNextionElementBasic &FOwner;

	public:
		virtual	bool TryProcessEvent( uint8_t *ABuffer ) override
		{
			if( ! FOwner.Identified )
				return false;

			if( ABuffer[ 2 ] != FOwner.ID )
				return false;

			if( ABuffer[ 1 ] != FOwner.PageNumber ) // FOwner.FOwner.GetPageNumber() )
				return false;

			if( ABuffer[ 3 ] )
				OutputPin.Notify( nullptr );

			return true;
		}

	public:
		DisplayNextionElementModifierPressEvent( ArduinoDisplayNextionElementBasic &AOwner ) :
			FOwner( AOwner )
		{
			FOwner.FOwner.RegisterEvents( this );
		}
	};
//---------------------------------------------------------------------------
	class DisplayNextionElementModifierReleaseEvent : public DisplayNextionEventsIntf
	{
	public:
		OpenWire::SourcePin	OutputPin;

	protected:
		ArduinoDisplayNextionElementBasic &FOwner;

	public:
		virtual	bool TryProcessEvent( uint8_t *ABuffer ) override
		{
			if( ! FOwner.Identified )
				return false;

			if( ABuffer[ 2 ] != FOwner.ID )
				return false;

			if( ABuffer[ 1 ] != FOwner.PageNumber ) // FOwner.FOwner.GetPageNumber() )
				return false;

			if( ! ABuffer[ 3 ] )
				OutputPin.Notify( nullptr );

			return true;
		}

	public:
		DisplayNextionElementModifierReleaseEvent( ArduinoDisplayNextionElementBasic &AOwner ) :
			FOwner( AOwner )
		{
			FOwner.FOwner.RegisterEvents( this );
		}
	};
//---------------------------------------------------------------------------
	class DisplayNextionElementCommand
	{
	public:
		Mitov::String Command;

	protected:
		DisplayNextionIntf &FOwner;

	public:
		inline void ClockInputPin_o_Receive( void *_Data )
		{
			FOwner.SendCommand( Command.c_str() );
		}

	public:
		DisplayNextionElementCommand( DisplayNextionIntf &AOwner ) :
			FOwner( AOwner )
		{
		}

	};
//---------------------------------------------------------------------------
	class DisplayNextionElementGetterCommandString
	{
	public:
		OpenWire::SourcePin	OutputPin;

	public:
		Mitov::String Command;

	protected:
		DisplayNextionIntf &FOwner;

	public:
		bool	FRequested = false;

	protected:
		struct TResponseElementGetValue : public TDisplayNextionResponseElementBasic
		{
		public:
			DisplayNextionElementGetterCommandString *FOwner;

		public:
			virtual bool ProcessTextResponse( Mitov::String AText ) override
			{
//				Serial.println( "RESPONSE" );
				FOwner->OutputPin.Notify( (void *)AText.c_str() );
				return true;
			}

		public:
			TResponseElementGetValue( DisplayNextionElementGetterCommandString *AOwner ) :
				FOwner( AOwner )
			{
				FOwner->FRequested = true;
			}

			virtual ~TResponseElementGetValue()
			{
				FOwner->FRequested = false;
			}
		};

	public:
		inline void ClockInputPin_o_Receive( void *_Data )
		{
			if( ! FRequested )
				FOwner.AddRequest( Command, -1, new TResponseElementGetValue( this ) );

		}

	public:
		DisplayNextionElementGetterCommandString( DisplayNextionIntf &AOwner ) :
			FOwner( AOwner )
		{
		}

	};
//---------------------------------------------------------------------------
	class DisplayNextionElementGetterCommandInteger
	{
	public:
		OpenWire::SourcePin	OutputPin;

	public:
		Mitov::String Command;

	protected:
		DisplayNextionIntf &FOwner;

	public:
		bool	FRequested = false;

	protected:
		struct TResponseElementGetValue : public TDisplayNextionResponseElementBasic
		{
		public:
			DisplayNextionElementGetterCommandInteger *FOwner;

		public:
			virtual bool ProcessResponse( uint8_t *ABuffer ) override
			{
//				Serial.println( "RESPONSE" );
				FOwner->OutputPin.SendValue<int32_t>( ABuffer[ 1 ] | ( int32_t( ABuffer[ 2 ] ) << 8 ) | ( int32_t( ABuffer[ 3 ] ) << 16 ) | ( int32_t( ABuffer[ 4 ] ) << 24 ) );
				return true;
			}

		public:
			TResponseElementGetValue( DisplayNextionElementGetterCommandInteger *AOwner ) :
				FOwner( AOwner )
			{
				FOwner->FRequested = true;
//				Serial.println( "CREATE" );
			}

			virtual ~TResponseElementGetValue()
			{
				FOwner->FRequested = false;
//				Serial.println( "DESTROY" );
			}
		};

	public:
		inline void ClockInputPin_o_Receive( void *_Data )
		{
			if( ! FRequested )
				FOwner.AddRequest( Command, -1, new TResponseElementGetValue( this ) );
		}

	public:
		DisplayNextionElementGetterCommandInteger( DisplayNextionIntf &AOwner ) :
			FOwner( AOwner )
		{
		}

	};
//---------------------------------------------------------------------------
#undef Min
#undef Max
}

#ifdef __TEST_FOR_DEBUG_PRINTS__
#undef Serial
#endif

#endif
