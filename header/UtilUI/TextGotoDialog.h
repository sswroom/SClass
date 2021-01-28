#ifndef _SM_UTILUI_TEXTGOTODIALOG
#define _SM_UTILUI_TEXTGOTODIALOG
#include "Media/MonitorMgr.h"
#include "UI/GUIButton.h"
#include "UI/GUIForm.h"
#include "UI/GUITextBox.h"
#include "UI/GUILabel.h"

namespace UtilUI
{
	class TextGotoDialog : public UI::GUIForm
	{
	private:
		UI::GUILabel *lblLine;
		UI::GUITextBox *txtLine;
		UI::GUIButton *btnOk;
		UI::GUIButton *btnCancel;
		Media::MonitorMgr *monMgr;
		OSInt currLine;

		static void __stdcall OnOKClicked(void *userObj);
		static void __stdcall OnCancelClicked(void *userObj);
	public:
		TextGotoDialog(UI::GUIClientControl *parent, UI::GUICore *ui, Media::MonitorMgr *monMgr);
		virtual ~TextGotoDialog();

		virtual void OnShow();
		virtual void OnMonitorChanged();

		void SetLineNumber(OSInt currLine);
		OSInt GetLineNumber();
	};
};
#endif
