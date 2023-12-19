#ifndef _SM_UI_WIN_WINTABCONTROL
#define _SM_UI_WIN_WINTABCONTROL
#include "UI/GUITabControl.h"
#include <windows.h>

namespace UI
{
	class GUITabPage;

	namespace Win
	{
		class WinTabControl : public GUITabControl
		{
		private:
			Data::ArrayListNN<UI::GUITabPage> tabPages;
			UOSInt selIndex;
			WNDPROC oriWndProc;
			HBRUSH hbrBackground;

			static OSInt __stdcall TCWndProc(void *hWnd, UInt32 msg, UOSInt wParam, OSInt lParam);
		public:
			WinTabControl(NotNullPtr<GUICore> ui, NotNullPtr<UI::GUIClientControl> parent);
			virtual ~WinTabControl();

			virtual NotNullPtr<GUITabPage> AddTabPage(NotNullPtr<Text::String> itemText);
			virtual NotNullPtr<GUITabPage> AddTabPage(Text::CStringNN itemText);
			virtual void SetSelectedIndex(UOSInt index);
			virtual void SetSelectedPage(NotNullPtr<GUITabPage> page);
			virtual UOSInt GetSelectedIndex();
			virtual Optional<GUITabPage> GetSelectedPage();
			virtual void SetTabPageName(UOSInt index, Text::CStringNN name);
			virtual UTF8Char *GetTabPageName(UOSInt index, UTF8Char *buff);
			virtual Math::RectArea<OSInt> GetTabPageRect();

			virtual OSInt OnNotify(UInt32 code, void *lParam);
			virtual void OnSizeChanged(Bool updateScn);
			virtual void SetDPI(Double hdpi, Double ddpi);
		};
	}
}
#endif
