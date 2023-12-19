#ifndef _SM_SSWR_AVIREAD_AVIRGISREPLAYFORM
#define _SM_SSWR_AVIREAD_AVIRGISREPLAYFORM
#include "Map/GPSTrack.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "SSWR/AVIRead/IMapNavigator.h"
#include "UI/GUIComboBox.h"
#include "UI/GUIForm.h"
#include "UI/GUIHSplitter.h"
#include "UI/GUILabel.h"
#include "UI/GUIListBox.h"
#include "UI/GUIListView.h"
#include "UI/GUIPanel.h"
#include "UI/GUIPopupMenu.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRGISReplayForm : public UI::GUIForm
		{
		private:
			UI::GUIListBox *lbRecord;
			NotNullPtr<UI::GUITextBox> txtGPSTime;
			NotNullPtr<UI::GUITextBox> txtLatitude;
			NotNullPtr<UI::GUITextBox> txtLongitude;
			NotNullPtr<UI::GUITextBox> txtAltitude;
			NotNullPtr<UI::GUITextBox> txtSpeedKnot;
			NotNullPtr<UI::GUITextBox> txtSpeedKM;
			NotNullPtr<UI::GUITextBox> txtHeading;
			NotNullPtr<UI::GUITextBox> txtValid;
			NotNullPtr<UI::GUITextBox> txtNSateView;
			NotNullPtr<UI::GUITextBox> txtNSateUsed;
			NotNullPtr<UI::GUITextBox> txtEndMark;
			NotNullPtr<UI::GUITextBox> txtStartMark;
			UI::GUIPopupMenu *mnuRecord;
			NotNullPtr<UI::GUITextBox> txtAddress;
			NotNullPtr<UI::GUILabel> lblAddress;
			NotNullPtr<UI::GUIComboBox> cboName;
			NotNullPtr<UI::GUILabel> lblDist;
			UI::GUIListView *lvExtra;

			NotNullPtr<SSWR::AVIRead::AVIRCore> core;
			NotNullPtr<Map::GPSTrack> track;
			IMapNavigator *navi;
			UOSInt startMark;
			UOSInt endMark;
			Text::String **names;
			UOSInt namesCnt;
			Bool threadRunning;
			Bool threadToStop;
			UOSInt currTrackId;

		private:
			static UInt32 __stdcall AddressThread(void *userObj);
			static void __stdcall OnCboNameChg(void *userObj);
			static void __stdcall OnLbRecordChg(void *userObj);
			static Bool __stdcall OnLbRecordRClick(void *userObj, Math::Coord2D<OSInt> scnPos, UI::GUIControl::MouseButton btn);
			void FreeNames();

		public:
			AVIRGISReplayForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core, NotNullPtr<Map::GPSTrack> track, IMapNavigator *navi);
			virtual ~AVIRGISReplayForm();

			virtual void EventMenuClicked(UInt16 cmdId);
			virtual void OnMonitorChanged();

			void UpdateRecList();
			void StopThread();
		};
	}
}
#endif
