#ifndef _SM_NET_RTPH264HANDLER
#define _SM_NET_RTPH264HANDLER
#include "Net/RTPVPLHandler.h"
#include "IO/MemoryStream.h"
#include "Sync/Mutex.h"

namespace Net
{
	class RTPH264Handler : public RTPVPLHandler
	{
	private:
		Int32 payloadType;
		FrameCallback cb;
		FrameChangeCallback fcCb;
		AnyType cbData;

		UInt32 frameNum;
		Media::FrameInfo frameInfo;
		Int32 packetMode;

		Sync::Mutex mut;
		IO::MemoryStream mstm;
		UInt32 lastSeq;
		Bool missSeq;
		UInt8 *sps;
		UOSInt spsSize;
		UInt8 *pps;
		UOSInt ppsSize;
		Bool isKey;
		Bool firstFrame;


	public:
		RTPH264Handler(Int32 payloadType);
		virtual ~RTPH264Handler();

		virtual void MediaDataReceived(UInt8 *buff, UOSInt dataSize, UInt32 seqNum, UInt32 ts);
		virtual void SetFormat(const UTF8Char *fmtStr);
		virtual Int32 GetPayloadType();

		virtual UTF8Char *GetSourceName(UTF8Char *buff);
		virtual Text::CStringNN GetFilterName();

		virtual Bool GetVideoInfo(NN<Media::FrameInfo> info, OutParam<UInt32> frameRateNorm, OutParam<UInt32> frameRateDenorm, OutParam<UOSInt> maxFrameSize);
		virtual Bool Init(FrameCallback cb, FrameChangeCallback fcCb, AnyType userData);
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
		virtual void EnumFrameInfos(FrameInfoCallback cb, AnyType userData);

		virtual UOSInt ReadNextFrame(UInt8 *frameBuff, UInt32 *frameTime, Media::FrameType *ftype); //ret 0 = no more frames

	};
}
#endif
