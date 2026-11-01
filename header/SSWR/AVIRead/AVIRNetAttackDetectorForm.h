#ifndef _SM_SSWR_AVIREAD_AVIRNETATTACKDETECTORFORM
#define _SM_SSWR_AVIREAD_AVIRNETATTACKDETECTORFORM
#include "Net/NetAttackDetector.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIForm.h"
#include "UI/GUIListBox.h"
#include "UI/GUITextBox.h"
#include "UI/ListBoxLogger.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRNetAttackDetectorForm : public UI::GUIForm
		{
		private:
			NN<UI::GUITextBox> txtLog;
			NN<UI::GUIListBox> lbLog;

			NN<SSWR::AVIRead::AVIRCore> core;
			NN<UI::ListBoxLogger> logger;
			NN<IO::LogTool> log;
			NN<Net::NetAttackDetector> netAttackDetect;

			static void __stdcall OnNetAttackEvent(NN<Net::SocketUtil::AddressInfo> addr, Net::NetAttackDetector::AttackType attackType, AnyType userObj);
			static void __stdcall OnLogSelChg(AnyType userObj);

		public:
			AVIRNetAttackDetectorForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRNetAttackDetectorForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
