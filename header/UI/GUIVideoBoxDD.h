#ifndef _SM_UI_GUIVIDEOBOXDD
#define _SM_UI_GUIVIDEOBOXDD
#include "IO/Stream.h"
#include "IO/Writer.h"
#include "Media/ColorManager.h"
#include "Media/IDeinterlacer.h"
#include "Media/IDeintResizer.h"
#include "Media/IImgFilter.h"
#include "Media/IImgResizer.h"
#include "Media/ImageCopy.h"
#include "Media/IVideoSource.h"
#include "Media/RefClock.h"
#include "Media/CS/CSConverter.h"
#include "Media/VideoFilter/AutoCropFilter.h"
#include "Media/VideoFilter/IVTCFilter.h"
#include "Media/VideoFilter/UVOffsetFilter.h"
#include "Sync/Mutex.h"
#include "Sync/RWMutex.h"
#include "UI/GUIDDrawControl.h"

namespace UI
{
	class GUIVideoBoxDD : public GUIDDrawControl, public Media::IColorHandler
	{
	public:
		typedef void (__stdcall *EndNotifier)(void *userObj);

		typedef enum
		{
			DT_FROM_VIDEO,
			DT_PROGRESSIVE,
			DT_INTERLACED_TFF,
			DT_INTERLACED_BFF,
			DT_30P_MODE
		} DeinterlaceType;

		typedef struct
		{
			UInt32 currTime;
			Int32 procDelay;
			Int32 dispDelay;
			Int32 dispJitter;
			Int32 videoDelay;
			UInt32 dispFrameTime;
			Int32 dispFrameNum;
			Int32 frameDispCnt;
			Int32 frameSkip1;
			Int32 frameSkip2;
			Int32 srcDelay;
			Int32 avOfst;
			UInt32 format;
			UOSInt srcWidth;
			UOSInt srcHeight;
			UOSInt dispWidth;
			UOSInt dispHeight;
			UOSInt seekCnt;
			Double par;
			Int32 dispBitDepth;
			Media::ColorProfile *color;
			Media::ColorProfile::YUVType srcYUVType;
			const UTF8Char *decoderName;
			Int32 buffProc;
			Int32 buffReady;
			Double hTime;
			Double vTime;
			Double csTime;
		} DebugValue;

		typedef enum
		{
			MA_STOP,
			MA_START,
			MA_PAUSE
		} MouseAction;
		typedef void (__stdcall *MouseActionHandler)(void *userObj, MouseAction ma, OSInt x, OSInt y);
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
			UOSInt destSize;
			UInt8 *destBuff;
			UOSInt destW;
			UOSInt destH;
			Int32 destBitDepth;
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
			Int32 resizerBitDepth;
			Bool resizer10Bit;
			Media::CS::CSConverter *csconv;
			UInt8 *lrBuff;
			UOSInt lrSize;
			UInt8 *diBuff;
			UOSInt diSize;
			Media::IDeinterlacer *deint;
			GUIVideoBoxDD *me;
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
		Media::FrameInfo *videoInfo;
		UInt32 frameRateNorm;
		UInt32 frameRateDenorm;
		Media::ImageCopy *outputCopier;
		Bool hasAudio;
		UInt32 refRate;
		Bool manualDeint;
		UInt32 lastFrameTime;
		Bool ignoreFrameTime;

		UOSInt threadCnt;
		UOSInt buffCnt;
		UOSInt allBuffCnt;
		VideoBuff *buffs;
		Sync::Mutex *buffMut;
		Sync::Event *buffEvt;
		ThreadStat *tstats;
		Bool threadToStop;
		Double forcePAR;
		Double monPAR;
		Bool toClear;

		Sync::Event *dispEvt;
		Bool dispToStop;
		Bool dispRunning;
		Bool dispForceUpdate;
		Sync::RWMutex *dispMut;
		Media::RefClock *dispClk;

		Sync::RWMutex *procMut;
		Int32 procThisCount;
		Double *procDelayBuff;
		Int32 procCnt;

		Sync::RWMutex *dispDelayMut;
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

		IO::Writer *debugLog;
		IO::Stream *debugFS;
		IO::Writer *debugLog2;
		IO::Stream *debugFS2;
		UInt32 debugFrameTime;
		Int32 debugFrameNum;
		Int32 videoDelay;
		UInt32 dispFrameTime;
		Int32 dispFrameNum;
		Int32 debugFrameDisp;
		Int32 debugFrameSkip1;
		Int32 debugFrameSkip2;
		Int32 avOfst;
		Media::ColorProfile::YUVType srcYUVType;
		Media::ColorProfile *srcColor;
		Bool curr10Bit;
		Double currSrcRefLuminance;

		Media::VideoFilter::IVTCFilter *ivtc;
		Media::VideoFilter::UVOffsetFilter *uvOfst;
		Media::VideoFilter::AutoCropFilter *autoCrop;
		Data::ArrayList<Media::IImgFilter*> *imgFilters;

		Int32 picCnt;
		EndNotifier endHdlr;
		void *endHdlrObj;

		MouseActionHandler maHdlr;
		void *maHdlrObj;
		Bool maDown;
		OSInt maDownX;
		OSInt maDownY;
		Int64 maDownTime;

	protected:
		void CalDisplayRect(OSInt srcWidth, OSInt srcHeight, OSInt *rect);
		void UpdateFromBuff(VideoBuff *vbuff);

		virtual void ProcessVideo(ThreadStat *tstat, VideoBuff *vbuff, VideoBuff *vbuff2);
		virtual Media::IImgResizer *CreateResizer(Media::ColorManagerSess *colorSess, Int32 bitDepth, Double srcRefLuminance);
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
	public:
		GUIVideoBoxDD(GUICore *ui, UI::GUIClientControl *parent, Media::ColorManagerSess *colorSess, UOSInt buffCnt, UOSInt threadCnt);
		virtual ~GUIVideoBoxDD();

		virtual const UTF8Char *GetObjectClass();
		virtual OSInt OnNotify(Int32 code, void *lParam);
		virtual void OnSizeChanged(Bool updateScn);

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

		virtual void YUVParamChanged(const Media::IColorHandler::YUVPARAM *yuvParam);
		virtual void RGBParamChanged(const Media::IColorHandler::RGBPARAM2 *rgbParam);

		virtual void OnMonitorChanged();

		virtual void OnSurfaceCreated();
		virtual void OnMouseWheel(OSInt x, OSInt y, Int32 amount);
		virtual void OnMouseMove(OSInt x, OSInt y);
		virtual void OnMouseDown(OSInt x, OSInt y, MouseButton button);
		virtual void OnMouseUp(OSInt x, OSInt y, MouseButton button);

		void GetDebugValues(DebugValue *dbg);
		void SetSrcRGBType(Media::CS::TransferType rgbType);
		void SetSrcPrimaries(Media::ColorProfile::ColorType colorType);
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

		void AddImgFilter(Media::IImgFilter *imgFilter);
		void HandleMouseActon(MouseActionHandler hdlr, void *userObj);
		void Snapshot();
	};
}
#endif
