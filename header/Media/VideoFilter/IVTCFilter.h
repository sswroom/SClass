#ifndef _SM_MEDIA_VIDEOFILTER_IVTCFILTER
#define _SM_MEDIA_VIDEOFILTER_IVTCFILTER
#include "IO/FileStream.h"
#include "IO/Writer.h"
#include "Media/VideoFilter/VideoFilterBase.h"
#include "Sync/Mutex.h"

namespace Media
{
	namespace VideoFilter
	{
		class IVTCFilter : public Media::VideoFilter::VideoFilterBase
		{
		private:
			typedef struct
			{
				UOSInt fieldCnt;
				UOSInt fieldDiff;
				UOSInt fieldMDiff;
				UOSInt field2Cnt;
				UOSInt field2Diff;
				UOSInt field2MDiff;
//				UOSInt field3Cnt;
//				UOSInt field4Cnt;
			} FieldStat;

			typedef struct
			{
				IVTCFilter *me;
				Int32 threadStat;
				Int32 currCmd;
				Sync::Event *evt;
				FieldStat fieldStat;
				UInt8 *oddPtr;
				UInt8 *evenPtr;
				UOSInt sw;
				UOSInt h;
			} ThreadStat;
		private:
			UInt8 *ivtcLastFrame;
			UInt8 *ivtcCurrFrame;
			UOSInt ivtcLastFrameSize;
			UOSInt ivtcFrameBuffSize;
			Bool ivtcLastExist;
			Media::FrameType ivtcLastFrameType;
			UInt32 ivtcLastFrameNum;
			UInt32 ivtcLastFrameTime;
			Bool ivtcLastFieldUsed;
			UOSInt ivtcLastField;
			UInt32 ivtcLastOdd;
			UInt32 ivtcLastEven;
			Int32 ivtcLastSC;
			Int32 ivtcExist;

			Bool fieldExist;
			UInt8 *fieldBuff;
			UOSInt fieldBuffSize;
			Media::FrameType fieldFrameType;
			UInt32 fieldTime;
			UInt32 fieldNum;
			UOSInt fieldDataSize;
			Bool fieldIsDiscont;

			Sync::Mutex *mut;
			Bool enabled;

			Sync::Event *mainEvt;
			UOSInt threadCnt;
			ThreadStat *threadStats;
			IO::FileStream *debugFS;
			IO::Writer *debugLog;

			Bool ivtcTToStop;
			Sync::Event *ivtcTEvt;
			Int32 ivtcTStatus; //0=not running, 1=idle, 2=processing
			Bool ivtcTRequest;
            UInt32 ivtcTFrameTime;
			UInt32 ivtcTFrameNum;
			UInt8 *ivtcTImgData;
			UOSInt ivtcTDataSize;
			Media::IVideoSource::FrameStruct ivtcTFrameStruct;
			Media::FrameType ivtcTFrameType;
			Media::IVideoSource::FrameFlag ivtcTFlags;
			Media::YCOffset ivtcTYCOfst;

			virtual void ProcessVideoFrame(UInt32 frameTime, UInt32 frameNum, UInt8 **imgData, UOSInt dataSize, Media::IVideoSource::FrameStruct frameStruct, void *userData, Media::FrameType frameType, Media::IVideoSource::FrameFlag flags, Media::YCOffset ycOfst);
			virtual void OnFrameChange(Media::IVideoSource::FrameChange fc);
			void do_IVTC(UInt32 frameTime, UInt32 frameNum, UInt8 **imgData, UOSInt dataSize, Media::IVideoSource::FrameStruct frameStruct, Media::FrameType frameType, Media::IVideoSource::FrameFlag flags, Media::YCOffset ycOfst);
			void ClearIVTC();
			void StartIVTC(UInt32 frameTime, UInt32 frameNum, UInt8 *imgData, UOSInt dataSize, Media::IVideoSource::FrameStruct frameStruct, Media::FrameType frameType, Media::IVideoSource::FrameFlag flags, Media::YCOffset ycOfst);

			static UInt32 __stdcall IVTCThread(void *userObj);
			static UInt32 __stdcall CalcThread(void *userObj);
			static void do_CalcFieldStat(FieldStat *fieldStat, UInt8 *oddPtr, UInt8 *evenPtr, UOSInt w, UOSInt h);
			static void do_CalcFieldStatP(FieldStat *fieldStat, UInt8 *framePtr, UOSInt w, UOSInt h);
			void CalcFieldStat(FieldStat *fieldStat, UInt8 *oddPtr, UInt8 *evenPtr, UOSInt w, UOSInt h);
			void CalcFieldStatP(FieldStat *fieldStat, UInt8 *framePtr, UOSInt w, UOSInt h);
		public:
			IVTCFilter(Media::IVideoSource *srcVideo);
			virtual ~IVTCFilter();

			virtual const UTF8Char *GetFilterName();

			void SetEnabled(Bool enabled);
			virtual void Stop();
		};
	}
}
#endif