#ifndef _SM_UI_GUIPICTUREBOX
#define _SM_UI_GUIPICTUREBOX
#include "Media/DrawEngine.h"
#include "Media/ImageResizer.h"
#include "Media/StaticImage.h"
#include "UI/GUIControl.h"

namespace UI
{
	class GUIPictureBox : public GUIControl
	{
	protected:
		Media::ImageResizer *resizer;
		Optional<Media::StaticImage> currImage;
		Bool hasBorder;
		Bool allowResize;
		Bool noBGColor;
		NN<Media::DrawEngine> eng;
		Data::ArrayListObj<MouseEventHandler> mouseDownHdlrs;
		Data::ArrayListObj<AnyType> mouseDownObjs;
		Data::ArrayListObj<MouseEventHandler> mouseMoveHdlrs;
		Data::ArrayListObj<AnyType> mouseMoveObjs;
		Data::ArrayListObj<MouseEventHandler> mouseUpHdlrs;
		Data::ArrayListObj<AnyType> mouseUpObjs;
	
		virtual void UpdatePreview() = 0;
	public:
		GUIPictureBox(NN<GUICore> ui, NN<UI::GUIClientControl> parent, NN<Media::DrawEngine> eng, Bool hasBorder, Bool allowResize);
		virtual ~GUIPictureBox();

		virtual IntOS OnNotify(UInt32 code, void *lParam) = 0;

		virtual Text::CStringNN GetObjectClass() const;
		virtual void OnSizeChanged(Bool updateScn);
		void SetImage(Optional<Media::StaticImage> currImage);
		void SetNoBGColor(Bool noBGColor);

		void HandleMouseDown(MouseEventHandler hdlr, AnyType userObj);
		void HandleMouseMove(MouseEventHandler hdlr, AnyType userObj);
		void HandleMouseUp(MouseEventHandler hdlr, AnyType userObj);

		void EventButtonDown(Math::Coord2D<IntOS> pos, UI::GUIControl::MouseButton btn);
		void EventButtonUp(Math::Coord2D<IntOS> pos, UI::GUIControl::MouseButton btn);
		void EventMouseMove(Math::Coord2D<IntOS> pos);
	};
}
#endif
