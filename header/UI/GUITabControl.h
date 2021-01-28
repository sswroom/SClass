#ifndef _SM_UI_GUITABCONTROL
#define _SM_UI_GUITABCONTROL
#include "UI/GUIClientControl.h"

namespace UI
{
	class GUITabPage;

	class GUITabControl : public GUIControl
	{
	private:
		Data::ArrayList<UI::GUITabPage*> *tabPages;
		Data::ArrayList<UIEvent> *selChgHdlrs;
		Data::ArrayList<void *> *selChgObjs;
		UOSInt selIndex;
		void *oriWndProc;
		void *hbrBackground;

		static OSInt __stdcall TCWndProc(void *hWnd, UInt32 msg, UOSInt wParam, OSInt lParam);
	public:
		GUITabControl(GUICore *ui, UI::GUIClientControl *parent);
		virtual ~GUITabControl();

		GUITabPage *AddTabPage(const UTF8Char *itemText);
		void SetSelectedIndex(UOSInt index);
		void SetSelectedPage(GUITabPage *page);
		UOSInt GetSelectedIndex();
		void SetTabPageName(UOSInt index, const UTF8Char *name);
		UTF8Char *GetTabPageName(UOSInt index, UTF8Char *buff);
		void GetTabPageRect(OSInt *x, OSInt *y, OSInt *w, OSInt *h);
		void *GetTabPageFont();

		virtual const UTF8Char *GetObjectClass();
		virtual OSInt OnNotify(Int32 code, void *lParam);
		virtual void OnSizeChanged(Bool updateScn);
		void HandleSelChanged(UIEvent hdlr, void *userObj);
		virtual void SetDPI(Double hdpi, Double ddpi);

		void EventSelChange(UOSInt selIndex);
	};
}
#endif
