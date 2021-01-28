#ifndef _SM_MEDIA_IDEINTERLACER
#define _SM_MEDIA_IDEINTERLACER

namespace Media
{
	class IDeinterlacer
	{
	public:
		virtual ~IDeinterlacer(){};

		virtual void Reinit(OSInt fieldCnt, OSInt fieldSep) = 0;
		virtual void Deinterlace(UInt8 *src, UInt8 *dest, OSInt isBottomField, OSInt width, OSInt dstep) = 0;
	};
};
#endif
