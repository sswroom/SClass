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
		UI::GUITabControl *ctrl;
		UOSInt index;
		void *custObj;
	public:
		GUITabPage(NotNullPtr<GUICore> ui, UI::GUIClientControl *parent, UI::GUITabControl *ctrl, UOSInt index);
		virtual ~GUITabPage();

		virtual void *GetFont();
		virtual Bool IsChildVisible();
		virtual Bool IsFormFocused();

		virtual Text::CString GetObjectClass();
		virtual OSInt OnNotify(UInt32 code, void *lParam);
		virtual GUIClientControl *GetParent();

		void SetCustObj(void *custObj);
		void *GetCustObj();
	};
}
#endif
