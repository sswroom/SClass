#ifndef _SM_UI_JAVAUI_JUITABCONTROL
#define _SM_UI_JAVAUI_JUITABCONTROL
#include "UI/GUITabControl.h"

namespace UI
{
	class GUITabPage;

	namespace JavaUI
	{
		class JUITabControl : public GUITabControl
		{
		private:
			Data::ArrayListNN<UI::GUITabPage> tabPages;
			UOSInt selIndex;

		public:
			JUITabControl(NN<GUICore> ui, NN<UI::GUIClientControl> parent);
			virtual ~JUITabControl();

			virtual NN<GUITabPage> AddTabPage(NN<Text::String> itemText);
			virtual NN<GUITabPage> AddTabPage(Text::CStringNN itemText);
			virtual void SetSelectedIndex(UOSInt index);
			virtual void SetSelectedPage(NN<GUITabPage> page);
			virtual UOSInt GetSelectedIndex();
			virtual Optional<GUITabPage> GetSelectedPage();
			virtual void SetTabPageName(UOSInt index, Text::CStringNN name);
			virtual UnsafeArrayOpt<UTF8Char> GetTabPageName(UOSInt index, UnsafeArray<UTF8Char> buff);
			virtual Math::RectArea<OSInt> GetTabPageRect();

			virtual OSInt OnNotify(UInt32 code, void *lParam);
			virtual void OnSizeChanged(Bool updateScn);
			virtual void SetDPI(Double hdpi, Double ddpi);
		};
	}
}
#endif
