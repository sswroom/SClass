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
			IntOS dragX;
			IntOS dragY;

		public:
			JUIVSplitter(NN<GUICore> ui, NN<UI::GUIClientControl> parent, Int32 height, Bool isBottom);
			virtual ~JUIVSplitter();

			virtual IntOS OnNotify(UInt32 code, void *lParam);
			virtual void EventMouseDown(UI::GUIControl::MouseButton btn, Math::Coord2D<IntOS> pos);
			virtual void EventMouseUp(UI::GUIControl::MouseButton btn, Math::Coord2D<IntOS> pos);
		};
	}
}
#endif
