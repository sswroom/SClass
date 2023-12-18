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
			UI::GUITabControl *tcMain;

			NotNullPtr<UI::GUITabPage> tpSummary;
			NotNullPtr<UI::GUIPanel> pnlSummary;
			UI::GUIListView *lvSummary;

			NotNullPtr<UI::GUITabPage> tpDetail;
			NotNullPtr<UI::GUIPanel> pnlDetail;
			NotNullPtr<UI::GUIButton> btnDetailCurr;
			UI::GUIListBox *lbDetail;
			UI::GUIHSplitter *hspDetail;
			UI::GUITabControl *tcDetail;

			NotNullPtr<UI::GUITabPage> tpDetInfo;
			UI::GUILabel *lblDetProcId;
			UI::GUITextBox *txtDetProcId;
			UI::GUILabel *lblDetParentId;
			UI::GUITextBox *txtDetParentId;
			UI::GUILabel *lblDetName;
			UI::GUITextBox *txtDetName;
			UI::GUILabel *lblDetPath;
			UI::GUITextBox *txtDetPath;
			UI::GUILabel *lblDetCmdLine;
			UI::GUITextBox *txtDetCmdLine;
			UI::GUILabel *lblDetWorkingDir;
			UI::GUITextBox *txtDetWorkingDir;
			UI::GUILabel *lblDetTrueProgPath;
			UI::GUITextBox *txtDetTrueProgPath;
			UI::GUILabel *lblDetPriority;
			UI::GUITextBox *txtDetPriority;
			UI::GUILabel* lblDetArchitecture;
			UI::GUITextBox* txtDetArchitecture;

			NotNullPtr<UI::GUITabPage> tpDetModule;
			NotNullPtr<UI::GUIPanel> pnlDetModule;
			NotNullPtr<UI::GUIButton> btnDetModule;
			UI::GUIListView *lvDetModule;

			NotNullPtr<UI::GUITabPage> tpDetThread;
			NotNullPtr<UI::GUIPanel> pnlDetThread;
			NotNullPtr<UI::GUIButton> btnDetThread;
			UI::GUIListView *lvDetThread;

			NotNullPtr<UI::GUITabPage> tpDetHeap;
			NotNullPtr<UI::GUIPanel> pnlDetHeap;
			NotNullPtr<UI::GUIButton> btnDetHeap;
			UI::GUIListBox *lbDetHeap;
			UI::GUIHSplitter *hspDetHeap;
			UI::GUITextBox *txtDetHeap;
			UI::GUIListView *lvDetHeap;

			NotNullPtr<UI::GUITabPage> tpDetHandle;
			NotNullPtr<UI::GUIPanel> pnlDetHandle;
			NotNullPtr<UI::GUIButton> btnDetHandle;
			UI::GUIListView *lvDetHandle;

			NotNullPtr<UI::GUITabPage> tpDetChart;
			NotNullPtr<UI::GUIGroupBox> grpDetChartCPU;
			UI::GUIRealtimeLineChart *rlcDetChartCPU;
			UI::GUIVSplitter *vspDetChartCPU;
			NotNullPtr<UI::GUIGroupBox> grpDetChartPage;
			UI::GUIRealtimeLineChart *rlcDetChartPage;
			UI::GUIVSplitter *vspDetChartPage;
			NotNullPtr<UI::GUIGroupBox> grpDetChartCount;
			UI::GUIRealtimeLineChart *rlcDetChartCount;
			UI::GUIVSplitter *vspDetChartCount;
			NotNullPtr<UI::GUIGroupBox> grpDetChartWS;
			UI::GUIRealtimeLineChart *rlcDetChartWS;

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

			static void __stdcall OnSumDblClicked(void *userObj, UOSInt index);
			static void __stdcall OnProcSelChg(void *userObj);
			static void __stdcall OnTimerTick(void *userObj);
			static void __stdcall OnTimerCPUTick(void *userObj);
			static void __stdcall OnDetailCurrClicked(void* userObj);
			static void __stdcall OnDetModuleRefClicked(void *userObj);
			static void __stdcall OnDetThreadRefClicked(void *userObj);
			static void __stdcall OnDetThreadDblClicked(void *userObj, UOSInt index);
			static void __stdcall OnDetHeapRefClicked(void *userObj);
			static void __stdcall OnDetHeapSelChg(void *userObj);
			static void __stdcall OnDetHeapItemSelChg(void *userObj);
			static void __stdcall OnDetHandleClicked(void *userObj);

			void UpdateProcModules();
			void UpdateProcThreads();
			void UpdateProcHeaps();
			void UpdateProcHeapDetail(UInt32 heapId);
			void UpdateProcHandles();
		public:
			AVIRProcInfoForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRProcInfoForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
