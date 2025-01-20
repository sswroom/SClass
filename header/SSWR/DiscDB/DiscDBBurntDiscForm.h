#ifndef _SM_SSWR_DISCDB_DISCDBBURNTDISCFORM
#define _SM_SSWR_DISCDB_DISCDBBURNTDISCFORM
#include "SSWR/DiscDB/DiscDBEnv.h"
#include "UI/GUIButton.h"
#include "UI/GUIComboBox.h"
#include "UI/GUIForm.h"
#include "UI/GUILabel.h"
#include "UI/GUIListBox.h"
#include "UI/GUIPanel.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace DiscDB
	{
		class DiscDBBurntDiscForm : public UI::GUIForm
		{
		private:
			typedef struct
			{
				UnsafeArray<const UTF8Char> mainTitle;
				NN<Text::String> type;
				UnsafeArrayOpt<const UTF8Char> chapter;
				UnsafeArrayOpt<const UTF8Char> chapterTitle;
				Text::CString videoFormat;
				Int32 width;
				Int32 height;
				Int32 fps;
				Int32 length;
				Text::CString audioFormat;
				Int32 samplingRate;
				Int32 bitRate;
				UnsafeArrayOpt<const UTF8Char> aspectRatio;
				UnsafeArrayOpt<const UTF8Char> remark;
			} MovieCols;

			typedef struct
			{
				NN<Text::String> fname;
				UInt64 fSize;
				Text::CStringNN cate;
				Int32 videoId;
				Bool video;
				MovieCols *anime;
			} BurntFile;

		private:
			SSWR::DiscDB::DiscDBEnv *env;
			Optional<BurntFile> selectedFile;
			Data::ArrayListNN<BurntFile> fileList;

			NN<UI::GUIPanel> pnlTop;
			NN<UI::GUIPanel> pnlBurntDisc;
			NN<UI::GUIButton> btnBrowse;
			NN<UI::GUIButton> btnFinish;
			NN<UI::GUIButton> btnNewDisc;
			NN<UI::GUILabel> lblDiscId;
			NN<UI::GUITextBox> txtDiscId;
			NN<UI::GUILabel> lblDiscType;
			NN<UI::GUITextBox> txtDiscType;
			NN<UI::GUILabel> lblDate;
			NN<UI::GUITextBox> txtDate;
			NN<UI::GUIListBox> lbDiscId;
			NN<UI::GUIListBox> lbBrand;
			NN<UI::GUIListBox> lbDVDName;
			NN<UI::GUIPanel> pnlFile;
			NN<UI::GUILabel> lblCategory;
			NN<UI::GUIComboBox> cboCategory;
			NN<UI::GUIPanel> pnlDVDV;
			NN<UI::GUILabel> lblDVDName;
			NN<UI::GUIComboBox> cboDVDName;
			NN<UI::GUILabel> lblSeries;
			NN<UI::GUIComboBox> cboSeries;
			NN<UI::GUILabel> lblVolume;
			NN<UI::GUIComboBox> cboVolume;
			NN<UI::GUILabel> lblDVDType;
			NN<UI::GUIComboBox> cboDVDType;
			NN<UI::GUIButton> btnAllFile;
			NN<UI::GUIButton> btnRemoveFile;
			NN<UI::GUIButton> btnBuildMovie;
			NN<UI::GUILabel> lblSectorSize;
			NN<UI::GUITextBox> txtSectorSize;
			NN<UI::GUIButton> btnSectorSize;
			NN<UI::GUIListBox> lbFileName;

			void UpdateDiscId();
			void UpdateBrand();
			void UpdateAnimeName();
			void UpdateSeries(); 
			void UpdateVolume();
			Bool UpdateFileInfo(NN<BurntFile> selectedFile);
			void UpdateType();
			void SetVideoField(Int32 videoId);
			UInt64 SearchSubDir(UnsafeArray<const UTF8Char> absPath, UnsafeArray<const UTF8Char> relPath, UInt64 maxSize);

			void BurntFileUpdateVideo(NN<BurntFile> file);
			NN<BurntFile> BurntFileNew(Text::CStringNN fileName, Text::CStringNN relPath, UInt64 fileSize);
			static void __stdcall BurntFileFree(NN<BurntFile> file);

			MovieCols *MovieColsNew(Text::CStringNN fileName); //
			static void MovieColsFree(MovieCols *anime);

			static void __stdcall OnBrowseClicked(AnyType userObj);
			static void __stdcall OnFileNameSelChg(AnyType userObj); //
			static void __stdcall OnBrandSelChg(AnyType userObj);
			static void __stdcall OnDVDNameSelChg(AnyType userObj);
			static void __stdcall OnCboDVDNameSelChg(AnyType userObj);
			static void __stdcall OnCboDVDNameTextChg(AnyType userObj);
			static void __stdcall OnSeriesSelChg(AnyType userObj);
			static void __stdcall OnFinishClicked(AnyType userObj);
			static void __stdcall OnVolumeSelChg(AnyType userObj);
			static void __stdcall OnRemoveFileClicked(AnyType userObj);
			static void __stdcall OnCategorySelChg(AnyType userObj);
			static void __stdcall OnDiscIdTextChg(AnyType userObj);
			static void __stdcall OnSectorSizeClicked(AnyType userObj);
			static void __stdcall OnAllFileClicked(AnyType userObj);

		public:
			DiscDBBurntDiscForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, SSWR::DiscDB::DiscDBEnv *env);
			virtual ~DiscDBBurntDiscForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
