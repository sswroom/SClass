#ifndef _SM_MEDIA_NEARESTROTATE
#define _SM_MEDIA_NEARESTROTATE
#include "Media/IImgRotate.h"

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
		Int32 *sBits;
		Int32 *pBits;
	} NROT_THREADSTAT;

	class NearestRotate : public IImgRotate
	{
	private:
		Int32 currId;
		Sync::Event *evtMain;
		NROT_THREADSTAT *stats;
		Int32 nThread;

		static void RotateTask(NROT_THREADSTAT *stat);
		static UInt32 WorkerThread(void *obj);

	public:
		NearestRotate();
		virtual ~NearestRotate();

		virtual Media::Image *Rotate(Media::Image *srcImg, Single centerX, Single centerY, Single angleRad, Bool keepCoord, Bool keepSize);
	};
};

#endif
