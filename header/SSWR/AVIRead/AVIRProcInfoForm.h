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
				NN<Text::String> procName;
				UInt32 parentProcId;
				Bool found;
			} ProcessInfo;
		private:
			NN<UI::GUITabControl> tcMain;

			NN<UI::GUITabPage> tpSummary;
			NN<UI::GUIPanel> pnlSummary;
			NN<UI::GUIListView> lvSummary;

			NN<UI::GUITabPage> tpDetail;
			NN<UI::GUIPanel> pnlDetail;
			NN<UI::GUIButton> btnDetailCurr;
			NN<UI::GUIListBox> lbDetail;
			NN<UI::GUIHSplitter> hspDetail;
			NN<UI::GUITabControl> tcDetail;

			NN<UI::GUITabPage> tpDetInfo;
			NN<UI::GUILabel> lblDetProcId;
			NN<UI::GUITextBox> txtDetProcId;
			NN<UI::GUILabel> lblDetParentId;
			NN<UI::GUITextBox> txtDetParentId;
			NN<UI::GUILabel> lblDetName;
			NN<UI::GUITextBox> txtDetName;
			NN<UI::GUILabel> lblDetPath;
			NN<UI::GUITextBox> txtDetPath;
			NN<UI::GUILabel> lblDetCmdLine;
			NN<UI::GUITextBox> txtDetCmdLine;
			NN<UI::GUILabel> lblDetWorkingDir;
			NN<UI::GUITextBox> txtDetWorkingDir;
			NN<UI::GUILabel> lblDetTrueProgPath;
			NN<UI::GUITextBox> txtDetTrueProgPath;
			NN<UI::GUILabel> lblDetPriority;
			NN<UI::GUITextBox> txtDetPriority;
			NN<UI::GUILabel> lblDetArchitecture;
			NN<UI::GUITextBox> txtDetArchitecture;

			NN<UI::GUITabPage> tpDetModule;
			NN<UI::GUIPanel> pnlDetModule;
			NN<UI::GUIButton> btnDetModule;
			NN<UI::GUIListView> lvDetModule;

			NN<UI::GUITabPage> tpDetThread;
			NN<UI::GUIPanel> pnlDetThread;
			NN<UI::GUIButton> btnDetThread;
			NN<UI::GUIListView> lvDetThread;

			NN<UI::GUITabPage> tpDetHeap;
			NN<UI::GUIPanel> pnlDetHeap;
			NN<UI::GUIButton> btnDetHeap;
			NN<UI::GUIListBox> lbDetHeap;
			NN<UI::GUIHSplitter> hspDetHeap;
			NN<UI::GUITextBox> txtDetHeap;
			NN<UI::GUIListView> lvDetHeap;

			NN<UI::GUITabPage> tpDetHandle;
			NN<UI::GUIPanel> pnlDetHandle;
			NN<UI::GUIButton> btnDetHandle;
			NN<UI::GUIListView> lvDetHandle;

			NN<UI::GUITabPage> tpDetChart;
			NN<UI::GUIGroupBox> grpDetChartCPU;
			NN<UI::GUIRealtimeLineChart> rlcDetChartCPU;
			NN<UI::GUIVSplitter> vspDetChartCPU;
			NN<UI::GUIGroupBox> grpDetChartPage;
			NN<UI::GUIRealtimeLineChart> rlcDetChartPage;
			NN<UI::GUIVSplitter> vspDetChartPage;
			NN<UI::GUIGroupBox> grpDetChartCount;
			NN<UI::GUIRealtimeLineChart> rlcDetChartCount;
			NN<UI::GUIVSplitter> vspDetChartCount;
			NN<UI::GUIGroupBox> grpDetChartWS;
			NN<UI::GUIRealtimeLineChart> rlcDetChartWS;

			NN<SSWR::AVIRead::AVIRCore> core;

			Data::ArrayListUInt32 procIds;
			Data::ArrayListNN<ProcessInfo> procList;

			UIntOS currProc;
			Optional<Manage::Process> currProcObj;
			Optional<Manage::SymbolResolver> currProcRes;
			Manage::HiResClock clk;
			Data::Timestamp lastUserTime;
			Data::Timestamp lastKernelTime;
			UIntOS threadCnt;

			static void __stdcall OnSumDblClicked(AnyType userObj, UIntOS index);
			static void __stdcall OnProcSelChg(AnyType userObj);
			static void __stdcall OnTimerTick(AnyType userObj);
			static void __stdcall OnTimerCPUTick(AnyType userObj);
			static void __stdcall OnDetailCurrClicked(AnyType userObj);
			static void __stdcall OnDetModuleRefClicked(AnyType userObj);
			static void __stdcall OnDetThreadRefClicked(AnyType userObj);
			static void __stdcall OnDetThreadDblClicked(AnyType userObj, UIntOS index);
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
			AVIRProcInfoForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRProcInfoForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
