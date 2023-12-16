#ifndef _SM_UI_WIN_WINDRAGDROP
#define _SM_UI_WIN_WINDRAGDROP
#include "Data/StringUTF8Map.h"
#include "UI/GUIControl.h"
#include <windows.h>
#include <ole2.h>

namespace UI
{
	namespace Win
	{
		class WinDropData : public UI::GUIDropData
		{
		private:
			IDataObject *pDataObj;
			Data::StringUTF8Map<FORMATETC *> *dataMap;

			void LoadData();
		public:
			WinDropData(IDataObject *pDataObj);
			virtual ~WinDropData();

			virtual UOSInt GetCount();
			virtual const UTF8Char *GetName(UOSInt index);
			virtual Bool GetDataText(const UTF8Char *name, NotNullPtr<Text::StringBuilderUTF8> sb);
			virtual IO::Stream *GetDataStream(const UTF8Char *name);
		};

		class WinDragDrop : public IDropTarget
		{
		private:
			UI::GUIDropHandler *hdlr;
			void *hWnd;
			UInt32 unkCnt;
			UI::GUIDropHandler::DragEffect currEff;

		public:
			WinDragDrop(void *hWnd, UI::GUIDropHandler *hdlr);
			virtual ~WinDragDrop();

			virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void **ppvObject);
			virtual ULONG STDMETHODCALLTYPE AddRef();
			virtual ULONG STDMETHODCALLTYPE Release();

			virtual HRESULT STDMETHODCALLTYPE DragEnter(IDataObject *pDataObj, DWORD grfKeyState, POINTL pt, DWORD *pdwEffect);
			virtual HRESULT STDMETHODCALLTYPE DragOver(DWORD grfKeyState, POINTL pt, DWORD *pdwEffect);
			virtual HRESULT STDMETHODCALLTYPE DragLeave();
			virtual HRESULT STDMETHODCALLTYPE Drop(IDataObject *pDataObj, DWORD grfKeyState, POINTL pt, DWORD *pdwEffect);

			void SetHandler(UI::GUIDropHandler *hdlr);
		};
	}
}

#endif
