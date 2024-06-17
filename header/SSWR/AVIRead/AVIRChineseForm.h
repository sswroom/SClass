#ifndef _SM_SSWR_AVIREAD_AVIRCHINESEFORM
#define _SM_SSWR_AVIREAD_AVIRCHINESEFORM
#include "SSWR/AVIRead/AVIRCore.h"
#include "Text/ChineseInfo.h"
#include "UI/GUIButton.h"
#include "UI/GUICheckBox.h"
#include "UI/GUIComboBox.h"
#include "UI/GUIForm.h"
#include "UI/GUILabel.h"
#include "UI/GUIPanel.h"
#include "UI/GUIPictureBoxSimple.h"
#include "UI/GUITabControl.h"
#include "UI/GUITabPage.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRChineseForm : public UI::GUIForm
		{
		private:
			NN<SSWR::AVIRead::AVIRCore> core;
			Text::ChineseInfo *chinese;
			NN<Media::DrawEngine> deng;
			UInt32 currChar;
			Optional<Media::DrawImage> charImg;
			NN<Text::String> currFont;
			UInt32 currRadical;

			NN<UI::GUIMainMenu> mnuMain;

			NN<UI::GUILabel> lblCharCode;
			NN<UI::GUITextBox> txtCharCode;
			NN<UI::GUIButton> btnPasteCharCode;
			NN<UI::GUILabel> lblChar;
			NN<UI::GUITextBox> txtChar;
			NN<UI::GUIButton> btnCharPrev;
			NN<UI::GUIButton> btnCharNext;
			NN<UI::GUIPictureBoxSimple> pbChar;
			NN<UI::GUILabel> lblRelatedCurr;
			NN<UI::GUITextBox> txtRelatedCurr;
			NN<UI::GUILabel> lblRelatedAdd;
			NN<UI::GUITextBox> txtRelatedAdd;
			NN<UI::GUIButton> btnRelatedGo;
			NN<UI::GUITabControl> tcMain;

			NN<UI::GUITabPage> tpBaseInfo;
			NN<UI::GUILabel> lblCurrChar;
			NN<UI::GUITextBox> txtCurrChar;
			NN<UI::GUILabel> lblUTF8Code;
			NN<UI::GUITextBox> txtUTF8Code;
			NN<UI::GUILabel> lblUTF16Code;
			NN<UI::GUITextBox> txtUTF16Code;
			NN<UI::GUILabel> lblUTF32Code;
			NN<UI::GUITextBox> txtUTF32Code;
			NN<UI::GUILabel> lblBlockRange;
			NN<UI::GUITextBox> txtBlockRange;
			NN<UI::GUILabel> lblBlockName;
			NN<UI::GUITextBox> txtBlockName;

			NN<UI::GUITabPage> tpCharInfo;
			NN<UI::GUILabel> lblRadical;
			NN<UI::GUITextBox> txtRadical;
			NN<UI::GUILabel> lblRadicalV;
			NN<UI::GUILabel> lblStrokeCount;
			NN<UI::GUITextBox> txtStrokeCount;
			NN<UI::GUILabel> lblCharType;
			NN<UI::GUIComboBox> cboCharType;
			NN<UI::GUILabel> lblFlags;
			NN<UI::GUICheckBox> chkMainChar;
			NN<UI::GUILabel> lblPronun1;
			NN<UI::GUITextBox> txtPronun1;
			NN<UI::GUILabel> lblPronun2;
			NN<UI::GUITextBox> txtPronun2;
			NN<UI::GUILabel> lblPronun3;
			NN<UI::GUITextBox> txtPronun3;
			NN<UI::GUILabel> lblPronun4;
			NN<UI::GUITextBox> txtPronun4;
			
			static void __stdcall OnCharChg(AnyType userObj);
			static Bool __stdcall OnCharMouseDown(AnyType userObj, Math::Coord2D<OSInt> scnPos, UI::GUIControl::MouseButton btn);
			static void __stdcall OnPasteCharCodeClicked(AnyType userObj);
			static void __stdcall OnCharPrevClicked(AnyType userObj);
			static void __stdcall OnCharNextClicked(AnyType userObj);
			static void __stdcall OnRadicalChg(AnyType userObj);
			static void __stdcall OnRelatedAddChg(AnyType userObj);
			static void __stdcall OnRelatedGoClicked(AnyType userObj);
			static void __stdcall OnFormClosed(AnyType userObj, NN<UI::GUIForm> frm);

			Bool SaveChar();
			void UpdateChar(UInt32 charCode);
			void UpdateImg();
			void UpdateRelation();
		public:
			AVIRChineseForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRChineseForm();

			virtual void OnMonitorChanged();

			virtual void EventMenuClicked(UInt16 cmdId);
		};
	};
};
#endif
