#ifndef _SM_UI_GUICHECKBOX
#define _SM_UI_GUICHECKBOX
#include "Data/ArrayList.h"
#include "UI/GUIClientControl.h"

namespace UI
{
	class GUICheckBox : public GUIControl
	{
	public:
		typedef void (__stdcall *CheckedChangeHandler)(void *userObj, Bool newState);
	private:
		Bool checked;
		Data::ArrayList<CheckedChangeHandler> *checkedChangeHdlrs;
		Data::ArrayList<void*> *checkedChangeObjs;
	public:
		GUICheckBox(GUICore *ui, UI::GUIClientControl *parent, const UTF8Char *initText, Bool checked);
		virtual ~GUICheckBox();

		virtual const UTF8Char *GetObjectClass();
		virtual OSInt OnNotify(Int32 code, void *lParam);

		void EventCheckedChange(Bool newState);

		Bool IsChecked();
		void SetChecked(Bool checked);

		void HandleCheckedChange(CheckedChangeHandler hdlr, void *obj);
	};
};

#endif
