#ifndef _SM_UI_JAVA_JAVAPANELBASE
#define _SM_UI_JAVA_JAVAPANELBASE
#include "Handles.h"
#include "UI/GUIPanelBase.h"

namespace UI
{
	namespace Java
	{
		class JavaPanelBase : public UI::GUIPanelBase
		{
		protected:
			NN<GUIPanel> master;

		public:
			JavaPanelBase(NN<GUIPanel> master, NN<GUICore> ui, Optional<ControlHandle> parentHWnd);
			JavaPanelBase(NN<GUIPanel> master, NN<GUICore> ui, NN<UI::GUIClientControl> parent);
			virtual ~JavaPanelBase();

			virtual Math::Coord2DDbl GetClientOfst();
			virtual Math::Size2DDbl GetClientSize();
			virtual void SetMinSize(Int32 minW, Int32 minH);
		};
	}
}
#endif
