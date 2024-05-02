#ifndef _SM_SSWR_AVIREADCE_AVIRCEPROCINFOFORM
#define _SM_SSWR_AVIREADCE_AVIRCEPROCINFOFORM

#include "Data/ArrayListNN.h"
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
				NN<Text::String> procName;
				UInt32 parentProcId;
				Bool found;
			} ProcessInfo;
		private:
			NN<UI::GUITabControl> tcMain;
			NN<UI::GUITabPage> tpSummary;
			NN<UI::GUITabPage> tpDetail;

			NN<UI::GUIPanel> pnlSummary;
			NN<UI::GUIListView> lvSummary;

			NN<UI::GUIListBox> lbDetail;
			NN<UI::GUIHSplitter> hspDetail;
			NN<UI::GUITabControl> tcDetail;
			NN<UI::GUITabPage> tpDetInfo;
			NN<UI::GUITabPage> tpDetModule;
			NN<UI::GUITabPage> tpDetThread;
			NN<UI::GUITabPage> tpDetHeap;
			NN<UI::GUITabPage> tpDetChart;

			NN<UI::GUILabel> lblDetProcId;
			NN<UI::GUITextBox> txtDetProcId;
			NN<UI::GUILabel> lblDetParentId;
			NN<UI::GUITextBox> txtDetParentId;
			NN<UI::GUILabel> lblDetName;
			NN<UI::GUITextBox> txtDetName;
			NN<UI::GUILabel> lblDetPath;
			NN<UI::GUITextBox> txtDetPath;
			NN<UI::GUILabel> lblDetPriority;
			NN<UI::GUITextBox> txtDetPriority;
			
			NN<UI::GUIPanel> pnlDetModule;
			NN<UI::GUIButton> btnDetModule;
			NN<UI::GUIListView> lvDetModule;

			NN<UI::GUIPanel> pnlDetThread;
			NN<UI::GUIButton> btnDetThread;
			NN<UI::GUIListView> lvDetThread;

			NN<UI::GUIPanel> pnlDetHeap;
			NN<UI::GUIButton> btnDetHeap;
			NN<UI::GUIListBox> lbDetHeap;
			NN<UI::GUIHSplitter> hspDetHeap;
			NN<UI::GUIListView> lvDetHeap;

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

			Data::ArrayListInt32 procIds;
			Data::ArrayListNN<ProcessInfo> procList;

			UInt32 currProc;
			Manage::Process *currProcObj;
			Manage::SymbolResolver *currProcRes;
			Manage::HiResClock clk;
			Data::Timestamp lastUserTime;
			Data::Timestamp lastKernelTime;
			Int32 threadCnt;

			static void __stdcall OnSumDblClicked(AnyType userObj, UOSInt index);
			static void __stdcall OnProcSelChg(AnyType userObj);
			static void __stdcall OnTimerTick(AnyType userObj);
			static void __stdcall OnTimerCPUTick(AnyType userObj);
			static void __stdcall OnDetModuleRefClicked(AnyType userObj);
			static void __stdcall OnDetThreadRefClicked(AnyType userObj);
			static void __stdcall OnDetThreadDblClicked(AnyType userObj, UOSInt index);
			static void __stdcall OnDetHeapRefClicked(AnyType userObj);
			static void __stdcall OnDetHeapSelChg(AnyType userObj);

			void UpdateProcModules();
			void UpdateProcThreads();
			void UpdateProcHeaps();
			void UpdateProcHeapDetail(Int32 heapId);
		public:
			AVIRCEProcInfoForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRCEProcInfoForm();
		};
	}
}
#endif
