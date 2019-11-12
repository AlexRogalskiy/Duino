////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2018 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_CURIE_NEURONS_h
#define _MITOV_CURIE_NEURONS_h

#include <Mitov.h>
//#include <CurieNeurons.h>

namespace Mitov
{
	namespace CurieNeuronRegisters
	{
		const int NEURONSIZE = 128;

		volatile uint8_t* PVP_REGISTER_BASE = (volatile uint8_t*)0xB0600000;
		volatile int*	const NM_NCR =      (volatile int*)(PVP_REGISTER_BASE + 0x00);
		volatile int*	const NM_COMP =     (volatile int*)(PVP_REGISTER_BASE + 0x04);
		volatile int*	const NM_LCOMP =    (volatile int*)(PVP_REGISTER_BASE + 0x08);
		volatile int*	const NM_DIST =     (volatile int*)(PVP_REGISTER_BASE + 0x0C);
		volatile int*	const NM_IDX =     	(volatile int*)(PVP_REGISTER_BASE + 0x0C);
		volatile int*	const NM_CAT =     	(volatile int*)(PVP_REGISTER_BASE + 0x10);
		volatile int*	const NM_AIF =     	(volatile int*)(PVP_REGISTER_BASE + 0x14);
		volatile int*	const NM_MINIF =    (volatile int*)(PVP_REGISTER_BASE + 0x18);
		volatile int*	const NM_MAXIF =    (volatile int*)(PVP_REGISTER_BASE + 0x1C);
		volatile int*	const NM_TESTCOMP = (volatile int*)(PVP_REGISTER_BASE + 0x20);
		volatile int*	const NM_TESTCAT =  (volatile int*)(PVP_REGISTER_BASE + 0x24);
		volatile int*	const NM_NID =  	(volatile int*)(PVP_REGISTER_BASE + 0x28);
		volatile int*	const NM_GCR =  	(volatile int*)(PVP_REGISTER_BASE + 0x2C);
		volatile int*	const NM_RSTCHAIN = (volatile int*)(PVP_REGISTER_BASE + 0x30);
		volatile int*	const NM_NSR =  	(volatile int*)(PVP_REGISTER_BASE + 0x34);
		volatile int*	const NM_FORGET =  	(volatile int*)(PVP_REGISTER_BASE + 0x3C);
		volatile int*	const NM_NCOUNT =  	(volatile int*)(PVP_REGISTER_BASE + 0x3C);
	}
//---------------------------------------------------------------------------
	class Arduino101CurieNeurons;
	class Arduino101NeuronTrainRecognizeOperation;
	class Arduino101NeuronItemBasic;
//---------------------------------------------------------------------------
	class Arduino101CurieNeurons
	{
	public:
		inline void SystemStart()
		{
			int TempNSR = *CurieNeuronRegisters::NM_NSR;
			*CurieNeuronRegisters::NM_FORGET = 0;
			*CurieNeuronRegisters::NM_NSR = 16;
			for( int i=0; i < CurieNeuronRegisters::NEURONSIZE; i++ ) 
				*CurieNeuronRegisters::NM_TESTCOMP = 0;

			*CurieNeuronRegisters::NM_NSR = TempNSR;
		}

	public:
		inline void ForgetInputPin_o_Receive( void *_Data )
		{
			*CurieNeuronRegisters::NM_FORGET = 0;
		}

	};
//---------------------------------------------------------------------------
	class Arduino101CurieNeuronsClassifier
	{
	public:
		bool	IsKNN = false;
		bool	UseInfinityDistance = false;

	public:
		uint8_t FContext;

	public:
		TValueArray<float>	FFeaturesValue;

	public:
		void FillFeatures()
		{
			int ACount = MitovMin<uint32_t>( FFeaturesValue.Size, 128 );
			for( int i = 0; i < ACount; ++i )
			{
				float AValue = constrain( FFeaturesValue.Data[ i ], 0.0, 1.0 ) * 255 + 0.5;
				if( i == ACount - 1 )
					*CurieNeuronRegisters::NM_LCOMP = AValue;

				else
					*CurieNeuronRegisters::NM_COMP = AValue;
			}
		}

		void Forget()
		{
			int ANCount = *CurieNeuronRegisters::NM_NCOUNT;
			int ATempNSR = *CurieNeuronRegisters::NM_NSR;
			*CurieNeuronRegisters::NM_NSR = 0x0010;
			*CurieNeuronRegisters::NM_RSTCHAIN = 0;
			int ACountOtherNeurons = 0;
			for( int i=0; i < ANCount; i++ )
			{
				int Temp = *CurieNeuronRegisters::NM_NCR;
				if( Temp && 0x7F != FContext )
					++ACountOtherNeurons;

				Temp = *CurieNeuronRegisters::NM_CAT; // Move to the next neuron
			}

			if( ! ACountOtherNeurons )
				*CurieNeuronRegisters::NM_FORGET = 0;

			else
			{
				int *ANeuronData = new int[ ACountOtherNeurons * ( 4 + ( 128 / 2 )) ];

				*CurieNeuronRegisters::NM_RSTCHAIN = 0;
				int *ACurrentPtr = ANeuronData;
				for( int i=0; i < ANCount; ++i )
				{
					int ATemp = *CurieNeuronRegisters::NM_NCR;
					if( ATemp && 0x7F == FContext )
						ATemp = *CurieNeuronRegisters::NM_CAT; // Move to the next neuron

					else
					{
						*ACurrentPtr++ = ATemp; // Save context
						uint8_t *AKnowledgePtr = (uint8_t *)ACurrentPtr;
						ACurrentPtr += 64;
						for( int j = 0; j < 128; ++j )
							*AKnowledgePtr++ = *CurieNeuronRegisters::NM_COMP;

						*ACurrentPtr++ = *CurieNeuronRegisters::NM_AIF;
						*ACurrentPtr++ = *CurieNeuronRegisters::NM_MINIF;
						*ACurrentPtr++ = *CurieNeuronRegisters::NM_CAT; // Move to the next neuron
					}
				}

//				*CurieNeuronRegisters::NM_NSR = ATempNSR;
				*CurieNeuronRegisters::NM_FORGET = 0;
//				*CurieNeuronRegisters::NM_NSR = 0x0010;
				*CurieNeuronRegisters::NM_RSTCHAIN = 0;

				ACurrentPtr = ANeuronData;
				for( int i=0; i < ANCount; ++i )
				{
					*CurieNeuronRegisters::NM_NCR = *ACurrentPtr++;
					uint8_t *AKnowledgePtr = (uint8_t *)ACurrentPtr;
					ACurrentPtr += 64;
					for( int j = 0; j < 128; ++j )
						*CurieNeuronRegisters::NM_COMP = *AKnowledgePtr++;

					*CurieNeuronRegisters::NM_AIF = *ACurrentPtr++;
					*CurieNeuronRegisters::NM_MINIF = *ACurrentPtr++;
					*CurieNeuronRegisters::NM_CAT = *ACurrentPtr++; // Move to the next neuron
				}

				delete [] ANeuronData;
			}

			*CurieNeuronRegisters::NM_NSR = ATempNSR; // set the NN back to its calling status
		}

	public:
		inline void FeaturesInputPin_o_Receive( void *_Data )
		{
			FFeaturesValue = *(TArray<float>*)_Data;
		}

	public:
		Arduino101CurieNeuronsClassifier( uint8_t AContext ) :
			FContext( AContext )
		{
		}
	};
//---------------------------------------------------------------------------
	class Arduino101NeuronForgetOperation
	{
	public:
		Arduino101CurieNeuronsClassifier &FOwner;

	public:
		inline void ClockInputPin_o_Receive( void *_Data )
		{
			FOwner.Forget();
		}

	public:
		Arduino101NeuronForgetOperation( Arduino101CurieNeuronsClassifier &AOwner ) :
			FOwner( AOwner )
		{
		}

	};
//---------------------------------------------------------------------------
	class Arduino101NeuronReadKnowledgeOperation
	{
	public:
		OpenWire::SourcePin	FeaturesOutputPin;
		OpenWire::SourcePin	CategoryOutputPin;
		OpenWire::SourcePin	InfluenceFieldOutputPin;
		OpenWire::SourcePin	MinInfluenceFieldOutputPin;
		OpenWire::SourcePin	NeuronCompletedOutputPin;
		OpenWire::SourcePin	CompletedOutputPin;

	public:
		Arduino101CurieNeuronsClassifier &FOwner;

	public:
		void ClockInputPin_o_Receive( void *_Data )
		{
			int ANCount = *CurieNeuronRegisters::NM_NCOUNT;
			int ATempNSR = *CurieNeuronRegisters::NM_NSR;

			*CurieNeuronRegisters::NM_NSR = 0x0010;
			*CurieNeuronRegisters::NM_RSTCHAIN = 0;

			float *AFeaturesData = new float[ 128 ];

			for( int i=0; i < ANCount; i++ )
			{
				int ATemp = *CurieNeuronRegisters::NM_NCR;
				if( ATemp && 0x7F == FOwner.FContext )
					ATemp = *CurieNeuronRegisters::NM_CAT; // Move to the next neuron

				else
				{
					float *ADataPtr = AFeaturesData;
					for( int j = 0; j < 128; ++j )
						*ADataPtr++ = float( *CurieNeuronRegisters::NM_COMP ) / 255;

					FeaturesOutputPin.SendValue( TArray<float>( 128, AFeaturesData ));
					InfluenceFieldOutputPin.SendValue<uint32_t>( *CurieNeuronRegisters::NM_AIF );
					MinInfluenceFieldOutputPin.SendValue<uint32_t>( *CurieNeuronRegisters::NM_MINIF );
					CategoryOutputPin.SendValue<int32_t>( *CurieNeuronRegisters::NM_CAT );
					NeuronCompletedOutputPin.Notify( nullptr );
				}
			}

			delete [] AFeaturesData;
			*CurieNeuronRegisters::NM_NSR = ATempNSR; // set the NN back to its calling status

			CompletedOutputPin.Notify( nullptr );
		}

	public:
		Arduino101NeuronReadKnowledgeOperation( Arduino101CurieNeuronsClassifier &AOwner ) :
			FOwner( AOwner )
		{
		}

	};
//---------------------------------------------------------------------------
	class Arduino101NeuronBasicAddNeuronOperation
	{
	public:
		Arduino101CurieNeuronsClassifier &FOwner;

	public:
		int16_t	Category = 1;
		int16_t	MinInfluenceField = 2;
		int16_t	InfluenceField = 0x4000;

	public:
		uint32_t _MaxIndex;

	protected:
		virtual void LoadFeatures() = 0;

	public:
		void ClockInputPin_o_Receive( void *_Data )
		{
			int ATempNSR = *CurieNeuronRegisters::NM_NSR;
			*CurieNeuronRegisters::NM_NSR = 0x0010;

			*CurieNeuronRegisters::NM_NCR = FOwner.FContext;
			LoadFeatures();

			*CurieNeuronRegisters::NM_AIF = InfluenceField;
			*CurieNeuronRegisters::NM_MINIF = MinInfluenceField;
			*CurieNeuronRegisters::NM_CAT = Category; // Move to the next neuron

			*CurieNeuronRegisters::NM_NSR = ATempNSR; // set the NN back to its calling status
		}

	public:
		inline void SystemStart()
		{
			ClockInputPin_o_Receive( nullptr );
		}

	public:
		Arduino101NeuronBasicAddNeuronOperation( Arduino101CurieNeuronsClassifier &AOwner ) :
			FOwner( AOwner )
		{
		}

	};
//---------------------------------------------------------------------------
	class Arduino101NeuronAddNeuronOperation : public Arduino101NeuronBasicAddNeuronOperation
	{
		typedef Arduino101NeuronBasicAddNeuronOperation inherited;

	public:
		float *_Values;

	public:
		float &operator[]( uint32_t AIndex ) { return _Values[ AIndex ]; }

	public:
		void UpdateArrayValue()
		{
			for( uint32_t i = 0; i <= MitovMin<uint32_t>( _MaxIndex, 127 ); ++i )
				*CurieNeuronRegisters::NM_COMP = _Values[ i ] * 255 + 0.5;
		}

	protected:
		virtual void LoadFeatures() override
		{
			UpdateArrayValue();
		}

	public:
		using inherited::inherited;

	};
//---------------------------------------------------------------------------
	class Arduino101NeuronAddNeuronOperationConst : public Arduino101NeuronBasicAddNeuronOperation
	{
		typedef Arduino101NeuronBasicAddNeuronOperation inherited;

	public:
		const /*PROGMEM*/ float *_Values;

	protected:
		virtual void LoadFeatures() override
		{
			for( uint32_t i = 0; i <= MitovMin<uint32_t>( _MaxIndex, 127 ); ++i )
				*CurieNeuronRegisters::NM_COMP = pgm_read_float( _Values + i ) * 255 + 0.5;
		}

	public:
		using inherited::inherited;

	};
//---------------------------------------------------------------------------
	class Arduino101NeuronLoadKnowledgeOperation
	{
	public:
		SimpleList<Arduino101NeuronItemBasic*>	FNeurons;

	public:
		Arduino101CurieNeuronsClassifier &FOwner;

	public:
		void ClockInputPin_o_Receive( void *_Data );

	public:
		inline void SystemStart()
		{
			ClockInputPin_o_Receive( nullptr );
		}

	public:
		Arduino101NeuronLoadKnowledgeOperation( Arduino101CurieNeuronsClassifier &AOwner ) :
			FOwner( AOwner )
		{
		}

	};
//---------------------------------------------------------------------------
	class Arduino101NeuronItemBasic
	{
	public:
		int16_t	Category = 1;
		int16_t	MinInfluenceField = 2;
		int16_t	InfluenceField = 0x4000;

	public:
		uint32_t _MaxIndex;

//	public:
//		Arduino101NeuronLoadKnowledgeOperation &FOwner;

	public:
		virtual void LoadFeatures() = 0;

	public:
		Arduino101NeuronItemBasic( Arduino101NeuronLoadKnowledgeOperation &AOwner )
//			FOwner( AOwner )
		{
			AOwner.FNeurons.push_back( this );
		}

	};
//---------------------------------------------------------------------------
	class Arduino101NeuronItem : public Arduino101NeuronItemBasic
	{
		typedef Arduino101NeuronItemBasic inherited;

	public:
		float *_Values;

	public:
		virtual void LoadFeatures() override
		{
			for( uint32_t i = 0; i <= MitovMin<uint32_t>( _MaxIndex, 127 ); ++i )
				*CurieNeuronRegisters::NM_COMP = _Values[ i ] * 255 + 0.5;
		}

	public:
		using inherited::inherited;

	};
//---------------------------------------------------------------------------
	class Arduino101NeuronItemConst : public Arduino101NeuronItemBasic
	{
		typedef Arduino101NeuronItemBasic inherited;

	public:
		const /*PROGMEM*/ float *_Values;

	public:
		virtual void LoadFeatures() override
		{
			for( uint32_t i = 0; i <= MitovMin<uint32_t>( _MaxIndex, 127 ); ++i )
				*CurieNeuronRegisters::NM_COMP = pgm_read_float( _Values + i ) * 255 + 0.5;
		}

	public:
		using inherited::inherited;

	};
//---------------------------------------------------------------------------
	class Arduino101CurieTrainNeurons
	{
	public:
//		OpenWire::ValueSimpleSinkPin<int32_t>	CategoryInputPin;
		OpenWire::TypedSourcePin<uint32_t>		CountUsedOutputPin;

	public:
		int16_t	Category = 1;
		int16_t	MinInfluenceField = 2;
		int16_t	MaxInfluenceField = 0x4000;

//	protected:
//		int32_t	FValue = 0;

	protected:
		Arduino101CurieNeuronsClassifier &FOwner;

/*
	public:
		void CategoryInputPin_o_Receive( void *_Data )
		{
			FValue = 
		}
*/
	public:
		void TrainInputPin_o_Receive( void *_Data )
		{
/*
			int ACount = 0;
			for( int i = 0; i < FOwner.FFeatures.size(); ++i )
				ACount += FOwner.FFeatures[ i ]->GetCount();

			uint8_t *ABuffer = new uint8_t[ ACount ];
			uint8_t *APtr = ABuffer;
			for( int i = 0; i < FOwner.FFeatures.size(); ++i )
				FOwner.FFeatures[ i ]->GetFeatures( APtr );

			Arduino101CurieNeurons::FNeurons.Learn( ABuffer, ACount, constrain( CategoryInputPin.Value, -32768, 32767 ));

			delete []ABuffer;
*/
			if( FOwner.IsKNN )
				*CurieNeuronRegisters::NM_NSR = *CurieNeuronRegisters::NM_NSR | 0x20;

			else
				*CurieNeuronRegisters::NM_NSR = *CurieNeuronRegisters::NM_NSR & ~0x20;

//			*CurieNeuronRegisters::NM_GCR = FOwner.FContext;
			if( FOwner.UseInfinityDistance )
				*CurieNeuronRegisters::NM_GCR = ( FOwner.FContext & 0x7F ) | 0x80;

			else
				*CurieNeuronRegisters::NM_GCR = FOwner.FContext & 0x7F;

			*CurieNeuronRegisters::NM_MINIF = MinInfluenceField;
			*CurieNeuronRegisters::NM_MAXIF = MaxInfluenceField;

			FOwner.FillFeatures();

//			Serial.println( "TRAIN" );
			*CurieNeuronRegisters::NM_CAT = Category;
		}

	public:
		Arduino101CurieTrainNeurons( Arduino101CurieNeuronsClassifier &AOwner ) :
			FOwner( AOwner )
		{
		}

	};
//---------------------------------------------------------------------------
	class Arduino101CurieRecognizeNeurons
	{
	public:
		OpenWire::SourcePin	CategoryOutputPin;
		OpenWire::SourcePin	DistanceOutputPin;
		OpenWire::SourcePin	NeuronIndexOutputPin;
		OpenWire::SourcePin	DegeneratedOutputPin;
		OpenWire::SourcePin	RecognizedOutputPin;
		OpenWire::SourcePin	DisagreenmentOutputPin;
		OpenWire::SourcePin	CountNeuronsOutputPin;

	protected:
		Arduino101CurieNeuronsClassifier &FOwner;

	public:
		void ClockInputPin_o_Receive( void *_Data )
		{
/*
			int ACount = 0;
			for( int i = 0; i < FOwner.FFeatures.size(); ++i )
				ACount += FOwner.FFeatures[ i ]->GetCount();

			uint8_t *ABuffer = new uint8_t[ ACount ];
			uint8_t *APtr = ABuffer;
			for( int i = 0; i < FOwner.FFeatures.size(); ++i )
				FOwner.FFeatures[ i ]->GetFeatures( APtr );
*/
			*CurieNeuronRegisters::NM_GCR = FOwner.FContext;

			FOwner.FillFeatures();

			int AResult = *CurieNeuronRegisters::NM_NSR;

//			Arduino101CurieNeurons::FNeurons.Learn( ABuffer, ACount, constrain( CategoryInputPin.Value, -32768, 32767 ));
/*
			int Adistance;
			int ACategory;
			int ANid;
			int AResult = Arduino101CurieNeurons::FNeurons.Classify( ABuffer, ACount, &Adistance, &ACategory, &ANid );
*/
			switch( AResult )
			{
				case 4:
//					CategoryOutputPin.SendValue<int32_t>( ACategory );
					DisagreenmentOutputPin.SendValue( true );
					RecognizedOutputPin.SendValue( true );
					break;

				case 8:
//					CategoryOutputPin.SendValue<int32_t>( ACategory );
					DisagreenmentOutputPin.SendValue( false );
					RecognizedOutputPin.SendValue( true );
					break;

				default:
					DisagreenmentOutputPin.SendValue( false );
					RecognizedOutputPin.SendValue( false );
			}

			uint32_t ANeuronCount = 0;

			for(;;)
			{
				uint32_t Adistance = *CurieNeuronRegisters::NM_DIST;
				if( Adistance == 0xFFFF )
					break;

				int ACategory = *CurieNeuronRegisters::NM_CAT;
				bool ADegenerated = ( ( ACategory & 0x8000 ) != 0 );
				ACategory &= 0x7FFF;

				uint32_t ANeuronId = *CurieNeuronRegisters::NM_NID;

				++ ANeuronCount;
				DegeneratedOutputPin.Notify( &ADegenerated );
				NeuronIndexOutputPin.Notify( &ANeuronId );
				DistanceOutputPin.Notify( &Adistance );
				CategoryOutputPin.SendValue<int32_t>( ACategory );
			}

			CountNeuronsOutputPin.Notify( &ANeuronCount );
//			delete []ABuffer;
		}

	public:
		Arduino101CurieRecognizeNeurons( Arduino101CurieNeuronsClassifier &AOwner ) :
			FOwner( AOwner )
		{
		}
	};
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
	void Arduino101NeuronLoadKnowledgeOperation::ClockInputPin_o_Receive( void *_Data )
	{
		int ATempNSR = *CurieNeuronRegisters::NM_NSR;
		*CurieNeuronRegisters::NM_NSR = 0x0010;

		*CurieNeuronRegisters::NM_NCR = FOwner.FContext;

		for( int i = 0; i < FNeurons.size(); ++i )
		{
			FNeurons[ i ]->LoadFeatures();
			*CurieNeuronRegisters::NM_AIF = FNeurons[ i ]->InfluenceField;
			*CurieNeuronRegisters::NM_MINIF = FNeurons[ i ]->MinInfluenceField;
			*CurieNeuronRegisters::NM_CAT = FNeurons[ i ]->Category; // Move to the next neuron
		}

		*CurieNeuronRegisters::NM_NSR = ATempNSR; // set the NN back to its calling status
	}
//---------------------------------------------------------------------------
}

#endif
