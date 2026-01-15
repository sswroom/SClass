#ifndef _SM_UI_JAVAUI_JUIHSPLITTER
#define _SM_UI_JAVAUI_JUIHSPLITTER
#include "UI/GUIHSplitter.h"

namespace UI
{
	namespace JavaUI
	{
		class JUIHSplitter : public UI::GUIHSplitter
		{
		private:
			Bool isRight;
			Bool dragMode;
			IntOS dragX;
			IntOS dragY;

		public:
			JUIHSplitter(NN<GUICore> ui, NN<UI::GUIClientControl> parent, Int32 width, Bool isRight);
			virtual ~JUIHSplitter();

			virtual IntOS OnNotify(UInt32 code, void *lParam);
			virtual void EventMouseDown(UI::GUIControl::MouseButton btn, Math::Coord2D<IntOS> pos);
			virtual void EventMouseUp(UI::GUIControl::MouseButton btn, Math::Coord2D<IntOS> pos);
		};
	}
}
#endif
