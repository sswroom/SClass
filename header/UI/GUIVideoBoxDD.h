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
		typedef void (__stdcall *MouseActionHandler)(AnyType userObj, MouseAction ma, Math::Coord2D<OSInt> pos);
	protected:
		IO::Writer *debugLog;
		IO::Stream *debugFS;
		IO::Writer *debugLog2;
		IO::Stream *debugFS2;

		MouseActionHandler maHdlr;
		AnyType maHdlrObj;
		Bool maDown;
		Math::Coord2D<OSInt> maDownPos;
		Int64 maDownTime;

	protected:
		//void UpdateFromBuff(VideoBuff *vbuff);
		virtual void LockUpdateSize(NN<Sync::MutexUsage> mutUsage);
		virtual void DrawFromSurface(NN<Media::MonitorSurface> surface, Math::Coord2D<OSInt> destTL, Math::Size2D<UOSInt> buffSize, Bool clearScn);

		virtual void BeginUpdateSize();
		virtual void EndUpdateSize();
	public:
		GUIVideoBoxDD(NN<GUICore> ui, NN<UI::GUIClientControl> parent, NN<Media::ColorManagerSess> colorSess, UOSInt buffCnt, UOSInt threadCnt);
		virtual ~GUIVideoBoxDD();

		virtual Text::CStringNN GetObjectClass() const;
		virtual OSInt OnNotify(UInt32 code, void *lParam);
		virtual void OnSizeChanged(Bool updateScn);

		virtual void YUVParamChanged(NN<const Media::IColorHandler::YUVPARAM> yuvParam);
		virtual void RGBParamChanged(NN<const Media::IColorHandler::RGBPARAM2> rgbParam);

		virtual void OnMonitorChanged();

		virtual void OnSurfaceCreated();
		virtual void OnMouseWheel(Math::Coord2D<OSInt> pos, Int32 amount);
		virtual void OnMouseMove(Math::Coord2D<OSInt> pos);
		virtual void OnMouseDown(Math::Coord2D<OSInt> pos, MouseButton button);
		virtual void OnMouseUp(Math::Coord2D<OSInt> pos, MouseButton button);

		void HandleMouseActon(MouseActionHandler hdlr, AnyType userObj);

		virtual void DestroyObject();
		virtual void SetRotateType(Media::RotateType rotateType);
		virtual Media::RotateType GetRotateType() const;
	};
}
#endif
