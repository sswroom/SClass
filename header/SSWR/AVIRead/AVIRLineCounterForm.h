#ifndef _SM_SSWR_AVIREAD_AVIREXEFORM
#define _SM_SSWR_AVIREAD_AVIREXEFORM
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIButton.h"
#include "UI/GUIForm.h"
#include "UI/GUILabel.h"
#include "UI/GUIListBox.h"
#include "UI/GUIListView.h"
#include "UI/GUIPanel.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRLineCounterForm : public UI::GUIForm
		{
		private:
			typedef struct
			{
				NotNullPtr<Text::String> fileName;
				UOSInt codeCnt;
				UOSInt nonEmpyCnt;
				UOSInt totalLineCnt;
			} FileInfo;
		private:
			NotNullPtr<UI::GUIPanel> pnlConfig;
			NotNullPtr<UI::GUILabel> lblPath;
			NotNullPtr<UI::GUITextBox> txtPath;
			NotNullPtr<UI::GUILabel> lblExtensions;
			NotNullPtr<UI::GUITextBox> txtExtensions;
			NotNullPtr<UI::GUIButton> btnExtensionsAdd;
			NotNullPtr<UI::GUIListBox> lbExtensions;
			NotNullPtr<UI::GUIButton> btnExtensionsRemove;
			NotNullPtr<UI::GUIButton> btnExtensionsClear;
			NotNullPtr<UI::GUIButton> btnCalc;
			NotNullPtr<UI::GUILabel> lblResult;
			NotNullPtr<UI::GUILabel> lblTotalLine;
			NotNullPtr<UI::GUITextBox> txtTotalLine;
			NotNullPtr<UI::GUILabel> lblNonEmpty;
			NotNullPtr<UI::GUITextBox> txtNonEmpty;
			NotNullPtr<UI::GUILabel> lblCode;
			NotNullPtr<UI::GUITextBox> txtCode;
			NotNullPtr<UI::GUIButton> btnResultSave;

			NotNullPtr<UI::GUIListView> lvResult;

			NotNullPtr<SSWR::AVIRead::AVIRCore> core;
			Data::ArrayListStringNN extList;
			Data::ArrayList<FileInfo *> resList;

			static void __stdcall OnExtensionsAddClicked(void *userObj);
			static void __stdcall OnExtensionsRemoveClicked(void *userObj);
			static void __stdcall OnExtensionsClearClicked(void *userObj);
			static void __stdcall OnCalcClicked(void *userObj);
			static void __stdcall OnResultSaveClicked(void *userObj);

			void CalcDir(UTF8Char *pathBuff, UTF8Char *pathBuffEnd);
			void ClearExts(Bool inclDisp);
			void ClearResult(Bool inclDisp);
		public:
			AVIRLineCounterForm(Optional<UI::GUIClientControl> parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRLineCounterForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
