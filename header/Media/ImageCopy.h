#ifndef _SM_MEDIA_IMAGECOPY
#define _SM_MEDIA_IMAGECOPY
#include "AnyType.h"
#include "Sync/Event.h"
#include "Sync/Mutex.h"
#include "Sync/ThreadUtil.h"

namespace Media
{
	typedef struct
	{
		Int32 status; // 0 = not running, 1 = waiting, 2 = to exit, 3 = copying, 4 = 4 = setPriority;
		NN<Sync::Event> evt;
		NN<Sync::Event> evtMain;
		UnsafeArray<const UInt8> inPt;
		UnsafeArray<UInt8> outPt;
		UIntOS copySize;
		UIntOS height;
		IntOS sstep;
		IntOS dstep;
	} IMGCOPYSTAT;

	class ImageCopy
	{
	private:
		Sync::Event evtMain;
		UnsafeArray<IMGCOPYSTAT> stats;
		UIntOS nThread;

		void MT_Copy(UnsafeArray<const UInt8> inPt, UnsafeArray<UInt8> outPt, UIntOS width, UIntOS height, IntOS sstep, IntOS dstep);

		static UInt32 __stdcall WorkerThread(AnyType obj);
	public:
		ImageCopy();
		~ImageCopy();

		void Copy32(UnsafeArray<const UInt8> src, IntOS sbpl, UnsafeArray<UInt8> dest, IntOS dbpl, UIntOS dwidth, UIntOS dheight);
		void Copy16(UnsafeArray<const UInt8> src, IntOS sbpl, UnsafeArray<UInt8> dest, IntOS dbpl, UIntOS dwidth, UIntOS dheight);
		void SetThreadPriority(Sync::ThreadUtil::ThreadPriority tp);
	};
}

#endif
