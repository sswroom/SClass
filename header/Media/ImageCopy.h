#ifndef _SM_MEDIA_IMAGECOPY
#define _SM_MEDIA_IMAGECOPY
#include "Sync/Event.h"
#include "Sync/Mutex.h"
#include "Sync/ThreadUtil.h"

namespace Media
{
	typedef struct
	{
		Int32 status; // 0 = not running, 1 = waiting, 2 = to exit, 3 = copying, 4 = 4 = setPriority;
		Sync::Event *evt;
		Sync::Event *evtMain;
		UInt8 *inPt;
		UInt8 *outPt;
		UOSInt copySize;
		UOSInt height;
		OSInt sstep;
		OSInt dstep;
	} IMGCOPYSTAT;

	class ImageCopy
	{
	private:
		Sync::Event evtMain;
		IMGCOPYSTAT *stats;
		UOSInt nThread;

		void MT_Copy(UInt8 *inPt, UInt8 *outPt, UOSInt width, UOSInt height, OSInt sstep, OSInt dstep);

		static UInt32 __stdcall WorkerThread(void *obj);
	public:
		ImageCopy();
		~ImageCopy();

		void Copy32(UInt8 *src, OSInt sbpl, UInt8 *dest, OSInt dbpl, UOSInt dwidth, UOSInt dheight);
		void Copy16(UInt8 *src, OSInt sbpl, UInt8 *dest, OSInt dbpl, UOSInt dwidth, UOSInt dheight);
		void SetThreadPriority(Sync::ThreadUtil::ThreadPriority tp);
	};
}

#endif
