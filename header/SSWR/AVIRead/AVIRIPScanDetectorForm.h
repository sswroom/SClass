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
			NN<UI::GUITextBox> txtLog;
			NN<UI::GUIListBox> lbLog;

			NN<SSWR::AVIRead::AVIRCore> core;
			NN<UI::ListBoxLogger> logger;
			IO::LogTool *log;
			Net::IPScanDetector *ipScanDetect;

			static void __stdcall OnIPScanEvent(UnsafeArray<const UInt8> hwAddr, AnyType userObj);
			static void __stdcall OnLogSelChg(AnyType userObj);

		public:
			AVIRIPScanDetectorForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRIPScanDetectorForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
