#include "Stdafx.h"
#include "IO/FileStream.h"
#include "IO/Path.h"
#include "IO/StmData/MemoryData.h"
#include "Manage/HiResClock.h"
#include "Net/HTTPClient.h"
#include "Net/HTTPOSClient.h"
#include "Net/MIME.h"
#include "Net/SSLEngineFactory.h"
#include "SSWR/AVIRead/AVIRHTTPClientForm.h"
#include "SSWR/AVIRead/AVIRUserAgentSelForm.h"
#include "Sync/MutexUsage.h"
#include "Sync/Thread.h"
#include "Text/IMIMEObj.h"
#include "Text/JSONBuilder.h"
#include "Text/MyString.h"
#include "Text/MyStringFloat.h"
#include "Text/URLString.h"
#include "Text/TextBinEnc/Base64Enc.h"
#include "Text/TextBinEnc/FormEncoding.h"
#include "UI/FileDialog.h"
#include "UI/MessageDialog.h"

void __stdcall SSWR::AVIRead::AVIRHTTPClientForm::OnUserAgentClicked(void *userObj)
{
	SSWR::AVIRead::AVIRHTTPClientForm *me = (SSWR::AVIRead::AVIRHTTPClientForm*)userObj;
	SSWR::AVIRead::AVIRUserAgentSelForm *frm;
	NEW_CLASS(frm, SSWR::AVIRead::AVIRUserAgentSelForm(0, me->ui, me->core, me->userAgent->ToCString()));
	if (frm->ShowDialog(me))
	{
		SDEL_STRING(me->userAgent);
		me->userAgent = Text::String::New(frm->GetUserAgent());
		me->lblUserAgent->SetText(me->userAgent->ToCString());
	}
	DEL_CLASS(frm);
}

void __stdcall SSWR::AVIRead::AVIRHTTPClientForm::OnRequestClicked(void *userObj)
{
	SSWR::AVIRead::AVIRHTTPClientForm *me = (SSWR::AVIRead::AVIRHTTPClientForm*)userObj;
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	Text::StringBuilderUTF8 sb;
	Text::StringBuilderUTF8 sbTmp;
	Text::CString mime;
	me->txtURL->GetText(&sb);
	if (!sb.StartsWith(UTF8STRC("http://")) && !sb.StartsWith(UTF8STRC("https://")))
	{
		UI::MessageDialog::ShowDialog(CSTR("Please enter valid http URL"), CSTR("Request"), me);
		return;
	}

	sbTmp.ClearStr();
	if (me->txtUserName->GetText(&sbTmp) && sbTmp.GetCharCnt() > 0)
	{
		me->reqUserName = Text::String::New(sbTmp.ToCString());
	}
	sbTmp.ClearStr();
	if (me->txtPassword->GetText(&sbTmp) && sbTmp.GetCharCnt() > 0)
	{
		me->reqPassword = Text::String::New(sbTmp.ToCString());
	}
	sbTmp.ClearStr();
	if (me->txtHeaders->GetText(&sbTmp) && sbTmp.GetCharCnt() > 0)
	{
		me->reqHeaders = Text::String::New(sbTmp.ToCString());
	}


	me->noShutdown = me->chkNoShutdown->IsChecked();
	me->reqMeth = (Net::WebUtil::RequestMethod)(OSInt)me->cboMethod->GetSelectedItem();
	me->reqOSClient = me->chkOSClient->IsChecked();
	if ((me->reqMeth == Net::WebUtil::RequestMethod::HTTP_GET) || (me->reqMeth == Net::WebUtil::RequestMethod::HTTP_DELETE))
	{
		UOSInt i = 0;
		UOSInt j = me->params->GetCount();
		if (j > 0)
		{
			if (sb.IndexOf('?') != INVALID_INDEX)
			{
				sb.AppendUTF8Char('&');
			}
			else
			{
				sb.AppendUTF8Char('?');
			}
			SSWR::AVIRead::AVIRHTTPClientForm::ParamValue *param;
			Text::StringBuilderUTF8 sb2;
			while (i < j)
			{
				param = me->params->GetItem(i);
				if (sb2.GetCharCnt() > 0)
				{
					sb2.AppendUTF8Char('&');
				}
				sptr = Text::TextBinEnc::FormEncoding::FormEncode(sbuff, param->name->v);
				sb2.AppendP(sbuff, sptr);
				sb2.AppendUTF8Char('=');
				sptr = Text::TextBinEnc::FormEncoding::FormEncode(sbuff, param->value->v);
				sb2.AppendP(sbuff, sptr);
				i++;
			}
			sb.Append(sb2);
		}
		me->reqBody = 0;
		me->reqBodyLen = 0;
		me->reqBodyType = 0;
	}
	else if (me->fileList->GetCount() == 1 && me->cboPostFormat->GetSelectedIndex() == 2)
	{
		Text::String *fileName = me->fileList->GetItem(0);
		UTF8Char sbuff[32];
		UTF8Char *sptr;
		IO::FileStream *fs;
		NEW_CLASS(fs, IO::FileStream(fileName, IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
		me->reqBodyLen = (UOSInt)fs->GetLength();
		me->reqBody = MemAlloc(UInt8, me->reqBodyLen);
		fs->Read((UInt8*)me->reqBody, me->reqBodyLen);
		DEL_CLASS(fs);
		if ((sptr = IO::Path::GetFileExt(sbuff, fileName->v, fileName->leng)) != 0)
		{
			mime = Net::MIME::GetMIMEFromExt(sbuff, (UOSInt)(sptr - sbuff));
			me->reqBodyType = Text::String::New(mime.v, mime.leng);
		}
		else
		{
			me->reqBodyType = 0;
		}
	}
	else if (me->fileList->GetCount() > 0)
	{
		Text::StringBuilderUTF8 sbBoundary;
		Text::StringBuilderUTF8 sb2;
		Data::DateTime dt;
		dt.SetCurrTimeUTC();
		sbBoundary.AppendC(UTF8STRC("---------------------------Boundary"));
		sbBoundary.AppendI64(dt.ToTicks());
		sb2.AppendC(UTF8STRC("multipart/form-data; boundary="));
		sb2.AppendC(sbBoundary.ToString(), sbBoundary.GetLength());
		me->reqBodyType = Text::String::New(sb2.ToString(), sb2.GetLength());
		IO::MemoryStream mstm(UTF8STRC("SSWR.AVIRead.AVIRHTTPClientForm.OnRequestClicked.mstm"));
		UOSInt i = 0;
		UOSInt j = me->params->GetCount();
		UOSInt k;
		SSWR::AVIRead::AVIRHTTPClientForm::ParamValue *param;
		Text::String *s;
		while (i < j)
		{
			param = me->params->GetItem(i);
			mstm.Write((const UInt8*)"--", 2);
			mstm.Write(sbBoundary.ToString(), sbBoundary.GetCharCnt());
			mstm.Write(UTF8STRC("\r\nContent-Disposition: form-data; name=\""));
			sptr = Text::TextBinEnc::FormEncoding::FormEncode(sbuff, param->name->v);
			mstm.Write(sbuff, (UOSInt)(sptr - sbuff));
			mstm.Write((const UInt8*)"\"\r\n\r\n", 5);
			mstm.Write(param->value->v, param->value->leng);
			mstm.Write((const UInt8*)"\r\n", 2);

			i++;
		}

		sb2.ClearStr();
		me->txtFileFormName->GetText(&sb2);
		UInt8 fileBuff[4096];
		i = 0;
		j = me->fileList->GetCount();
		while (i < j)
		{
			s = me->fileList->GetItem(i);
			IO::FileStream *fs;
			UInt64 fileLength;
			UInt64 ofst;
			NEW_CLASS(fs, IO::FileStream(s->ToCString(), IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
			fileLength = fs->GetLength();
			if (fileLength > 0 && fileLength < 104857600)
			{
				mstm.Write((const UInt8*)"--", 2);
				mstm.Write(sbBoundary.ToString(), sbBoundary.GetCharCnt());
				mstm.Write(UTF8STRC("\r\nContent-Disposition: form-data; "));
				if (sb.GetCharCnt() > 0)
				{
					mstm.Write((const UInt8*)"name=\"", 6);
					sptr = Text::TextBinEnc::FormEncoding::FormEncode(sbuff, sb2.ToString());
					mstm.Write(sbuff, (UOSInt)(sptr - sbuff));
					mstm.Write((const UInt8*)"\"; ", 3);
				}
				k = s->LastIndexOf(IO::Path::PATH_SEPERATOR);
				mstm.Write((const UInt8*)"filename=\"", 10);
				sptr = Text::TextBinEnc::FormEncoding::FormEncode(sbuff, &s->v[k + 1]);
				mstm.Write(sbuff, (UOSInt)(sptr - sbuff));
				mstm.Write((const UInt8*)"\"\r\n", 3);

				sptr = IO::Path::GetFileExt(sbuff, &s->v[k], s->leng - k);
				mime = Net::MIME::GetMIMEFromExt(sbuff, (UOSInt)(sptr - sbuff));
				mstm.Write((const UInt8*)"Content-Type: ", 14);
				mstm.Write(mime.v, mime.leng);
				mstm.Write((const UInt8*)"\r\n\r\n", 4);

				ofst = 0;
				while (ofst < fileLength)
				{
					k = fs->Read(fileBuff, 4096);
					if (k <= 0)
					{
						break;
					}
					mstm.Write(fileBuff, k);
					ofst += k;
				}
				mstm.Write((const UInt8*)"\r\n", 2);
			}
			DEL_CLASS(fs);
			i++;
		}
		mstm.Write((const UInt8*)"--", 2);
		mstm.Write(sbBoundary.ToString(), sbBoundary.GetCharCnt());
		mstm.Write((const UInt8*)"--", 2);

		UOSInt buffSize;
		UInt8 *reqBuff = mstm.GetBuff(&buffSize);

		me->reqBody = MemAlloc(UInt8, buffSize);
		me->reqBodyLen = buffSize;
		MemCopyNO((UInt8*)me->reqBody, reqBuff, buffSize);
	}
	else if (me->cboPostFormat->GetSelectedIndex() == 1)
	{
		UOSInt i = 0;
		UOSInt j = me->params->GetCount();
		SSWR::AVIRead::AVIRHTTPClientForm::ParamValue *param;
		Text::StringBuilderUTF8 sb2;
		Text::JSONBuilder *json;
		NEW_CLASS(json, Text::JSONBuilder(&sb2, Text::JSONBuilder::OT_OBJECT));
		while (i < j)
		{
			param = me->params->GetItem(i);
			json->ObjectAddStrUTF8(param->name->v, param->value->v);
			i++;
		}
		DEL_CLASS(json);
		me->reqBody = Text::StrCopyNew(sb2.ToString());
		me->reqBodyLen = sb2.GetCharCnt();
		me->reqBodyType = Text::String::New(UTF8STRC("application/json"));
	}
	else
	{
		UOSInt i = 0;
		UOSInt j = me->params->GetCount();
		SSWR::AVIRead::AVIRHTTPClientForm::ParamValue *param;
		Text::StringBuilderUTF8 sb2;
		while (i < j)
		{
			param = me->params->GetItem(i);
			if (sb2.GetCharCnt() > 0)
			{
				sb2.AppendUTF8Char('&');
			}
			sptr = Text::TextBinEnc::FormEncoding::FormEncode(sbuff, param->name->v);
			sb2.AppendP(sbuff, sptr);
			sb2.AppendUTF8Char('=');
			sptr = Text::TextBinEnc::FormEncoding::FormEncode(sbuff, param->value->v);
			sb2.AppendP(sbuff, sptr);
			i++;
		}
		me->reqBody = Text::StrCopyNew(sb2.ToString());
		me->reqBodyLen = sb2.GetCharCnt();
		me->reqBodyType = Text::String::New(UTF8STRC("application/x-www-form-urlencoded"));
	}
	me->reqURL = Text::String::New(sb.ToString(), sb.GetLength());
	me->threadEvt->Set();
	while (me->threadRunning && me->reqURL && !me->respChanged)
	{
		Sync::Thread::Sleep(1);
	}
}

void __stdcall SSWR::AVIRead::AVIRHTTPClientForm::OnSaveClicked(void *userObj)
{
	SSWR::AVIRead::AVIRHTTPClientForm *me = (SSWR::AVIRead::AVIRHTTPClientForm*)userObj;
	if (me->respData == 0)
	{
		return;
	}
	UI::FileDialog *dlg;
	NEW_CLASS(dlg, UI::FileDialog(L"SSWR", L"AVIRead", L"HTTPClientSave", true));
	if (me->respHeaders)
	{
		UOSInt i = me->respHeaders->GetCount();
		while (i-- > 0)
		{
			Text::String *hdr = me->respHeaders->GetItem(i);
			if (hdr->StartsWithICase(UTF8STRC("Content-Disposition: ")))
			{
				Text::StringBuilderUTF8 sb;
				sb.AppendC(hdr->v + 21, hdr->leng - 21);
				Text::PString sarr[2];
				UOSInt j;
				sarr[1].v = sb.ToString();
				sarr[1].leng = sb.GetLength();
				while (true)
				{
					j = Text::StrSplitTrimP(sarr, 2, sarr[1], ';');
					if (Text::StrStartsWithC(sarr[0].v, sarr[0].leng, UTF8STRC("filename=")))
					{
						if (sarr[0].v[9] == '\"')
						{
							j = Text::StrIndexOfChar(&sarr[0].v[10], '\"');
							if (j != INVALID_INDEX)
							{
								sarr[0].v[10 + j] = 0;
							}
							dlg->SetFileName(&sarr[0].v[10]);
						}
						else
						{
							dlg->SetFileName(&sarr[0].v[9]);
						}
						break;
					}
					if (j != 2)
					{
						break;
					}
				}
			}
		}
	}
	if (dlg->ShowDialog(me->GetHandle()))
	{
		IO::FileStream *fs;
		Bool succ = false;
		NEW_CLASS(fs, IO::FileStream(dlg->GetFileName(), IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
		Sync::MutexUsage mutUsage(me->respMut);
		if (me->respData)
		{
			UOSInt buffSize;
			UOSInt writeSize;
			UInt8 *buff = me->respData->GetBuff(&buffSize);
			writeSize = fs->Write(buff, buffSize);
			succ = (writeSize == buffSize);
		}
		mutUsage.EndUse();
		DEL_CLASS(fs);
		if (!succ)
		{
			UI::MessageDialog::ShowDialog(CSTR("Error in storing to file"), CSTR("HTTP Client"), me);
		}
	}
	DEL_CLASS(dlg);
}

void __stdcall SSWR::AVIRead::AVIRHTTPClientForm::OnViewClicked(void *userObj)
{
	SSWR::AVIRead::AVIRHTTPClientForm *me = (SSWR::AVIRead::AVIRHTTPClientForm*)userObj;
	Sync::MutexUsage mutUsage(me->respMut);
	if (me->respData)
	{
		UOSInt buffSize;
		UInt8 *buff = me->respData->GetBuff(&buffSize);
		Text::IMIMEObj *mimeObj;
		IO::StmData::MemoryData *md;
		NEW_CLASS(md, IO::StmData::MemoryData(buff, buffSize));
		mimeObj = Text::IMIMEObj::ParseFromData(md, me->respContType->ToCString());
		DEL_CLASS(md);
		if (mimeObj)
		{
			me->core->OpenObject(mimeObj);
		}
	}
	mutUsage.EndUse();
}

void __stdcall SSWR::AVIRead::AVIRHTTPClientForm::OnDataStrClicked(void *userObj)
{
	SSWR::AVIRead::AVIRHTTPClientForm *me = (SSWR::AVIRead::AVIRHTTPClientForm*)userObj;
	Text::StringBuilderUTF8 sb;
	SSWR::AVIRead::AVIRHTTPClientForm::ParamValue *param;
	UTF8Char sbuff[512];
	me->txtDataStr->GetText(&sb);
	me->ClearParams();
	me->lvReqData->ClearItems();
	if (sb.GetCharCnt() > 0)
	{
		UTF8Char *sptr = sb.ToString();
		UOSInt spInd;
		UOSInt eqInd;
		UOSInt i;
		while (true)
		{
			spInd = Text::StrIndexOfChar(sptr, '&');
			if (spInd != INVALID_INDEX)
			{
				sptr[spInd] = 0;
			}
			eqInd = Text::StrIndexOfChar(sptr, '=');
			param = MemAlloc(SSWR::AVIRead::AVIRHTTPClientForm::ParamValue, 1);
			if (eqInd != INVALID_INDEX)
			{
				sptr[eqInd] = 0;
				sptr = Text::TextBinEnc::FormEncoding::FormDecode(sbuff, sptr);
				param->name = Text::String::New(sbuff, (UOSInt)(sptr - sbuff));
				sptr = Text::TextBinEnc::FormEncoding::FormDecode(sbuff, &sptr[eqInd + 1]);
				param->value = Text::String::New(sbuff, (UOSInt)(sptr - sbuff));
			}
			else
			{
				sptr = Text::TextBinEnc::FormEncoding::FormDecode(sbuff, sptr);
				param->name = Text::String::New(sbuff, (UOSInt)(sptr - sbuff));
				param->value = Text::String::NewEmpty();
			}
			me->params->Add(param);
			i = me->lvReqData->AddItem(param->name, param);
			me->lvReqData->SetSubItem(i, 1, param->value);

			if (spInd == INVALID_INDEX)
			{
				break;
			}
			sptr = &sptr[spInd + 1];
		}

	}
}

void __stdcall SSWR::AVIRead::AVIRHTTPClientForm::OnFileSelectClicked(void *userObj)
{
	SSWR::AVIRead::AVIRHTTPClientForm *me = (SSWR::AVIRead::AVIRHTTPClientForm*)userObj;
	UI::FileDialog *dlg;
	NEW_CLASS(dlg, UI::FileDialog(L"SSWR", L"AVIRead", L"HTTPClientSelect", false));
	dlg->SetAllowMultiSel(true);
	if (dlg->ShowDialog(me->GetHandle()))
	{
		me->ClearFiles();

		UOSInt i = 0;
		UOSInt j = dlg->GetFileNameCount();
		const UTF8Char *fileName;
		while (i < j)
		{
			fileName = dlg->GetFileNames(i);
			me->fileList->Add(Text::String::NewNotNull(fileName));
			i++;
		}
		Text::StringBuilderUTF8 sb;
		sb.AppendUOSInt(j);
		if (j > 1)
		{
			sb.AppendC(UTF8STRC(" files selected"));
		}
		else
		{
			sb.AppendC(UTF8STRC(" files selected"));
		}
		me->lblFileStatus->SetText(sb.ToCString());
	}
	DEL_CLASS(dlg);
}

void __stdcall SSWR::AVIRead::AVIRHTTPClientForm::OnFileClearClicked(void *userObj)
{
	SSWR::AVIRead::AVIRHTTPClientForm *me = (SSWR::AVIRead::AVIRHTTPClientForm*)userObj;
	me->ClearFiles();
	me->lblFileStatus->SetText(CSTR("No files selected"));
}

UInt32 __stdcall SSWR::AVIRead::AVIRHTTPClientForm::ProcessThread(void *userObj)
{
	SSWR::AVIRead::AVIRHTTPClientForm *me = (SSWR::AVIRead::AVIRHTTPClientForm*)userObj;
	Text::String *currURL;
	const UTF8Char *currBody;
	UOSInt currBodyLen;
	Text::String *currBodyType;
	Text::String *currUserName;
	Text::String *currPassword;
	Text::String *currHeaders;
	Net::WebUtil::RequestMethod currMeth;
	Bool currOSClient;
	UInt8 buff[4096];
	UTF8Char *sbuff;
	UTF8Char *sptr;
	UOSInt i;
	UOSInt j;
	me->threadRunning = true;
	sbuff = MemAlloc(UTF8Char, 65536);
	while (!me->threadToStop)
	{
		if (me->reqURL && !me->respChanged)
		{
			currURL = me->reqURL;
			currBody = me->reqBody;
			currBodyLen = me->reqBodyLen;
			currBodyType = me->reqBodyType;
			currMeth = me->reqMeth;
			currUserName = me->reqUserName;
			currPassword = me->reqPassword;
			currOSClient = me->reqOSClient;
			currHeaders = me->reqHeaders;
			me->reqURL = 0;
			me->reqBody = 0;
			me->reqBodyLen = 0;
			me->reqBodyType = 0;
			me->reqUserName = 0;
			me->reqPassword = 0;
			me->reqHeaders = 0;
			
			Net::HTTPClient *cli;
			cli = Net::HTTPClient::CreateClient(me->core->GetSocketFactory(), currOSClient?0:me->ssl, me->userAgent->ToCString(), me->noShutdown, currURL->StartsWith(UTF8STRC("https://")));
//			NEW_CLASS(cli, Net::HTTPOSClient(me->core->GetSocketFactory(), me->userAgent, me->noShutdown));
			if (cli->Connect(currURL->ToCString(), currMeth, &me->respTimeDNS, &me->respTimeConn, false))
			{
				IO::MemoryStream *mstm;
				Text::String *contType = 0;
				NEW_CLASS(mstm, IO::MemoryStream(UTF8STRC("SSWR.AVIRead.AVIRHTTPClientForm.respData")));
				cli->AddHeaderC(CSTR("Accept"), CSTR("*/*"));
				cli->AddHeaderC(CSTR("Accept-Charset"), CSTR("*"));
				cli->AddHeaderC(CSTR("User-Agent"), me->userAgent->ToCString());
				if (me->noShutdown)
				{
					cli->AddHeaderC(CSTR("Connection"), CSTR("keep-alive"));
				}
				else
				{
					cli->AddHeaderC(CSTR("Connection"), CSTR("close"));
				}
				cli->AddHeaderC(CSTR("Accept-Encoding"), CSTR("gzip, deflate"));
				
				sptr = me->AppendCookie(sbuff, currURL->v, currURL->leng);
				if (sptr)
				{
					cli->AddHeaderC(CSTR("Cookie"), CSTRP(sbuff, sptr));
				}

				if (currMeth != Net::WebUtil::RequestMethod::HTTP_GET && currBody)
				{
					sptr = Text::StrUOSInt(sbuff, currBodyLen);
					cli->AddHeaderC(CSTR("Content-Length"), CSTRP(sbuff, sptr));
					cli->AddHeaderC(CSTR("Content-Type"), currBodyType->ToCString());
					cli->Write(currBody, currBodyLen);
				}
				if (currHeaders)
				{
					Text::StringBuilderUTF8 sb;
					sb.Append(currHeaders);
					Text::PString sarr[2];
					Text::PString sarr2[2];
					sarr[1] = sb;
					while (true)
					{
						i = Text::StrSplitLineP(sarr, 2, sarr[1]);
						if (Text::StrSplitTrimP(sarr2, 2, sarr[0], ':') == 2)
						{
							cli->AddHeaderC(sarr2[0].ToCString(), sarr2[1].ToCString());
						}

						if (i != 2)
						{
							break;
						}
					}
				}

				cli->EndRequest(&me->respTimeReq, &me->respTimeResp);
				UInt64 totalRead = 0;
				UOSInt thisRead;
				while ((thisRead = cli->Read(buff, 4096)) > 0)
				{
					mstm->Write(buff, thisRead);
					totalRead += thisRead;
				}
				me->respTimeTotal = cli->GetTotalTime();
				me->respSize = totalRead;
				me->respStatus = cli->GetRespStatus();
				if (me->respStatus == 401 && currUserName != 0 && currPassword != 0)
				{
					DEL_CLASS(cli);
					cli = Net::HTTPClient::CreateClient(me->core->GetSocketFactory(), me->ssl, me->userAgent->ToCString(), me->noShutdown, currURL->StartsWith(UTF8STRC("https://")));
					if (cli->Connect(currURL->ToCString(), currMeth, &me->respTimeDNS, &me->respTimeConn, false))
					{
						contType = 0;
						mstm->Clear();
						cli->AddHeaderC(CSTR("Accept"), CSTR("*/*"));
						cli->AddHeaderC(CSTR("Accept-Charset"), CSTR("*"));
						i = (UOSInt)(currPassword->ConcatTo(Text::StrConcatC(currUserName->ConcatTo(buff), UTF8STRC(":"))) - buff);
						Text::StringBuilderUTF8 sbAuth;
						sbAuth.AppendC(UTF8STRC("Basic "));
						Text::TextBinEnc::Base64Enc b64Enc;
						b64Enc.EncodeBin(&sbAuth, buff, i);
						cli->AddHeaderC(CSTR("Authorization"), sbAuth.ToCString());
						
						sptr = me->AppendCookie(sbuff, currURL->v, currURL->leng);
						if (sptr)
						{
							cli->AddHeaderC(CSTR("Cookie"), CSTRP(sbuff, sptr));
						}

						if (currMeth != Net::WebUtil::RequestMethod::HTTP_GET && currBody)
						{
							sptr = Text::StrUOSInt(sbuff, currBodyLen);
							cli->AddHeaderC(CSTR("Content-Length"), CSTRP(sbuff, sptr));
							cli->AddHeaderC(CSTR("Content-Type"), currBodyType->ToCString());
							cli->Write(currBody, currBodyLen);
						}
						if (currHeaders)
						{
							Text::StringBuilderUTF8 sb;
							sb.Append(currHeaders);
							Text::PString sarr[2];
							Text::PString sarr2[2];
							sarr[1] = sb;
							while (true)
							{
								i = Text::StrSplitLineP(sarr, 2, sarr[1]);
								if (Text::StrSplitTrimP(sarr2, 2, sarr[0], ':') == 2)
								{
									cli->AddHeaderC(sarr2[0].ToCString(), sarr2[1].ToCString());
								}

								if (i != 2)
								{
									break;
								}
							}
						}

						cli->EndRequest(&me->respTimeReq, &me->respTimeResp);
						totalRead = 0;
						while ((thisRead = cli->Read(buff, 4096)) > 0)
						{
							mstm->Write(buff, thisRead);
							totalRead += thisRead;
						}
						me->respTimeTotal = cli->GetTotalTime();
						me->respSize = totalRead;
						me->respStatus = cli->GetRespStatus();
					}
					else
					{
						me->respTimeDNS = -1;
						me->respTimeConn = -1;
						me->respTimeReq = -1;
						me->respTimeResp = -1;
						me->respTimeTotal = -1;
						me->respSize = 0;
						me->respStatus = 0;
					}
				}
				me->ClearHeaders();
				i = 0;
				j = cli->GetRespHeaderCnt();
				while (i < j)
				{
					me->respHeaders->Add(cli->GetRespHeader(i)->Clone());
					i++;
				}
				Text::StringBuilderUTF8 sb;
				if (cli->GetRespHeader(CSTR("Content-Type"), &sb))
				{
					contType = Text::String::New(sb.ToString(), sb.GetLength());
				}
				me->respSvrAddr = *cli->GetSvrAddr();
				Sync::MutexUsage respMutUsage(me->respMut);
				SDEL_STRING(me->respReqURL)
				SDEL_STRING(me->respContType);
				SDEL_CLASS(me->respData);
				me->respReqURL = currURL->Clone();
				me->respContType = contType;
				me->respData = mstm;
				SDEL_STRING(me->respCert);
				Crypto::Cert::Certificate *cert = cli->GetServerCert();
				if (cert)
				{
					Text::StringBuilderUTF8 sb;
					cert->ToString(&sb);
					me->respCert = Text::String::New(sb.ToString(), sb.GetLength());
				}
				respMutUsage.EndUse();
			}
			else
			{
				me->respTimeDNS = -1;
				me->respTimeConn = -1;
				me->respTimeReq = -1;
				me->respTimeResp = -1;
				me->respTimeTotal = -1;
				me->respSize = 0;
				me->respStatus = 0;
				Sync::MutexUsage mutUsage(me->respMut);
				SDEL_STRING(me->respReqURL)
				SDEL_CLASS(me->respData);
				SDEL_STRING(me->respContType);
				me->respReqURL = currURL->Clone();
				SDEL_STRING(me->respCert);
				mutUsage.EndUse();
			}

			DEL_CLASS(cli);
			me->respChanged = true;

			currURL->Release();
			if (currBody)
			{
				MemFree((UInt8*)currBody);
				currBody = 0;
			}
			SDEL_STRING(currBodyType);
			SDEL_STRING(currUserName);
			SDEL_STRING(currPassword);
			SDEL_STRING(currHeaders);
		}
		else
		{
			me->threadEvt->Wait(1000);
		}
	}
	MemFree(sbuff);
	SDEL_STRING(me->reqURL);
	SDEL_TEXT(me->reqBody);
	SDEL_STRING(me->reqBodyType);
	me->threadToStop = false;
	me->threadRunning = false;
	return 0;
}

void __stdcall SSWR::AVIRead::AVIRHTTPClientForm::OnTimerTick(void *userObj)
{
	SSWR::AVIRead::AVIRHTTPClientForm *me = (SSWR::AVIRead::AVIRHTTPClientForm*)userObj;
	Text::String *hdr;
	UTF8Char sbuff[64];
	UTF8Char *sptr;
	UOSInt i;
	UOSInt j;
	if (me->respChanged)
	{
		me->txtReqURL->SetText(me->respReqURL->ToCString());
		sptr = Net::SocketUtil::GetAddrName(sbuff, &me->respSvrAddr);
		me->txtSvrIP->SetText(CSTRP(sbuff, sptr));
		if (me->respTimeDNS == -1)
		{
			me->txtTimeDNS->SetText(CSTR("-1"));
		}
		else
		{
			sptr = Text::StrDoubleFmt(sbuff, me->respTimeDNS, "0.0000000000");
			me->txtTimeDNS->SetText(CSTRP(sbuff, sptr));
		}
		if (me->respTimeConn == -1)
		{
			me->txtTimeConn->SetText(CSTR("-1"));
		}
		else
		{
			sptr = Text::StrDoubleFmt(sbuff, me->respTimeConn - me->respTimeDNS, "0.0000000000");
			me->txtTimeConn->SetText(CSTRP(sbuff, sptr));
		}
		if (me->respTimeReq == -1)
		{
			me->txtTimeSendHdr->SetText(CSTR("-1"));
		}
		else
		{
			sptr = Text::StrDoubleFmt(sbuff, me->respTimeReq - me->respTimeConn, "0.0000000000");
			me->txtTimeSendHdr->SetText(CSTRP(sbuff, sptr));
		}
		if (me->respTimeResp == -1)
		{
			me->txtTimeResp->SetText(CSTR("-1"));
		}
		else
		{
			sptr = Text::StrDoubleFmt(sbuff, me->respTimeResp - me->respTimeReq, "0.0000000000");
			me->txtTimeResp->SetText(CSTRP(sbuff, sptr));
		}
		if (me->respTimeTotal == -1)
		{
			me->txtTimeTotal->SetText(CSTR("-1"));
		}
		else
		{
			sptr = Text::StrDoubleFmt(sbuff, me->respTimeTotal - me->respTimeResp, "0.0000000000");
			me->txtTimeTotal->SetText(CSTRP(sbuff, sptr));
		}
		sptr = Text::StrUInt64(sbuff, me->respSize);
		me->txtRespSize->SetText(CSTRP(sbuff, sptr));
		sptr = Text::StrInt32(sbuff, me->respStatus);
		me->txtRespStatus->SetText(CSTRP(sbuff, sptr));

		me->lvHeaders->ClearItems();
		i = 0;
		j = me->respHeaders->GetCount();
		while (i < j)
		{
			hdr = me->respHeaders->GetItem(i);
			if (hdr->StartsWithICase(UTF8STRC("Set-Cookie: ")))
			{
				SSWR::AVIRead::AVIRHTTPClientForm::HTTPCookie *cookie = me->SetCookie(hdr->ToCString().Substring(12), me->respReqURL->ToCString());
				if (cookie)
				{
					UOSInt k = me->lvCookie->AddItem(cookie->domain, cookie);
					if (cookie->path)
					{
						me->lvCookie->SetSubItem(k, 1, cookie->path);
					}
					me->lvCookie->SetSubItem(k, 2, cookie->name);
					me->lvCookie->SetSubItem(k, 3, cookie->value);
				}
			}
			me->lvHeaders->AddItem(hdr, 0);
			i++;
		}
		if (me->respCert)
		{
			me->txtCert->SetText(me->respCert->ToCString());
		}
		else
		{
			me->txtCert->SetText(CSTR(""));
		}
		me->respChanged = false;
		me->tcMain->SetSelectedIndex(1);
	}
}

void SSWR::AVIRead::AVIRHTTPClientForm::ClearHeaders()
{
	LIST_FREE_STRING(this->respHeaders);
}

void SSWR::AVIRead::AVIRHTTPClientForm::ClearParams()
{
	UOSInt i;
	SSWR::AVIRead::AVIRHTTPClientForm::ParamValue *param;
	i = this->params->GetCount();
	while (i-- > 0)
	{
		param = this->params->GetItem(i);
		param->name->Release();
		param->value->Release();
		MemFree(param);
	}
	this->params->Clear();
}

void SSWR::AVIRead::AVIRHTTPClientForm::ClearCookie()
{
	UOSInt i;
	SSWR::AVIRead::AVIRHTTPClientForm::HTTPCookie *cookie;
	i = this->cookieList->GetCount();
	while (i-- > 0)
	{
		cookie = this->cookieList->GetItem(i);
		cookie->name->Release();
		cookie->value->Release();
		cookie->domain->Release();
		SDEL_STRING(cookie->path);
		MemFree(cookie);
	}
	this->cookieList->Clear();

}

void SSWR::AVIRead::AVIRHTTPClientForm::ClearFiles()
{
	UOSInt i = this->fileList->GetCount();
	while (i-- > 0)
	{
		this->fileList->GetItem(i)->Release();
	}
	this->fileList->Clear();
}

SSWR::AVIRead::AVIRHTTPClientForm::HTTPCookie *SSWR::AVIRead::AVIRHTTPClientForm::SetCookie(Text::CString cookieStr, Text::CString reqURL)
{
	UTF8Char domain[512];
	UTF8Char path[512];
	UTF8Char *pathEnd;
	Text::PString sarr[2];
	UTF8Char *cookieValue;
	UOSInt cnt;
	UOSInt i;
	Bool secure = false;
	Int64 expiryTime = 0;
	Bool valid = true;
	path[0] = 0;
	pathEnd = path;
	UTF8Char *domainEnd = Text::URLString::GetURLDomain(domain, reqURL.v, reqURL.leng, 0);
	Text::StringBuilderUTF8 sb;
	sb.Append(cookieStr);
	cnt = Text::StrSplitTrimP(sarr, 2, sb, ';');
	cookieValue = sarr[0].v;
	i = Text::StrIndexOfCharC(cookieValue, sarr[0].leng, '=');
	if (i == INVALID_INDEX)
	{
		return 0;
	}
	while (cnt == 2)
	{
		cnt = Text::StrSplitTrimP(sarr, 2, sarr[1], ';');
		if (Text::StrEqualsC(sarr[0].v, sarr[0].leng, UTF8STRC("Secure")))
		{
			secure = true;
		}
		else if (sarr[0].Equals(UTF8STRC("HttpOnly")))
		{

		}
		else if (sarr[0].StartsWith(UTF8STRC("SameSite=")))
		{

		}
		else if (sarr[0].StartsWith(UTF8STRC("Expires=")))
		{
			Data::DateTime dt;
			dt.SetValue(&sarr[0].v[8], sarr[0].leng - 8);
			expiryTime = dt.ToTicks();
		}
		else if (sarr[0].StartsWith(UTF8STRC("Max-Age=")))
		{
			Data::DateTime dt;
			dt.SetCurrTimeUTC();
			dt.AddSecond(Text::StrToOSInt(&sarr[0].v[8]));
			expiryTime = dt.ToTicks();
		}
		else if (sarr[0].StartsWith(UTF8STRC("Domain=")))
		{
			if (Text::StrEqualsICaseC(domain, (UOSInt)(domainEnd - domain), &sarr[0].v[7], sarr[0].leng - 7))
			{

			}
			else
			{
				UOSInt len1 = (UOSInt)(domainEnd - domain);
				UOSInt len2 = sarr[0].leng - 7;
				if (len1 > len2 && len2 > 0 && domain[len1 - len2 - 1] == '.' && Text::StrEquals(&domain[len1 - len2], &sarr[0].v[7]))
				{
					Text::StrConcatC(domain, &sarr[0].v[7], len2);
				}
				else if (len1 + 1 == len2 && sarr[0].v[7] == '.' && Text::StrEqualsC(domain, len1, &sarr[0].v[8], len2 - 1))
				{
					Text::StrConcatC(domain, &sarr[0].v[7], len2);
				}
				else
				{
					valid = false;
				}
			}
		}
		else if (sarr[0].StartsWith(UTF8STRC("Path=")))
		{
			pathEnd = Text::StrConcatC(path, &sarr[0].v[5], sarr[0].leng - 5);
		}
	}
	if (valid)
	{
		Text::String *cookieName = Text::String::New(cookieValue, (UOSInt)i);
		SSWR::AVIRead::AVIRHTTPClientForm::HTTPCookie *cookie;
		Bool eq;
		UOSInt j = this->cookieList->GetCount();
		while (j-- > 0)
		{
			cookie = this->cookieList->GetItem(j);
			eq = Text::StrEquals(cookie->domain->v, domain) && cookie->secure == secure && cookie->name->Equals(cookieName);
			if (cookie->path == 0)
			{
				eq = eq && (path[0] == 0);
			}
			else
			{
				eq = eq && cookie->path->Equals(path, (UOSInt)(pathEnd - path));
			}
			if (eq)
			{
				Sync::MutexUsage mutUsage(this->cookieMut);
				SDEL_STRING(cookie->value);
				cookie->value  = Text::String::NewNotNull(&cookieValue[i + 1]);
				mutUsage.EndUse();
				cookieName->Release();
				return cookie;
			}
		}
		cookie = MemAlloc(SSWR::AVIRead::AVIRHTTPClientForm::HTTPCookie, 1);
		cookie->domain = Text::String::NewNotNull(domain);
		if (path[0])
		{
			cookie->path = Text::String::New(path, (UOSInt)(pathEnd - path));
		}
		else
		{
			cookie->path = 0;
		}
		cookie->secure = secure;
		cookie->expireTime = expiryTime;
		cookie->name = cookieName;
		cookie->value = Text::String::NewNotNull(&cookieValue[i + 1]);
		Sync::MutexUsage mutUsage(this->cookieMut);
		this->cookieList->Add(cookie);
		mutUsage.EndUse();
		return cookie;
	}
	else
	{
		return 0;
	}
}

UTF8Char *SSWR::AVIRead::AVIRHTTPClientForm::AppendCookie(UTF8Char *sbuff, const UTF8Char *reqURL, UOSInt urlLen)
{
	UInt8 buff[4096];
	UTF8Char *sptr;
	HTTPCookie *cookie;
	UOSInt len1;
	UOSInt len2;
	UOSInt i;
	UOSInt j;
	UTF8Char *cookiePtr = 0;
	UTF8Char *pathPtr;
	UTF8Char *pathPtrEnd;
	sptr = Text::URLString::GetURLDomain(buff, reqURL, urlLen, 0);
	pathPtr = sptr + 1;
	pathPtrEnd = Text::URLString::GetURLPath(pathPtr, reqURL, urlLen);
	len1 = (UOSInt)(sptr - buff);;
	Sync::MutexUsage mutUsage(this->cookieMut);
	i = 0;
	j = this->cookieList->GetCount();
	while (i < j)
	{
		cookie = this->cookieList->GetItem(i);
		len2 = cookie->domain->leng;
		Bool valid = false;
		if (len1 == len2 && cookie->domain->Equals(buff, len1))
		{
			valid = true;
		}
		else if (len1 > len2 && buff[len1 - len2 - 1] == '.' && Text::StrEquals(&buff[len1 - len2], cookie->domain->v))
		{
			valid = true;
		}
		else if (len1 + 1 == len2 && cookie->domain->v[0] == '.' && Text::StrEquals(buff, &cookie->domain->v[1]))
		{
			valid = true;
		}
		if (valid)
		{
			if (cookie->path == 0 || Text::StrStartsWithC(pathPtr, (UOSInt)(pathPtrEnd - pathPtr), cookie->path->v, cookie->path->leng))
			{
				if (cookiePtr == 0)
				{
					cookiePtr = cookie->name->ConcatTo(sbuff);
				}
				else
				{
					cookiePtr = Text::StrConcatC(cookiePtr, UTF8STRC("; "));
					cookiePtr = cookie->name->ConcatTo(cookiePtr);
				}
				cookiePtr = Text::StrConcatC(cookiePtr, UTF8STRC("="));
				cookiePtr = cookie->value->ConcatTo(cookiePtr);
			}
		}
		i++;
	}
	mutUsage.EndUse();
	return cookiePtr;
}

SSWR::AVIRead::AVIRHTTPClientForm::AVIRHTTPClientForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetFont(0, 0, 8.25, false);
	this->SetText(CSTR("HTTP Client"));

	this->core = core;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
	this->sockf = core->GetSocketFactory();
	this->ssl = Net::SSLEngineFactory::Create(this->sockf, true);
	Net::HTTPClient::PrepareSSL(this->ssl);
	this->respChanged = false;
	this->threadRunning = false;
	this->threadToStop = false;
	this->reqURL = 0;
	this->reqBody = 0;
	this->reqBodyLen = 0;
	this->reqBodyType = 0;
	this->reqUserName = 0;
	this->reqPassword = 0;
	this->reqHeaders = 0;
	NEW_CLASS(this->threadEvt, Sync::Event(true));
	NEW_CLASS(this->respHeaders, Data::ArrayList<Text::String *>());
	NEW_CLASS(this->respMut, Sync::Mutex());
	NEW_CLASS(this->params, Data::ArrayList<SSWR::AVIRead::AVIRHTTPClientForm::ParamValue*>());
	NEW_CLASS(this->cookieList, Data::ArrayList<SSWR::AVIRead::AVIRHTTPClientForm::HTTPCookie*>());
	NEW_CLASS(this->cookieMut, Sync::Mutex());
	NEW_CLASS(this->fileList, Data::ArrayList<Text::String*>());
	this->respContType = 0;
	this->respReqURL = 0;
	this->respData = 0;
	this->respCert = 0;
	this->userAgent = Text::String::New(UTF8STRC("SSWR/1.0"));

	NEW_CLASS(this->tcMain, UI::GUITabControl(ui, this));
	this->tcMain->SetDockType(UI::GUIControl::DOCK_FILL);

	this->tpRequest = this->tcMain->AddTabPage(CSTR("Request"));
	NEW_CLASS(this->pnlRequest, UI::GUIPanel(ui, this->tpRequest));
	this->pnlRequest->SetRect(0, 0, 100, 292, false);
	this->pnlRequest->SetDockType(UI::GUIControl::DOCK_TOP);
	NEW_CLASS(this->lblURL, UI::GUILabel(ui, this->pnlRequest, CSTR("URL")));
	this->lblURL->SetRect(4, 4, 100, 23, false);
	NEW_CLASS(this->txtURL, UI::GUITextBox(ui, this->pnlRequest, CSTR("http://")));
	this->txtURL->SetRect(104, 4, 400, 23, false);
	NEW_CLASS(this->chkNoShutdown, UI::GUICheckBox(ui, this->pnlRequest, CSTR("No Shutdown"), true));
	this->chkNoShutdown->SetRect(504, 4, 100, 23, false);
	NEW_CLASS(this->lblMethod, UI::GUILabel(ui, this->pnlRequest, CSTR("Method")));
	this->lblMethod->SetRect(4, 28, 100, 23, false);
	NEW_CLASS(cboMethod, UI::GUIComboBox(ui, this->pnlRequest, false));
	this->cboMethod->SetRect(104, 28, 100, 23, false);
	this->cboMethod->AddItem(CSTR("GET"), (void*)Net::WebUtil::RequestMethod::HTTP_GET);
	this->cboMethod->AddItem(CSTR("POST"), (void*)Net::WebUtil::RequestMethod::HTTP_POST);
	this->cboMethod->AddItem(CSTR("PUT"), (void*)Net::WebUtil::RequestMethod::HTTP_PUT);
	this->cboMethod->AddItem(CSTR("PATCH"), (void*)Net::WebUtil::RequestMethod::HTTP_PATCH);
	this->cboMethod->AddItem(CSTR("DELETE"), (void*)Net::WebUtil::RequestMethod::HTTP_DELETE);
	this->cboMethod->SetSelectedIndex(0);
	NEW_CLASS(this->chkOSClient, UI::GUICheckBox(ui, this->pnlRequest, CSTR("OS Client"), false));
	this->chkOSClient->SetRect(204, 28, 100, 23, false);
	NEW_CLASS(this->btnUserAgent, UI::GUIButton(ui, this->pnlRequest, CSTR("User Agent")));
	this->btnUserAgent->SetRect(4, 52, 75, 23, false);
	this->btnUserAgent->HandleButtonClick(OnUserAgentClicked, this);
	NEW_CLASS(this->lblUserAgent, UI::GUILabel(ui, this->pnlRequest, this->userAgent->ToCString()));
	this->lblUserAgent->SetRect(104, 52, 400, 23, false);
	NEW_CLASS(this->lblUserName, UI::GUILabel(ui, this->pnlRequest, CSTR("UserName")));
	this->lblUserName->SetRect(4, 76, 100, 23, false);
	NEW_CLASS(this->txtUserName, UI::GUITextBox(ui, this->pnlRequest, CSTR("")));
	this->txtUserName->SetRect(104, 76, 150, 23, false);
	NEW_CLASS(this->lblPassword, UI::GUILabel(ui, this->pnlRequest, CSTR("Password")));
	this->lblPassword->SetRect(4, 100, 100, 23, false);
	NEW_CLASS(this->txtPassword, UI::GUITextBox(ui, this->pnlRequest, CSTR("")));
	this->txtPassword->SetPasswordChar('*');
	this->txtPassword->SetRect(104, 100, 150, 23, false);
	NEW_CLASS(this->lblFileUpload, UI::GUILabel(ui, this->pnlRequest, CSTR("File Upload")));
	this->lblFileUpload->SetRect(4, 124, 100, 23, false);
	NEW_CLASS(this->txtFileFormName, UI::GUITextBox(ui, this->pnlRequest, CSTR("")));
	this->txtFileFormName->SetRect(104, 124, 150, 23, false);
	NEW_CLASS(this->btnFileSelect, UI::GUIButton(ui, this->pnlRequest, CSTR("Select")));
	this->btnFileSelect->SetRect(254, 124, 75, 23, false);
	this->btnFileSelect->HandleButtonClick(OnFileSelectClicked, this);
	NEW_CLASS(this->btnFileClear, UI::GUIButton(ui, this->pnlRequest, CSTR("Clear")));
	this->btnFileClear->SetRect(334, 124, 75, 23, false);
	this->btnFileClear->HandleButtonClick(OnFileClearClicked, this);
	NEW_CLASS(this->lblFileStatus, UI::GUILabel(ui, this->pnlRequest, CSTR("No files selected")));
	this->lblFileStatus->SetRect(414, 124, 200, 23, false);
	NEW_CLASS(this->lblDataStr, UI::GUILabel(ui, this->pnlRequest, CSTR("Data String")));
	this->lblDataStr->SetRect(4, 148, 100, 23, false);
	NEW_CLASS(this->txtDataStr, UI::GUITextBox(ui, this->pnlRequest, CSTR("")));
	this->txtDataStr->SetRect(104, 148, 400, 23, false);
	NEW_CLASS(this->btnDataStr, UI::GUIButton(ui, this->pnlRequest, CSTR("Parse")));
	this->btnDataStr->SetRect(504, 148, 75, 23, false);
	this->btnDataStr->HandleButtonClick(OnDataStrClicked, this);
	NEW_CLASS(this->lblPostFormat, UI::GUILabel(ui, this->pnlRequest, CSTR("Post Format")));
	this->lblPostFormat->SetRect(4, 172, 100, 23, false);
	NEW_CLASS(this->cboPostFormat, UI::GUIComboBox(ui, this->pnlRequest, false));
	this->cboPostFormat->SetRect(104, 172, 150, 23, false);
	this->cboPostFormat->AddItem(CSTR("application/x-www-form-urlencoded"), 0);
	this->cboPostFormat->AddItem(CSTR("application/json"), 0);
	this->cboPostFormat->AddItem(CSTR("RAW"), 0);
	this->cboPostFormat->SetSelectedIndex(0);
	NEW_CLASS(this->lblHeaders, UI::GUILabel(ui, this->pnlRequest, CSTR("Headers")));
	this->lblHeaders->SetRect(4, 196, 100, 23, false);
	NEW_CLASS(this->txtHeaders, UI::GUITextBox(ui, this->pnlRequest, CSTR(""), true));
	this->txtHeaders->SetRect(104, 196, 300, 71, false);
	NEW_CLASS(this->btnRequest, UI::GUIButton(ui, this->pnlRequest, CSTR("Request")));
	this->btnRequest->SetRect(104, 268, 75, 23, false);
	this->btnRequest->HandleButtonClick(OnRequestClicked, this);
	NEW_CLASS(this->lvReqData, UI::GUIListView(ui, this->tpRequest, UI::GUIListView::LVSTYLE_TABLE, 2));
	this->lvReqData->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvReqData->AddColumn(CSTR("Name"), 150);
	this->lvReqData->AddColumn(CSTR("Value"), 400);

	this->tpResponse = this->tcMain->AddTabPage(CSTR("Response"));
	NEW_CLASS(this->pnlResponse, UI::GUIPanel(ui, this->tpResponse));
	this->pnlResponse->SetRect(0, 0, 100, 223, false);
	this->pnlResponse->SetDockType(UI::GUIControl::DOCK_TOP);
	NEW_CLASS(this->lblReqURL, UI::GUILabel(ui, this->pnlResponse, CSTR("Req URL")));
	this->lblReqURL->SetRect(4, 4, 100, 23, false);
	NEW_CLASS(this->txtReqURL, UI::GUITextBox(ui, this->pnlResponse, CSTR("")));
	this->txtReqURL->SetRect(104, 4, 400, 23, false);
	this->txtReqURL->SetReadOnly(true);
	NEW_CLASS(this->lblSvrIP, UI::GUILabel(ui, this->pnlResponse, CSTR("Server IP")));
	this->lblSvrIP->SetRect(4, 28, 100, 23, false);
	NEW_CLASS(this->txtSvrIP, UI::GUITextBox(ui, this->pnlResponse, CSTR("")));
	this->txtSvrIP->SetRect(104, 28, 150, 23, false);
	this->txtSvrIP->SetReadOnly(true);
	NEW_CLASS(this->lblTimeDNS, UI::GUILabel(ui, this->pnlResponse, CSTR("DNS Time")));
	this->lblTimeDNS->SetRect(4, 52, 100, 23, false);
	NEW_CLASS(this->txtTimeDNS, UI::GUITextBox(ui, this->pnlResponse, CSTR("")));
	this->txtTimeDNS->SetRect(104, 52, 150, 23, false);
	this->txtTimeDNS->SetReadOnly(true);
	NEW_CLASS(this->lblTimeConn, UI::GUILabel(ui, this->pnlResponse, CSTR("Conn Time")));
	this->lblTimeConn->SetRect(4, 76, 100, 23, false);
	NEW_CLASS(this->txtTimeConn, UI::GUITextBox(ui, this->pnlResponse, CSTR("")));
	this->txtTimeConn->SetRect(104, 76, 150, 23, false);
	this->txtTimeConn->SetReadOnly(true);
	NEW_CLASS(this->lblTimeSendHdr, UI::GUILabel(ui, this->pnlResponse, CSTR("Request Time")));
	this->lblTimeSendHdr->SetRect(4, 100, 100, 23, false);
	NEW_CLASS(this->txtTimeSendHdr, UI::GUITextBox(ui, this->pnlResponse, CSTR("")));
	this->txtTimeSendHdr->SetRect(104, 100, 150, 23, false);
	this->txtTimeSendHdr->SetReadOnly(true);
	NEW_CLASS(this->lblTimeResp, UI::GUILabel(ui, this->pnlResponse, CSTR("Response Time")));
	this->lblTimeResp->SetRect(4, 124, 100, 23, false);
	NEW_CLASS(this->txtTimeResp, UI::GUITextBox(ui, this->pnlResponse, CSTR("")));
	this->txtTimeResp->SetRect(104, 124, 150, 23, false);
	this->txtTimeResp->SetReadOnly(true);
	NEW_CLASS(this->lblTimeTotal, UI::GUILabel(ui, this->pnlResponse, CSTR("Download Time")));
	this->lblTimeTotal->SetRect(4, 148, 100, 23, false);
	NEW_CLASS(this->txtTimeTotal, UI::GUITextBox(ui, this->pnlResponse, CSTR("")));
	this->txtTimeTotal->SetRect(104, 148, 150, 23, false);
	this->txtTimeTotal->SetReadOnly(true);
	NEW_CLASS(this->lblRespStatus, UI::GUILabel(ui, this->pnlResponse, CSTR("Status Code")));
	this->lblRespStatus->SetRect(4, 172, 100, 23, false);
	NEW_CLASS(this->txtRespStatus, UI::GUITextBox(ui, this->pnlResponse, CSTR("")));
	this->txtRespStatus->SetRect(104, 172, 150, 23, false);
	this->txtRespStatus->SetReadOnly(true);
	NEW_CLASS(this->lblRespSize, UI::GUILabel(ui, this->pnlResponse, CSTR("Download Size")));
	this->lblRespSize->SetRect(4, 196, 100, 23, false);
	NEW_CLASS(this->txtRespSize, UI::GUITextBox(ui, this->pnlResponse, CSTR("")));
	this->txtRespSize->SetRect(104, 196, 150, 23, false);
	this->txtRespSize->SetReadOnly(true);
	NEW_CLASS(this->pnlControl, UI::GUIPanel(ui, this->tpResponse));
	this->pnlControl->SetRect(0, 0, 100, 31, false);
	this->pnlControl->SetDockType(UI::GUIControl::DOCK_BOTTOM);
	NEW_CLASS(this->btnSave, UI::GUIButton(ui, this->pnlControl, CSTR("Save")));
	this->btnSave->SetRect(4, 4, 75, 23, false);
	this->btnSave->HandleButtonClick(OnSaveClicked, this);
	NEW_CLASS(this->btnView, UI::GUIButton(ui, this->pnlControl, CSTR("View")));
	this->btnView->SetRect(84, 4, 75, 23, false);
	this->btnView->HandleButtonClick(OnViewClicked, this);
	NEW_CLASS(this->lvHeaders, UI::GUIListView(ui, this->tpResponse, UI::GUIListView::LVSTYLE_TABLE, 1));
	this->lvHeaders->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvHeaders->SetShowGrid(true);
	this->lvHeaders->SetFullRowSelect(true);
	this->lvHeaders->AddColumn(CSTR("Header"), 1000);

	this->tpCert = this->tcMain->AddTabPage(CSTR("Cert"));
	NEW_CLASS(this->txtCert, UI::GUITextBox(ui, this->tpCert, CSTR(""), true));
	this->txtCert->SetDockType(UI::GUIControl::DOCK_FILL);
	this->txtCert->SetReadOnly(true);

	this->tpCookie = this->tcMain->AddTabPage(CSTR("Cookie"));
	NEW_CLASS(this->lvCookie, UI::GUIListView(ui, this->tpCookie, UI::GUIListView::LVSTYLE_TABLE, 4));
	this->lvCookie->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvCookie->SetShowGrid(true);
	this->lvCookie->SetFullRowSelect(true);
	this->lvCookie->AddColumn(CSTR("Domain"), 150);
	this->lvCookie->AddColumn(CSTR("Path"), 150);
	this->lvCookie->AddColumn(CSTR("Name"), 150);
	this->lvCookie->AddColumn(CSTR("Value"), 350);

	this->SetDefaultButton(this->btnRequest);
	this->txtURL->Focus();
	Sync::Thread::Create(ProcessThread, this, 0);
	this->AddTimer(100, OnTimerTick, this);
}

SSWR::AVIRead::AVIRHTTPClientForm::~AVIRHTTPClientForm()
{
	this->threadToStop = true;
	this->threadEvt->Set();
	while (this->threadRunning)
	{
		Sync::Thread::Sleep(1);
	}
	DEL_CLASS(this->threadEvt);
	this->ClearHeaders();
	DEL_CLASS(this->respHeaders);
	this->ClearParams();
	DEL_CLASS(this->params);
	this->ClearCookie();
	DEL_CLASS(this->cookieList);
	DEL_CLASS(this->cookieMut);
	this->ClearFiles();
	DEL_CLASS(this->fileList);
	SDEL_STRING(this->respReqURL);
	SDEL_STRING(this->respContType);
	SDEL_CLASS(this->respData);
	SDEL_STRING(this->respCert);
	DEL_CLASS(this->respMut);
	this->userAgent->Release();
	SDEL_CLASS(this->ssl);
}

void SSWR::AVIRead::AVIRHTTPClientForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
