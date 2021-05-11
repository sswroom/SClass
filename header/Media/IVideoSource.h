#ifndef _SM_MEDIA_IVIDEOSOURCE
#define _SM_MEDIA_IVIDEOSOURCE
#include "Data/ArrayListInt32.h"
#include "Data/ArrayListUInt32.h"
#include "Media/FrameInfo.h"
#include "Media/IMediaSource.h"
#include "Sync/Event.h"

namespace Media
{
	class StaticImage;

	class IVideoSource : public IMediaSource
	{
	protected:
		Data::ArrayListInt32 *propNames;
		Data::ArrayListUInt32 *propSizes;
		Data::ArrayList<UInt8*> *propBuffs;

	public:
		typedef enum
		{
			FS_I,
			FS_P,
			FS_B,
			FS_N
		} FrameStruct;
		typedef enum
		{
			FC_PAR,
			FC_ENDPLAY,
			FC_CROP,
			FC_SRCCHG
		} FrameChange;
		typedef enum
		{
			FF_NONE = 0,
			FF_DISCONTTIME = 1,
			FF_BFRAMEPROC = 2,
			FF_RFF = 4,
			FF_REALTIME = 8,
			FF_FORCEDISP = 16
		} FrameFlag;

		typedef void (__stdcall *FrameCallback)(UInt32 frameTime, UInt32 frameNum, UInt8 **imgData, UOSInt dataSize, FrameStruct frameStruct, void *userData, Media::FrameType frameType, Media::IVideoSource::FrameFlag flags, Media::YCOffset ycOfst);
		typedef void (__stdcall *FrameChangeCallback)(FrameChange frChg, void *userData);
		typedef Bool (__stdcall *FrameInfoCallback)(UInt32 frameTime, UOSInt frameNum, UOSInt dataSize, Media::IVideoSource::FrameStruct frameStruct, Media::FrameType frameType, void *userData, Media::YCOffset ycOfst); //return true to continue
		typedef void (__stdcall *ImageCallback)(void *userData, UInt32 frameTime, UInt32 frameNum, Media::StaticImage *img);

		IVideoSource();
		virtual ~IVideoSource();

		virtual Bool CaptureImage(ImageCallback imgCb, void *userData);

		virtual UTF8Char *GetSourceName(UTF8Char *buff) = 0;
		virtual const UTF8Char *GetFilterName() = 0;

		virtual void SetBorderCrop(UOSInt cropLeft, UOSInt cropTop, UOSInt cropRight, UOSInt cropBottom) = 0;
		virtual void GetBorderCrop(UOSInt *cropLeft, UOSInt *cropTop, UOSInt *cropRight, UOSInt *cropBottom) = 0;
		virtual Bool GetVideoInfo(Media::FrameInfo *info, UInt32 *frameRateNorm, UInt32 *frameRateDenorm, UOSInt *maxFrameSize) = 0;
		virtual Bool Init(FrameCallback cb, FrameChangeCallback fcCb, void *userData) = 0;
		virtual Bool Start() = 0; //true = succeed
		virtual void Stop() = 0;
		virtual Bool IsRunning() = 0;

		virtual Int32 GetStreamTime() = 0; //ms, -1 = infinity
		virtual Bool CanSeek() = 0;
		virtual UInt32 SeekToTime(UInt32 time) = 0; //ms, ret actual time
		virtual Bool IsRealTimeSrc() = 0;
		virtual Bool SetPreferFrameType(Media::FrameType ftype);

		virtual UOSInt GetDataSeekCount() = 0;

		virtual OSInt GetFrameCount() = 0; //-1 = unknown;
		virtual UInt32 GetFrameTime(UOSInt frameIndex) = 0;
		virtual void EnumFrameInfos(FrameInfoCallback cb, void *userData) = 0;
		virtual UOSInt GetFrameSize(UOSInt frameIndex);
		virtual UOSInt ReadFrame(UOSInt frameIndex, UInt8 *buff);
		virtual Bool ReadFrameBegin();
		virtual Bool ReadFrameEnd();

		virtual OSInt ReadNextFrame(UInt8 *frameBuff, Int32 *frameTime, Media::FrameType *ftype) = 0; //ret 0 = no more frames
		void SetProp(Int32 propName, const UInt8 *propBuff, UInt32 propBuffSize);
		virtual UInt8 *GetProp(Int32 propName, UInt32 *size);

		virtual MediaType GetMediaType();
	};
}
#endif
