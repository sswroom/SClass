#ifndef _SM_SSWR_AVIREAD_AVIRIPSCANDETECTORFORM
#define _SM_SSWR_AVIREAD_AVIRIPSCANDETECTORFORM
#include "Net/IPScanDetector.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIForm.h"
#include "UI/GUIListBox.h"
#include "UI/GUITextBox.h"
#include "UI/ListBoxLogger.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRIPScanDetectorForm : public UI::GUIForm
		{
		private:
			NotNullPtr<UI::GUITextBox> txtLog;
			NotNullPtr<UI::GUIListBox> lbLog;

			NotNullPtr<SSWR::AVIRead::AVIRCore> core;
			NotNullPtr<UI::ListBoxLogger> logger;
			IO::LogTool *log;
			Net::IPScanDetector *ipScanDetect;

			static void __stdcall OnIPScanEvent(const UInt8 *hwAddr, void *userObj);
			static void __stdcall OnLogSelChg(void *userObj);

		public:
			AVIRIPScanDetectorForm(Optional<UI::GUIClientControl> parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRIPScanDetectorForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
