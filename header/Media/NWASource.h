#ifndef _SM_MEDIA_NWASOURCE
#define _SM_MEDIA_NWASOURCE
#include "IO/IStreamData.h"
#include "Media/LPCMSource.h"

namespace Media
{
	class NWASource : public LPCMSource
	{
	private:
		UInt32 currBlock;
		UInt32 sampleCount;
		UInt32 blockSize;
		UInt32 nBlocks;
		UInt32 compLevel;
		UInt32 *blockOfsts;
		UInt8 *blockBuff;

	public:
		NWASource(IO::IStreamData *fd, UInt32 sampleCount, UInt32 blockSize, UInt32 compLevel, UInt32 nBlocks, Media::AudioFormat *format, Text::String *name);
		NWASource(IO::IStreamData *fd, UInt32 sampleCount, UInt32 blockSize, UInt32 compLevel, UInt32 nBlocks, Media::AudioFormat *format, const UTF8Char *name);
		virtual ~NWASource();

		virtual Int32 GetStreamTime(); //ms
		virtual UInt32 SeekToTime(UInt32 time); //ms, ret actual time
		virtual UOSInt ReadBlock(UInt8 *buff, UOSInt blkSize); //ret actual block size
		virtual UOSInt GetMinBlockSize();
		virtual UInt32 GetCurrTime();
	};
}
#endif
