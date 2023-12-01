#ifndef _SM_SSWR_AVIREAD_AVIRGISTILEDOWNLOADFORM
#define _SM_SSWR_AVIREAD_AVIRGISTILEDOWNLOADFORM
#include "Map/TileMapLayer.h"
#include "Map/TileMapWriter.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "SSWR/AVIRead/IMapNavigator.h"
#include "Sync/Mutex.h"
#include "UI/GUIButton.h"
#include "UI/GUIForm.h"
#include "UI/GUILabel.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRGISTileDownloadForm : public UI::GUIForm
		{
		private:
			struct ThreadStat
			{
				AVIRGISTileDownloadForm *me;
				Sync::Event *threadEvt;
				Int32 threadStat; //0 = not running, 1 = idle, 2 = has task, 3 = toStop
				UOSInt lyrId;
				Math::Coord2D<Int32> imageId;
				Map::TileMapWriter *writer;
				NotNullPtr<Map::TileMap> tileMap;
			};
		private:
			NotNullPtr<UI::GUILabel> lblMinLevel;
			NotNullPtr<UI::GUITextBox> txtMinLevel;
			NotNullPtr<UI::GUILabel> lblMaxLevel;
			NotNullPtr<UI::GUITextBox> txtMaxLevel;
			NotNullPtr<UI::GUIButton> btnArea;
			NotNullPtr<UI::GUIButton> btnSave;
			NotNullPtr<UI::GUIButton> btnStop;

			NotNullPtr<UI::GUILabel> lblTotalImages;
			NotNullPtr<UI::GUITextBox> txtTotalImages;
			NotNullPtr<UI::GUILabel> lblLayer;
			NotNullPtr<UI::GUITextBox> txtLayer;
			NotNullPtr<UI::GUILabel> lblImages;
			NotNullPtr<UI::GUITextBox> txtImages;
			NotNullPtr<UI::GUILabel> lblError;
			NotNullPtr<UI::GUITextBox> txtError;

			NotNullPtr<SSWR::AVIRead::AVIRCore> core;
			Math::Coord2DDbl sel1;
			Math::Coord2DDbl sel2;
			Bool selecting;
			Bool isDown;
			Math::Coord2D<OSInt> downPos;

			Int32 errCnt;
			Sync::Event mainEvt;
			Bool stopDownload;
			UOSInt threadCnt;
			ThreadStat *threadStat;

			NotNullPtr<Map::TileMapLayer> lyr;
			IMapNavigator *navi;

			static Bool __stdcall OnMouseDown(void *userObj, Math::Coord2D<OSInt> scnPos);
			static Bool __stdcall OnMouseUp(void *userObj, Math::Coord2D<OSInt> scnPos);
			static Bool __stdcall OnMouseMove(void *userObj, Math::Coord2D<OSInt> scnPos);

			static void __stdcall OnAreaClicked(void *userObj);
			static void __stdcall OnSaveDirClicked(void *userObj);
			static void __stdcall OnSaveFileClicked(void *userObj);
			static void __stdcall OnStopClicked(void *userObj);

			void SaveTilesDir(Text::CStringNN folderName, UOSInt userMinLevel, UOSInt userMaxLevel);
			void SaveTilesFile(Text::CStringNN fileName, UOSInt fileType, UOSInt userMinLevel, UOSInt userMaxLevel);
			void WriteTiles(NotNullPtr<Map::TileMapWriter> writer, UOSInt userMinLevel, UOSInt userMaxLevel);
			Bool GetLevels(OutParam<UOSInt> minLevel, OutParam<UOSInt> maxLevel);
			static UInt32 __stdcall ProcThread(void *userObj);
		public:
			AVIRGISTileDownloadForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core, NotNullPtr<Map::TileMapLayer> lyr, IMapNavigator *navi);
			virtual ~AVIRGISTileDownloadForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
