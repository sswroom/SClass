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
		NN<UI::GUIPanel> pnlStatus;
		NN<UI::GUITextBox> txtStatus;
		NN<UI::GUIMainMenu> mnuMain;

		UI::GUIForm *srchFrm;
		NN<Media::MonitorMgr> monMgr;
		
		static void __stdcall OnFileDrop(AnyType userObj, Data::DataArray<NN<Text::String>> files);
		static void __stdcall OnTextPosUpd(AnyType userObj, UInt32 textPosX, UOSInt textPosY);
		static void __stdcall OnSearchClosed(AnyType userObj, NN<UI::GUIForm> frm);
	public:
		TextViewerForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<Media::MonitorMgr> monMgr, NN<Media::DrawEngine> deng, UInt32 codePage);
		virtual ~TextViewerForm();

		virtual void EventMenuClicked(UInt16 cmdId);
		virtual void OnMonitorChanged();

		Bool SearchText(Text::CStringNN txt);
		Bool LoadFile(NN<Text::String> filePath);
		Bool LoadStreamData(NN<IO::StreamData> data);
		Bool OpenSearch(Text::CString txt);
	};
}
#endif
