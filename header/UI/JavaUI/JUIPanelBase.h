#ifndef _SM_UI_JAVAUI_JUIPANELBASE
#define _SM_UI_JAVAUI_JUIPANELBASE
#include "Handles.h"
#include "UI/GUIPanelBase.h"

namespace UI
{
	namespace JavaUI
	{
		class JUIPanelBase : public UI::GUIPanelBase
		{
		protected:
			NN<GUIPanel> master;

		public:
			JUIPanelBase(NN<GUIPanel> master, NN<GUICore> ui, Optional<ControlHandle> parentHWnd);
			JUIPanelBase(NN<GUIPanel> master, NN<GUICore> ui, NN<UI::GUIClientControl> parent);
			virtual ~JUIPanelBase();

			virtual Math::Coord2DDbl GetClientOfst();
			virtual Math::Size2DDbl GetClientSize();
			virtual void SetMinSize(Int32 minW, Int32 minH);
		};
	}
}
#endif
