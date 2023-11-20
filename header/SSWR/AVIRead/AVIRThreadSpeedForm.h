#ifndef _SM_SSWR_AVIREAD_AVIRTHREADSPEEDFORM
#define _SM_SSWR_AVIREAD_AVIRTHREADSPEEDFORM
#include "Manage/HiResClock.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "Sync/Event.h"
#include "Sync/Mutex.h"
#include "UI/GUIButton.h"
#include "UI/GUIForm.h"
#include "UI/GUILabel.h"
#include "UI/GUIListView.h"
#include "UI/GUIPanel.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRThreadSpeedForm : public UI::GUIForm
		{
		private:
			NotNullPtr<UI::GUIPanel> pnlCtrl;
			UI::GUIButton *btnTest;
			UI::GUIListView *lvResult;

			NotNullPtr<SSWR::AVIRead::AVIRCore> core;

			Int32 tmpVal;
			Manage::HiResClock clk;
			Sync::Mutex mut;
			Sync::Event threadEvt;
			Sync::Event mainEvt;
			Double t;

			static UInt32 __stdcall TestThread(void *userObj);
			static void __stdcall OnTestClicked(void *userObj);
		public:
			AVIRThreadSpeedForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRThreadSpeedForm();

			virtual void OnMonitorChanged();
		};
	};
};
#endif
