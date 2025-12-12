#ifndef _SM_UI_JAVAUI_JUIVSPLITTER
#define _SM_UI_JAVAUI_JUIVSPLITTER
#include "UI/GUIVSplitter.h"

namespace UI
{
	namespace JavaUI
	{
		class JUIVSplitter : public GUIVSplitter
		{
		private:
			Bool isBottom;
			Bool dragMode;
			OSInt dragX;
			OSInt dragY;

		public:
			JUIVSplitter(NN<GUICore> ui, NN<UI::GUIClientControl> parent, Int32 height, Bool isBottom);
			virtual ~JUIVSplitter();

			virtual OSInt OnNotify(UInt32 code, void *lParam);
			virtual void EventMouseDown(UI::GUIControl::MouseButton btn, Math::Coord2D<OSInt> pos);
			virtual void EventMouseUp(UI::GUIControl::MouseButton btn, Math::Coord2D<OSInt> pos);
		};
	}
}
#endif
