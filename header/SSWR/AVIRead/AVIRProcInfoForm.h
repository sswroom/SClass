#ifndef _SM_SSWR_AVIREAD_AVIRPROCINFOFORM
#define _SM_SSWR_AVIREAD_AVIRPROCINFOFORM

#include "Manage/HiResClock.h"
#include "Manage/SymbolResolver.h"
#include "SSWR/AVIRead/AVIRCore.h"
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
	namespace AVIRead
	{
		class AVIRProcInfoForm : public UI::GUIForm
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
			NotNullPtr<UI::GUIPanel> pnlSummary;
			NotNullPtr<UI::GUIListView> lvSummary;

			NotNullPtr<UI::GUITabPage> tpDetail;
			NotNullPtr<UI::GUIPanel> pnlDetail;
			NotNullPtr<UI::GUIButton> btnDetailCurr;
			NotNullPtr<UI::GUIListBox> lbDetail;
			NotNullPtr<UI::GUIHSplitter> hspDetail;
			NotNullPtr<UI::GUITabControl> tcDetail;

			NotNullPtr<UI::GUITabPage> tpDetInfo;
			NotNullPtr<UI::GUILabel> lblDetProcId;
			NotNullPtr<UI::GUITextBox> txtDetProcId;
			NotNullPtr<UI::GUILabel> lblDetParentId;
			NotNullPtr<UI::GUITextBox> txtDetParentId;
			NotNullPtr<UI::GUILabel> lblDetName;
			NotNullPtr<UI::GUITextBox> txtDetName;
			NotNullPtr<UI::GUILabel> lblDetPath;
			NotNullPtr<UI::GUITextBox> txtDetPath;
			NotNullPtr<UI::GUILabel> lblDetCmdLine;
			NotNullPtr<UI::GUITextBox> txtDetCmdLine;
			NotNullPtr<UI::GUILabel> lblDetWorkingDir;
			NotNullPtr<UI::GUITextBox> txtDetWorkingDir;
			NotNullPtr<UI::GUILabel> lblDetTrueProgPath;
			NotNullPtr<UI::GUITextBox> txtDetTrueProgPath;
			NotNullPtr<UI::GUILabel> lblDetPriority;
			NotNullPtr<UI::GUITextBox> txtDetPriority;
			NotNullPtr<UI::GUILabel> lblDetArchitecture;
			NotNullPtr<UI::GUITextBox> txtDetArchitecture;

			NotNullPtr<UI::GUITabPage> tpDetModule;
			NotNullPtr<UI::GUIPanel> pnlDetModule;
			NotNullPtr<UI::GUIButton> btnDetModule;
			NotNullPtr<UI::GUIListView> lvDetModule;

			NotNullPtr<UI::GUITabPage> tpDetThread;
			NotNullPtr<UI::GUIPanel> pnlDetThread;
			NotNullPtr<UI::GUIButton> btnDetThread;
			NotNullPtr<UI::GUIListView> lvDetThread;

			NotNullPtr<UI::GUITabPage> tpDetHeap;
			NotNullPtr<UI::GUIPanel> pnlDetHeap;
			NotNullPtr<UI::GUIButton> btnDetHeap;
			NotNullPtr<UI::GUIListBox> lbDetHeap;
			NotNullPtr<UI::GUIHSplitter> hspDetHeap;
			NotNullPtr<UI::GUITextBox> txtDetHeap;
			NotNullPtr<UI::GUIListView> lvDetHeap;

			NotNullPtr<UI::GUITabPage> tpDetHandle;
			NotNullPtr<UI::GUIPanel> pnlDetHandle;
			NotNullPtr<UI::GUIButton> btnDetHandle;
			NotNullPtr<UI::GUIListView> lvDetHandle;

			NotNullPtr<UI::GUITabPage> tpDetChart;
			NotNullPtr<UI::GUIGroupBox> grpDetChartCPU;
			NotNullPtr<UI::GUIRealtimeLineChart> rlcDetChartCPU;
			NotNullPtr<UI::GUIVSplitter> vspDetChartCPU;
			NotNullPtr<UI::GUIGroupBox> grpDetChartPage;
			NotNullPtr<UI::GUIRealtimeLineChart> rlcDetChartPage;
			NotNullPtr<UI::GUIVSplitter> vspDetChartPage;
			NotNullPtr<UI::GUIGroupBox> grpDetChartCount;
			NotNullPtr<UI::GUIRealtimeLineChart> rlcDetChartCount;
			NotNullPtr<UI::GUIVSplitter> vspDetChartCount;
			NotNullPtr<UI::GUIGroupBox> grpDetChartWS;
			NotNullPtr<UI::GUIRealtimeLineChart> rlcDetChartWS;

			NotNullPtr<SSWR::AVIRead::AVIRCore> core;

			Data::ArrayListUInt32 procIds;
			Data::ArrayList<ProcessInfo*> procList;

			UOSInt currProc;
			Manage::Process *currProcObj;
			Manage::SymbolResolver *currProcRes;
			Manage::HiResClock clk;
			Data::Timestamp lastUserTime;
			Data::Timestamp lastKernelTime;
			UOSInt threadCnt;

			static void __stdcall OnSumDblClicked(AnyType userObj, UOSInt index);
			static void __stdcall OnProcSelChg(AnyType userObj);
			static void __stdcall OnTimerTick(AnyType userObj);
			static void __stdcall OnTimerCPUTick(AnyType userObj);
			static void __stdcall OnDetailCurrClicked(AnyType userObj);
			static void __stdcall OnDetModuleRefClicked(AnyType userObj);
			static void __stdcall OnDetThreadRefClicked(AnyType userObj);
			static void __stdcall OnDetThreadDblClicked(AnyType userObj, UOSInt index);
			static void __stdcall OnDetHeapRefClicked(AnyType userObj);
			static void __stdcall OnDetHeapSelChg(AnyType userObj);
			static void __stdcall OnDetHeapItemSelChg(AnyType userObj);
			static void __stdcall OnDetHandleClicked(AnyType userObj);

			void UpdateProcModules();
			void UpdateProcThreads();
			void UpdateProcHeaps();
			void UpdateProcHeapDetail(UInt32 heapId);
			void UpdateProcHandles();
		public:
			AVIRProcInfoForm(Optional<UI::GUIClientControl> parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRProcInfoForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
