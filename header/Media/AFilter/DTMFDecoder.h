#ifndef _SM_MEDIA_AFILTER_DTMFDECODER
#define _SM_MEDIA_AFILTER_DTMFDECODER
#include "AnyType.h"
#include "Media/AudioFilter.h"
#include "Sync/Event.h"
#include "Sync/Mutex.h"
#include "Sync/Thread.h"

namespace Media
{
	namespace AFilter
	{
		class DTMFDecoder : public AudioFilter
		{
		public:
			typedef void (CALLBACKFUNC ToneChangeEvent)(AnyType userObj, WChar newTone);
		private:
			ToneChangeEvent toneChgHdlr;
			AnyType toneChgObj;

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
			static void __stdcall CalcThread(NN<Sync::Thread> thread);
			void ResetStatus();
		public:
			DTMFDecoder(NN<Media::AudioSource> audSrc, UOSInt calcInt);
			virtual ~DTMFDecoder();

			virtual Data::Duration SeekToTime(Data::Duration time);
			virtual UOSInt ReadBlock(Data::ByteArray blk); //ret actual block size

			void HandleToneChange(ToneChangeEvent hdlr, AnyType userObj);
		};
	}
}
#endif
