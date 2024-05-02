#ifndef _SM_SSWR_AVIREAD_AVIRBENCHMARKFORM
#define _SM_SSWR_AVIREAD_AVIRBENCHMARKFORM
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIButton.h"
#include "UI/GUIComboBox.h"
#include "UI/GUIForm.h"
#include "UI/GUILabel.h"
#include "UI/GUIListView.h"
#include "UI/GUIPanel.h"
#include "UI/GUIProgressBar.h"
#include "UI/GUITabControl.h"
#include "UI/GUITabPage.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRBenchmarkForm : public UI::GUIForm
		{
		private:
			typedef enum
			{
				TT_COPY = 1,
				TT_WRITE,
				TT_READ
			} TestType;

			typedef struct
			{
				TestType tt;
				UOSInt testSize;
				Double resultRate;
			} TestResult;
		private:
			NN<SSWR::AVIRead::AVIRCore> core;
			Data::ArrayListNN<TestResult> resultList;
			Bool processing;

			NN<UI::GUIPanel> pnlCtrl;
			NN<UI::GUIButton> btnStart;
			NN<UI::GUIButton> btnQuick;
			NN<UI::GUIButton> btnSave;
			NN<UI::GUIProgressBar> pbMain;
			NN<UI::GUITabControl> tcMain;

			NN<UI::GUITabPage> tpCopy;
			NN<UI::GUIListView> lvCopy;

			NN<UI::GUITabPage> tpRead;
			NN<UI::GUIListView> lvRead;

			NN<UI::GUITabPage> tpWrite;
			NN<UI::GUIListView> lvWrite;

			NN<UI::GUITabPage> tpInfo;
			NN<UI::GUIPanel> pnlInfo;
			NN<UI::GUILabel> lblPlatform;
			NN<UI::GUITextBox> txtPlatform;
			NN<UI::GUILabel> lblCPU;
			NN<UI::GUITextBox> txtCPU;
			NN<UI::GUIListView> lvRAM;

			static UTF8Char *ByteDisp(UTF8Char *sbuff, UOSInt byteSize);
			void StartTest(UOSInt startSize, UOSInt buffSize);
			void ClearResult();
			static void __stdcall OnStartClicked(AnyType userObj);
			static void __stdcall OnQuickClicked(AnyType userObj);
			static void __stdcall OnSaveClicked(AnyType userObj);
		public:
			AVIRBenchmarkForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRBenchmarkForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
