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
			SSWR::AVIRead::AVIRCore *core;
			Net::SocketFactory *sockf;
			Net::TCPPortScanner *scanner;
			Bool listUpdated;

			UI::GUIPanel *pnlControl;
			UI::GUILabel *lblIP;
			UI::GUITextBox *txtIP;
			UI::GUILabel *lblThreadCnt;
			UI::GUITextBox *txtThreadCnt;
			UI::GUILabel *lblMaxPort;
			UI::GUITextBox *txtMaxPort;
			UI::GUIButton *btnStart;
			UI::GUIListView *lvPort;

			static void __stdcall OnStartClicked(void *userObj);
			static void __stdcall OnTimerTick(void *userObj);
			static void __stdcall OnPortUpdated(void *userObj, UInt16 port);
		public:
			AVIRTCPPortScanForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core);
			virtual ~AVIRTCPPortScanForm();

			virtual void OnMonitorChanged();
		};
	};
};
#endif
