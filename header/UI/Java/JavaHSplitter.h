#ifndef _SM_UI_JAVA_JAVAHSPLITTER
#define _SM_UI_JAVA_JAVAHSPLITTER
#include "UI/GUIHSplitter.h"

namespace UI
{
	namespace Java
	{
		class JavaHSplitter : public UI::GUIHSplitter
		{
		private:
			Bool isRight;
			Bool dragMode;
			OSInt dragX;
			OSInt dragY;

		public:
			JavaHSplitter(NotNullPtr<GUICore> ui, NotNullPtr<UI::GUIClientControl> parent, Int32 width, Bool isRight);
			virtual ~JavaHSplitter();

			virtual OSInt OnNotify(UInt32 code, void *lParam);
			virtual void EventMouseDown(UI::GUIControl::MouseButton btn, Math::Coord2D<OSInt> pos);
			virtual void EventMouseUp(UI::GUIControl::MouseButton btn, Math::Coord2D<OSInt> pos);
		};
	}
}
#endif
