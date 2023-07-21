#ifndef _SM_UI_GUIPICTUREBOXSIMPLE
#define _SM_UI_GUIPICTUREBOXSIMPLE
#include "Media/DrawEngine.h"
#include "Media/StaticImage.h"

namespace UI
{
	class GUIPictureBoxSimple : public GUIControl
	{
	private:
		struct ClassData;
		static Int32 useCnt;
		ClassData *clsData;
		Media::StaticImage *currImage;
		Media::DrawImage *prevImageD;
		Bool hasBorder;
		Bool noBGColor;
		Media::DrawEngine *eng;
		Data::ArrayList<MouseEventHandler> mouseDownHdlrs;
		Data::ArrayList<void *> mouseDownObjs;
		Data::ArrayList<MouseEventHandler> mouseMoveHdlrs;
		Data::ArrayList<void *> mouseMoveObjs;
		Data::ArrayList<MouseEventHandler> mouseUpHdlrs;
		Data::ArrayList<void *> mouseUpObjs;

		static OSInt __stdcall PBWndProc(void *hWnd, UInt32 msg, UOSInt wParam, OSInt lParam);
		void OnPaint();
		void Init(void *hInst);
		void Deinit(void *hInst);
		void UpdatePreview();
	public:
		GUIPictureBoxSimple(NotNullPtr<GUICore> ui, UI::GUIClientControl *parent, Media::DrawEngine *eng, Bool hasBorder);
		virtual ~GUIPictureBoxSimple();

		virtual Text::CString GetObjectClass();
		virtual OSInt OnNotify(UInt32 code, void *lParam);

		void HandleMouseDown(MouseEventHandler hdlr, void *userObj);
		void HandleMouseMove(MouseEventHandler hdlr, void *userObj);
		void HandleMouseUp(MouseEventHandler hdlr, void *userObj);

		void SetImage(Media::StaticImage *currImage);
		void SetImageDImg(Media::DrawImage *img);
		void SetNoBGColor(Bool noBGColor);
	};
};
#endif
