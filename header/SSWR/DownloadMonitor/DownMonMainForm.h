#ifndef _SM_SSWR_DOWNLOADMONITOR_DOWNMONMAINFORM
#define _SM_SSWR_DOWNLOADMONITOR_DOWNMONMAINFORM
#include "SSWR/DownloadMonitor/DownMonCore.h"
#include "UI/GUIButton.h"
#include "UI/GUICheckBox.h"
#include "UI/GUIForm.h"
#include "UI/GUILabel.h"
#include "UI/GUIListView.h"
#include "UI/GUIPanel.h"
#include "UI/GUIPopupMenu.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace DownloadMonitor
	{
		class DownMonMainForm : public UI::GUIForm
		{
		private:
			SSWR::DownloadMonitor::DownMonCore *core;
			SSWR::DownloadMonitor::DownMonCore::CheckStatus currStatus;
			Sync::Mutex endedMut;
			Data::ArrayList<Int32> endedList;
			Bool alarmSet;
			Int64 alarmTime;

			NotNullPtr<UI::GUIPanel> pnlButtons;
			NotNullPtr<UI::GUILabel> lblStatus;
			NotNullPtr<UI::GUITextBox> txtStatus;
			NotNullPtr<UI::GUICheckBox> chkAutoStart;
			NotNullPtr<UI::GUIButton> btnPasteTable;
			NotNullPtr<UI::GUIButton> btnPasteHTML;
			NotNullPtr<UI::GUIButton> btnCopyTable;
			NotNullPtr<UI::GUIButton> btnFileEnd;
			NotNullPtr<UI::GUIButton> btnWebUpdate;
			NotNullPtr<UI::GUILabel> lblAlarm;
			NotNullPtr<UI::GUIButton> btn30Minutes;
			UI::GUIListView *lvFiles;

			static void __stdcall OnTimerTick(void *userObj);
			static void __stdcall OnPasteTableClicked(void *userObj);
			static void __stdcall OnPasteHTMLClicked(void *userObj);
			static void __stdcall OnCopyTableClicked(void *userObj);
			static void __stdcall OnFilesDblClick(void *userObj, UOSInt itemIndex);
			static void __stdcall OnFileEndClicked(void *userObj);
			static void __stdcall OnWebUpdateClicked(void *userObj);
			static void __stdcall On30MinutesClicked(void *userObj);
			static void __stdcall OnFileEnd(void *userObj, Int32 fileId, Int32 webType);

			static Int32 ParseURL(Text::CString url, Int32 *webType);
			void LoadList();
			void SaveList();
		public:
			DownMonMainForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, SSWR::DownloadMonitor::DownMonCore *core);
			virtual ~DownMonMainForm();
		};
	}
}
#endif