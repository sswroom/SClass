#ifndef _SM_IO_IPROGRESSHANDLER
#define _SM_IO_IPROGRESSHANDLER

namespace IO
{
	class IProgressHandler
	{
	public:
		virtual void ProgressStart(const UTF8Char *name, UInt64 count) = 0;
		virtual void ProgressUpdate(UInt64 currCount, UInt64 newCount) = 0;
		virtual void ProgressEnd() = 0;
	};
}
#endif
