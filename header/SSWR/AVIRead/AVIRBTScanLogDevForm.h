#ifndef _SM_SSWR_AVIREAD_AVIRBTSCANLOGDEVFORM
#define _SM_SSWR_AVIREAD_AVIRBTSCANLOGDEVFORM
#include "IO/BTScanLog.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIButton.h"
#include "UI/GUIForm.h"
#include "UI/GUILabel.h"
#include "UI/GUIListView.h"
#include "UI/GUIPanel.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRBTScanLogDevForm : public UI::GUIForm
		{
		private:
			NN<SSWR::AVIRead::AVIRCore> core;

			NN<UI::GUIPanel> pnlDevInfo;
			NN<UI::GUILabel> lblMAC;
			NN<UI::GUITextBox> txtMAC;
			NN<UI::GUILabel> lblName;
			NN<UI::GUITextBox> txtName;
			NN<UI::GUIButton> btnCSV;
			NN<UI::GUIListView> lvContent;

			NN<const IO::BTScanLog::DevEntry> entry;

			static void __stdcall OnCSVClicked(AnyType userObj);
		public:
			AVIRBTScanLogDevForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core, NN<const IO::BTScanLog::DevEntry> entry);
			virtual ~AVIRBTScanLogDevForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
