#ifndef _SM_SSWR_AVIREAD_AVIRARPSCANFORM
#define _SM_SSWR_AVIREAD_AVIRARPSCANFORM
#include "Data/FastMapNN.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "Sync/Mutex.h"
#include "UI/GUIButton.h"
#include "UI/GUIComboBox.h"
#include "UI/GUIForm.h"
#include "UI/GUILabel.h"
#include "UI/GUIListView.h"
#include "UI/GUIPanel.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRARPScanForm : public UI::GUIForm
		{
		private:
			typedef struct
			{
				const UTF8Char *ifName;
				UInt32 ipAddr;
				UInt8 hwAddr[6];
			} AdapterInfo;
			
			typedef struct
			{
				UInt32 ipAddr;
				UInt8 hwAddr[6];
			} IPMapInfo;
			
		private:
			NN<UI::GUIPanel> pnlCtrl;
			NN<UI::GUILabel> lblAdapter;
			NN<UI::GUIComboBox> cboAdapter;
			NN<UI::GUIButton> btnScan;
			NN<UI::GUIListView> lvARP;

			NN<SSWR::AVIRead::AVIRCore> core;

			Data::ArrayListNN<AdapterInfo> adapters;
			Sync::Mutex arpMut;
			Data::FastMapNN<UInt32, IPMapInfo> arpMap;
			Bool arpUpdated;

			static void __stdcall OnARPHandler(UnsafeArray<const UInt8> hwAddr, UInt32 ipAddr, AnyType userObj);
			static void __stdcall OnTimerTick(AnyType userObj);
			static void __stdcall OnScanClicked(AnyType userObj);

			void UpdateARPList();
		public:
			AVIRARPScanForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRARPScanForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
