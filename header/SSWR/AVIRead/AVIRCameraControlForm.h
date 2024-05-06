#ifndef _SM_SSWR_AVIREAD_AVIRCAMERACONTROLFORM
#define _SM_SSWR_AVIREAD_AVIRCAMERACONTROLFORM
#include "Data/StringMapNN.h"
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
			NN<UI::GUIListView> lvInfo;
			NN<UI::GUIVSplitter> vspInfo;
			NN<UI::GUIListView> lvFiles;
			NN<UI::GUIPanel> pnlControl;
			NN<UI::GUIPictureBoxSimple> pbPreview;
			NN<UI::GUIButton> btnDownload;

			NN<SSWR::AVIRead::AVIRCore> core;
			IO::CameraControl *camera;
			Data::StringMapNN<Media::ImageList> previewMap;

			static void __stdcall OnDownloadClicked(AnyType userObj);
			static void __stdcall OnFilesDblClick(AnyType userObj, UOSInt index);
			static void __stdcall OnFilesSelChg(AnyType userObj);
		public:
			AVIRCameraControlForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core, IO::CameraControl *camera);
			virtual ~AVIRCameraControlForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
