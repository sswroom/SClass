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
			NotNullPtr<UI::GUILabel> lblSRID;
			NotNullPtr<UI::GUITextBox> txtSRID;
			NotNullPtr<UI::GUIButton> btnSRID;
			NotNullPtr<UI::GUIButton> btnSRIDPrev;
			NotNullPtr<UI::GUIButton> btnSRIDNext;
			NotNullPtr<UI::GUITextBox> txtWKT;
			NotNullPtr<UI::GUIHSplitter> hspWKT;
			NotNullPtr<UI::GUITextBox> txtDisp;

			NotNullPtr<SSWR::AVIRead::AVIRCore> core;

			static void __stdcall OnSRIDClicked(AnyType userObj);
			static void __stdcall OnSRIDPrevClicked(AnyType userObj);
			static void __stdcall OnSRIDNextClicked(AnyType userObj);

			void ShowInfo(const Math::CoordinateSystemManager::SpatialRefInfo *srinfo);
		public:
			AVIRCoordInfoForm(Optional<UI::GUIClientControl> parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRCoordInfoForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
