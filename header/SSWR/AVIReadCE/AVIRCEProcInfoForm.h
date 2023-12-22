#ifndef _SM_SSWR_AVIREADCE_AVIRCEPROCINFOFORM
#define _SM_SSWR_AVIREADCE_AVIRCEPROCINFOFORM

#include "Manage/HiResClock.h"
#include "Manage/SymbolResolver.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "Text/String.h"
#include "UI/GUIButton.h"
#include "UI/GUICheckBox.h"
#include "UI/GUIForm.h"
#include "UI/GUIGroupBox.h"
#include "UI/GUIHSplitter.h"
#include "UI/GUILabel.h"
#include "UI/GUIListBox.h"
#include "UI/GUIListView.h"
#include "UI/GUIPanel.h"
#include "UI/GUIRealtimeLineChart.h"
#include "UI/GUITabControl.h"
#include "UI/GUITabPage.h"
#include "UI/GUITextBox.h"
#include "UI/GUIVSplitter.h"

namespace SSWR
{
	namespace AVIReadCE
	{
		class AVIRCEProcInfoForm : public UI::GUIForm
		{
		private:
			typedef struct
			{
				UInt32 procId;
				NotNullPtr<Text::String> procName;
				UInt32 parentProcId;
				Bool found;
			} ProcessInfo;
		private:
			NotNullPtr<UI::GUITabControl> tcMain;
			NotNullPtr<UI::GUITabPage> tpSummary;
			NotNullPtr<UI::GUITabPage> tpDetail;

			NotNullPtr<UI::GUIPanel> pnlSummary;
			NotNullPtr<UI::GUIListView> lvSummary;

			NotNullPtr<UI::GUIListBox> lbDetail;
			NotNullPtr<UI::GUIHSplitter> hspDetail;
			NotNullPtr<UI::GUITabControl> tcDetail;
			NotNullPtr<UI::GUITabPage> tpDetInfo;
			NotNullPtr<UI::GUITabPage> tpDetModule;
			NotNullPtr<UI::GUITabPage> tpDetThread;
			NotNullPtr<UI::GUITabPage> tpDetHeap;
			NotNullPtr<UI::GUITabPage> tpDetChart;

			NotNullPtr<UI::GUILabel> lblDetProcId;
			NotNullPtr<UI::GUITextBox> txtDetProcId;
			NotNullPtr<UI::GUILabel> lblDetParentId;
			NotNullPtr<UI::GUITextBox> txtDetParentId;
			NotNullPtr<UI::GUILabel> lblDetName;
			NotNullPtr<UI::GUITextBox> txtDetName;
			NotNullPtr<UI::GUILabel> lblDetPath;
			NotNullPtr<UI::GUITextBox> txtDetPath;
			NotNullPtr<UI::GUILabel> lblDetPriority;
			NotNullPtr<UI::GUITextBox> txtDetPriority;
			
			NotNullPtr<UI::GUIPanel> pnlDetModule;
			NotNullPtr<UI::GUIButton> btnDetModule;
			NotNullPtr<UI::GUIListView> lvDetModule;

			NotNullPtr<UI::GUIPanel> pnlDetThread;
			NotNullPtr<UI::GUIButton> btnDetThread;
			NotNullPtr<UI::GUIListView> lvDetThread;

			NotNullPtr<UI::GUIPanel> pnlDetHeap;
			NotNullPtr<UI::GUIButton> btnDetHeap;
			NotNullPtr<UI::GUIListBox> lbDetHeap;
			NotNullPtr<UI::GUIHSplitter> hspDetHeap;
			NotNullPtr<UI::GUIListView> lvDetHeap;

			NotNullPtr<UI::GUIGroupBox> grpDetChartCPU;
			UI::GUIRealtimeLineChart *rlcDetChartCPU;
			NotNullPtr<UI::GUIVSplitter> vspDetChartCPU;
			NotNullPtr<UI::GUIGroupBox> grpDetChartPage;
			UI::GUIRealtimeLineChart *rlcDetChartPage;
			NotNullPtr<UI::GUIVSplitter> vspDetChartPage;
			NotNullPtr<UI::GUIGroupBox> grpDetChartCount;
			UI::GUIRealtimeLineChart *rlcDetChartCount;
			NotNullPtr<UI::GUIVSplitter> vspDetChartCount;
			NotNullPtr<UI::GUIGroupBox> grpDetChartWS;
			UI::GUIRealtimeLineChart *rlcDetChartWS;

			NotNullPtr<SSWR::AVIRead::AVIRCore> core;

			Data::ArrayListInt32 *procIds;
			Data::ArrayList<ProcessInfo*> *procList;

			UInt32 currProc;
			Manage::Process *currProcObj;
			Manage::SymbolResolver *currProcRes;
			Manage::HiResClock *clk;
			Data::Timestamp lastUserTime;
			Data::Timestamp lastKernelTime;
			Int32 threadCnt;

			static void __stdcall OnSumDblClicked(void *userObj, UOSInt index);
			static void __stdcall OnProcSelChg(void *userObj);
			static void __stdcall OnTimerTick(void *userObj);
			static void __stdcall OnTimerCPUTick(void *userObj);
			static void __stdcall OnDetModuleRefClicked(void *userObj);
			static void __stdcall OnDetThreadRefClicked(void *userObj);
			static void __stdcall OnDetThreadDblClicked(void *userObj, UOSInt index);
			static void __stdcall OnDetHeapRefClicked(void *userObj);
			static void __stdcall OnDetHeapSelChg(void *userObj);

			void UpdateProcModules();
			void UpdateProcThreads();
			void UpdateProcHeaps();
			void UpdateProcHeapDetail(Int32 heapId);
		public:
			AVIRCEProcInfoForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRCEProcInfoForm();
		};
	}
}
#endif
