#include "Stdafx.h"
#include "IO/FileStream.h"
#include "IO/MemoryStream.h"
#include "IO/Path.h"
#include "Manage/CPUDB.h"
#include "SSWR/Benchmark/BenchmarkTest.h"
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
	if (req->GetHeaderC(&sb, UTF8STRC("Content-Type")))
	{
		if (!sb.EqualsC(UTF8STRC("text/plain")))
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
			printf("leng out of range: %d\r\n", (UInt32)leng);
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
			NEW_CLASS(mstm, IO::MemoryStream(leng, UTF8STRC("BenchmarkWebHandler.UploadReq.mstm")));
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
				sptr = Text::StrConcatC(sptr, UTF8STRC("SBench_"));
				sptrTmp = sptr;
				sptr = Text::StrConcat(sptr, platform);
				Text::StrReplace(sptrTmp, '/', '_');
				Text::StrReplace(sptrTmp, ':', '_');
				sptr = Text::StrConcatC(sptr, UTF8STRC("_"));
				sptr = Text::StrInt64(sptr, t);
				sptr = Text::StrConcatC(sptr, UTF8STRC(".txt"));

				NEW_CLASS(fs, IO::FileStream(sbuff, IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
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
	UTF8Char *u8ptr2;
	if (req->GetQueryValueStr(UTF8STRC("model"), fileName, 512))
	{
		UOSInt fileSize;
		IO::Path::GetProcessFileName(path);
		u8ptr = IO::Path::AppendPath(path, (const UTF8Char*)"CPUInfo");
		*u8ptr++ = IO::Path::PATH_SEPERATOR;
		u8ptr = Text::StrConcat(u8ptr, fileName);
		NEW_CLASS(fs, IO::FileStream(path, IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
		fileSize = (UOSInt)fs->GetLength();
		if (fileSize > 0)
		{
			UInt8 *fileBuff = MemAlloc(UInt8, fileSize);
			fs->Read(fileBuff, fileSize);
			DEL_CLASS(fs);

			resp->SetStatusCode(Net::WebStatus::SC_OK);
			resp->AddDefHeaders(req);
			resp->AddContentType(UTF8STRC("text/plain"));
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
		req->GetQueryValueI32(UTF8STRC("family"), &cpuFamily);
		req->GetQueryValueI32(UTF8STRC("modelId"), &cpuModel);
		req->GetQueryValueI32(UTF8STRC("stepping"), &cpuStepping);
		if (cpuFamily != 0 && cpuModel != 0 && cpuStepping != 0)
		{
			req->GetHeader(fileName, (const UTF8Char*)"Content-Length", 512);
			UOSInt reqSize;
			const UInt8 *reqData = req->GetReqData(&reqSize);
			if (reqSize > 0 && reqSize <= 128)
			{
				IO::Path::GetProcessFileName(path);
				u8ptr = IO::Path::AppendPath(path, (const UTF8Char*)"X86CPUInfo.txt");
				NEW_CLASS(fs, IO::FileStream(path, IO::FileMode::Append, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));

				u8ptr = Text::StrInt32(fileName, cpuFamily);
				*u8ptr++ = '\t';
				u8ptr = Text::StrInt32(u8ptr, cpuModel);
				*u8ptr++ = '\t';
				u8ptr = Text::StrInt32(u8ptr, cpuStepping);
				*u8ptr++ = '\t';
				fs->Write(fileName, u8ptr - fileName);
				fs->Write(reqData, reqSize);
				fs->Write((const UInt8*)"\r\n", 2);
				DEL_CLASS(fs);

				resp->SetStatusCode(Net::WebStatus::SC_OK);
				resp->AddDefHeaders(req);
				resp->AddContentType(UTF8STRC("text/html; charset=UTF-8"));
				resp->AddContentLength(2);
				resp->Write((const UInt8*)"ok", 2);
				return true;
			}
		}
		else
		{
			UOSInt fileSize;
			const UInt8 *fileBuff;
			if (req->GetQueryValueStr(UTF8STRC("file"), fileName, 512))
			{
				fileBuff = req->GetReqData(&fileSize);
				Text::StrConcatC(fileName, UTF8STRC("cpuinfo"));
			}
			else
			{
				req->ParseHTTPForm();
				fileBuff = req->GetHTTPFormFile((const UTF8Char*)"uploadfile", 0, fileName, 512, &fileSize);
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
				NEW_CLASS(mstm, IO::MemoryStream(fileSize, UTF8STRC("SSWR.Benchmark.BenchmarkWebHandler.CPUInfoReq.mstm")));
				mstm->Write(fileBuff, fileSize);
				mstm->SeekFromBeginning(0);
				const UTF8Char *cpuModel = Manage::CPUDB::ParseCPUInfo(mstm);
				DEL_CLASS(mstm);

				if (cpuModel)
				{
					msg = fileName;
					Text::StrConcat(Text::StrConcatC(fileName, UTF8STRC("Identified as ")), cpuModel);

					IO::Path::GetProcessFileName(path);
					u8ptr = IO::Path::AppendPath(path, (const UTF8Char*)"CPUInfo");
					IO::Path::CreateDirectory(path);
					*u8ptr++ = IO::Path::PATH_SEPERATOR;
					u8ptr = Text::StrConcat(u8ptr, cpuModel);

					if (IO::Path::GetPathType(path) == IO::Path::PathType::Unknown)
					{
						NEW_CLASS(fs, IO::FileStream(path, IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
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
					u8ptr = Text::StrConcatC(u8ptr, UTF8STRC("Unknown_"));
					u8ptr = Text::StrInt64(u8ptr, dt.ToTicks());

					NEW_CLASS(fs, IO::FileStream(path, IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
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

	sbOut.AppendC(UTF8STRC("<html><head><title>CPUInfo</title></head>\r\n"));
	sbOut.AppendC(UTF8STRC("<body>\r\n"));
	sbOut.AppendC(UTF8STRC("<h2>CPUInfo</h2>\r\n"));
	sbOut.AppendC(UTF8STRC("<form name=\"upload\" method=\"POST\" action=\"cpuinfo\" enctype=\"multipart/form-data\">"));
	sbOut.AppendC(UTF8STRC("Upload /proc/cpuinfo: <input type=\"file\" name=\"uploadfile\"/><br/><input type=\"submit\"/>"));
	sbOut.AppendC(UTF8STRC("</form>"));
	if (msg)
	{
		sbOut.Append(msg);
	}

	sbOut.AppendC(UTF8STRC("<h3>Uploaded list</h3>\r\n"));
	sbOut.AppendC(UTF8STRC("<table border=\"1\">\r\n"));
	sbOut.AppendC(UTF8STRC("<tr><td>Model</td><td>Brand</td><td>Name</td><td>Archtecture</td></tr>\r\n"));
	IO::Path::GetProcessFileName(path);
	u8ptr = IO::Path::AppendPath(path, (const UTF8Char*)"CPUInfo");
	IO::Path::CreateDirectory(path);
	*u8ptr++ = IO::Path::PATH_SEPERATOR;
	Text::StrConcatC(u8ptr, IO::Path::ALL_FILES, IO::Path::ALL_FILES_LEN);
	IO::Path::FindFileSession *sess = IO::Path::FindFile(path);
	if (sess)
	{
		IO::Path::PathType pt;
		while ((u8ptr2 = IO::Path::FindNextFile(u8ptr, sess, 0, &pt, 0)) != 0)
		{
			if (pt == IO::Path::PathType::File)
			{
				if (Text::StrStartsWithC(u8ptr, (UOSInt)(u8ptr2 - u8ptr), UTF8STRC("Unknown")))
				{
					sbOut.AppendC(UTF8STRC("<tr><td>"));
					sbOut.AppendC(UTF8STRC("<a href=\"cpuinfo?model="));
					sbOut.Append(u8ptr);
					sbOut.AppendC(UTF8STRC("\">Unknown</a></td><td>"));
					sbOut.AppendC(UTF8STRC("?</td><td>?</td><td>?"));
					sbOut.AppendC(UTF8STRC("</td></tr>\r\n"));
				}
				else
				{
					const Manage::CPUDB::CPUSpec *cpu = Manage::CPUDB::GetCPUSpec(u8ptr);
					sbOut.AppendC(UTF8STRC("<tr><td>"));
					sbOut.AppendC(UTF8STRC("<a href=\"cpuinfo?model="));
					sbOut.Append(u8ptr);
					sbOut.AppendC(UTF8STRC("\">"));
					sbOut.Append(u8ptr);
					sbOut.AppendC(UTF8STRC("</a></td><td>"));
					if (cpu)
					{
						sbOut.Append(Manage::CPUVendor::GetBrandName(cpu->brand));
						sbOut.AppendC(UTF8STRC("</td><td>"));
						if (cpu->name)
						{
							sbOut.Append((const UTF8Char*)cpu->name);
						}
						sbOut.AppendC(UTF8STRC("</td><td>"));
						switch (cpu->contextType)
						{
						case Manage::ThreadContext::CT_ARM:
							sbOut.AppendC(UTF8STRC("ARM 32-bit"));
							break;
						case Manage::ThreadContext::CT_ARM64:
							sbOut.AppendC(UTF8STRC("ARM 64-bit"));
							break;
						case Manage::ThreadContext::CT_MIPS:
							sbOut.AppendC(UTF8STRC("MIPS 32-bit"));
							break;
						case Manage::ThreadContext::CT_X86_32:
							sbOut.AppendC(UTF8STRC("x86 32-bit"));
							break;
						case Manage::ThreadContext::CT_X86_64:
							sbOut.AppendC(UTF8STRC("x86 64-bit"));
							break;
						default:
							sbOut.AppendC(UTF8STRC("?"));
							break;
						}
					}
					else
					{
						printf("CPU not found: \"%s\"\r\n", u8ptr);
						sbOut.AppendC(UTF8STRC("?</td><td>?</td><td>?"));
					}
					sbOut.AppendC(UTF8STRC("</td></tr>\r\n"));
				}
			}
		}
		IO::Path::FindFileClose(sess);
	}
	sbOut.AppendC(UTF8STRC("</table>\r\n"));

	sbOut.AppendC(UTF8STRC("</body>\r\n"));
	sbOut.AppendC(UTF8STRC("</html>"));

	resp->SetStatusCode(Net::WebStatus::SC_OK);
	resp->AddDefHeaders(req);
	resp->AddContentType(UTF8STRC("text/html; charset=UTF-8"));
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
