#ifndef _SM_SSWR_ORGANMGR_ORGANBOOKFORM
#define _SM_SSWR_ORGANMGR_ORGANBOOKFORM

#include "Data/SortableArrayList.h"
#include "SSWR/OrganMgr/OrganEnv.h"
#include "UI/GUIButton.h"
#include "UI/GUIDateTimePicker.h"
#include "UI/GUIForm.h"
#include "UI/GUIHSplitter.h"
#include "UI/GUILabel.h"
#include "UI/GUIListView.h"
#include "UI/GUIPanel.h"
#include "UI/GUITabControl.h"
#include "UI/GUITabPage.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace OrganMgr
	{
		class BookArrayList : public Data::SortableArrayList<OrganBook*>
		{
		public:
			BookArrayList();
			virtual ~BookArrayList();

			void Sort();
			static OSInt __stdcall CompareBook(OrganBook *book1, OrganBook *book2);
			virtual OSInt CompareItem(OrganBook *book1, OrganBook *book2);
		};

		class OrganBookForm : public UI::GUIForm
		{
		private:
			OrganEnv *env;

			UI::GUIPanel *pnlBook;
			UI::GUIPanel *pnlBookDet;
			UI::GUIListView *lvBook;
			UI::GUIHSplitter *hspBook;
			UI::GUIButton *btnBookPaste;
			UI::GUILabel *lblBookPublish;
			UI::GUIDateTimePicker *dtpBookPublish;
			UI::GUILabel *lblBookAuthor;
			UI::GUITextBox *txtBookAuthor;
			UI::GUILabel *lblBookTitle;
			UI::GUITextBox *txtBookTitle;
			UI::GUILabel *lblBookSource;
			UI::GUITextBox *txtBookSource;
			UI::GUILabel *lblBookURL;
			UI::GUITextBox *txtBookURL;
			UI::GUILabel *lblBookGroup;
			UI::GUITextBox *txtBookGroup;
			UI::GUIButton *btnBookAdd;
			UI::GUITabControl *tcBook;
			UI::GUITabPage *tpBookRef;
			UI::GUITabPage *tpBookAuthor;

			BookArrayList *bookList;
			Bool changed;

			static void __stdcall OnBookPublishChg(void *userObj, Data::DateTime *newDate);
			static void __stdcall OnBookAddClicked(void *userObj);
			static void __stdcall OnBookPasteClicked(void *userObj);

			void UpdateBookList();
		public:
			OrganBookForm(UI::GUIClientControl *parent, UI::GUICore *ui, OrganEnv *env);
			virtual ~OrganBookForm();

			virtual void OnMonitorChanged();

			Bool IsChanged();
		};
	}
}
#endif