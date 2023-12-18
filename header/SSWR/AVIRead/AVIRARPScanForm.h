#ifndef _SM_SSWR_AVIREAD_AVIRARPSCANFORM
#define _SM_SSWR_AVIREAD_AVIRARPSCANFORM
#include "Data/FastMap.h"
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
			NotNullPtr<UI::GUIPanel> pnlCtrl;
			UI::GUILabel *lblAdapter;
			NotNullPtr<UI::GUIComboBox> cboAdapter;
			NotNullPtr<UI::GUIButton> btnScan;
			UI::GUIListView *lvARP;

			NotNullPtr<SSWR::AVIRead::AVIRCore> core;

			Data::ArrayList<AdapterInfo*> adapters;
			Sync::Mutex arpMut;
			Data::FastMap<UInt32, IPMapInfo *> arpMap;
			Bool arpUpdated;

			static void __stdcall OnARPHandler(const UInt8 *hwAddr, UInt32 ipAddr, void *userObj);
			static void __stdcall OnTimerTick(void *userObj);
			static void __stdcall OnScanClicked(void *userObj);

			void UpdateARPList();
		public:
			AVIRARPScanForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRARPScanForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
