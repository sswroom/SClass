#ifndef _SM_SSWR_AVIREAD_AVIRIMAGEUPLOADERFORM
#define _SM_SSWR_AVIREAD_AVIRIMAGEUPLOADERFORM
#include "Data/ArrayList.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIButton.h"
#include "UI/GUICheckBox.h"
#include "UI/GUIForm.h"
#include "UI/GUILabel.h"
#include "UI/GUIListView.h"
#include "UI/GUIPanel.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRImageUploaderForm : public UI::GUIForm
		{
		private:
			enum class FileStatus
			{
				Pending,
				Success,
				Failed
			};

			struct FileItem
			{
			Text::String *fileName;
			UInt64 fileSize;
			FileStatus status;
			};
		private:
			SSWR::AVIRead::AVIRCore *core;
			Data::ArrayList<FileItem *> *items;

			UI::GUIPanel *pnlCtrl;
			UI::GUILabel *lblUsername;
			UI::GUITextBox *txtUsername;
			UI::GUILabel *lblPassword;
			UI::GUITextBox *txtPassword;
			UI::GUICheckBox *chkErrorCont;
			UI::GUIButton *btnUpload;
			UI::GUIListView *lvStatus;

			static void __stdcall OnFileDrop(void *userObj, const UTF8Char **files, UOSInt fileCnt);
			static void __stdcall OnUploadClicked(void *userObj);
			static void FreeItem(FileItem *item);
		public:
			AVIRImageUploaderForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, SSWR::AVIRead::AVIRCore *core);
			virtual ~AVIRImageUploaderForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
