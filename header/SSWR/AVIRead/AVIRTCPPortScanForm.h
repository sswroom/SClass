#ifndef _SM_SSWR_AVIREAD_AVIRTCPPORTSCANFORM
#define _SM_SSWR_AVIREAD_AVIRTCPPORTSCANFORM
#include "Net/TCPPortScanner.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "Sync/Mutex.h"
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
		class AVIRTCPPortScanForm : public UI::GUIForm
		{
		private:
			NN<SSWR::AVIRead::AVIRCore> core;
			NN<Net::SocketFactory> sockf;
			Net::TCPPortScanner *scanner;
			Bool listUpdated;

			NN<UI::GUIPanel> pnlControl;
			NN<UI::GUILabel> lblIP;
			NN<UI::GUITextBox> txtIP;
			NN<UI::GUILabel> lblThreadCnt;
			NN<UI::GUITextBox> txtThreadCnt;
			NN<UI::GUILabel> lblMaxPort;
			NN<UI::GUITextBox> txtMaxPort;
			NN<UI::GUIButton> btnStart;
			NN<UI::GUIListView> lvPort;

			static void __stdcall OnStartClicked(AnyType userObj);
			static void __stdcall OnTimerTick(AnyType userObj);
			static void __stdcall OnPortUpdated(AnyType userObj, UInt16 port);
		public:
			AVIRTCPPortScanForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRTCPPortScanForm();

			virtual void OnMonitorChanged();
		};
	};
};
#endif
