#ifndef _SM_UI_GUICLIENTCONTROL
#define _SM_UI_GUICLIENTCONTROL
#include "UI/GUIControl.h"

namespace UI
{
	class GUIClientControl : public UI::GUIControl
	{
	protected:
		Data::ArrayList<UI::GUIControl*> *children;
		Double undockLeft;
		Double undockTop;
		Double undockRight;
		Double undockBottom;
		Bool hasFillCtrl;
		void *container;

		void InitContainer();
	protected:
		GUIClientControl(GUICore *ui, UI::GUIClientControl *parent);
		GUIClientControl(Bool forVirtualUse);
		virtual void UpdateFont();
		void ClearChildren();
		Bool MyEraseBkg(void *hdc);
	public:
		virtual ~GUIClientControl();

		virtual void GetClientOfst(Double *x, Double *y);
		virtual void GetClientSize(Double *w, Double *h);
		virtual void AddChild(GUIControl *child);
		virtual Bool IsChildVisible() = 0;
		
		UOSInt GetChildCount();
		UI::GUIControl *GetChild(UOSInt index);
		void FocusChild(GUIControl *child);

		void UpdateChildrenSize(Bool redraw);
		virtual void OnSizeChanged(Bool updateScn);
		virtual void SetDPI(Double hdpi, Double ddpi);

		void *GetContainer();
	};
}
#endif
