#include "Stdafx.h"
#include "Data/ByteTool.h"
#include "Data/ChartPlotter.h"
#include "Exporter/GUIPNGExporter.h"
#include "IO/MemoryStream.h"
#include "Math/Math.h"
#include "Math/Unit/Pressure.h"
#include "Media/ImageList.h"
#include "Media/StaticImage.h"
#include "SSWR/SMonitor/SMonitorWebHandler.h"
#include "SSWR/SMonitor/SAnalogSensor.h"
#include "Sync/RWMutexUsage.h"
#include "Text/JSText.h"
#include "Text/MyStringFloat.h"
#include "Text/StringBuilderUTF8.h"
#include "Text/XML.h"
#include "Text/UTF8Writer.h"

Bool __stdcall SSWR::SMonitor::SMonitorWebHandler::OnSessDeleted(NN<Net::WebServer::WebSession> sess, AnyType userObj)
{
//	SSWR::SMonitor::SMonitorWebHandler *me = (SSWR::SMonitor::SMonitorWebHandler*)userObj;
	return false;
}

Bool __stdcall SSWR::SMonitor::SMonitorWebHandler::OnSessCheck(NN<Net::WebServer::WebSession> sess, AnyType userObj)
{
//	SSWR::SMonitor::SMonitorWebHandler *me = (SSWR::SMonitor::SMonitorWebHandler*)userObj;
	Data::DateTime dt;
	dt.SetCurrTimeUTC();
	Int64 t = dt.ToTicks() - sess->GetValueInt64(CSTR("LastSessTime"));
	if (t > 900000)
	{
		return true;
	}
	return false;
}

Bool __stdcall SSWR::SMonitor::SMonitorWebHandler::DefaultReq(NN<SSWR::SMonitor::SMonitorWebHandler> me, NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp)
{
	return resp->RedirectURL(req, CSTR("/monitor/index"), -2);
}

Bool __stdcall SSWR::SMonitor::SMonitorWebHandler::IndexReq(NN<SSWR::SMonitor::SMonitorWebHandler> me, NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp)
{
	Text::UTF8Writer *writer;
	UnsafeArray<UInt8> buff;
	UOSInt buffSize;
	UTF8Char sbuff[64];
	UnsafeArray<UTF8Char> sptr;
	NN<Text::String> s;
	IO::MemoryStream mstm;

	if (!me->core->UserExist())
	{
		if (req->GetReqMethod() == Net::WebUtil::RequestMethod::HTTP_POST)
		{
			req->ParseHTTPForm();
			NN<Text::String> pwd;
			NN<Text::String> retype;
			if (req->GetHTTPFormStr(CSTR("password")).SetTo(pwd) && req->GetHTTPFormStr(CSTR("retype")).SetTo(retype))
			{
				if (pwd->leng >= 3 && pwd->Equals(retype))
				{
					me->core->UserAdd((const UTF8Char*)"admin", pwd->v, 1);
					return resp->RedirectURL(req, CSTR("index"), 0);
				}
			}
		}
		NEW_CLASS(writer, Text::UTF8Writer(mstm));
		WriteHeaderBegin(writer);
		WriteHeaderEnd(writer);
		writer->WriteLine(CSTR("<body onload=\"document.getElementById('pwd').focus()\"><center>"));
		writer->WriteLine(CSTR("<form name=\"createForm\" method=\"POST\" action=\"index\">"));
		writer->WriteLine(CSTR("<table>"));
		writer->WriteLine(CSTR("<tr><td></td><td>Password for admin</td></tr>"));
		writer->WriteLine(CSTR("<tr><td>Password</td><td><input type=\"password\" name=\"password\" id=\"pwd\"/></td></tr>"));
		writer->WriteLine(CSTR("<tr><td>Retype</td><td><input type=\"password\" name=\"retype\"/></td></tr>"));
		writer->WriteLine(CSTR("<tr><td></td><td><input type=\"submit\"/></td></tr>"));
		writer->WriteLine(CSTR("</table>"));
		writer->WriteLine(CSTR("</form"));
		writer->WriteLine(CSTR("</center></body>"));
		writer->WriteLine(CSTR("</html>"));
	}
	else
	{
		Net::WebServer::WebSession *sess = me->sessMgr->GetSession(req, resp).OrNull();
		NEW_CLASS(writer, Text::UTF8Writer(mstm));
		WriteHeaderBegin(writer);
		WriteHeaderEnd(writer);
		writer->WriteLine(CSTR("<body onload=\"window.setTimeout(new Function('document.location.replace(\\'index\\')'), 60000)\">"));
		writer->WriteLine(CSTR("<table width=\"100%\"><tr><td width=\"100\" class=\"menu\">"));
		me->WriteMenu(writer, sess);
		writer->WriteLine(CSTR("</td><td>"));
		Int32 userId;
		Int32 userType;
		if (sess)
		{
			userId = sess->GetValueInt32(CSTR("UserId"));
			userType = sess->GetValueInt32(CSTR("UserType"));
		}
		else
		{
			userId = 0;
			userType = 0;
		}

		NN<Text::String> reqDevId;
		NN<Text::String> reqOutput;
		if (req->GetQueryValue(CSTR("devid")).SetTo(reqDevId) && req->GetQueryValue(CSTR("output")).SetTo(reqOutput))
		{
			Int64 idevId = reqDevId->ToInt64();
			if (me->core->UserHasDevice(userId, userType, idevId))
			{
				Text::StringBuilderUTF8 sb;
				sb.Append(reqOutput);
				UnsafeArray<UTF8Char> sarr[3];
				UOSInt i;
				i = Text::StrSplit(sarr, 3, sb.v, ',');
				if (i == 2)
				{
					me->core->DeviceSetOutput(idevId, Text::StrToUInt32(sarr[0]), Text::StrToInt32(sarr[1]) != 0);
				}
			}
		}

		Data::ArrayListNN<SMonitorCore::DeviceInfo> devList;
		NN<SMonitorCore::DeviceInfo> dev;
		Data::DateTime dt;
		me->core->UserGetDevices(userId, userType, devList);
		UOSInt i;
		UOSInt j;
		UOSInt k;
		UOSInt l;
		writer->WriteLine(CSTR("<h2>Home</h2>"));
		writer->WriteLine(CSTR("<table width=\"100%\" border=\"1\"><tr><td>Device Name</td><td>Last Reading Time</td><td>Readings Today</td><td>Digitals</td><td>Output</td></tr>"));
		i = 0;
		j = devList.GetCount();
		while (i < j)
		{
			dev = devList.GetItemNoCheck(i);
			Sync::RWMutexUsage mutUsage(dev->mut, false);
			writer->Write(CSTR("<tr><td>"));
			if (dev->devName.SetTo(s))
			{
				WriteHTMLText(writer, s);
			}
			else
			{
				WriteHTMLText(writer, dev->platformName);
			}
			writer->Write(CSTR("</td><td>"));
			if (dev->readingTime == 0)
			{
				writer->Write(CSTR("-"));
			}
			else
			{
				dt.SetTicks(dev->readingTime);
				dt.ToLocalTime();
				sptr = dt.ToString(sbuff, "yyyy-MM-dd HH:mm:ss.fff zzzz");
				writer->Write(CSTRP(sbuff, sptr));
			}
			writer->Write(CSTR("</td><td>"));
			k = 0;
			l = dev->nReading;
			while (k < l)
			{
				writer->Write(CSTR("<img src=\"devreadingimg?id="));
				sptr = Text::StrInt64(sbuff, dev->cliId);
				writer->Write(CSTRP(sbuff, sptr));
				writer->Write(CSTR("&sensor="));
				sptr = Text::StrInt32(sbuff, ReadInt16(dev->readings[k].status));
				writer->Write(CSTRP(sbuff, sptr));
				writer->Write(CSTR("&reading="));
				sptr = Text::StrInt32(sbuff, ReadInt16(&dev->readings[k].status[4]));
				writer->Write(CSTRP(sbuff, sptr));
				writer->Write(CSTR("&readingType="));
				sptr = Text::StrInt32(sbuff, ReadInt16(&dev->readings[k].status[6]));
				writer->Write(CSTRP(sbuff, sptr));
				writer->Write(CSTR("\"/>"));

				if (ReadInt16(&dev->readings[k].status[6]) == SSWR::SMonitor::SAnalogSensor::RT_RHUMIDITY)
				{
					writer->Write(CSTR("<br/>"));
					writer->Write(CSTR("<img src=\"devreadingimg?id="));
					sptr = Text::StrInt64(sbuff, dev->cliId);
					writer->Write(CSTRP(sbuff, sptr));
					writer->Write(CSTR("&sensor="));
					sptr = Text::StrInt32(sbuff, ReadInt16(dev->readings[k].status));
					writer->Write(CSTRP(sbuff, sptr));
					writer->Write(CSTR("&reading="));
					sptr = Text::StrInt32(sbuff, ReadInt16(&dev->readings[k].status[4]));
					writer->Write(CSTRP(sbuff, sptr));
					writer->Write(CSTR("&readingType="));
					sptr = Text::StrInt32(sbuff, SSWR::SMonitor::SAnalogSensor::RT_AHUMIDITY);
					writer->Write(CSTRP(sbuff, sptr));
					writer->Write(CSTR("\"/>"));
				}
				k++;
				if (k != l)
				{
					writer->Write(CSTR("<br/>"));
				}
			}
			writer->Write(CSTR("</td><td>"));
			UnsafeArray<const UTF8Char> nns;
			k = 0;
			l = dev->ndigital;
			while (k < l)
			{
				if (dev->digitalNames[k].SetTo(nns))
				{
					WriteHTMLText(writer, nns);
				}
				else
				{
					writer->Write(CSTR("Digital "));
					sptr = Text::StrUOSInt(sbuff, k);
					writer->Write(CSTRP(sbuff, sptr));
				}
				writer->Write(CSTR(": "));
				writer->Write((dev->digitalVals & (UInt32)(1 << k))?CSTR("1"):CSTR("0"));
				k++;
				if (k != l)
				{
					writer->Write(CSTR("<br/>"));
				}
			}

			writer->Write(CSTR("</td><td>"));
			k = 0;
			l = dev->nOutput;
			while (k < l)
			{
				writer->Write(CSTR("Output "));
				sptr = Text::StrUOSInt(sbuff, k);
				writer->Write(CSTRP(sbuff, sptr));
				writer->Write(CSTR(": "));
				writer->Write(CSTR("<a href=\"index?devid="));
				sptr = Text::StrInt64(sbuff, dev->cliId);
				writer->Write(CSTRP(sbuff, sptr));
				writer->Write(CSTR("&output="));
				sptr = Text::StrUOSInt(sbuff, k);
				writer->Write(CSTRP(sbuff, sptr));
				writer->Write(CSTR(",1\">On</a> <a href=\"index?devid="));
				sptr = Text::StrInt64(sbuff, dev->cliId);
				writer->Write(CSTRP(sbuff, sptr));
				writer->Write(CSTR("&output="));
				sptr = Text::StrUOSInt(sbuff, k);
				writer->Write(CSTRP(sbuff, sptr));
				writer->Write(CSTR(",0\">Off</a>"));
				k++;
				if (k != l)
				{
					writer->Write(CSTR("<br/>"));
				}
			}
			mutUsage.EndUse();
			writer->WriteLine(CSTR("</td></tr>"));
			
			i++;
		}
		writer->WriteLine(CSTR("</table>"));

		writer->WriteLine(CSTR("</td></tr></table>"));
		writer->WriteLine(CSTR("</body>"));
		writer->WriteLine(CSTR("</html>"));
		if (sess)
		{
			sess->EndUse();
		}
	}

	DEL_CLASS(writer);
	buff = mstm.GetBuff(buffSize);
	resp->AddDefHeaders(req);
	resp->AddContentType(CSTR("text/html"));
	resp->AddContentLength(buffSize);
	resp->AddHeader(CSTR("Cache-Control"), CSTR("no-cache"));
	resp->Write(Data::ByteArrayR(buff, buffSize));
	return true;
}

Bool __stdcall SSWR::SMonitor::SMonitorWebHandler::LoginReq(NN<SSWR::SMonitor::SMonitorWebHandler> me, NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp)
{
	Text::UTF8Writer *writer;
	UnsafeArray<UInt8> buff;
	UOSInt buffSize;
	NN<Net::WebServer::WebSession> sess;
	UnsafeArrayOpt<const UTF8Char> msg = 0;
	if (me->sessMgr->GetSession(req, resp).SetTo(sess))
	{
		sess->EndUse();
		return resp->RedirectURL(req, CSTR("index"), 0);
	}
	if (req->GetReqMethod() == Net::WebUtil::RequestMethod::HTTP_POST)
	{
		req->ParseHTTPForm();
		NN<Text::String> s;
		NN<Text::String> s2;
		NN<Text::String> s3;
		if (req->GetHTTPFormStr(CSTR("action")).SetTo(s) && req->GetHTTPFormStr(CSTR("user")).SetTo(s2) && req->GetHTTPFormStr(CSTR("pwd")).SetTo(s3) && s->Equals(UTF8STRC("login")))
		{
			if (s2->v[0])
			{
				NN<SSWR::SMonitor::SMonitorCore::LoginInfo> login;
				if (me->core->UserLogin(s2->v, s3->v).SetTo(login))
				{
					sess = me->sessMgr->CreateSession(req, resp);
					sess->SetValueInt32(CSTR("UserId"), login->userId);
					sess->SetValueInt32(CSTR("LoginId"), login->loginId);
					sess->SetValueInt32(CSTR("UserType"), login->userType);
					sess->EndUse();
					me->core->UserFreeLogin(login);

					return resp->VirtualRedirectURL(req, CSTR("/monitor/index"), 0);
				}
				else
				{
					msg = (const UTF8Char*)"User name or password not correct";
				}
			}
		}
	}

	IO::MemoryStream mstm;
	NEW_CLASS(writer, Text::UTF8Writer(mstm));
	WriteHeaderBegin(writer);
	WriteHeaderEnd(writer);
	writer->WriteLine(CSTR("<body onload=\"document.getElementById('user').focus()\">"));
	writer->WriteLine(CSTR("<table width=\"100%\"><tr><td width=\"100\" class=\"menu\">"));
	me->WriteMenu(writer, 0);
	writer->WriteLine(CSTR("</td><td><center>"));
	writer->WriteLine(CSTR("<form name=\"loginForm\" method=\"POST\" action=\"login\">"));
	writer->WriteLine(CSTR("<input type=\"hidden\" name=\"action\" value=\"login\"/>"));
	writer->WriteLine(CSTR("<table>"));
	writer->WriteLine(CSTR("<tr><td>User name</td><td><input type=\"text\" name=\"user\" id=\"user\" /></td></tr>"));
	writer->WriteLine(CSTR("<tr><td>Password</td><td><input type=\"password\" name=\"pwd\" id=\"pwd\"/></td></tr>"));
	writer->WriteLine(CSTR("<tr><td></td><td><input type=\"submit\"/></td></tr>"));
	UnsafeArray<const UTF8Char> nnmsg;
	if (msg.SetTo(nnmsg))
	{
		writer->WriteLine(CSTR("<tr><td></td><td>"));
		WriteHTMLText(writer, nnmsg);
		writer->WriteLine(CSTR("</td></tr>"));
	}
	writer->WriteLine(CSTR("</table>"));
	writer->WriteLine(CSTR("</form>"));
	writer->WriteLine(CSTR("</center></td></tr></table></body>"));
	writer->WriteLine(CSTR("</html>"));
	DEL_CLASS(writer);
	buff = mstm.GetBuff(buffSize);
	resp->AddDefHeaders(req);
	resp->AddContentType(CSTR("text/html"));
	resp->AddContentLength(buffSize);
	resp->AddHeader(CSTR("Cache-Control"), CSTR("no-cache"));
	resp->Write(Data::ByteArrayR(buff, buffSize));
	return true;
}

Bool __stdcall SSWR::SMonitor::SMonitorWebHandler::LogoutReq(NN<SSWR::SMonitor::SMonitorWebHandler> me, NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp)
{
	NN<Net::WebServer::WebSession> sess;
	if (me->sessMgr->GetSession(req, resp).SetTo(sess))
	{
		sess->EndUse();
		me->sessMgr->DeleteSession(req, resp);
	}
	return resp->RedirectURL(req, CSTR("/monitor/index"), 0);
}

Bool __stdcall SSWR::SMonitor::SMonitorWebHandler::DeviceReq(NN<SSWR::SMonitor::SMonitorWebHandler> me, NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp)
{
	Text::UTF8Writer *writer;
	UnsafeArray<UInt8> buff;
	UOSInt buffSize;
	UTF8Char sbuff[64];
	UnsafeArray<UTF8Char> sptr;
	UnsafeArray<const UTF8Char> nns;
	NN<Text::String> s;
	Int64 devId;
	NN<Net::WebServer::WebSession> sess;
	if (!me->sessMgr->GetSession(req, resp).SetTo(sess))
	{
		return resp->RedirectURL(req, CSTR("/monitor/index"), 0);
	}
	if (req->GetQueryValueI64(CSTR("photo"), devId))
	{
		if (me->core->UserHasDevice(sess->GetValueInt32(CSTR("UserId")), sess->GetValueInt32(CSTR("UserType")), devId))
		{
			me->core->SendCapturePhoto(devId);
		}
	}

	IO::MemoryStream mstm;
	NEW_CLASS(writer, Text::UTF8Writer(mstm));
	WriteHeaderBegin(writer);
	WriteHeaderEnd(writer);
	writer->WriteLine(CSTR("<body onload=\"window.setTimeout(new Function('document.location.reload()'), 60000)\">"));
	writer->WriteLine(CSTR("<table width=\"100%\"><tr><td width=\"100\" class=\"menu\">"));
	me->WriteMenu(writer, sess);
	writer->WriteLine(CSTR("</td><td>"));

	Data::ArrayListNN<SMonitorCore::DeviceInfo> devList;
	NN<SMonitorCore::DeviceInfo> dev;
	Data::DateTime dt;
	me->core->UserGetDevices(sess->GetValueInt32(CSTR("UserId")), sess->GetValueInt32(CSTR("UserType")), devList);
	UOSInt i;
	UOSInt j;
	UOSInt k;
	UOSInt l;
	writer->WriteLine(CSTR("<h2>Device</h2>"));
	writer->WriteLine(CSTR("<table width=\"100%\" border=\"1\"><tr><td>Device Name</td><td>Platform Name</td><td>CPU Name</td><td>Version</td><td>Reading Time</td><td>Readings</td><td>Digitals</td><td>Action</td></tr>"));
	i = 0;
	j = devList.GetCount();
	while (i < j)
	{
		dev = devList.GetItemNoCheck(i);
		writer->Write(CSTR("<tr><td>"));
		Sync::RWMutexUsage mutUsage(dev->mut, false);
		if (dev->devName.SetTo(s))
		{
			WriteHTMLText(writer, s);
		}
		else
		{
			WriteHTMLText(writer, dev->platformName);
		}
		writer->Write(CSTR("</a></td><td>"));
		WriteHTMLText(writer, dev->platformName);
		writer->Write(CSTR("</td><td>"));
		WriteHTMLText(writer, dev->cpuName);
		writer->Write(CSTR("</td><td>"));
		if (dev->version == 0)
		{
			writer->Write(CSTR("-"));
		}
		else
		{
			dt.SetTicks(dev->version);
			dt.ToLocalTime();
			sptr = dt.ToString(sbuff, "yyyy-MM-dd HH:mm:ss.fff");
			writer->Write(CSTRP(sbuff, sptr));
		}
		writer->Write(CSTR("</td><td>"));
		if (dev->readingTime == 0)
		{
			writer->Write(CSTR("-"));
		}
		else
		{
			dt.SetTicks(dev->readingTime);
			dt.ToLocalTime();
			sptr = dt.ToString(sbuff, "yyyy-MM-dd HH:mm:ss.fff zzzz");
			writer->Write(CSTRP(sbuff, sptr));
		}
		writer->Write(CSTR("</td><td>"));
		k = 0;
		l = dev->nReading;
		while (k < l)
		{
			if (dev->readingNames[k].SetTo(nns))
			{
				WriteHTMLText(writer, nns);
			}
			else
			{
				writer->Write(CSTR("Sensor "));
				sptr = Text::StrInt32(sbuff, ReadInt16(dev->readings[k].status));
				writer->Write(CSTRP(sbuff, sptr));
				writer->Write(CSTR(" "));
				writer->Write(SSWR::SMonitor::SAnalogSensor::GetReadingTypeName((SSWR::SMonitor::SAnalogSensor::ReadingType)ReadUInt16(&dev->readings[k].status[6])));
			}

			writer->Write(CSTR(" = "));
			sptr = Text::StrDouble(sbuff, dev->readings[k].reading);
			writer->Write(CSTRP(sbuff, sptr));
			k++;
			if (k != l)
			{
				writer->Write(CSTR("<br/>"));
			}
		}
		writer->Write(CSTR("</td><td>"));
		k = 0;
		l = dev->ndigital;
		while (k < l)
		{
			if (dev->digitalNames[k].SetTo(nns))
			{
				WriteHTMLText(writer, nns);
			}
			else
			{
				writer->Write(CSTR("Digital "));
				sptr = Text::StrUOSInt(sbuff, k);
				writer->Write(CSTRP(sbuff, sptr));
			}
			writer->Write(CSTR(": "));
			writer->Write((dev->digitalVals & (UInt32)(1 << k))?CSTR("1"):CSTR("0"));
			k++;
			if (k != l)
			{
				writer->Write(CSTR("<br/>"));
			}
		}
		writer->Write(CSTR("</td><td><a href=\"devedit?id="));
		sptr = Text::StrInt64(sbuff, dev->cliId);
		writer->Write(CSTRP(sbuff, sptr));
		writer->Write(CSTR("\">Edit</a><br/>"));
		writer->Write(CSTR("<a href=\"devreading?id="));
		writer->Write(CSTRP(sbuff, sptr));
		writer->Write(CSTR("\">Reading Names</a><br/>"));
		writer->Write(CSTR("<a href=\"devdigitals?id="));
		writer->Write(CSTRP(sbuff, sptr));
		writer->Write(CSTR("\">Digital Names</a><br/>"));
		writer->Write(CSTR("<a href=\"device?photo="));
		writer->Write(CSTRP(sbuff, sptr));
		writer->Write(CSTR("\">Capture Photo</a>"));
		mutUsage.EndUse();
		writer->WriteLine(CSTR("</td></tr>"));
		
		i++;
	}
	writer->WriteLine(CSTR("</table>"));

	writer->WriteLine(CSTR("</td></tr></table></body>"));
	writer->WriteLine(CSTR("</html>"));
	sess->EndUse();

	DEL_CLASS(writer);
	buff = mstm.GetBuff(buffSize);
	resp->AddDefHeaders(req);
	resp->AddContentType(CSTR("text/html"));
	resp->AddContentLength(buffSize);
	resp->AddHeader(CSTR("Cache-Control"), CSTR("no-cache"));
	resp->Write(Data::ByteArrayR(buff, buffSize));
	return true;
}

Bool __stdcall SSWR::SMonitor::SMonitorWebHandler::DeviceEditReq(NN<SSWR::SMonitor::SMonitorWebHandler> me, NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp)
{
	Text::UTF8Writer *writer;
	UnsafeArray<UInt8> buff;
	UOSInt buffSize;
	UTF8Char sbuff[64];
	UnsafeArray<UTF8Char> sptr;
	NN<Text::String> s;
	NN<Net::WebServer::WebSession> sess;
	if (!me->sessMgr->GetSession(req, resp).SetTo(sess))
	{
		return resp->RedirectURL(req, CSTR("/monitor/index"), 0);
	}
	NN<Text::String> cid;
	Int64 cliId = 0;
	if (req->GetQueryValue(CSTR("id")).SetTo(cid))
	{
		cliId = cid->ToInt64();
	}
	if (cliId == 0)
	{
		sess->EndUse();
		return resp->RedirectURL(req, CSTR("/monitor/device"), 0);
	}
	NN<SMonitorCore::DeviceInfo> dev;
	if (!me->core->UserHasDevice(sess->GetValueInt32(CSTR("UserId")), sess->GetValueInt32(CSTR("UserType")), cliId) || !me->core->DeviceGet(cliId).SetTo(dev))
	{
		sess->EndUse();
		return resp->RedirectURL(req, CSTR("/monitor/device"), 0);
	}

	if (req->GetReqMethod() == Net::WebUtil::RequestMethod::HTTP_POST)
	{
		req->ParseHTTPForm();
		NN<Text::String> action;
		if (req->GetHTTPFormStr(CSTR("action")).SetTo(action) && action->Equals(UTF8STRC("modify")))
		{
			NN<Text::String> devName;
			Int32 flags = 0;
			NN<Text::String> s;
			if (req->GetHTTPFormStr(CSTR("anonymous")).SetTo(s) && s->v[0] == '1')
			{
				flags |= 1;
			}
			if (req->GetHTTPFormStr(CSTR("removed")).SetTo(s) && s->v[0] == '1')
			{
				flags |= 2;
			}
			if (req->GetHTTPFormStr(CSTR("devName")).SetTo(devName))
			{
				if (me->core->DeviceModify(cliId, devName->ToCString(), flags))
				{
					sess->EndUse();
					return resp->RedirectURL(req, CSTR("/monitor/device"), 0);
				}
			}
		}
	}

	IO::MemoryStream mstm;
	NEW_CLASS(writer, Text::UTF8Writer(mstm));
	WriteHeaderBegin(writer);
	WriteHeaderEnd(writer);
	writer->WriteLine(CSTR("<body onload=\"document.forms[0].devName.focus()\">"));
	writer->WriteLine(CSTR("<table width=\"100%\"><tr><td width=\"100\" class=\"menu\">"));
	me->WriteMenu(writer, sess);
	writer->WriteLine(CSTR("</td><td>"));

	writer->WriteLine(CSTR("<h2>Edit Device</h2>"));
	writer->Write(CSTR("<form name=\"modifyForm\" method=\"POST\" action=\"devedit?id="));
	sptr = Text::StrInt64(sbuff, cliId);
	writer->Write(CSTRP(sbuff, sptr));
	writer->WriteLine(CSTR("\">"));
	writer->WriteLine(CSTR("<table width=\"100%\" border=\"1\">"));
	writer->Write(CSTR("<input type=\"hidden\" name=\"action\" value=\"modify\"/>"));
	Sync::RWMutexUsage mutUsage(dev->mut, false);
	writer->Write(CSTR("<tr><td>Platform Name</td><td>"));
	WriteHTMLText(writer, dev->platformName);
	writer->WriteLine(CSTR("</td></tr>"));
	writer->Write(CSTR("<tr><td>CPU Name</td><td>"));
	WriteHTMLText(writer, dev->cpuName);
	writer->WriteLine(CSTR("</td></tr>"));

	writer->Write(CSTR("<tr><td>Device Name</td><td><input type=\"text\" name=\"devName\" "));
	if (dev->devName.SetTo(s))
	{
		writer->Write(CSTR(" value="));
		WriteAttrText(writer, s);
	}
	writer->WriteLine(CSTR("/></td></tr>"));
	writer->Write(CSTR("<tr><td>Flags</td><td><input type=\"checkbox\" name=\"anonymous\" id=\"anonymous\" value=\"1\""));
	if (dev->flags & 1)
	{
		writer->Write(CSTR(" checked"));
	}
	writer->WriteLine(CSTR("/><label for=\"anonymous\">Anonymous Access</label><br/><input type=\"checkbox\" name=\"removed\" id=\"removed\" value=\"1\""));
	if (dev->flags & 2)
	{
		writer->Write(CSTR(" checked"));
	}
	writer->Write(CSTR("/><label for=\"removed\">Removed</label></td></tr>"));
	writer->WriteLine(CSTR("<tr><td></td><td><input type=\"submit\"/></td></tr>"));
	writer->WriteLine(CSTR("</table></form>"));
	mutUsage.EndUse();

	writer->WriteLine(CSTR("</td></tr></table></body>"));
	writer->WriteLine(CSTR("</html>"));
	sess->EndUse();

	DEL_CLASS(writer);
	buff = mstm.GetBuff(buffSize);
	resp->AddDefHeaders(req);
	resp->AddContentType(CSTR("text/html"));
	resp->AddContentLength(buffSize);
	resp->AddHeader(CSTR("Cache-Control"), CSTR("no-cache"));
	resp->Write(Data::ByteArrayR(buff, buffSize));
	return true;
}

Bool __stdcall SSWR::SMonitor::SMonitorWebHandler::DeviceReadingReq(NN<SSWR::SMonitor::SMonitorWebHandler> me, NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp)
{
	Text::UTF8Writer *writer;
	UnsafeArray<UInt8> buff;
	UOSInt buffSize;
	UTF8Char sbuff[64];
	UnsafeArray<UTF8Char> sptr;
	UnsafeArray<const UTF8Char> nns;
	UOSInt i;
	UOSInt j;
	NN<SMonitorCore::DeviceInfo> dev;
	NN<Net::WebServer::WebSession> sess;
	if (!me->sessMgr->GetSession(req, resp).SetTo(sess))
	{
		return resp->RedirectURL(req, CSTR("/monitor/index"), 0);
	}
	NN<Text::String> cid;
	Int64 cliId = 0;
	if (req->GetQueryValue(CSTR("id")).SetTo(cid))
	{
		cliId = cid->ToInt64();
	}
	if (cliId == 0)
	{
		sess->EndUse();
		return resp->RedirectURL(req, CSTR("/monitor/device"), 0);
	}
	if (!me->core->UserHasDevice(sess->GetValueInt32(CSTR("UserId")), sess->GetValueInt32(CSTR("UserType")), cliId) || !me->core->DeviceGet(cliId).SetTo(dev))
	{
		sess->EndUse();
		return resp->RedirectURL(req, CSTR("/monitor/device"), 0);
	}

	if (req->GetReqMethod() == Net::WebUtil::RequestMethod::HTTP_POST)
	{
		req->ParseHTTPForm();
		NN<Text::String> action;
		if (req->GetHTTPFormStr(CSTR("action")).SetTo(action) && action->Equals(UTF8STRC("reading")))
		{
			Text::StringBuilderUTF8 sb;
			NN<Text::String> s;
			i = 0;
			j = dev->nReading;
			while (i < j)
			{
				if (i > 0)
				{
					sb.AppendC(UTF8STRC("|"));
				}
				sptr = Text::StrUOSInt(Text::StrConcatC(sbuff, UTF8STRC("readingName")), i);
				if (req->GetHTTPFormStr(CSTRP(sbuff, sptr)).SetTo(s))
				{
					sb.Append(s);
				}
				i++;
			}
			if (me->core->DeviceSetReadings(dev, sb.ToString()))
			{
				sess->EndUse();
				return resp->RedirectURL(req, CSTR("/monitor/device"), 0);
			}
		}
	}

	if (dev->nReading <= 0)
	{
		sess->EndUse();
		return resp->RedirectURL(req, CSTR("/monitor/device"), 0);
	}

	IO::MemoryStream mstm;
	NEW_CLASS(writer, Text::UTF8Writer(mstm));
	WriteHeaderBegin(writer);
	WriteHeaderEnd(writer);
	writer->WriteLine(CSTR("<body onload=\"document.forms[0].readingName0.focus()\">"));
	writer->WriteLine(CSTR("<table width=\"100%\"><tr><td width=\"100\" class=\"menu\">"));
	me->WriteMenu(writer, sess);
	writer->WriteLine(CSTR("</td><td>"));

	writer->WriteLine(CSTR("<h2>Reading Names</h2>"));
	writer->Write(CSTR("<form name=\"modifyForm\" method=\"POST\" action=\"devreading?id="));
	sptr = Text::StrInt64(sbuff, cliId);
	writer->Write(CSTRP(sbuff, sptr));
	writer->WriteLine(CSTR("\">"));
	writer->Write(CSTR("<input type=\"hidden\" name=\"action\" value=\"reading\"/>"));
	writer->WriteLine(CSTR("<table width=\"100%\" border=\"1\">"));
	Sync::RWMutexUsage mutUsage(dev->mut, false);
	j = dev->nReading;
	i = 0;
	while (i < j)
	{
		writer->Write(CSTR("<tr><td>Reading "));
		sptr = Text::StrUOSInt(sbuff, i);
		writer->Write(CSTRP(sbuff, sptr));
		writer->Write(CSTR("</td><td><input type=\"text\" name=\"readingName"));
		writer->Write(CSTRP(sbuff, sptr));
		writer->Write(CSTR("\" "));

		if (dev->readingNames[i].SetTo(nns))
		{
			writer->Write(CSTR(" value="));
			WriteAttrText(writer, nns);
		}
		writer->Write(CSTR("/></td><td>Sensor "));
		sptr = Text::StrInt32(sbuff, ReadInt16(&dev->readings[i].status[0]));
		writer->Write(CSTRP(sbuff, sptr));
		if (ReadInt16(&dev->readings[i].status[2]) != SSWR::SMonitor::SAnalogSensor::ST_UNKNOWN)
		{
			writer->Write(CSTR(" ("));
			writer->Write(SSWR::SMonitor::SAnalogSensor::GetSensorTypeName((SSWR::SMonitor::SAnalogSensor::SensorType)ReadInt16(&dev->readings[i].status[2])));
			writer->Write(CSTR(")"));
		}
		writer->Write(CSTR(" Reading "));
		sptr = Text::StrInt32(sbuff, ReadInt16(&dev->readings[i].status[4]));
		writer->Write(CSTRP(sbuff, sptr));
		if (ReadInt16(&dev->readings[i].status[6]) != SSWR::SMonitor::SAnalogSensor::RT_UNKNOWN)
		{
			writer->Write(CSTR(" "));
			writer->Write(SSWR::SMonitor::SAnalogSensor::GetReadingTypeName((SSWR::SMonitor::SAnalogSensor::ReadingType)ReadInt16(&dev->readings[i].status[6])));
		}
		writer->WriteLine(CSTR("</td></tr>"));
		i++;
	}
	writer->WriteLine(CSTR("<tr><td></td><td><input type=\"submit\"/></td></tr>"));
	writer->WriteLine(CSTR("</table></form>"));
	mutUsage.EndUse();

	writer->WriteLine(CSTR("</td></tr></table></body>"));
	writer->WriteLine(CSTR("</html>"));
	sess->EndUse();

	DEL_CLASS(writer);
	buff = mstm.GetBuff(buffSize);
	resp->AddDefHeaders(req);
	resp->AddContentType(CSTR("text/html"));
	resp->AddContentLength(buffSize);
	resp->AddHeader(CSTR("Cache-Control"), CSTR("no-cache"));
	resp->Write(Data::ByteArrayR(buff, buffSize));
	return true;
}

Bool __stdcall SSWR::SMonitor::SMonitorWebHandler::DeviceDigitalsReq(NN<SSWR::SMonitor::SMonitorWebHandler> me, NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp)
{
	Text::UTF8Writer *writer;
	UnsafeArray<UInt8> buff;
	UOSInt buffSize;
	UTF8Char sbuff[64];
	UnsafeArray<UTF8Char> sptr;
	UnsafeArray<const UTF8Char> nns;
	UOSInt i;
	UOSInt j;
	NN<SMonitorCore::DeviceInfo> dev;
	NN<Net::WebServer::WebSession> sess;
	if (!me->sessMgr->GetSession(req, resp).SetTo(sess))
	{
		return resp->RedirectURL(req, CSTR("/monitor/index"), 0);
	}
	NN<Text::String> cid;
	Int64 cliId = 0;
	if (req->GetQueryValue(CSTR("id")).SetTo(cid))
	{
		cliId = cid->ToInt64();
	}
	if (cliId == 0)
	{
		sess->EndUse();
		return resp->RedirectURL(req, CSTR("/monitor/device"), 0);
	}
	if (!me->core->UserHasDevice(sess->GetValueInt32(CSTR("UserId")), sess->GetValueInt32(CSTR("UserType")), cliId) || !me->core->DeviceGet(cliId).SetTo(dev))
	{
		sess->EndUse();
		return resp->RedirectURL(req, CSTR("/monitor/device"), 0);
	}

	if (req->GetReqMethod() == Net::WebUtil::RequestMethod::HTTP_POST)
	{
		req->ParseHTTPForm();
		NN<Text::String> action;
		if (req->GetHTTPFormStr(CSTR("action")).SetTo(action) && action->Equals(UTF8STRC("digitals")))
		{
			Text::StringBuilderUTF8 sb;
			NN<Text::String> s;
			i = 0;
			j = dev->ndigital;
			while (i < j)
			{
				if (i > 0)
				{
					sb.AppendC(UTF8STRC("|"));
				}
				sptr = Text::StrUOSInt(Text::StrConcatC(sbuff, UTF8STRC("digitalName")), i);
				if (req->GetHTTPFormStr(CSTRP(sbuff, sptr)).SetTo(s))
				{
					sb.Append(s);
				}
				i++;
			}
			if (me->core->DeviceSetDigitals(dev, sb.ToString()))
			{
				sess->EndUse();
				return resp->RedirectURL(req, CSTR("/monitor/device"), 0);
			}
		}
	}

	if (dev->ndigital <= 0)
	{
		sess->EndUse();
		return resp->RedirectURL(req, CSTR("/monitor/device"), 0);
	}

	IO::MemoryStream mstm;
	NEW_CLASS(writer, Text::UTF8Writer(mstm));
	WriteHeaderBegin(writer);
	WriteHeaderEnd(writer);
	writer->WriteLine(CSTR("<body onload=\"document.forms[0].digitalName0.focus()\">"));
	writer->WriteLine(CSTR("<table width=\"100%\"><tr><td width=\"100\" class=\"menu\">"));
	me->WriteMenu(writer, sess);
	writer->WriteLine(CSTR("</td><td>"));

	writer->WriteLine(CSTR("<h2>Digital Names</h2>"));
	writer->Write(CSTR("<form name=\"modifyForm\" method=\"POST\" action=\"devdigitals?id="));
	sptr = Text::StrInt64(sbuff, cliId);
	writer->Write(CSTRP(sbuff, sptr));
	writer->WriteLine(CSTR("\">"));
	writer->Write(CSTR("<input type=\"hidden\" name=\"action\" value=\"digitals\"/>"));
	writer->WriteLine(CSTR("<table width=\"100%\" border=\"1\">"));
	Sync::RWMutexUsage mutUsage(dev->mut, false);
	j = dev->ndigital;
	i = 0;
	while (i < j)
	{
		writer->Write(CSTR("<tr><td>Digital "));
		sptr = Text::StrUOSInt(sbuff, i);
		writer->Write(CSTRP(sbuff, sptr));
		writer->Write(CSTR("</td><td><input type=\"text\" name=\"digitalName"));
		writer->Write(CSTRP(sbuff, sptr));
		writer->Write(CSTR("\" "));

		if (dev->digitalNames[i].SetTo(nns))
		{
			writer->Write(CSTR(" value="));
			WriteAttrText(writer, nns);
		}
		writer->Write(CSTR("/></td></tr>"));
		i++;
	}
	writer->WriteLine(CSTR("<tr><td></td><td><input type=\"submit\"/></td></tr>"));
	writer->WriteLine(CSTR("</table></form>"));
	mutUsage.EndUse();

	writer->WriteLine(CSTR("</td></tr></table></body>"));
	writer->WriteLine(CSTR("</html>"));
	sess->EndUse();

	DEL_CLASS(writer);
	buff = mstm.GetBuff(buffSize);
	resp->AddDefHeaders(req);
	resp->AddContentType(CSTR("text/html"));
	resp->AddContentLength(buffSize);
	resp->AddHeader(CSTR("Cache-Control"), CSTR("no-cache"));
	resp->Write(Data::ByteArrayR(buff, buffSize));
	return true;
}

Bool __stdcall SSWR::SMonitor::SMonitorWebHandler::DeviceReadingImgReq(NN<SSWR::SMonitor::SMonitorWebHandler> me, NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp)
{
	NN<Text::String> s;
	Int64 cliId = 0;
	Int32 userId = 0;
	Int32 userType = 0;
	Int32 sensorId = 0;
	Int32 readingId = 0;
	Int32 readingType = 0;
	Bool valid = true;
	NN<SSWR::SMonitor::SMonitorCore::DeviceInfo> dev;

	if (!req->GetQueryValue(CSTR("id")).SetTo(s))
	{
		valid = false;
	}
	else if (!s->ToInt64(cliId))
	{
		valid = false;
	}
	
	if (!req->GetQueryValue(CSTR("sensor")).SetTo(s))
	{
		valid = false;
	}
	else if (!s->ToInt32(sensorId))
	{
		valid = false;
	}

	if (!req->GetQueryValue(CSTR("reading")).SetTo(s))
	{
		valid = false;
	}
	else if (!s->ToInt32(readingId))
	{
		valid = false;
	}

	if (!req->GetQueryValue(CSTR("readingType")).SetTo(s))
	{
		valid = false;
	}
	else if (!s->ToInt32(readingType))
	{
		valid = false;
	}

	if (!valid || !me->core->DeviceGet(cliId).SetTo(dev))
	{
		resp->ResponseError(req, Net::WebStatus::SC_NOT_FOUND);
		return true;
	}

	NN<Net::WebServer::WebSession> sess;
	if (me->sessMgr->GetSession(req, resp).SetTo(sess))
	{
		userId = sess->GetValueInt32(CSTR("UserId"));
		userType = sess->GetValueInt32(CSTR("UserType"));
		sess->EndUse();
	}

	if (!me->core->UserHasDevice(userId, userType, cliId))
	{
		resp->ResponseError(req, Net::WebStatus::SC_NOT_FOUND);
		return true;
	}

	UOSInt i;
	UOSInt j;
	UOSInt k;
	NN<IO::MemoryStream> mstm;
	UnsafeArray<UInt8> buff;
	UOSInt buffSize;

	if (dev->valUpdated)
	{
		Sync::RWMutexUsage mutUsage(dev->mut, true);
		dev->valUpdated = false;
		i = dev->imgCaches.GetCount();
		while (i-- > 0)
		{
			mstm = dev->imgCaches.GetItemNoCheck(i);
			mstm.Delete();
		}
		dev->imgCaches.Clear();
	}
	
	Sync::RWMutexUsage mutUsage(dev->mut, false);
	if (dev->imgCaches.Get((sensorId << 16) + (readingId << 8) + (readingType)).SetTo(mstm))
	{
		buff = mstm->GetBuff(buffSize);
		resp->AddDefHeaders(req);
		resp->AddContentType(CSTR("image/png"));
		resp->AddContentLength(buffSize);
		resp->AddHeader(CSTR("Cache-Control"), CSTR("no-cache"));
		resp->Write(Data::ByteArrayR(buff, buffSize));
		return true;
	}

	NN<Media::DrawEngine> deng = me->core->GetDrawEngine();
	NN<Media::DrawImage> dimg;
	if (deng->CreateImage32(Math::Size2D<UOSInt>(640, 120), Media::AT_ALPHA_ALL_FF).SetTo(dimg))
	{
		NN<Media::DrawFont> f;
		NN<Media::DrawBrush> b;
		UOSInt readingIndex = (UOSInt)-1;
		Int32 readingTypeD;
		Data::ChartPlotter *chart;
		j = dev->nReading;
		i = 0;
		while (i < j)
		{
			if (ReadInt16(&dev->readings[i].status[0]) == sensorId && ReadInt16(&dev->readings[i].status[4]) == readingId)
			{
				if (readingType == ReadInt16(&dev->readings[i].status[6]))
				{
					readingIndex = i;
					readingTypeD = ReadInt16(&dev->readings[i].status[6]);
					break;
				}
				else if (readingType == SSWR::SMonitor::SAnalogSensor::RT_AHUMIDITY && ReadInt16(&dev->readings[i].status[6]) == SSWR::SMonitor::SAnalogSensor::RT_RHUMIDITY)
				{
					readingIndex = i;
					readingTypeD = ReadInt16(&dev->readings[i].status[6]);
					break;
				}
			}
			i++;
		}
		if (readingIndex == (UOSInt)-1)
		{
			f = dimg->NewFontPx(CSTR("Arial"), 12, Media::DrawEngine::DFS_ANTIALIAS, 0);
			b = dimg->NewBrushARGB(0xffffffff);
			dimg->DrawRect(Math::Coord2DDbl(0, 0), dimg->GetSize().ToDouble(), 0, b);
			dimg->DelBrush(b);
			b = dimg->NewBrushARGB(0xff000000);
			dimg->DrawString(Math::Coord2DDbl(0, 0), CSTR("Sensor not found"), f, b);
			dimg->DelBrush(b);
			dimg->DelFont(f);
		}
		else
		{
			Data::ArrayListInt64 dateList;
			Data::ArrayListDbl valList;
			NN<SSWR::SMonitor::SMonitorCore::DevRecord2> rec;
			if (readingType == SSWR::SMonitor::SAnalogSensor::RT_AHUMIDITY && readingTypeD == SSWR::SMonitor::SAnalogSensor::RT_RHUMIDITY)
			{
				UOSInt treadingIndex = (UOSInt)-1;
				Double tempDeg;
				Double rh;
				Bool hasTemp;
				Bool hasRH;
				i = 0;
				j = dev->nReading;
				while (i < j)
				{
					if (ReadInt16(&dev->readings[i].status[0]) == sensorId && ReadInt16(&dev->readings[i].status[6]) == SSWR::SMonitor::SAnalogSensor::RT_TEMPERATURE)
					{
						treadingIndex = i;
						break;
					}
					i++;
				}

				i = 0;
				j = dev->todayRecs.GetCount();
				while (i < j)
				{
					rec = dev->todayRecs.GetItemNoCheck(i);
					hasTemp = false;
					hasRH = false;
					if (rec->nreading > readingIndex && ReadInt16(&rec->readings[readingIndex].status[0]) == sensorId && ReadInt16(&rec->readings[readingIndex].status[4]) == readingId)
					{
						if (ReadInt16(&rec->readings[readingIndex].status[6]) == readingTypeD)
						{
							hasRH = true;
							rh = rec->readings[readingIndex].reading;
						}
					}
					else
					{
						k = rec->nreading;
						while (k-- > 0)
						{
							if (ReadInt16(&rec->readings[k].status[0]) == sensorId && ReadInt16(&rec->readings[k].status[4]) == readingId && ReadInt16(&rec->readings[k].status[6]) == readingTypeD)
							{
								hasRH = true;
								rh = rec->readings[k].reading;
								break;
							}
						}
					}

					if (treadingIndex != (UOSInt)-1 && rec->nreading > treadingIndex && ReadInt16(&rec->readings[treadingIndex].status[0]) == sensorId && ReadInt16(&rec->readings[treadingIndex].status[6]) == SSWR::SMonitor::SAnalogSensor::RT_TEMPERATURE)
					{
						hasTemp = true;
						tempDeg = rec->readings[treadingIndex].reading;
					}
					else
					{
						k = rec->nreading;
						while (k-- > 0)
						{
							if (ReadInt16(&rec->readings[k].status[0]) == sensorId && ReadInt16(&rec->readings[k].status[6]) == SSWR::SMonitor::SAnalogSensor::RT_TEMPERATURE)
							{
								hasTemp = true;
								tempDeg = rec->readings[k].reading;
								break;
							}
						}
					}

					if (hasTemp && hasRH)
					{
						dateList.Add(rec->recTime);
						valList.Add(Math::Unit::Pressure::WaterVapourPressure(Math::Unit::Pressure::PU_PASCAL, Math::Unit::Temperature::TU_CELSIUS, tempDeg, rh));
					}
					i++;
				}
			}
			else
			{
				i = 0;
				j = dev->todayRecs.GetCount();
				while (i < j)
				{
					rec = dev->todayRecs.GetItemNoCheck(i);
					if (rec->nreading > readingIndex && ReadInt16(&rec->readings[readingIndex].status[0]) == sensorId && ReadInt16(&rec->readings[readingIndex].status[4]) == readingId)
					{
						if (ReadInt16(&rec->readings[readingIndex].status[6]) == readingType)
						{
							dateList.Add(rec->recTime);
							valList.Add(rec->readings[readingIndex].reading);
						}
					}
					else
					{
						k = rec->nreading;
						while (k-- > 0)
						{
							if (ReadInt16(&rec->readings[k].status[0]) == sensorId && ReadInt16(&rec->readings[k].status[4]) == readingId)
							{
								if (ReadInt16(&rec->readings[k].status[6]) == readingType)
								{
									dateList.Add(rec->recTime);
									valList.Add(rec->readings[k].reading);
								}
								break;
							}
						}
					}
					i++;
				}
			}

			Text::StringBuilderUTF8 sb;
			UnsafeArray<const UTF8Char> nns;
			if (dev->readingNames[readingIndex].SetTo(nns))
			{
				sb.AppendSlow(nns);
			}
			else
			{
				sb.AppendC(UTF8STRC("Sensor "));
				sb.AppendI32(ReadInt16(dev->readings[readingIndex].status));
				if (ReadInt16(&dev->readings[readingIndex].status[6]) != SSWR::SMonitor::SAnalogSensor::RT_UNKNOWN)
				{
					sb.AppendC(UTF8STRC(" "));
					sb.Append(SSWR::SMonitor::SAnalogSensor::GetReadingTypeName((SSWR::SMonitor::SAnalogSensor::ReadingType)ReadInt16(&dev->readings[readingIndex].status[6])));
				}
			}
			Double currVal;
			if (readingType == SSWR::SMonitor::SAnalogSensor::RT_AHUMIDITY && readingTypeD == SSWR::SMonitor::SAnalogSensor::RT_RHUMIDITY)
			{
				currVal = 0;
				i = 0;
				j = dev->nReading;
				while (i < j)
				{
					if (ReadInt16(&dev->readings[i].status[0]) == sensorId && ReadInt16(&dev->readings[i].status[6]) == SSWR::SMonitor::SAnalogSensor::RT_TEMPERATURE)
					{
						currVal = Math::Unit::Pressure::WaterVapourPressure(Math::Unit::Pressure::PU_PASCAL, Math::Unit::Temperature::TU_CELSIUS, dev->readings[i].reading, dev->readings[readingIndex].reading);
						break;
					}
					i++;
				}
			}
			else
			{
				currVal = dev->readings[readingIndex].reading;
			}
			sb.AppendC(UTF8STRC(" ("));
			sb.AppendDouble(currVal);
			sb.AppendC(UTF8STRC(")"));
			if (dateList.GetCount() >= 2)
			{
				Int64 maxTime;
				Int64 thisTime;
				Double yesterdayVal = 0;
				Int64 yesterdayMaxTime = 0;
				Data::ArrayListInt64 dateList2;
				Data::ArrayListDbl valList2;
				maxTime = dateList.GetItem(dateList.GetCount() - 1);
				if (readingType == SSWR::SMonitor::SAnalogSensor::RT_AHUMIDITY && readingTypeD == SSWR::SMonitor::SAnalogSensor::RT_RHUMIDITY)
				{
					UOSInt treadingIndex = (UOSInt)-1;
					Double tempDeg;
					Double rh;
					Bool hasTemp;
					Bool hasRH;
					i = 0;
					j = dev->nReading;
					while (i < j)
					{
						if (ReadInt16(&dev->readings[i].status[0]) == sensorId && ReadInt16(&dev->readings[i].status[6]) == SSWR::SMonitor::SAnalogSensor::RT_TEMPERATURE)
						{
							treadingIndex = i;
							break;
						}
						i++;
					}

					i = 0;
					j = dev->yesterdayRecs.GetCount();
					while (i < j)
					{
						rec = dev->yesterdayRecs.GetItemNoCheck(i);
						hasTemp = false;
						hasRH = false;
						if (rec->nreading > readingIndex && ReadInt16(&rec->readings[readingIndex].status[0]) == sensorId && ReadInt16(&rec->readings[readingIndex].status[4]) == readingId)
						{
							if (ReadInt16(&rec->readings[readingIndex].status[6]) == readingTypeD)
							{
								hasRH = true;
								rh = rec->readings[readingIndex].reading;
							}
						}
						else
						{
							k = rec->nreading;
							while (k-- > 0)
							{
								if (ReadInt16(&rec->readings[k].status[0]) == sensorId && ReadInt16(&rec->readings[k].status[4]) == readingId)
								{
									if (ReadInt16(&rec->readings[k].status[6]) == readingTypeD)
									{
										hasRH = true;
										rh = rec->readings[readingIndex].reading;
									}
									break;
								}
							}
						}

						if (treadingIndex != (UOSInt)-1 && rec->nreading > treadingIndex && ReadInt16(&rec->readings[treadingIndex].status[0]) == sensorId && ReadInt16(&rec->readings[treadingIndex].status[6]) == SSWR::SMonitor::SAnalogSensor::RT_TEMPERATURE)
						{
							hasTemp = true;
							tempDeg = rec->readings[treadingIndex].reading;
						}
						else
						{
							k = rec->nreading;
							while (k-- > 0)
							{
								if (ReadInt16(&rec->readings[k].status[0]) == sensorId && ReadInt16(&rec->readings[k].status[6]) == SSWR::SMonitor::SAnalogSensor::RT_TEMPERATURE)
								{
									hasTemp = true;
									tempDeg = rec->readings[k].reading;
									break;
								}
							}
						}

						if (hasTemp && hasRH)
						{
							thisTime = rec->recTime + 86400000LL;
							if (thisTime <= maxTime)
							{
								Double pres = Math::Unit::Pressure::WaterVapourPressure(Math::Unit::Pressure::PU_PASCAL, Math::Unit::Temperature::TU_CELSIUS, tempDeg, rh);
								dateList2.Add(thisTime);
								valList2.Add(pres);
								if (thisTime > yesterdayMaxTime)
								{
									yesterdayMaxTime = thisTime;
									yesterdayVal = pres;
								}
							}
						}
						i++;
					}

					if (yesterdayMaxTime != 0)
					{
						if (yesterdayVal < currVal)
						{
							sb.AppendC(UTF8STRC(" +"));
						}
						else
						{
							sb.AppendC(UTF8STRC(" "));
						}
						sb.AppendDouble(currVal - yesterdayVal);
					}
				}
				else
				{
					i = 0;
					j = dev->yesterdayRecs.GetCount();
					while (i < j)
					{
						rec = dev->yesterdayRecs.GetItemNoCheck(i);
						if (rec->nreading > readingIndex && ReadInt16(&rec->readings[readingIndex].status[0]) == sensorId && ReadInt16(&rec->readings[readingIndex].status[4]) == readingId)
						{
							if (ReadInt16(&rec->readings[readingIndex].status[6]) == readingType)
							{
								thisTime = rec->recTime + 86400000LL;
								if (thisTime <= maxTime)
								{
									dateList2.Add(thisTime);
									valList2.Add(rec->readings[readingIndex].reading);
									if (thisTime > yesterdayMaxTime)
									{
										yesterdayMaxTime = thisTime;
										yesterdayVal = rec->readings[readingIndex].reading;
									}
								}
							}
						}
						else
						{
							k = rec->nreading;
							while (k-- > 0)
							{
								if (ReadInt16(&rec->readings[k].status[0]) == sensorId && ReadInt16(&rec->readings[k].status[4]) == readingId)
								{
									if (ReadInt16(&rec->readings[k].status[6]) == readingType)
									{
										thisTime = rec->recTime + 86400000LL;
										if (thisTime <= maxTime)
										{
											dateList2.Add(thisTime);
											valList2.Add(rec->readings[k].reading);
											if (thisTime > yesterdayMaxTime)
											{
												yesterdayMaxTime = thisTime;
												yesterdayVal = rec->readings[k].reading;
											}
										}
									}
									break;
								}
							}
						}
						i++;
					}

					if (yesterdayMaxTime != 0)
					{
						if (yesterdayVal < dev->readings[readingIndex].reading)
						{
							sb.AppendC(UTF8STRC(" +"));
						}
						else
						{
							sb.AppendC(UTF8STRC(" "));
						}
						sb.AppendDouble(dev->readings[readingIndex].reading - yesterdayVal);
					}
				}

				NEW_CLASS(chart, Data::ChartPlotter(sb.ToCString()));
				if (dateList2.GetCount() >= 2)
				{
					chart->AddLineChart(CSTR("Yesterday"), Data::ChartPlotter::NewData(valList2), Data::ChartPlotter::NewDataDate(dateList2.Arr(), dateList2.GetCount()), 0xffcccccc);
				}
				chart->AddLineChart(CSTR("Reading"), Data::ChartPlotter::NewData(valList), Data::ChartPlotter::NewDataDate(dateList.Arr(), dateList.GetCount()), 0xffff0000);

				chart->SetDateFormat(CSTR(""));
				chart->SetFontHeightPt(10);
				chart->SetTimeZoneQHR(32);
				chart->SetTimeFormat(CSTR("HH:mm"));
				chart->Plot(dimg, 0, 0, UOSInt2Double(dimg->GetWidth()), UOSInt2Double(dimg->GetHeight()));
				DEL_CLASS(chart);
			}
			else
			{
				f = dimg->NewFontPx(CSTR("Arial"), 12, Media::DrawEngine::DFS_ANTIALIAS, 0);
				b = dimg->NewBrushARGB(0xffffffff);
				dimg->DrawRect(Math::Coord2DDbl(0, 0), dimg->GetSize().ToDouble(), 0, b);
				dimg->DelBrush(b);
				b = dimg->NewBrushARGB(0xff000000);
				dimg->DrawString(Math::Coord2DDbl(0, 0), sb.ToCString(), f, b);
				dimg->DrawString(Math::Coord2DDbl(0, 12), CSTR("Data not enough"), f, b);
				dimg->DelBrush(b);
				dimg->DelFont(f);
			}
		}
		mutUsage.EndUse();
		
		Exporter::GUIPNGExporter *exporter;
		NN<Media::ImageList> imgList;
		NN<Media::StaticImage> simg;

		NEW_CLASSNN(imgList, Media::ImageList(CSTR("temp.png")));
		if (dimg->ToStaticImage().SetTo(simg))
		{
			imgList->AddImage(simg, 0);
		}
		deng->DeleteImage(dimg);

		NEW_CLASSNN(mstm, IO::MemoryStream());
		NEW_CLASS(exporter, Exporter::GUIPNGExporter());
		exporter->ExportFile(mstm, CSTR("temp.png"), imgList, 0);
		DEL_CLASS(exporter);
		imgList.Delete();

		buff = mstm->GetBuff(buffSize);
		resp->AddDefHeaders(req);
		resp->AddContentType(CSTR("image/png"));
		resp->AddContentLength(buffSize);
		resp->AddHeader(CSTR("Cache-Control"), CSTR("no-cache"));
		resp->Write(Data::ByteArrayR(buff, buffSize));

		mutUsage.ReplaceMutex(dev->mut, true);
		if (dev->imgCaches.Put((sensorId << 16) + (readingId << 8) + (readingType), mstm).SetTo(mstm))
		{
			mstm.Delete();
		}
		return true;
	}
	else
	{
		return resp->ResponseError(req, Net::WebStatus::SC_INTERNAL_SERVER_ERROR);
	}
}

Bool __stdcall SSWR::SMonitor::SMonitorWebHandler::DevicePastDataReq(NN<SSWR::SMonitor::SMonitorWebHandler> me, NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp)
{
	Text::UTF8Writer *writer;
	Int32 userId = 0;
	Int32 userType = 0;
	UnsafeArray<UInt8> buff;
	UOSInt buffSize;
	UTF8Char sbuff[64];
	UnsafeArray<UTF8Char> sptr;
	NN<Text::String> s;
	Net::WebServer::WebSession *sess = me->sessMgr->GetSession(req, resp).OrNull();
	if (sess)
	{
		userId = sess->GetValueInt32(CSTR("UserId"));
		userType = sess->GetValueInt32(CSTR("UserType"));
	}

	UOSInt i;
	UOSInt j;
	UOSInt k;
	UOSInt l;
	Data::ArrayListNN<SSWR::SMonitor::SMonitorCore::DeviceInfo> devList;
	NN<SSWR::SMonitor::SMonitorCore::DeviceInfo> dev;
	IO::MemoryStream mstm;
	NEW_CLASS(writer, Text::UTF8Writer(mstm));
	WriteHeaderBegin(writer);
	writer->WriteLine(CSTR("<script type=\"text/javascript\">"));
	writer->WriteLine(CSTR("var clients = new Object();"));
	writer->WriteLine(CSTR("var cli;"));
	writer->WriteLine(CSTR("var reading;"));
	me->core->UserGetDevices(userId, userType, devList);
	i = 0;
	j = devList.GetCount();
	while (i < j)
	{
		dev = devList.GetItemNoCheck(i);
		writer->WriteLine(CSTR("cli = new Object();"));
		writer->Write(CSTR("cli.cliId = "));
		sptr = Text::StrInt64(sbuff, dev->cliId);
		writer->Write(CSTRP(sbuff, sptr));
		writer->WriteLine(CSTR(";"));
		Sync::RWMutexUsage mutUsage(dev->mut, false);
		writer->Write(CSTR("cli.name = "));
		if (dev->devName.SetTo(s))
		{
			WriteJSText(writer, s);
		}
		else
		{
			WriteJSText(writer, dev->platformName);
		}
		writer->WriteLine(CSTR(";"));
		writer->WriteLine(CSTR("cli.readings = new Array();"));
		UnsafeArray<const UTF8Char> nns;
		k = 0;
		l = dev->nReading;
		while (k < l)
		{
			writer->WriteLine(CSTR("reading = new Object();"));
			writer->Write(CSTR("reading.name = "));
			if (dev->readingNames[k].SetTo(nns))
			{
				WriteJSText(writer, nns);
			}
			else
			{
				Text::StrInt32(Text::StrConcatC(Text::StrInt32(Text::StrConcatC(sbuff, UTF8STRC("Sensor ")), ReadInt16(dev->readings[k].status)), UTF8STRC(" Reading ")), ReadInt16(&dev->readings[k].status[4]));
				WriteJSText(writer, sbuff);
			}
			writer->WriteLine(CSTR(";"));
			writer->Write(CSTR("reading.sensor = "));
			sptr = Text::StrInt32(sbuff, ReadInt16(dev->readings[k].status));
			writer->Write(CSTRP(sbuff, sptr));
			writer->WriteLine(CSTR(";"));
			writer->Write(CSTR("reading.reading = "));
			sptr = Text::StrInt32(sbuff, ReadInt16(&dev->readings[k].status[4]));
			writer->Write(CSTRP(sbuff, sptr));
			writer->WriteLine(CSTR(";"));

			writer->WriteLine(CSTR("cli.readings.push(reading);"));
			k++;
		}
		writer->WriteLine(CSTR("clients[cli.cliId] = cli;"));
		mutUsage.EndUse();
		i++;
	}

	writer->WriteLine(CSTR("</script>"));
	writer->WriteLine(CSTR("<script type=\"text/javascript\" src=\"files/pastdata.js\">"));
	writer->WriteLine(CSTR("</script>"));
	WriteHeaderEnd(writer);
	writer->WriteLine(CSTR("<body onload=\"afterLoad()\">"));
	writer->WriteLine(CSTR("<table width=\"100%\"><tr><td width=\"100\" class=\"menu\">"));
	me->WriteMenu(writer, sess);
	writer->WriteLine(CSTR("</td><td>"));
	writer->WriteLine(CSTR("<h2>Past Data</h2>"));
	writer->WriteLine(CSTR("Device Name<select name=\"cliId\" id=\"cliId\" onchange=\"devChg()\"></select><br/>"));
	writer->WriteLine(CSTR("Reading<select name=\"reading\" id=\"reading\" onchange=\"selChg()\"></select><br/>"));
	writer->WriteLine(CSTR("Date<select name=\"year\" id=\"year\" onchange=\"selChg()\"></select><select name=\"month\" id=\"month\" onchange=\"selChg()\"></select><select name=\"day\" id=\"day\" onchange=\"selChg()\"></select><br/>"));
	writer->WriteLine(CSTR("<img src=\"about:blank\" id=\"dataimg\"/>"));

	writer->WriteLine(CSTR("</td></tr></table></body>"));
	writer->WriteLine(CSTR("</html>"));
	if (sess)
	{
		sess->EndUse();
	}

	DEL_CLASS(writer);
	buff = mstm.GetBuff(buffSize);
	resp->AddDefHeaders(req);
	resp->AddContentType(CSTR("text/html"));
	resp->AddContentLength(buffSize);
	resp->AddHeader(CSTR("Cache-Control"), CSTR("no-cache"));
	resp->Write(Data::ByteArrayR(buff, buffSize));
	return true;
}

Bool __stdcall SSWR::SMonitor::SMonitorWebHandler::DevicePastDataImgReq(NN<SSWR::SMonitor::SMonitorWebHandler> me, NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp)
{
	NN<Text::String> s;
	Int64 cliId = 0;
	Int64 startTime = 0;
	Int32 userId = 0;
	Int32 userType = 0;
	Int32 sensorId = 0;
	Int32 readingId = 0;
	Bool valid = true;
	if (!req->GetQueryValue(CSTR("id")).SetTo(s))
	{
		valid = false;
	}
	else if (!s->ToInt64(cliId))
	{
		valid = false;
	}

	if (!req->GetQueryValue(CSTR("sensor")).SetTo(s))
	{
		valid = false;
	}
	else if (!s->ToInt32(sensorId))
	{
		valid = false;
	}

	if (!req->GetQueryValue(CSTR("reading")).SetTo(s))
	{
		valid = false;
	}
	else if (!s->ToInt32(readingId))
	{
		valid = false;
	}

	if (!req->GetQueryValue(CSTR("starttime")).SetTo(s))
	{
		valid = false;
	}
	else if (!s->ToInt64(startTime))
	{
		valid = false;
	}

	if (!valid)
	{
		resp->ResponseError(req, Net::WebStatus::SC_NOT_FOUND);
		return true;
	}

	NN<Net::WebServer::WebSession> sess;
	if (me->sessMgr->GetSession(req, resp).SetTo(sess))
	{
		userId = sess->GetValueInt32(CSTR("UserId"));
		userType = sess->GetValueInt32(CSTR("UserType"));
		sess->EndUse();
	}
	NN<SSWR::SMonitor::SMonitorCore::DeviceInfo> dev;
	if (!me->core->UserHasDevice(userId, userType, cliId) || !me->core->DeviceGet(cliId).SetTo(dev))
	{
		resp->ResponseError(req, Net::WebStatus::SC_NOT_FOUND);
		return true;
	}

	Data::ArrayListNN<SSWR::SMonitor::SMonitorCore::DevRecord2> recList;
	UOSInt i;
	UOSInt j;
	UOSInt k;
	UnsafeArray<UInt8> buff;
	UOSInt buffSize;

	NN<Media::DrawEngine> deng = me->core->GetDrawEngine();
	NN<Media::DrawImage> dimg;
	if (deng->CreateImage32(Math::Size2D<UOSInt>(640, 120), Media::AT_ALPHA_ALL_FF).SetTo(dimg))
	{
		NN<Media::DrawFont> f;
		NN<Media::DrawBrush> b;
		UOSInt readingIndex = (UOSInt)-1;
		Int32 readingType = 0;
		Data::ChartPlotter *chart;
		Sync::RWMutexUsage mutUsage(dev->mut, false);
		i = dev->nReading;
		while (i-- > 0)
		{
			if (ReadInt16(&dev->readings[i].status[0]) == sensorId && ReadInt16(&dev->readings[i].status[4]) == readingId)
			{
				readingIndex = i;
				readingType = ReadInt16(&dev->readings[i].status[6]);
				break;
			}
		}
		mutUsage.EndUse();
		if (readingIndex == (UOSInt)-1)
		{
			f = dimg->NewFontPx(CSTR("Arial"), 12, Media::DrawEngine::DFS_ANTIALIAS, 0);
			b = dimg->NewBrushARGB(0xffffffff);
			dimg->DrawRect(Math::Coord2DDbl(0, 0), dimg->GetSize().ToDouble(), 0, b);
			dimg->DelBrush(b);
			b = dimg->NewBrushARGB(0xff000000);
			dimg->DrawString(Math::Coord2DDbl(0, 0), CSTR("Sensor not found"), f, b);
			dimg->DelBrush(b);
			dimg->DelFont(f);
		}
		else
		{
			UTF8Char sbuff[64];
			UnsafeArray<UTF8Char> sptr;
			Data::ArrayListInt64 dateList;
			Data::ArrayListDbl valList;
			NN<SSWR::SMonitor::SMonitorCore::DevRecord2> rec;
			me->core->DeviceQueryRec(cliId, startTime, startTime + 86400000, recList);
			i = 0;
			j = recList.GetCount();
			while (i < j)
			{
				rec = recList.GetItemNoCheck(i);
				if (rec->nreading > readingIndex && ReadInt16(&rec->readings[readingIndex].status[0]) == sensorId && ReadInt16(&rec->readings[readingIndex].status[4]) == readingId)
				{
					if (ReadInt16(&rec->readings[readingIndex].status[6]) == readingType)
					{
						dateList.Add(rec->recTime);
						valList.Add(rec->readings[readingIndex].reading);
					}
				}
				else
				{
					k = rec->nreading;
					while (k-- > 0)
					{
						if (ReadInt16(&rec->readings[k].status[0]) == sensorId && ReadInt16(&rec->readings[k].status[4]) == readingId)
						{
							if (ReadInt16(&rec->readings[k].status[6]) == readingType)
							{
								dateList.Add(rec->recTime);
								valList.Add(rec->readings[k].reading);
							}
							break;
						}
					}
				}
				MemFreeNN(rec);
				i++;
			}

			Text::StringBuilderUTF8 sb;
			Sync::RWMutexUsage mutUsage(dev->mut, false);
			UnsafeArray<const UTF8Char> nns;
			if (dev->readingNames[readingIndex].SetTo(nns))
			{
				sb.AppendSlow(nns);
			}
			else
			{
				sb.AppendC(UTF8STRC("Sensor "));
				sb.AppendI32(ReadInt16(dev->readings[readingIndex].status));
				if (ReadInt16(&dev->readings[readingIndex].status[6]) != SSWR::SMonitor::SAnalogSensor::RT_UNKNOWN)
				{
					sb.AppendC(UTF8STRC(" "));
					sb.Append(SSWR::SMonitor::SAnalogSensor::GetReadingTypeName((SSWR::SMonitor::SAnalogSensor::ReadingType)ReadInt16(&dev->readings[readingIndex].status[6])));
				}
			}
			mutUsage.EndUse();
			Data::DateTime dt;
			dt.SetTicks(startTime);
			dt.ToLocalTime();
			sptr = dt.ToString(sbuff, "yyyy-MM-dd");
			sb.AppendC(UTF8STRC(" "));
			sb.AppendP(sbuff, sptr);

			if (dateList.GetCount() >= 2)
			{
				NEW_CLASS(chart, Data::ChartPlotter(sb.ToCString()));
				chart->AddLineChart(CSTR("Reading"), Data::ChartPlotter::NewData(valList), Data::ChartPlotter::NewDataDate(dateList.Arr(), dateList.GetCount()), 0xffff0000);
				chart->SetDateFormat(CSTR(""));
				chart->SetFontHeightPt(10);
				chart->SetTimeZoneQHR(32);
				chart->SetTimeFormat(CSTR("HH:mm"));
				chart->Plot(dimg, 0, 0, UOSInt2Double(dimg->GetWidth()), UOSInt2Double(dimg->GetHeight()));
				DEL_CLASS(chart);
			}
			else
			{
				f = dimg->NewFontPx(CSTR("Arial"), 12, Media::DrawEngine::DFS_ANTIALIAS, 0);
				b = dimg->NewBrushARGB(0xffffffff);
				dimg->DrawRect(Math::Coord2DDbl(0, 0), dimg->GetSize().ToDouble(), 0, b);
				dimg->DelBrush(b);
				b = dimg->NewBrushARGB(0xff000000);
				dimg->DrawString(Math::Coord2DDbl(0, 0), sb.ToCString(), f, b);
				dimg->DrawString(Math::Coord2DDbl(0, 12), CSTR("Data not enough"), f, b);
				dimg->DelBrush(b);
				dimg->DelFont(f);
			}
		}
		
		Exporter::GUIPNGExporter *exporter;
		NN<Media::ImageList> imgList;
		NN<Media::StaticImage> simg;

		NEW_CLASSNN(imgList, Media::ImageList(CSTR("temp.png")));
		if (dimg->ToStaticImage().SetTo(simg))
		{
			imgList->AddImage(simg, 0);
		}
		deng->DeleteImage(dimg);

		IO::MemoryStream mstm;
		NEW_CLASS(exporter, Exporter::GUIPNGExporter());
		exporter->ExportFile(mstm, CSTR("temp.png"), imgList, 0);
		DEL_CLASS(exporter);
		imgList.Delete();

		buff = mstm.GetBuff(buffSize);
		resp->AddDefHeaders(req);
		resp->AddContentType(CSTR("image/png"));
		resp->AddContentLength(buffSize);
		resp->AddHeader(CSTR("Cache-Control"), CSTR("no-cache"));
		resp->Write(Data::ByteArrayR(buff, buffSize));
		return true;
	}
	else
	{
		return resp->ResponseError(req, Net::WebStatus::SC_INTERNAL_SERVER_ERROR);
	}
}

Bool __stdcall SSWR::SMonitor::SMonitorWebHandler::UserPasswordReq(NN<SSWR::SMonitor::SMonitorWebHandler> me, NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp)
{
	Text::UTF8Writer *writer;
	UnsafeArray<UInt8> buff;
	UOSInt buffSize;
	NN<Net::WebServer::WebSession> sess;
	Text::CString msg = CSTR_NULL;
	if (!me->sessMgr->GetSession(req, resp).SetTo(sess))
	{
		return resp->RedirectURL(req, CSTR("/monitor/index"), 0);
	}

	if (req->GetReqMethod() == Net::WebUtil::RequestMethod::HTTP_POST)
	{
		req->ParseHTTPForm();
		NN<Text::String> pwd;
		NN<Text::String> retype;
		if (!req->GetHTTPFormStr(CSTR("password")).SetTo(pwd) || pwd->v[0] == 0)
		{
			msg = CSTR("Password is empty");
		}
		else if (!req->GetHTTPFormStr(CSTR("retype")).SetTo(retype) || retype->v[0] == 0)
		{
			msg = CSTR("Retype is empty");
		}
		else if (!pwd->Equals(retype))
		{
			msg = CSTR("Password and retype do not match");
		}
		else
		{
			UOSInt len = pwd->leng;
			if (len < 3)
			{
				msg = CSTR("Password is too short");
			}
			else
			{
				if (me->core->UserSetPassword(sess->GetValueInt32(CSTR("UserId")), pwd->v))
				{
					msg = CSTR("Password is changed successfully");
				}
				else
				{
					msg = CSTR("Error in changing password");
				}
			}
		}
	}

	IO::MemoryStream mstm;
	NEW_CLASS(writer, Text::UTF8Writer(mstm));
	WriteHeaderBegin(writer);
	WriteHeaderEnd(writer);
	writer->WriteLine(CSTR("<body onload=\"document.getElementById('password').focus()\">"));
	writer->WriteLine(CSTR("<table width=\"100%\"><tr><td width=\"100\" class=\"menu\">"));
	me->WriteMenu(writer, sess);
	writer->WriteLine(CSTR("</td><td>"));
	writer->WriteLine(CSTR("<h2>Modify Password</h2>"));
	writer->WriteLine(CSTR("<center>"));
	writer->WriteLine(CSTR("<form method=\"POST\" action=\"userpassword\">"));
	writer->WriteLine(CSTR("<table border=\"0\"><tr><td>Password</td><td><input type=\"password\" name=\"password\" id=\"password\"/></td></tr>"));
	writer->WriteLine(CSTR("<tr><td>Retype</td><td><input type=\"password\" name=\"retype\" id=\"retype\"/></td></tr>"));
	writer->WriteLine(CSTR("<tr><td></td><td><input type=\"submit\"/></td></tr>"));
	writer->WriteLine(CSTR("</table>"));
	Text::CStringNN nnmsg;
	if (msg.SetTo(nnmsg))
	{
		WriteHTMLText(writer, nnmsg);
	}
	writer->WriteLine(CSTR("</form></center>"));
	writer->WriteLine(CSTR("</td></tr></table></body>"));
	writer->WriteLine(CSTR("</html>"));
	sess->EndUse();

	DEL_CLASS(writer);
	buff = mstm.GetBuff(buffSize);
	resp->AddDefHeaders(req);
	resp->AddContentType(CSTR("text/html"));
	resp->AddContentLength(buffSize);
	resp->AddHeader(CSTR("Cache-Control"), CSTR("no-cache"));
	resp->Write(Data::ByteArrayR(buff, buffSize));
	return true;
}

Bool __stdcall SSWR::SMonitor::SMonitorWebHandler::UsersReq(NN<SSWR::SMonitor::SMonitorWebHandler> me, NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp)
{
	Text::UTF8Writer *writer;
	UnsafeArray<UInt8> buff;
	UOSInt buffSize;
	UTF8Char sbuff[64];
	UnsafeArray<UTF8Char> sptr;
	NN<Net::WebServer::WebSession> sess;
	if (!me->sessMgr->GetSession(req, resp).SetTo(sess))
	{
		return resp->RedirectURL(req, CSTR("/monitor/index"), 0);
	}
	if (sess->GetValueInt32(CSTR("UserType")) != 1)
	{
		sess->EndUse();
		return resp->RedirectURL(req, CSTR("/monitor/index"), 0);
	}

	Data::ArrayListNN<SSWR::SMonitor::SMonitorCore::WebUser> userList;
	NN<SSWR::SMonitor::SMonitorCore::WebUser> user;
	UOSInt i;
	UOSInt j;
	me->core->UserGetList(userList);
	IO::MemoryStream mstm;
	NEW_CLASS(writer, Text::UTF8Writer(mstm));
	WriteHeaderBegin(writer);
	WriteHeaderEnd(writer);
	writer->WriteLine(CSTR("<body>"));
	writer->WriteLine(CSTR("<table width=\"100%\"><tr><td width=\"100\" class=\"menu\">"));
	me->WriteMenu(writer, sess);
	writer->WriteLine(CSTR("</td><td>"));
	writer->WriteLine(CSTR("<h2>Users</h2>"));
	writer->WriteLine(CSTR("<a href=\"useradd\">Add User</a><br/>"));
	writer->WriteLine(CSTR("<table border=\"1\"><tr><td>User Name</td><td>Action</td></tr>"));
	i = 0;
	j = userList.GetCount();
	while (i < j)
	{
		user = userList.GetItemNoCheck(i);
		writer->Write(CSTR("<tr><td>"));
		Sync::RWMutexUsage mutUsage(user->mut, false);
		sptr = Text::StrInt32(sbuff, user->userId);
		WriteHTMLText(writer, user->userName);
		mutUsage.EndUse();
		writer->Write(CSTR("</td><td><a href=\"userreset?id="));
		writer->Write(CSTRP(sbuff, sptr));
		writer->Write(CSTR("\">Reset Password</a>"));
		writer->Write(CSTR("<br/><a href=\"userassign?id="));
		writer->Write(CSTRP(sbuff, sptr));
		writer->Write(CSTR("\">Assign Devices</a>"));
		writer->WriteLine(CSTR("</td></tr>"));
		i++;
	}
	writer->WriteLine(CSTR("</table>"));
	writer->WriteLine(CSTR("</td></tr></table></body>"));
	writer->WriteLine(CSTR("</html>"));
	sess->EndUse();

	DEL_CLASS(writer);
	buff = mstm.GetBuff(buffSize);
	resp->AddDefHeaders(req);
	resp->AddContentType(CSTR("text/html"));
	resp->AddContentLength(buffSize);
	resp->AddHeader(CSTR("Cache-Control"), CSTR("no-cache"));
	resp->Write(Data::ByteArrayR(buff, buffSize));
	return true;
}

Bool __stdcall SSWR::SMonitor::SMonitorWebHandler::UserAddReq(NN<SSWR::SMonitor::SMonitorWebHandler> me, NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp)
{
	Text::UTF8Writer *writer;
	UnsafeArray<UInt8> buff;
	UOSInt buffSize;
	NN<Net::WebServer::WebSession> sess;
	if (!me->sessMgr->GetSession(req, resp).SetTo(sess))
	{
		return resp->RedirectURL(req, CSTR("/monitor/index"), 0);
	}
	if (sess->GetValueInt32(CSTR("UserType")) != 1)
	{
		sess->EndUse();
		return resp->RedirectURL(req, CSTR("/monitor/index"), 0);
	}

	if (req->GetReqMethod() == Net::WebUtil::RequestMethod::HTTP_POST)
	{
		req->ParseHTTPForm();
		NN<Text::String> action;
		NN<Text::String> userName;
		if (req->GetHTTPFormStr(CSTR("action")).SetTo(action) && req->GetHTTPFormStr(CSTR("username")).SetTo(userName) && action->Equals(UTF8STRC("adduser")))
		{
			UOSInt len = userName->leng;
			if (len >= 3 && len < 128)
			{
				if (me->core->UserAdd(userName->v, userName->v, 2))
				{
					sess->EndUse();
					return resp->RedirectURL(req, CSTR("/monitor/users"), 0);
				}
			}
		}
	}

	IO::MemoryStream mstm;
	NEW_CLASS(writer, Text::UTF8Writer(mstm));
	WriteHeaderBegin(writer);
	WriteHeaderEnd(writer);
	writer->WriteLine(CSTR("<body onload=\"document.getElementById('username').focus()\">"));
	writer->WriteLine(CSTR("<table width=\"100%\"><tr><td width=\"100\" class=\"menu\">"));
	me->WriteMenu(writer, sess);
	writer->WriteLine(CSTR("</td><td>"));
	writer->WriteLine(CSTR("<h2>Add User</h2>"));
	writer->WriteLine(CSTR("<form name=\"adduser\" method=\"POST\" action=\"useradd\">"));
	writer->WriteLine(CSTR("<input type=\"hidden\" name=\"action\" value=\"adduser\"/>"));
	writer->WriteLine(CSTR("<table border=\"1\"><tr><td>User Name</td><td><input type=\"text\" name=\"username\" id=\"username\" /></td></tr>"));
	writer->WriteLine(CSTR("<tr><td></td><td><input type=\"submit\" /></td></tr>"));
	writer->WriteLine(CSTR("</table>"));
	writer->WriteLine(CSTR("</td></tr></table></body>"));
	writer->WriteLine(CSTR("</html>"));
	sess->EndUse();

	DEL_CLASS(writer);
	buff = mstm.GetBuff(buffSize);
	resp->AddDefHeaders(req);
	resp->AddContentType(CSTR("text/html"));
	resp->AddContentLength(buffSize);
	resp->AddHeader(CSTR("Cache-Control"), CSTR("no-cache"));
	resp->Write(Data::ByteArrayR(buff, buffSize));
	return true;
}

Bool __stdcall SSWR::SMonitor::SMonitorWebHandler::UserAssignReq(NN<SSWR::SMonitor::SMonitorWebHandler> me, NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp)
{
	Text::UTF8Writer *writer;
	UnsafeArray<UInt8> buff;
	UOSInt buffSize;
	UTF8Char sbuff[64];
	UnsafeArray<UTF8Char> sptr;
	NN<Net::WebServer::WebSession> sess;
	NN<SSWR::SMonitor::SMonitorCore::WebUser> user;
	Int32 userId;
	UOSInt i;
	UOSInt j;
	NN<Text::String> s;

	if (!req->GetQueryValueI32(CSTR("id"), userId))
	{
		return resp->RedirectURL(req, CSTR("/monitor/users"), 0);
	}
	if (!me->core->UserGet(userId).SetTo(user) || user->userType != 2)
	{
		return resp->RedirectURL(req, CSTR("/monitor/users"), 0);
	}
	if (!me->sessMgr->GetSession(req, resp).SetTo(sess))
	{
		return resp->RedirectURL(req, CSTR("/monitor/index"), 0);
	}
	if (sess->GetValueInt32(CSTR("UserType")) != 1)
	{
		sess->EndUse();
		return resp->RedirectURL(req, CSTR("/monitor/index"), 0);
	}

	if (req->GetReqMethod() == Net::WebUtil::RequestMethod::HTTP_POST)
	{
		req->ParseHTTPForm();
		NN<Text::String> action;
		NN<Text::String> devicestr;
		if (req->GetHTTPFormStr(CSTR("action")).SetTo(action) && req->GetHTTPFormStr(CSTR("device")).SetTo(devicestr) && action->Equals(UTF8STRC("userassign")))
		{
			Data::ArrayListInt64 devIds;
			UnsafeArray<UTF8Char> sarr[2];
			Text::StringBuilderUTF8 sb;
			Int64 cliId;
			Bool valid = true;
			sb.Append(devicestr);
			sarr[1] = sb.v;
			while (true)
			{
				i = Text::StrSplit(sarr, 2, sarr[1], ',');
				if (!Text::StrToInt64(sarr[0], cliId))
				{
					valid = false;
					break;
				}
				devIds.Add(cliId);
				if (i != 2)
					break;
			}

			if (valid && me->core->UserAssign(userId, devIds))
			{
				sess->EndUse();
				return resp->RedirectURL(req, CSTR("/monitor/users"), 0);
			}
		}
	}

	Data::ArrayListNN<SSWR::SMonitor::SMonitorCore::DeviceInfo> devList;
	NN<SSWR::SMonitor::SMonitorCore::DeviceInfo> dev;
	me->core->UserGetDevices(sess->GetValueInt32(CSTR("UserId")), 1, devList);

	IO::MemoryStream mstm;
	NEW_CLASS(writer, Text::UTF8Writer(mstm));
	WriteHeaderBegin(writer);
	WriteHeaderEnd(writer);
	writer->WriteLine(CSTR("<body onload=\"document.getElementById('username').focus()\">"));
	writer->WriteLine(CSTR("<table width=\"100%\"><tr><td width=\"100\" class=\"menu\">"));
	me->WriteMenu(writer, sess);
	writer->WriteLine(CSTR("</td><td>"));
	writer->WriteLine(CSTR("<h2>User Assign</h2>"));
	writer->Write(CSTR("User Name: "));
	Sync::RWMutexUsage userMutUsage(user->mut, false);
	WriteHTMLText(writer, user->userName);
	writer->WriteLine(CSTR("<br/>"));
	writer->Write(CSTR("<form name=\"userassign\" method=\"POST\" action=\"userassign?id="));
	sptr = Text::StrInt32(sbuff, user->userId);
	writer->Write(CSTRP(sbuff, sptr));
	writer->WriteLine(CSTR("\">"));
	writer->WriteLine(CSTR("<input type=\"hidden\" name=\"action\" value=\"userassign\"/>"));
	writer->WriteLine(CSTR("Device List<br/>"));
	i = 0;
	j = devList.GetCount();
	while (i < j)
	{
		dev = devList.GetItemNoCheck(i);
		Sync::RWMutexUsage devMutUsage(dev->mut, false);
		writer->Write(CSTR("<input type=\"checkbox\" name=\"device\" id=\"device"));
		sptr = Text::StrInt64(sbuff, dev->cliId);
		writer->Write(CSTRP(sbuff, sptr));
		writer->Write(CSTR("\" value=\""));
		writer->Write(CSTRP(sbuff, sptr));
		writer->Write(CSTR("\""));
		if (user->devMap.Get(dev->cliId).NotNull())
		{
			writer->Write(CSTR(" checked"));
		}
		writer->Write(CSTR("/><label for=\"device"));
		writer->Write(CSTRP(sbuff, sptr));
		writer->Write(CSTR("\">"));
		if (dev->devName.SetTo(s))
		{
			WriteHTMLText(writer, s);
		}
		else
		{
			WriteHTMLText(writer, dev->platformName);
		}
		writer->Write(CSTR("</label><br/>"));
		devMutUsage.EndUse();
		i++;
	}
	userMutUsage.EndUse();
	writer->WriteLine(CSTR("<input type=\"submit\" />"));
	writer->WriteLine(CSTR("</td></tr></table></body>"));
	writer->WriteLine(CSTR("</html>"));
	sess->EndUse();

	DEL_CLASS(writer);
	buff = mstm.GetBuff(buffSize);
	resp->AddDefHeaders(req);
	resp->AddContentType(CSTR("text/html"));
	resp->AddContentLength(buffSize);
	resp->AddHeader(CSTR("Cache-Control"), CSTR("no-cache"));
	resp->Write(Data::ByteArrayR(buff, buffSize));
	return true;
}

void __stdcall SSWR::SMonitor::SMonitorWebHandler::WriteHeaderBegin(IO::Writer *writer)
{
	writer->WriteLine(CSTR("<html><head><title>Monitor</title>"));
	writer->WriteLine(CSTR("<style>"));
	writer->WriteLine(CSTR(".menu {"));
	writer->WriteLine(CSTR("	vertical-align: top;"));
	writer->WriteLine(CSTR("}"));
	writer->WriteLine(CSTR("</style>"));
}

void __stdcall SSWR::SMonitor::SMonitorWebHandler::WriteHeaderEnd(IO::Writer *writer)
{
	writer->WriteLine(CSTR("</head>"));
}

void __stdcall SSWR::SMonitor::SMonitorWebHandler::WriteMenu(IO::Writer *writer, Optional<Net::WebServer::WebSession> sess)
{
	Int32 userType = 0;
	NN<Net::WebServer::WebSession> nnsess;
	if (sess.SetTo(nnsess))
	{
		nnsess->SetValueInt64(CSTR("LastSessTime"), Data::DateTimeUtil::GetCurrTimeMillis());
		userType = nnsess->GetValueInt32(CSTR("UserType"));
	}
	if (userType == 0)
	{
		writer->WriteLine(CSTR("<a href=\"login\">Login</a><br/>"));
	}
	if (userType != 0)
	{
		writer->WriteLine(CSTR("<a href=\"logout\">Logout</a><br/>"));
	}
	writer->WriteLine(CSTR("<br/>"));
	writer->WriteLine(CSTR("<a href=\"index\">Home</a><br/>"));
	if (userType != 0)
	{
		writer->WriteLine(CSTR("<a href=\"device\">Devices</a><br/>"));
		writer->WriteLine(CSTR("<a href=\"userpassword\">Password</a><br/>"));
	}
	writer->WriteLine(CSTR("<a href=\"pastdata\">Past Data</a><br/>"));
	if (userType == 1)
	{
		writer->WriteLine(CSTR("<a href=\"users\">Users</a><br/>"));
	}
}

void __stdcall SSWR::SMonitor::SMonitorWebHandler::WriteHTMLText(IO::Writer *writer, UnsafeArray<const UTF8Char> txt)
{
	NN<Text::String> xmlTxt = Text::XML::ToNewHTMLBodyText(txt);
	writer->Write(xmlTxt->ToCString());
	xmlTxt->Release();
}

void __stdcall SSWR::SMonitor::SMonitorWebHandler::WriteHTMLText(IO::Writer *writer, NN<Text::String> txt)
{
	NN<Text::String> xmlTxt = Text::XML::ToNewHTMLBodyText(txt->v);
	writer->Write(xmlTxt->ToCString());
	xmlTxt->Release();
}

void __stdcall SSWR::SMonitor::SMonitorWebHandler::WriteHTMLText(IO::Writer *writer, Text::CString txt)
{
	NN<Text::String> xmlTxt = Text::XML::ToNewHTMLBodyText(txt.OrEmpty().v);
	writer->Write(xmlTxt->ToCString());
	xmlTxt->Release();
}

void __stdcall SSWR::SMonitor::SMonitorWebHandler::WriteAttrText(IO::Writer *writer, UnsafeArray<const UTF8Char> txt)
{
	NN<Text::String> xmlTxt = Text::XML::ToNewAttrText(txt);
	writer->Write(xmlTxt->ToCString());
	xmlTxt->Release();
}

void __stdcall SSWR::SMonitor::SMonitorWebHandler::WriteAttrText(IO::Writer *writer, Text::String *txt)
{
	NN<Text::String> xmlTxt = Text::XML::ToNewAttrText(STR_PTR(txt));
	writer->Write(xmlTxt->ToCString());
	xmlTxt->Release();
}

void __stdcall SSWR::SMonitor::SMonitorWebHandler::WriteAttrText(IO::Writer *writer, NN<Text::String> txt)
{
	NN<Text::String> xmlTxt = Text::XML::ToNewAttrText(txt->v);
	writer->Write(xmlTxt->ToCString());
	xmlTxt->Release();
}

void __stdcall SSWR::SMonitor::SMonitorWebHandler::WriteJSText(IO::Writer *writer, UnsafeArray<const UTF8Char> txt)
{
	NN<Text::String> jsTxt = Text::JSText::ToNewJSText(UnsafeArrayOpt<const UTF8Char>(txt));
	writer->Write(jsTxt->ToCString());
	jsTxt->Release();
}

void __stdcall SSWR::SMonitor::SMonitorWebHandler::WriteJSText(IO::Writer *writer, Text::String *txt)
{
	NN<Text::String> jsTxt = Text::JSText::ToNewJSText(Optional<Text::String>(txt));
	writer->Write(jsTxt->ToCString());
	jsTxt->Release();
}

void __stdcall SSWR::SMonitor::SMonitorWebHandler::WriteJSText(IO::Writer *writer, NN<Text::String> txt)
{
	NN<Text::String> jsTxt = Text::JSText::ToNewJSText(txt);
	writer->Write(jsTxt->ToCString());
	jsTxt->Release();
}

Bool SSWR::SMonitor::SMonitorWebHandler::ProcessRequest(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp, Text::CStringNN subReq)
{
	if (this->DoRequest(req, resp, subReq))
	{
		return true;
	}
	RequestHandler reqHdlr = this->reqMap->GetC(subReq);
	if (reqHdlr)
	{
		return reqHdlr(*this, req, resp);
	}
	resp->ResponseError(req, Net::WebStatus::SC_NOT_FOUND);
	return true;
}

SSWR::SMonitor::SMonitorWebHandler::SMonitorWebHandler(SSWR::SMonitor::SMonitorCore *core)
{
	this->core = core;
	NEW_CLASS(this->reqMap, Data::FastStringMap<RequestHandler>());
	NEW_CLASS(this->sessMgr, Net::WebServer::MemoryWebSessionManager(CSTR("/monitor"), OnSessDeleted, this, 60000, OnSessCheck, this, CSTR("SMonSessId")));
	this->reqMap->PutC(CSTR(""), DefaultReq);
	this->reqMap->PutC(CSTR("/index"), IndexReq);
	this->reqMap->PutC(CSTR("/login"), LoginReq);
	this->reqMap->PutC(CSTR("/logout"), LogoutReq);
	this->reqMap->PutC(CSTR("/device"), DeviceReq);
	this->reqMap->PutC(CSTR("/devedit"), DeviceEditReq);
	this->reqMap->PutC(CSTR("/devreading"), DeviceReadingReq);
	this->reqMap->PutC(CSTR("/devdigitals"), DeviceDigitalsReq);
	this->reqMap->PutC(CSTR("/devreadingimg"), DeviceReadingImgReq);
	this->reqMap->PutC(CSTR("/pastdata"), DevicePastDataReq);
	this->reqMap->PutC(CSTR("/pastdataimg"), DevicePastDataImgReq);
	this->reqMap->PutC(CSTR("/userpassword"), UserPasswordReq);
	this->reqMap->PutC(CSTR("/users"), UsersReq);
	this->reqMap->PutC(CSTR("/useradd"), UserAddReq);
//	this->reqMap->PutC(CSTR("/userreset"), UserResetReq);
	this->reqMap->PutC(CSTR("/userassign"), UserAssignReq);
}

SSWR::SMonitor::SMonitorWebHandler::~SMonitorWebHandler()
{
	DEL_CLASS(this->sessMgr);
	DEL_CLASS(this->reqMap);
}
