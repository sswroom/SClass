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
		NotNullPtr<UI::GUIPanel> pnlStatus;
		NotNullPtr<UI::GUITextBox> txtStatus;
		NotNullPtr<UI::GUIMainMenu> mnuMain;

		UI::GUIForm *srchFrm;
		Media::MonitorMgr *monMgr;
		
		static void __stdcall OnFileDrop(AnyType userObj, Data::DataArray<NotNullPtr<Text::String>> files);
		static void __stdcall OnTextPosUpd(AnyType userObj, UInt32 textPosX, UOSInt textPosY);
		static void __stdcall OnSearchClosed(AnyType userObj, NotNullPtr<UI::GUIForm> frm);
	public:
		TextViewerForm(Optional<UI::GUIClientControl> parent, NotNullPtr<UI::GUICore> ui, Media::MonitorMgr *monMgr, NotNullPtr<Media::DrawEngine> deng, UInt32 codePage);
		virtual ~TextViewerForm();

		virtual void EventMenuClicked(UInt16 cmdId);
		virtual void OnMonitorChanged();

		Bool SearchText(Text::CString txt);
		Bool LoadFile(NotNullPtr<Text::String> filePath);
		Bool LoadStreamData(NotNullPtr<IO::StreamData> data);
		Bool OpenSearch(Text::CString txt);
	};
}
#endif
