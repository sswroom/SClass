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
			NotNullPtr<UI::GUIPanel> pnlCoord;
			UI::GUILabel *lblSRID;
			UI::GUITextBox *txtSRID;
			NotNullPtr<UI::GUIButton> btnSRID;
			NotNullPtr<UI::GUIButton> btnSRIDPrev;
			NotNullPtr<UI::GUIButton> btnSRIDNext;
			UI::GUITextBox *txtWKT;
			NotNullPtr<UI::GUIHSplitter> hspWKT;
			UI::GUITextBox *txtDisp;

			NotNullPtr<SSWR::AVIRead::AVIRCore> core;

			static void __stdcall OnSRIDClicked(void *userObj);
			static void __stdcall OnSRIDPrevClicked(void *userObj);
			static void __stdcall OnSRIDNextClicked(void *userObj);

			void ShowInfo(const Math::CoordinateSystemManager::SpatialRefInfo *srinfo);
		public:
			AVIRCoordInfoForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRCoordInfoForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
