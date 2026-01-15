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
		NN<Net::RTPCliChannel> ch;
		NN<Net::RTPVPLHandler> hdlr;
	public:
		RTPVSource(NN<Net::RTPCliChannel> ch, NN<Net::RTPVPLHandler> hdlr);
		virtual ~RTPVSource();

		virtual UnsafeArrayOpt<UTF8Char> GetSourceName(UnsafeArray<UTF8Char> buff);
		virtual Text::CStringNN GetFilterName();

		virtual Bool GetVideoInfo(NN<Media::FrameInfo> info, OutParam<UInt32> frameRateNorm, OutParam<UInt32> frameRateDenorm, OutParam<UIntOS> maxFrameSize);
		virtual Bool Init(FrameCallback cb, FrameChangeCallback fcCb, AnyType userData);
		virtual Bool Start(); //true = succeed
		virtual void Stop();
		virtual Bool IsRunning();

		virtual Data::Duration GetStreamTime();
		virtual Bool CanSeek();
		virtual Data::Duration SeekToTime(Data::Duration time);
		virtual Bool IsRealTimeSrc();
		virtual Bool TrimStream(UInt32 trimTimeStart, UInt32 trimTimeEnd, OptOut<Int32> syncTime);

		virtual UIntOS GetDataSeekCount();

		virtual Bool HasFrameCount();
		virtual UIntOS GetFrameCount();
		virtual Data::Duration GetFrameTime(UIntOS frameIndex);
		virtual void EnumFrameInfos(FrameInfoCallback cb, AnyType userData);

		virtual UIntOS ReadNextFrame(UnsafeArray<UInt8> frameBuff, OutParam<UInt32> frameTime, OutParam<Media::FrameType> ftype); //ret 0 = no more frames
	};
}
#endif
