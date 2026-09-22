#ifndef _SM_SSWR_AVIREAD_AVIRMAP3DFORM
#define _SM_SSWR_AVIREAD_AVIRMAP3DFORM
#include "Map/MapEnv.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIForm.h"
#include "UI/GUILabel.h"
#include "UI/GUIPanel.h"
#include "UI/GUITextBox.h"
#include "UI/GUITrackBar.h"
#include "UI/GUIMap3DControl.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRMap3DForm : public UI::GUIForm
		{
		private:
			NN<SSWR::AVIRead::AVIRCore> core;
			NN<Map::MapEnv> env;
			Bool releaseEnv;
			NN<Media::ColorManagerSess> colorSess;
			NN<UI::GUIPanel> pnlCtrl;
			NN<UI::GUIMap3DControl> mapCtrl;
			NN<UI::GUITextBox> txtCenter;
			NN<UI::GUILabel> lblVAngle;
			NN<UI::GUITrackBar> tbVAngle;
			NN<UI::GUILabel> lblHAngle;
			NN<UI::GUITrackBar> tbHAngle;

		private:
			static void __stdcall OnMapUpdated(AnyType userObj, Math::Coord2DDbl center);
			static void __stdcall OnVAngleScrolled(AnyType userObj, UIntOS newVal);
			static void __stdcall OnHAngleScrolled(AnyType userObj, UIntOS newVal);
			void UpdateCenterText(Math::Coord2DDbl center);
		public:
			AVIRMap3DForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core, NN<Map::MapEnv> env, Bool releaseEnv);
			virtual ~AVIRMap3DForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif