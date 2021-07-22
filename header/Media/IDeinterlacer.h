#ifndef _SM_MEDIA_IDEINTERLACER
#define _SM_MEDIA_IDEINTERLACER

namespace Media
{
	class IDeinterlacer
	{
	public:
		virtual ~IDeinterlacer(){};

		virtual void Reinit(UOSInt fieldCnt, UOSInt fieldSep) = 0;
		virtual void Deinterlace(UInt8 *src, UInt8 *dest, Bool bottomField, UOSInt width, OSInt dstep) = 0;
	};
}
#endif
