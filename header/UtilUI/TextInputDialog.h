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
		NN<UI::GUILabel> lblMessage;
		NN<UI::GUITextBox> txtInput;
		NN<UI::GUIButton> btnOk;
		NN<UI::GUIButton> btnCancel;
		Media::MonitorMgr *monMgr;
		Text::String *retInput;

		static void __stdcall OnOKClicked(AnyType userObj);
		static void __stdcall OnCancelClicked(AnyType userObj);
	public:
		TextInputDialog(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, Media::MonitorMgr *monMgr, Text::CStringNN title, Text::CStringNN message);
		virtual ~TextInputDialog();

		virtual void OnShow();
		virtual void OnMonitorChanged();

		void SetInputString(Text::CStringNN s);
		Bool GetInputString(NN<Text::StringBuilderUTF8> sb);
	};
}
#endif
