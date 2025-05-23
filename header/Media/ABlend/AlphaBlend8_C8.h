#ifndef _SM_MEDIA_ABLEND_ALPHABLEND8_C8
#define _SM_MEDIA_ABLEND_ALPHABLEND8_C8
#include "AnyType.h"
#include "Data/ArrayListNN.h"
#include "Media/ColorProfile.h"
#include "Media/ColorSess.h"
#include "Media/ImageAlphaBlend.h"
#include "Sync/Event.h"
#include "Sync/Mutex.h"

namespace Media
{
	namespace ABlend
	{
		class AlphaBlend8_C8 : public Media::ImageAlphaBlend, public Media::ColorHandler
		{
		private:
			typedef struct
			{
				NN<AlphaBlend8_C8> me;
				UOSInt index;
				NN<Sync::Event> evt;
				Int32 status; //0 = not running, 1 = running/idle, 2 = toStop, 3 = stopped, 4 = Blend, 5 = BlendPA

				UnsafeArray<UInt8> dest;
				OSInt dbpl;
				UnsafeArray<const UInt8> src;
				OSInt sbpl;
				UOSInt width;
				UOSInt height;
			} ThreadStat;

			class LUTInfo
			{
			public:
				Media::ColorProfile sProfile;
				Media::ColorProfile dProfile;
				Media::ColorProfile oProfile;
				UnsafeArray<UInt8> rgbTable;
			};
		private:
			Optional<Data::ArrayListNN<LUTInfo>> lutList;
			UnsafeArray<UInt8> rgbTable;
			Optional<Media::ColorSess> colorSess;
			UnsafeArray<ThreadStat> stats;
			UOSInt threadCnt;
			Sync::Mutex mut;
			Sync::Event mainEvt;
			
			void MTBlend(UnsafeArray<UInt8> dest, OSInt dbpl, UnsafeArray<const UInt8> src, OSInt sbpl, UOSInt width, UOSInt height);
			void MTBlendPA(UnsafeArray<UInt8> dest, OSInt dbpl, UnsafeArray<const UInt8> src, OSInt sbpl, UOSInt width, UOSInt height);

			void UpdateLUT();
			static UInt32 __stdcall ProcessThread(AnyType userObj);
		public:
			AlphaBlend8_C8(Optional<Media::ColorSess> colorSess, Bool multiProfile);
			virtual ~AlphaBlend8_C8();

			virtual void Blend(UnsafeArray<UInt8> dest, OSInt dbpl, UnsafeArray<const UInt8> src, OSInt sbpl, UOSInt width, UOSInt height, Media::AlphaType srcAType);
			virtual void PremulAlpha(UnsafeArray<UInt8> dest, OSInt dbpl, UnsafeArray<const UInt8> src, OSInt sbpl, UOSInt width, UOSInt height);

			virtual void YUVParamChanged(NN<const Media::ColorHandler::YUVPARAM> yuvParam);
			virtual void RGBParamChanged(NN<const Media::ColorHandler::RGBPARAM2> rgbParam);

			void SetColorSess(Optional<Media::ColorSess> colorSess);
			void EndColorSess(NN<Media::ColorSess> colorSess);
		};
	}
}
#endif
