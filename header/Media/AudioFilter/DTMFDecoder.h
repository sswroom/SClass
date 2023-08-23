#ifndef _SM_MEDIA_AUDIOFILTER_DTMFDECODER
#define _SM_MEDIA_AUDIOFILTER_DTMFDECODER
#include "Media/IAudioFilter.h"
#include "Sync/Event.h"
#include "Sync/Mutex.h"
#include "Sync/Thread.h"

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
			Sync::Mutex sampleMut;
			UOSInt calcLeft;
			UOSInt calcInt;
			UInt8 *calcBuff;
			Bool calcReady;
			Sync::Mutex calcMut;
			UInt32 nChannels;
			UInt32 bitCount;
			UInt32 align;
			UInt32 frequency;
			Sync::Thread thread;

		private:
			static void __stdcall CalcThread(NotNullPtr<Sync::Thread> thread);
			void ResetStatus();
		public:
			DTMFDecoder(Media::IAudioSource *audSrc, UOSInt calcInt);
			virtual ~DTMFDecoder();

			virtual UInt32 SeekToTime(UInt32 time); //ms, ret actual time
			virtual UOSInt ReadBlock(Data::ByteArray blk); //ret actual block size

			void HandleToneChange(ToneChangeEvent hdlr, void *userObj);
		};
	}
}
#endif
