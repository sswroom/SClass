#ifndef _SM_SSWR_AVIREAD_AVIRREGIONALMAPFORM
#define _SM_SSWR_AVIREAD_AVIRREGIONALMAPFORM
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIForm.h"
#include "UI/GUIListView.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRRegionalMapForm : public UI::GUIForm
		{
		private:
			SSWR::AVIRead::AVIRCore *core;
			Net::SSLEngine *ssl;

			UI::GUITextBox *txtDesc;
			UI::GUIListView *lvMaps;
			Map::IMapDrawLayer *layer;

			static void __stdcall OnMapsSelChg(void *userObj);
			static void __stdcall OnMapsDblClk(void *userObj, UOSInt index);
		public:
			AVIRRegionalMapForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core, Net::SSLEngine *ssl);
			virtual ~AVIRRegionalMapForm();

			virtual void OnMonitorChanged();

			Map::IMapDrawLayer *GetMapLayer();
		};
	}
}
#endif
