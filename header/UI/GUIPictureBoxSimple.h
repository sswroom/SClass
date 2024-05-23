#ifndef _SM_UI_GUIPICTUREBOXSIMPLE
#define _SM_UI_GUIPICTUREBOXSIMPLE
#include "Media/DrawEngine.h"
#include "Media/StaticImage.h"
#include "UI/GUIControl.h"

namespace UI
{
	class GUIPictureBoxSimple : public GUIControl
	{
	protected:
		Bool hasBorder;
		Bool noBGColor;
		NN<Media::DrawEngine> eng;
		Data::ArrayList<MouseEventHandler> mouseDownHdlrs;
		Data::ArrayList<AnyType > mouseDownObjs;
		Data::ArrayList<MouseEventHandler> mouseMoveHdlrs;
		Data::ArrayList<AnyType> mouseMoveObjs;
		Data::ArrayList<MouseEventHandler> mouseUpHdlrs;
		Data::ArrayList<AnyType> mouseUpObjs;

	public:
		GUIPictureBoxSimple(NN<GUICore> ui, NN<UI::GUIClientControl> parent, NN<Media::DrawEngine> eng, Bool hasBorder);
		virtual ~GUIPictureBoxSimple();

		virtual OSInt OnNotify(UInt32 code, void *lParam) = 0;
		virtual void SetImage(Optional<Media::StaticImage> currImage) = 0;
		virtual void SetImageDImg(Optional<Media::DrawImage> img) = 0;

		virtual Text::CStringNN GetObjectClass() const;
		void SetNoBGColor(Bool noBGColor);

		void HandleMouseDown(MouseEventHandler hdlr, AnyType userObj);
		void HandleMouseMove(MouseEventHandler hdlr, AnyType userObj);
		void HandleMouseUp(MouseEventHandler hdlr, AnyType userObj);

		void EventButtonDown(Math::Coord2D<OSInt> pos, UI::GUIControl::MouseButton btn);
		void EventButtonUp(Math::Coord2D<OSInt> pos, UI::GUIControl::MouseButton btn);
		void EventMouseMove(Math::Coord2D<OSInt> pos);
	};
}
#endif
