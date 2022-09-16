#ifndef _SM_SSWR_AVIREAD_AVIRCHINESEFORM
#define _SM_SSWR_AVIREAD_AVIRCHINESEFORM
#include "SSWR/AVIRead/AVIRCore.h"
#include "Text/ChineseInfo.h"
#include "UI/GUIButton.h"
#include "UI/GUICheckBox.h"
#include "UI/GUIComboBox.h"
#include "UI/GUIForm.h"
#include "UI/GUIGroupBox.h"
#include "UI/GUILabel.h"
#include "UI/GUIPanel.h"
#include "UI/GUIPictureBoxSimple.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRChineseForm : public UI::GUIForm
		{
		private:
			SSWR::AVIRead::AVIRCore *core;
			Text::ChineseInfo *chinese;
			Media::DrawEngine *deng;
			UInt32 currChar;
			Media::DrawImage *charImg;
			Text::String *currFont;
			UInt32 currRadical;

			UI::GUIMainMenu *mnuMain;

			UI::GUILabel *lblCharCode;
			UI::GUITextBox *txtCharCode;
			UI::GUILabel *lblChar;
			UI::GUITextBox *txtChar;
			UI::GUIButton *btnCharPrev;
			UI::GUIButton *btnCharNext;
			UI::GUIPictureBoxSimple *pbChar;
			UI::GUILabel *lblRelatedCurr;
			UI::GUITextBox *txtRelatedCurr;
			UI::GUILabel *lblRelatedAdd;
			UI::GUITextBox *txtRelatedAdd;
			UI::GUIButton *btnRelatedGo;

			UI::GUIGroupBox *grpCharInfo;
			UI::GUILabel *lblRadical;
			UI::GUITextBox *txtRadical;
			UI::GUILabel *lblRadicalV;
			UI::GUILabel *lblStrokeCount;
			UI::GUITextBox *txtStrokeCount;
			UI::GUILabel *lblCharType;
			UI::GUIComboBox *cboCharType;
			UI::GUILabel *lblFlags;
			UI::GUICheckBox *chkMainChar;
			UI::GUILabel *lblPronun1;
			UI::GUITextBox *txtPronun1;
			UI::GUILabel *lblPronun2;
			UI::GUITextBox *txtPronun2;
			UI::GUILabel *lblPronun3;
			UI::GUITextBox *txtPronun3;
			UI::GUILabel *lblPronun4;
			UI::GUITextBox *txtPronun4;
			
			static void __stdcall OnCharChg(void *userObj);
			static Bool __stdcall OnCharMouseDown(void *userObj, Math::Coord2D<OSInt> scnPos, UI::GUIControl::MouseButton btn);
			static void __stdcall OnCharPrevClicked(void *userObj);
			static void __stdcall OnCharNextClicked(void *userObj);
			static void __stdcall OnRadicalChg(void *userObj);
			static void __stdcall OnRelatedAddChg(void *userObj);
			static void __stdcall OnRelatedGoClicked(void *userObj);
			static void __stdcall OnFormClosed(void *userObj, UI::GUIForm *frm);

			Bool SaveChar();
			void UpdateChar(UInt32 charCode);
			void UpdateImg();
			void UpdateRelation();
		public:
			AVIRChineseForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core);
			virtual ~AVIRChineseForm();

			virtual void OnMonitorChanged();

			virtual void EventMenuClicked(UInt16 cmdId);
		};
	};
};
#endif
