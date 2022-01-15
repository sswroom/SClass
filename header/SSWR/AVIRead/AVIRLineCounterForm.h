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
				Text::String *fileName;
				UOSInt lineCnt;
			} FileInfo;
		private:
			UI::GUIPanel *pnlConfig;
			UI::GUILabel *lblPath;
			UI::GUITextBox *txtPath;
			UI::GUILabel *lblExtensions;
			UI::GUITextBox *txtExtensions;
			UI::GUIButton *btnExtensionsAdd;
			UI::GUIListBox *lbExtensions;
			UI::GUIButton *btnExtensionsRemove;
			UI::GUIButton *btnExtensionsClear;
			UI::GUIButton *btnCalc;
			UI::GUILabel *lblResult;
			UI::GUILabel *lblTotalLine;
			UI::GUITextBox *txtTotalLine;
			UI::GUIButton *btnResultSave;

			UI::GUIListView *lvResult;

			SSWR::AVIRead::AVIRCore *core;
			Data::ArrayList<Text::String *> *extList;
			Data::ArrayList<FileInfo *> *resList;

			static void __stdcall OnExtensionsAddClicked(void *userObj);
			static void __stdcall OnExtensionsRemoveClicked(void *userObj);
			static void __stdcall OnExtensionsClearClicked(void *userObj);
			static void __stdcall OnCalcClicked(void *userObj);
			static void __stdcall OnResultSaveClicked(void *userObj);

			void CalcDir(UTF8Char *pathBuff, UTF8Char *pathBuffEnd);
			void ClearExts();
			void ClearResult();
		public:
			AVIRLineCounterForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core);
			virtual ~AVIRLineCounterForm();

			virtual void OnMonitorChanged();
		};
	};
};
#endif
