#ifndef _SM_SSWR_AVIREAD_AVIRGISEXPORTIMAGEFORM
#define _SM_SSWR_AVIREAD_AVIRGISEXPORTIMAGEFORM
#include "Map/MapEnv.h"
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
		class AVIRGISExportImageForm : public UI::GUIForm
		{
		private:
			NN<UI::GUILabel> lblCenterX;
			NN<UI::GUITextBox> txtCenterX;
			NN<UI::GUILabel> lblCenterY;
			NN<UI::GUITextBox> txtCenterY;
			NN<UI::GUILabel> lblScale;
			NN<UI::GUITextBox> txtScale;
			NN<UI::GUILabel> lblDimension;
			NN<UI::GUITextBox> txtDimensionW;
			NN<UI::GUITextBox> txtDimensionH;
			NN<UI::GUIComboBox> cboDimensionUnit;
			NN<UI::GUILabel> lblDPI;
			NN<UI::GUITextBox> txtDPI;
			NN<UI::GUIButton> btnExport;

			NN<SSWR::AVIRead::AVIRCore> core;
			NN<Map::MapEnv> env;
			Math::Coord2DDbl center;
			Double scale;

			static void __stdcall OnExportClicked(AnyType userObj);
		public:
			AVIRGISExportImageForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core, NN<Map::MapEnv> env, Math::Coord2DDbl center, Double scale);
			virtual ~AVIRGISExportImageForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
