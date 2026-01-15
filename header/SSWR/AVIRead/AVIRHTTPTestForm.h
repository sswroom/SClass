#ifndef _SM_SSWR_AVIREAD_AVIRHTTPTESTFORM
#define _SM_SSWR_AVIREAD_AVIRHTTPTESTFORM
#include "Data/ArrayListNN.hpp"
#include "Manage/HiResClock.h"
#include "Net/HTTPClient.h"
#include "Net/JMeter/JMeterStep.h"
#include "Net/JMeter/JMeterCookieManager.h"
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
			NN<SSWR::AVIRead::AVIRCore> core;
			Manage::HiResClock clk;
			NN<Net::TCPClientFactory> clif;
			Optional<Net::SSLEngine> ssl;
			UnsafeArrayOpt<NN<Sync::Thread>> threads;
			Sync::Mutex connMut;
			Data::ArrayListNN<Net::JMeter::JMeterStep> connSteps;
			UIntOS connCurrIndex;
			UInt32 connLeftCnt;
			UInt32 threadCnt;
			UInt32 threadCurrCnt;
			UInt32 connCnt;
			UInt32 failCnt;
			UInt64 totalSize;
			Double t;
			Net::JMeter::JMeterCookieManager cookieManager;

			NN<UI::GUIGroupBox> grpURL;
			NN<UI::GUIPanel> pnlURL;
			NN<UI::GUIPanel> pnlURLCtrl;
			NN<UI::GUIListBox> lbURL;
			NN<UI::GUIComboBox> cboMethod;
			NN<UI::GUITextBox> txtURL;
			NN<UI::GUILabel> lblPostData;
			NN<UI::GUIComboBox> cboPostType;
			NN<UI::GUITextBox> txtPostData;
			NN<UI::GUICheckBox> chkKAConn;
			NN<UI::GUICheckBox> chkGZip;
			NN<UI::GUIButton> btnURLAdd;
			NN<UI::GUIButton> btnURLClear;
			NN<UI::GUIPanel> pnlRequest;
			NN<UI::GUILabel> lblConcurrCnt;
			NN<UI::GUITextBox> txtConcurrCnt;
			NN<UI::GUILabel> lblTotalConnCnt;
			NN<UI::GUITextBox> txtTotalConnCnt;
			NN<UI::GUIButton> btnStart;
			NN<UI::GUIGroupBox> grpStatus;
			NN<UI::GUILabel> lblConnLeftCnt;
			NN<UI::GUITextBox> txtConnLeftCnt;
			NN<UI::GUILabel> lblThreadCnt;
			NN<UI::GUITextBox> txtThreadCnt;
			NN<UI::GUILabel> lblSuccCnt;
			NN<UI::GUITextBox> txtSuccCnt;
			NN<UI::GUILabel> lblFailCnt;
			NN<UI::GUITextBox> txtFailCnt;
			NN<UI::GUILabel> lblTimeUsed;
			NN<UI::GUITextBox> txtTimeUsed;
			NN<UI::GUILabel> lblTotalSize;
			NN<UI::GUITextBox> txtTotalSize;
			NN<UI::GUILabel> lblReqPerSec;
			NN<UI::GUITextBox> txtReqPerSec;
			NN<UI::GUILabel> lblDataRate;
			NN<UI::GUITextBox> txtDataRate;

			static void __stdcall OnStartClicked(AnyType userObj);
			static void __stdcall OnURLAddClicked(AnyType userObj);
			static void __stdcall OnURLClearClicked(AnyType userObj);
			static void __stdcall ProcessThread(NN<Sync::Thread> thread);
			static void __stdcall OnTimerTick(AnyType userObj);
			void StopThreads();
			void ClearSteps();
		public:
			AVIRHTTPTestForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRHTTPTestForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
