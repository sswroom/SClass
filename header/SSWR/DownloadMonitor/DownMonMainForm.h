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
			Sync::Mutex *endedMut;
			Data::ArrayList<Int32> *endedList;
			Bool alarmSet;
			Int64 alarmTime;

			UI::GUIPanel *pnlButtons;
			UI::GUILabel *lblStatus;
			UI::GUITextBox *txtStatus;
			UI::GUICheckBox *chkAutoStart;
			UI::GUIButton *btnPasteTable;
			UI::GUIButton *btnPasteHTML;
			UI::GUIButton *btnCopyTable;
			UI::GUIButton *btnFileEnd;
			UI::GUIButton *btnWebUpdate;
			UI::GUILabel *lblAlarm;
			UI::GUIButton *btn30Minutes;
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

			static Int32 ParseURL(const UTF8Char *url, Int32 *webType);
			void LoadList();
			void SaveList();
		public:
			DownMonMainForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::DownloadMonitor::DownMonCore *core);
			virtual ~DownMonMainForm();
		};
	}
}
#endif