#ifndef _SM_SSWR_AVIREAD_AVIRGISREPLAYFORM
#define _SM_SSWR_AVIREAD_AVIRGISREPLAYFORM
#include "Map/GPSTrack.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "SSWR/AVIRead/AVIRMapNavigator.h"
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
			NN<UI::GUIListBox> lbRecord;
			NN<UI::GUITextBox> txtGPSTime;
			NN<UI::GUITextBox> txtLatitude;
			NN<UI::GUITextBox> txtLongitude;
			NN<UI::GUITextBox> txtAltitude;
			NN<UI::GUITextBox> txtSpeedKnot;
			NN<UI::GUITextBox> txtSpeedKM;
			NN<UI::GUITextBox> txtHeading;
			NN<UI::GUITextBox> txtValid;
			NN<UI::GUITextBox> txtNSateView;
			NN<UI::GUITextBox> txtNSateUsed;
			NN<UI::GUITextBox> txtEndMark;
			NN<UI::GUITextBox> txtStartMark;
			NN<UI::GUIPopupMenu> mnuRecord;
			NN<UI::GUITextBox> txtAddress;
			NN<UI::GUILabel> lblAddress;
			NN<UI::GUIComboBox> cboName;
			NN<UI::GUILabel> lblDist;
			NN<UI::GUIListView> lvExtra;

			NN<SSWR::AVIRead::AVIRCore> core;
			NN<Map::GPSTrack> track;
			NN<AVIRMapNavigator> navi;
			UOSInt startMark;
			UOSInt endMark;
			UnsafeArrayOpt<Optional<Text::String>> names;
			UOSInt namesCnt;
			Bool threadRunning;
			Bool threadToStop;
			UOSInt currTrackId;

		private:
			static UInt32 __stdcall AddressThread(AnyType userObj);
			static void __stdcall OnCboNameChg(AnyType userObj);
			static void __stdcall OnLbRecordChg(AnyType userObj);
			static UI::EventState __stdcall OnLbRecordRClick(AnyType userObj, Math::Coord2D<OSInt> scnPos, UI::GUIControl::MouseButton btn);
			void FreeNames();

		public:
			AVIRGISReplayForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core, NN<Map::GPSTrack> track, NN<AVIRMapNavigator> navi);
			virtual ~AVIRGISReplayForm();

			virtual void EventMenuClicked(UInt16 cmdId);
			virtual void OnMonitorChanged();

			void UpdateRecList();
			void StopThread();
		};
	}
}
#endif
