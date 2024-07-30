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
			NN<SSWR::AVIRead::AVIRCore> core;
			Manage::HiResClock clk;
			NN<Net::TCPClientFactory> clif;
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

			NN<UI::GUIGroupBox> grpURL;
			NN<UI::GUIPanel> pnlURL;
			NN<UI::GUIPanel> pnlURLCtrl;
			NN<UI::GUIListBox> lbURL;
			NN<UI::GUITextBox> txtURL;
			NN<UI::GUIButton> btnURLAdd;
			NN<UI::GUIButton> btnURLClear;
			NN<UI::GUIPanel> pnlRequest;
			NN<UI::GUILabel> lblConcurrCnt;
			NN<UI::GUITextBox> txtConcurrCnt;
			NN<UI::GUILabel> lblTotalConnCnt;
			NN<UI::GUITextBox> txtTotalConnCnt;
			NN<UI::GUILabel> lblMethod;
			NN<UI::GUIComboBox> cboMethod;
			NN<UI::GUILabel> lblPostSize;
			NN<UI::GUITextBox> txtPostSize;
			NN<UI::GUICheckBox> chkKAConn;
			NN<UI::GUICheckBox> chkGZip;
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

			static void __stdcall OnStartClicked(AnyType userObj);
			static void __stdcall OnURLAddClicked(AnyType userObj);
			static void __stdcall OnURLClearClicked(AnyType userObj);
			static void __stdcall ProcessThread(NN<Sync::Thread> thread);
			static void __stdcall OnTimerTick(AnyType userObj);
			void StopThreads();
			void ClearURLs();
			Optional<Text::String> GetNextURL();
		public:
			AVIRHTTPTestForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRHTTPTestForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
