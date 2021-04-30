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
			UInt32 frameNum;
		} FrameBuff;

		Media::IStreamControl *pbc;
		FrameCallback frameCb;
		FrameChangeCallback fcCb;
		void *frameCbData;
		Media::FrameInfo *info;
		Int32 frameRateNorm;
		Int32 frameRateDenorm;
		UOSInt maxFrameSize;

		UInt64 totalFrameSize;
		UInt32 totalFrameCnt;

		UInt8 *frameBuff;
		UOSInt frameBuffSize;
		Int32 thisFrameTime;
		Int32 syncFrameTime;
		UOSInt syncFieldCnt;
		Bool firstFrame;
		OSInt frameStart;
		Int32 frameNum;
		Int32 writeCnt;

		Bool playing;
		Bool playInit;
		Bool playToStop;
		Bool playEOF;
		Sync::Event *playEvt;
		Sync::Mutex *playMut;
		Sync::Mutex *pbcMut;
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

		void SubmitFrame(OSInt frameSize, OSInt frameStart, OSInt pictureStart);
		void ClearPlayBuff();
		static UInt32 __stdcall PlayThread(void *userObj);
	public:
		M2VStreamSource(Media::IStreamControl *pbc);
		virtual ~M2VStreamSource();

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

		virtual OSInt GetFrameCount(); //-1 = unknown;
		virtual UInt32 GetFrameTime(UOSInt frameIndex);
		virtual void EnumFrameInfos(FrameInfoCallback cb, void *userData);
		virtual UOSInt GetFrameSize(UOSInt frameIndex);
		virtual UOSInt ReadFrame(UOSInt frameIndex, UInt8 *buff);

		virtual OSInt ReadNextFrame(UInt8 *frameBuff, Int32 *frameTime, Media::FrameType *ftype); //ret 0 = no more frames

		virtual void DetectStreamInfo(UInt8 *header, UOSInt headerSize);
		virtual void ClearFrameBuff();
		virtual void SetStreamTime(Int32 time);
		virtual void WriteFrameStream(UInt8 *buff, UOSInt buffSize);
		virtual Int32 GetFrameStreamTime();
		virtual void EndFrameStream();
		virtual Int64 GetBitRate();
	};
}
#endif
