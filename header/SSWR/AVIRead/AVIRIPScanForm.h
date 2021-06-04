#ifndef _SM_SSWR_AVIREAD_AVIRIPSCANFORM
#define _SM_SSWR_AVIREAD_AVIRIPSCANFORM
#include "Data/UInt32Map.h"
#include "Manage/HiResClock.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "Sync/Mutex.h"
#include "UI/GUIButton.h"
#include "UI/GUIComboBox.h"
#include "UI/GUIForm.h"
#include "UI/GUILabel.h"
#include "UI/GUIListView.h"
#include "UI/GUIPanel.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRIPScanForm : public UI::GUIForm
		{
		private:
			typedef struct
			{
				UInt32 ip;
				UInt8 mac[6];
				Double respTime;
			} ScanResult;
		private:
			SSWR::AVIRead::AVIRCore *core;
			Net::SocketFactory *sockf;
			Sync::Mutex *resultMut;
			Data::UInt32Map<ScanResult*> *results;
			Manage::HiResClock *clk;
			UInt32 *soc;
			Bool threadRunning;
			Bool threadToStop;

			UI::GUIPanel *pnlControl;
			UI::GUILabel *lblIP;
			UI::GUIComboBox *cboIP;
			UI::GUIButton *btnStart;
			UI::GUIListView *lvIP;

			static void __stdcall ICMPChecksum(UInt8 *buff, OSInt buffSize);
			static UInt32 __stdcall Ping1Thread(void *userObj);
			static UInt32 __stdcall Ping2Thread(void *userObj);
			static void __stdcall OnStartClicked(void *userObj);
			static void __stdcall OnTimerTick(void *userObj);
			void AppendMACs(UInt32 ip);
			void ClearResults();
		public:
			AVIRIPScanForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core);
			virtual ~AVIRIPScanForm();

			virtual void OnMonitorChanged();
		};
	};
};
#endif
