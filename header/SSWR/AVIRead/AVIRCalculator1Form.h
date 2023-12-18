#ifndef _SM_SSWR_AVIREAD_AVIRCALCULATOR1FORM
#define _SM_SSWR_AVIREAD_AVIRCALCULATOR1FORM
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIButton.h"
#include "UI/GUIForm.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRCalculator1Form : public UI::GUIForm
		{
		private:
			NotNullPtr<SSWR::AVIRead::AVIRCore> core;

			UI::GUITextBox *txtResult;

			NotNullPtr<UI::GUIButton> btnKeyMC;
			NotNullPtr<UI::GUIButton> btnKeyMR;
			NotNullPtr<UI::GUIButton> btnKeyMMinus;
			NotNullPtr<UI::GUIButton> btnKeyMPlus;
			NotNullPtr<UI::GUIButton> btnKeyDiv;

			NotNullPtr<UI::GUIButton> btnKeySqrt;
			NotNullPtr<UI::GUIButton> btnKey7;
			NotNullPtr<UI::GUIButton> btnKey8;
			NotNullPtr<UI::GUIButton> btnKey9;
			NotNullPtr<UI::GUIButton> btnKeyMul;

			NotNullPtr<UI::GUIButton> btnKeyPercent;
			NotNullPtr<UI::GUIButton> btnKey4;
			NotNullPtr<UI::GUIButton> btnKey5;
			NotNullPtr<UI::GUIButton> btnKey6;
			NotNullPtr<UI::GUIButton> btnKeyMinus;

			NotNullPtr<UI::GUIButton> btnKeyNeg;
			NotNullPtr<UI::GUIButton> btnKey1;
			NotNullPtr<UI::GUIButton> btnKey2;
			NotNullPtr<UI::GUIButton> btnKey3;
			NotNullPtr<UI::GUIButton> btnKeyPlus;

			NotNullPtr<UI::GUIButton> btnKeyCancel;
			NotNullPtr<UI::GUIButton> btnKey0;
			NotNullPtr<UI::GUIButton> btnKeyDot;
			NotNullPtr<UI::GUIButton> btnKeyEqual;

			static void __stdcall OnKeyMC(void *userObj);
			static void __stdcall OnKeyMR(void *userObj);
			static void __stdcall OnKeyMMinus(void *userObj);
			static void __stdcall OnKeyMPlus(void *userObj);
			static void __stdcall OnKeyDiv(void *userObj);

			static void __stdcall OnKeySqrt(void *userObj);
			static void __stdcall OnKey7(void *userObj);
			static void __stdcall OnKey8(void *userObj);
			static void __stdcall OnKey9(void *userObj);
			static void __stdcall OnKeyMul(void *userObj);

			static void __stdcall OnKeyPercent(void *userObj);
			static void __stdcall OnKey4(void *userObj);
			static void __stdcall OnKey5(void *userObj);
			static void __stdcall OnKey6(void *userObj);
			static void __stdcall OnKeyMinus(void *userObj);

			static void __stdcall OnKeyNeg(void *userObj);
			static void __stdcall OnKey1(void *userObj);
			static void __stdcall OnKey2(void *userObj);
			static void __stdcall OnKey3(void *userObj);
			static void __stdcall OnKeyPlus(void *userObj);

			static void __stdcall OnKeyCancel(void *userObj);
			static void __stdcall OnKey0(void *userObj);
			static void __stdcall OnKeyDot(void *userObj);
			static void __stdcall OnKeyEqual(void *userObj);
		public:
			AVIRCalculator1Form(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRCalculator1Form();

			virtual void EventMenuClicked(UInt16 cmdId);
			virtual void OnMonitorChanged();
		};
	};
};
#endif
