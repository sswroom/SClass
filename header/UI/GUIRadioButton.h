#ifndef _SM_UI_GUIRADIOBUTTON
#define _SM_UI_GUIRADIOBUTTON
#include "UI/GUIClientControl.h"

namespace UI
{
	class GUIRadioButton : public GUIControl
	{
	public:
		typedef void (__stdcall *SelectedChangeHandler)(void *userObj, Bool newState);
	private:
		Bool selected;
		Data::ArrayList<SelectedChangeHandler> *selectedChangeHdlrs;
		Data::ArrayList<void*> *selectedChangeObjs;

	public:
		void ChangeSelected(Bool selVal);
	public:
		GUIRadioButton(GUICore *ui, UI::GUIClientControl *parent, Text::CString initText, Bool selected);
		virtual ~GUIRadioButton();

		virtual Text::CString GetObjectClass();
		virtual OSInt OnNotify(UInt32 code, void *lParam);

		Bool IsSelected();
		void Select();

		void HandleSelectedChange(SelectedChangeHandler hdlr, void *userObj);
	};
};

#endif
