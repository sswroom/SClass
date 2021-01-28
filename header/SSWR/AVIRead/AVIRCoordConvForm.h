#ifndef _SM_SSWR_AVIREAD_AVIRCOORDCONVFORM
#define _SM_SSWR_AVIREAD_AVIRCOORDCONVFORM
#include "SSWR/AVIRead/AVIRCore.h"
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
			UI::GUIPanel *pnlSrc;
			UI::GUILabel *lblSrc;
			UI::GUIRadioButton *radSrcGeo;
			UI::GUIRadioButton *radSrcProj;
			UI::GUIComboBox *cboSrc;
			UI::GUIPanel *pnlDest;
			UI::GUILabel *lblDest;
			UI::GUIRadioButton *radDestGeo;
			UI::GUIRadioButton *radDestProj;
			UI::GUIComboBox *cboDest;
			UI::GUIButton *btnConvFile;
			UI::GUIPanel *pnlCoord;
			UI::GUILabel *lblName;
			UI::GUITextBox *txtName;
			UI::GUILabel *lblEasting;
			UI::GUITextBox *txtEasting;
			UI::GUILabel *lblNorthing;
			UI::GUITextBox *txtNorthing;
			UI::GUILabel *lblHeight;
			UI::GUITextBox *txtHeight;
			UI::GUIButton *btnAdd;
			UI::GUIButton *btnClear;
			UI::GUIListView *lvCoord;

			SSWR::AVIRead::AVIRCore *core;
			Data::ArrayList<const UTF8Char *> *nameList;
			Data::ArrayList<Double> *xList;
			Data::ArrayList<Double> *yList;
			Data::ArrayList<Double> *zList;
			Bool inited;

			static void __stdcall OnSrcRadChanged(void *userObj, Bool newValue);
			static void __stdcall OnSrcCboChanged(void *userObj);
			static void __stdcall OnDestRadChanged(void *userObj, Bool newValue);
			static void __stdcall OnDestCboChanged(void *userObj);
			static void __stdcall OnAddClicked(void *userObj);
			static void __stdcall OnClearClicked(void *userObj);
			static void __stdcall OnConvFileClicked(void *userObj);

			void ClearItems();
			void UpdateList();
			void FillCoordGeo(UI::GUIComboBox *cbo);
			void FillCoordProj(UI::GUIComboBox *cbo);
		public:
			AVIRCoordConvForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core);
			virtual ~AVIRCoordConvForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
