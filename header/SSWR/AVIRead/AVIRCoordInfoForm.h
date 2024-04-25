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
			NN<UI::GUIPanel> pnlCoord;
			NN<UI::GUILabel> lblSRID;
			NN<UI::GUITextBox> txtSRID;
			NN<UI::GUIButton> btnSRID;
			NN<UI::GUIButton> btnSRIDPrev;
			NN<UI::GUIButton> btnSRIDNext;
			NN<UI::GUITextBox> txtWKT;
			NN<UI::GUIHSplitter> hspWKT;
			NN<UI::GUITextBox> txtDisp;

			NN<SSWR::AVIRead::AVIRCore> core;

			static void __stdcall OnSRIDClicked(AnyType userObj);
			static void __stdcall OnSRIDPrevClicked(AnyType userObj);
			static void __stdcall OnSRIDNextClicked(AnyType userObj);

			void ShowInfo(const Math::CoordinateSystemManager::SpatialRefInfo *srinfo);
		public:
			AVIRCoordInfoForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRCoordInfoForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
