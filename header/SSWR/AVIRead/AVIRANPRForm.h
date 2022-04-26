#ifndef _SM_SSWR_AVIREAD_AVIRANPRFORM
#define _SM_SSWR_AVIREAD_AVIRANPRFORM
#include "Media/ANPR.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIButton.h"
#include "UI/GUIForm.h"
#include "UI/GUIHSplitter.h"
#include "UI/GUILabel.h"
#include "UI/GUIListView.h"
#include "UI/GUIPanel.h"
#include "UI/GUIPictureBoxDD.h"
#include "UI/GUIPictureBoxSimple.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRANPRForm : public UI::GUIForm
		{
		private:
			enum class ActionType
			{
				None,
				Corners,
				Plate
			};

			typedef struct
			{
				Math::RectArea<UOSInt> area;
				Text::String *result;
				Double maxTileAngle;
				Double pxArea;
				UOSInt confidence;
				Media::StaticImage *plateImg;
			} ResultInfo;
			
		private:
			SSWR::AVIRead::AVIRCore *core;
			Media::ANPR anpr;
			Media::ColorManagerSess *colorSess;
			Data::ArrayList<ResultInfo*> results;
			Media::StaticImage *currImg;
			ActionType selectMode;
			Data::ArrayList<Math::Coord2D<Double>> points;

			UI::GUIPanel *pnlCtrl;
			UI::GUIButton *btnSelPlate;
			UI::GUIButton *btnSelCorners;
			UI::GUILabel *lblSelStatus;
			UI::GUIPanel *pnlPlate;
			UI::GUIPictureBoxSimple *pbPlate;
			UI::GUIListView *lvPlate;
			UI::GUIHSplitter *hspPlate;
			UI::GUIPictureBoxDD *pbImg;

			static void __stdcall OnFileHandler(void *userObj, Text::String **files, UOSInt nFiles);
			static void __stdcall OnPlateSelChg(void *userObj);
			static void __stdcall OnSelPlateClicked(void *userObj);
			static void __stdcall OnSelCornersClicked(void *userObj);
			static Bool __stdcall OnImgDown(void *userObj, Math::Coord2D<OSInt> scnPos, MouseButton btn);
			static void __stdcall OnANPRResult(void *userObj, Media::StaticImage *simg, Math::RectArea<UOSInt> *area, Text::String *result, Double maxTileAngle, Double pxArea, UOSInt confidence, Media::StaticImage *plateImg);
			void ClearResults();
		public:
			AVIRANPRForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core);
			virtual ~AVIRANPRForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
