#ifndef _SM_SSWR_AVIREAD_AVIRGISEXPORTTEMPLATEFORM
#define _SM_SSWR_AVIREAD_AVIRGISEXPORTTEMPLATEFORM
#include "Map/MapEnv.h"
#include "Media/SVGDocument.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIButton.h"
#include "UI/GUIComboBox.h"
#include "UI/GUIForm.h"
#include "UI/GUILabel.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRGISExportTemplateForm : public UI::GUIForm
		{
		private:
			NN<UI::GUILabel> lblCenterX;
			NN<UI::GUITextBox> txtCenterX;
			NN<UI::GUILabel> lblCenterY;
			NN<UI::GUITextBox> txtCenterY;
			NN<UI::GUILabel> lblScale;
			NN<UI::GUITextBox> txtScale;
			NN<UI::GUILabel> lblTemplate;
			NN<UI::GUITextBox> txtTemplate;
			NN<UI::GUILabel> lblId;
			NN<UI::GUIComboBox> cboId;
			NN<UI::GUILabel> lblDimension;
			NN<UI::GUITextBox> txtDimensionW;
			NN<UI::GUITextBox> txtDimensionH;
			NN<UI::GUIButton> btnExport;

			NN<SSWR::AVIRead::AVIRCore> core;
			Optional<Media::SVGDocument> doc;
			NN<Map::MapEnv> env;
			Math::Coord2DDbl center;
			Double scale;

			static void __stdcall OnFiles(AnyType userObj, Data::DataArray<NN<Text::String>> fileNames);
			static void __stdcall OnExportClicked(AnyType userObj);
		public:
			AVIRGISExportTemplateForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core, NN<Map::MapEnv> env, Math::Coord2DDbl center, Double scale);
			virtual ~AVIRGISExportTemplateForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
