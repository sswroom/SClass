#ifndef _SM_UI_GUIVIDEOBOXDD
#define _SM_UI_GUIVIDEOBOXDD
#include "IO/Stream.h"
#include "IO/Writer.h"
#include "Media/ColorManager.h"
#include "Media/VideoRenderer.h"
#include "UI/GUIDDrawControl.h"

namespace UI
{
	class GUIVideoBoxDD : public GUIDDrawControl, public Media::IColorHandler, public Media::VideoRenderer
	{
	public:

		typedef struct
		{
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
			UOSInt srcWidth;
			UOSInt srcHeight;
			UOSInt dispWidth;
			UOSInt dispHeight;
			UOSInt seekCnt;
			Double par;
			UInt32 dispBitDepth;
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
		IO::Writer *debugLog;
		IO::Stream *debugFS;
		IO::Writer *debugLog2;
		IO::Stream *debugFS2;

		MouseActionHandler maHdlr;
		void *maHdlrObj;
		Bool maDown;
		OSInt maDownX;
		OSInt maDownY;
		Int64 maDownTime;

	protected:
		//void UpdateFromBuff(VideoBuff *vbuff);
		virtual void LockUpdateSize(Sync::MutexUsage *mutUsage);
		virtual void DrawFromMem(UInt8 *memPtr, OSInt lineAdd, OSInt destX, OSInt destY, UOSInt buffWidth, UOSInt buffHeight, Bool clearScn);

		virtual void BeginUpdateSize();
		virtual void EndUpdateSize();
	public:
		GUIVideoBoxDD(GUICore *ui, UI::GUIClientControl *parent, Media::ColorManagerSess *colorSess, UOSInt buffCnt, UOSInt threadCnt);
		virtual ~GUIVideoBoxDD();

		virtual const UTF8Char *GetObjectClass();
		virtual OSInt OnNotify(UInt32 code, void *lParam);
		virtual void OnSizeChanged(Bool updateScn);

		virtual void YUVParamChanged(const Media::IColorHandler::YUVPARAM *yuvParam);
		virtual void RGBParamChanged(const Media::IColorHandler::RGBPARAM2 *rgbParam);

		virtual void OnMonitorChanged();

		virtual void OnSurfaceCreated();
		virtual void OnMouseWheel(OSInt x, OSInt y, Int32 amount);
		virtual void OnMouseMove(OSInt x, OSInt y);
		virtual void OnMouseDown(OSInt x, OSInt y, MouseButton button);
		virtual void OnMouseUp(OSInt x, OSInt y, MouseButton button);

		void GetDebugValues(DebugValue *dbg);
		void HandleMouseActon(MouseActionHandler hdlr, void *userObj);

		virtual void DestroyObject();
	};
}
#endif
