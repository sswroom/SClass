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
		OSInt index;
		void *custObj;
	public:
		GUITabPage(GUICore *ui, UI::GUIClientControl *parent, UI::GUITabControl *ctrl, OSInt index);
		virtual ~GUITabPage();

		virtual void *GetFont();
		virtual Bool IsChildVisible();
		virtual Bool IsFormFocused();

		virtual const UTF8Char *GetObjectClass();
		virtual OSInt OnNotify(Int32 code, void *lParam);
		virtual GUIClientControl *GetParent();

		void SetCustObj(void *custObj);
		void *GetCustObj();
	};
};
#endif
