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

		void HandleMouseActon(MouseActionHandler hdlr, void *userObj);

		virtual void DestroyObject();
	};
}
#endif
