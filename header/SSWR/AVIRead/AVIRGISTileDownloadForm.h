#ifndef _SM_SSWR_AVIREAD_AVIRGISTILEDOWNLOADFORM
#define _SM_SSWR_AVIREAD_AVIRGISTILEDOWNLOADFORM
#include "Map/TileMapLayer.h"
#include "Map/TileMapWriter.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "SSWR/AVIRead/AVIRMapNavigator.h"
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
				NN<AVIRGISTileDownloadForm> me;
				NN<Sync::Event> threadEvt;
				Int32 threadStat; //0 = not running, 1 = idle, 2 = has task, 3 = toStop
				UIntOS lyrId;
				Math::Coord2D<Int32> imageId;
				Optional<Map::TileMapWriter> writer;
				NN<Map::TileMap> tileMap;
			};
		private:
			NN<UI::GUILabel> lblMinLevel;
			NN<UI::GUITextBox> txtMinLevel;
			NN<UI::GUILabel> lblMaxLevel;
			NN<UI::GUITextBox> txtMaxLevel;
			NN<UI::GUIButton> btnArea;
			NN<UI::GUIButton> btnSave;
			NN<UI::GUIButton> btnStop;

			NN<UI::GUILabel> lblTotalImages;
			NN<UI::GUITextBox> txtTotalImages;
			NN<UI::GUILabel> lblLayer;
			NN<UI::GUITextBox> txtLayer;
			NN<UI::GUILabel> lblImages;
			NN<UI::GUITextBox> txtImages;
			NN<UI::GUILabel> lblError;
			NN<UI::GUITextBox> txtError;

			NN<SSWR::AVIRead::AVIRCore> core;
			Math::Coord2DDbl sel1;
			Math::Coord2DDbl sel2;
			Bool selecting;
			Bool isDown;
			Math::Coord2D<IntOS> downPos;

			Int32 errCnt;
			Sync::Event mainEvt;
			Bool stopDownload;
			UIntOS threadCnt;
			UnsafeArray<ThreadStat> threadStat;

			NN<Map::TileMapLayer> lyr;
			NN<AVIRMapNavigator> navi;

			static UI::EventState __stdcall OnMouseDown(AnyType userObj, Math::Coord2D<IntOS> scnPos);
			static UI::EventState __stdcall OnMouseUp(AnyType userObj, Math::Coord2D<IntOS> scnPos);
			static UI::EventState __stdcall OnMouseMove(AnyType userObj, Math::Coord2D<IntOS> scnPos);

			static void __stdcall OnAreaClicked(AnyType userObj);
			static void __stdcall OnSaveDirClicked(AnyType userObj);
			static void __stdcall OnSaveFileClicked(AnyType userObj);
			static void __stdcall OnStopClicked(AnyType userObj);

			void SaveTilesDir(Text::CStringNN folderName, UIntOS userMinLevel, UIntOS userMaxLevel);
			void SaveTilesFile(Text::CStringNN fileName, UIntOS fileType, UIntOS userMinLevel, UIntOS userMaxLevel);
			void WriteTiles(NN<Map::TileMapWriter> writer, UIntOS userMinLevel, UIntOS userMaxLevel);
			Bool GetLevels(OutParam<UIntOS> minLevel, OutParam<UIntOS> maxLevel);
			static UInt32 __stdcall ProcThread(AnyType userObj);
		public:
			AVIRGISTileDownloadForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core, NN<Map::TileMapLayer> lyr, NN<AVIRMapNavigator> navi);
			virtual ~AVIRGISTileDownloadForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
