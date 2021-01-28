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
			SSWR::AVIRead::AVIRCore *core;

			UI::GUITextBox *txtResult;

			UI::GUIButton *btnKeyMC;
			UI::GUIButton *btnKeyMR;
			UI::GUIButton *btnKeyMMinus;
			UI::GUIButton *btnKeyMPlus;
			UI::GUIButton *btnKeyDiv;

			UI::GUIButton *btnKeySqrt;
			UI::GUIButton *btnKey7;
			UI::GUIButton *btnKey8;
			UI::GUIButton *btnKey9;
			UI::GUIButton *btnKeyMul;

			UI::GUIButton *btnKeyPercent;
			UI::GUIButton *btnKey4;
			UI::GUIButton *btnKey5;
			UI::GUIButton *btnKey6;
			UI::GUIButton *btnKeyMinus;

			UI::GUIButton *btnKeyNeg;
			UI::GUIButton *btnKey1;
			UI::GUIButton *btnKey2;
			UI::GUIButton *btnKey3;
			UI::GUIButton *btnKeyPlus;

			UI::GUIButton *btnKeyCancel;
			UI::GUIButton *btnKey0;
			UI::GUIButton *btnKeyDot;
			UI::GUIButton *btnKeyEqual;

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
			AVIRCalculator1Form(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core);
			virtual ~AVIRCalculator1Form();

			virtual void EventMenuClicked(UInt16 cmdId);
			virtual void OnMonitorChanged();
		};
	};
};
#endif
