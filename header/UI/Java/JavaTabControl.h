#ifndef _SM_UI_JAVA_JAVATABCONTROL
#define _SM_UI_JAVA_JAVATABCONTROL
#include "UI/GUITabControl.h"

namespace UI
{
	class GUITabPage;

	namespace Java
	{
		class JavaTabControl : public GUITabControl
		{
		private:
			Data::ArrayListNN<UI::GUITabPage> tabPages;
			UOSInt selIndex;

		public:
			JavaTabControl(NotNullPtr<GUICore> ui, NotNullPtr<UI::GUIClientControl> parent);
			virtual ~JavaTabControl();

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
