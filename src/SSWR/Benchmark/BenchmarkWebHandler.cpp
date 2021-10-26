#include "Stdafx.h"
#include "IO/FileStream.h"
#include "IO/MemoryStream.h"
#include "IO/Path.h"
#include "Manage/CPUDB.h"
#include "Manage/CPUInfo.h"
#include "SSWR/Benchmark/BenchmarkWebHandler.h"
#include "Text/StringBuilderUTF8.h"
#include "Text/UTF8Reader.h"

#include <stdio.h>

Bool __stdcall SSWR::Benchmark::BenchmarkWebHandler::UploadReq(SSWR::Benchmark::BenchmarkWebHandler *me, Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp)
{
	Bool valid = true;
	if (req->GetReqMethod() != Net::WebServer::IWebRequest::RequestMethod::HTTP_POST)
	{
		valid = false;
	}
	Text::StringBuilderUTF8 sb;
	if (req->GetHeader(&sb, (const UTF8Char*)"Content-Type"))
	{
		if (!sb.Equals((const UTF8Char*)"text/plain"))
		{
			printf("Content-Type invalid\r\n");
			valid = false;
		}
	}
	else
	{
		printf("Content-Type not found\r\n");
		valid = false;
	}
	if (valid)
	{
		UOSInt leng;
		const UInt8 *data;
		data = req->GetReqData(&leng);
		if (leng <= 128 || leng >= 65536)
		{
			printf("leng out of range: %d\r\n", (Int32)leng);
			valid = false;
		}
		else
		{
			const UTF8Char *platform = 0;
			const UTF8Char *cpu = 0;
			IO::MemoryStream *mstm;
			Text::UTF8Reader *reader;
			UTF8Char sbuff[512];
			UTF8Char *sptr;
			NEW_CLASS(mstm, IO::MemoryStream(leng, (const UTF8Char*)"BenchmarkWebHandler.UploadReq.mstm"));
			mstm->Write(data, leng);
			mstm->SeekFromBeginning(0);
			NEW_CLASS(reader, Text::UTF8Reader(mstm));

			sb.ClearStr();
			if (reader->ReadLine(&sb, 512))
			{
				if (!sb.Equals((const UTF8Char*)"SBench Result:"))
				{
					printf("SBench not found\r\n");
					valid = false;
				}
			}
			else
			{
				printf("SBench ended\r\n");
				valid = false;
			}

			if (valid)
			{
				sb.ClearStr();
				if (reader->ReadLine(&sb, 512))
				{
					if (!sb.Equals((const UTF8Char*)"Computer Info:"))
					{
						printf("Computer Info not found\r\n");
						valid = false;
					}
				}
				else
				{
					printf("Computer Info ended\r\n");
					valid = false;
				}
			}

			if (valid)
			{
				sb.ClearStr();
				if (reader->ReadLine(&sb, 512))
				{
					if (sb.StartsWith((const UTF8Char*)"Platform: "))
					{
						platform = Text::StrCopyNew(sb.ToString() + 10);
					}
					else
					{
						printf("Platform not found\r\n");
						valid = false;
					}
				}
				else
				{
					printf("Platform ended\r\n");
					valid = false;
				}
			}

			if (valid)
			{
				sb.ClearStr();
				if (reader->ReadLine(&sb, 512))
				{
					if (sb.StartsWith((const UTF8Char*)"CPU: "))
					{
						cpu = Text::StrCopyNew(sb.ToString() + 5);
					}
					else
					{
						printf("CPU not found\r\n");
						valid = false;
					}
				}
				else
				{
					printf("CPU ended\r\n");
					valid = false;
				}
			}

			DEL_CLASS(reader);
			DEL_CLASS(mstm);

			if (valid)
			{
				IO::FileStream *fs;
				Data::DateTime dt;
				UTF8Char *sptrTmp;
				dt.SetCurrTimeUTC();
				Int64 t = dt.ToTicks();
				IO::Path::GetProcessFileName(sbuff);
				sptr = IO::Path::AppendPath(sbuff, (const UTF8Char*)"Benchmark");
				IO::Path::CreateDirectory(sbuff);
				*sptr++ = IO::Path::PATH_SEPERATOR;
				sptr = Text::StrConcat(sptr, (const UTF8Char*)"SBench_");
				sptrTmp = sptr;
				sptr = Text::StrConcat(sptr, platform);
				Text::StrReplace(sptrTmp, '/', '_');
				Text::StrReplace(sptrTmp, ':', '_');
				sptr = Text::StrConcat(sptr, (const UTF8Char*)"_");
				sptr = Text::StrInt64(sptr, t);
				sptr = Text::StrConcat(sptr, (const UTF8Char*)".txt");

				NEW_CLASS(fs, IO::FileStream(sbuff, IO::FileStream::FileMode::Create, IO::FileStream::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
				if (fs->IsError())
				{
					printf("Error in creating file\r\n");
					valid = false;
				}
				else
				{
					fs->Write(data, leng);
				}
				DEL_CLASS(fs);
			}
			SDEL_TEXT(platform);
			SDEL_TEXT(cpu);
		}
	}
	if (!valid)
	{
		resp->ResponseError(req, Net::WebStatus::SC_INTERNAL_SERVER_ERROR);
		return true;
	}
	resp->AddDefHeaders(req);
	resp->SetStatusCode(Net::WebStatus::SC_OK);
	resp->AddContentLength(2);
	resp->Write((const UInt8*)"ok", 2);
	return true;
}

Bool __stdcall SSWR::Benchmark::BenchmarkWebHandler::CPUInfoReq(SSWR::Benchmark::BenchmarkWebHandler *me, Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp)
{
	const UTF8Char *msg = 0;
	IO::FileStream *fs;
	UTF8Char fileName[512];
	UTF8Char path[512];
	UTF8Char *u8ptr;
	if (req->GetQueryValueStr((const UTF8Char*)"model", fileName, 512))
	{
		UOSInt fileSize;
		IO::Path::GetProcessFileName(path);
		u8ptr = IO::Path::AppendPath(path, (const UTF8Char*)"CPUInfo");
		*u8ptr++ = IO::Path::PATH_SEPERATOR;
		u8ptr = Text::StrConcat(u8ptr, fileName);
		NEW_CLASS(fs, IO::FileStream(path, IO::FileStream::FileMode::ReadOnly, IO::FileStream::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
		fileSize = (UOSInt)fs->GetLength();
		if (fileSize > 0)
		{
			UInt8 *fileBuff = MemAlloc(UInt8, fileSize);
			fs->Read(fileBuff, fileSize);
			DEL_CLASS(fs);

			resp->SetStatusCode(Net::WebStatus::SC_OK);
			resp->AddDefHeaders(req);
			resp->AddContentType((const UTF8Char*)"text/plain");
			resp->AddContentLength(fileSize);
			resp->Write(fileBuff, fileSize);
			MemFree(fileBuff);
			return true;
		}
		else
		{
			DEL_CLASS(fs);
		}
	}
	if (req->GetReqMethod() == Net::WebServer::IWebRequest::RequestMethod::HTTP_POST)
	{
		Int32 cpuFamily = 0;
		Int32 cpuModel = 0;
		Int32 cpuStepping = 0;
		req->GetQueryValueI32((const UTF8Char*)"family", &cpuFamily);
		req->GetQueryValueI32((const UTF8Char*)"modelId", &cpuModel);
		req->GetQueryValueI32((const UTF8Char*)"stepping", &cpuStepping);
		if (cpuFamily != 0 && cpuModel != 0 && cpuStepping != 0)
		{
			req->GetHeader(fileName, (const UTF8Char*)"Content-Length", 512);
			UOSInt reqSize;
			const UInt8 *reqData = req->GetReqData(&reqSize);
			if (reqSize > 0 && reqSize <= 128)
			{
				IO::Path::GetProcessFileName(path);
				u8ptr = IO::Path::AppendPath(path, (const UTF8Char*)"X86CPUInfo.txt");
				NEW_CLASS(fs, IO::FileStream(path, IO::FileStream::FileMode::Append, IO::FileStream::FileShare::DenyNone, IO::FileStream::BufferType::Normal));

				u8ptr = Text::StrInt32(fileName, cpuFamily);
				*u8ptr++ = '\t';
				u8ptr = Text::StrInt32(u8ptr, cpuModel);
				*u8ptr++ = '\t';
				u8ptr = Text::StrInt32(u8ptr, cpuStepping);
				*u8ptr++ = '\t';
				fs->Write(fileName, (UOSInt)(u8ptr - fileName));
				fs->Write(reqData, reqSize);
				fs->Write((const UInt8*)"\r\n", 2);
				DEL_CLASS(fs);

				resp->SetStatusCode(Net::WebStatus::SC_OK);
				resp->AddDefHeaders(req);
				resp->AddContentType((const UTF8Char*)"text/html; charset=UTF-8");
				resp->AddContentLength(2);
				resp->Write((const UInt8*)"ok", 2);
				return true;
			}
		}
		else
		{
			UOSInt fileSize;
			const UInt8 *fileBuff;
			if (req->GetQueryValueStr((const UTF8Char*)"file", fileName, 512))
			{
				fileBuff = req->GetReqData(&fileSize);
				Text::StrConcat(fileName, (const UTF8Char*)"cpuinfo");
			}
			else
			{
				req->ParseHTTPForm();
				fileBuff = req->GetHTTPFormFile((const UTF8Char*)"uploadfile", 0, fileName, sizeof(fileName), &fileSize);
			}
			if (fileBuff == 0)
			{
				msg = (const UTF8Char*)"Upload file not found";
			}
			else if (fileSize <= 0 || fileSize > 65536)
			{
				msg = (const UTF8Char*)"File size invalid";
			}
			else if (!Text::StrEquals(fileName, (const UTF8Char*)"cpuinfo"))
			{
				msg = (const UTF8Char*)"File name invalid";
			}
			else
			{
				IO::MemoryStream *mstm;
				NEW_CLASS(mstm, IO::MemoryStream(fileSize, (const UTF8Char*)"SSWR.Benchmark.BenchmarkWebHandler.CPUInfoReq.mstm"));
				mstm->Write(fileBuff, fileSize);
				mstm->SeekFromBeginning(0);
				const UTF8Char *cpuModel = Manage::CPUDB::ParseCPUInfo(mstm);
				DEL_CLASS(mstm);

				if (cpuModel)
				{
					msg = fileName;
					Text::StrConcat(Text::StrConcat(fileName, (const UTF8Char*)"Identified as "), cpuModel);

					IO::Path::GetProcessFileName(path);
					u8ptr = IO::Path::AppendPath(path, (const UTF8Char*)"CPUInfo");
					IO::Path::CreateDirectory(path);
					*u8ptr++ = IO::Path::PATH_SEPERATOR;
					u8ptr = Text::StrConcat(u8ptr, cpuModel);

					if (IO::Path::GetPathType(path) == IO::Path::PathType::Unknown)
					{
						NEW_CLASS(fs, IO::FileStream(path, IO::FileStream::FileMode::Create, IO::FileStream::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
						if (fileSize == fs->Write(fileBuff, fileSize))
						{
						}
						DEL_CLASS(fs);
					}
				}
				else
				{
					Data::DateTime dt;
					dt.SetCurrTimeUTC();
					IO::Path::GetProcessFileName(path);
					u8ptr = IO::Path::AppendPath(path, (const UTF8Char*)"CPUInfo");
					IO::Path::CreateDirectory(path);
					*u8ptr++ = IO::Path::PATH_SEPERATOR;
					u8ptr = Text::StrConcat(u8ptr, (const UTF8Char*)"Unknown_");
					u8ptr = Text::StrInt64(u8ptr, dt.ToTicks());

					NEW_CLASS(fs, IO::FileStream(path, IO::FileStream::FileMode::Create, IO::FileStream::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
					if (fileSize == fs->Write(fileBuff, fileSize))
					{
						msg = (const UTF8Char*)"File uploaded successfully";
					}
					else
					{
						msg = (const UTF8Char*)"Error in storing file";
					}
					DEL_CLASS(fs);
				}
			}
		}
	}

	Text::StringBuilderUTF8 sbOut;

	sbOut.Append((const UTF8Char*)"<html><head><title>CPUInfo</title></head>\r\n");
	sbOut.Append((const UTF8Char*)"<body>\r\n");
	sbOut.Append((const UTF8Char*)"<h2>CPUInfo</h2>\r\n");
	sbOut.Append((const UTF8Char*)"<form name=\"upload\" method=\"POST\" action=\"cpuinfo\" enctype=\"multipart/form-data\">");
	sbOut.Append((const UTF8Char*)"Upload /proc/cpuinfo: <input type=\"file\" name=\"uploadfile\"/><br/><input type=\"submit\"/>");
	sbOut.Append((const UTF8Char*)"</form>");
	if (msg)
	{
		sbOut.Append(msg);
	}

	sbOut.Append((const UTF8Char*)"<h3>Uploaded list</h3>\r\n");
	sbOut.Append((const UTF8Char*)"<table border=\"1\">\r\n");
	sbOut.Append((const UTF8Char*)"<tr><td>Model</td><td>Brand</td><td>Name</td><td>Archtecture</td></tr>\r\n");
	IO::Path::GetProcessFileName(path);
	u8ptr = IO::Path::AppendPath(path, (const UTF8Char*)"CPUInfo");
	IO::Path::CreateDirectory(path);
	*u8ptr++ = IO::Path::PATH_SEPERATOR;
	Text::StrConcat(u8ptr, IO::Path::ALL_FILES);
	IO::Path::FindFileSession *sess = IO::Path::FindFile(path);
	if (sess)
	{
		IO::Path::PathType pt;
		while (IO::Path::FindNextFile(u8ptr, sess, 0, &pt, 0))
		{
			if (pt == IO::Path::PathType::File)
			{
				if (Text::StrStartsWith(u8ptr, (const UTF8Char*)"Unknown"))
				{
					sbOut.Append((const UTF8Char*)"<tr><td>");
					sbOut.Append((const UTF8Char*)"<a href=\"cpuinfo?model=");
					sbOut.Append(u8ptr);
					sbOut.Append((const UTF8Char*)"\">Unknown</a></td><td>");
					sbOut.Append((const UTF8Char*)"?</td><td>?</td><td>?");
					sbOut.Append((const UTF8Char*)"</td></tr>\r\n");
				}
				else
				{
					const Manage::CPUDB::CPUSpec *cpu = Manage::CPUDB::GetCPUSpec(u8ptr);
					sbOut.Append((const UTF8Char*)"<tr><td>");
					sbOut.Append((const UTF8Char*)"<a href=\"cpuinfo?model=");
					sbOut.Append(u8ptr);
					sbOut.Append((const UTF8Char*)"\">");
					sbOut.Append(u8ptr);
					sbOut.Append((const UTF8Char*)"</a></td><td>");
					if (cpu)
					{
						sbOut.Append(Manage::CPUVendor::GetBrandName(cpu->brand));
						sbOut.Append((const UTF8Char*)"</td><td>");
						if (cpu->name)
						{
							sbOut.Append((const UTF8Char*)cpu->name);
						}
						sbOut.Append((const UTF8Char*)"</td><td>");
						switch (cpu->contextType)
						{
						case Manage::ThreadContext::CT_ARM:
							sbOut.Append((const UTF8Char*)"ARM 32-bit");
							break;
						case Manage::ThreadContext::CT_ARM64:
							sbOut.Append((const UTF8Char*)"ARM 64-bit");
							break;
						case Manage::ThreadContext::CT_MIPS:
							sbOut.Append((const UTF8Char*)"MIPS 32-bit");
							break;
						case Manage::ThreadContext::CT_MIPS64:
							sbOut.Append((const UTF8Char*)"MIPS 64-bit");
							break;
						case Manage::ThreadContext::CT_X86_32:
							sbOut.Append((const UTF8Char*)"x86 32-bit");
							break;
						case Manage::ThreadContext::CT_X86_64:
							sbOut.Append((const UTF8Char*)"x86 64-bit");
							break;
						case Manage::ThreadContext::CT_AVR:
							sbOut.Append((const UTF8Char*)"AVR");
							break;
						default:
							sbOut.Append((const UTF8Char*)"?");
							break;
						}
					}
					else
					{
						printf("CPU not found: \"%s\"\r\n", u8ptr);
						sbOut.Append((const UTF8Char*)"?</td><td>?</td><td>?");
					}
					sbOut.Append((const UTF8Char*)"</td></tr>\r\n");
				}
			}
		}
		IO::Path::FindFileClose(sess);
	}
	sbOut.Append((const UTF8Char*)"</table>\r\n");

	sbOut.Append((const UTF8Char*)"</body>\r\n");
	sbOut.Append((const UTF8Char*)"</html>");

	resp->SetStatusCode(Net::WebStatus::SC_OK);
	resp->AddDefHeaders(req);
	resp->AddContentType((const UTF8Char*)"text/html; charset=UTF-8");
	resp->AddContentLength(sbOut.GetLength());
	resp->Write(sbOut.ToString(), sbOut.GetLength());
	return true;
}

Bool SSWR::Benchmark::BenchmarkWebHandler::ProcessRequest(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, const UTF8Char *subReq)
{
	if (this->DoRequest(req, resp, subReq))
	{
		return true;
	}
	RequestHandler reqHdlr = this->reqMap->Get(subReq);
	if (reqHdlr)
	{
		return reqHdlr(this, req, resp);
	}
	resp->ResponseError(req, Net::WebStatus::SC_NOT_FOUND);
	return true;
}

SSWR::Benchmark::BenchmarkWebHandler::BenchmarkWebHandler()
{
	NEW_CLASS(this->reqMap, Data::StringUTF8Map<RequestHandler>());
	this->reqMap->Put((const UTF8Char*)"/upload", UploadReq);
	this->reqMap->Put((const UTF8Char*)"/cpuinfo", CPUInfoReq);
}

SSWR::Benchmark::BenchmarkWebHandler::~BenchmarkWebHandler()
{
	DEL_CLASS(this->reqMap);
}
