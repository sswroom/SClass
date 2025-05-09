#include "Stdafx.h"
#include "Data/Compress/Inflater.h"
#include "IO/FileStream.h"
#include "IO/Path.h"
#include "IO/StmData/MemoryDataRef.h"
#include "Manage/HiResClock.h"
#include "Net/HTTPClient.h"
#include "Net/HTTPOSClient.h"
#include "Net/MIME.h"
#include "Net/SSLEngineFactory.h"
#include "SSWR/AVIRead/AVIRHTTPClientForm.h"
#include "SSWR/AVIRead/AVIRSSLCertKeyForm.h"
#include "SSWR/AVIRead/AVIRUserAgentSelForm.h"
#include "Sync/MutexUsage.h"
#include "Sync/SimpleThread.h"
#include "Sync/ThreadUtil.h"
#include "Text/MIMEObject.h"
#include "Text/JSONBuilder.h"
#include "Text/MyString.h"
#include "Text/MyStringFloat.h"
#include "Text/URLString.h"
#include "Text/TextBinEnc/Base64Enc.h"
#include "Text/TextBinEnc/FormEncoding.h"
#include "UI/GUIFileDialog.h"

void __stdcall SSWR::AVIRead::AVIRHTTPClientForm::OnUserAgentClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRHTTPClientForm> me = userObj.GetNN<SSWR::AVIRead::AVIRHTTPClientForm>();
	SSWR::AVIRead::AVIRUserAgentSelForm frm(0, me->ui, me->core, me->userAgent->ToCString());
	if (frm.ShowDialog(me))
	{
		me->userAgent->Release();
		me->userAgent = Text::String::New(frm.GetUserAgent());
		me->lblUserAgent->SetText(me->userAgent->ToCString());
	}
}

void __stdcall SSWR::AVIRead::AVIRHTTPClientForm::OnRequestClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRHTTPClientForm> me = userObj.GetNN<SSWR::AVIRead::AVIRHTTPClientForm>();
	UTF8Char sbuffLocal[512];
	UnsafeArray<UTF8Char> sbuff = sbuffLocal;
	UnsafeArrayOpt<UTF8Char> sbuffPtr = 0;
	UOSInt sbuffLen = sizeof(sbuffLocal);
	UnsafeArray<UTF8Char> sptr;
	Text::StringBuilderUTF8 sb;
	Text::StringBuilderUTF8 sbTmp;
	Text::CStringNN mime;
	me->txtURL->GetText(sb);
	if (!sb.StartsWith(UTF8STRC("http://")) && !sb.StartsWith(UTF8STRC("https://")))
	{
		me->ui->ShowMsgOK(CSTR("Please enter valid http URL"), CSTR("Request"), me);
		return;
	}

	sbTmp.ClearStr();
	if (me->txtUserName->GetText(sbTmp) && sbTmp.GetCharCnt() > 0)
	{
		me->reqUserName = Text::String::New(sbTmp.ToCString()).Ptr();
	}
	sbTmp.ClearStr();
	if (me->txtPassword->GetText(sbTmp) && sbTmp.GetCharCnt() > 0)
	{
		me->reqPassword = Text::String::New(sbTmp.ToCString()).Ptr();
	}
	sbTmp.ClearStr();
	if (me->txtHeaders->GetText(sbTmp) && sbTmp.GetCharCnt() > 0)
	{
		me->reqHeaders = Text::String::New(sbTmp.ToCString()).Ptr();
	}


	me->noShutdown = me->chkNoShutdown->IsChecked();
	me->reqMeth = (Net::WebUtil::RequestMethod)me->cboMethod->GetSelectedItem().GetOSInt();
	me->reqOSClient = me->chkOSClient->IsChecked();
	me->reqAllowComp = me->chkAllowComp->IsChecked();
	if ((me->reqMeth == Net::WebUtil::RequestMethod::HTTP_GET) || (me->reqMeth == Net::WebUtil::RequestMethod::HTTP_DELETE))
	{
		UOSInt i = 0;
		UOSInt j = me->params.GetCount();
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
			NN<SSWR::AVIRead::AVIRHTTPClientForm::ParamValue> param;
			Text::StringBuilderUTF8 sb2;
			while (i < j)
			{
				param = me->params.GetItemNoCheck(i);
				if (sb2.GetCharCnt() > 0)
				{
					sb2.AppendUTF8Char('&');
				}
				sptr = Text::TextBinEnc::FormEncoding::FormEncode(sbuff, param->name->v);
				sb2.AppendP(sbuff, sptr);
				sb2.AppendUTF8Char('=');
				if (param->value->leng * 3 + 1 > sbuffLen)
				{
					if (sbuffPtr.SetTo(sbuff)) MemFreeArr(sbuff);
					sbuffLen = param->value->leng * 3 + 1;
					sbuff = MemAllocArr(UTF8Char, sbuffLen);
					sbuffPtr = sbuff;
				}
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
	else if (me->fileList.GetCount() == 1 && me->cboPostFormat->GetSelectedIndex() == 2)
	{
		NN<Text::String> fileName = Text::String::OrEmpty(me->fileList.GetItem(0));
		UnsafeArray<UTF8Char> sptr;
		{
			IO::FileStream fs(fileName, IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
			me->reqBodyLen = (UOSInt)fs.GetLength();
			me->reqBody = MemAlloc(UInt8, me->reqBodyLen);
			fs.Read(Data::ByteArray((UInt8*)me->reqBody.Ptr(), me->reqBodyLen));
		}
		sptr = IO::Path::GetFileExt(sbuff, fileName->v, fileName->leng);
		if (sptr != sbuff)
		{
			mime = Net::MIME::GetMIMEFromExt(CSTRP(sbuff, sptr));
			me->reqBodyType = Text::String::New(mime.v, mime.leng).Ptr();
		}
		else
		{
			me->reqBodyType = 0;
		}
	}
	else if (me->fileList.GetCount() > 0)
	{
		Text::StringBuilderUTF8 sbBoundary;
		Text::StringBuilderUTF8 sb2;
		Data::DateTime dt;
		dt.SetCurrTimeUTC();
		sbBoundary.AppendC(UTF8STRC("---------------------------Boundary"));
		sbBoundary.AppendI64(dt.ToTicks());
		sb2.AppendC(UTF8STRC("multipart/form-data; boundary="));
		sb2.AppendC(sbBoundary.ToString(), sbBoundary.GetLength());
		me->reqBodyType = Text::String::New(sb2.ToString(), sb2.GetLength()).Ptr();
		IO::MemoryStream mstm;
		UOSInt i = 0;
		UOSInt j = me->params.GetCount();
		UOSInt k;
		NN<SSWR::AVIRead::AVIRHTTPClientForm::ParamValue> param;
		NN<Text::String> s;
		while (i < j)
		{
			param = me->params.GetItemNoCheck(i);
			mstm.Write(CSTR("--").ToByteArray());
			mstm.Write(sbBoundary.ToByteArray());
			mstm.Write(CSTR("\r\nContent-Disposition: form-data; name=\"").ToByteArray());
			sptr = Text::TextBinEnc::FormEncoding::FormEncode(sbuff, param->name->v);
			mstm.Write(CSTRP(sbuff, sptr).ToByteArray());
			mstm.Write(CSTR("\"\r\n\r\n").ToByteArray());
			mstm.Write(param->value->ToByteArray());
			mstm.Write(CSTR("\r\n").ToByteArray());

			i++;
		}

		sb2.ClearStr();
		me->txtFileFormName->GetText(sb2);
		UInt8 fileBuff[4096];
		Data::ArrayIterator<NN<Text::String>> it = me->fileList.Iterator();
		while (it.HasNext())
		{
			s = it.Next();
			UInt64 fileLength;
			UInt64 ofst;
			IO::FileStream fs(s->ToCString(), IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
			fileLength = fs.GetLength();
			if (fileLength > 0 && fileLength < 104857600)
			{
				mstm.Write(CSTR("--").ToByteArray());
				mstm.Write(sbBoundary.ToByteArray());
				mstm.Write(CSTR("\r\nContent-Disposition: form-data; ").ToByteArray());
				if (sb.GetCharCnt() > 0)
				{
					mstm.Write(CSTR("name=\"").ToByteArray());
					sptr = Text::TextBinEnc::FormEncoding::FormEncode(sbuff, sb2.ToString());
					mstm.Write(CSTRP(sbuff, sptr).ToByteArray());
					mstm.Write(CSTR("\"; ").ToByteArray());
				}
				k = s->LastIndexOf(IO::Path::PATH_SEPERATOR);
				mstm.Write(CSTR("filename=\"").ToByteArray());
				sptr = Text::TextBinEnc::FormEncoding::FormEncode(sbuff, &s->v[k + 1]);
				mstm.Write(CSTRP(sbuff, sptr).ToByteArray());
				mstm.Write(CSTR("\"\r\n").ToByteArray());

				sptr = IO::Path::GetFileExt(sbuff, &s->v[k], s->leng - k);
				mime = Net::MIME::GetMIMEFromExt(CSTRP(sbuff, sptr));
				mstm.Write(CSTR("Content-Type: ").ToByteArray());
				mstm.Write(mime.ToByteArray());
				mstm.Write(CSTR("\r\n\r\n").ToByteArray());

				ofst = 0;
				while (ofst < fileLength)
				{
					k = fs.Read(BYTEARR(fileBuff));
					if (k <= 0)
					{
						break;
					}
					mstm.Write(Data::ByteArrayR(fileBuff, k));
					ofst += k;
				}
				mstm.Write(CSTR("\r\n").ToByteArray());
			}
		}
		mstm.Write(CSTR("--").ToByteArray());
		mstm.Write(sbBoundary.ToByteArray());
		mstm.Write(CSTR("--").ToByteArray());

		UOSInt buffSize;
		UnsafeArray<UInt8> reqBuff = mstm.GetBuff(buffSize);

		me->reqBody = MemAlloc(UInt8, buffSize);
		me->reqBodyLen = buffSize;
		MemCopyNO((UInt8*)me->reqBody.Ptr(), reqBuff.Ptr(), buffSize);
	}
	else if (me->cboPostFormat->GetSelectedIndex() == 1)
	{
		UOSInt i = 0;
		UOSInt j = me->params.GetCount();
		if (j > 0)
		{
			NN<SSWR::AVIRead::AVIRHTTPClientForm::ParamValue> param;
			Text::JSONBuilder json(Text::JSONBuilder::OT_OBJECT);
			while (i < j)
			{
				param = me->params.GetItemNoCheck(i);
				json.ObjectAddStrUTF8(param->name->ToCString(), UnsafeArray<const UTF8Char>(param->value->v));
				i++;
			}
			Text::CStringNN js = json.Build();
			me->reqBody = Text::StrCopyNew(js.v).Ptr();
			me->reqBodyLen = js.leng;
			me->reqBodyType = Text::String::New(UTF8STRC("application/json")).Ptr();
		}
		else
		{
			Text::StringBuilderUTF8 sb;
			me->txtDataStr->GetText(sb);
			if (sb.GetLength() > 0 && ((sb.StartsWith('{') && sb.EndsWith('}')) || (sb.StartsWith('[') && sb.EndsWith(']'))))
			{
				me->reqBody = Text::StrCopyNew(sb.v).Ptr();
				me->reqBodyLen = sb.leng;
				me->reqBodyType = Text::String::New(UTF8STRC("application/json")).Ptr();
			}
			else
			{
				me->reqBody = Text::StrCopyNew((const UTF8Char*)"{}").Ptr();
				me->reqBodyLen = 2;
				me->reqBodyType = Text::String::New(UTF8STRC("application/json")).Ptr();
			}
		}
	}
	else
	{
		UOSInt i = 0;
		UOSInt j = me->params.GetCount();
		NN<SSWR::AVIRead::AVIRHTTPClientForm::ParamValue> param;
		Text::StringBuilderUTF8 sb2;
		while (i < j)
		{
			param = me->params.GetItemNoCheck(i);
			if (sb2.GetCharCnt() > 0)
			{
				sb2.AppendUTF8Char('&');
			}
			sptr = Text::TextBinEnc::FormEncoding::FormEncode(sbuff, param->name->v);
			sb2.AppendP(sbuff, sptr);
			sb2.AppendUTF8Char('=');
			if (param->value->leng * 3 + 1 > sbuffLen)
			{
				if (sbuffPtr.SetTo(sbuff)) MemFreeArr(sbuff);
				sbuffLen = param->value->leng * 3 + 1;
				sbuff = MemAlloc(UTF8Char, sbuffLen);
				sbuffPtr = sbuff;
			}
			sptr = Text::TextBinEnc::FormEncoding::FormEncode(sbuff, param->value->v);
			sb2.AppendP(sbuff, sptr);
			i++;
		}
		me->reqBody = Text::StrCopyNew(sb2.ToString()).Ptr();
		me->reqBodyLen = sb2.GetCharCnt();
		me->reqBodyType = Text::String::New(UTF8STRC("application/x-www-form-urlencoded")).Ptr();
	}
	me->reqURL = Text::String::New(sb.ToString(), sb.GetLength()).Ptr();
	me->procThread.Notify();
	if (sbuffPtr.SetTo(sbuff)) MemFreeArr(sbuff);
	while (me->procThread.IsRunning() && me->reqURL.NotNull() && !me->respChanged)
	{
		Sync::SimpleThread::Sleep(1);
	}
}

void __stdcall SSWR::AVIRead::AVIRHTTPClientForm::OnSaveClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRHTTPClientForm> me = userObj.GetNN<SSWR::AVIRead::AVIRHTTPClientForm>();
	if (me->respData.IsNull())
	{
		return;
	}
	NN<UI::GUIFileDialog> dlg = me->ui->NewFileDialog(L"SSWR", L"AVIRead", L"HTTPClientSave", true);
	Data::ArrayIterator<NN<Text::String>> it = me->respHeaders.Iterator();
	while (it.HasNext())
	{
		NN<Text::String> hdr = it.Next();
		if (hdr->StartsWithICase(UTF8STRC("Content-Disposition: ")))
		{
			Text::StringBuilderUTF8 sb;
			sb.AppendC(hdr->v + 21, hdr->leng - 21);
			Text::PString sarr[2];
			UOSInt j;
			sarr[1] = sb;
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
							sarr[0].leng = j + 10;
						}
						dlg->SetFileName(sarr[0].ToCString().Substring(10));
					}
					else
					{
						dlg->SetFileName(sarr[0].ToCString().Substring(9));
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
	if (dlg->ShowDialog(me->GetHandle()))
	{
		Bool succ = false;
		{
			IO::FileStream fs(dlg->GetFileName(), IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
			Sync::MutexUsage mutUsage(me->respMut);
			NN<IO::MemoryStream> respData;
			if (me->respData.SetTo(respData))
			{
				UOSInt buffSize;
				UOSInt writeSize;
				UnsafeArray<UInt8> buff = respData->GetBuff(buffSize);
				writeSize = fs.Write(Data::ByteArrayR(buff, buffSize));
				succ = (writeSize == buffSize);
			}
		}
		if (!succ)
		{
			me->ui->ShowMsgOK(CSTR("Error in storing to file"), CSTR("HTTP Client"), me);
		}
	}
	dlg.Delete();
}

void __stdcall SSWR::AVIRead::AVIRHTTPClientForm::OnViewClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRHTTPClientForm> me = userObj.GetNN<SSWR::AVIRead::AVIRHTTPClientForm>();
	NN<IO::MemoryStream> respData;
	Sync::MutexUsage mutUsage(me->respMut);
	if (me->respData.SetTo(respData))
	{
		UOSInt buffSize;
		UnsafeArray<UInt8> buff = respData->GetBuff(buffSize);
		NN<Text::MIMEObject> mimeObj;
		{
			IO::StmData::MemoryDataRef md(buff, buffSize);
			Text::CStringNN contType;
			NN<Text::String> respContType;
			if (me->respContType.SetTo(respContType))
			{
				contType = respContType->ToCString();
			}
			else
			{
				contType = CSTR("application/octet-stream");
			}
			if (Text::MIMEObject::ParseFromData(md, contType).SetTo(mimeObj))
			{
				me->core->OpenObject(mimeObj);
			}
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRHTTPClientForm::OnDataStrClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRHTTPClientForm> me = userObj.GetNN<SSWR::AVIRead::AVIRHTTPClientForm>();
	Text::StringBuilderUTF8 sb;
	NN<SSWR::AVIRead::AVIRHTTPClientForm::ParamValue> param;
	UnsafeArray<UTF8Char> sbuff;
	UTF8Char sbuffLocal[512];
	UnsafeArrayOpt<UTF8Char> sbuffPtr = 0;
	me->txtDataStr->GetText(sb);
	me->ClearParams();
	me->lvReqData->ClearItems();
	if (sb.GetCharCnt() > 0)
	{
		if (sb.leng > 512)
		{
			sbuff = MemAllocArr(UTF8Char, sb.leng + 1);
			sbuffPtr = sbuff;
		}
		else
		{
			sbuff = sbuffLocal;
		}
		UnsafeArray<UTF8Char> sptr = sb.v;
		UnsafeArray<UTF8Char> sbuffEnd;
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
			param = MemAllocNN(SSWR::AVIRead::AVIRHTTPClientForm::ParamValue);
			if (eqInd != INVALID_INDEX)
			{
				sptr[eqInd] = 0;
				sbuffEnd = Text::TextBinEnc::FormEncoding::FormDecode(sbuff, sptr);
				param->name = Text::String::NewP(sbuff, sbuffEnd);
				sbuffEnd = Text::TextBinEnc::FormEncoding::FormDecode(sbuff, &sptr[eqInd + 1]);
				param->value = Text::String::NewP(sbuff, sbuffEnd);
			}
			else
			{
				sbuffEnd = Text::TextBinEnc::FormEncoding::FormDecode(sbuff, sptr);
				param->name = Text::String::NewP(sbuff, sbuffEnd);
				param->value = Text::String::NewEmpty();
			}
			me->params.Add(param);
			i = me->lvReqData->AddItem(param->name, param);
			me->lvReqData->SetSubItem(i, 1, param->value);

			if (spInd == INVALID_INDEX)
			{
				break;
			}
			sptr = &sptr[spInd + 1];
		}
		if (sbuffPtr.SetTo(sbuff)) MemFreeArr(sbuff);
	}
}

void __stdcall SSWR::AVIRead::AVIRHTTPClientForm::OnFileSelectClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRHTTPClientForm> me = userObj.GetNN<SSWR::AVIRead::AVIRHTTPClientForm>();
	NN<UI::GUIFileDialog> dlg = me->ui->NewFileDialog(L"SSWR", L"AVIRead", L"HTTPClientSelect", false);
	dlg->SetAllowMultiSel(true);
	if (dlg->ShowDialog(me->GetHandle()))
	{
		me->ClearFiles();

		UOSInt i = 0;
		UOSInt j = dlg->GetFileNameCount();
		Optional<Text::String> fileName;
		while (i < j)
		{
			fileName = dlg->GetFileNames(i);
			me->fileList.Add(Text::String::OrEmpty(fileName)->Clone());
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
	dlg.Delete();
}

void __stdcall SSWR::AVIRead::AVIRHTTPClientForm::OnFileClearClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRHTTPClientForm> me = userObj.GetNN<SSWR::AVIRead::AVIRHTTPClientForm>();
	me->ClearFiles();
	me->lblFileStatus->SetText(CSTR("No files selected"));
}

void __stdcall SSWR::AVIRead::AVIRHTTPClientForm::OnCertClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRHTTPClientForm> me = userObj.GetNN<SSWR::AVIRead::AVIRHTTPClientForm>();
	NN<Crypto::Cert::X509File> file;
	if (me->respCert.SetTo(file))
	{
		me->core->OpenObject(file->Clone());
	}
}

void __stdcall SSWR::AVIRead::AVIRHTTPClientForm::OnClientCertClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRHTTPClientForm> me = userObj.GetNN<SSWR::AVIRead::AVIRHTTPClientForm>();
	Data::ArrayListNN<Crypto::Cert::X509Cert> caCerts;
	SSWR::AVIRead::AVIRSSLCertKeyForm frm(0, me->ui, me->core, me->ssl, me->cliCert, me->cliKey, caCerts);
	if (frm.ShowDialog(me) == UI::GUIForm::DR_OK)
	{
		NN<Crypto::Cert::X509Cert> nnCert;
		NN<Crypto::Cert::X509File> nnKey;
		me->cliCert.Delete();
		me->cliKey.Delete();
		me->cliCert = frm.GetCert();
		me->cliKey = frm.GetKey();
		Text::StringBuilderUTF8 sb;
		if (me->cliCert.SetTo(nnCert)) nnCert->ToShortString(sb);
		sb.AppendC(UTF8STRC(", "));
		if (me->cliKey.SetTo(nnKey)) nnKey->ToShortString(sb);
		me->lblClientCert->SetText(sb.ToCString());
	}
}

void __stdcall SSWR::AVIRead::AVIRHTTPClientForm::ProcessThread(NN<Sync::Thread> thread)
{
	NN<SSWR::AVIRead::AVIRHTTPClientForm> me = thread->GetUserObj().GetNN<SSWR::AVIRead::AVIRHTTPClientForm>();
	NN<Text::String> currURL;
	UnsafeArrayOpt<const UTF8Char> currBody;
	UnsafeArray<const UTF8Char> nncurrBody;
	UOSInt currBodyLen;
	Optional<Text::String> currBodyType;
	Optional<Text::String> currUserName;
	Optional<Text::String> currPassword;
	Optional<Text::String> currHeaders;
	NN<Text::String> s;
	NN<Text::String> s2;
	Bool currAllowComp;
	Net::WebUtil::RequestMethod currMeth;
	Bool currOSClient;
	UInt8 buff[4096];
	UnsafeArray<UTF8Char> sbuff;
	UnsafeArray<UTF8Char> sptr;
	UOSInt i;
	sbuff = MemAllocArr(UTF8Char, 65536);
	while (!thread->IsStopping())
	{
		if (me->reqURL.SetTo(currURL) && !me->respChanged)
		{
			currBody = me->reqBody;
			currBodyType = me->reqBodyType;
			currBodyLen = me->reqBodyLen;
			currMeth = me->reqMeth;
			currUserName = me->reqUserName;
			currPassword = me->reqPassword;
			currOSClient = me->reqOSClient;
			currHeaders = me->reqHeaders;
			currAllowComp = me->reqAllowComp;
			me->reqURL = 0;
			me->reqBody = 0;
			me->reqBodyLen = 0;
			me->reqBodyType = 0;
			me->reqUserName = 0;
			me->reqPassword = 0;
			me->reqHeaders = 0;
			me->reqAllowComp = false;
			
			NN<Net::HTTPClient> cli;
			me->respTimeStart = Data::Timestamp::Now();
			cli = Net::HTTPClient::CreateClient(me->core->GetTCPClientFactory(), currOSClient?0:me->ssl, me->userAgent->ToCString(), me->noShutdown, currURL->StartsWith(UTF8STRC("https://")));
			NN<Crypto::Cert::X509Cert> cliCert;
			NN<Crypto::Cert::X509File> cliKey;
			if (me->cliCert.SetTo(cliCert) && me->cliKey.SetTo(cliKey))
			{
				cli->SetClientCert(cliCert, cliKey);
			}
			if (cli->Connect(currURL->ToCString(), currMeth, me->respTimeDNS, me->respTimeConn, false))
			{
				IO::MemoryStream *mstm;
				Text::String *contType = 0;
				NEW_CLASS(mstm, IO::MemoryStream());
				cli->AddHeaderC(CSTR("Accept"), CSTR("*/*"));
				cli->AddHeaderC(CSTR("Accept-Language"), CSTR("*"));
				cli->AddHeaderC(CSTR("User-Agent"), me->userAgent->ToCString());
				if (me->noShutdown)
				{
					cli->AddHeaderC(CSTR("Connection"), CSTR("keep-alive"));
				}
				else
				{
					cli->AddHeaderC(CSTR("Connection"), CSTR("close"));
				}
				if (currAllowComp)
				{
					cli->AddHeaderC(CSTR("Accept-Encoding"), CSTR("gzip, deflate"));
				}
				
				if (me->AppendCookie(sbuff, currURL->ToCString()).SetTo(sptr))
				{
					cli->AddHeaderC(CSTR("Cookie"), CSTRP(sbuff, sptr));
				}
				
				if (currHeaders.SetTo(s))
				{
					Text::StringBuilderUTF8 sb;
					sb.Append(s);
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

				NN<Text::String> nncurrBodyType;
				if (currMeth != Net::WebUtil::RequestMethod::HTTP_GET && currBody.SetTo(nncurrBody) && currBodyType.SetTo(nncurrBodyType))
				{
					sptr = Text::StrUOSInt(sbuff, currBodyLen);
					cli->AddHeaderC(CSTR("Content-Length"), CSTRP(sbuff, sptr));
					cli->AddHeaderC(CSTR("Content-Type"), nncurrBodyType->ToCString());
					cli->Write(Data::ByteArrayR(nncurrBody, currBodyLen));
				}

				cli->EndRequest(me->respTimeReq, me->respTimeResp);
				UInt64 totalRead = 0;
				UOSInt thisRead;
				while ((thisRead = cli->Read(BYTEARR(buff))) > 0)
				{
					mstm->Write(Data::ByteArrayR(buff, thisRead));
					totalRead += thisRead;
				}
				me->respTimeTotal = cli->GetTotalTime();
				me->respTransfSize = totalRead;
				me->respULSize = cli->GetTotalUpload();
				me->respDLSize = cli->GetTotalDownload();
				me->respStatus = cli->GetRespStatus();
				if (me->respStatus == 401 && currUserName.SetTo(s) && currPassword.SetTo(s2))
				{
					cli.Delete();
					cli = Net::HTTPClient::CreateClient(me->core->GetTCPClientFactory(), me->ssl, me->userAgent->ToCString(), me->noShutdown, currURL->StartsWith(UTF8STRC("https://")));
					if (cli->Connect(currURL->ToCString(), currMeth, me->respTimeDNS, me->respTimeConn, false))
					{
						contType = 0;
						mstm->Clear();
						cli->AddHeaderC(CSTR("Accept"), CSTR("*/*"));
						cli->AddHeaderC(CSTR("Accept-Charset"), CSTR("*"));
						i = (UOSInt)(s2->ConcatTo(Text::StrConcatC(s->ConcatTo(buff), UTF8STRC(":"))) - buff);
						Text::StringBuilderUTF8 sbAuth;
						sbAuth.AppendC(UTF8STRC("Basic "));
						Text::TextBinEnc::Base64Enc b64Enc;
						b64Enc.EncodeBin(sbAuth, buff, i);
						cli->AddHeaderC(CSTR("Authorization"), sbAuth.ToCString());
						
						if (me->AppendCookie(sbuff, currURL->ToCString()).SetTo(sptr))
						{
							cli->AddHeaderC(CSTR("Cookie"), CSTRP(sbuff, sptr));
						}

						if (currMeth != Net::WebUtil::RequestMethod::HTTP_GET && currBody.SetTo(nncurrBody) && currBodyType.SetTo(nncurrBodyType))
						{
							sptr = Text::StrUOSInt(sbuff, currBodyLen);
							cli->AddHeaderC(CSTR("Content-Length"), CSTRP(sbuff, sptr));
							cli->AddHeaderC(CSTR("Content-Type"), nncurrBodyType->ToCString());
							cli->Write(Data::ByteArrayR(nncurrBody, currBodyLen));
						}
						if (currHeaders.SetTo(s))
						{
							Text::StringBuilderUTF8 sb;
							sb.Append(s);
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

						cli->EndRequest(me->respTimeReq, me->respTimeResp);
						totalRead = 0;
						while ((thisRead = cli->Read(BYTEARR(buff))) > 0)
						{
							mstm->Write(Data::ByteArrayR(buff, thisRead));
							totalRead += thisRead;
						}
						me->respTimeTotal = cli->GetTotalTime();
						me->respTransfSize = totalRead;
						me->respULSize = cli->GetTotalUpload();
						me->respDLSize = cli->GetTotalDownload();
						me->respStatus = cli->GetRespStatus();
					}
					else
					{
						me->respTimeDNS = -1;
						me->respTimeConn = -1;
						me->respTimeReq = -1;
						me->respTimeResp = -1;
						me->respTimeTotal = -1;
						me->respTransfSize = 0;
						me->respULSize = 0;
						me->respDLSize = 0;
						me->respStatus = 0;
					}
				}
				me->ClearHeaders();
				Data::ArrayIterator<NN<Text::String>> it = cli->RespHeaderIterator();
				while (it.HasNext())
				{
					me->respHeaders.Add(it.Next()->Clone());
				}
				Text::StringBuilderUTF8 sb;
				if (cli->GetRespHeader(CSTR("Content-Type"), sb))
				{
					contType = Text::String::New(sb.ToString(), sb.GetLength()).Ptr();
				}
				sb.ClearStr();
				if (cli->GetRespHeader(CSTR("Content-Encoding"), sb))
				{
					if (sb.Equals(UTF8STRC("gzip")))
					{
						UOSInt respSize;
						UnsafeArray<const UInt8> respData = mstm->GetBuff(respSize);
						if (respSize > 16 && respData[0] == 0x1F && respData[1] == 0x8B && respData[2] == 0x8)
						{
							NN<IO::MemoryStream> mstm2;
							NEW_CLASSNN(mstm2, IO::MemoryStream(ReadUInt32(&respData[respSize - 4])));
							Data::Compress::Inflater inflate(mstm2, false);
							i = 10;
//							IO::StmData::MemoryDataRef mdata(&respData[i], respSize - i - 8);
							if (inflate.Write(Data::ByteArrayR(&respData[i], respSize - i - 8)) == respSize - i - 8)
							{
								DEL_CLASS(mstm);
								mstm = mstm2.Ptr();
							}
							else
							{
								mstm2.Delete();
							}
						}
					}
				}
				me->respSvrAddr = cli->GetSvrAddr().Ptr()[0];
				Sync::MutexUsage respMutUsage(me->respMut);
				OPTSTR_DEL(me->respReqURL)
				OPTSTR_DEL(me->respContType);
				me->respData.Delete();
				me->respReqURL = currURL->Clone();
				me->respContType = contType;
				me->respData = mstm;
				OPTSTR_DEL(me->respCertText);
				me->respCert.Delete();
				NN<const Data::ReadingListNN<Crypto::Cert::Certificate>> certs;
				if (cli->GetServerCerts().SetTo(certs))
				{
					Text::StringBuilderUTF8 sb;
					NN<Crypto::Cert::X509File> x509;
					if (Crypto::Cert::X509File::CreateFromCerts(certs).SetTo(x509))
					{
						x509->ToString(sb);
						me->respCert = x509;
						me->respCertText = Text::String::New(sb.ToString(), sb.GetLength()).Ptr();
					}
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
				me->respTransfSize = 0;
				me->respULSize = 0;
				me->respDLSize = 0;
				me->respStatus = 0;
				Sync::MutexUsage mutUsage(me->respMut);
				OPTSTR_DEL(me->respReqURL)
				me->respData.Delete();
				OPTSTR_DEL(me->respContType);
				me->respReqURL = currURL->Clone();
				OPTSTR_DEL(me->respCertText);
				me->respCert.Delete();
			}

			cli.Delete();
			me->respChanged = true;

			currURL->Release();
			if (currBody.SetTo(nncurrBody))
			{
				MemFreeArr(nncurrBody);
				currBody = 0;
			}
			OPTSTR_DEL(currBodyType);
			OPTSTR_DEL(currUserName);
			OPTSTR_DEL(currPassword);
			OPTSTR_DEL(currHeaders);
		}
		else
		{
			thread->Wait(1000);
		}
	}
	MemFreeArr(sbuff);
	OPTSTR_DEL(me->reqURL);
	SDEL_TEXT(me->reqBody);
	OPTSTR_DEL(me->reqBodyType);
}

void __stdcall SSWR::AVIRead::AVIRHTTPClientForm::OnTimerTick(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRHTTPClientForm> me = userObj.GetNN<SSWR::AVIRead::AVIRHTTPClientForm>();
	NN<Text::String> hdr;
	UTF8Char sbuff[64];
	UnsafeArray<UTF8Char> sptr;
	NN<Text::String> s;
	UOSInt i;
	UOSInt j;
	NN<IO::MemoryStream> respData;
	if (me->respChanged)
	{
		me->txtReqURL->SetText(Text::String::OrEmpty(me->respReqURL)->ToCString());
		if (!Net::SocketUtil::GetAddrName(sbuff, me->respSvrAddr).SetTo(sptr))
		{
			me->txtSvrIP->SetText(CSTR(""));
		}
		else
		{
			me->txtSvrIP->SetText(CSTRP(sbuff, sptr));
		}
		sptr = me->respTimeStart.ToStringNoZone(sbuff);
		me->txtStartTime->SetText(CSTRP(sbuff, sptr));
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
		sptr = Text::StrUInt64(sbuff, me->respDLSize);
		me->txtRespDLSize->SetText(CSTRP(sbuff, sptr));
		sptr = Text::StrUInt64(sbuff, me->respULSize);
		me->txtRespULSize->SetText(CSTRP(sbuff, sptr));
		sptr = Text::StrUInt64(sbuff, me->respTransfSize);
		me->txtRespTransfSize->SetText(CSTRP(sbuff, sptr));
		if (me->respData.SetTo(respData))
		{
			sptr = Text::StrUInt64(sbuff, respData->GetLength());
			me->txtRespContSize->SetText(CSTRP(sbuff, sptr));
		}
		else
		{
			me->txtRespContSize->SetText(CSTR("-"));
		}
		sptr = Text::StrInt32(sbuff, me->respStatus);
		me->txtRespStatus->SetText(CSTRP(sbuff, sptr));

		me->lvHeaders->ClearItems();
		i = 0;
		j = me->respHeaders.GetCount();
		while (i < j)
		{
			hdr = Text::String::OrEmpty(me->respHeaders.GetItem(i));
			if (hdr->StartsWithICase(UTF8STRC("Set-Cookie: ")))
			{
				NN<SSWR::AVIRead::AVIRHTTPClientForm::HTTPCookie> cookie;
				if (me->SetCookie(hdr->ToCString().Substring(12), Text::String::OrEmpty(me->respReqURL)->ToCString()).SetTo(cookie))
				{
					UOSInt k = me->lvCookie->AddItem(cookie->domain, cookie);
					if (cookie->path.SetTo(s))
					{
						me->lvCookie->SetSubItem(k, 1, s);
					}
					me->lvCookie->SetSubItem(k, 2, cookie->name);
					me->lvCookie->SetSubItem(k, 3, cookie->value);
				}
			}
			me->lvHeaders->AddItem(hdr, 0);
			i++;
		}
		me->txtCert->SetText(Text::String::OrEmpty(me->respCertText)->ToCString());
		me->btnCert->SetEnabled(me->respCert != 0);
		me->respChanged = false;
		me->tcMain->SetSelectedIndex(1);
	}
}

void SSWR::AVIRead::AVIRHTTPClientForm::ClearHeaders()
{
	this->respHeaders.FreeAll();
}

void SSWR::AVIRead::AVIRHTTPClientForm::ClearParams()
{
	UOSInt i;
	NN<SSWR::AVIRead::AVIRHTTPClientForm::ParamValue> param;
	i = this->params.GetCount();
	while (i-- > 0)
	{
		param = this->params.GetItemNoCheck(i);
		param->name->Release();
		param->value->Release();
		MemFreeNN(param);
	}
	this->params.Clear();
}

void SSWR::AVIRead::AVIRHTTPClientForm::ClearCookie()
{
	UOSInt i;
	NN<SSWR::AVIRead::AVIRHTTPClientForm::HTTPCookie> cookie;
	i = this->cookieList.GetCount();
	while (i-- > 0)
	{
		cookie = this->cookieList.GetItemNoCheck(i);
		cookie->name->Release();
		cookie->value->Release();
		cookie->domain->Release();
		OPTSTR_DEL(cookie->path);
		MemFreeNN(cookie);
	}
	this->cookieList.Clear();

}

void SSWR::AVIRead::AVIRHTTPClientForm::ClearFiles()
{
	UOSInt i = this->fileList.GetCount();
	while (i-- > 0)
	{
		OPTSTR_DEL(this->fileList.GetItem(i));
	}
	this->fileList.Clear();
}

Optional<SSWR::AVIRead::AVIRHTTPClientForm::HTTPCookie> SSWR::AVIRead::AVIRHTTPClientForm::SetCookie(Text::CStringNN cookieStr, Text::CStringNN reqURL)
{
	UTF8Char domain[512];
	UTF8Char path[512];
	UnsafeArray<UTF8Char> pathEnd;
	Text::PString sarr[2];
	UnsafeArray<UTF8Char> cookieValue;
	UnsafeArray<UTF8Char> cookieValueEnd;
	UOSInt cnt;
	UOSInt i;
	Bool secure = false;
	Int64 expiryTime = 0;
	Bool valid = true;
	path[0] = 0;
	pathEnd = path;
	UnsafeArray<UTF8Char> domainEnd = Text::URLString::GetURLDomain(domain, reqURL, 0);
	Text::StringBuilderUTF8 sb;
	sb.Append(cookieStr);
	cnt = Text::StrSplitTrimP(sarr, 2, sb, ';');
	cookieValue = sarr[0].v;
	cookieValueEnd = cookieValue + sarr[0].leng;
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
			dt.SetValue(sarr[0].ToCString().Substring(8));
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
					domainEnd = Text::StrConcatC(domain, &sarr[0].v[7], len2);
				}
				else if (len1 + 1 == len2 && sarr[0].v[7] == '.' && Text::StrEqualsC(domain, len1, &sarr[0].v[8], len2 - 1))
				{
					domainEnd = Text::StrConcatC(domain, &sarr[0].v[7], len2);
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
		NN<Text::String> cookieName = Text::String::New(cookieValue, (UOSInt)i);
		NN<SSWR::AVIRead::AVIRHTTPClientForm::HTTPCookie> cookie;
		NN<Text::String> cpath;
		Bool eq;
		UOSInt j = this->cookieList.GetCount();
		while (j-- > 0)
		{
			cookie = this->cookieList.GetItemNoCheck(j);
			eq = cookie->domain->Equals(domain, (UOSInt)(domainEnd - domain)) && cookie->secure == secure && cookie->name->Equals(cookieName);
			if (!cookie->path.SetTo(cpath))
			{
				eq = eq && (path[0] == 0);
			}
			else
			{
				eq = eq && cpath->Equals(path, (UOSInt)(pathEnd - path));
			}
			if (eq)
			{
				Sync::MutexUsage mutUsage(this->cookieMut);
				cookie->value->Release();
				cookie->value  = Text::String::NewP(&cookieValue[i + 1], cookieValueEnd);
				mutUsage.EndUse();
				cookieName->Release();
				return cookie;
			}
		}
		cookie = MemAllocNN(SSWR::AVIRead::AVIRHTTPClientForm::HTTPCookie);
		cookie->domain = Text::String::NewP(domain, domainEnd);
		if (path[0])
		{
			cookie->path = Text::String::New(path, (UOSInt)(pathEnd - path)).Ptr();
		}
		else
		{
			cookie->path = 0;
		}
		cookie->secure = secure;
		cookie->expireTime = expiryTime;
		cookie->name = cookieName;
		cookie->value = Text::String::NewP(&cookieValue[i + 1], cookieValueEnd);
		Sync::MutexUsage mutUsage(this->cookieMut);
		this->cookieList.Add(cookie);
		mutUsage.EndUse();
		return cookie;
	}
	else
	{
		return 0;
	}
}

UnsafeArrayOpt<UTF8Char> SSWR::AVIRead::AVIRHTTPClientForm::AppendCookie(UnsafeArray<UTF8Char> sbuff, Text::CStringNN reqURL)
{
	UInt8 buff[4096];
	UnsafeArray<UTF8Char> sptr;
	NN<HTTPCookie> cookie;
	UOSInt len1;
	UOSInt len2;
	UOSInt i;
	UOSInt j;
	NN<Text::String> cpath;
	UnsafeArrayOpt<UTF8Char> cookiePtr = 0;
	UnsafeArray<UTF8Char> nncookiePtr;
	UnsafeArray<UTF8Char> pathPtr;
	UnsafeArray<UTF8Char> pathPtrEnd;
	sptr = Text::URLString::GetURLDomain(buff, reqURL, 0);
	pathPtr = sptr + 1;
	pathPtrEnd = Text::URLString::GetURLPath(pathPtr, reqURL);
	len1 = (UOSInt)(sptr - buff);;
	Sync::MutexUsage mutUsage(this->cookieMut);
	i = 0;
	j = this->cookieList.GetCount();
	while (i < j)
	{
		cookie = this->cookieList.GetItemNoCheck(i);
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
			if (!cookie->path.SetTo(cpath) || Text::StrStartsWithC(pathPtr, (UOSInt)(pathPtrEnd - pathPtr), cpath->v, cpath->leng))
			{
				if (!cookiePtr.SetTo(nncookiePtr))
				{
					nncookiePtr = cookie->name->ConcatTo(sbuff);
				}
				else
				{
					nncookiePtr = Text::StrConcatC(nncookiePtr, UTF8STRC("; "));
					nncookiePtr = cookie->name->ConcatTo(nncookiePtr);
				}
				nncookiePtr = Text::StrConcatC(nncookiePtr, UTF8STRC("="));
				cookiePtr = cookie->value->ConcatTo(nncookiePtr);
			}
		}
		i++;
	}
	mutUsage.EndUse();
	return cookiePtr;
}

SSWR::AVIRead::AVIRHTTPClientForm::AVIRHTTPClientForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 1024, 768, ui), procThread(ProcessThread, this, CSTR("HTTPClient"))
{
	this->SetFont(0, 0, 8.25, false);
	this->SetText(CSTR("HTTP Client"));

	this->core = core;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
	this->clif = core->GetTCPClientFactory();
	this->ssl = Net::SSLEngineFactory::Create(this->clif, true);
	Net::HTTPClient::PrepareSSL(this->ssl);
	this->respChanged = false;
	this->cliCert = 0;
	this->cliKey = 0;
	this->reqURL = 0;
	this->reqBody = 0;
	this->reqBodyLen = 0;
	this->reqBodyType = 0;
	this->reqUserName = 0;
	this->reqPassword = 0;
	this->reqHeaders = 0;
	this->respContType = 0;
	this->respReqURL = 0;
	this->respData = 0;
	this->respCertText = 0;
	this->respCert = 0;
	this->userAgent = Text::String::New(UTF8STRC("SSWR/1.0"));
	this->respSvrAddr.addrType = Net::AddrType::Unknown;
	
	this->tcMain = ui->NewTabControl(*this);
	this->tcMain->SetDockType(UI::GUIControl::DOCK_FILL);

	this->tpRequest = this->tcMain->AddTabPage(CSTR("Request"));
	this->pnlRequest = ui->NewPanel(this->tpRequest);
	this->pnlRequest->SetRect(0, 0, 100, 316, false);
	this->pnlRequest->SetDockType(UI::GUIControl::DOCK_TOP);
	this->lblURL = ui->NewLabel(this->pnlRequest, CSTR("URL"));
	this->lblURL->SetRect(4, 4, 100, 23, false);
	this->txtURL = ui->NewTextBox(this->pnlRequest, CSTR("http://"));
	this->txtURL->SetRect(104, 4, 400, 23, false);
	this->chkNoShutdown = ui->NewCheckBox(this->pnlRequest, CSTR("No Shutdown"), true);
	this->chkNoShutdown->SetRect(504, 4, 100, 23, false);
	this->lblMethod = ui->NewLabel(this->pnlRequest, CSTR("Method"));
	this->lblMethod->SetRect(4, 28, 100, 23, false);
	this->cboMethod = ui->NewComboBox(this->pnlRequest, false);
	this->cboMethod->SetRect(104, 28, 100, 23, false);
	this->cboMethod->AddItem(CSTR("GET"), (void*)Net::WebUtil::RequestMethod::HTTP_GET);
	this->cboMethod->AddItem(CSTR("POST"), (void*)Net::WebUtil::RequestMethod::HTTP_POST);
	this->cboMethod->AddItem(CSTR("PUT"), (void*)Net::WebUtil::RequestMethod::HTTP_PUT);
	this->cboMethod->AddItem(CSTR("PATCH"), (void*)Net::WebUtil::RequestMethod::HTTP_PATCH);
	this->cboMethod->AddItem(CSTR("DELETE"), (void*)Net::WebUtil::RequestMethod::HTTP_DELETE);
	this->cboMethod->AddItem(CSTR("TRACE"), (void*)Net::WebUtil::RequestMethod::HTTP_TRACE);
	this->cboMethod->SetSelectedIndex(0);
	this->chkOSClient = ui->NewCheckBox(this->pnlRequest, CSTR("OS Client"), false);
	this->chkOSClient->SetRect(204, 28, 100, 23, false);
	this->chkAllowComp = ui->NewCheckBox(this->pnlRequest, CSTR("Allow ZIP"), true);
	this->chkAllowComp->SetRect(304, 28, 100, 23, false);
	this->btnUserAgent = ui->NewButton(this->pnlRequest, CSTR("User Agent"));
	this->btnUserAgent->SetRect(4, 52, 75, 23, false);
	this->btnUserAgent->HandleButtonClick(OnUserAgentClicked, this);
	this->lblUserAgent = ui->NewLabel(this->pnlRequest, this->userAgent->ToCString());
	this->lblUserAgent->SetRect(104, 52, 400, 23, false);
	this->btnClientCert = ui->NewButton(this->pnlRequest, CSTR("Client Cert"));
	this->btnClientCert->SetRect(4, 76, 75, 23, false);
	this->btnClientCert->HandleButtonClick(OnClientCertClicked, this);
	this->lblClientCert = ui->NewLabel(this->pnlRequest, CSTR(""));
	this->lblClientCert->SetRect(104, 76, 400, 23, false);
	this->lblUserName = ui->NewLabel(this->pnlRequest, CSTR("UserName"));
	this->lblUserName->SetRect(4, 100, 100, 23, false);
	this->txtUserName = ui->NewTextBox(this->pnlRequest, CSTR(""));
	this->txtUserName->SetRect(104, 100, 150, 23, false);
	this->lblPassword = ui->NewLabel(this->pnlRequest, CSTR("Password"));
	this->lblPassword->SetRect(4, 124, 100, 23, false);
	this->txtPassword = ui->NewTextBox(this->pnlRequest, CSTR(""));
	this->txtPassword->SetPasswordChar('*');
	this->txtPassword->SetRect(104, 124, 150, 23, false);
	this->lblFileUpload = ui->NewLabel(this->pnlRequest, CSTR("File Upload"));
	this->lblFileUpload->SetRect(4, 148, 100, 23, false);
	this->txtFileFormName = ui->NewTextBox(this->pnlRequest, CSTR(""));
	this->txtFileFormName->SetRect(104, 148, 150, 23, false);
	this->btnFileSelect = ui->NewButton(this->pnlRequest, CSTR("Select"));
	this->btnFileSelect->SetRect(254, 148, 75, 23, false);
	this->btnFileSelect->HandleButtonClick(OnFileSelectClicked, this);
	this->btnFileClear = ui->NewButton(this->pnlRequest, CSTR("Clear"));
	this->btnFileClear->SetRect(334, 148, 75, 23, false);
	this->btnFileClear->HandleButtonClick(OnFileClearClicked, this);
	this->lblFileStatus = ui->NewLabel(this->pnlRequest, CSTR("No files selected"));
	this->lblFileStatus->SetRect(414, 148, 200, 23, false);
	this->lblDataStr = ui->NewLabel(this->pnlRequest, CSTR("Data String"));
	this->lblDataStr->SetRect(4, 172, 100, 23, false);
	this->txtDataStr = ui->NewTextBox(this->pnlRequest, CSTR(""));
	this->txtDataStr->SetRect(104, 172, 400, 23, false);
	this->btnDataStr = ui->NewButton(this->pnlRequest, CSTR("Parse"));
	this->btnDataStr->SetRect(504, 172, 75, 23, false);
	this->btnDataStr->HandleButtonClick(OnDataStrClicked, this);
	this->lblPostFormat = ui->NewLabel(this->pnlRequest, CSTR("Post Format"));
	this->lblPostFormat->SetRect(4, 196, 100, 23, false);
	this->cboPostFormat = ui->NewComboBox(this->pnlRequest, false);
	this->cboPostFormat->SetRect(104, 196, 150, 23, false);
	this->cboPostFormat->AddItem(CSTR("application/x-www-form-urlencoded"), 0);
	this->cboPostFormat->AddItem(CSTR("application/json"), 0);
	this->cboPostFormat->AddItem(CSTR("RAW"), 0);
	this->cboPostFormat->SetSelectedIndex(0);
	this->lblHeaders = ui->NewLabel(this->pnlRequest, CSTR("Headers"));
	this->lblHeaders->SetRect(4, 220, 100, 23, false);
	this->txtHeaders = ui->NewTextBox(this->pnlRequest, CSTR(""), true);
	this->txtHeaders->SetRect(104, 220, 300, 71, false);
	this->btnRequest = ui->NewButton(this->pnlRequest, CSTR("Request"));
	this->btnRequest->SetRect(104, 292, 75, 23, false);
	this->btnRequest->HandleButtonClick(OnRequestClicked, this);
	this->lvReqData = ui->NewListView(this->tpRequest, UI::ListViewStyle::Table, 2);
	this->lvReqData->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvReqData->AddColumn(CSTR("Name"), 150);
	this->lvReqData->AddColumn(CSTR("Value"), 400);

	this->tpResponse = this->tcMain->AddTabPage(CSTR("Response"));
	this->pnlResponse = ui->NewPanel(this->tpResponse);
	this->pnlResponse->SetRect(0, 0, 100, 319, false);
	this->pnlResponse->SetDockType(UI::GUIControl::DOCK_TOP);
	this->lblReqURL = ui->NewLabel(this->pnlResponse, CSTR("Req URL"));
	this->lblReqURL->SetRect(4, 4, 100, 23, false);
	this->txtReqURL = ui->NewTextBox(this->pnlResponse, CSTR(""));
	this->txtReqURL->SetRect(104, 4, 400, 23, false);
	this->txtReqURL->SetReadOnly(true);
	this->lblSvrIP = ui->NewLabel(this->pnlResponse, CSTR("Server IP"));
	this->lblSvrIP->SetRect(4, 28, 100, 23, false);
	this->txtSvrIP = ui->NewTextBox(this->pnlResponse, CSTR(""));
	this->txtSvrIP->SetRect(104, 28, 150, 23, false);
	this->txtSvrIP->SetReadOnly(true);
	this->lblStartTime = ui->NewLabel(this->pnlResponse, CSTR("Start Time"));
	this->lblStartTime->SetRect(4, 52, 100, 23, false);
	this->txtStartTime = ui->NewTextBox(this->pnlResponse, CSTR(""));
	this->txtStartTime->SetRect(104, 52, 200, 23, false);
	this->txtStartTime->SetReadOnly(true);
	this->lblTimeDNS = ui->NewLabel(this->pnlResponse, CSTR("DNS Time"));
	this->lblTimeDNS->SetRect(4, 76, 100, 23, false);
	this->txtTimeDNS = ui->NewTextBox(this->pnlResponse, CSTR(""));
	this->txtTimeDNS->SetRect(104, 76, 150, 23, false);
	this->txtTimeDNS->SetReadOnly(true);
	this->lblTimeConn = ui->NewLabel(this->pnlResponse, CSTR("Conn Time"));
	this->lblTimeConn->SetRect(4, 100, 100, 23, false);
	this->txtTimeConn = ui->NewTextBox(this->pnlResponse, CSTR(""));
	this->txtTimeConn->SetRect(104, 100, 150, 23, false);
	this->txtTimeConn->SetReadOnly(true);
	this->lblTimeSendHdr = ui->NewLabel(this->pnlResponse, CSTR("Request Time"));
	this->lblTimeSendHdr->SetRect(4, 124, 100, 23, false);
	this->txtTimeSendHdr = ui->NewTextBox(this->pnlResponse, CSTR(""));
	this->txtTimeSendHdr->SetRect(104, 124, 150, 23, false);
	this->txtTimeSendHdr->SetReadOnly(true);
	this->lblTimeResp = ui->NewLabel(this->pnlResponse, CSTR("Response Time"));
	this->lblTimeResp->SetRect(4, 148, 100, 23, false);
	this->txtTimeResp = ui->NewTextBox(this->pnlResponse, CSTR(""));
	this->txtTimeResp->SetRect(104, 148, 150, 23, false);
	this->txtTimeResp->SetReadOnly(true);
	this->lblTimeTotal = ui->NewLabel(this->pnlResponse, CSTR("Download Time"));
	this->lblTimeTotal->SetRect(4, 172, 100, 23, false);
	this->txtTimeTotal = ui->NewTextBox(this->pnlResponse, CSTR(""));
	this->txtTimeTotal->SetRect(104, 172, 150, 23, false);
	this->txtTimeTotal->SetReadOnly(true);
	this->lblRespStatus = ui->NewLabel(this->pnlResponse, CSTR("Status Code"));
	this->lblRespStatus->SetRect(4, 196, 100, 23, false);
	this->txtRespStatus = ui->NewTextBox(this->pnlResponse, CSTR(""));
	this->txtRespStatus->SetRect(104, 196, 150, 23, false);
	this->txtRespStatus->SetReadOnly(true);
	this->lblRespDLSize = ui->NewLabel(this->pnlResponse, CSTR("Download Size"));
	this->lblRespDLSize->SetRect(4, 220, 100, 23, false);
	this->txtRespDLSize = ui->NewTextBox(this->pnlResponse, CSTR(""));
	this->txtRespDLSize->SetRect(104, 220, 150, 23, false);
	this->txtRespDLSize->SetReadOnly(true);
	this->lblRespULSize = ui->NewLabel(this->pnlResponse, CSTR("Upload Size"));
	this->lblRespULSize->SetRect(4, 244, 100, 23, false);
	this->txtRespULSize = ui->NewTextBox(this->pnlResponse, CSTR(""));
	this->txtRespULSize->SetRect(104, 244, 150, 23, false);
	this->txtRespULSize->SetReadOnly(true);
	this->lblRespTransfSize = ui->NewLabel(this->pnlResponse, CSTR("Transfer Size"));
	this->lblRespTransfSize->SetRect(4, 268, 100, 23, false);
	this->txtRespTransfSize = ui->NewTextBox(this->pnlResponse, CSTR(""));
	this->txtRespTransfSize->SetRect(104, 268, 150, 23, false);
	this->txtRespTransfSize->SetReadOnly(true);
	this->lblRespContSize = ui->NewLabel(this->pnlResponse, CSTR("Content Size"));
	this->lblRespContSize->SetRect(4, 292, 100, 23, false);
	this->txtRespContSize = ui->NewTextBox(this->pnlResponse, CSTR(""));
	this->txtRespContSize->SetRect(104, 292, 150, 23, false);
	this->txtRespContSize->SetReadOnly(true);
	this->pnlControl = ui->NewPanel(this->tpResponse);
	this->pnlControl->SetRect(0, 0, 100, 31, false);
	this->pnlControl->SetDockType(UI::GUIControl::DOCK_BOTTOM);
	this->btnSave = ui->NewButton(this->pnlControl, CSTR("Save"));
	this->btnSave->SetRect(4, 4, 75, 23, false);
	this->btnSave->HandleButtonClick(OnSaveClicked, this);
	this->btnView = ui->NewButton(this->pnlControl, CSTR("View"));
	this->btnView->SetRect(84, 4, 75, 23, false);
	this->btnView->HandleButtonClick(OnViewClicked, this);
	this->lvHeaders = ui->NewListView(this->tpResponse, UI::ListViewStyle::Table, 1);
	this->lvHeaders->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvHeaders->SetShowGrid(true);
	this->lvHeaders->SetFullRowSelect(true);
	this->lvHeaders->AddColumn(CSTR("Header"), 1000);

	this->tpCert = this->tcMain->AddTabPage(CSTR("Cert"));
	this->pnlCert = ui->NewPanel(this->tpCert);
	this->pnlCert->SetRect(0, 0, 100, 31, false);
	this->pnlCert->SetDockType(UI::GUIControl::DOCK_BOTTOM);
	this->btnCert = ui->NewButton(this->pnlCert, CSTR("Open"));
	this->btnCert->SetRect(4, 4, 75, 23, false);
	this->btnCert->HandleButtonClick(OnCertClicked, this);
	this->btnCert->SetEnabled(false);
	this->txtCert = ui->NewTextBox(this->tpCert, CSTR(""), true);
	this->txtCert->SetDockType(UI::GUIControl::DOCK_FILL);
	this->txtCert->SetReadOnly(true);

	this->tpCookie = this->tcMain->AddTabPage(CSTR("Cookie"));
	this->lvCookie = ui->NewListView(this->tpCookie, UI::ListViewStyle::Table, 4);
	this->lvCookie->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvCookie->SetShowGrid(true);
	this->lvCookie->SetFullRowSelect(true);
	this->lvCookie->AddColumn(CSTR("Domain"), 150);
	this->lvCookie->AddColumn(CSTR("Path"), 150);
	this->lvCookie->AddColumn(CSTR("Name"), 150);
	this->lvCookie->AddColumn(CSTR("Value"), 350);

	this->SetDefaultButton(this->btnRequest);
	this->txtURL->Focus();
	this->procThread.Start();
	this->AddTimer(100, OnTimerTick, this);
}

SSWR::AVIRead::AVIRHTTPClientForm::~AVIRHTTPClientForm()
{
	this->procThread.Stop();
	this->ClearHeaders();
	this->ClearParams();
	this->ClearCookie();
	this->ClearFiles();
	OPTSTR_DEL(this->respReqURL);
	OPTSTR_DEL(this->respContType);
	this->respData.Delete();
	OPTSTR_DEL(this->respCertText);
	this->respCert.Delete();
	this->cliCert.Delete();
	this->cliKey.Delete();
	this->userAgent->Release();
	this->ssl.Delete();
}

void SSWR::AVIRead::AVIRHTTPClientForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
