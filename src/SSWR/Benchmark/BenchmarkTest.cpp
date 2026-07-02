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

Bool __stdcall SSWR::Benchmark::BenchmarkWebHandler::UploadReq(NN<SSWR::Benchmark::BenchmarkWebHandler> me, NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp)
{
	Bool valid = true;
	if (req->GetReqMethod() != Net::WebUtil::RequestMethod::HTTP_POST)
	{
		valid = false;
	}
	Text::StringBuilderUTF8 sb;
	if (req->GetHeaderC(sb, CSTR("Content-Type")))
	{
		if (!sb.Equals(UTF8STRC("text/plain")))
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
		UIntOS leng;
		UnsafeArray<const UInt8> data;
		if (!req->GetReqData(leng).SetTo(data) || leng <= 128 || leng >= 65536)
		{
			printf("leng out of range: %d\r\n", (UInt32)leng);
			valid = false;
		}
		else
		{
			UnsafeArrayOpt<const UTF8Char> platform = nullptr;
			UnsafeArrayOpt<const UTF8Char> cpu = nullptr;
			NN<IO::MemoryStream> mstm;
			NN<Text::UTF8Reader> reader;
			UTF8Char sbuff[512];
			UnsafeArray<UTF8Char> sptr;
			NEW_CLASSNN(mstm, IO::MemoryStream(leng));
			mstm->Write(Data::ByteArrayR(data, leng));
			mstm->SeekFromBeginning(0);
			NEW_CLASSNN(reader, Text::UTF8Reader(mstm));

			sb.ClearStr();
			if (reader->ReadLine(sb, 512))
			{
				if (!sb.Equals(UTF8STRC("SBench Result:")))
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
				if (reader->ReadLine(sb, 512))
				{
					if (!sb.Equals(UTF8STRC("Computer Info:")))
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
				if (reader->ReadLine(sb, 512))
				{
					if (sb.StartsWith(UTF8STRC("Platform: ")))
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
				if (reader->ReadLine(sb, 512))
				{
					if (sb.StartsWith(UTF8STRC("CPU: ")))
					{
						cpu = Text::StrCopyNewC(sb.ToString() + 5, sb.GetLength() - 5).Ptr();
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

			reader.Delete();
			mstm.Delete();

			UnsafeArray<const UTF8Char> nnplatform;
			if (valid && platform.SetTo(nnplatform))
			{
				Data::DateTime dt;
				UnsafeArray<UTF8Char> sptrTmp;
				dt.SetCurrTimeUTC();
				Int64 t = dt.ToTicks();
				sptr = IO::Path::GetProcessFileName(sbuff).Or(sbuff);
				sptr = IO::Path::AppendPath(sbuff, sptr, CSTR("Benchmark"));
				IO::Path::CreateDirectory(CSTRP(sbuff, sptr));
				*sptr++ = IO::Path::PATH_SEPERATOR;
				sptr = Text::StrConcatC(sptr, UTF8STRC("SBench_"));
				sptrTmp = sptr;
				sptr = Text::StrConcat(sptr, nnplatform);
				Text::StrReplace(sptrTmp, '/', '_');
				Text::StrReplace(sptrTmp, ':', '_');
				sptr = Text::StrConcatC(sptr, UTF8STRC("_"));
				sptr = Text::StrInt64(sptr, t);
				sptr = Text::StrConcatC(sptr, UTF8STRC(".txt"));

				IO::FileStream fs(CSTRP(sbuff, sptr), IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
				if (fs.IsError())
				{
					printf("Error in creating file\r\n");
					valid = false;
				}
				else
				{
					fs.Write(Data::ByteArrayR(data, leng));
				}
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
	resp->Write(CSTR("ok").ToByteArray());
	return true;
}

Bool __stdcall SSWR::Benchmark::BenchmarkWebHandler::CPUInfoReq(NN<SSWR::Benchmark::BenchmarkWebHandler> me, NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp)
{
	Text::CString msg = nullptr;
	UTF8Char fileName[512];
	UnsafeArray<UTF8Char> fileNameEnd;
	UTF8Char path[512];
	UnsafeArray<UTF8Char> sptr;
	UnsafeArray<UTF8Char> sptr2;
	if (req->GetQueryValueStr(CSTR("model"), fileName, 512).NotNull())
	{
		UIntOS fileSize;
		sptr = IO::Path::GetProcessFileName(path).Or(path);
		sptr = IO::Path::AppendPath(path, sptr, CSTR("CPUInfo"));
		*sptr++ = IO::Path::PATH_SEPERATOR;
		sptr = Text::StrConcat(sptr, fileName);
		IO::FileStream fs(CSTRP(path, sptr), IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
		fileSize = (UIntOS)fs.GetLength();
		if (fileSize > 0)
		{
			UnsafeArray<UInt8> fileBuff = MemAllocArr(UInt8, fileSize);
			fs.Read(Data::ByteArray(fileBuff, fileSize));

			resp->SetStatusCode(Net::WebStatus::SC_OK);
			resp->AddDefHeaders(req);
			resp->AddContentType(CSTR("text/plain"));
			resp->AddContentLength(fileSize);
			resp->Write(Data::ByteArrayR(fileBuff, fileSize));
			MemFreeArr(fileBuff);
			return true;
		}
	}
	if (req->GetReqMethod() == Net::WebUtil::RequestMethod::HTTP_POST)
	{
		Int32 cpuFamily = 0;
		Int32 cpuModel = 0;
		Int32 cpuStepping = 0;
		req->GetQueryValueI32(CSTR("family"), cpuFamily);
		req->GetQueryValueI32(CSTR("modelId"), cpuModel);
		req->GetQueryValueI32(CSTR("stepping"), cpuStepping);
		if (cpuFamily != 0 && cpuModel != 0 && cpuStepping != 0)
		{
			req->GetHeader(fileName, CSTR("Content-Length"), 512);
			UIntOS reqSize;
			UnsafeArray<const UInt8> reqData;
			if (req->GetReqData(reqSize).SetTo(reqData) && reqSize > 0 && reqSize <= 128)
			{
				sptr = IO::Path::GetProcessFileName(path).Or(path);
				sptr = IO::Path::AppendPath(path, sptr, CSTR("X86CPUInfo.txt"));
				IO::FileStream fs(CSTRP(path, sptr), IO::FileMode::Append, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);

				sptr = Text::StrInt32(fileName, cpuFamily);
				*sptr++ = '\t';
				sptr = Text::StrInt32(sptr, cpuModel);
				*sptr++ = '\t';
				sptr = Text::StrInt32(sptr, cpuStepping);
				*sptr++ = '\t';
				fs.Write(Data::ByteArrayR(fileName, sptr - fileName));
				fs.Write(Data::ByteArrayR(reqData, reqSize));
				fs.Write(CSTR("\r\n").ToByteArray());

				resp->SetStatusCode(Net::WebStatus::SC_OK);
				resp->AddDefHeaders(req);
				resp->AddContentType(CSTR("text/html; charset=UTF-8"));
				resp->AddContentLength(2);
				resp->Write(CSTR("ok").ToByteArray());
				return true;
			}
		}
		else
		{
			UIntOS fileSize;
			UnsafeArrayOpt<const UInt8> fileBuff;
			UnsafeArray<const UInt8> nnfileBuff;
			if (req->GetQueryValueStr(CSTR("file"), fileName, 512).NotNull())
			{
				fileBuff = req->GetReqData(fileSize);
				fileNameEnd = Text::StrConcatC(fileName, UTF8STRC("cpuinfo"));
			}
			else
			{
				req->ParseHTTPForm();
				fileBuff = req->GetHTTPFormFile(CSTR("uploadfile"), 0, fileName, 512, fileNameEnd, fileSize);
			}
			if (!fileBuff.SetTo(nnfileBuff))
			{
				msg = CSTR("Upload file not found");
			}
			else if (fileSize <= 0 || fileSize > 65536)
			{
				msg = CSTR("File size invalid");
			}
			else if (!Text::StrEquals(fileName, (const UTF8Char*)"cpuinfo"))
			{
				msg = CSTR("File name invalid");
			}
			else
			{
				Text::CString cpuModel;
				{
					IO::MemoryStream mstm(fileSize);
					mstm.Write(Data::ByteArrayR(nnfileBuff, fileSize));
					mstm.SeekFromBeginning(0);
					cpuModel = Manage::CPUDB::ParseCPUInfo(mstm);
				}

				Text::CStringNN nncpuModel;
				if (cpuModel.SetTo(nncpuModel))
				{
					msg = CSTRP(fileName, fileNameEnd);
					nncpuModel.ConcatTo(Text::StrConcatC(fileName, UTF8STRC("Identified as ")));

					sptr = IO::Path::GetProcessFileName(path).Or(path);
					sptr = IO::Path::AppendPath(path, sptr, CSTR("CPUInfo"));
					IO::Path::CreateDirectory(CSTRP(path, sptr));
					*sptr++ = IO::Path::PATH_SEPERATOR;
					sptr = nncpuModel.ConcatTo(sptr);

					if (IO::Path::GetPathType(CSTRP(path, sptr)) == IO::Path::PathType::Unknown)
					{
						IO::FileStream fs(CSTRP(path, sptr), IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
						if (fileSize == fs.Write(Data::ByteArrayR(nnfileBuff, fileSize)))
						{
						}
					}
				}
				else
				{
					Data::DateTime dt;
					dt.SetCurrTimeUTC();
					sptr = IO::Path::GetProcessFileName(path).Or(path);
					sptr = IO::Path::AppendPath(path, sptr, CSTR("CPUInfo"));
					IO::Path::CreateDirectory(CSTRP(path, sptr));
					*sptr++ = IO::Path::PATH_SEPERATOR;
					sptr = Text::StrConcatC(sptr, UTF8STRC("Unknown_"));
					sptr = Text::StrInt64(sptr, dt.ToTicks());

					IO::FileStream fs(CSTRP(path, sptr), IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
					if (fileSize == fs.Write(Data::ByteArrayR(nnfileBuff, fileSize)))
					{
						msg = CSTR("File uploaded successfully");
					}
					else
					{
						msg = CSTR("Error in storing file");
					}
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
	sbOut.AppendOpt(msg);
	sbOut.AppendC(UTF8STRC("<h3>Uploaded list</h3>\r\n"));
	sbOut.AppendC(UTF8STRC("<table border=\"1\">\r\n"));
	sbOut.AppendC(UTF8STRC("<tr><td>Model</td><td>Brand</td><td>Name</td><td>Archtecture</td></tr>\r\n"));
	sptr = IO::Path::GetProcessFileName(path).Or(path);
	sptr = IO::Path::AppendPath(path, sptr, CSTR("CPUInfo"));
	IO::Path::CreateDirectory(CSTRP(path, sptr));
	*sptr++ = IO::Path::PATH_SEPERATOR;
	Text::StrConcatC(sptr, IO::Path::ALL_FILES, IO::Path::ALL_FILES_LEN);
	NN<IO::Path::FindFileSession> sess;
	if (IO::Path::FindFile(CSTRP(path, sptr)).SetTo(sess))
	{
		IO::Path::PathType pt;
		while (IO::Path::FindNextFile(sptr, sess, 0, pt, 0).SetTo(sptr2))
		{
			if (pt == IO::Path::PathType::File)
			{
				if (Text::StrStartsWithC(sptr, (UIntOS)(sptr2 - sptr), UTF8STRC("Unknown")))
				{
					sbOut.AppendC(UTF8STRC("<tr><td>"));
					sbOut.AppendC(UTF8STRC("<a href=\"cpuinfo?model="));
					sbOut.AppendP(sptr, sptr2);
					sbOut.AppendC(UTF8STRC("\">Unknown</a></td><td>"));
					sbOut.AppendC(UTF8STRC("?</td><td>?</td><td>?"));
					sbOut.AppendC(UTF8STRC("</td></tr>\r\n"));
				}
				else
				{
					NN<const Manage::CPUDB::CPUSpec> cpu;
					sbOut.AppendC(UTF8STRC("<tr><td>"));
					sbOut.AppendC(UTF8STRC("<a href=\"cpuinfo?model="));
					sbOut.AppendP(sptr, sptr2);
					sbOut.AppendC(UTF8STRC("\">"));
					sbOut.AppendP(sptr, sptr2);
					sbOut.AppendC(UTF8STRC("</a></td><td>"));
					if (Manage::CPUDB::GetCPUSpec(CSTRP(sptr, sptr2)).SetTo(cpu))
					{
						sbOut.Append(Manage::CPUVendor::GetBrandName(cpu->brand));
						sbOut.AppendC(UTF8STRC("</td><td>"));
						sbOut.AppendSlow(UnsafeArray<const UTF8Char>::ConvertFrom(cpu->name));
						sbOut.AppendC(UTF8STRC("</td><td>"));
						switch (cpu->contextType)
						{
						case Manage::ThreadContext::ContextType::ARM:
							sbOut.AppendC(UTF8STRC("ARM 32-bit"));
							break;
						case Manage::ThreadContext::ContextType::ARM64:
							sbOut.AppendC(UTF8STRC("ARM 64-bit"));
							break;
						case Manage::ThreadContext::ContextType::MIPS:
							sbOut.AppendC(UTF8STRC("MIPS 32-bit"));
							break;
						case Manage::ThreadContext::ContextType::X86_32:
							sbOut.AppendC(UTF8STRC("x86 32-bit"));
							break;
						case Manage::ThreadContext::ContextType::X86_64:
							sbOut.AppendC(UTF8STRC("x86 64-bit"));
							break;
						default:
							sbOut.AppendC(UTF8STRC("?"));
							break;
						}
					}
					else
					{
						printf("CPU not found: \"%s\"\r\n", sptr);
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
	resp->AddContentType(CSTR("text/html; charset=UTF-8"));
	resp->AddContentLength(sbOut.GetLength());
	resp->Write(sbOut.ToByteArray());
	return true;
}

Bool SSWR::Benchmark::BenchmarkWebHandler::ProcessRequest(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp, Text::CStringNN subReq)
{
	if (this->DoRequest(req, resp, subReq))
	{
		return true;
	}
	RequestHandler reqHdlr = this->reqMap.GetC(subReq);
	if (reqHdlr)
	{
		return reqHdlr(*this, req, resp);
	}
	resp->ResponseError(req, Net::WebStatus::SC_NOT_FOUND);
	return true;
}

SSWR::Benchmark::BenchmarkWebHandler::BenchmarkWebHandler()
{
	this->reqMap.PutC(CSTR("/upload"), UploadReq);
	this->reqMap.PutC(CSTR("/cpuinfo"), CPUInfoReq);
}

SSWR::Benchmark::BenchmarkWebHandler::~BenchmarkWebHandler()
{
}
