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
			NotNullPtr<SSWR::AVIRead::AVIRCore> core;
			NotNullPtr<Net::SocketFactory> sockf;
			Net::TCPPortScanner *scanner;
			Bool listUpdated;

			NotNullPtr<UI::GUIPanel> pnlControl;
			NotNullPtr<UI::GUILabel> lblIP;
			NotNullPtr<UI::GUITextBox> txtIP;
			NotNullPtr<UI::GUILabel> lblThreadCnt;
			NotNullPtr<UI::GUITextBox> txtThreadCnt;
			NotNullPtr<UI::GUILabel> lblMaxPort;
			NotNullPtr<UI::GUITextBox> txtMaxPort;
			NotNullPtr<UI::GUIButton> btnStart;
			NotNullPtr<UI::GUIListView> lvPort;

			static void __stdcall OnStartClicked(AnyType userObj);
			static void __stdcall OnTimerTick(AnyType userObj);
			static void __stdcall OnPortUpdated(AnyType userObj, UInt16 port);
		public:
			AVIRTCPPortScanForm(Optional<UI::GUIClientControl> parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRTCPPortScanForm();

			virtual void OnMonitorChanged();
		};
	};
};
#endif
