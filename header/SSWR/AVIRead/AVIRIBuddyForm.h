#ifndef _SM_SSWR_AVIREAD_AVIRIBUDDYFORM
#define _SM_SSWR_AVIREAD_AVIRIBUDDYFORM
#include "IO/Device/IBuddy.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIButton.h"
#include "UI/GUICheckBox.h"
#include "UI/GUIForm.h"
#include "UI/GUILabel.h"
#include "UI/GUIListBox.h"
#include "UI/GUIPanel.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRIBuddyForm : public UI::GUIForm
		{
		private:
			NN<SSWR::AVIRead::AVIRCore> core;
			Data::ArrayList<IO::Device::IBuddy*> *buddyList;
			IO::Device::IBuddy *currBuddy;
			IO::Device::IBuddy::IBuddyBodyEffect currTurn;
			IO::Device::IBuddy::IBuddyWingEffect currWing;
			IO::Device::IBuddy::IBuddyHeadEffect currHead;
			IO::Device::IBuddy::IBuddyHeartEffect currHeart;

			NN<UI::GUIListBox> lbDevice;
			NN<UI::GUIPanel> pnlMain;
			NN<UI::GUILabel> lblHead;
			NN<UI::GUIListBox> lbHead;
			NN<UI::GUILabel> lblHeart;
			NN<UI::GUICheckBox> chkHeart;
			NN<UI::GUILabel> lblTurn;
			NN<UI::GUIButton> btnTurnLeft;
			NN<UI::GUIButton> btnTurnRight;
			NN<UI::GUILabel> lblWing;
			NN<UI::GUIButton> btnWingUp;
			NN<UI::GUIButton> btnWingDown;

			static void __stdcall OnHeadChanged(AnyType userObj);
			static void __stdcall OnHeartChanged(AnyType userObj, Bool newState);
			static void __stdcall OnTurnLeftDown(AnyType userObj, Bool isDown);
			static void __stdcall OnTurnRightDown(AnyType userObj, Bool isDown);
			static void __stdcall OnWingUpDown(AnyType userObj, Bool isDown);
			static void __stdcall OnWingDownDown(AnyType userObj, Bool isDown);
			static void __stdcall OnDevChanged(AnyType userObj);
		public:
			AVIRIBuddyForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRIBuddyForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
