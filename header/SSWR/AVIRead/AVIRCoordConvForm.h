#ifndef _SM_SSWR_AVIREAD_AVIRCOORDCONVFORM
#define _SM_SSWR_AVIREAD_AVIRCOORDCONVFORM
#include "Data/ArrayListNN.h"
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
			NotNullPtr<UI::GUIPanel> pnlSrc;
			NotNullPtr<UI::GUILabel> lblSrc;
			UI::GUIRadioButton *radSrcGeo;
			UI::GUIRadioButton *radSrcProj;
			NotNullPtr<UI::GUIComboBox> cboSrc;
			NotNullPtr<UI::GUIPanel> pnlDest;
			NotNullPtr<UI::GUILabel> lblDest;
			UI::GUIRadioButton *radDestGeo;
			UI::GUIRadioButton *radDestProj;
			NotNullPtr<UI::GUIComboBox> cboDest;
			NotNullPtr<UI::GUIButton> btnConvFile;
			NotNullPtr<UI::GUIPanel> pnlCoord;
			NotNullPtr<UI::GUILabel> lblName;
			NotNullPtr<UI::GUITextBox> txtName;
			NotNullPtr<UI::GUILabel> lblEasting;
			NotNullPtr<UI::GUITextBox> txtEasting;
			NotNullPtr<UI::GUILabel> lblNorthing;
			NotNullPtr<UI::GUITextBox> txtNorthing;
			NotNullPtr<UI::GUILabel> lblHeight;
			NotNullPtr<UI::GUITextBox> txtHeight;
			NotNullPtr<UI::GUIButton> btnAdd;
			NotNullPtr<UI::GUIButton> btnClear;
			NotNullPtr<UI::GUIButton> btnCopyAll;
			NotNullPtr<UI::GUIListView> lvCoord;
			NotNullPtr<UI::GUITextBox> txtStatus;

			NotNullPtr<SSWR::AVIRead::AVIRCore> core;
			Data::ArrayListStringNN nameList;
			Data::ArrayList<Double> xList;
			Data::ArrayList<Double> yList;
			Data::ArrayList<Double> zList;
			Bool inited;

			static void __stdcall OnSrcRadChanged(void *userObj, Bool newValue);
			static void __stdcall OnSrcCboChanged(void *userObj);
			static void __stdcall OnDestRadChanged(void *userObj, Bool newValue);
			static void __stdcall OnDestCboChanged(void *userObj);
			static void __stdcall OnAddClicked(void *userObj);
			static void __stdcall OnClearClicked(void *userObj);
			static void __stdcall OnCopyAllClicked(void *userObj);
			static void __stdcall OnConvFileClicked(void *userObj);
			static void __stdcall OnCoordDblClk(void *userObj, UOSInt itemIndex);
			static void __stdcall OnFileDrop(void *userObj, NotNullPtr<Text::String> *files, UOSInt nFiles);

			void ClearItems(Bool updateList);
			void UpdateList();
			void FillCoordGeo(NotNullPtr<UI::GUIComboBox> cbo);
			void FillCoordProj(NotNullPtr<UI::GUIComboBox> cbo);
		public:
			AVIRCoordConvForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRCoordConvForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
