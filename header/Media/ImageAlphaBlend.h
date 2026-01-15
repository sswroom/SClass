#ifndef _SM_MEDIA_IMAGEALPHABLEND
#define _SM_MEDIA_IMAGEALPHABLEND
#include "Media/FrameInfo.h"

namespace Media
{
	class ImageAlphaBlend
	{
	protected:
		Media::ColorProfile sProfile;
		Media::ColorProfile dProfile;
		Media::ColorProfile oProfile;
		Bool changed;

		ImageAlphaBlend();
	public:
		virtual ~ImageAlphaBlend();

		void SetSourceProfile(NN<const Media::ColorProfile> sProfile);
		void SetDestProfile(NN<const Media::ColorProfile> dProfile);
		void SetOutputProfile(NN<const Media::ColorProfile> oProfile);

//		static void Add(UInt8 *dest, IntOS dbpl, UInt8 *src, IntOS sbpl, IntOS width, IntOS height);
//		static void Subtract(UInt8 *dest, IntOS dbpl, UInt8 *src, IntOS sbpl, IntOS width, IntOS height);

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
		virtual void Blend(UnsafeArray<UInt8> dest, IntOS dbpl, UnsafeArray<const UInt8> src, IntOS sbpl, UIntOS width, UIntOS height, Media::AlphaType srcAType) = 0;
		virtual void PremulAlpha(UnsafeArray<UInt8> dest, IntOS dbpl, UnsafeArray<const UInt8> src, IntOS sbpl, UIntOS width, UIntOS height) = 0;
	};
}
#endif
