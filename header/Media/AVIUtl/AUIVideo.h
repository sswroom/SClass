#ifndef _SM_MEDIA_AVIUTL_AUIVIDEO
#define _SM_MEDIA_AVIUTL_AUIVIDEO
#include "Media/VideoSourceBase.h"
#include "Media/AVIUtl/AUIPlugin.h"
#include "Sync/Event.h"
#include "Sync/Mutex.h"

namespace Media
{
	namespace AVIUtl
	{
		class AUIVideo : public Media::VideoSourceBase
		{
		private:
			NN<Media::AVIUtl::AUIPlugin> plugin;
			NN<Media::AVIUtl::AUIPlugin::AUIInput> input;
			NN<const Media::FrameInfo> frameInfo;
			UInt32 frameRateNorm;
			UInt32 frameRateDenorm;
			UInt32 frameCnt;
			Sync::Mutex frameNumMut;
			UInt32 currFrameNum;

			FrameCallback playCb;
			FrameChangeCallback fcCb;
			AnyType playCbData;
			Bool playing;
			Bool threadRunning;
			Bool threadToStop;
			Sync::Event threadEvt;
			
			static UInt32 __stdcall PlayThread(AnyType userObj);
			UOSInt GetMaxFrameSize();
		public:
			AUIVideo(NN<Media::AVIUtl::AUIPlugin> plugin, NN<Media::AVIUtl::AUIPlugin::AUIInput> input, NN<const Media::FrameInfo> frameInfo, UInt32 frameRateNorm, UInt32 frameRateDenorm, UInt32 frameCnt);
			~AUIVideo();

			virtual UnsafeArrayOpt<UTF8Char> GetSourceName(UnsafeArray<UTF8Char> buff);
			virtual Text::CStringNN GetFilterName();

			virtual Bool GetVideoInfo(NN<Media::FrameInfo> info, OutParam<UInt32> frameRateNorm, OutParam<UInt32> frameRateDenorm, OutParam<UOSInt> maxFrameSize);
			virtual Bool Init(FrameCallback cb, FrameChangeCallback fcCb, AnyType userData);
			virtual Bool Start();
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

			virtual UOSInt ReadNextFrame(UnsafeArray<UInt8> frameBuff, OutParam<UInt32> frameTime, OutParam<Media::FrameType> ftype);

		};
	}
}
#endif
