#ifndef _SM_MEDIA_DEINTERLACER
#define _SM_MEDIA_DEINTERLACER

namespace Media
{
	class Deinterlacer
	{
	public:
		virtual ~Deinterlacer(){};

		virtual void Reinit(UIntOS fieldCnt, UIntOS fieldSep) = 0;
		virtual void Deinterlace(UnsafeArray<UInt8> src, UnsafeArray<UInt8> dest, Bool bottomField, UIntOS width, IntOS dstep) = 0;
	};
}
#endif
