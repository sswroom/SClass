#ifndef _SM_MEDIA_VFVIDEOSTREAM
#define _SM_MEDIA_VFVIDEOSTREAM
#include "Media/VFPManager.h"
#include "Media/FrameInfo.h"
#include "Media/VideoSourceBase.h"

namespace Media
{
	class VFVideoStream : public VideoSourceBase
	{
	private:
		Media::VFMediaFile *mfile;
		
		Media::FrameInfo info;
		UInt32 frameRate;
		UInt32 frameRateScale;
		UInt32 frameCnt;
		Bool seeked;

		UInt32 currFrameNum;
		Bool playing;
		FrameCallback frameCb;
		FrameChangeCallback fcCb;
		AnyType frameCbObj;
		Bool threadRunning;
		Bool threadToStop;
		Sync::Event threadEvt;

		static UInt32 __stdcall PlayThread(void *userObj);
	public:
		VFVideoStream(Media::VFMediaFile *mfile);
		~VFVideoStream();

		virtual UTF8Char *GetSourceName(UTF8Char *buff);
		virtual Text::CStringNN GetFilterName();

		virtual Bool GetVideoInfo(NotNullPtr<Media::FrameInfo> info, OutParam<UInt32> frameRateNorm, OutParam<UInt32> frameRateDenorm, OutParam<UOSInt> maxFrameSize);
		virtual Bool Init(FrameCallback cb, FrameChangeCallback fcCb, AnyType userObj);
		virtual Bool Start();
		virtual void Stop();
		virtual Bool IsRunning();

		virtual Data::Duration GetStreamTime();
		virtual Bool CanSeek();
		virtual Data::Duration SeekToTime(Data::Duration time);
		virtual Bool IsRealTimeSrc();
		virtual Bool TrimStream(UInt32 trimTimeStart, UInt32 trimTimeEnd, Int32 *syncTime);
		virtual Bool SetPreferFrameType(Media::FrameType ftype);

		virtual UOSInt GetDataSeekCount();

		virtual Bool HasFrameCount();
		virtual UOSInt GetFrameCount();
		virtual Data::Duration GetFrameTime(UOSInt frameIndex);
		virtual void EnumFrameInfos(FrameInfoCallback cb, AnyType userData);

		virtual UOSInt ReadNextFrame(UInt8 *frameBuff, UInt32 *frameTime, Media::FrameType *ftype); //ret 0 = no more frames
	};
}
#endif
