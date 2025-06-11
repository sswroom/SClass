#ifndef _SM_SSWR_AVIREAD_AVIREDGEANALYSEFORM
#define _SM_SSWR_AVIREAD_AVIREDGEANALYSEFORM
#include "Net/WebServer/WebListener.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIButton.h"
#include "UI/GUILabel.h"
#include "UI/GUIListBox.h"
#include "UI/GUIListView.h"
#include "UI/GUIPanel.h"
#include "UI/GUITabControl.h"
#include "UI/GUITabPage.h"
#include "UI/GUITextBox.h"
#include "UI/ListBoxLogger.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIREdgeAnalyseForm : public UI::GUIForm
		{
		public:
			struct EdgeRecord
			{
				Data::Timestamp ts;
				NN<Text::String> type;
				NN<Text::JSONBase> json;
			};
		private:
			NN<SSWR::AVIRead::AVIRCore> core;
			Optional<Net::WebServer::WebListener> webHookListener;
			NN<Net::WebServer::WebHandler> webHookHdlr;
			IO::LogTool log;
			NN<UI::ListBoxLogger> logger;
			Data::ArrayListNN<EdgeRecord> records;
			Sync::Mutex newRecordsMut;
			Data::ArrayListNN<EdgeRecord> newRecords;

			NN<UI::GUITabControl> tcMain;

			NN<UI::GUITabPage> tpWebHook;
			NN<UI::GUIPanel> pnlWebHook;
			NN<UI::GUILabel> lblWebHookPort;
			NN<UI::GUITextBox> txtWebHookPort;
			NN<UI::GUIButton> btnWebHook;
			NN<UI::GUITextBox> txtWebHookData;
			NN<UI::GUIListView> lvWebHookData;

			NN<UI::GUITabPage> tpLog;
			NN<UI::GUIListBox> lbLog;
			NN<UI::GUITextBox> txtLog;
			
			static void __stdcall OnWebHookClicked(AnyType userObj);
			static void __stdcall OnWebHookDataSelChg(AnyType userObj);
			static void __stdcall OnLogSelChg(AnyType userObj);
			static void __stdcall OnTimerTick(AnyType userObj);
			static void __stdcall FreeRecord(NN<EdgeRecord> record);
		public:
			AVIREdgeAnalyseForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIREdgeAnalyseForm();

			virtual void OnMonitorChanged();
			void AddRecord(Data::Timestamp ts, NN<Text::String> type, NN<Text::JSONBase> json);
		};
	}
}
#endif
