#ifndef _SM_SSWR_AVIREAD_AVIROSMTILEGENERATORFORM
#define _SM_SSWR_AVIREAD_AVIROSMTILEGENERATORFORM
#include "Data/ArrayListNN.hpp"
#include "Map/MapDrawLayer.h"
#include "Map/MapEnv.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIButton.h"
#include "UI/GUIComboBox.h"
#include "UI/GUIForm.h"
#include "UI/GUIListBox.h"
#include "UI/GUILabel.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIROSMTileGeneratorForm : public UI::GUIForm
		{
		private:
			NN<UI::GUIListBox> lbLayers;
			NN<UI::GUILabel> lblBounds;
			NN<UI::GUITextBox> txtBounds;
			NN<UI::GUILabel> lblMinLevel;
			NN<UI::GUIComboBox> cboMinLevel;
			NN<UI::GUILabel> lblMaxLevel;
			NN<UI::GUIComboBox> cboMaxLevel;
			NN<UI::GUILabel> lblFormat;
			NN<UI::GUIComboBox> cboFormat;
			NN<UI::GUILabel> lblQuality;
			NN<UI::GUITextBox> txtQuality;
			NN<UI::GUIButton> btnExport;

			NN<SSWR::AVIRead::AVIRCore> core;
			NN<Map::MapEnv> env;
			Data::ArrayListNN<Map::MapDrawLayer> layers;

			static void __stdcall OnFileDrop(AnyType userObj, Data::DataArray<NN<Text::String>> files);
			static void __stdcall OnExportClicked(AnyType userObj);

			void UpdateBounds();
			void AddFile(Text::CStringNN fileName);
			void ExportTiles(Text::CStringNN fileName);
		public:
			AVIROSMTileGeneratorForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIROSMTileGeneratorForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
