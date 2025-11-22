#ifndef _SM_SSWR_AVIREAD_AVIRCOORDCONVFORM
#define _SM_SSWR_AVIREAD_AVIRCOORDCONVFORM
#include "Data/ArrayListNN.hpp"
#include "SSWR/AVIRead/AVIRCore.h"
#include "Text/String.h"
#include "UI/GUIButton.h"
#include "UI/GUIComboBox.h"
#include "UI/GUIForm.h"
#include "UI/GUILabel.h"
#include "UI/GUIListView.h"
#include "UI/GUIPanel.h"
#include "UI/GUIRadioButton.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRCoordConvForm : public UI::GUIForm
		{
		private:
			NN<UI::GUIPanel> pnlSrc;
			NN<UI::GUILabel> lblSrc;
			NN<UI::GUIRadioButton> radSrcGeo;
			NN<UI::GUIRadioButton> radSrcProj;
			NN<UI::GUIComboBox> cboSrc;
			NN<UI::GUIPanel> pnlDest;
			NN<UI::GUILabel> lblDest;
			NN<UI::GUIRadioButton> radDestGeo;
			NN<UI::GUIRadioButton> radDestProj;
			NN<UI::GUIComboBox> cboDest;
			NN<UI::GUIButton> btnConvFile;
			NN<UI::GUIPanel> pnlCoord;
			NN<UI::GUILabel> lblName;
			NN<UI::GUITextBox> txtName;
			NN<UI::GUILabel> lblEasting;
			NN<UI::GUITextBox> txtEasting;
			NN<UI::GUILabel> lblNorthing;
			NN<UI::GUITextBox> txtNorthing;
			NN<UI::GUILabel> lblHeight;
			NN<UI::GUITextBox> txtHeight;
			NN<UI::GUIButton> btnAdd;
			NN<UI::GUIButton> btnClear;
			NN<UI::GUIButton> btnCopyAll;
			NN<UI::GUIListView> lvCoord;
			NN<UI::GUITextBox> txtStatus;

			NN<SSWR::AVIRead::AVIRCore> core;
			Data::ArrayListStringNN nameList;
			Data::ArrayList<Double> xList;
			Data::ArrayList<Double> yList;
			Data::ArrayList<Double> zList;
			Bool inited;

			static void __stdcall OnSrcRadChanged(AnyType userObj, Bool newValue);
			static void __stdcall OnSrcCboChanged(AnyType userObj);
			static void __stdcall OnDestRadChanged(AnyType userObj, Bool newValue);
			static void __stdcall OnDestCboChanged(AnyType userObj);
			static void __stdcall OnAddClicked(AnyType userObj);
			static void __stdcall OnClearClicked(AnyType userObj);
			static void __stdcall OnCopyAllClicked(AnyType userObj);
			static void __stdcall OnConvFileClicked(AnyType userObj);
			static void __stdcall OnCoordDblClk(AnyType userObj, UOSInt itemIndex);
			static void __stdcall OnFileDrop(AnyType userObj, Data::DataArray<NN<Text::String>> files);

			void ClearItems(Bool updateList);
			void UpdateList();
			void FillCoordGeo(NN<UI::GUIComboBox> cbo);
			void FillCoordProj(NN<UI::GUIComboBox> cbo);
		public:
			AVIRCoordConvForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRCoordConvForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
