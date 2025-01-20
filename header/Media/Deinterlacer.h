#ifndef _SM_MEDIA_DEINTERLACER
#define _SM_MEDIA_DEINTERLACER

namespace Media
{
	class Deinterlacer
	{
	public:
		virtual ~Deinterlacer(){};

		virtual void Reinit(UOSInt fieldCnt, UOSInt fieldSep) = 0;
		virtual void Deinterlace(UnsafeArray<UInt8> src, UnsafeArray<UInt8> dest, Bool bottomField, UOSInt width, OSInt dstep) = 0;
	};
}
#endif
