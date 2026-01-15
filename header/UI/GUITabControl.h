#ifndef _SM_UI_GUITABCONTROL
#define _SM_UI_GUITABCONTROL
#include "Data/ArrayListNN.hpp"
#include "Text/CString.h"
#include "UI/GUIClientControl.h"

namespace UI
{
	class GUITabPage;

	class GUITabControl : public GUIControl
	{
	protected:
		Data::ArrayListObj<UIEvent> selChgHdlrs;
		Data::ArrayListObj<AnyType> selChgObjs;

	public:
		GUITabControl(NN<GUICore> ui, NN<UI::GUIClientControl> parent);
		virtual ~GUITabControl();

		virtual Text::CStringNN GetObjectClass() const;
		void *GetTabPageFont();
		void HandleSelChanged(UIEvent hdlr, AnyType userObj);
		void EventSelChange();

		virtual NN<GUITabPage> AddTabPage(NN<Text::String> itemText) = 0;
		virtual NN<GUITabPage> AddTabPage(Text::CStringNN itemText) = 0;
		virtual void SetSelectedIndex(UIntOS index) = 0;
		virtual void SetSelectedPage(NN<GUITabPage> page) = 0;
		virtual UIntOS GetSelectedIndex() = 0;
		virtual Optional<GUITabPage> GetSelectedPage() = 0;
		virtual void SetTabPageName(UIntOS index, Text::CStringNN name) = 0;
		virtual UnsafeArrayOpt<UTF8Char> GetTabPageName(UIntOS index, UnsafeArray<UTF8Char> buff) = 0;
		virtual Math::RectArea<IntOS> GetTabPageRect() = 0;

		virtual IntOS OnNotify(UInt32 code, void *lParam) = 0;
		virtual void OnSizeChanged(Bool updateScn) = 0;
		virtual void SetDPI(Double hdpi, Double ddpi) = 0;
	};
}
#endif
