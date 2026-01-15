#ifndef _SM_MEDIA_M2VSTREAMSOURCE
#define _SM_MEDIA_M2VSTREAMSOURCE
#include "AnyType.h"
#include "Data/CallbackStorage.h"
#include "IO/Writer.h"
#include "Media/MediaStream.h"
#include "Media/MediaStreamControl.h"
#include "Media/VideoSourceBase.h"
#include "Sync/Mutex.h"

namespace IO
{
	class Stream;
}

namespace Media
{
	class M2VStreamSource : public Media::VideoSourceBase, public Media::MediaStream
	{
	private:
		typedef struct
		{
			UInt8 *frame;
			Data::Duration frameTime;
			UIntOS frameSize;
			IntOS pictureStart;
			UIntOS frameNum;
		} FrameBuff;

		NN<Media::MediaStreamControl> pbc;
		FrameCallback frameCb;
		FrameChangeCallback fcCb;
		AnyType frameCbData;
		Media::FrameInfo info;
		UInt32 frameRateNorm;
		UInt32 frameRateDenorm;
		UIntOS maxFrameSize;

		UInt64 totalFrameSize;
		UInt32 totalFrameCnt;

		UInt8 *frameBuff;
		UIntOS frameBuffSize;
		Data::Duration thisFrameTime;
		Data::Duration syncFrameTime;
		UIntOS syncFieldCnt;
		Bool firstFrame;
		UIntOS frameStart;
		UIntOS frameNum;
		Int32 writeCnt;

		Bool playing;
		Bool playInit;
		Bool playToStop;
		Bool playEOF;
		Sync::Event playEvt;
		Sync::Mutex playMut;
		Sync::Mutex pbcMut;
		FrameBuff *playBuff;
		IntOS playBuffStart;
		IntOS playBuffEnd;
		Double par;
		UInt64 bitRate;

		Bool finfoMode;
		Data::CallbackStorage<FrameInfoCallback> finfoCb;

		IO::Stream *debugFS;
		IO::Writer *debugLog;
		Sync::Mutex *debugMut;

		void SubmitFrame(UIntOS frameSize, UIntOS frameStart, UIntOS pictureStart);
		void ClearPlayBuff();
		static UInt32 __stdcall PlayThread(AnyType userObj);
	public:
		M2VStreamSource(NN<Media::MediaStreamControl> pbc);
		virtual ~M2VStreamSource();

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
		virtual UIntOS GetFrameSize(UIntOS frameIndex);
		virtual UIntOS ReadFrame(UIntOS frameIndex, UnsafeArray<UInt8> buff);

		virtual UIntOS ReadNextFrame(UnsafeArray<UInt8> frameBuff, OutParam<UInt32> frameTime, OutParam<Media::FrameType> ftype); //ret 0 = no more frames

		virtual void DetectStreamInfo(UInt8 *header, UIntOS headerSize);
		virtual void ClearFrameBuff();
		virtual void SetStreamTime(Data::Duration time);
		virtual void WriteFrameStream(UInt8 *buff, UIntOS buffSize);
		virtual Data::Duration GetFrameStreamTime();
		virtual void EndFrameStream();
		virtual UInt64 GetBitRate();
	};
}
#endif
