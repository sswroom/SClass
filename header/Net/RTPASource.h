#ifndef _SM_NET_RTPASOURCE
#define _SM_NET_RTPASOURCE
#include "Media/AudioSource.h"
#include "Net/RTPCliChannel.h"
#include "Net/RTPAPLHandler.h"

namespace Net
{
	class RTPASource : public Media::AudioSource
	{
	private:
		NN<Net::RTPCliChannel> ch;
		NN<Net::RTPAPLHandler> hdlr;
	public:
		RTPASource(NN<Net::RTPCliChannel> ch, NN<Net::RTPAPLHandler> hdlr);
		virtual ~RTPASource();

		virtual UnsafeArrayOpt<UTF8Char> GetSourceName(UnsafeArray<UTF8Char> buff);
		virtual Bool CanSeek();
		virtual Data::Duration GetStreamTime();
		virtual Data::Duration SeekToTime(Data::Duration time);
		virtual Bool TrimStream(UInt32 trimTimeStart, UInt32 trimTimeEnd, OptOut<Int32> syncTime);

		virtual void GetFormat(NN<Media::AudioFormat> format);

		virtual Bool Start(Optional<Sync::Event> evt, UOSInt blkSize);
		virtual void Stop();
		virtual UOSInt ReadBlock(Data::ByteArray blk);
		virtual UOSInt GetMinBlockSize();
		virtual Data::Duration GetCurrTime();
		virtual Bool IsEnd();
	};
}
#endif
