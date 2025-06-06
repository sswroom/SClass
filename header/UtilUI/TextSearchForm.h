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
		NN<UI::GUILabel> lblSearch;
		NN<UI::GUITextBox> txtSearch;
		NN<UI::GUIButton> btnSearch;
		NN<UI::GUIButton> btnClose;
		NN<Media::MonitorMgr> monMgr;
		NN<UtilUI::TextViewerForm> mainFrm;

		static void __stdcall OnSearchClicked(AnyType userObj);
		static void __stdcall OnCloseClicked(AnyType userObj);
	public:
		TextSearchForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<Media::MonitorMgr> monMgr, NN<UtilUI::TextViewerForm> mainFrm);
		virtual ~TextSearchForm();

		virtual void OnShow();
		virtual void OnMonitorChanged();

		void SetSearchText(Text::CStringNN txt);
	};
}
#endif
