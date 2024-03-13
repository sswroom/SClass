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
				Data::ArrayListStringNN *fileNames;
			} LogGroup;
		private:
			NotNullPtr<SSWR::AVIRead::AVIRCore> core;

			NotNullPtr<UI::GUILabel> lblLogDir;
			NotNullPtr<UI::GUITextBox> txtLogDir;
			NotNullPtr<UI::GUILabel> lblLogName;
			NotNullPtr<UI::GUITextBox> txtLogName;
			NotNullPtr<UI::GUIButton> btnStart;

			static void __stdcall OnStartClicked(void *userObj);
		public:
			AVIRLogBackupForm(Optional<UI::GUIClientControl> parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRLogBackupForm();

			virtual void OnMonitorChanged();
		};
	};
};
#endif
