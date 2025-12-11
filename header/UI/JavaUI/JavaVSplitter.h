#ifndef _SM_UI_JAVAUI_JAVAVSPLITTER
#define _SM_UI_JAVAUI_JAVAVSPLITTER
#include "UI/GUIVSplitter.h"

namespace UI
{
	namespace JavaUI
	{
		class JavaVSplitter : public GUIVSplitter
		{
		private:
			Bool isBottom;
			Bool dragMode;
			OSInt dragX;
			OSInt dragY;

		public:
			JavaVSplitter(NN<GUICore> ui, NN<UI::GUIClientControl> parent, Int32 height, Bool isBottom);
			virtual ~JavaVSplitter();

			virtual OSInt OnNotify(UInt32 code, void *lParam);
			virtual void EventMouseDown(UI::GUIControl::MouseButton btn, Math::Coord2D<OSInt> pos);
			virtual void EventMouseUp(UI::GUIControl::MouseButton btn, Math::Coord2D<OSInt> pos);
		};
	}
}
#endif
