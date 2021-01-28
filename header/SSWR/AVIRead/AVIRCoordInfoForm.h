#ifndef _SM_SSWR_AVIREAD_AVIRCOORDINFOFORM
#define _SM_SSWR_AVIREAD_AVIRCOORDINFOFORM
#include "Math/CoordinateSystemManager.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIButton.h"
#include "UI/GUIForm.h"
#include "UI/GUIHSplitter.h"
#include "UI/GUILabel.h"
#include "UI/GUIPanel.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRCoordInfoForm : public UI::GUIForm
		{
		private:
			UI::GUIPanel *pnlCoord;
			UI::GUILabel *lblSRID;
			UI::GUITextBox *txtSRID;
			UI::GUIButton *btnSRID;
			UI::GUIButton *btnSRIDPrev;
			UI::GUIButton *btnSRIDNext;
			UI::GUITextBox *txtWKT;
			UI::GUIHSplitter *hspWKT;
			UI::GUITextBox *txtDisp;

			SSWR::AVIRead::AVIRCore *core;

			static void __stdcall OnSRIDClicked(void *userObj);
			static void __stdcall OnSRIDPrevClicked(void *userObj);
			static void __stdcall OnSRIDNextClicked(void *userObj);

			void ShowInfo(const Math::CoordinateSystemManager::SpatialRefInfo *srinfo);
		public:
			AVIRCoordInfoForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core);
			virtual ~AVIRCoordInfoForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
