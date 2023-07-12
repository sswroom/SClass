#ifndef _SM_SSWR_AVIREAD_AVIRFILESIZEPACKFORM
#define _SM_SSWR_AVIREAD_AVIRFILESIZEPACKFORM
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIButton.h"
#include "UI/GUIComboBox.h"
#include "UI/GUIForm.h"
#include "UI/GUIHSplitter.h"
#include "UI/GUILabel.h"
#include "UI/GUIListBox.h"
#include "UI/GUIPanel.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRFileSizePackForm : public UI::GUIForm
		{
		private:
			class MyFile : public Data::IComparable
			{
			private:
				Text::String *fileName;
				UInt64 fileSize;
			public:
				MyFile(Text::CString fileName, UInt64 fileSize);
				virtual ~MyFile();

				Bool ToString(Text::StringBuilderUTF8 *sb) const;
				Text::String *GetName() const;
				UInt64 GetSize() const;
				virtual OSInt CompareTo(Data::IComparable *obj) const;
			};

		private:
			Data::ArrayList<MyFile *> fileList;
			Data::ArrayList<MyFile *> packList;
			const UTF8Char *filePath;
			SSWR::AVIRead::AVIRCore *core;

			UI::GUIPanel *pnlFile;
			UI::GUILabel *lblDir;
			UI::GUITextBox *txtDir;
			UI::GUIButton *btnStart;
			UI::GUILabel *lblMaxSize;
			UI::GUIComboBox *cboMaxSize;
			UI::GUILabel *lblTotalSize;
			UI::GUITextBox *txtTotalSize;
			UI::GUILabel *lblDirName;
			UI::GUITextBox *txtDirName;
			UI::GUIButton *btnMove;
			UI::GUIListBox *lbFilePack;
			UI::GUIHSplitter *hspFile;
			UI::GUIListBox *lbFileDir;

			static void __stdcall OnStartClicked(void *userObj);
			static void __stdcall OnMoveClicked(void *userObj);

			void ReleaseObjects();
			void GenList();
			UInt64 NewCalc(Data::ArrayList<MyFile *> *fileList, Data::ArrayList<MyFile *> *packList, UInt64 maxSize, UInt64 minSize);
		public:
			AVIRFileSizePackForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core);
			virtual ~AVIRFileSizePackForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
