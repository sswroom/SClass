#ifndef _SM_MEDIA_VIDEOSOURCE
#define _SM_MEDIA_VIDEOSOURCE
#include "AnyType.h"
#include "Data/ArrayListInt32.h"
#include "Data/ArrayListObj.hpp"
#include "Data/ArrayListUInt32.h"
#include "Media/FrameInfo.h"
#include "Media/MediaSource.h"
#include "Sync/Event.h"
#include "Text/CString.h"

namespace Media
{
	class StaticImage;

	class VideoSource : public MediaSource
	{
	protected:
		Data::ArrayListInt32 *propNames;
		Data::ArrayListUInt32 *propSizes;
		Data::ArrayListObj<UInt8*> *propBuffs;

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

		typedef void (CALLBACKFUNC FrameCallback)(Data::Duration frameTime, UInt32 frameNum, UnsafeArray<UnsafeArray<UInt8>> imgData, UIntOS dataSize, FrameStruct frameStruct, AnyType userData, Media::FrameType frameType, Media::VideoSource::FrameFlag flags, Media::YCOffset ycOfst);
		typedef void (CALLBACKFUNC FrameChangeCallback)(FrameChange frChg, AnyType userData);
		typedef Bool (CALLBACKFUNC FrameInfoCallback)(Data::Duration frameTime, UIntOS frameNum, UIntOS dataSize, Media::VideoSource::FrameStruct frameStruct, Media::FrameType frameType, AnyType userData, Media::YCOffset ycOfst); //return true to continue
		typedef void (CALLBACKFUNC ImageCallback)(AnyType userData, Data::Duration frameTime, UInt32 frameNum, NN<Media::StaticImage> img);

		VideoSource();
		virtual ~VideoSource();

		virtual Bool CaptureImage(ImageCallback imgCb, AnyType userData);

		virtual UnsafeArrayOpt<UTF8Char> GetSourceName(UnsafeArray<UTF8Char> buff) = 0;
		virtual Text::CStringNN GetFilterName() = 0;

		virtual void SetBorderCrop(UIntOS cropLeft, UIntOS cropTop, UIntOS cropRight, UIntOS cropBottom) = 0;
		virtual void GetBorderCrop(OutParam<UIntOS> cropLeft, OutParam<UIntOS> cropTop, OutParam<UIntOS> cropRight, OutParam<UIntOS> cropBottom) = 0;
		virtual Bool GetVideoInfo(NN<Media::FrameInfo> info, OutParam<UInt32> frameRateNorm, OutParam<UInt32> frameRateDenorm, OutParam<UIntOS> maxFrameSize) = 0;
		virtual Bool Init(FrameCallback cb, FrameChangeCallback fcCb, AnyType userData) = 0;
		virtual Bool Start() = 0; //true = succeed
		virtual void Stop() = 0;
		virtual Bool IsRunning() = 0;

		virtual Data::Duration GetStreamTime() = 0;
		virtual Bool CanSeek() = 0;
		virtual Data::Duration SeekToTime(Data::Duration time) = 0;
		virtual Bool IsRealTimeSrc() = 0;
		virtual Bool SetPreferFrameType(Media::FrameType ftype);

		virtual UIntOS GetDataSeekCount() = 0;

		virtual Bool HasFrameCount() = 0;
		virtual UIntOS GetFrameCount() = 0;
		virtual Data::Duration GetFrameTime(UIntOS frameIndex) = 0;
		virtual void EnumFrameInfos(FrameInfoCallback cb, AnyType userData) = 0;
		virtual UIntOS GetFrameSize(UIntOS frameIndex);
		virtual UIntOS ReadFrame(UIntOS frameIndex, UnsafeArray<UInt8> buff);
		virtual Bool ReadFrameBegin();
		virtual Bool ReadFrameEnd();

		virtual UIntOS ReadNextFrame(UnsafeArray<UInt8> frameBuff, OutParam<UInt32> frameTime, OutParam<Media::FrameType> ftype) = 0; //ret 0 = no more frames
		void SetProp(Int32 propName, UnsafeArray<const UInt8> propBuff, UInt32 propBuffSize);
		virtual UInt8 *GetProp(Int32 propName, UInt32 *size);

		virtual MediaType GetMediaType();
	};
}
#endif
