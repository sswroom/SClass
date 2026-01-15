#ifndef _SM_MEDIA_FILEVIDEOSOURCE
#define _SM_MEDIA_FILEVIDEOSOURCE
#include "AnyType.h"
#include "Data/ArrayListUInt32.h"
#include "Data/ArrayListUInt64.h"
#include "IO/StreamData.h"
#include "Media/VideoSourceBase.h"
#include "Sync/Mutex.h"

namespace Media
{
	class FileVideoSource : public Media::VideoSourceBase
	{
	private:
		typedef struct
		{
			UInt32 frameTime;
			UInt32 frameNum;
			UnsafeArray<UInt8> frameBuff;
			UOSInt frameSize;
			Media::VideoSource::FrameStruct frameStruct;
			Media::FrameType fType;
			Media::VideoSource::FrameFlag flags;
			Media::YCOffset ycOfst;
		} OutputFrameInfo;
	private:
		NN<IO::StreamData> data;
		Media::FrameInfo frameInfo;
		UInt32 frameRateNorm;
		UInt32 frameRateDenorm;
		UOSInt maxFrameSize;
		UOSInt currFrameSize;
		Bool timeBased;

		Data::ArrayListUInt64 frameOfsts;
		Data::ArrayListUInt32 frameSizes;
		Data::ArrayListUInt32 frameParts;
		Data::ArrayListNative<Bool> frameIsKey;
		Data::ArrayListUInt32 frameTimes;

		UInt32 currFrameNum;
		Bool playing;
		Bool playEnd;
		Bool playToStop;
		FrameCallback playCb;
		FrameChangeCallback fcCb;
		AnyType playCbData;
		Sync::Event playEvt;
		Sync::Mutex pbcMut;

		Bool outputRunning;
		Bool outputToStop;
		Sync::Event outputEvt;
		Sync::Mutex outputMut;
		OSInt outputStart;
		OSInt outputCount;
		OutputFrameInfo *outputFrames;

		Sync::Event mainEvt;

		static UInt32 __stdcall PlayThread(AnyType userObj);
		static UInt32 __stdcall OutputThread(AnyType userObj);
	public:
		FileVideoSource(NN<IO::StreamData> data, NN<const Media::FrameInfo> frameInfo, UInt32 frameRateNorm, UInt32 frameRateDenorm, Bool timeBased);
		virtual ~FileVideoSource();

		void AddNewFrame(UInt64 frameOfst, UInt32 frameSize, Bool isKey, UInt32 frameTime);
		void AddFramePart(UInt64 frameOfst, UInt32 frameSize);
		void SetFrameRate(UInt32 frameRateNorm, UInt32 frameRateDenorm);

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
		virtual UOSInt ReadFrame(UOSInt frameIndex, UnsafeArray<UInt8> frameBuff);

		virtual UOSInt ReadNextFrame(UnsafeArray<UInt8> frameBuff, OutParam<UInt32> frameTime, OutParam<Media::FrameType> ftype); //ret 0 = no more frames

	};
}
#endif
