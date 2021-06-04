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
			SSWR::AVIRead::AVIRCore *core;
			Data::ArrayList<TestResult*> *resultList;
			Bool processing;

			UI::GUIPanel *pnlCtrl;
			UI::GUIButton *btnStart;
			UI::GUIButton *btnQuick;
			UI::GUIButton *btnSave;
			UI::GUIProgressBar *pbMain;
			UI::GUITabControl *tcMain;

			UI::GUITabPage *tpCopy;
			UI::GUIListView *lvCopy;

			UI::GUITabPage *tpRead;
			UI::GUIListView *lvRead;

			UI::GUITabPage *tpWrite;
			UI::GUIListView *lvWrite;

			UI::GUITabPage *tpInfo;
			UI::GUIPanel *pnlInfo;
			UI::GUILabel *lblPlatform;
			UI::GUITextBox *txtPlatform;
			UI::GUILabel *lblCPU;
			UI::GUITextBox *txtCPU;
			UI::GUIListView *lvRAM;

			static UTF8Char *ByteDisp(UTF8Char *sbuff, UOSInt byteSize);
			void StartTest(UOSInt startSize, UOSInt buffSize);
			void ClearResult();
			static void __stdcall OnStartClicked(void *userObj);
			static void __stdcall OnQuickClicked(void *userObj);
			static void __stdcall OnSaveClicked(void *userObj);
		public:
			AVIRBenchmarkForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core);
			virtual ~AVIRBenchmarkForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
