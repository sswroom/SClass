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

			NotNullPtr<UI::GUITextBox> txtResult;

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

			static void __stdcall OnKeyMC(AnyType userObj);
			static void __stdcall OnKeyMR(AnyType userObj);
			static void __stdcall OnKeyMMinus(AnyType userObj);
			static void __stdcall OnKeyMPlus(AnyType userObj);
			static void __stdcall OnKeyDiv(AnyType userObj);

			static void __stdcall OnKeySqrt(AnyType userObj);
			static void __stdcall OnKey7(AnyType userObj);
			static void __stdcall OnKey8(AnyType userObj);
			static void __stdcall OnKey9(AnyType userObj);
			static void __stdcall OnKeyMul(AnyType userObj);

			static void __stdcall OnKeyPercent(AnyType userObj);
			static void __stdcall OnKey4(AnyType userObj);
			static void __stdcall OnKey5(AnyType userObj);
			static void __stdcall OnKey6(AnyType userObj);
			static void __stdcall OnKeyMinus(AnyType userObj);

			static void __stdcall OnKeyNeg(AnyType userObj);
			static void __stdcall OnKey1(AnyType userObj);
			static void __stdcall OnKey2(AnyType userObj);
			static void __stdcall OnKey3(AnyType userObj);
			static void __stdcall OnKeyPlus(AnyType userObj);

			static void __stdcall OnKeyCancel(AnyType userObj);
			static void __stdcall OnKey0(AnyType userObj);
			static void __stdcall OnKeyDot(AnyType userObj);
			static void __stdcall OnKeyEqual(AnyType userObj);
		public:
			AVIRCalculator1Form(Optional<UI::GUIClientControl> parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRCalculator1Form();

			virtual void EventMenuClicked(UInt16 cmdId);
			virtual void OnMonitorChanged();
		};
	};
};
#endif
