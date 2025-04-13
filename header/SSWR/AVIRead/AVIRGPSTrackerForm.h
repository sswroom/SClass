#ifndef _SM_SSWR_AVIREAD_AVIRGPSTRACKERFORM
#define _SM_SSWR_AVIREAD_AVIRGPSTRACKERFORM
#include "Data/SyncCircularBuff.h"
#include "IO/GPSNMEA.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "SSWR/AVIRead/AVIRMapNavigator.h"
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
			NN<SSWR::AVIRead::AVIRCore> core;
			NN<Map::LocationService> locSvc;
			Bool relLocSvc;

			Map::GPSTrack::GPSRecord3 recCurr;
			Optional<Map::GPSTrack> gpsTrk;
			Sync::Mutex recMut;
			Bool recUpdated;
			UOSInt recSateCnt;
			Map::LocationService::SateStatus recSates[32];
			Optional<SSWR::AVIRead::AVIRMapNavigator> mapNavi;
			NN<Math::GeographicCoordinateSystem> wgs84;
			Math::Coord2DDbl lastPos;
			Double dist;
			Bool lastDown;
			Data::DateTime lastUpdateTime;
			Bool dispOffClk;
			Int64 dispOffTime;
			Bool dispIsOff;

			Sync::Mutex nmeaMut;
			UnsafeArray<Optional<Text::String>> nmeaBuff;
			UOSInt nmeaIndex;
			Bool nmeaUpdated;

			NN<UI::GUITabControl> tcMain;

			NN<UI::GUITabPage> tpLocation;
			NN<UI::GUILabel> lblStreamStatus;
			NN<UI::GUITextBox> txtStreamStatus;
			NN<UI::GUILabel> lblGPSTime;
			NN<UI::GUITextBox> txtGPSTime;
			NN<UI::GUILabel> lblLatitude;
			NN<UI::GUITextBox> txtLatitude;
			NN<UI::GUILabel> lblLongitude;
			NN<UI::GUITextBox> txtLongitude;
			NN<UI::GUILabel> lblAltitude;
			NN<UI::GUITextBox> txtAltitude;
			NN<UI::GUILabel> lblSpeed;
			NN<UI::GUITextBox> txtSpeed;
			NN<UI::GUILabel> lblHeading;
			NN<UI::GUITextBox> txtHeading;
			NN<UI::GUILabel> lblGPSValid;
			NN<UI::GUITextBox> txtGPSValid;
			NN<UI::GUILabel> lblNSateUsed;
			NN<UI::GUITextBox> txtNSateUsed;
			NN<UI::GUILabel> lblNSateUsedGPS;
			NN<UI::GUITextBox> txtNSateUsedGPS;
			NN<UI::GUILabel> lblNSateUsedSBAS;
			NN<UI::GUITextBox> txtNSateUsedSBAS;
			NN<UI::GUILabel> lblNSateUsedGLO;
			NN<UI::GUITextBox> txtNSateUsedGLO;
			NN<UI::GUILabel> lblNSateViewGPS;
			NN<UI::GUITextBox> txtNSateViewGPS;
			NN<UI::GUILabel> lblNSateViewGLO;
			NN<UI::GUITextBox> txtNSateViewGLO;
			NN<UI::GUILabel> lblNSateViewGA;
			NN<UI::GUITextBox> txtNSateViewGA;
			NN<UI::GUILabel> lblNSateViewQZSS;
			NN<UI::GUITextBox> txtNSateViewQZSS;
			NN<UI::GUILabel> lblNSateViewBD;
			NN<UI::GUITextBox> txtNSateViewBD;
			NN<UI::GUICheckBox> chkAutoPan;
			NN<UI::GUICheckBox> chkNoSleep;
			NN<UI::GUICheckBox> chkTopMost;
			NN<UI::GUIButton> btnDispOff;
			NN<UI::GUILabel> lblDistance;
			NN<UI::GUITextBox> txtDistance;

			NN<UI::GUITabPage> tpAlert;
			NN<UI::GUITabControl> tcAlert;
			NN<UI::GUITabPage> tpAlertAdd;
			NN<UI::GUIListBox> lbAlertLyr;
			NN<UI::GUIHSplitter> hspAlertAdd;
			NN<UI::GUIListBox> lbAlertAdd;
			NN<UI::GUITabPage> tpAlertView;
			NN<UI::GUIPanel> pnlAlertView;
			NN<UI::GUIListBox> lbAlert;

			NN<UI::GUITabPage> tpMTK;
			NN<UI::GUIGroupBox> grpMTKFirmware;
			NN<UI::GUILabel> lblMTKRelease;
			NN<UI::GUITextBox> txtMTKRelease;
			NN<UI::GUILabel> lblMTKBuildID;
			NN<UI::GUITextBox> txtMTKBuildID;
			NN<UI::GUILabel> lblMTKProdMode;
			NN<UI::GUITextBox> txtMTKProdMode;
			NN<UI::GUILabel> lblMTKSDKVer;
			NN<UI::GUITextBox> txtMTKSDKVer;
			NN<UI::GUIButton> btnMTKFirmware;
			NN<UI::GUIButton> btnMTKLogDownload;
			NN<UI::GUIButton> btnMTKLogDelete;
			NN<UI::GUIButton> btnMTKFactoryReset;
			NN<UI::GUIButton> btnMTKTest;

			NN<UI::GUITabPage> tpSate;
			NN<UI::GUIListView> lvSate;

			NN<UI::GUITabPage> tpNMEA;
			NN<UI::GUIListBox> lbNMEA;

			static void __stdcall OnGPSUpdate(AnyType userObj, NN<Map::GPSTrack::GPSRecord3> record, Data::DataArray<Map::LocationService::SateStatus> sates);
			static void __stdcall OnTimerTick(AnyType userObj);
			static void __stdcall OnMTKFirmwareClicked(AnyType userObj);
			static void __stdcall OnMTKLogDownloadClicked(AnyType userObj);
			static void __stdcall OnMTKLogDeleteClicked(AnyType userObj);
			static void __stdcall OnMTKTestClicked(AnyType userObj);
			static void __stdcall OnMTKFactoryResetClicked(AnyType userObj);
			static void __stdcall OnDispOffClicked(AnyType userObj);
			static void __stdcall OnTopMostChg(AnyType userObj, Bool newState);
			static void __stdcall OnNMEALine(AnyType userObj, UnsafeArray<const UTF8Char> line, UOSInt lineLen);
			static void __stdcall SignalFocusLost(AnyType userObj);
			NN<UI::GUIButton> NewDisplayOffButton(NN<UI::GUICore> ui, NN<UI::GUIClientControl> parent, Text::CStringNN txt);
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
			AVIRGPSTrackerForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core, NN<Map::LocationService> locSvc, Bool toRelease);
			virtual ~AVIRGPSTrackerForm();

			virtual void OnMonitorChanged();
			virtual void OnFocus();

			void SetGPSTrack(Optional<Map::GPSTrack> gpsTrk);
			void SetMapNavigator(Optional<SSWR::AVIRead::AVIRMapNavigator> mapNavi);

			void DispOffFocusLost();
		};
	}
}
#endif
