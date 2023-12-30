#ifndef _SM_UI_GUIPICTUREBOX
#define _SM_UI_GUIPICTUREBOX
#include "Media/DrawEngine.h"
#include "Media/IImgResizer.h"
#include "Media/StaticImage.h"
#include "UI/GUIControl.h"

namespace UI
{
	class GUIPictureBox : public GUIControl
	{
	protected:
		Media::IImgResizer *resizer;
		Media::StaticImage *currImage;
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
	
		virtual void UpdatePreview() = 0;
	public:
		GUIPictureBox(NotNullPtr<GUICore> ui, NotNullPtr<UI::GUIClientControl> parent, NotNullPtr<Media::DrawEngine> eng, Bool hasBorder, Bool allowResize);
		virtual ~GUIPictureBox();

		virtual OSInt OnNotify(UInt32 code, void *lParam) = 0;

		virtual Text::CStringNN GetObjectClass() const;
		virtual void OnSizeChanged(Bool updateScn);
		void SetImage(Media::StaticImage *currImage);
		void SetNoBGColor(Bool noBGColor);

		void HandleMouseDown(MouseEventHandler hdlr, void *userObj);
		void HandleMouseMove(MouseEventHandler hdlr, void *userObj);
		void HandleMouseUp(MouseEventHandler hdlr, void *userObj);

		void EventButtonDown(Math::Coord2D<OSInt> pos, UI::GUIControl::MouseButton btn);
		void EventButtonUp(Math::Coord2D<OSInt> pos, UI::GUIControl::MouseButton btn);
		void EventMouseMove(Math::Coord2D<OSInt> pos);
	};
}
#endif
