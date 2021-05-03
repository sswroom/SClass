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
				const UTF8Char *mainTitle;
				const UTF8Char *type;
				const UTF8Char *chapter;
				const UTF8Char *chapterTitle;
				const UTF8Char *videoFormat;
				Int32 width;
				Int32 height;
				Int32 fps;
				Int32 length;
				const UTF8Char *audioFormat;
				Int32 samplingRate;
				Int32 bitRate;
				const UTF8Char *aspectRatio;
				const UTF8Char *remark;
			} MovieCols;

			typedef struct
			{
				const UTF8Char *fname;
				UInt64 fSize;
				const UTF8Char *cate;
				Int32 videoId;
				Bool video;
				MovieCols *anime;
			} BurntFile;

		private:
			SSWR::DiscDB::DiscDBEnv *env;
			BurntFile *selectedFile;
			Data::ArrayList<BurntFile*> *fileList;

			UI::GUIPanel *pnlTop;
			UI::GUIPanel *pnlBurntDisc;
			UI::GUIButton *btnBrowse;
			UI::GUIButton *btnFinish;
			UI::GUIButton *btnNewDisc;
			UI::GUILabel *lblDiscId;
			UI::GUITextBox *txtDiscId;
			UI::GUILabel *lblDiscType;
			UI::GUITextBox *txtDiscType;
			UI::GUILabel *lblDate;
			UI::GUITextBox *txtDate;
			UI::GUIListBox *lbDiscId;
			UI::GUIListBox *lbBrand;
			UI::GUIListBox *lbDVDName;
			UI::GUIPanel *pnlFile;
			UI::GUILabel *lblCategory;
			UI::GUIComboBox *cboCategory;
			UI::GUIPanel *pnlDVDV;
			UI::GUILabel *lblDVDName;
			UI::GUIComboBox *cboDVDName;
			UI::GUILabel *lblSeries;
			UI::GUIComboBox *cboSeries;
			UI::GUILabel *lblVolume;
			UI::GUIComboBox *cboVolume;
			UI::GUILabel *lblDVDType;
			UI::GUIComboBox *cboDVDType;
			UI::GUIButton *btnAllFile;
			UI::GUIButton *btnRemoveFile;
			UI::GUIButton *btnBuildMovie;
			UI::GUILabel *lblSectorSize;
			UI::GUITextBox *txtSectorSize;
			UI::GUIButton *btnSectorSize;
			UI::GUIListBox *lbFileName;

			void UpdateDiscId();
			void UpdateBrand();
			void UpdateAnimeName();
			void UpdateSeries(); 
			void UpdateVolume();
			Bool UpdateFileInfo();
			void UpdateType();
			void SetVideoField(Int32 videoId);
			UInt64 SearchSubDir(const UTF8Char *absPath, const UTF8Char *relPath, UInt64 maxSize);

			void BurntFileUpdateVideo(BurntFile *file);
			BurntFile *BurntFileNew(const UTF8Char *fileName, const UTF8Char *relPath, UInt64 fileSize);
			void BurntFileFree(BurntFile *file);

			MovieCols *MovieColsNew(const UTF8Char *fileName); //
			void MovieColsFree(MovieCols *anime);

			static void __stdcall OnBrowseClicked(void *userObj);
			static void __stdcall OnFileNameSelChg(void *userObj); //
			static void __stdcall OnBrandSelChg(void *userObj);
			static void __stdcall OnDVDNameSelChg(void *userObj);
			static void __stdcall OnCboDVDNameSelChg(void *userObj);
			static void __stdcall OnCboDVDNameTextChg(void *userObj);
			static void __stdcall OnSeriesSelChg(void *userObj);
			static void __stdcall OnFinishClicked(void *userObj);
			static void __stdcall OnVolumeSelChg(void *userObj);
			static void __stdcall OnRemoveFileClicked(void *userObj);
			static void __stdcall OnCategorySelChg(void *userObj);
			static void __stdcall OnDiscIdTextChg(void *userObj);
			static void __stdcall OnSectorSizeClicked(void *userObj);
			static void __stdcall OnAllFileClicked(void *userObj);

		public:
			DiscDBBurntDiscForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::DiscDB::DiscDBEnv *env);
			virtual ~DiscDBBurntDiscForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
