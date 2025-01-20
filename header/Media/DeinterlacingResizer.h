#ifndef _SM_MEDIA_DEINTERLACINGRESIZER
#define _SM_MEDIA_DEINTERLACINGRESIZER
#include "Media/StaticImage.h"

namespace Media
{
	class DeinterlacingResizer
	{
	public:
		typedef enum
		{
			DT_FULL_FRAME,
			DT_TOP_FIELD,
			DT_BOTTOM_FIELD
		} DeintType;

	protected:
		Media::AlphaType srcAlphaType;
	public:
		DeinterlacingResizer(Media::AlphaType srcAlphaType);
		virtual ~DeinterlacingResizer(){};

		void SetSrcAlphaType(Media::AlphaType alphaType);

		virtual void DeintResize(DeintType dType, UnsafeArray<UInt8> src, UOSInt sbpl, Double swidth, Double sheight, UnsafeArray<UInt8> dest, UOSInt dbpl, UOSInt dwidth, UOSInt dheight, Bool upsideDown) = 0;
		virtual void SetDISrcRefLuminance(Double srcRefLuminance);
	};
}
#endif
