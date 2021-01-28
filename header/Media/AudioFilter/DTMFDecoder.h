#ifndef _SM_MEDIA_AUDIOFILTER_DTMFDECODER
#define _SM_MEDIA_AUDIOFILTER_DTMFDECODER
#include "Media/IAudioFilter.h"
#include "Sync/Event.h"
#include "Sync/Mutex.h"

namespace Media
{
	namespace AudioFilter
	{
		class DTMFDecoder : public IAudioFilter
		{
		public:
			typedef void (__stdcall *ToneChangeEvent)(void *userObj, WChar newTone);
		private:
			ToneChangeEvent toneChgHdlr;
			void *toneChgObj;

			WChar currTone;
			UInt8 *sampleBuff;
			UOSInt sampleBuffSize;
			UOSInt sampleOfst;
			UOSInt sampleCnt;
			Sync::Mutex *sampleMut;
			UOSInt calcLeft;
			OSInt calcInt;
			UInt8 *calcBuff;
			Bool calcReady;
			Sync::Mutex *calcMut;
			UInt32 nChannels;
			UInt32 bitCount;
			UInt32 align;
			UInt32 frequency;

			Bool threadToStop;
			Bool threadRunning;
			Sync::Event *threadEvt;

		private:
			static UInt32 __stdcall CalcThread(void *userObj);
			void ResetStatus();
		public:
			DTMFDecoder(Media::IAudioSource *audSrc, OSInt calcInt);
			virtual ~DTMFDecoder();

			virtual Int32 SeekToTime(Int32 time); //ms, ret actual time
			virtual UOSInt ReadBlock(UInt8 *buff, UOSInt blkSize); //ret actual block size

			void HandleToneChange(ToneChangeEvent hdlr, void *userObj);
		};
	}
}
#endif
