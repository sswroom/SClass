#ifndef _SM_UTILUI_TEXTINPUTDIALOG
#define _SM_UTILUI_TEXTINPUTDIALOG
#include "Media/MonitorMgr.h"
#include "UI/GUIButton.h"
#include "UI/GUIForm.h"
#include "UI/GUITextBox.h"
#include "UI/GUILabel.h"

namespace UtilUI
{
	class TextInputDialog : public UI::GUIForm
	{
	private:
		UI::GUILabel *lblMessage;
		UI::GUITextBox *txtInput;
		UI::GUIButton *btnOk;
		UI::GUIButton *btnCancel;
		Media::MonitorMgr *monMgr;
		Text::String *retInput;

		static void __stdcall OnOKClicked(void *userObj);
		static void __stdcall OnCancelClicked(void *userObj);
	public:
		TextInputDialog(UI::GUIClientControl *parent, UI::GUICore *ui, Media::MonitorMgr *monMgr, Text::CString title, Text::CString message);
		virtual ~TextInputDialog();

		virtual void OnShow();
		virtual void OnMonitorChanged();

		Bool GetInputString(Text::StringBuilderUTF8 *sb);
	};
}
#endif