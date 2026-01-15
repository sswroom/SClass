#ifndef _SM_UI_GUITABPAGE
#define _SM_UI_GUITABPAGE
#include "UI/GUIPanel.h"
#include "UI/GUIClientControl.h"

namespace UI
{
	class GUITabControl;
	class GUITabPage : public GUIPanel
	{
	private:
		NN<UI::GUITabControl> ctrl;
		UIntOS index;
		void *custObj;
	public:
		GUITabPage(NN<GUICore> ui, Optional<UI::GUIClientControl> parent, NN<UI::GUITabControl> ctrl, UIntOS index);
		virtual ~GUITabPage();

		virtual void *GetFont();
		virtual Bool IsChildVisible();
		virtual Bool IsFormFocused();

		virtual Text::CStringNN GetObjectClass() const;
		virtual IntOS OnNotify(UInt32 code, void *lParam);
		virtual Optional<GUIClientControl> GetParent();

		void SetCustObj(void *custObj);
		void *GetCustObj();
	};
}
#endif
