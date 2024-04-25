#ifndef _SM_SSWR_ORGANMGR_ORGANTRIPFORM
#define _SM_SSWR_ORGANMGR_ORGANTRIPFORM

#include "SSWR/OrganMgr/OrganEnv.h"
#include "UI/GUIButton.h"
#include "UI/GUIDateTimePicker.h"
#include "UI/GUIForm.h"
#include "UI/GUILabel.h"
#include "UI/GUIListBox.h"
#include "UI/GUIPanel.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace OrganMgr
	{
		class OrganTripForm : public UI::GUIForm
		{
		private:
			NN<UI::GUIPanel> pnlDetail;
			NN<UI::GUILabel> lblFrom;
			NN<UI::GUILabel> lblTo;
			NN<UI::GUILabel> lblLocation;
			NN<UI::GUIDateTimePicker> dtpFrom;
			NN<UI::GUIDateTimePicker> dtpTo;
			NN<UI::GUIButton> btnDate1Hr;
			NN<UI::GUITextBox> txtLocation;
			NN<UI::GUIButton> btnLocation;
			NN<UI::GUIButton> btnLocationLast;
			NN<UI::GUIButton> btnAdd;
			NN<UI::GUIListBox> lbTrips;
			NN<OrganEnv> env;
			Int32 locId;
			Bool updating;
			Data::Timestamp refTime;
			
			void UpdateList();
			static void __stdcall OnTripSelChg(AnyType userObj);
			static void __stdcall OnAddClicked(AnyType userObj);
			static void __stdcall OnLocationClicked(AnyType userObj);
			static void __stdcall OnDate1HrClicked(AnyType userObj);
			static void __stdcall OnLocationLastClicked(AnyType userObj);
		public:
			OrganTripForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<OrganEnv> env);
			~OrganTripForm();

			virtual void OnMonitorChanged();

			void SetTimes(const Data::Timestamp &refTime, const Data::Timestamp &fromTime, const Data::Timestamp &toTime);
		};
	}
}
#endif