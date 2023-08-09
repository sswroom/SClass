#ifndef _SM_SSWR_AVIREAD_AVIRLOGBACKUPFORM
#define _SM_SSWR_AVIREAD_AVIRLOGBACKUPFORM
#include "Data/ArrayListNN.h"
#include "UI/GUIButton.h"
#include "UI/GUIForm.h"
#include "UI/GUILabel.h"
#include "UI/GUITextBox.h"
#include "SSWR/AVIRead/AVIRCore.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRLogBackupForm : public UI::GUIForm
		{
		private:
			typedef struct
			{
				const UTF8Char *logName;
				Data::ArrayListNN<Text::String> *fileNames;
			} LogGroup;
		private:
			NotNullPtr<SSWR::AVIRead::AVIRCore> core;

			UI::GUILabel *lblLogDir;
			UI::GUITextBox *txtLogDir;
			UI::GUILabel *lblLogName;
			UI::GUITextBox *txtLogName;
			UI::GUIButton *btnStart;

			static void __stdcall OnStartClicked(void *userObj);
		public:
			AVIRLogBackupForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRLogBackupForm();

			virtual void OnMonitorChanged();
		};
	};
};
#endif
