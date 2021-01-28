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
		Net::RTPCliChannel *ch;
		Net::RTPAPLHandler *hdlr;
	public:
		RTPASource(Net::RTPCliChannel *ch, Net::RTPAPLHandler *hdlr);
		virtual ~RTPASource();

		virtual UTF8Char *GetSourceName(UTF8Char *buff);
		virtual Bool CanSeek();
		virtual Int32 GetStreamTime();
		virtual Int32 SeekToTime(Int32 time);
		virtual Bool TrimStream(Int32 trimTimeStart, Int32 trimTimeEnd, Int32 *syncTime);

		virtual void GetFormat(Media::AudioFormat *format);

		virtual Bool Start(Sync::Event *evt, UOSInt blkSize);
		virtual void Stop();
		virtual UOSInt ReadBlock(UInt8 *buff, UOSInt blkSize);
		virtual UOSInt GetMinBlockSize();
		virtual Int32 GetCurrTime();
		virtual Bool IsEnd();
	};
}
#endif
