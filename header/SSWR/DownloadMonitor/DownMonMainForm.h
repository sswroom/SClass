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
			Data::ArrayListNative<Int32> endedList;
			Bool alarmSet;
			Int64 alarmTime;

			NN<UI::GUIPanel> pnlButtons;
			NN<UI::GUILabel> lblStatus;
			NN<UI::GUITextBox> txtStatus;
			NN<UI::GUICheckBox> chkAutoStart;
			NN<UI::GUIButton> btnPasteTable;
			NN<UI::GUIButton> btnPasteHTML;
			NN<UI::GUIButton> btnCopyTable;
			NN<UI::GUIButton> btnFileEnd;
			NN<UI::GUIButton> btnWebUpdate;
			NN<UI::GUILabel> lblAlarm;
			NN<UI::GUIButton> btn30Minutes;
			NN<UI::GUIListView> lvFiles;

			static void __stdcall OnTimerTick(AnyType userObj);
			static void __stdcall OnPasteTableClicked(AnyType userObj);
			static void __stdcall OnPasteHTMLClicked(AnyType userObj);
			static void __stdcall OnCopyTableClicked(AnyType userObj);
			static void __stdcall OnFilesDblClick(AnyType userObj, UIntOS itemIndex);
			static void __stdcall OnFileEndClicked(AnyType userObj);
			static void __stdcall OnWebUpdateClicked(AnyType userObj);
			static void __stdcall On30MinutesClicked(AnyType userObj);
			static void __stdcall OnFileEnd(AnyType userObj, Int32 fileId, Int32 webType);

			static Int32 ParseURL(Text::CStringNN url, OutParam<Int32> webType);
			void LoadList();
			void SaveList();
		public:
			DownMonMainForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, SSWR::DownloadMonitor::DownMonCore *core);
			virtual ~DownMonMainForm();
		};
	}
}
#endif