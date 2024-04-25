#ifndef _SM_UI_JAVA_JAVAGROUPBOX
#define _SM_UI_JAVA_JAVAGROUPBOX
#include "UI/GUIGroupBox.h"

namespace UI
{
	namespace Java
	{
		class JavaGroupBox : public GUIGroupBox
		{
		public:
			JavaGroupBox(NN<GUICore> ui, NN<UI::GUIClientControl> parent, Text::CStringNN name);
			virtual ~JavaGroupBox();

			virtual Math::Coord2DDbl GetClientOfst();
			virtual Math::Size2DDbl GetClientSize();
		};
	}
}
#endif
