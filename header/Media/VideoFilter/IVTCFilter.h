#ifndef _SM_MEDIA_VIDEOFILTER_IVTCFILTER
#define _SM_MEDIA_VIDEOFILTER_IVTCFILTER
#include "AnyType.h"
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
				UIntOS fieldCnt;
				UIntOS fieldDiff;
				UIntOS fieldMDiff;
				UIntOS field2Cnt;
				UIntOS field2Diff;
				UIntOS field2MDiff;
//				UIntOS field3Cnt;
//				UIntOS field4Cnt;
			} FieldStat;

			typedef struct
			{
				IVTCFilter *me;
				UIntOS index;
				Int32 threadStat;
				Int32 currCmd;
				Sync::Event *evt;
				FieldStat fieldStat;
				UnsafeArray<UInt8> oddPtr;
				UnsafeArray<UInt8> evenPtr;
				UIntOS sw;
				UIntOS h;
			} ThreadStat;
		private:
			UInt8 *ivtcLastFrame;
			UInt8 *ivtcCurrFrame;
			UIntOS ivtcLastFrameSize;
			UIntOS ivtcFrameBuffSize;
			Bool ivtcLastExist;
			Media::FrameType ivtcLastFrameType;
			UInt32 ivtcLastFrameNum;
			Data::Duration ivtcLastFrameTime;
			Bool ivtcLastFieldUsed;
			UIntOS ivtcLastField;
			UInt32 ivtcLastOdd;
			UInt32 ivtcLastEven;
			Int32 ivtcLastSC;
			Int32 ivtcExist;

			Bool fieldExist;
			UnsafeArray<UInt8> fieldBuff;
			UIntOS fieldBuffSize;
			Media::FrameType fieldFrameType;
			Data::Duration fieldTime;
			UInt32 fieldNum;
			UIntOS fieldDataSize;
			Bool fieldIsDiscont;

			Sync::Mutex mut;
			Bool enabled;

			Sync::Event mainEvt;
			UIntOS threadCnt;
			ThreadStat *threadStats;
			IO::FileStream *debugFS;
			IO::Writer *debugLog;

			Bool ivtcTToStop;
			Sync::Event ivtcTEvt;
			Int32 ivtcTStatus; //0=not running, 1=idle, 2=processing
			Bool ivtcTRequest;
            Data::Duration ivtcTFrameTime;
			UInt32 ivtcTFrameNum;
			UnsafeArrayOpt<UInt8> ivtcTImgData;
			UIntOS ivtcTDataSize;
			Media::VideoSource::FrameStruct ivtcTFrameStruct;
			Media::FrameType ivtcTFrameType;
			Media::VideoSource::FrameFlag ivtcTFlags;
			Media::YCOffset ivtcTYCOfst;

			virtual void ProcessVideoFrame(Data::Duration frameTime, UInt32 frameNum, UnsafeArray<UnsafeArray<UInt8>> imgData, UIntOS dataSize, Media::VideoSource::FrameStruct frameStruct, AnyType userData, Media::FrameType frameType, Media::VideoSource::FrameFlag flags, Media::YCOffset ycOfst);
			virtual void OnFrameChange(Media::VideoSource::FrameChange fc);
			void do_IVTC(Data::Duration frameTime, UInt32 frameNum, UnsafeArray<UnsafeArray<UInt8>> imgData, UIntOS dataSize, Media::VideoSource::FrameStruct frameStruct, Media::FrameType frameType, Media::VideoSource::FrameFlag flags, Media::YCOffset ycOfst);
			void ClearIVTC();
			void StartIVTC(Data::Duration frameTime, UInt32 frameNum, UnsafeArray<UInt8> imgData, UIntOS dataSize, Media::VideoSource::FrameStruct frameStruct, Media::FrameType frameType, Media::VideoSource::FrameFlag flags, Media::YCOffset ycOfst);

			static UInt32 __stdcall IVTCThread(AnyType userObj);
			static UInt32 __stdcall CalcThread(AnyType userObj);
			static void do_CalcFieldStat(FieldStat *fieldStat, UInt8 *oddPtr, UInt8 *evenPtr, UIntOS w, UIntOS h);
			static void do_CalcFieldStatP(FieldStat *fieldStat, UInt8 *framePtr, UIntOS w, UIntOS h);
			void CalcFieldStat(NN<FieldStat> fieldStat, UnsafeArray<UInt8> oddPtr, UnsafeArray<UInt8> evenPtr, UIntOS w, UIntOS h);
			void CalcFieldStatP(NN<FieldStat> fieldStat, UnsafeArray<UInt8> framePtr, UIntOS w, UIntOS h);
		public:
			IVTCFilter(Media::VideoSource *srcVideo);
			virtual ~IVTCFilter();

			virtual Text::CStringNN GetFilterName();

			void SetEnabled(Bool enabled);
			virtual void Stop();
		};
	}
}
#endif