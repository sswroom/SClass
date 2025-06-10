#ifndef _SM_SSWR_AVIREAD_AVIREDGEANALYSEFORM
#define _SM_SSWR_AVIREAD_AVIREDGEANALYSEFORM
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIButton.h"
#include "UI/GUILabel.h"
#include "UI/GUIListView.h"
#include "UI/GUIPanel.h"
#include "UI/GUITabControl.h"
#include "UI/GUITabPage.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIREdgeAnalyseForm : public UI::GUIForm
		{
		private:
			NN<SSWR::AVIRead::AVIRCore> core;
			NN<UI::GUITabControl> tcMain;

			NN<UI::GUITabPage> tpWebHook;
			NN<UI::GUIPanel> pnlWebHook;
			NN<UI::GUILabel> lblWebHookPort;
			NN<UI::GUITextBox> txtWebHookPort;
			NN<UI::GUIButton> btnWebHook;
			NN<UI::GUITextBox> txtWebHookData;
			NN<UI::GUIListView> lvWebHookData;
			
			static void __stdcall OnWebHookClicked(AnyType userObj);
			static void __stdcall OnWebHookDataSelChg(AnyType userObj);
		public:
			AVIREdgeAnalyseForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIREdgeAnalyseForm();

			virtual void OnMonitorChanged();
			void AddRecord(Data::Timestamp ts, NN<Text::String> type, NN<Text::JSONBase> json);
		};
	}
}
#endif
