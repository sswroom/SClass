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
		NotNullPtr<Net::RTPCliChannel> ch;
		NotNullPtr<Net::RTPVPLHandler> hdlr;
	public:
		RTPVSource(NotNullPtr<Net::RTPCliChannel> ch, NotNullPtr<Net::RTPVPLHandler> hdlr);
		virtual ~RTPVSource();

		virtual UTF8Char *GetSourceName(UTF8Char *buff);
		virtual Text::CStringNN GetFilterName();

		virtual Bool GetVideoInfo(NotNullPtr<Media::FrameInfo> info, OutParam<UInt32> frameRateNorm, OutParam<UInt32> frameRateDenorm, OutParam<UOSInt> maxFrameSize);
		virtual Bool Init(FrameCallback cb, FrameChangeCallback fcCb, void *userData);
		virtual Bool Start(); //true = succeed
		virtual void Stop();
		virtual Bool IsRunning();

		virtual Data::Duration GetStreamTime();
		virtual Bool CanSeek();
		virtual Data::Duration SeekToTime(Data::Duration time);
		virtual Bool IsRealTimeSrc();
		virtual Bool TrimStream(UInt32 trimTimeStart, UInt32 trimTimeEnd, Int32 *syncTime);

		virtual UOSInt GetDataSeekCount();

		virtual Bool HasFrameCount();
		virtual UOSInt GetFrameCount();
		virtual Data::Duration GetFrameTime(UOSInt frameIndex);
		virtual void EnumFrameInfos(FrameInfoCallback cb, void *userData);

		virtual UOSInt ReadNextFrame(UInt8 *frameBuff, UInt32 *frameTime, Media::FrameType *ftype); //ret 0 = no more frames
	};
}
#endif
