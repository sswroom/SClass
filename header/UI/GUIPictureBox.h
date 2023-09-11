#ifndef _SM_UI_GUIPICTUREBOX
#define _SM_UI_GUIPICTUREBOX
#include "Media/ColorManager.h"
#include "Media/DrawEngine.h"
#include "Media/IImgResizer.h"
#include "Media/StaticImage.h"
#include "Media/CS/CSConverter.h"
#include "Parser/ParserList.h"
#include "UI/GUIDDrawControl.h"

namespace UI
{
	class GUIPictureBox : public GUIControl
	{
	private:
		static Int32 useCnt;
		Media::IImgResizer *resizer;
		Media::StaticImage *currImage;
		Media::DrawImage *prevImageD;
		Bool hasBorder;
		Bool allowResize;
		Bool noBGColor;
		NotNullPtr<Media::DrawEngine> eng;
		Data::ArrayList<MouseEventHandler> mouseDownHdlrs;
		Data::ArrayList<void *> mouseDownObjs;
		Data::ArrayList<MouseEventHandler> mouseMoveHdlrs;
		Data::ArrayList<void *> mouseMoveObjs;
		Data::ArrayList<MouseEventHandler> mouseUpHdlrs;
		Data::ArrayList<void *> mouseUpObjs;
		void *clsData;

		static OSInt __stdcall PBWndProc(void *hWnd, UInt32 msg, UInt32 wParam, OSInt lParam);
		void OnPaint();
		void Init(void *hInst);
		void Deinit(void *hInst);
		void UpdatePreview();
	public:
		GUIPictureBox(NotNullPtr<GUICore> ui, UI::GUIClientControl *parent, NotNullPtr<Media::DrawEngine> eng, Bool hasBorder, Bool allowResize);
		virtual ~GUIPictureBox();

		virtual Text::CStringNN GetObjectClass() const;
		virtual OSInt OnNotify(UInt32 code, void *lParam);
		virtual void OnSizeChanged(Bool updateScn);

		void HandleMouseDown(MouseEventHandler hdlr, void *userObj);
		void HandleMouseMove(MouseEventHandler hdlr, void *userObj);
		void HandleMouseUp(MouseEventHandler hdlr, void *userObj);

		void SetImage(Media::StaticImage *currImage);
		void SetNoBGColor(Bool noBGColor);

		void EventButtonDown(OSInt x, OSInt y, UI::GUIControl::MouseButton btn);
		void EventButtonUp(OSInt x, OSInt y, UI::GUIControl::MouseButton btn);
		void EventMouseMove(OSInt x, OSInt y);
	};
}
#endif
