#ifndef _SM_UI_GTK_GTKPANELBASE
#define _SM_UI_GTK_GTKPANELBASE
#include "Handles.h"
#include "UI/GUIPanelBase.h"

namespace UI
{
	namespace GTK
	{
		class GTKPanelBase : public UI::GUIPanelBase
		{
		protected:
			NotNullPtr<GUIPanel> master;

		public:
			GTKPanelBase(NotNullPtr<GUIPanel> master, NotNullPtr<GUICore> ui, ControlHandle *parentHWnd);
			GTKPanelBase(NotNullPtr<GUIPanel> master, NotNullPtr<GUICore> ui, NotNullPtr<UI::GUIClientControl> parent);
			virtual ~GTKPanelBase();

			virtual Math::Coord2DDbl GetClientOfst();
			virtual Math::Size2DDbl GetClientSize();
			virtual void SetMinSize(Int32 minW, Int32 minH);
		};
	}
}
#endif
