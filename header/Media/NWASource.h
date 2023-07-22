#ifndef _SM_MEDIA_NWASOURCE
#define _SM_MEDIA_NWASOURCE
#include "IO/StreamData.h"
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
		NWASource(NotNullPtr<IO::StreamData> fd, UInt32 sampleCount, UInt32 blockSize, UInt32 compLevel, UInt32 nBlocks, Media::AudioFormat *format, NotNullPtr<Text::String> name);
		NWASource(NotNullPtr<IO::StreamData> fd, UInt32 sampleCount, UInt32 blockSize, UInt32 compLevel, UInt32 nBlocks, Media::AudioFormat *format, Text::CString name);
		virtual ~NWASource();

		virtual Int32 GetStreamTime(); //ms
		virtual UInt32 SeekToTime(UInt32 time); //ms, ret actual time
		virtual UOSInt ReadBlock(Data::ByteArray buff); //ret actual block size
		virtual UOSInt GetMinBlockSize();
		virtual UInt32 GetCurrTime();
	};
}
#endif
