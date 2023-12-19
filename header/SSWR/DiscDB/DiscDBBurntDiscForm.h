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
				NotNullPtr<Text::String> type;
				const UTF8Char *chapter;
				const UTF8Char *chapterTitle;
				Text::CString videoFormat;
				Int32 width;
				Int32 height;
				Int32 fps;
				Int32 length;
				Text::CString audioFormat;
				Int32 samplingRate;
				Int32 bitRate;
				const UTF8Char *aspectRatio;
				const UTF8Char *remark;
			} MovieCols;

			typedef struct
			{
				NotNullPtr<Text::String> fname;
				UInt64 fSize;
				Text::CString cate;
				Int32 videoId;
				Bool video;
				MovieCols *anime;
			} BurntFile;

		private:
			SSWR::DiscDB::DiscDBEnv *env;
			BurntFile *selectedFile;
			Data::ArrayList<BurntFile*> *fileList;

			NotNullPtr<UI::GUIPanel> pnlTop;
			NotNullPtr<UI::GUIPanel> pnlBurntDisc;
			NotNullPtr<UI::GUIButton> btnBrowse;
			NotNullPtr<UI::GUIButton> btnFinish;
			NotNullPtr<UI::GUIButton> btnNewDisc;
			NotNullPtr<UI::GUILabel> lblDiscId;
			NotNullPtr<UI::GUITextBox> txtDiscId;
			NotNullPtr<UI::GUILabel> lblDiscType;
			NotNullPtr<UI::GUITextBox> txtDiscType;
			NotNullPtr<UI::GUILabel> lblDate;
			NotNullPtr<UI::GUITextBox> txtDate;
			UI::GUIListBox *lbDiscId;
			UI::GUIListBox *lbBrand;
			UI::GUIListBox *lbDVDName;
			NotNullPtr<UI::GUIPanel> pnlFile;
			NotNullPtr<UI::GUILabel> lblCategory;
			NotNullPtr<UI::GUIComboBox> cboCategory;
			NotNullPtr<UI::GUIPanel> pnlDVDV;
			NotNullPtr<UI::GUILabel> lblDVDName;
			NotNullPtr<UI::GUIComboBox> cboDVDName;
			NotNullPtr<UI::GUILabel> lblSeries;
			NotNullPtr<UI::GUIComboBox> cboSeries;
			NotNullPtr<UI::GUILabel> lblVolume;
			NotNullPtr<UI::GUIComboBox> cboVolume;
			NotNullPtr<UI::GUILabel> lblDVDType;
			NotNullPtr<UI::GUIComboBox> cboDVDType;
			NotNullPtr<UI::GUIButton> btnAllFile;
			NotNullPtr<UI::GUIButton> btnRemoveFile;
			NotNullPtr<UI::GUIButton> btnBuildMovie;
			NotNullPtr<UI::GUILabel> lblSectorSize;
			NotNullPtr<UI::GUITextBox> txtSectorSize;
			NotNullPtr<UI::GUIButton> btnSectorSize;
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
			BurntFile *BurntFileNew(Text::CString fileName, Text::CString relPath, UInt64 fileSize);
			void BurntFileFree(BurntFile *file);

			MovieCols *MovieColsNew(Text::CString fileName); //
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
			DiscDBBurntDiscForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, SSWR::DiscDB::DiscDBEnv *env);
			virtual ~DiscDBBurntDiscForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
