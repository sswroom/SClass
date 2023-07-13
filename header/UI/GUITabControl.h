#ifndef _SM_UI_GUITABCONTROL
#define _SM_UI_GUITABCONTROL
#include "Text/CString.h"
#include "UI/GUIClientControl.h"

namespace UI
{
	class GUITabPage;

	class GUITabControl : public GUIControl
	{
	private:
		Data::ArrayList<UI::GUITabPage*> tabPages;
		Data::ArrayList<UIEvent> selChgHdlrs;
		Data::ArrayList<void *> selChgObjs;
		UOSInt selIndex;
		void *oriWndProc;
		void *hbrBackground;

		static OSInt __stdcall TCWndProc(void *hWnd, UInt32 msg, UOSInt wParam, OSInt lParam);
	public:
		GUITabControl(GUICore *ui, UI::GUIClientControl *parent);
		virtual ~GUITabControl();

		GUITabPage *AddTabPage(NotNullPtr<Text::String> itemText);
		GUITabPage *AddTabPage(Text::CString itemText);
		void SetSelectedIndex(UOSInt index);
		void SetSelectedPage(GUITabPage *page);
		UOSInt GetSelectedIndex();
		GUITabPage *GetSelectedPage();
		void SetTabPageName(UOSInt index, Text::CString name);
		UTF8Char *GetTabPageName(UOSInt index, UTF8Char *buff);
		void GetTabPageRect(OSInt *x, OSInt *y, UOSInt *w, UOSInt *h);
		void *GetTabPageFont();

		virtual Text::CString GetObjectClass();
		virtual OSInt OnNotify(UInt32 code, void *lParam);
		virtual void OnSizeChanged(Bool updateScn);
		void HandleSelChanged(UIEvent hdlr, void *userObj);
		virtual void SetDPI(Double hdpi, Double ddpi);

		void EventSelChange(UOSInt selIndex);
	};
}
#endif
