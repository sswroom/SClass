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
			NN<SSWR::AVIRead::AVIRCore> core;

			NN<UI::GUITextBox> txtResult;

			NN<UI::GUIButton> btnKeyMC;
			NN<UI::GUIButton> btnKeyMR;
			NN<UI::GUIButton> btnKeyMMinus;
			NN<UI::GUIButton> btnKeyMPlus;
			NN<UI::GUIButton> btnKeyDiv;

			NN<UI::GUIButton> btnKeySqrt;
			NN<UI::GUIButton> btnKey7;
			NN<UI::GUIButton> btnKey8;
			NN<UI::GUIButton> btnKey9;
			NN<UI::GUIButton> btnKeyMul;

			NN<UI::GUIButton> btnKeyPercent;
			NN<UI::GUIButton> btnKey4;
			NN<UI::GUIButton> btnKey5;
			NN<UI::GUIButton> btnKey6;
			NN<UI::GUIButton> btnKeyMinus;

			NN<UI::GUIButton> btnKeyNeg;
			NN<UI::GUIButton> btnKey1;
			NN<UI::GUIButton> btnKey2;
			NN<UI::GUIButton> btnKey3;
			NN<UI::GUIButton> btnKeyPlus;

			NN<UI::GUIButton> btnKeyCancel;
			NN<UI::GUIButton> btnKey0;
			NN<UI::GUIButton> btnKeyDot;
			NN<UI::GUIButton> btnKeyEqual;

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
			AVIRCalculator1Form(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRCalculator1Form();

			virtual void EventMenuClicked(UInt16 cmdId);
			virtual void OnMonitorChanged();
		};
	};
};
#endif
