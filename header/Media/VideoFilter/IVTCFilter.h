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
				OSInt fieldCnt;
				OSInt fieldDiff;
				OSInt fieldMDiff;
				OSInt field2Cnt;
				OSInt field2Diff;
				OSInt field2MDiff;
//				OSInt field3Cnt;
//				OSInt field4Cnt;
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
				OSInt sw;
				OSInt h;
			} ThreadStat;
		private:
			UInt8 *ivtcLastFrame;
			UInt8 *ivtcCurrFrame;
			OSInt ivtcLastFrameSize;
			OSInt ivtcFrameBuffSize;
			Bool ivtcLastExist;
			Media::FrameType ivtcLastFrameType;
			Int32 ivtcLastFrameNum;
			Int32 ivtcLastFrameTime;
			Bool ivtcLastFieldUsed;
			OSInt ivtcLastField;
			Int32 ivtcLastOdd;
			Int32 ivtcLastEven;
			Int32 ivtcLastSC;
			Int32 ivtcExist;

			Bool fieldExist;
			UInt8 *fieldBuff;
			OSInt fieldBuffSize;
			Media::FrameType fieldFrameType;
			UInt32 fieldTime;
			UInt32 fieldNum;
			OSInt fieldDataSize;
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
			OSInt ivtcTDataSize;
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
			static void do_CalcFieldStat(FieldStat *fieldStat, UInt8 *oddPtr, UInt8 *evenPtr, OSInt w, OSInt h);
			static void do_CalcFieldStatP(FieldStat *fieldStat, UInt8 *framePtr, OSInt w, OSInt h);
			void CalcFieldStat(FieldStat *fieldStat, UInt8 *oddPtr, UInt8 *evenPtr, OSInt w, OSInt h);
			void CalcFieldStatP(FieldStat *fieldStat, UInt8 *framePtr, OSInt w, OSInt h);
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