#ifndef _SM_NET_RTPASOURCE
#define _SM_NET_RTPASOURCE
#include "Media/IAudioSource.h"
#include "Net/RTPCliChannel.h"
#include "Net/RTPAPLHandler.h"

namespace Net
{
	class RTPASource : public Media::IAudioSource
	{
	private:
		NotNullPtr<Net::RTPCliChannel> ch;
		NotNullPtr<Net::RTPAPLHandler> hdlr;
	public:
		RTPASource(NotNullPtr<Net::RTPCliChannel> ch, NotNullPtr<Net::RTPAPLHandler> hdlr);
		virtual ~RTPASource();

		virtual UTF8Char *GetSourceName(UTF8Char *buff);
		virtual Bool CanSeek();
		virtual Data::Duration GetStreamTime();
		virtual Data::Duration SeekToTime(Data::Duration time);
		virtual Bool TrimStream(UInt32 trimTimeStart, UInt32 trimTimeEnd, Int32 *syncTime);

		virtual void GetFormat(NotNullPtr<Media::AudioFormat> format);

		virtual Bool Start(Sync::Event *evt, UOSInt blkSize);
		virtual void Stop();
		virtual UOSInt ReadBlock(Data::ByteArray blk);
		virtual UOSInt GetMinBlockSize();
		virtual Data::Duration GetCurrTime();
		virtual Bool IsEnd();
	};
}
#endif
