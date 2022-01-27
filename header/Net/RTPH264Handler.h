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
		void *cbData;

		UInt32 frameNum;
		Media::FrameInfo *frameInfo;
		Int32 packetMode;

		Sync::Mutex *mut;
		IO::MemoryStream *mstm;
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
		virtual Text::CString GetFilterName();

		virtual Bool GetVideoInfo(Media::FrameInfo *info, UInt32 *frameRateNorm, UInt32 *frameRateDenorm, UOSInt *maxFrameSize);
		virtual Bool Init(FrameCallback cb, FrameChangeCallback fcCb, void *userData);
		virtual Bool Start(); //true = succeed
		virtual void Stop();
		virtual Bool IsRunning();

		virtual Int32 GetStreamTime(); //ms, -1 = infinity
		virtual Bool CanSeek();
		virtual UInt32 SeekToTime(UInt32 time); //ms, ret actual time
		virtual Bool IsRealTimeSrc();
		virtual Bool TrimStream(UInt32 trimTimeStart, UInt32 trimTimeEnd, Int32 *syncTime);

		virtual UOSInt GetDataSeekCount();

		virtual Bool HasFrameCount();
		virtual UOSInt GetFrameCount();
		virtual UInt32 GetFrameTime(UOSInt frameIndex);
		virtual void EnumFrameInfos(FrameInfoCallback cb, void *userData);

		virtual UOSInt ReadNextFrame(UInt8 *frameBuff, UInt32 *frameTime, Media::FrameType *ftype); //ret 0 = no more frames

	};
}
#endif
