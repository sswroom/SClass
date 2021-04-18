#ifndef _SM_MEDIA_ABLEND_ALPHABLEND8_C8
#define _SM_MEDIA_ABLEND_ALPHABLEND8_C8
#include "Data/ArrayList.h"
#include "Media/ColorProfile.h"
#include "Media/ColorSess.h"
#include "Media/ImageAlphaBlend.h"
#include "Sync/Event.h"
#include "Sync/Mutex.h"

namespace Media
{
	namespace ABlend
	{
		class AlphaBlend8_C8 : public Media::ImageAlphaBlend, public Media::IColorHandler
		{
		private:
			typedef struct
			{
				AlphaBlend8_C8 *me;
				Sync::Event *evt;
				Int32 status; //0 = not running, 1 = running/idle, 2 = toStop, 3 = stopped, 4 = Blend, 5 = BlendPA

				UInt8 *dest;
				OSInt dbpl;
				const UInt8 *src;
				OSInt sbpl;
				UOSInt width;
				UOSInt height;
			} ThreadStat;

			typedef struct
			{
				Media::ColorProfile *sProfile;
				Media::ColorProfile *dProfile;
				Media::ColorProfile *oProfile;
				UInt8 *rgbTable;
			} LUTInfo;
		private:
			Data::ArrayList<LUTInfo*> *lutList;
			UInt8 *rgbTable;
			Media::ColorSess *colorSess;
			ThreadStat *stats;
			UOSInt threadCnt;
			Sync::Mutex *mut;
			Sync::Event *mainEvt;
			
			void MTBlend(UInt8 *dest, OSInt dbpl, const UInt8 *src, OSInt sbpl, UOSInt width, UOSInt height);
			void MTBlendPA(UInt8 *dest, OSInt dbpl, const UInt8 *src, OSInt sbpl, UOSInt width, UOSInt height);

			void UpdateLUT();
			static UInt32 __stdcall ProcessThread(void *userObj);
		public:
			AlphaBlend8_C8(Media::ColorSess *colorSess, Bool multiProfile);
			virtual ~AlphaBlend8_C8();

			virtual void Blend(UInt8 *dest, OSInt dbpl, const UInt8 *src, OSInt sbpl, UOSInt width, UOSInt height, Media::AlphaType srcAType);
			virtual void PremulAlpha(UInt8 *dest, OSInt dbpl, const UInt8 *src, OSInt sbpl, UOSInt width, UOSInt height);

			virtual void YUVParamChanged(const Media::IColorHandler::YUVPARAM *yuvParam);
			virtual void RGBParamChanged(const Media::IColorHandler::RGBPARAM2 *rgbParam);
		};
	}
}
#endif
