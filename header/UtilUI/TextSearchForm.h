#ifndef _SM_UTILUI_TEXTSEARCHFORM
#define _SM_UTILUI_TEXTSEARCHFORM
#include "Media/MonitorMgr.h"
#include "UI/GUIButton.h"
#include "UI/GUIForm.h"
#include "UI/GUITextBox.h"
#include "UI/GUILabel.h"
#include "UtilUI/TextViewerForm.h"

namespace UtilUI
{
	class TextSearchForm : public UI::GUIForm
	{
	private:
		UI::GUILabel *lblSearch;
		UI::GUITextBox *txtSearch;
		UI::GUIButton *btnSearch;
		UI::GUIButton *btnClose;
		Media::MonitorMgr *monMgr;
		UtilUI::TextViewerForm *mainFrm;

		static void __stdcall OnSearchClicked(void *userObj);
		static void __stdcall OnCloseClicked(void *userObj);
	public:
		TextSearchForm(UI::GUIClientControl *parent, UI::GUICore *ui, Media::MonitorMgr *monMgr, UtilUI::TextViewerForm *mainFrm);
		virtual ~TextSearchForm();

		virtual void OnShow();
		virtual void OnMonitorChanged();
	};
};
#endif
