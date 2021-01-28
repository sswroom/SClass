#ifndef _SM_UTILUI_TEXTVIEWERFORM
#define _SM_UTILUI_TEXTVIEWERFORM
#include "Media/MonitorMgr.h"
#include "UI/GUIForm.h"
#include "UI/GUIMainMenu.h"
#include "UI/GUIPanel.h"
#include "UI/GUITextBox.h"
#include "UI/GUITextFileView.h"

namespace UtilUI
{
	class TextViewerForm : public UI::GUIForm
	{
	private:
		UI::GUITextFileView *txtView;
		UI::GUIPanel *pnlStatus;
		UI::GUITextBox *txtStatus;
		UI::GUIMainMenu *mnuMain;

		UI::GUIForm *srchFrm;
		Media::MonitorMgr *monMgr;
		
		static void __stdcall OnFileDrop(void *userObj, const UTF8Char **files, OSInt nFiles);
		static void __stdcall OnTextPosUpd(void *userObj, Int32 textPosX, OSInt textPosY);
		static void __stdcall OnSearchClosed(void *userObj, UI::GUIForm *frm);
	public:
		TextViewerForm(UI::GUIClientControl *parent, UI::GUICore *ui, Media::MonitorMgr *monMgr, Int32 codePage);
		virtual ~TextViewerForm();

		virtual void EventMenuClicked(UInt16 cmdId);
		virtual void OnMonitorChanged();

		Bool SearchText(const UTF8Char *txt);
	};
};
#endif
