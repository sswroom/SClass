#ifndef _SM_MEDIA_IVIDEOSOURCE
#define _SM_MEDIA_IVIDEOSOURCE
#include "AnyType.h"
#include "Data/ArrayListInt32.h"
#include "Data/ArrayListUInt32.h"
#include "Media/FrameInfo.h"
#include "Media/IMediaSource.h"
#include "Sync/Event.h"
#include "Text/CString.h"

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

		typedef void (CALLBACKFUNC FrameCallback)(Data::Duration frameTime, UInt32 frameNum, UnsafeArray<UnsafeArray<UInt8>> imgData, UOSInt dataSize, FrameStruct frameStruct, AnyType userData, Media::FrameType frameType, Media::IVideoSource::FrameFlag flags, Media::YCOffset ycOfst);
		typedef void (CALLBACKFUNC FrameChangeCallback)(FrameChange frChg, AnyType userData);
		typedef Bool (CALLBACKFUNC FrameInfoCallback)(Data::Duration frameTime, UOSInt frameNum, UOSInt dataSize, Media::IVideoSource::FrameStruct frameStruct, Media::FrameType frameType, AnyType userData, Media::YCOffset ycOfst); //return true to continue
		typedef void (CALLBACKFUNC ImageCallback)(AnyType userData, Data::Duration frameTime, UInt32 frameNum, NN<Media::StaticImage> img);

		IVideoSource();
		virtual ~IVideoSource();

		virtual Bool CaptureImage(ImageCallback imgCb, AnyType userData);

		virtual UnsafeArrayOpt<UTF8Char> GetSourceName(UnsafeArray<UTF8Char> buff) = 0;
		virtual Text::CStringNN GetFilterName() = 0;

		virtual void SetBorderCrop(UOSInt cropLeft, UOSInt cropTop, UOSInt cropRight, UOSInt cropBottom) = 0;
		virtual void GetBorderCrop(OutParam<UOSInt> cropLeft, OutParam<UOSInt> cropTop, OutParam<UOSInt> cropRight, OutParam<UOSInt> cropBottom) = 0;
		virtual Bool GetVideoInfo(NN<Media::FrameInfo> info, OutParam<UInt32> frameRateNorm, OutParam<UInt32> frameRateDenorm, OutParam<UOSInt> maxFrameSize) = 0;
		virtual Bool Init(FrameCallback cb, FrameChangeCallback fcCb, AnyType userData) = 0;
		virtual Bool Start() = 0; //true = succeed
		virtual void Stop() = 0;
		virtual Bool IsRunning() = 0;

		virtual Data::Duration GetStreamTime() = 0;
		virtual Bool CanSeek() = 0;
		virtual Data::Duration SeekToTime(Data::Duration time) = 0;
		virtual Bool IsRealTimeSrc() = 0;
		virtual Bool SetPreferFrameType(Media::FrameType ftype);

		virtual UOSInt GetDataSeekCount() = 0;

		virtual Bool HasFrameCount() = 0;
		virtual UOSInt GetFrameCount() = 0;
		virtual Data::Duration GetFrameTime(UOSInt frameIndex) = 0;
		virtual void EnumFrameInfos(FrameInfoCallback cb, AnyType userData) = 0;
		virtual UOSInt GetFrameSize(UOSInt frameIndex);
		virtual UOSInt ReadFrame(UOSInt frameIndex, UnsafeArray<UInt8> buff);
		virtual Bool ReadFrameBegin();
		virtual Bool ReadFrameEnd();

		virtual UOSInt ReadNextFrame(UnsafeArray<UInt8> frameBuff, OutParam<UInt32> frameTime, OutParam<Media::FrameType> ftype) = 0; //ret 0 = no more frames
		void SetProp(Int32 propName, UnsafeArray<const UInt8> propBuff, UInt32 propBuffSize);
		virtual UInt8 *GetProp(Int32 propName, UInt32 *size);

		virtual MediaType GetMediaType();
	};
}
#endif
