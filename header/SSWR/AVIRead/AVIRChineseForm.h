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
			NotNullPtr<SSWR::AVIRead::AVIRCore> core;
			Text::ChineseInfo *chinese;
			NotNullPtr<Media::DrawEngine> deng;
			UInt32 currChar;
			Media::DrawImage *charImg;
			NotNullPtr<Text::String> currFont;
			UInt32 currRadical;

			UI::GUIMainMenu *mnuMain;

			NotNullPtr<UI::GUILabel> lblCharCode;
			NotNullPtr<UI::GUITextBox> txtCharCode;
			NotNullPtr<UI::GUILabel> lblChar;
			NotNullPtr<UI::GUITextBox> txtChar;
			NotNullPtr<UI::GUIButton> btnCharPrev;
			NotNullPtr<UI::GUIButton> btnCharNext;
			UI::GUIPictureBoxSimple *pbChar;
			NotNullPtr<UI::GUILabel> lblRelatedCurr;
			NotNullPtr<UI::GUITextBox> txtRelatedCurr;
			NotNullPtr<UI::GUILabel> lblRelatedAdd;
			NotNullPtr<UI::GUITextBox> txtRelatedAdd;
			NotNullPtr<UI::GUIButton> btnRelatedGo;

			NotNullPtr<UI::GUIGroupBox> grpCharInfo;
			NotNullPtr<UI::GUILabel> lblRadical;
			NotNullPtr<UI::GUITextBox> txtRadical;
			NotNullPtr<UI::GUILabel> lblRadicalV;
			NotNullPtr<UI::GUILabel> lblStrokeCount;
			NotNullPtr<UI::GUITextBox> txtStrokeCount;
			NotNullPtr<UI::GUILabel> lblCharType;
			NotNullPtr<UI::GUIComboBox> cboCharType;
			NotNullPtr<UI::GUILabel> lblFlags;
			NotNullPtr<UI::GUICheckBox> chkMainChar;
			NotNullPtr<UI::GUILabel> lblPronun1;
			NotNullPtr<UI::GUITextBox> txtPronun1;
			NotNullPtr<UI::GUILabel> lblPronun2;
			NotNullPtr<UI::GUITextBox> txtPronun2;
			NotNullPtr<UI::GUILabel> lblPronun3;
			NotNullPtr<UI::GUITextBox> txtPronun3;
			NotNullPtr<UI::GUILabel> lblPronun4;
			NotNullPtr<UI::GUITextBox> txtPronun4;
			
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
			AVIRChineseForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRChineseForm();

			virtual void OnMonitorChanged();

			virtual void EventMenuClicked(UInt16 cmdId);
		};
	};
};
#endif
