#include "stdafx.h"
#include "MyMemory.h"
#include "Text/MyString.h"
#include "IO/FileStream.h"
#include "IO/Path.h"
#include "IO/WindowZIP.h"
#include "Sync/Thread.h"
#include "Manage/Process.h"
#include "Manage/ThreadInfo.h"
#include <windows.h>
#include <Shldisp.h>

IO::WindowZIP::WindowZIP(const WChar *zipFile)
{
	IO::Path::PathType pt = IO::Path::GetPathTypeW(zipFile);
	this->error = true;
	this->filePath = Text::StrCopyNew(zipFile);
	if (pt == IO::Path::PathType::File)
	{
		this->error = false;
	}
	else if (pt == IO::Path::PathType::Directory)
	{
		return;
	}
	else
	{
		UInt8 buff[] = {80,75,5,6,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
		IO::FileStream *fs;
		NEW_CLASS(fs, IO::FileStream(zipFile, IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
		if (fs->Write(buff, 22) == 22)
			this->error = false;
		DEL_CLASS(fs);
	}
	CoInitialize(0);
}

IO::WindowZIP::~WindowZIP()
{
	if (this->filePath)
	{
		Text::StrDelNew(this->filePath);
	}
	if (!this->error)
	{
		CoUninitialize();
	}
}

Bool IO::WindowZIP::AddFile(const WChar *sourceFile)
{
	if (this->error)
		return false;

	Bool succ = false;
	UInt32 threadId;
	OSInt i;
	IShellDispatch *pISD;
	HRESULT hResult;
	Folder *pToFolder = 0;
	VARIANT vDir;
	VARIANT vFile;
	VARIANT vOpt;

	hResult = CoCreateInstance(CLSID_Shell, NULL, CLSCTX_INPROC_SERVER, IID_IShellDispatch, (void **)&pISD);
	if (!SUCCEEDED(hResult))
		return false;

	VariantInit(&vDir);
	vDir.vt = VT_BSTR;
	vDir.bstrVal = (BSTR)this->filePath;
	hResult = pISD->NameSpace(vDir, &pToFolder);
	if (SUCCEEDED(hResult))
	{
		VariantInit(&vFile);
		vFile.vt = VT_BSTR;
		vFile.bstrVal = (BSTR)sourceFile;

		VariantInit(&vOpt);
		vOpt.vt = VT_I4;
		vOpt.lVal = 4;

		Manage::Process proc;
		Data::ArrayList<UInt32> *currThreads;
		Data::ArrayList<UInt32> *newThreads;
		NEW_CLASS(currThreads, Data::ArrayList<UInt32>());
		NEW_CLASS(newThreads, Data::ArrayList<UInt32>());
		proc.GetThreadIds(currThreads);
		hResult = pToFolder->CopyHere(vFile, vOpt);
		if (SUCCEEDED(hResult))
		{
			succ = true;
			proc.GetThreadIds(newThreads);
			if (newThreads->GetCount() > currThreads->GetCount())
			{
				i = newThreads->GetCount();
				while (i-- > 0)
				{
					threadId = newThreads->GetItem(i);
					if (currThreads->IndexOf(threadId) == INVALID_INDEX)
					{
						Manage::ThreadInfo thread(proc.GetCurrProcId(), threadId);
						thread.WaitForThreadExit(10000);
						UInt32 code = thread.GetExitCode();
						if (code != 0)
						{
							succ = false;
						}
					}
				}
			}

		}

		pToFolder->Release();
	}
	pISD->Release();
	return succ;
}

OSInt IO::WindowZIP::GetFileList(Data::ArrayList<const WChar *> *fileList)
{
	if (this->error)
		return false;

	OSInt retCnt = 0;
	OSInt i;
	long j;
	UOSInt k;
	IShellDispatch *pISD;
	HRESULT hResult;
	Folder *pToFolder = 0;
	FolderItems *items;
	FolderItem *item;
	VARIANT vDir;
	VARIANT vOpt;

	hResult = CoCreateInstance(CLSID_Shell, NULL, CLSCTX_INPROC_SERVER, IID_IShellDispatch, (void **)&pISD);
	if (!SUCCEEDED(hResult))
		return 0;

	VariantInit(&vDir);
	vDir.vt = VT_BSTR;
	vDir.bstrVal = (BSTR)this->filePath;
	hResult = pISD->NameSpace(vDir, &pToFolder);
	if (SUCCEEDED(hResult))
	{
		if (SUCCEEDED(pToFolder->Items(&items)))
		{
			i = 0;
			j = 0;
			items->get_Count(&j);
			while (i < j)
			{
				vOpt.vt = VT_I4;
				vOpt.lVal = (LONG)i;
				if (SUCCEEDED(items->Item(vOpt, &item)))
				{
					BSTR fname;
					VARIANT_BOOL vbool = 0;
					item->get_IsFolder(&vbool);
					if (!vbool)
					{
						if (SUCCEEDED(item->get_Path(&fname)))
						{
							k = Text::StrLastIndexOf(fname, '\\');
							fileList->Add(Text::StrCopyNew(&fname[k + 1]));
							retCnt++;
						}
					}
					item->Release();
				}
				i++;
			}

			items->Release();
		}

		pToFolder->Release();
	}
	pISD->Release();
	return retCnt;
}

void IO::WindowZIP::FreeFileList(Data::ArrayList<const WChar *> *fileList)
{
	OSInt i = fileList->GetCount();
	while (i-- > 0)
	{
		Text::StrDelNew(fileList->RemoveAt(i));
	}
}

Bool IO::WindowZIP::ExtractFile(const WChar *fileName, const WChar *destPath)
{
	if (this->error)
		return false;

	Bool succ = false;
	UInt32 threadId;
	OSInt i;
	IShellDispatch *pISD;
	HRESULT hResult;
	Folder *pToFolder = 0;
	Folder *pDestFolder = 0;
	FolderItem *item;
	VARIANT vDir;
	VARIANT vFile;
	VARIANT vOpt;

	hResult = CoCreateInstance(CLSID_Shell, NULL, CLSCTX_INPROC_SERVER, IID_IShellDispatch, (void **)&pISD);
	if (!SUCCEEDED(hResult))
		return false;

	VariantInit(&vDir);
	vDir.vt = VT_BSTR;
	vDir.bstrVal = (BSTR)this->filePath;
	hResult = pISD->NameSpace(vDir, &pToFolder);
	if (SUCCEEDED(hResult))
	{
		vDir.vt = VT_BSTR;
		vDir.bstrVal = (BSTR)destPath;
		hResult = pISD->NameSpace(vDir, &pDestFolder);
		if (SUCCEEDED(hResult))
		{
			if (SUCCEEDED(pToFolder->ParseName((BSTR)fileName, &item)))
			{
				VariantInit(&vOpt);
				vOpt.vt = VT_I4;
				vOpt.lVal = 4;

				VariantInit(&vFile);
				vFile.vt = VT_DISPATCH;
				vFile.pdispVal = item;

				Manage::Process proc;
				Data::ArrayList<UInt32> *currThreads;
				Data::ArrayList<UInt32> *newThreads;
				NEW_CLASS(currThreads, Data::ArrayList<UInt32>());
				NEW_CLASS(newThreads, Data::ArrayList<UInt32>());
				proc.GetThreadIds(currThreads);
				hResult = pDestFolder->CopyHere(vFile, vOpt);
				if (SUCCEEDED(hResult))
				{
					proc.GetThreadIds(newThreads);
					if (newThreads->GetCount() > currThreads->GetCount())
					{
						i = newThreads->GetCount();
						while (i-- > 0)
						{
							threadId = newThreads->GetItem(i);
							if (currThreads->IndexOf(threadId) == INVALID_INDEX)
							{
								Manage::ThreadInfo thread(proc.GetCurrProcId(), threadId);
								thread.WaitForThreadExit(10000);
							}
						}
					}

					succ = true;
				}
				DEL_CLASS(currThreads);
				DEL_CLASS(newThreads);
				if (item)
				{
					item->Release();
				}
			}

			pDestFolder->Release();
		}
		pToFolder->Release();
	}
	pISD->Release();
	return succ;
}
