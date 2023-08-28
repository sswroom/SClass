#ifndef _SM_MEDIA_VIDEORENDERER
#define _SM_MEDIA_VIDEORENDERER
#include "Media/IDeinterlacer.h"
#include "Media/IDeintResizer.h"
#include "Media/IImgFilter.h"
#include "Media/IImgResizer.h"
#include "Media/ImageCopy.h"
#include "Media/IVideoSource.h"
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
		typedef void (__stdcall *EndNotifier)(void *userObj);

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

		class RendererStatus
		{
		public:
			UInt32 currTime;
			Int32 procDelay;
			Int32 dispDelay;
			Int32 dispJitter;
			Int32 videoDelay;
			UInt32 dispFrameTime;
			UInt32 dispFrameNum;
			UInt32 frameDispCnt;
			UInt32 frameSkipBefore;
			UInt32 frameSkipAfter;
			Int32 srcDelay;
			Int32 avOfst;
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
			UInt8 *srcBuff;
			OSInt srcW;
			OSInt srcH;

			UInt32 frameTime;
			UInt32 frameNum;
			Bool discontTime;
			Media::MonitorSurface *destSurface;
			Math::Size2D<UOSInt> destSize;
			UInt32 destBitDepth;
			Media::FrameType frameType;
			Media::YCOffset ycOfst;
			Media::IVideoSource::FrameFlag flags;
		} VideoBuff;

		typedef struct
		{
			Int32 status; //0 = not running, 1 = idle
			Int32 procType;
			Media::IImgResizer *resizer;
			Media::IDeintResizer *dresizer;
			UInt32 resizerBitDepth;
			Bool resizer10Bit;
			Media::CS::CSConverter *csconv;
			UInt8 *lrBuff;
			UOSInt lrSize;
			UInt8 *diBuff;
			UOSInt diSize;
			Media::IDeinterlacer *deint;
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
		Media::ColorManagerSess *colorSess;
		Media::IVideoSource *video;
		Media::FrameInfo videoInfo;
		Media::MonitorSurfaceMgr *surfaceMgr;
		UInt32 frameRateNorm;
		UInt32 frameRateDenorm;
		Media::ImageCopy outputCopier;
		Bool hasAudio;
		UInt32 refRate;
		Bool manualDeint;
		UInt32 lastFrameTime;
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
		Media::RefClock *dispClk;

		Sync::RWMutex procMut;
		Int32 procThisCount;
		Double *procDelayBuff;
		Int32 procCnt;

		Sync::RWMutex dispDelayMut;
		Double *dispDelayBuff;
		Double *dispJitterBuff;
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

		Int32 videoDelay;
		UInt32 dispFrameTime;
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
		Data::ArrayList<Media::IImgFilter*> imgFilters;

		Int32 picCnt;
		EndNotifier endHdlr;
		void *endHdlrObj;

		void CalDisplayRect(UOSInt srcWidth, UOSInt srcHeight, DrawRect *rect);

		virtual void ProcessVideo(ThreadStat *tstat, VideoBuff *vbuff, VideoBuff *vbuff2);
		virtual Media::IImgResizer *CreateResizer(Media::ColorManagerSess *colorSess, UInt32 bitDepth, Double srcRefLuminance);
		virtual void CreateCSConv(ThreadStat *tstat, Media::FrameInfo *info);
		virtual void CreateThreadResizer(ThreadStat *tstat);

		static void __stdcall OnVideoFrame(UInt32 frameTime, UInt32 frameNum, UInt8 **imgData, UOSInt dataSize, Media::IVideoSource::FrameStruct frameStruct, void *userData, Media::FrameType frameType, Media::IVideoSource::FrameFlag flags, Media::YCOffset ycOfst);
		static void __stdcall OnVideoChange(Media::IVideoSource::FrameChange fc, void *userData);

		static UInt32 __stdcall ProcessThread(void *userObj);
		static UInt32 __stdcall DisplayThread(void *userObj);
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

		virtual void LockUpdateSize(NotNullPtr<Sync::MutexUsage> mutUsage) = 0;
		virtual void DrawFromSurface(Media::MonitorSurface *surface, Math::Coord2D<OSInt> destTL, Math::Size2D<UOSInt> buffSize, Bool clearScn) = 0;
	public:
		VideoRenderer(Media::ColorManagerSess *colorSess, Media::MonitorSurfaceMgr *surfaceMgr, UOSInt buffCnt, UOSInt threadCnt);
		virtual ~VideoRenderer();

		void SetVideo(Media::IVideoSource *video);
		void SetHasAudio(Bool hasAudio);
		void SetTimeDelay(Int32 timeDelay);
		void VideoInit(Media::RefClock *clk);
		void VideoStart();
		void StopPlay();
		void UpdateCrop();
		//void SetFrameType(Bool forse, Media::FrameType frameType);
		void SetDeintType(DeinterlaceType deintType);
		void ClearBuff();

		void SetEndNotify(EndNotifier endHdlr, void *userObj);
		void SetSrcRGBType(Media::CS::TransferType rgbType);
		void SetSrcRGBTransfer(const Media::CS::TransferParam *transf);
		void SetSrcPrimaries(Media::ColorProfile::ColorType colorType);
		void SetSrcPrimaries(const Media::ColorProfile::ColorPrimaries *primaries);
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

		void AddImgFilter(Media::IImgFilter *imgFilter);
		void Snapshot();
		void GetStatus(RendererStatus *status);
		Media::MonitorSurfaceMgr* GetSurfaceMgr();
	};
}
#endif
