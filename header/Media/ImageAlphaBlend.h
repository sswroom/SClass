#ifndef _SM_MEDIA_IMAGEALPHABLEND
#define _SM_MEDIA_IMAGEALPHABLEND
#include "Media/FrameInfo.h"

namespace Media
{
	class ImageAlphaBlend
	{
	protected:
		Media::ColorProfile *sProfile;
		Media::ColorProfile *dProfile;
		Media::ColorProfile *oProfile;
		Bool changed;

		ImageAlphaBlend();
	public:
		virtual ~ImageAlphaBlend();

		void SetSourceProfile(const Media::ColorProfile *sProfile);
		void SetDestProfile(const Media::ColorProfile *dProfile);
		void SetOutputProfile(const Media::ColorProfile *oProfile);

//		static void Add(UInt8 *dest, OSInt dbpl, UInt8 *src, OSInt sbpl, OSInt width, OSInt height);
//		static void Subtract(UInt8 *dest, OSInt dbpl, UInt8 *src, OSInt sbpl, OSInt width, OSInt height);

		/*
		sa16 = (sa << 8) | sa
		sai16 = 65535 - sa16
		da16 = (da << 8) | da
		dai16 = 65535 - da16

		dc = dpc * 65535 / da16
		oc = (dc * sai16 + spc * sa16) / 65535
		oa = sa16 + sai16 * da16 / 65535

		dest should be AT_ALPHA, output should be AT_ALPHA
		*/
		virtual void Blend(UInt8 *dest, OSInt dbpl, const UInt8 *src, OSInt sbpl, UOSInt width, UOSInt height, Media::AlphaType srcAType) = 0;
		virtual void PremulAlpha(UInt8 *dest, OSInt dbpl, const UInt8 *src, OSInt sbpl, UOSInt width, UOSInt height) = 0;
	};
}
#endif
