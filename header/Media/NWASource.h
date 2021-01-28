#ifndef _SM_MEDIA_NWASOURCE
#define _SM_MEDIA_NWASOURCE
#include "IO/IStreamData.h"
#include "Media/LPCMSource.h"

namespace Media
{
	class NWASource : public LPCMSource
	{
	private:
		Int32 currBlock;
		Int32 sampleCount;
		UInt32 blockSize;
		Int32 nBlocks;
		Int32 compLevel;
		UInt32 *blockOfsts;
		UInt8 *blockBuff;

	public:
		NWASource(IO::IStreamData *fd, Int32 sampleCount, UInt32 blockSize, Int32 compLevel, Int32 nBlocks, Media::AudioFormat *format, const UTF8Char *name);
		virtual ~NWASource();

		virtual Int32 GetStreamTime(); //ms
		virtual Int32 SeekToTime(Int32 time); //ms, ret actual time
		virtual UOSInt ReadBlock(UInt8 *buff, UOSInt blkSize); //ret actual block size
		virtual UOSInt GetMinBlockSize();
		virtual Int32 GetCurrTime();
	};
}
#endif
