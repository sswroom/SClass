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
			NotNullPtr<SSWR::AVIRead::AVIRCore> core;

			NotNullPtr<UI::GUIPanel> pnlDevInfo;
			NotNullPtr<UI::GUILabel> lblMAC;
			NotNullPtr<UI::GUITextBox> txtMAC;
			NotNullPtr<UI::GUILabel> lblName;
			NotNullPtr<UI::GUITextBox> txtName;
			NotNullPtr<UI::GUIButton> btnCSV;
			NotNullPtr<UI::GUIListView> lvContent;

			NotNullPtr<const IO::BTScanLog::DevEntry> entry;

			static void __stdcall OnCSVClicked(AnyType userObj);
		public:
			AVIRBTScanLogDevForm(Optional<UI::GUIClientControl> parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core, NotNullPtr<const IO::BTScanLog::DevEntry> entry);
			virtual ~AVIRBTScanLogDevForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
