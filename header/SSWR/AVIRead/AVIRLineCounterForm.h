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
				NN<Text::String> fileName;
				UOSInt codeCnt;
				UOSInt nonEmpyCnt;
				UOSInt totalLineCnt;
			} FileInfo;
		private:
			NN<UI::GUIPanel> pnlConfig;
			NN<UI::GUILabel> lblPath;
			NN<UI::GUITextBox> txtPath;
			NN<UI::GUILabel> lblExtensions;
			NN<UI::GUITextBox> txtExtensions;
			NN<UI::GUIButton> btnExtensionsAdd;
			NN<UI::GUIListBox> lbExtensions;
			NN<UI::GUIButton> btnExtensionsRemove;
			NN<UI::GUIButton> btnExtensionsClear;
			NN<UI::GUIButton> btnCalc;
			NN<UI::GUILabel> lblResult;
			NN<UI::GUILabel> lblTotalLine;
			NN<UI::GUITextBox> txtTotalLine;
			NN<UI::GUILabel> lblNonEmpty;
			NN<UI::GUITextBox> txtNonEmpty;
			NN<UI::GUILabel> lblCode;
			NN<UI::GUITextBox> txtCode;
			NN<UI::GUIButton> btnResultSave;

			NN<UI::GUIListView> lvResult;

			NN<SSWR::AVIRead::AVIRCore> core;
			Data::ArrayListStringNN extList;
			Data::ArrayListNN<FileInfo> resList;

			static void __stdcall OnExtensionsAddClicked(AnyType userObj);
			static void __stdcall OnExtensionsRemoveClicked(AnyType userObj);
			static void __stdcall OnExtensionsClearClicked(AnyType userObj);
			static void __stdcall OnCalcClicked(AnyType userObj);
			static void __stdcall OnResultSaveClicked(AnyType userObj);

			void CalcDir(UnsafeArray<UTF8Char> pathBuff, UnsafeArray<UTF8Char> pathBuffEnd);
			void ClearExts(Bool inclDisp);
			void ClearResult(Bool inclDisp);
		public:
			AVIRLineCounterForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRLineCounterForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
