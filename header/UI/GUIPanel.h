#ifndef _SM_UI_GUIPANEL
#define _SM_UI_GUIPANEL
#include "UI/GUIClientControl.h"
#include "UI/GUIPanelBase.h"

namespace UI
{
	class GUIPanel : public GUIClientControl
	{
	private:
		NN<UI::GUIPanelBase> base;
	public:
		GUIPanel(NN<GUICore> ui, Optional<ControlHandle> parentHWnd);
		GUIPanel(NN<GUICore> ui, NN<UI::GUIClientControl> parent);
		virtual ~GUIPanel();

		virtual Bool IsChildVisible();

		virtual Math::Coord2DDbl GetClientOfst();
		virtual Math::Size2DDbl GetClientSize();
		virtual Text::CStringNN GetObjectClass() const;
		virtual IntOS OnNotify(UInt32 code, void *lParam);

		void SetMinSize(Int32 minW, Int32 minH);
		void SetHandle(Optional<ControlHandle> hwnd);
		NN<UI::GUIPanelBase> GetBase() const;
	};
}
#endif
