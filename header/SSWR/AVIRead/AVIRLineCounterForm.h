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
				UOSInt lineCnt;
			} FileInfo;
		private:
			NotNullPtr<UI::GUIPanel> pnlConfig;
			NotNullPtr<UI::GUILabel> lblPath;
			UI::GUITextBox *txtPath;
			NotNullPtr<UI::GUILabel> lblExtensions;
			UI::GUITextBox *txtExtensions;
			NotNullPtr<UI::GUIButton> btnExtensionsAdd;
			UI::GUIListBox *lbExtensions;
			NotNullPtr<UI::GUIButton> btnExtensionsRemove;
			NotNullPtr<UI::GUIButton> btnExtensionsClear;
			NotNullPtr<UI::GUIButton> btnCalc;
			NotNullPtr<UI::GUILabel> lblResult;
			NotNullPtr<UI::GUILabel> lblTotalLine;
			UI::GUITextBox *txtTotalLine;
			NotNullPtr<UI::GUIButton> btnResultSave;

			UI::GUIListView *lvResult;

			NotNullPtr<SSWR::AVIRead::AVIRCore> core;
			Data::ArrayListStringNN extList;
			Data::ArrayList<FileInfo *> resList;

			static void __stdcall OnExtensionsAddClicked(void *userObj);
			static void __stdcall OnExtensionsRemoveClicked(void *userObj);
			static void __stdcall OnExtensionsClearClicked(void *userObj);
			static void __stdcall OnCalcClicked(void *userObj);
			static void __stdcall OnResultSaveClicked(void *userObj);

			void CalcDir(UTF8Char *pathBuff, UTF8Char *pathBuffEnd);
			void ClearExts();
			void ClearResult();
		public:
			AVIRLineCounterForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRLineCounterForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
