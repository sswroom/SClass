#ifndef _SM_NET_RTPVSOURCE
#define _SM_NET_RTPVSOURCE
#include "Media/VideoSourceBase.h"
#include "Net/RTPCliChannel.h"
#include "Net/RTPVPLHandler.h"

namespace Net
{
	class RTPVSource : public Media::VideoSourceBase
	{
	private:
		Net::RTPCliChannel *ch;
		Net::RTPVPLHandler *hdlr;
	public:
		RTPVSource(Net::RTPCliChannel *ch, Net::RTPVPLHandler *hdlr);
		virtual ~RTPVSource();

		virtual UTF8Char *GetSourceName(UTF8Char *buff);
		virtual const UTF8Char *GetFilterName();

		virtual Bool GetVideoInfo(Media::FrameInfo *info, Int32 *frameRateNorm, Int32 *frameRateDenorm, UOSInt *maxFrameSize);
		virtual Bool Init(FrameCallback cb, FrameChangeCallback fcCb, void *userData);
		virtual Bool Start(); //true = succeed
		virtual void Stop();
		virtual Bool IsRunning();

		virtual Int32 GetStreamTime(); //ms, -1 = infinity
		virtual Bool CanSeek();
		virtual UInt32 SeekToTime(UInt32 time); //ms, ret actual time
		virtual Bool IsRealTimeSrc();
		virtual Bool TrimStream(UInt32 trimTimeStart, UInt32 trimTimeEnd, Int32 *syncTime);

		virtual OSInt GetDataSeekCount();

		virtual OSInt GetFrameCount();
		virtual UInt32 GetFrameTime(UOSInt frameIndex);
		virtual void EnumFrameInfos(FrameInfoCallback cb, void *userData);

		virtual OSInt ReadNextFrame(UInt8 *frameBuff, Int32 *frameTime, Media::FrameType *ftype); //ret 0 = no more frames
	};
}
#endif
