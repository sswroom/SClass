#ifndef _SM_UI_GUICLIENTCONTROL
#define _SM_UI_GUICLIENTCONTROL
#include "UI/GUIControl.h"

namespace UI
{
	class GUIClientControl : public UI::GUIControl
	{
	protected:
		Data::ArrayList<UI::GUIControl*> children;
		Double undockLeft;
		Double undockTop;
		Double undockRight;
		Double undockBottom;
		Bool hasFillCtrl;
		void *container;

		void InitContainer();
	protected:
		GUIClientControl(NotNullPtr<GUICore> ui, Optional<UI::GUIClientControl> parent);
		virtual void UpdateFont();
		void ClearChildren();
		Bool MyEraseBkg(void *hdc);
	public:
		virtual ~GUIClientControl();

		virtual Math::Coord2DDbl GetClientOfst();
		virtual Math::Size2DDbl GetClientSize();
		virtual void AddChild(GUIControl *child);
		virtual Bool IsChildVisible() = 0;
		
		UOSInt GetChildCount();
		UI::GUIControl *GetChild(UOSInt index);
		void FocusChild(GUIControl *child);

		void UpdateChildrenSize(Bool redraw);
		virtual void OnSizeChanged(Bool updateScn);
		virtual void SetDPI(Double hdpi, Double ddpi);

		void *GetContainer();

		virtual void DestroyObject();
	};
}
#endif
