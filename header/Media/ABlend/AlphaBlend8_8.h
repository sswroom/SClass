#ifndef _SM_MEDIA_ABLEND_ALPHABLEND8_8
#define _SM_MEDIA_ABLEND_ALPHABLEND8_8
#include "AnyType.h"
#include "Media/ImageAlphaBlend.h"
#include "Sync/Event.h"
#include "Sync/Mutex.h"

namespace Media
{
	namespace ABlend
	{
		class AlphaBlend8_8 : public Media::ImageAlphaBlend
		{
		private:
			typedef struct
			{
				NN<AlphaBlend8_8> me;
				NN<Sync::Event> evt;
				Int32 status; //0 = not running, 1 = running/idle, 2 = toStop, 3 = stopped, 4 = Blend, 5 = BlendPA

				UnsafeArray<UInt8> dest;
				OSInt dbpl;
				UnsafeArray<const UInt8> src;
				OSInt sbpl;
				UOSInt width;
				UOSInt height;
			} ThreadStat;
		private:
			UnsafeArray<ThreadStat> stats;
			UOSInt threadCnt;
			Sync::Mutex mut;
			Sync::Event mainEvt;
			
			void MTBlend(UnsafeArray<UInt8> dest, OSInt dbpl, UnsafeArray<const UInt8> src, OSInt sbpl, UOSInt width, UOSInt height);
			void MTBlendPA(UnsafeArray<UInt8> dest, OSInt dbpl, UnsafeArray<const UInt8> src, OSInt sbpl, UOSInt width, UOSInt height);

			static UInt32 __stdcall ProcessThread(AnyType userObj);
		public:
			AlphaBlend8_8();
			virtual ~AlphaBlend8_8();

			virtual void Blend(UnsafeArray<UInt8> dest, OSInt dbpl, UnsafeArray<const UInt8> src, OSInt sbpl, UOSInt width, UOSInt height, Media::AlphaType srcAType);
			virtual void PremulAlpha(UnsafeArray<UInt8> dest, OSInt dbpl, UnsafeArray<const UInt8> src, OSInt sbpl, UOSInt width, UOSInt height);
		};
	}
}
#endif
