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
			NotNullPtr<UI::GUITextBox> txtURL;
			NotNullPtr<UI::GUIButton> btnURLAdd;
			NotNullPtr<UI::GUIButton> btnURLClear;
			NotNullPtr<UI::GUIPanel> pnlRequest;
			NotNullPtr<UI::GUILabel> lblConcurrCnt;
			NotNullPtr<UI::GUITextBox> txtConcurrCnt;
			NotNullPtr<UI::GUILabel> lblTotalConnCnt;
			NotNullPtr<UI::GUITextBox> txtTotalConnCnt;
			NotNullPtr<UI::GUILabel> lblMethod;
			NotNullPtr<UI::GUIComboBox> cboMethod;
			NotNullPtr<UI::GUILabel> lblPostSize;
			NotNullPtr<UI::GUITextBox> txtPostSize;
			UI::GUICheckBox *chkKAConn;
			UI::GUICheckBox *chkGZip;
			NotNullPtr<UI::GUIButton> btnStart;
			NotNullPtr<UI::GUIGroupBox> grpStatus;
			NotNullPtr<UI::GUILabel> lblConnLeftCnt;
			NotNullPtr<UI::GUITextBox> txtConnLeftCnt;
			NotNullPtr<UI::GUILabel> lblThreadCnt;
			NotNullPtr<UI::GUITextBox> txtThreadCnt;
			NotNullPtr<UI::GUILabel> lblSuccCnt;
			NotNullPtr<UI::GUITextBox> txtSuccCnt;
			NotNullPtr<UI::GUILabel> lblFailCnt;
			NotNullPtr<UI::GUITextBox> txtFailCnt;
			NotNullPtr<UI::GUILabel> lblTimeUsed;
			NotNullPtr<UI::GUITextBox> txtTimeUsed;

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
