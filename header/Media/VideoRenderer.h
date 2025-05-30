#ifndef _SM_MEDIA_VIDEORENDERER
#define _SM_MEDIA_VIDEORENDERER
#include "Media/Deinterlacer.h"
#include "Media/DeinterlacingResizer.h"
#include "Media/ImageFilter.h"
#include "Media/ImageResizer.h"
#include "Media/ImageCopy.h"
#include "Media/VideoSource.h"
#include "Media/MonitorSurfaceMgr.h"
#include "Media/RefClock.h"
#include "Media/CS/CSConverter.h"
#include "Media/VideoFilter/AutoCropFilter.h"
#include "Media/VideoFilter/IVTCFilter.h"
#include "Media/VideoFilter/UVOffsetFilter.h"
#include "Sync/MutexUsage.h"

namespace Media
{
	class VideoRenderer
	{
	public:
		typedef void (CALLBACKFUNC EndNotifier)(AnyType userObj);

		enum DeinterlaceType
		{
			DT_FROM_VIDEO,
			DT_PROGRESSIVE,
			DT_INTERLACED_TFF,
			DT_INTERLACED_BFF,
			DT_30P_MODE
		};

		struct DrawRect
		{
			Math::Coord2D<OSInt> tl;
			Math::Size2D<UOSInt> size;
		};

		class RendererStatus2
		{
		public:
			Data::Duration currTime;
			Int32 procDelay;
			Int32 dispDelay;
			Int32 dispJitter;
			Data::Duration videoDelay;
			Data::Duration dispFrameTime;
			UInt32 dispFrameNum;
			UInt32 frameDispCnt;
			UInt32 frameSkipBefore;
			UInt32 frameSkipAfter;
			Int32 srcDelay;
			Int32 avOfst;
			Media::RotateType rotateType;
			UInt32 format;
			Math::Size2D<UOSInt> srcSize;
			Math::Size2D<UOSInt> dispSize;
			UOSInt seekCnt;
			Double par;
			UInt32 dispBitDepth;
			Media::ColorProfile color;
			Media::ColorProfile::YUVType srcYUVType;
			Text::CString decoderName;
			Int32 buffProc;
			Int32 buffReady;
			Double hTime;
			Double vTime;
			Double csTime;
		};

	protected:
		typedef struct
		{
			Bool isEmpty;
			Bool isProcessing;
			Bool isOutputReady;
			UnsafeArrayOpt<UInt8> srcBuff;
			OSInt srcW;
			OSInt srcH;

			Data::Duration frameTime;
			UInt32 frameNum;
			Bool discontTime;
			Optional<Media::MonitorSurface> destSurface;
			Math::Size2D<UOSInt> destSize;
			UInt32 destBitDepth;
			Media::FrameType frameType;
			Media::YCOffset ycOfst;
			Media::VideoSource::FrameFlag flags;
		} VideoBuff;

		typedef struct
		{
			Int32 status; //0 = not running, 1 = idle
			Int32 procType;
			Media::ImageResizer *resizer;
			Media::DeinterlacingResizer *dresizer;
			UInt32 resizerBitDepth;
			Bool resizer10Bit;
			Optional<Media::CS::CSConverter> csconv;
			UInt8 *lrBuff;
			UOSInt lrSize;
			UInt8 *diBuff;
			UOSInt diSize;
			Media::Deinterlacer *deint;
			VideoRenderer *me;
			Bool srcChanged;
			Double hTime;
			Double vTime;
			Double csTime;
			Sync::Event *evt;
			Bool cs10Bit;
			Double resizerSrcRefLuminance;
		} ThreadStat;
	protected:
		NN<Media::ColorManagerSess> colorSess;
		Optional<Media::VideoSource> video;
		Media::FrameInfo videoInfo;
		NN<Media::MonitorSurfaceMgr> surfaceMgr;
		UInt32 frameRateNorm;
		UInt32 frameRateDenorm;
		Media::ImageCopy outputCopier;
		Bool hasAudio;
		UInt32 refRate;
		Bool manualDeint;
		Data::Duration lastFrameTime;
		Bool ignoreFrameTime;

		UOSInt threadCnt;
		UOSInt buffCnt;
		UOSInt allBuffCnt;
		VideoBuff *buffs;
		Sync::Mutex buffMut;
		Sync::Event buffEvt;
		ThreadStat *tstats;
		Bool threadToStop;
		Double forcePAR;
		Double monPAR;
		Bool toClear;

		Sync::Event dispEvt;
		Bool dispToStop;
		Bool dispRunning;
		Bool dispForceUpdate;
		Sync::RWMutex dispMut;
		Optional<Media::RefClock> dispClk;

		Sync::RWMutex procMut;
		Int32 procThisCount;
		Double *procDelayBuff;
		Int32 procCnt;

		Sync::RWMutex dispDelayMut;
		Double *dispDelayBuff;
		Data::Duration *dispJitterBuff;
		Int32 dispCnt;

		UOSInt cropLeft;
		UOSInt cropTop;
		UOSInt cropRight;
		UOSInt cropBottom;
		DeinterlaceType deintType;
//		Bool forseFT;
//		Media::FrameType forseFTVal;

		Int32 timeDelay;
		Bool captureFrame;
		Bool playing;
//		Sync::Mutex *videoMut;
		Bool videoPause;
		Int32 videoProcCnt;
		Bool updatingSize;

		Data::Duration videoDelay;
		Data::Duration dispFrameTime;
		UInt32 dispFrameNum;
		UInt32 frameDispCnt;
		UInt32 frameSkipBeforeProc;
		UInt32 frameSkipAfterProc;
		Int32 avOfst;
		Media::ColorProfile::YUVType srcYUVType;
		Media::ColorProfile srcColor;
		Bool curr10Bit;
		Double currSrcRefLuminance;

		Math::Size2D<UOSInt> outputSize;
		UInt32 outputBpp;
		Media::PixelFormat outputPf;

		Media::VideoFilter::IVTCFilter ivtc;
		Media::VideoFilter::UVOffsetFilter uvOfst;
		Media::VideoFilter::AutoCropFilter autoCrop;
		Data::ArrayListNN<Media::ImageFilter> imgFilters;

		Int32 picCnt;
		EndNotifier endHdlr;
		AnyType endHdlrObj;

		void CalDisplayRect(UOSInt srcWidth, UOSInt srcHeight, DrawRect *rect);

		virtual void ProcessVideo(NN<ThreadStat> tstat, VideoBuff *vbuff, VideoBuff *vbuff2);
		virtual NN<Media::ImageResizer> CreateResizer(NN<Media::ColorManagerSess> colorSess, UInt32 bitDepth, Double srcRefLuminance);
		virtual void CreateCSConv(NN<ThreadStat> tstat, Media::FrameInfo *info);
		virtual void CreateThreadResizer(NN<ThreadStat> tstat);

		static void __stdcall OnVideoFrame(Data::Duration frameTime, UInt32 frameNum, UnsafeArray<UnsafeArray<UInt8>> imgData, UOSInt dataSize, Media::VideoSource::FrameStruct frameStruct, AnyType userData, Media::FrameType frameType, Media::VideoSource::FrameFlag flags, Media::YCOffset ycOfst);
		static void __stdcall OnVideoChange(Media::VideoSource::FrameChange fc, AnyType userData);

		static UInt32 __stdcall ProcessThread(AnyType userObj);
		static UInt32 __stdcall DisplayThread(AnyType userObj);
		void StopThreads();

		void VideoBeginLoad();
		void VideoEndLoad();
		void VideoBeginProc();
		void VideoEndProc();
		
		Int32 CalProcDelay();
		Int32 CalDispDelay();
		Int32 CalDispJitter();

		void UpdateRefreshRate(UInt32 refRate);
		void UpdateOutputSize(Math::Size2D<UOSInt> outputSize);
		void UpdateDispInfo(Math::Size2D<UOSInt> outputSize, UInt32 bpp, Media::PixelFormat pf);

		virtual void LockUpdateSize(NN<Sync::MutexUsage> mutUsage) = 0;
		virtual void DrawFromSurface(NN<Media::MonitorSurface> surface, Math::Coord2D<OSInt> destTL, Math::Size2D<UOSInt> buffSize, Bool clearScn) = 0;
	public:
		VideoRenderer(NN<Media::ColorManagerSess> colorSess, NN<Media::MonitorSurfaceMgr> surfaceMgr, UOSInt buffCnt, UOSInt threadCnt);
		virtual ~VideoRenderer();

		void SetVideo(Optional<Media::VideoSource> video);
		void SetHasAudio(Bool hasAudio);
		void SetTimeDelay(Int32 timeDelay);
		void VideoInit(NN<Media::RefClock> clk);
		void VideoStart();
		void StopPlay();
		void UpdateCrop();
		//void SetFrameType(Bool forse, Media::FrameType frameType);
		void SetDeintType(DeinterlaceType deintType);
		void ClearBuff();

		void SetEndNotify(EndNotifier endHdlr, AnyType userObj);
		void SetSrcRGBType(Media::CS::TransferType rgbType);
		void SetSrcRGBTransfer(NN<const Media::CS::TransferParam> transf);
		void SetSrcPrimaries(Media::ColorProfile::ColorType colorType);
		void SetSrcPrimaries(NN<const Media::ColorProfile::ColorPrimaries> primaries);
		void SetSrcWP(Media::ColorProfile::WhitePointType wpType);
		void SetSrcWPTemp(Double colorTemp);
		void SetSrcYUVType(Media::ColorProfile::YUVType yuvType);
		void SetSrcPAR(Double forcePAR);
		void SetMonPAR(Double forcePAR);
		void SetIVTCEnable(Bool enableIVTC);
		void SetUVOfst(Int32 uOfst, Int32 vOfst);
		void SetAutoCropEnable(Bool enableCrop);
		Int32 GetAVOfst();
		void SetAVOfst(Int32 avOfst);
		void SetIgnoreFrameTime(Bool ignoreFrameTime);
		virtual void SetRotateType(Media::RotateType rotateType) = 0;
		virtual Media::RotateType GetRotateType() const = 0;

		void AddImgFilter(NN<Media::ImageFilter> imgFilter);
		void Snapshot();
		void GetStatus(NN<RendererStatus2> status);
		NN<Media::MonitorSurfaceMgr> GetSurfaceMgr();
	};
}
#endif
