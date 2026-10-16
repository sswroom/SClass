#ifndef _SM_SSWR_AVIREAD_AVIRMODEL3DVIEWERFORM
#define _SM_SSWR_AVIREAD_AVIRMODEL3DVIEWERFORM
#include "Model/Model3D.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUICustomDraw.h"
#include "UI/GUIForm.h"
#include "UI/GUILabel.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRModel3DViewerForm : public UI::GUIForm
		{
		private:
			class ModelControl;

			NN<SSWR::AVIRead::AVIRCore> core;
			NN<Model::Model3D> model;
			NN<Media::ColorManagerSess> colorSess;
			NN<UI::GUILabel> lblInfo;
			NN<ModelControl> modelCtrl;

		public:
			AVIRModel3DViewerForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core, NN<Model::Model3D> model);
			virtual ~AVIRModel3DViewerForm();

			virtual void OnMonitorChanged();
		};
	}
}

#endif