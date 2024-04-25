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
				NN<Text::String> fileName;
				UInt64 fileSize;
				FileStatus status;
			};
		private:
			NN<SSWR::AVIRead::AVIRCore> core;
			Data::ArrayList<FileItem *> *items;

			NN<UI::GUIPanel> pnlCtrl;
			NN<UI::GUILabel> lblUsername;
			NN<UI::GUITextBox> txtUsername;
			NN<UI::GUILabel> lblPassword;
			NN<UI::GUITextBox> txtPassword;
			NN<UI::GUICheckBox> chkErrorCont;
			NN<UI::GUIButton> btnUpload;
			NN<UI::GUIListView> lvStatus;

			static void __stdcall OnFileDrop(AnyType userObj, Data::DataArray<NN<Text::String>> files);
			static void __stdcall OnUploadClicked(AnyType userObj);
			static void FreeItem(FileItem *item);
		public:
			AVIRImageUploaderForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRImageUploaderForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
