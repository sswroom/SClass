#ifndef _SM_MEDIA_M2VSTREAMSOURCE
#define _SM_MEDIA_M2VSTREAMSOURCE
#include "IO/Writer.h"
#include "Media/IMediaStream.h"
#include "Media/IStreamControl.h"
#include "Media/VideoSourceBase.h"
#include "Sync/Mutex.h"

namespace IO
{
	class Stream;
}

namespace Media
{
	class M2VStreamSource : public Media::VideoSourceBase, public Media::IMediaStream
	{
	private:
		typedef struct
		{
			UInt8 *frame;
			UInt32 frameTime;
			UOSInt frameSize;
			OSInt pictureStart;
			UOSInt frameNum;
		} FrameBuff;

		Media::IStreamControl *pbc;
		FrameCallback frameCb;
		FrameChangeCallback fcCb;
		void *frameCbData;
		Media::FrameInfo info;
		UInt32 frameRateNorm;
		UInt32 frameRateDenorm;
		UOSInt maxFrameSize;

		UInt64 totalFrameSize;
		UInt32 totalFrameCnt;

		UInt8 *frameBuff;
		UOSInt frameBuffSize;
		UInt32 thisFrameTime;
		UInt32 syncFrameTime;
		UOSInt syncFieldCnt;
		Bool firstFrame;
		UOSInt frameStart;
		UOSInt frameNum;
		Int32 writeCnt;

		Bool playing;
		Bool playInit;
		Bool playToStop;
		Bool playEOF;
		Sync::Event playEvt;
		Sync::Mutex playMut;
		Sync::Mutex pbcMut;
		FrameBuff *playBuff;
		OSInt playBuffStart;
		OSInt playBuffEnd;
		Double par;
		UInt64 bitRate;

		Bool finfoMode;
		void *finfoData;
		FrameInfoCallback finfoCb;

		IO::Stream *debugFS;
		IO::Writer *debugLog;
		Sync::Mutex *debugMut;

		void SubmitFrame(UOSInt frameSize, UOSInt frameStart, UOSInt pictureStart);
		void ClearPlayBuff();
		static UInt32 __stdcall PlayThread(void *userObj);
	public:
		M2VStreamSource(Media::IStreamControl *pbc);
		virtual ~M2VStreamSource();

		virtual UTF8Char *GetSourceName(UTF8Char *buff);
		virtual Text::CString GetFilterName();

		virtual Bool GetVideoInfo(NotNullPtr<Media::FrameInfo> info, OutParam<UInt32> frameRateNorm, OutParam<UInt32> frameRateDenorm, OutParam<UOSInt> maxFrameSize);
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
		virtual UOSInt GetFrameSize(UOSInt frameIndex);
		virtual UOSInt ReadFrame(UOSInt frameIndex, UInt8 *buff);

		virtual UOSInt ReadNextFrame(UInt8 *frameBuff, UInt32 *frameTime, Media::FrameType *ftype); //ret 0 = no more frames

		virtual void DetectStreamInfo(UInt8 *header, UOSInt headerSize);
		virtual void ClearFrameBuff();
		virtual void SetStreamTime(UInt32 time);
		virtual void WriteFrameStream(UInt8 *buff, UOSInt buffSize);
		virtual Int32 GetFrameStreamTime();
		virtual void EndFrameStream();
		virtual UInt64 GetBitRate();
	};
}
#endif
