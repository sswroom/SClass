#ifndef _SM_UI_GUITABCONTROL
#define _SM_UI_GUITABCONTROL
#include "Data/ArrayListNN.h"
#include "Text/CString.h"
#include "UI/GUIClientControl.h"

namespace UI
{
	class GUITabPage;

	class GUITabControl : public GUIControl
	{
	protected:
		Data::ArrayList<UIEvent> selChgHdlrs;
		Data::ArrayList<AnyType> selChgObjs;

	public:
		GUITabControl(NN<GUICore> ui, NN<UI::GUIClientControl> parent);
		virtual ~GUITabControl();

		virtual Text::CStringNN GetObjectClass() const;
		void *GetTabPageFont();
		void HandleSelChanged(UIEvent hdlr, AnyType userObj);
		void EventSelChange();

		virtual NN<GUITabPage> AddTabPage(NN<Text::String> itemText) = 0;
		virtual NN<GUITabPage> AddTabPage(Text::CStringNN itemText) = 0;
		virtual void SetSelectedIndex(UOSInt index) = 0;
		virtual void SetSelectedPage(NN<GUITabPage> page) = 0;
		virtual UOSInt GetSelectedIndex() = 0;
		virtual Optional<GUITabPage> GetSelectedPage() = 0;
		virtual void SetTabPageName(UOSInt index, Text::CStringNN name) = 0;
		virtual UTF8Char *GetTabPageName(UOSInt index, UTF8Char *buff) = 0;
		virtual Math::RectArea<OSInt> GetTabPageRect() = 0;

		virtual OSInt OnNotify(UInt32 code, void *lParam) = 0;
		virtual void OnSizeChanged(Bool updateScn) = 0;
		virtual void SetDPI(Double hdpi, Double ddpi) = 0;
	};
}
#endif
