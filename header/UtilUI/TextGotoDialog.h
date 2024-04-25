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
		NN<UI::GUILabel> lblLine;
		NN<UI::GUITextBox> txtLine;
		NN<UI::GUIButton> btnOk;
		NN<UI::GUIButton> btnCancel;
		Media::MonitorMgr *monMgr;
		UOSInt currLine;

		static void __stdcall OnOKClicked(AnyType userObj);
		static void __stdcall OnCancelClicked(AnyType userObj);
	public:
		TextGotoDialog(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, Media::MonitorMgr *monMgr);
		virtual ~TextGotoDialog();

		virtual void OnShow();
		virtual void OnMonitorChanged();

		void SetLineNumber(UOSInt currLine);
		UOSInt GetLineNumber();
	};
}
#endif
