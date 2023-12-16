#ifndef _SM_SSWR_AVIREAD_AVIRHTTPTESTFORM
#define _SM_SSWR_AVIREAD_AVIRHTTPTESTFORM
#include "Data/ArrayListNN.h"
#include "Manage/HiResClock.h"
#include "Net/HTTPClient.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "Sync/Thread.h"
#include "UI/GUIButton.h"
#include "UI/GUICheckBox.h"
#include "UI/GUIComboBox.h"
#include "UI/GUIForm.h"
#include "UI/GUIGroupBox.h"
#include "UI/GUIHSplitter.h"
#include "UI/GUILabel.h"
#include "UI/GUIListBox.h"
#include "UI/GUIPanel.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRHTTPTestForm : public UI::GUIForm
		{
		private:
			NotNullPtr<SSWR::AVIRead::AVIRCore> core;
			Manage::HiResClock clk;
			NotNullPtr<Net::SocketFactory> sockf;
			Optional<Net::SSLEngine> ssl;
			Sync::Thread **threads;
			Sync::Mutex connMut;
			Data::ArrayListStringNN connURLs;
			Net::WebUtil::RequestMethod method;
			UInt32 postSize;
			UOSInt connCurrIndex;
			UInt32 connLeftCnt;
			UInt32 threadCnt;
			UInt32 threadCurrCnt;
			UInt32 connCnt;
			UInt32 failCnt;
			Bool kaConn;
			Bool enableGZip;
			Double t;

			NotNullPtr<UI::GUIGroupBox> grpURL;
			NotNullPtr<UI::GUIPanel> pnlURL;
			NotNullPtr<UI::GUIPanel> pnlURLCtrl;
			UI::GUIListBox *lbURL;
			UI::GUITextBox *txtURL;
			UI::GUIButton *btnURLAdd;
			UI::GUIButton *btnURLClear;
			NotNullPtr<UI::GUIPanel> pnlRequest;
			UI::GUILabel *lblConcurrCnt;
			UI::GUITextBox *txtConcurrCnt;
			UI::GUILabel *lblTotalConnCnt;
			UI::GUITextBox *txtTotalConnCnt;
			UI::GUILabel *lblMethod;
			UI::GUIComboBox *cboMethod;
			UI::GUILabel *lblPostSize;
			UI::GUITextBox *txtPostSize;
			UI::GUICheckBox *chkKAConn;
			UI::GUICheckBox *chkGZip;
			UI::GUIButton *btnStart;
			NotNullPtr<UI::GUIGroupBox> grpStatus;
			UI::GUILabel *lblConnLeftCnt;
			UI::GUITextBox *txtConnLeftCnt;
			UI::GUILabel *lblThreadCnt;
			UI::GUITextBox *txtThreadCnt;
			UI::GUILabel *lblSuccCnt;
			UI::GUITextBox *txtSuccCnt;
			UI::GUILabel *lblFailCnt;
			UI::GUITextBox *txtFailCnt;
			UI::GUILabel *lblTimeUsed;
			UI::GUITextBox *txtTimeUsed;

			static void __stdcall OnStartClicked(void *userObj);
			static void __stdcall OnURLAddClicked(void *userObj);
			static void __stdcall OnURLClearClicked(void *userObj);
			static void __stdcall ProcessThread(NotNullPtr<Sync::Thread> thread);
			static void __stdcall OnTimerTick(void *userObj);
			void StopThreads();
			void ClearURLs();
			Optional<Text::String> GetNextURL();
		public:
			AVIRHTTPTestForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRHTTPTestForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
