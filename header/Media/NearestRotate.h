#ifndef _SM_MEDIA_NEARESTROTATE
#define _SM_MEDIA_NEARESTROTATE
#include "Media/ImageRotater.h"

namespace Media
{
	typedef struct
	{
		Int32 status; // 0 = not running, 1 = waiting, 2 = to exit, 3 = rotating, 4 = end rotating
		Sync::Event *evt;
		Single srcWidth;
		Single srcHeight;
		Int32 srcIWidth;
		Int32 destWidth;
		Int32 destStartY;
		Int32 destEndY;
		Single ofstCorrX;
		Single ofstCorrY;
		Single centerX;
		Single centerY;
		Single sinAngle;
		Single cosAngle;
		UnsafeArray<Int32> sBits;
		UnsafeArray<Int32> pBits;
	} NROT_THREADSTAT;

	class NearestRotate : public ImageRotater
	{
	private:
		Int32 currId;
		Sync::Event evtMain;
		UnsafeArray<NROT_THREADSTAT> stats;
		UIntOS nThread;

		static void RotateTask(NN<NROT_THREADSTAT> stat);
		static UInt32 WorkerThread(AnyType obj);

	public:
		NearestRotate();
		virtual ~NearestRotate();

		virtual Optional<Media::RasterImage> Rotate(NN<Media::RasterImage> srcImg, Single centerX, Single centerY, Single angleRad, Bool keepCoord, Bool keepSize);
	};
}

#endif
