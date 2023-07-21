#ifndef _SM_UTILUI_TEXTVIEWERFORM
#define _SM_UTILUI_TEXTVIEWERFORM
#include "Media/DrawEngine.h"
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
		
		static void __stdcall OnFileDrop(void *userObj, NotNullPtr<Text::String> *files, UOSInt nFiles);
		static void __stdcall OnTextPosUpd(void *userObj, UInt32 textPosX, UOSInt textPosY);
		static void __stdcall OnSearchClosed(void *userObj, UI::GUIForm *frm);
	public:
		TextViewerForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, Media::MonitorMgr *monMgr, Media::DrawEngine *deng, UInt32 codePage);
		virtual ~TextViewerForm();

		virtual void EventMenuClicked(UInt16 cmdId);
		virtual void OnMonitorChanged();

		Bool SearchText(Text::CString txt);
		Bool LoadFile(NotNullPtr<Text::String> filePath);
		Bool OpenSearch(Text::CString txt);
	};
}
#endif
