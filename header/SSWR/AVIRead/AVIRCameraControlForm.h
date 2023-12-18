#ifndef _SM_SSWR_AVIREAD_AVIRCAMERACONTROLFORM
#define _SM_SSWR_AVIREAD_AVIRCAMERACONTROLFORM
#include "IO/CameraControl.h"
#include "Media/ImageList.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIButton.h"
#include "UI/GUIForm.h"
#include "UI/GUILabel.h"
#include "UI/GUIListView.h"
#include "UI/GUIPanel.h"
#include "UI/GUIPictureBoxSimple.h"
#include "UI/GUITextBox.h"
#include "UI/GUIVSplitter.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRCameraControlForm : public UI::GUIForm
		{
		private:
			UI::GUIListView *lvInfo;
			UI::GUIVSplitter *vspInfo;
			UI::GUIListView *lvFiles;
			NotNullPtr<UI::GUIPanel> pnlControl;
			UI::GUIPictureBoxSimple *pbPreview;
			NotNullPtr<UI::GUIButton> btnDownload;

			NotNullPtr<SSWR::AVIRead::AVIRCore> core;
			IO::CameraControl *camera;
			Data::StringUTF8Map<Media::ImageList *> previewMap;

			static void __stdcall OnDownloadClicked(void *userObj);
			static void __stdcall OnFilesDblClick(void *userObj, UOSInt index);
			static void __stdcall OnFilesSelChg(void *userObj);
		public:
			AVIRCameraControlForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core, IO::CameraControl *camera);
			virtual ~AVIRCameraControlForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
