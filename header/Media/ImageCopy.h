#ifndef _SM_MEDIA_IMAGECOPY
#define _SM_MEDIA_IMAGECOPY
#include "Sync/Event.h"
#include "Sync/Mutex.h"
#include "Sync/Thread.h"

namespace Media
{
	typedef struct
	{
		Int32 status; // 0 = not running, 1 = waiting, 2 = to exit, 3 = copying, 4 = 4 = setPriority;
		Sync::Event *evt;
		Sync::Event *evtMain;
		UInt8 *inPt;
		UInt8 *outPt;
		OSInt copySize;
		OSInt height;
		OSInt sstep;
		OSInt dstep;
	} IMGCOPYSTAT;

	class ImageCopy
	{
	private:
		Sync::Mutex *mut;
		Sync::Event *evtMain;
		IMGCOPYSTAT *stats;
		UOSInt nThread;

		void MT_Copy(UInt8 *inPt, UInt8 *outPt, OSInt width, OSInt height, OSInt sstep, OSInt dstep);

		static UInt32 __stdcall WorkerThread(void *obj);
	public:
		ImageCopy();
		~ImageCopy();

		void Copy32(UInt8 *src, OSInt sbpl, UInt8 *dest, OSInt dbpl, OSInt dwidth, OSInt dheight);
		void Copy16(UInt8 *src, OSInt sbpl, UInt8 *dest, OSInt dbpl, OSInt dwidth, OSInt dheight);
		void SetThreadPriority(Sync::Thread::ThreadPriority tp);
	};
};

#endif
