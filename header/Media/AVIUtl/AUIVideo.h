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
			Media::AVIUtl::AUIPlugin *plugin;
			Media::AVIUtl::AUIPlugin::AUIInput *input;
			Media::FrameInfo *frameInfo;
			Int32 frameRateNorm;
			Int32 frameRateDenorm;
			Int32 frameCnt;
			Sync::Mutex *frameNumMut;
			Int32 currFrameNum;

			FrameCallback playCb;
			FrameChangeCallback fcCb;
			void *playCbData;
			Bool playing;
			Bool threadRunning;
			Bool threadToStop;
			Sync::Event *threadEvt;
			
			static UInt32 __stdcall PlayThread(void *userObj);
			UOSInt GetMaxFrameSize();
		public:
			AUIVideo(Media::AVIUtl::AUIPlugin *plugin, Media::AVIUtl::AUIPlugin::AUIInput *input, Media::FrameInfo *frameInfo, Int32 frameRateNorm, Int32 frameRateDenorm, Int32 frameCnt);
			~AUIVideo();

			virtual UTF8Char *GetSourceName(UTF8Char *buff);
			virtual const UTF8Char *GetFilterName();

			virtual Bool GetVideoInfo(Media::FrameInfo *info, Int32 *frameRateNorm, Int32 *frameRateDenorm, UOSInt *maxFrameSize);
			virtual Bool Init(FrameCallback cb, FrameChangeCallback fcCb, void *userData);
			virtual Bool Start();
			virtual void Stop();
			virtual Bool IsRunning();

			virtual Int32 GetStreamTime();
			virtual Bool CanSeek();
			virtual Int32 SeekToTime(Int32 time);
			virtual Bool IsRealTimeSrc();
			virtual Bool TrimStream(Int32 trimTimeStart, Int32 trimTimeEnd, Int32 *syncTime);

			virtual OSInt GetDataSeekCount();

			virtual OSInt GetFrameCount(); //-1 = unknown;
			virtual UInt32 GetFrameTime(UOSInt frameIndex);
			virtual void EnumFrameInfos(FrameInfoCallback cb, void *userData);

			virtual OSInt ReadNextFrame(UInt8 *frameBuff, Int32 *frameTime, Media::FrameType *ftype);

		};
	}
}
#endif
