#ifndef _SM_SSWR_AVIREAD_AVIRANPRFORM
#define _SM_SSWR_AVIREAD_AVIRANPRFORM
#include "Media/ANPR.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIForm.h"
#include "UI/GUIHSplitter.h"
#include "UI/GUIListView.h"
#include "UI/GUIPictureBoxDD.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRANPRForm : public UI::GUIForm
		{
		private:
			typedef struct
			{
				Math::RectArea<UOSInt> area;
				Text::String *result;
				Double maxTileAngle;
				Double pxArea;
				UOSInt confidence;
			} ResultInfo;
			
		private:
			SSWR::AVIRead::AVIRCore *core;
			Media::ANPR anpr;
			Media::ColorManagerSess *colorSess;
			Data::ArrayList<ResultInfo*> results;
			Media::StaticImage *currImg;

			UI::GUIListView *lvPlate;
			UI::GUIHSplitter *hspPlate;
			UI::GUIPictureBoxDD *pbImg;

			static void __stdcall OnFileHandler(void *userObj, Text::String **files, UOSInt nFiles);
			static void __stdcall OnANPRResult(void *userObj, Media::StaticImage *simg, Math::RectArea<UOSInt> *area, Text::String *result, Double maxTileAngle, Double pxArea, UOSInt confidence);
			void ClearResults();
		public:
			AVIRANPRForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core);
			virtual ~AVIRANPRForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
