#ifndef _SM_MEDIA_M2VSTREAMSOURCE
#define _SM_MEDIA_M2VSTREAMSOURCE
#include "AnyType.h"
#include "Data/CallbackStorage.h"
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
			Data::Duration frameTime;
			UOSInt frameSize;
			OSInt pictureStart;
			UOSInt frameNum;
		} FrameBuff;

		NN<Media::IStreamControl> pbc;
		FrameCallback frameCb;
		FrameChangeCallback fcCb;
		AnyType frameCbData;
		Media::FrameInfo info;
		UInt32 frameRateNorm;
		UInt32 frameRateDenorm;
		UOSInt maxFrameSize;

		UInt64 totalFrameSize;
		UInt32 totalFrameCnt;

		UInt8 *frameBuff;
		UOSInt frameBuffSize;
		Data::Duration thisFrameTime;
		Data::Duration syncFrameTime;
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
		Data::CallbackStorage<FrameInfoCallback> finfoCb;

		IO::Stream *debugFS;
		IO::Writer *debugLog;
		Sync::Mutex *debugMut;

		void SubmitFrame(UOSInt frameSize, UOSInt frameStart, UOSInt pictureStart);
		void ClearPlayBuff();
		static UInt32 __stdcall PlayThread(AnyType userObj);
	public:
		M2VStreamSource(NN<Media::IStreamControl> pbc);
		virtual ~M2VStreamSource();

		virtual UnsafeArrayOpt<UTF8Char> GetSourceName(UnsafeArray<UTF8Char> buff);
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
		virtual Bool TrimStream(UInt32 trimTimeStart, UInt32 trimTimeEnd, OptOut<Int32> syncTime);

		virtual UOSInt GetDataSeekCount();

		virtual Bool HasFrameCount();
		virtual UOSInt GetFrameCount();
		virtual Data::Duration GetFrameTime(UOSInt frameIndex);
		virtual void EnumFrameInfos(FrameInfoCallback cb, AnyType userData);
		virtual UOSInt GetFrameSize(UOSInt frameIndex);
		virtual UOSInt ReadFrame(UOSInt frameIndex, UnsafeArray<UInt8> buff);

		virtual UOSInt ReadNextFrame(UnsafeArray<UInt8> frameBuff, OutParam<UInt32> frameTime, OutParam<Media::FrameType> ftype); //ret 0 = no more frames

		virtual void DetectStreamInfo(UInt8 *header, UOSInt headerSize);
		virtual void ClearFrameBuff();
		virtual void SetStreamTime(Data::Duration time);
		virtual void WriteFrameStream(UInt8 *buff, UOSInt buffSize);
		virtual Data::Duration GetFrameStreamTime();
		virtual void EndFrameStream();
		virtual UInt64 GetBitRate();
	};
}
#endif
