#ifndef _SM_SSWR_AVIREAD_AVIRHTTPTESTFORM
#define _SM_SSWR_AVIREAD_AVIRHTTPTESTFORM

#include "Manage/HiResClock.h"
#include "Net/HTTPClient.h"
#include "SSWR/AVIRead/AVIRCore.h"
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
			typedef struct
			{
				AVIRHTTPTestForm *me;
				Bool threadRunning;
				Bool threadToStop;
				Sync::Event *evt;
			} ThreadStatus;
		private:
			SSWR::AVIRead::AVIRCore *core;
			Manage::HiResClock *clk;
			Net::SocketFactory *sockf;
			ThreadStatus *threadStatus;
			Sync::Mutex *connMut;
			Data::ArrayList<const UTF8Char *> *connURLs;
			const Char *method;
			OSInt postSize;
			UOSInt connCurrIndex;
			Int32 connLeftCnt;
			Int32 threadCnt;
			Int32 threadCurrCnt;
			Int32 connCnt;
			Int32 failCnt;
			Bool kaConn;
			Double t;

			UI::GUIGroupBox *grpURL;
			UI::GUIPanel *pnlURL;
			UI::GUIPanel *pnlURLCtrl;
			UI::GUIListBox *lbURL;
			UI::GUITextBox *txtURL;
			UI::GUIButton *btnURLAdd;
			UI::GUIButton *btnURLClear;
			UI::GUIPanel *pnlRequest;
			UI::GUILabel *lblConcurrCnt;
			UI::GUITextBox *txtConcurrCnt;
			UI::GUILabel *lblTotalConnCnt;
			UI::GUITextBox *txtTotalConnCnt;
			UI::GUILabel *lblMethod;
			UI::GUIComboBox *cboMethod;
			UI::GUILabel *lblPostSize;
			UI::GUITextBox *txtPostSize;
			UI::GUICheckBox *chkKAConn;
			UI::GUIButton *btnStart;
			UI::GUIGroupBox *grpStatus;
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
			static UInt32 __stdcall ProcessThread(void *userObj);
			static void __stdcall OnTimerTick(void *userObj);
			void StopThreads();
			void ClearURLs();
			const UTF8Char *GetNextURL();
		public:
			AVIRHTTPTestForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core);
			virtual ~AVIRHTTPTestForm();

			virtual void OnMonitorChanged();
		};
	};
};
#endif
