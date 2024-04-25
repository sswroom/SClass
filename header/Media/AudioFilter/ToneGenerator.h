#ifndef _SM_MEDIA_AUDIOFILTER_TONEGENERATOR
#define _SM_MEDIA_AUDIOFILTER_TONEGENERATOR
#include "Media/IAudioFilter.h"
#include "Sync/Mutex.h"

namespace Media
{
	namespace AudioFilter
	{
		class ToneGenerator : public IAudioFilter
		{
		public:
			typedef enum
			{
				IT_SINCWAVE,
				IT_SQUAREWAVE,
				IT_TRIANGLEWAVE,
				IT_SAWWAVE
			} InstrumentType;

			typedef enum
			{
				TT_A1,
				TT_A1S,
				TT_B1,
				TT_C1,
				TT_C1S,
				TT_D1,
				TT_D1S,
				TT_E1,
				TT_F1,
				TT_F1S,
				TT_G1,
				TT_G1S,
				TT_A2,
				TT_A2S,
				TT_B2,
				TT_C2,
				TT_C2S,
				TT_D2,
				TT_D2S,
				TT_E2,
				TT_F2,
				TT_F2S,
				TT_G2,
				TT_G2S,
				TT_A3,
				TT_A3S,
				TT_B3,
				TT_C3,
				TT_C3S,
				TT_D3,
				TT_D3S,
				TT_E3,
				TT_F3,
				TT_F3S,
				TT_G3,
				TT_G3S,
				TT_A4,
				TT_A4S,
				TT_B4,
				TT_C4,
				TT_C4S,
				TT_D4,
				TT_D4S,
				TT_E4,
				TT_F4,
				TT_F4S,
				TT_G4,
				TT_G4S,
				TT_A5,
				TT_A5S,
				TT_B5,
				TT_C5,
				TT_C5S,
				TT_D5,
				TT_D5S,
				TT_E5,
				TT_F5,
				TT_F5S,
				TT_G5,
				TT_G5S,

				TT_FIRST = TT_A1,
				TT_LAST = TT_G5S
			} ToneType;

			typedef struct
			{

			} ToneStatus;

		protected:
			AudioFormat format;
			InstrumentType instType;

		public:
			ToneGenerator(NN<IAudioSource> sourceAudio);
			virtual ~ToneGenerator();

			virtual void GetFormat(NN<AudioFormat> format);
			virtual UOSInt ReadBlock(Data::ByteArray buff); //ret actual block size

			void ClearTones();
			void SetInstrument(InstrumentType instType);

			static Double GetToneFreq(ToneType tone);
			static Text::CString GetToneName(ToneType tone);
		};
	}
}
#endif
