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
			class DisplayOffButton : public UI::GUIButton
			{
			private:
				AVIRGPSTrackerForm *frm;
			public:
				DisplayOffButton(UI::GUICore *ui, UI::GUIClientControl *parent, Text::CString txt, AVIRGPSTrackerForm *frm);
				virtual ~DisplayOffButton();

				virtual void OnFocusLost();
			};
		private:
			SSWR::AVIRead::AVIRCore *core;
			Map::ILocationService *locSvc;
			Bool relLocSvc;

			Map::GPSTrack::GPSRecord3 recCurr;
			Map::GPSTrack *gpsTrk;
			Sync::Mutex *recMut;
			Bool recUpdated;
			UOSInt recSateCnt;
			Map::ILocationService::SateStatus recSates[32];
			SSWR::AVIRead::IMapNavigator *mapNavi;
			Math::GeographicCoordinateSystem *wgs84;
			Math::Coord2DDbl lastPos;
			Double dist;
			Bool lastDown;
			Data::DateTime *lastUpdateTime;
			Bool dispOffClk;
			Int64 dispOffTime;
			Bool dispIsOff;

			Sync::Mutex *nmeaMut;
			Text::String **nmeaBuff;
			UOSInt nmeaIndex;
			Bool nmeaUpdated;

			UI::GUITabControl *tcMain;

			UI::GUITabPage *tpLocation;
			UI::GUILabel *lblStreamStatus;
			UI::GUITextBox *txtStreamStatus;
			UI::GUILabel *lblGPSTime;
			UI::GUITextBox *txtGPSTime;
			UI::GUILabel *lblLatitude;
			UI::GUITextBox *txtLatitude;
			UI::GUILabel *lblLongitude;
			UI::GUITextBox *txtLongitude;
			UI::GUILabel *lblAltitude;
			UI::GUITextBox *txtAltitude;
			UI::GUILabel *lblSpeed;
			UI::GUITextBox *txtSpeed;
			UI::GUILabel *lblHeading;
			UI::GUITextBox *txtHeading;
			UI::GUILabel *lblGPSValid;
			UI::GUITextBox *txtGPSValid;
			UI::GUILabel *lblNSateUsed;
			UI::GUITextBox *txtNSateUsed;
			UI::GUILabel *lblNSateUsedGPS;
			UI::GUITextBox *txtNSateUsedGPS;
			UI::GUILabel *lblNSateUsedSBAS;
			UI::GUITextBox *txtNSateUsedSBAS;
			UI::GUILabel *lblNSateUsedGLO;
			UI::GUITextBox *txtNSateUsedGLO;
			UI::GUILabel *lblNSateViewGPS;
			UI::GUITextBox *txtNSateViewGPS;
			UI::GUILabel *lblNSateViewGLO;
			UI::GUITextBox *txtNSateViewGLO;
			UI::GUILabel *lblNSateViewGA;
			UI::GUITextBox *txtNSateViewGA;
			UI::GUILabel *lblNSateViewQZSS;
			UI::GUITextBox *txtNSateViewQZSS;
			UI::GUILabel *lblNSateViewBD;
			UI::GUITextBox *txtNSateViewBD;
			UI::GUICheckBox *chkAutoPan;
			UI::GUICheckBox *chkNoSleep;
			UI::GUICheckBox *chkTopMost;
			UI::GUIButton *btnDispOff;
			UI::GUILabel *lblDistance;
			UI::GUITextBox *txtDistance;

			UI::GUITabPage *tpAlert;
			UI::GUITabControl *tcAlert;
			UI::GUITabPage *tpAlertAdd;
			UI::GUIListBox *lbAlertLyr;
			UI::GUIHSplitter *hspAlertAdd;
			UI::GUIListBox *lbAlertAdd;
			UI::GUITabPage *tpAlertView;
			UI::GUIPanel *pnlAlertView;
			UI::GUIListBox *lbAlert;

			UI::GUITabPage *tpMTK;
			UI::GUIGroupBox *grpMTKFirmware;
			UI::GUILabel *lblMTKRelease;
			UI::GUITextBox *txtMTKRelease;
			UI::GUILabel *lblMTKBuildID;
			UI::GUITextBox *txtMTKBuildID;
			UI::GUILabel *lblMTKProdMode;
			UI::GUITextBox *txtMTKProdMode;
			UI::GUILabel *lblMTKSDKVer;
			UI::GUITextBox *txtMTKSDKVer;
			UI::GUIButton *btnMTKFirmware;
			UI::GUIButton *btnMTKLogDownload;
			UI::GUIButton *btnMTKLogDelete;
			UI::GUIButton *btnMTKFactoryReset;
			UI::GUIButton *btnMTKTest;

			UI::GUITabPage *tpSate;
			UI::GUIListView *lvSate;

			UI::GUITabPage *tpNMEA;
			UI::GUIListBox *lbNMEA;

			static void __stdcall OnGPSUpdate(void *userObj, Map::GPSTrack::GPSRecord3 *record, UOSInt sateCnt, Map::ILocationService::SateStatus *sates);
			static void __stdcall OnTimerTick(void *userObj);
			static void __stdcall OnMTKFirmwareClicked(void *userObj);
			static void __stdcall OnMTKLogDownloadClicked(void *userObj);
			static void __stdcall OnMTKLogDeleteClicked(void *userObj);
			static void __stdcall OnMTKTestClicked(void *userObj);
			static void __stdcall OnMTKFactoryResetClicked(void *userObj);
			static void __stdcall OnDispOffClicked(void *userObj);
			static void __stdcall OnTopMostChg(void *userObj, Bool newState);
			static void __stdcall OnNMEALine(void *userObj, const UTF8Char *line, UOSInt lineLen);
		public:
			AVIRGPSTrackerForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core, Map::ILocationService *locSvc, Bool toRelease);
			virtual ~AVIRGPSTrackerForm();

			virtual void OnMonitorChanged();
			virtual void OnFocus();

			void SetGPSTrack(Map::GPSTrack *gpsTrk);
			void SetMapNavigator(SSWR::AVIRead::IMapNavigator *mapNavi);

			void DispOffFocusLost();
		};
	};
};
#endif
