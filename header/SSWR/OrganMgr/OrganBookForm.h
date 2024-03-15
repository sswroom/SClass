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
			virtual OSInt Compare(OrganBook *book1, OrganBook *book2) const;
		};

		class OrganBookForm : public UI::GUIForm
		{
		private:
			OrganEnv *env;

			NotNullPtr<UI::GUIPanel> pnlBook;
			NotNullPtr<UI::GUIPanel> pnlBookDet;
			NotNullPtr<UI::GUIListView> lvBook;
			NotNullPtr<UI::GUIHSplitter> hspBook;
			NotNullPtr<UI::GUIButton> btnBookPaste;
			NotNullPtr<UI::GUILabel> lblBookPublish;
			NotNullPtr<UI::GUIDateTimePicker> dtpBookPublish;
			NotNullPtr<UI::GUILabel> lblBookAuthor;
			NotNullPtr<UI::GUITextBox> txtBookAuthor;
			NotNullPtr<UI::GUILabel> lblBookTitle;
			NotNullPtr<UI::GUITextBox> txtBookTitle;
			NotNullPtr<UI::GUILabel> lblBookSource;
			NotNullPtr<UI::GUITextBox> txtBookSource;
			NotNullPtr<UI::GUILabel> lblBookURL;
			NotNullPtr<UI::GUITextBox> txtBookURL;
			NotNullPtr<UI::GUILabel> lblBookGroup;
			NotNullPtr<UI::GUITextBox> txtBookGroup;
			NotNullPtr<UI::GUIButton> btnBookAdd;
			NotNullPtr<UI::GUITabControl> tcBook;
			NotNullPtr<UI::GUITabPage> tpBookRef;
			NotNullPtr<UI::GUITabPage> tpBookAuthor;

			BookArrayList bookList;
			Bool changed;

			static void __stdcall OnBookPublishChg(AnyType userObj, NotNullPtr<Data::DateTime> newDate);
			static void __stdcall OnBookAddClicked(AnyType userObj);
			static void __stdcall OnBookPasteClicked(AnyType userObj);

			void UpdateBookList();
		public:
			OrganBookForm(Optional<UI::GUIClientControl> parent, NotNullPtr<UI::GUICore> ui, OrganEnv *env);
			virtual ~OrganBookForm();

			virtual void OnMonitorChanged();

			Bool IsChanged();
		};
	}
}
#endif