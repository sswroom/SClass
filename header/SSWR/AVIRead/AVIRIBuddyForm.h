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
			NotNullPtr<SSWR::AVIRead::AVIRCore> core;
			Data::ArrayList<IO::Device::IBuddy*> *buddyList;
			IO::Device::IBuddy *currBuddy;
			IO::Device::IBuddy::IBuddyBodyEffect currTurn;
			IO::Device::IBuddy::IBuddyWingEffect currWing;
			IO::Device::IBuddy::IBuddyHeadEffect currHead;
			IO::Device::IBuddy::IBuddyHeartEffect currHeart;

			NotNullPtr<UI::GUIListBox> lbDevice;
			NotNullPtr<UI::GUIPanel> pnlMain;
			NotNullPtr<UI::GUILabel> lblHead;
			NotNullPtr<UI::GUIListBox> lbHead;
			NotNullPtr<UI::GUILabel> lblHeart;
			NotNullPtr<UI::GUICheckBox> chkHeart;
			NotNullPtr<UI::GUILabel> lblTurn;
			NotNullPtr<UI::GUIButton> btnTurnLeft;
			NotNullPtr<UI::GUIButton> btnTurnRight;
			NotNullPtr<UI::GUILabel> lblWing;
			NotNullPtr<UI::GUIButton> btnWingUp;
			NotNullPtr<UI::GUIButton> btnWingDown;

			static void __stdcall OnHeadChanged(void *userObj);
			static void __stdcall OnHeartChanged(void *userObj, Bool newState);
			static void __stdcall OnTurnLeftDown(void *userObj, Bool isDown);
			static void __stdcall OnTurnRightDown(void *userObj, Bool isDown);
			static void __stdcall OnWingUpDown(void *userObj, Bool isDown);
			static void __stdcall OnWingDownDown(void *userObj, Bool isDown);
			static void __stdcall OnDevChanged(void *userObj);
		public:
			AVIRIBuddyForm(Optional<UI::GUIClientControl> parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRIBuddyForm();

			virtual void OnMonitorChanged();
		};
	};
};
#endif
