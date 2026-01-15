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
			UIntOS selIndex;
			WNDPROC oriWndProc;
			HBRUSH hbrBackground;

			static IntOS __stdcall TCWndProc(void *hWnd, UInt32 msg, UIntOS wParam, IntOS lParam);
		public:
			WinTabControl(NN<GUICore> ui, NN<UI::GUIClientControl> parent);
			virtual ~WinTabControl();

			virtual NN<GUITabPage> AddTabPage(NN<Text::String> itemText);
			virtual NN<GUITabPage> AddTabPage(Text::CStringNN itemText);
			virtual void SetSelectedIndex(UIntOS index);
			virtual void SetSelectedPage(NN<GUITabPage> page);
			virtual UIntOS GetSelectedIndex();
			virtual Optional<GUITabPage> GetSelectedPage();
			virtual void SetTabPageName(UIntOS index, Text::CStringNN name);
			virtual UnsafeArrayOpt<UTF8Char> GetTabPageName(UIntOS index, UnsafeArray<UTF8Char> buff);
			virtual Math::RectArea<IntOS> GetTabPageRect();

			virtual IntOS OnNotify(UInt32 code, void *lParam);
			virtual void OnSizeChanged(Bool updateScn);
			virtual void SetDPI(Double hdpi, Double ddpi);
		};
	}
}
#endif
