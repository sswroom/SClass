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
		NWASource(NN<IO::StreamData> fd, UInt32 sampleCount, UInt32 blockSize, UInt32 compLevel, UInt32 nBlocks, NN<const Media::AudioFormat> format, NN<Text::String> name);
		NWASource(NN<IO::StreamData> fd, UInt32 sampleCount, UInt32 blockSize, UInt32 compLevel, UInt32 nBlocks, NN<const Media::AudioFormat> format, Text::CStringNN name);
		virtual ~NWASource();

		virtual Data::Duration GetStreamTime();
		virtual Data::Duration SeekToTime(Data::Duration time);
		virtual UIntOS ReadBlock(Data::ByteArray buff); //ret actual block size
		virtual UIntOS GetMinBlockSize();
		virtual Data::Duration GetCurrTime();
	};
}
#endif
