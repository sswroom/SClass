#ifndef _SM_SSWR_AVIREAD_AVIRGPSTRACKERFORM
#define _SM_SSWR_AVIREAD_AVIRGPSTRACKERFORM
#include "Data/SyncCircularBuff.h"
#include "IO/GPSNMEA.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "SSWR/AVIRead/IMapNavigator.h"
#include "UI/GUIButton.h"
#include "UI/GUICheckBox.h"
#include "UI/GUIForm.h"
#include "UI/GUIGroupBox.h"
#include "UI/GUIHSplitter.h"
#include "UI/GUILabel.h"
#include "UI/GUIListBox.h"
#include "UI/GUIListView.h"
#include "UI/GUITabControl.h"
#include "UI/GUITabPage.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRGPSTrackerForm : public UI::GUIForm
		{
		private:
			NotNullPtr<SSWR::AVIRead::AVIRCore> core;
			Map::ILocationService *locSvc;
			Bool relLocSvc;

			Map::GPSTrack::GPSRecord3 recCurr;
			Map::GPSTrack *gpsTrk;
			Sync::Mutex recMut;
			Bool recUpdated;
			UOSInt recSateCnt;
			Map::ILocationService::SateStatus recSates[32];
			SSWR::AVIRead::IMapNavigator *mapNavi;
			Math::GeographicCoordinateSystem *wgs84;
			Math::Coord2DDbl lastPos;
			Double dist;
			Bool lastDown;
			Data::DateTime lastUpdateTime;
			Bool dispOffClk;
			Int64 dispOffTime;
			Bool dispIsOff;

			Sync::Mutex nmeaMut;
			Text::String **nmeaBuff;
			UOSInt nmeaIndex;
			Bool nmeaUpdated;

			NotNullPtr<UI::GUITabControl> tcMain;

			NotNullPtr<UI::GUITabPage> tpLocation;
			NotNullPtr<UI::GUILabel> lblStreamStatus;
			NotNullPtr<UI::GUITextBox> txtStreamStatus;
			NotNullPtr<UI::GUILabel> lblGPSTime;
			NotNullPtr<UI::GUITextBox> txtGPSTime;
			NotNullPtr<UI::GUILabel> lblLatitude;
			NotNullPtr<UI::GUITextBox> txtLatitude;
			NotNullPtr<UI::GUILabel> lblLongitude;
			NotNullPtr<UI::GUITextBox> txtLongitude;
			NotNullPtr<UI::GUILabel> lblAltitude;
			NotNullPtr<UI::GUITextBox> txtAltitude;
			NotNullPtr<UI::GUILabel> lblSpeed;
			NotNullPtr<UI::GUITextBox> txtSpeed;
			NotNullPtr<UI::GUILabel> lblHeading;
			NotNullPtr<UI::GUITextBox> txtHeading;
			NotNullPtr<UI::GUILabel> lblGPSValid;
			NotNullPtr<UI::GUITextBox> txtGPSValid;
			NotNullPtr<UI::GUILabel> lblNSateUsed;
			NotNullPtr<UI::GUITextBox> txtNSateUsed;
			NotNullPtr<UI::GUILabel> lblNSateUsedGPS;
			NotNullPtr<UI::GUITextBox> txtNSateUsedGPS;
			NotNullPtr<UI::GUILabel> lblNSateUsedSBAS;
			NotNullPtr<UI::GUITextBox> txtNSateUsedSBAS;
			NotNullPtr<UI::GUILabel> lblNSateUsedGLO;
			NotNullPtr<UI::GUITextBox> txtNSateUsedGLO;
			NotNullPtr<UI::GUILabel> lblNSateViewGPS;
			NotNullPtr<UI::GUITextBox> txtNSateViewGPS;
			NotNullPtr<UI::GUILabel> lblNSateViewGLO;
			NotNullPtr<UI::GUITextBox> txtNSateViewGLO;
			NotNullPtr<UI::GUILabel> lblNSateViewGA;
			NotNullPtr<UI::GUITextBox> txtNSateViewGA;
			NotNullPtr<UI::GUILabel> lblNSateViewQZSS;
			NotNullPtr<UI::GUITextBox> txtNSateViewQZSS;
			NotNullPtr<UI::GUILabel> lblNSateViewBD;
			NotNullPtr<UI::GUITextBox> txtNSateViewBD;
			NotNullPtr<UI::GUICheckBox> chkAutoPan;
			NotNullPtr<UI::GUICheckBox> chkNoSleep;
			NotNullPtr<UI::GUICheckBox> chkTopMost;
			NotNullPtr<UI::GUIButton> btnDispOff;
			NotNullPtr<UI::GUILabel> lblDistance;
			NotNullPtr<UI::GUITextBox> txtDistance;

			NotNullPtr<UI::GUITabPage> tpAlert;
			NotNullPtr<UI::GUITabControl> tcAlert;
			NotNullPtr<UI::GUITabPage> tpAlertAdd;
			NotNullPtr<UI::GUIListBox> lbAlertLyr;
			NotNullPtr<UI::GUIHSplitter> hspAlertAdd;
			NotNullPtr<UI::GUIListBox> lbAlertAdd;
			NotNullPtr<UI::GUITabPage> tpAlertView;
			NotNullPtr<UI::GUIPanel> pnlAlertView;
			NotNullPtr<UI::GUIListBox> lbAlert;

			NotNullPtr<UI::GUITabPage> tpMTK;
			NotNullPtr<UI::GUIGroupBox> grpMTKFirmware;
			NotNullPtr<UI::GUILabel> lblMTKRelease;
			NotNullPtr<UI::GUITextBox> txtMTKRelease;
			NotNullPtr<UI::GUILabel> lblMTKBuildID;
			NotNullPtr<UI::GUITextBox> txtMTKBuildID;
			NotNullPtr<UI::GUILabel> lblMTKProdMode;
			NotNullPtr<UI::GUITextBox> txtMTKProdMode;
			NotNullPtr<UI::GUILabel> lblMTKSDKVer;
			NotNullPtr<UI::GUITextBox> txtMTKSDKVer;
			NotNullPtr<UI::GUIButton> btnMTKFirmware;
			NotNullPtr<UI::GUIButton> btnMTKLogDownload;
			NotNullPtr<UI::GUIButton> btnMTKLogDelete;
			NotNullPtr<UI::GUIButton> btnMTKFactoryReset;
			NotNullPtr<UI::GUIButton> btnMTKTest;

			NotNullPtr<UI::GUITabPage> tpSate;
			NotNullPtr<UI::GUIListView> lvSate;

			NotNullPtr<UI::GUITabPage> tpNMEA;
			NotNullPtr<UI::GUIListBox> lbNMEA;

			static void __stdcall OnGPSUpdate(AnyType userObj, NotNullPtr<Map::GPSTrack::GPSRecord3> record, Data::DataArray<Map::ILocationService::SateStatus> sates);
			static void __stdcall OnTimerTick(AnyType userObj);
			static void __stdcall OnMTKFirmwareClicked(AnyType userObj);
			static void __stdcall OnMTKLogDownloadClicked(AnyType userObj);
			static void __stdcall OnMTKLogDeleteClicked(AnyType userObj);
			static void __stdcall OnMTKTestClicked(AnyType userObj);
			static void __stdcall OnMTKFactoryResetClicked(AnyType userObj);
			static void __stdcall OnDispOffClicked(AnyType userObj);
			static void __stdcall OnTopMostChg(AnyType userObj, Bool newState);
			static void __stdcall OnNMEALine(AnyType userObj, const UTF8Char *line, UOSInt lineLen);
			static void __stdcall SignalFocusLost(AnyType userObj);
			NotNullPtr<UI::GUIButton> NewDisplayOffButton(NotNullPtr<UI::GUICore> ui, NotNullPtr<UI::GUIClientControl> parent, Text::CStringNN txt);
		public:
#if defined(_MSC_VER)
			void* operator new(size_t i)
			{
				return MAllocA64(i);
			}

			void operator delete(void* p)
			{
				MemFreeA(p);
			}
#endif
			AVIRGPSTrackerForm(Optional<UI::GUIClientControl> parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core, Map::ILocationService *locSvc, Bool toRelease);
			virtual ~AVIRGPSTrackerForm();

			virtual void OnMonitorChanged();
			virtual void OnFocus();

			void SetGPSTrack(Map::GPSTrack *gpsTrk);
			void SetMapNavigator(SSWR::AVIRead::IMapNavigator *mapNavi);

			void DispOffFocusLost();
		};
	}
}
#endif
