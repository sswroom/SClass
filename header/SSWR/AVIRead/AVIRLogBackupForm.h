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
			NN<SSWR::AVIRead::AVIRCore> core;

			NN<UI::GUILabel> lblLogDir;
			NN<UI::GUITextBox> txtLogDir;
			NN<UI::GUILabel> lblLogName;
			NN<UI::GUITextBox> txtLogName;
			NN<UI::GUIButton> btnStart;

			static void __stdcall OnStartClicked(AnyType userObj);
		public:
			AVIRLogBackupForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRLogBackupForm();

			virtual void OnMonitorChanged();
		};
	};
};
#endif
