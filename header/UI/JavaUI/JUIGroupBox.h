#ifndef _SM_UI_JAVAUI_JUIGROUPBOX
#define _SM_UI_JAVAUI_JUIGROUPBOX
#include "UI/GUIGroupBox.h"

namespace UI
{
	namespace JavaUI
	{
		class JUIGroupBox : public GUIGroupBox
		{
		public:
			JUIGroupBox(NN<GUICore> ui, NN<UI::GUIClientControl> parent, Text::CStringNN name);
			virtual ~JUIGroupBox();

			virtual Math::Coord2DDbl GetClientOfst();
			virtual Math::Size2DDbl GetClientSize();
		};
	}
}
#endif
