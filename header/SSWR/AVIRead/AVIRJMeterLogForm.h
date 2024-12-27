#ifndef _SM_SSWR_AVIREAD_AVIRJMETERLOGFORM
#define _SM_SSWR_AVIREAD_AVIRJMETERLOGFORM
#include "IO/JMeterLog.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIButton.h"
#include "UI/GUIForm.h"
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
		class AVIRJMeterLogForm : public UI::GUIForm
		{
		private:
			NN<UI::GUIPanel> pnlFile;
			NN<UI::GUILabel> lblLogFile;
			NN<UI::GUITextBox> txtLogFile;
			NN<UI::GUIButton> btnBrowse;
			NN<UI::GUILabel> lblStartTime;
			NN<UI::GUITextBox> txtStartTime;
			NN<UI::GUILabel> lblEndTime;
			NN<UI::GUITextBox> txtEndTime;
			NN<UI::GUITabControl> tcMain;

			NN<UI::GUITabPage> tpGroup;
			NN<UI::GUIListView> lvGroup;

			NN<UI::GUITabPage> tpThread;
			NN<UI::GUIListView> lvThread;

			NN<UI::GUITabPage> tpConcurr;
			NN<UI::GUIPanel> pnlConcurr;
			NN<UI::GUILabel> lblConcurr;
			NN<UI::GUITextBox> txtConcurr;
			NN<UI::GUIButton> btnConcurr;
			NN<UI::GUIListView> lvConcurr;

			NN<UI::GUITabPage> tpExport;
			NN<UI::GUILabel> lblExportStart;
			NN<UI::GUITextBox> txtExportStart;
			NN<UI::GUILabel> lblExportEnd;
			NN<UI::GUITextBox> txtExportEnd;
			NN<UI::GUIButton> btnExport;

			NN<SSWR::AVIRead::AVIRCore> core;
			Optional<IO::JMeterLog> log;

			static void __stdcall OnFileDropped(AnyType userObj, Data::DataArray<NN<Text::String>> files);
			static void __stdcall OnBrowseClicked(AnyType userObj);
			static void __stdcall OnConcurrClicked(AnyType userObj);
			static void __stdcall OnExportClicked(AnyType userObj);
			void OpenFile(Text::CStringNN fileName);
		public:
			AVIRJMeterLogForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRJMeterLogForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
