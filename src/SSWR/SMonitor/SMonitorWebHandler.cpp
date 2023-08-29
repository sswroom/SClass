#include "Stdafx.h"
#include "Data/ByteTool.h"
#include "Data/LineChart.h"
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

Bool __stdcall SSWR::SMonitor::SMonitorWebHandler::OnSessDeleted(Net::WebServer::IWebSession* sess, void *userObj)
{
//	SSWR::SMonitor::SMonitorWebHandler *me = (SSWR::SMonitor::SMonitorWebHandler*)userObj;
	return false;
}

Bool __stdcall SSWR::SMonitor::SMonitorWebHandler::OnSessCheck(Net::WebServer::IWebSession* sess, void *userObj)
{
//	SSWR::SMonitor::SMonitorWebHandler *me = (SSWR::SMonitor::SMonitorWebHandler*)userObj;
	Data::DateTime dt;
	dt.SetCurrTimeUTC();
	Int64 t = dt.ToTicks() - sess->GetValueInt64(UTF8STRC("LastSessTime"));
	if (t > 900000)
	{
		return true;
	}
	return false;
}

Bool __stdcall SSWR::SMonitor::SMonitorWebHandler::DefaultReq(SSWR::SMonitor::SMonitorWebHandler *me, NotNullPtr<Net::WebServer::IWebRequest> req, NotNullPtr<Net::WebServer::IWebResponse> resp)
{
	return resp->RedirectURL(req, CSTR("/monitor/index"), -2);
}

Bool __stdcall SSWR::SMonitor::SMonitorWebHandler::IndexReq(SSWR::SMonitor::SMonitorWebHandler *me, NotNullPtr<Net::WebServer::IWebRequest> req, NotNullPtr<Net::WebServer::IWebResponse> resp)
{
	Text::UTF8Writer *writer;
	UInt8 *buff;
	UOSInt buffSize;
	UTF8Char sbuff[64];
	UTF8Char *sptr;
	IO::MemoryStream mstm;

	if (!me->core->UserExist())
	{
		if (req->GetReqMethod() == Net::WebUtil::RequestMethod::HTTP_POST)
		{
			req->ParseHTTPForm();
			Text::String *pwd = req->GetHTTPFormStr(CSTR("password"));
			Text::String *retype = req->GetHTTPFormStr(CSTR("password"));
			if (pwd && retype)
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
		writer->WriteLineC(UTF8STRC("<body onload=\"document.getElementById('pwd').focus()\"><center>"));
		writer->WriteLineC(UTF8STRC("<form name=\"createForm\" method=\"POST\" action=\"index\">"));
		writer->WriteLineC(UTF8STRC("<table>"));
		writer->WriteLineC(UTF8STRC("<tr><td></td><td>Password for admin</td></tr>"));
		writer->WriteLineC(UTF8STRC("<tr><td>Password</td><td><input type=\"password\" name=\"password\" id=\"pwd\"/></td></tr>"));
		writer->WriteLineC(UTF8STRC("<tr><td>Retype</td><td><input type=\"password\" name=\"retype\"/></td></tr>"));
		writer->WriteLineC(UTF8STRC("<tr><td></td><td><input type=\"submit\"/></td></tr>"));
		writer->WriteLineC(UTF8STRC("</table>"));
		writer->WriteLineC(UTF8STRC("</form"));
		writer->WriteLineC(UTF8STRC("</center></body>"));
		writer->WriteLineC(UTF8STRC("</html>"));
	}
	else
	{
		Net::WebServer::IWebSession *sess = me->sessMgr->GetSession(req, resp);
		NEW_CLASS(writer, Text::UTF8Writer(mstm));
		WriteHeaderBegin(writer);
		WriteHeaderEnd(writer);
		writer->WriteLineC(UTF8STRC("<body onload=\"window.setTimeout(new Function('document.location.replace(\\'index\\')'), 60000)\">"));
		writer->WriteLineC(UTF8STRC("<table width=\"100%\"><tr><td width=\"100\" class=\"menu\">"));
		me->WriteMenu(writer, sess);
		writer->WriteLineC(UTF8STRC("</td><td>"));
		Int32 userId;
		Int32 userType;
		if (sess)
		{
			userId = sess->GetValueInt32(UTF8STRC("UserId"));
			userType = sess->GetValueInt32(UTF8STRC("UserType"));
		}
		else
		{
			userId = 0;
			userType = 0;
		}

		Text::String *reqDevId = req->GetQueryValue(CSTR("devid"));
		Text::String *reqOutput = req->GetQueryValue(CSTR("output"));
		if (reqDevId && reqOutput)
		{
			Int64 idevId = reqDevId->ToInt64();
			if (me->core->UserHasDevice(userId, userType, idevId))
			{
				Text::StringBuilderUTF8 sb;
				sb.Append(reqOutput);
				UTF8Char *sarr[3];
				UOSInt i;
				i = Text::StrSplit(sarr, 3, sb.v, ',');
				if (i == 2)
				{
					me->core->DeviceSetOutput(idevId, Text::StrToUInt32(sarr[0]), Text::StrToInt32(sarr[1]) != 0);
				}
			}
		}

		Data::ArrayList<ISMonitorCore::DeviceInfo *> devList;
		ISMonitorCore::DeviceInfo *dev;
		Data::DateTime dt;
		me->core->UserGetDevices(userId, userType, &devList);
		UOSInt i;
		UOSInt j;
		UOSInt k;
		UOSInt l;
		writer->WriteLineC(UTF8STRC("<h2>Home</h2>"));
		writer->WriteLineC(UTF8STRC("<table width=\"100%\" border=\"1\"><tr><td>Device Name</td><td>Last Reading Time</td><td>Readings Today</td><td>Digitals</td><td>Output</td></tr>"));
		i = 0;
		j = devList.GetCount();
		while (i < j)
		{
			dev = devList.GetItem(i);
			Sync::RWMutexUsage mutUsage(dev->mut, false);
			writer->WriteStrC(UTF8STRC("<tr><td>"));
			if (dev->devName)
			{
				WriteHTMLText(writer, Text::String::OrEmpty(dev->devName));
			}
			else
			{
				WriteHTMLText(writer, dev->platformName);
			}
			writer->WriteStrC(UTF8STRC("</td><td>"));
			if (dev->readingTime == 0)
			{
				writer->WriteStrC(UTF8STRC("-"));
			}
			else
			{
				dt.SetTicks(dev->readingTime);
				dt.ToLocalTime();
				sptr = dt.ToString(sbuff, "yyyy-MM-dd HH:mm:ss.fff zzzz");
				writer->WriteStrC(sbuff, (UOSInt)(sptr - sbuff));
			}
			writer->WriteStrC(UTF8STRC("</td><td>"));
			k = 0;
			l = dev->nReading;
			while (k < l)
			{
				writer->WriteStrC(UTF8STRC("<img src=\"devreadingimg?id="));
				sptr = Text::StrInt64(sbuff, dev->cliId);
				writer->WriteStrC(sbuff, (UOSInt)(sptr - sbuff));
				writer->WriteStrC(UTF8STRC("&sensor="));
				sptr = Text::StrInt32(sbuff, ReadInt16(dev->readings[k].status));
				writer->WriteStrC(sbuff, (UOSInt)(sptr - sbuff));
				writer->WriteStrC(UTF8STRC("&reading="));
				sptr = Text::StrInt32(sbuff, ReadInt16(&dev->readings[k].status[4]));
				writer->WriteStrC(sbuff, (UOSInt)(sptr - sbuff));
				writer->WriteStrC(UTF8STRC("&readingType="));
				sptr = Text::StrInt32(sbuff, ReadInt16(&dev->readings[k].status[6]));
				writer->WriteStrC(sbuff, (UOSInt)(sptr - sbuff));
				writer->WriteStrC(UTF8STRC("\"/>"));

				if (ReadInt16(&dev->readings[k].status[6]) == SSWR::SMonitor::SAnalogSensor::RT_RHUMIDITY)
				{
					writer->WriteStrC(UTF8STRC("<br/>"));
					writer->WriteStrC(UTF8STRC("<img src=\"devreadingimg?id="));
					sptr = Text::StrInt64(sbuff, dev->cliId);
					writer->WriteStrC(sbuff, (UOSInt)(sptr - sbuff));
					writer->WriteStrC(UTF8STRC("&sensor="));
					sptr = Text::StrInt32(sbuff, ReadInt16(dev->readings[k].status));
					writer->WriteStrC(sbuff, (UOSInt)(sptr - sbuff));
					writer->WriteStrC(UTF8STRC("&reading="));
					sptr = Text::StrInt32(sbuff, ReadInt16(&dev->readings[k].status[4]));
					writer->WriteStrC(sbuff, (UOSInt)(sptr - sbuff));
					writer->WriteStrC(UTF8STRC("&readingType="));
					sptr = Text::StrInt32(sbuff, SSWR::SMonitor::SAnalogSensor::RT_AHUMIDITY);
					writer->WriteStrC(sbuff, (UOSInt)(sptr - sbuff));
					writer->WriteStrC(UTF8STRC("\"/>"));
				}
				k++;
				if (k != l)
				{
					writer->WriteStrC(UTF8STRC("<br/>"));
				}
			}
			writer->WriteStrC(UTF8STRC("</td><td>"));
			k = 0;
			l = dev->ndigital;
			while (k < l)
			{
				if (dev->digitalNames[k])
				{
					WriteHTMLText(writer, dev->digitalNames[k]);
				}
				else
				{
					writer->WriteStrC(UTF8STRC("Digital "));
					sptr = Text::StrUOSInt(sbuff, k);
					writer->WriteStrC(sbuff, (UOSInt)(sptr - sbuff));
				}
				writer->WriteStrC(UTF8STRC(": "));
				writer->WriteStrC((dev->digitalVals & (UInt32)(1 << k))?(const UTF8Char*)"1":(const UTF8Char*)"0", 1);
				k++;
				if (k != l)
				{
					writer->WriteStrC(UTF8STRC("<br/>"));
				}
			}

			writer->WriteStrC(UTF8STRC("</td><td>"));
			k = 0;
			l = dev->nOutput;
			while (k < l)
			{
				writer->WriteStrC(UTF8STRC("Output "));
				sptr = Text::StrUOSInt(sbuff, k);
				writer->WriteStrC(sbuff, (UOSInt)(sptr - sbuff));
				writer->WriteStrC(UTF8STRC(": "));
				writer->WriteStrC(UTF8STRC("<a href=\"index?devid="));
				sptr = Text::StrInt64(sbuff, dev->cliId);
				writer->WriteStrC(sbuff, (UOSInt)(sptr - sbuff));
				writer->WriteStrC(UTF8STRC("&output="));
				sptr = Text::StrUOSInt(sbuff, k);
				writer->WriteStrC(sbuff, (UOSInt)(sptr - sbuff));
				writer->WriteStrC(UTF8STRC(",1\">On</a> <a href=\"index?devid="));
				sptr = Text::StrInt64(sbuff, dev->cliId);
				writer->WriteStrC(sbuff, (UOSInt)(sptr - sbuff));
				writer->WriteStrC(UTF8STRC("&output="));
				sptr = Text::StrUOSInt(sbuff, k);
				writer->WriteStrC(sbuff, (UOSInt)(sptr - sbuff));
				writer->WriteStrC(UTF8STRC(",0\">Off</a>"));
				k++;
				if (k != l)
				{
					writer->WriteStrC(UTF8STRC("<br/>"));
				}
			}
			mutUsage.EndUse();
			writer->WriteLineC(UTF8STRC("</td></tr>"));
			
			i++;
		}
		writer->WriteLineC(UTF8STRC("</table>"));

		writer->WriteLineC(UTF8STRC("</td></tr></table>"));
		writer->WriteLineC(UTF8STRC("</body>"));
		writer->WriteLineC(UTF8STRC("</html>"));
		if (sess)
		{
			sess->EndUse();
		}
	}

	DEL_CLASS(writer);
	buff = mstm.GetBuff(&buffSize);
	resp->AddDefHeaders(req);
	resp->AddContentType(CSTR("text/html"));
	resp->AddContentLength(buffSize);
	resp->AddHeader(CSTR("Cache-Control"), CSTR("no-cache"));
	resp->Write(buff, buffSize);
	return true;
}

Bool __stdcall SSWR::SMonitor::SMonitorWebHandler::LoginReq(SSWR::SMonitor::SMonitorWebHandler *me, NotNullPtr<Net::WebServer::IWebRequest> req, NotNullPtr<Net::WebServer::IWebResponse> resp)
{
	Text::UTF8Writer *writer;
	UInt8 *buff;
	UOSInt buffSize;
	Net::WebServer::IWebSession *sess = me->sessMgr->GetSession(req, resp);
	const UTF8Char *msg = 0;
	if (sess)
	{
		sess->EndUse();
		return resp->RedirectURL(req, CSTR("index"), 0);
	}
	if (req->GetReqMethod() == Net::WebUtil::RequestMethod::HTTP_POST)
	{
		req->ParseHTTPForm();
		Text::String *s = req->GetHTTPFormStr(CSTR("action"));
		Text::String *s2 = req->GetHTTPFormStr(CSTR("user"));
		Text::String *s3 = req->GetHTTPFormStr(CSTR("pwd"));
		if (s && s2 && s3 && s->Equals(UTF8STRC("login")))
		{
			if (s2->v[0])
			{
				SSWR::SMonitor::ISMonitorCore::LoginInfo *login = me->core->UserLogin(s2->v, s3->v);
				if (login != 0)
				{
					sess = me->sessMgr->CreateSession(req, resp);
					sess->SetValueInt32(UTF8STRC("UserId"), login->userId);
					sess->SetValueInt32(UTF8STRC("LoginId"), login->loginId);
					sess->SetValueInt32(UTF8STRC("UserType"), login->userType);
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
	writer->WriteLineC(UTF8STRC("<body onload=\"document.getElementById('user').focus()\">"));
	writer->WriteLineC(UTF8STRC("<table width=\"100%\"><tr><td width=\"100\" class=\"menu\">"));
	me->WriteMenu(writer, sess);
	writer->WriteLineC(UTF8STRC("</td><td><center>"));
	writer->WriteLineC(UTF8STRC("<form name=\"loginForm\" method=\"POST\" action=\"login\">"));
	writer->WriteLineC(UTF8STRC("<input type=\"hidden\" name=\"action\" value=\"login\"/>"));
	writer->WriteLineC(UTF8STRC("<table>"));
	writer->WriteLineC(UTF8STRC("<tr><td>User name</td><td><input type=\"text\" name=\"user\" id=\"user\" /></td></tr>"));
	writer->WriteLineC(UTF8STRC("<tr><td>Password</td><td><input type=\"password\" name=\"pwd\" id=\"pwd\"/></td></tr>"));
	writer->WriteLineC(UTF8STRC("<tr><td></td><td><input type=\"submit\"/></td></tr>"));
	if (msg)
	{
		writer->WriteLineC(UTF8STRC("<tr><td></td><td>"));
		WriteHTMLText(writer, msg);
		writer->WriteLineC(UTF8STRC("</td></tr>"));
	}
	writer->WriteLineC(UTF8STRC("</table>"));
	writer->WriteLineC(UTF8STRC("</form>"));
	writer->WriteLineC(UTF8STRC("</center></td></tr></table></body>"));
	writer->WriteLineC(UTF8STRC("</html>"));
	DEL_CLASS(writer);
	buff = mstm.GetBuff(&buffSize);
	resp->AddDefHeaders(req);
	resp->AddContentType(CSTR("text/html"));
	resp->AddContentLength(buffSize);
	resp->AddHeader(CSTR("Cache-Control"), CSTR("no-cache"));
	resp->Write(buff, buffSize);
	return true;
}

Bool __stdcall SSWR::SMonitor::SMonitorWebHandler::LogoutReq(SSWR::SMonitor::SMonitorWebHandler *me, NotNullPtr<Net::WebServer::IWebRequest> req, NotNullPtr<Net::WebServer::IWebResponse> resp)
{
	Net::WebServer::IWebSession *sess = me->sessMgr->GetSession(req, resp);
	if (sess)
	{
		sess->EndUse();
		me->sessMgr->DeleteSession(req, resp);
	}
	return resp->RedirectURL(req, CSTR("/monitor/index"), 0);
}

Bool __stdcall SSWR::SMonitor::SMonitorWebHandler::DeviceReq(SSWR::SMonitor::SMonitorWebHandler *me, NotNullPtr<Net::WebServer::IWebRequest> req, NotNullPtr<Net::WebServer::IWebResponse> resp)
{
	Text::UTF8Writer *writer;
	UInt8 *buff;
	UOSInt buffSize;
	UTF8Char sbuff[64];
	UTF8Char *sptr;
	Int64 devId;
	Net::WebServer::IWebSession *sess = me->sessMgr->GetSession(req, resp);
	if (sess == 0)
	{
		return resp->RedirectURL(req, CSTR("/monitor/index"), 0);
	}
	if (req->GetQueryValueI64(CSTR("photo"), devId))
	{
		if (me->core->UserHasDevice(sess->GetValueInt32(UTF8STRC("UserId")), sess->GetValueInt32(UTF8STRC("UserType")), devId))
		{
			me->core->SendCapturePhoto(devId);
		}
	}

	IO::MemoryStream mstm;
	NEW_CLASS(writer, Text::UTF8Writer(mstm));
	WriteHeaderBegin(writer);
	WriteHeaderEnd(writer);
	writer->WriteLineC(UTF8STRC("<body onload=\"window.setTimeout(new Function('document.location.reload()'), 60000)\">"));
	writer->WriteLineC(UTF8STRC("<table width=\"100%\"><tr><td width=\"100\" class=\"menu\">"));
	me->WriteMenu(writer, sess);
	writer->WriteLineC(UTF8STRC("</td><td>"));

	Data::ArrayList<ISMonitorCore::DeviceInfo *> devList;
	ISMonitorCore::DeviceInfo *dev;
	Data::DateTime dt;
	me->core->UserGetDevices(sess->GetValueInt32(UTF8STRC("UserId")), sess->GetValueInt32(UTF8STRC("UserType")), &devList);
	UOSInt i;
	UOSInt j;
	UOSInt k;
	UOSInt l;
	writer->WriteLineC(UTF8STRC("<h2>Device</h2>"));
	writer->WriteLineC(UTF8STRC("<table width=\"100%\" border=\"1\"><tr><td>Device Name</td><td>Platform Name</td><td>CPU Name</td><td>Version</td><td>Reading Time</td><td>Readings</td><td>Digitals</td><td>Action</td></tr>"));
	i = 0;
	j = devList.GetCount();
	while (i < j)
	{
		dev = devList.GetItem(i);
		writer->WriteStrC(UTF8STRC("<tr><td>"));
		Sync::RWMutexUsage mutUsage(dev->mut, false);
		if (dev->devName)
		{
			WriteHTMLText(writer, Text::String::OrEmpty(dev->devName));
		}
		else
		{
			WriteHTMLText(writer, dev->platformName);
		}
		writer->WriteStrC(UTF8STRC("</a></td><td>"));
		WriteHTMLText(writer, dev->platformName);
		writer->WriteStrC(UTF8STRC("</td><td>"));
		WriteHTMLText(writer, dev->cpuName);
		writer->WriteStrC(UTF8STRC("</td><td>"));
		if (dev->version == 0)
		{
			writer->WriteStrC(UTF8STRC("-"));
		}
		else
		{
			dt.SetTicks(dev->version);
			dt.ToLocalTime();
			sptr = dt.ToString(sbuff, "yyyy-MM-dd HH:mm:ss.fff");
			writer->WriteStrC(sbuff, (UOSInt)(sptr - sbuff));
		}
		writer->WriteStrC(UTF8STRC("</td><td>"));
		if (dev->readingTime == 0)
		{
			writer->WriteStrC(UTF8STRC("-"));
		}
		else
		{
			dt.SetTicks(dev->readingTime);
			dt.ToLocalTime();
			sptr = dt.ToString(sbuff, "yyyy-MM-dd HH:mm:ss.fff zzzz");
			writer->WriteStrC(sbuff, (UOSInt)(sptr - sbuff));
		}
		writer->WriteStrC(UTF8STRC("</td><td>"));
		k = 0;
		l = dev->nReading;
		while (k < l)
		{
			if (dev->readingNames[k])
			{
				WriteHTMLText(writer, dev->readingNames[k]);
			}
			else
			{
				writer->WriteStrC(UTF8STRC("Sensor "));
				sptr = Text::StrInt32(sbuff, ReadInt16(dev->readings[k].status));
				writer->WriteStrC(sbuff, (UOSInt)(sptr - sbuff));
				writer->WriteStrC(UTF8STRC(" "));
				writer->WriteStr(SSWR::SMonitor::SAnalogSensor::GetReadingTypeName((SSWR::SMonitor::SAnalogSensor::ReadingType)ReadUInt16(&dev->readings[k].status[6])));
			}

			writer->WriteStrC(UTF8STRC(" = "));
			sptr = Text::StrDouble(sbuff, dev->readings[k].reading);
			writer->WriteStrC(sbuff, (UOSInt)(sptr - sbuff));
			k++;
			if (k != l)
			{
				writer->WriteStrC(UTF8STRC("<br/>"));
			}
		}
		writer->WriteStrC(UTF8STRC("</td><td>"));
		k = 0;
		l = dev->ndigital;
		while (k < l)
		{
			if (dev->digitalNames[k])
			{
				WriteHTMLText(writer, dev->digitalNames[k]);
			}
			else
			{
				writer->WriteStrC(UTF8STRC("Digital "));
				sptr = Text::StrUOSInt(sbuff, k);
				writer->WriteStrC(sbuff, (UOSInt)(sptr - sbuff));
			}
			writer->WriteStrC(UTF8STRC(": "));
			writer->WriteStrC((dev->digitalVals & (UInt32)(1 << k))?(const UTF8Char*)"1":(const UTF8Char*)"0", 1);
			k++;
			if (k != l)
			{
				writer->WriteStrC(UTF8STRC("<br/>"));
			}
		}
		writer->WriteStrC(UTF8STRC("</td><td><a href=\"devedit?id="));
		sptr = Text::StrInt64(sbuff, dev->cliId);
		writer->WriteStrC(sbuff, (UOSInt)(sptr - sbuff));
		writer->WriteStrC(UTF8STRC("\">Edit</a><br/>"));
		writer->WriteStrC(UTF8STRC("<a href=\"devreading?id="));
		writer->WriteStrC(sbuff, (UOSInt)(sptr - sbuff));
		writer->WriteStrC(UTF8STRC("\">Reading Names</a><br/>"));
		writer->WriteStrC(UTF8STRC("<a href=\"devdigitals?id="));
		writer->WriteStrC(sbuff, (UOSInt)(sptr - sbuff));
		writer->WriteStrC(UTF8STRC("\">Digital Names</a><br/>"));
		writer->WriteStrC(UTF8STRC("<a href=\"device?photo="));
		writer->WriteStrC(sbuff, (UOSInt)(sptr - sbuff));
		writer->WriteStrC(UTF8STRC("\">Capture Photo</a>"));
		mutUsage.EndUse();
		writer->WriteLineC(UTF8STRC("</td></tr>"));
		
		i++;
	}
	writer->WriteLineC(UTF8STRC("</table>"));

	writer->WriteLineC(UTF8STRC("</td></tr></table></body>"));
	writer->WriteLineC(UTF8STRC("</html>"));
	sess->EndUse();

	DEL_CLASS(writer);
	buff = mstm.GetBuff(&buffSize);
	resp->AddDefHeaders(req);
	resp->AddContentType(CSTR("text/html"));
	resp->AddContentLength(buffSize);
	resp->AddHeader(CSTR("Cache-Control"), CSTR("no-cache"));
	resp->Write(buff, buffSize);
	return true;
}

Bool __stdcall SSWR::SMonitor::SMonitorWebHandler::DeviceEditReq(SSWR::SMonitor::SMonitorWebHandler *me, NotNullPtr<Net::WebServer::IWebRequest> req, NotNullPtr<Net::WebServer::IWebResponse> resp)
{
	Text::UTF8Writer *writer;
	UInt8 *buff;
	UOSInt buffSize;
	UTF8Char sbuff[64];
	UTF8Char *sptr;
	Net::WebServer::IWebSession *sess = me->sessMgr->GetSession(req, resp);
	if (sess == 0)
	{
		return resp->RedirectURL(req, CSTR("/monitor/index"), 0);
	}
	Text::String *cid = req->GetQueryValue(CSTR("id"));
	Int64 cliId = 0;
	if (cid)
	{
		cliId = cid->ToInt64();
	}
	if (cliId == 0)
	{
		sess->EndUse();
		return resp->RedirectURL(req, CSTR("/monitor/device"), 0);
	}
	if (!me->core->UserHasDevice(sess->GetValueInt32(UTF8STRC("UserId")), sess->GetValueInt32(UTF8STRC("UserType")), cliId))
	{
		sess->EndUse();
		return resp->RedirectURL(req, CSTR("/monitor/device"), 0);
	}

	if (req->GetReqMethod() == Net::WebUtil::RequestMethod::HTTP_POST)
	{
		req->ParseHTTPForm();
		Text::String *action = req->GetHTTPFormStr(CSTR("action"));
		if (action && action->Equals(UTF8STRC("modify")))
		{
			Text::String *devName = req->GetHTTPFormStr(CSTR("devName"));
			Int32 flags = 0;
			Text::String *s;
			s = req->GetHTTPFormStr(CSTR("anonymous"));
			if (s && s->v[0] == '1')
			{
				flags |= 1;
			}
			s = req->GetHTTPFormStr(CSTR("removed"));
			if (s && s->v[0] == '1')
			{
				flags |= 2;
			}
			if (devName)
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
	writer->WriteLineC(UTF8STRC("<body onload=\"document.forms[0].devName.focus()\">"));
	writer->WriteLineC(UTF8STRC("<table width=\"100%\"><tr><td width=\"100\" class=\"menu\">"));
	me->WriteMenu(writer, sess);
	writer->WriteLineC(UTF8STRC("</td><td>"));

	writer->WriteLineC(UTF8STRC("<h2>Edit Device</h2>"));
	ISMonitorCore::DeviceInfo *dev = me->core->DeviceGet(cliId);
	writer->WriteStrC(UTF8STRC("<form name=\"modifyForm\" method=\"POST\" action=\"devedit?id="));
	sptr = Text::StrInt64(sbuff, cliId);
	writer->WriteStrC(sbuff, (UOSInt)(sptr - sbuff));
	writer->WriteLineC(UTF8STRC("\">"));
	writer->WriteLineC(UTF8STRC("<table width=\"100%\" border=\"1\">"));
	writer->WriteStrC(UTF8STRC("<input type=\"hidden\" name=\"action\" value=\"modify\"/>"));
	Sync::RWMutexUsage mutUsage(dev->mut, false);
	writer->WriteStrC(UTF8STRC("<tr><td>Platform Name</td><td>"));
	WriteHTMLText(writer, dev->platformName);
	writer->WriteLineC(UTF8STRC("</td></tr>"));
	writer->WriteStrC(UTF8STRC("<tr><td>CPU Name</td><td>"));
	WriteHTMLText(writer, dev->cpuName);
	writer->WriteLineC(UTF8STRC("</td></tr>"));

	writer->WriteStrC(UTF8STRC("<tr><td>Device Name</td><td><input type=\"text\" name=\"devName\" "));
	if (dev->devName)
	{
		writer->WriteStrC(UTF8STRC(" value="));
		WriteAttrText(writer, dev->devName);
	}
	writer->WriteLineC(UTF8STRC("/></td></tr>"));
	writer->WriteStrC(UTF8STRC("<tr><td>Flags</td><td><input type=\"checkbox\" name=\"anonymous\" id=\"anonymous\" value=\"1\""));
	if (dev->flags & 1)
	{
		writer->WriteStrC(UTF8STRC(" checked"));
	}
	writer->WriteLineC(UTF8STRC("/><label for=\"anonymous\">Anonymous Access</label><br/><input type=\"checkbox\" name=\"removed\" id=\"removed\" value=\"1\""));
	if (dev->flags & 2)
	{
		writer->WriteStrC(UTF8STRC(" checked"));
	}
	writer->WriteStrC(UTF8STRC("/><label for=\"removed\">Removed</label></td></tr>"));
	writer->WriteLineC(UTF8STRC("<tr><td></td><td><input type=\"submit\"/></td></tr>"));
	writer->WriteLineC(UTF8STRC("</table></form>"));
	mutUsage.EndUse();

	writer->WriteLineC(UTF8STRC("</td></tr></table></body>"));
	writer->WriteLineC(UTF8STRC("</html>"));
	sess->EndUse();

	DEL_CLASS(writer);
	buff = mstm.GetBuff(&buffSize);
	resp->AddDefHeaders(req);
	resp->AddContentType(CSTR("text/html"));
	resp->AddContentLength(buffSize);
	resp->AddHeader(CSTR("Cache-Control"), CSTR("no-cache"));
	resp->Write(buff, buffSize);
	return true;
}

Bool __stdcall SSWR::SMonitor::SMonitorWebHandler::DeviceReadingReq(SSWR::SMonitor::SMonitorWebHandler *me, NotNullPtr<Net::WebServer::IWebRequest> req, NotNullPtr<Net::WebServer::IWebResponse> resp)
{
	Text::UTF8Writer *writer;
	UInt8 *buff;
	UOSInt buffSize;
	UTF8Char sbuff[64];
	UTF8Char *sptr;
	UOSInt i;
	UOSInt j;
	ISMonitorCore::DeviceInfo *dev;
	Net::WebServer::IWebSession *sess = me->sessMgr->GetSession(req, resp);
	if (sess == 0)
	{
		return resp->RedirectURL(req, CSTR("/monitor/index"), 0);
	}
	Text::String *cid = req->GetQueryValue(CSTR("id"));
	Int64 cliId = 0;
	if (cid)
	{
		cliId = cid->ToInt64();
	}
	if (cliId == 0)
	{
		sess->EndUse();
		return resp->RedirectURL(req, CSTR("/monitor/device"), 0);
	}
	if (!me->core->UserHasDevice(sess->GetValueInt32(UTF8STRC("UserId")), sess->GetValueInt32(UTF8STRC("UserType")), cliId))
	{
		sess->EndUse();
		return resp->RedirectURL(req, CSTR("/monitor/device"), 0);
	}

	dev = me->core->DeviceGet(cliId);
	if (req->GetReqMethod() == Net::WebUtil::RequestMethod::HTTP_POST)
	{
		req->ParseHTTPForm();
		Text::String *action = req->GetHTTPFormStr(CSTR("action"));
		if (action && action->Equals(UTF8STRC("reading")))
		{
			Text::StringBuilderUTF8 sb;
			Text::String *s;
			i = 0;
			j = dev->nReading;
			while (i < j)
			{
				if (i > 0)
				{
					sb.AppendC(UTF8STRC("|"));
				}
				sptr = Text::StrUOSInt(Text::StrConcatC(sbuff, UTF8STRC("readingName")), i);
				s = req->GetHTTPFormStr(CSTRP(sbuff, sptr));
				if (s)
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
	writer->WriteLineC(UTF8STRC("<body onload=\"document.forms[0].readingName0.focus()\">"));
	writer->WriteLineC(UTF8STRC("<table width=\"100%\"><tr><td width=\"100\" class=\"menu\">"));
	me->WriteMenu(writer, sess);
	writer->WriteLineC(UTF8STRC("</td><td>"));

	writer->WriteLineC(UTF8STRC("<h2>Reading Names</h2>"));
	writer->WriteStrC(UTF8STRC("<form name=\"modifyForm\" method=\"POST\" action=\"devreading?id="));
	sptr = Text::StrInt64(sbuff, cliId);
	writer->WriteStrC(sbuff, (UOSInt)(sptr - sbuff));
	writer->WriteLineC(UTF8STRC("\">"));
	writer->WriteStrC(UTF8STRC("<input type=\"hidden\" name=\"action\" value=\"reading\"/>"));
	writer->WriteLineC(UTF8STRC("<table width=\"100%\" border=\"1\">"));
	Sync::RWMutexUsage mutUsage(dev->mut, false);
	j = dev->nReading;
	i = 0;
	while (i < j)
	{
		writer->WriteStrC(UTF8STRC("<tr><td>Reading "));
		sptr = Text::StrUOSInt(sbuff, i);
		writer->WriteStrC(sbuff, (UOSInt)(sptr - sbuff));
		writer->WriteStrC(UTF8STRC("</td><td><input type=\"text\" name=\"readingName"));
		writer->WriteStrC(sbuff, (UOSInt)(sptr - sbuff));
		writer->WriteStrC(UTF8STRC("\" "));

		if (dev->readingNames[i])
		{
			writer->WriteStrC(UTF8STRC(" value="));
			WriteAttrText(writer, dev->readingNames[i]);
		}
		writer->WriteStrC(UTF8STRC("/></td><td>Sensor "));
		sptr = Text::StrInt32(sbuff, ReadInt16(&dev->readings[i].status[0]));
		writer->WriteStrC(sbuff, (UOSInt)(sptr - sbuff));
		if (ReadInt16(&dev->readings[i].status[2]) != SSWR::SMonitor::SAnalogSensor::ST_UNKNOWN)
		{
			writer->WriteStrC(UTF8STRC(" ("));
			writer->WriteStr(SSWR::SMonitor::SAnalogSensor::GetSensorTypeName((SSWR::SMonitor::SAnalogSensor::SensorType)ReadInt16(&dev->readings[i].status[2])));
			writer->WriteStrC(UTF8STRC(")"));
		}
		writer->WriteStrC(UTF8STRC(" Reading "));
		sptr = Text::StrInt32(sbuff, ReadInt16(&dev->readings[i].status[4]));
		writer->WriteStrC(sbuff, (UOSInt)(sptr - sbuff));
		if (ReadInt16(&dev->readings[i].status[6]) != SSWR::SMonitor::SAnalogSensor::RT_UNKNOWN)
		{
			writer->WriteStrC(UTF8STRC(" "));
			writer->WriteStr(SSWR::SMonitor::SAnalogSensor::GetReadingTypeName((SSWR::SMonitor::SAnalogSensor::ReadingType)ReadInt16(&dev->readings[i].status[6])));
		}
		writer->WriteLineC(UTF8STRC("</td></tr>"));
		i++;
	}
	writer->WriteLineC(UTF8STRC("<tr><td></td><td><input type=\"submit\"/></td></tr>"));
	writer->WriteLineC(UTF8STRC("</table></form>"));
	mutUsage.EndUse();

	writer->WriteLineC(UTF8STRC("</td></tr></table></body>"));
	writer->WriteLineC(UTF8STRC("</html>"));
	sess->EndUse();

	DEL_CLASS(writer);
	buff = mstm.GetBuff(&buffSize);
	resp->AddDefHeaders(req);
	resp->AddContentType(CSTR("text/html"));
	resp->AddContentLength(buffSize);
	resp->AddHeader(CSTR("Cache-Control"), CSTR("no-cache"));
	resp->Write(buff, buffSize);
	return true;
}

Bool __stdcall SSWR::SMonitor::SMonitorWebHandler::DeviceDigitalsReq(SSWR::SMonitor::SMonitorWebHandler *me, NotNullPtr<Net::WebServer::IWebRequest> req, NotNullPtr<Net::WebServer::IWebResponse> resp)
{
	Text::UTF8Writer *writer;
	UInt8 *buff;
	UOSInt buffSize;
	UTF8Char sbuff[64];
	UTF8Char *sptr;
	UOSInt i;
	UOSInt j;
	ISMonitorCore::DeviceInfo *dev;
	Net::WebServer::IWebSession *sess = me->sessMgr->GetSession(req, resp);
	if (sess == 0)
	{
		return resp->RedirectURL(req, CSTR("/monitor/index"), 0);
	}
	Text::String *cid = req->GetQueryValue(CSTR("id"));
	Int64 cliId = 0;
	if (cid)
	{
		cliId = cid->ToInt64();
	}
	if (cliId == 0)
	{
		sess->EndUse();
		return resp->RedirectURL(req, CSTR("/monitor/device"), 0);
	}
	if (!me->core->UserHasDevice(sess->GetValueInt32(UTF8STRC("UserId")), sess->GetValueInt32(UTF8STRC("UserType")), cliId))
	{
		sess->EndUse();
		return resp->RedirectURL(req, CSTR("/monitor/device"), 0);
	}

	dev = me->core->DeviceGet(cliId);
	if (req->GetReqMethod() == Net::WebUtil::RequestMethod::HTTP_POST)
	{
		req->ParseHTTPForm();
		Text::String *action = req->GetHTTPFormStr(CSTR("action"));
		if (action && action->Equals(UTF8STRC("digitals")))
		{
			Text::StringBuilderUTF8 sb;
			Text::String *s;
			i = 0;
			j = dev->ndigital;
			while (i < j)
			{
				if (i > 0)
				{
					sb.AppendC(UTF8STRC("|"));
				}
				sptr = Text::StrUOSInt(Text::StrConcatC(sbuff, UTF8STRC("digitalName")), i);
				s = req->GetHTTPFormStr(CSTRP(sbuff, sptr));
				if (s)
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
	writer->WriteLineC(UTF8STRC("<body onload=\"document.forms[0].digitalName0.focus()\">"));
	writer->WriteLineC(UTF8STRC("<table width=\"100%\"><tr><td width=\"100\" class=\"menu\">"));
	me->WriteMenu(writer, sess);
	writer->WriteLineC(UTF8STRC("</td><td>"));

	writer->WriteLineC(UTF8STRC("<h2>Digital Names</h2>"));
	writer->WriteStrC(UTF8STRC("<form name=\"modifyForm\" method=\"POST\" action=\"devdigitals?id="));
	sptr = Text::StrInt64(sbuff, cliId);
	writer->WriteStrC(sbuff, (UOSInt)(sptr - sbuff));
	writer->WriteLineC(UTF8STRC("\">"));
	writer->WriteStrC(UTF8STRC("<input type=\"hidden\" name=\"action\" value=\"digitals\"/>"));
	writer->WriteLineC(UTF8STRC("<table width=\"100%\" border=\"1\">"));
	Sync::RWMutexUsage mutUsage(dev->mut, false);
	j = dev->ndigital;
	i = 0;
	while (i < j)
	{
		writer->WriteStrC(UTF8STRC("<tr><td>Digital "));
		sptr = Text::StrUOSInt(sbuff, i);
		writer->WriteStrC(sbuff, (UOSInt)(sptr - sbuff));
		writer->WriteStrC(UTF8STRC("</td><td><input type=\"text\" name=\"digitalName"));
		writer->WriteStrC(sbuff, (UOSInt)(sptr - sbuff));
		writer->WriteStrC(UTF8STRC("\" "));

		if (dev->digitalNames[i])
		{
			writer->WriteStrC(UTF8STRC(" value="));
			WriteAttrText(writer, dev->digitalNames[i]);
		}
		writer->WriteStrC(UTF8STRC("/></td></tr>"));
		i++;
	}
	writer->WriteLineC(UTF8STRC("<tr><td></td><td><input type=\"submit\"/></td></tr>"));
	writer->WriteLineC(UTF8STRC("</table></form>"));
	mutUsage.EndUse();

	writer->WriteLineC(UTF8STRC("</td></tr></table></body>"));
	writer->WriteLineC(UTF8STRC("</html>"));
	sess->EndUse();

	DEL_CLASS(writer);
	buff = mstm.GetBuff(&buffSize);
	resp->AddDefHeaders(req);
	resp->AddContentType(CSTR("text/html"));
	resp->AddContentLength(buffSize);
	resp->AddHeader(CSTR("Cache-Control"), CSTR("no-cache"));
	resp->Write(buff, buffSize);
	return true;
}

Bool __stdcall SSWR::SMonitor::SMonitorWebHandler::DeviceReadingImgReq(SSWR::SMonitor::SMonitorWebHandler *me, NotNullPtr<Net::WebServer::IWebRequest> req, NotNullPtr<Net::WebServer::IWebResponse> resp)
{
	Text::String *s;
	Int64 cliId = 0;
	Int32 userId = 0;
	Int32 userType = 0;
	Int32 sensorId = 0;
	Int32 readingId = 0;
	Int32 readingType = 0;
	Bool valid = true;
	s = req->GetQueryValue(CSTR("id"));
	if (s == 0)
	{
		valid = false;
	}
	else if (!s->ToInt64(cliId))
	{
		valid = false;
	}

	s = req->GetQueryValue(CSTR("sensor"));
	if (s == 0)
	{
		valid = false;
	}
	else if (!s->ToInt32(sensorId))
	{
		valid = false;
	}

	s = req->GetQueryValue(CSTR("reading"));
	if (s == 0)
	{
		valid = false;
	}
	else if (!s->ToInt32(readingId))
	{
		valid = false;
	}

	s = req->GetQueryValue(CSTR("readingType"));
	if (s == 0)
	{
		valid = false;
	}
	else if (!s->ToInt32(readingType))
	{
		valid = false;
	}

	if (!valid)
	{
		resp->ResponseError(req, Net::WebStatus::SC_NOT_FOUND);
		return true;
	}

	Net::WebServer::IWebSession *sess = me->sessMgr->GetSession(req, resp);
	if (sess)
	{
		userId = sess->GetValueInt32(UTF8STRC("UserId"));
		userType = sess->GetValueInt32(UTF8STRC("UserType"));
		sess->EndUse();
	}

	if (!me->core->UserHasDevice(userId, userType, cliId))
	{
		resp->ResponseError(req, Net::WebStatus::SC_NOT_FOUND);
		return true;
	}

	SSWR::SMonitor::ISMonitorCore::DeviceInfo *dev = me->core->DeviceGet(cliId);
	UOSInt i;
	UOSInt j;
	UOSInt k;
	IO::MemoryStream *mstm;
	UInt8 *buff;
	UOSInt buffSize;

	if (dev->valUpdated)
	{
		Sync::RWMutexUsage mutUsage(dev->mut, true);
		dev->valUpdated = false;
		i = dev->imgCaches.GetCount();
		while (i-- > 0)
		{
			mstm = dev->imgCaches.GetItem(i);
			DEL_CLASS(mstm);
		}
		dev->imgCaches.Clear();
	}
	
	Sync::RWMutexUsage mutUsage(dev->mut, false);
	mstm = dev->imgCaches.Get((sensorId << 16) + (readingId << 8) + (readingType));
	if (mstm)
	{
		buff = mstm->GetBuff(&buffSize);
		resp->AddDefHeaders(req);
		resp->AddContentType(CSTR("image/png"));
		resp->AddContentLength(buffSize);
		resp->AddHeader(CSTR("Cache-Control"), CSTR("no-cache"));
		resp->Write(buff, buffSize);
		return true;
	}

	NotNullPtr<Media::DrawEngine> deng = me->core->GetDrawEngine();
	Media::DrawImage *dimg = deng->CreateImage32(Math::Size2D<UOSInt>(640, 120), Media::AT_NO_ALPHA);
	Media::DrawFont *f;
	Media::DrawBrush *b;
	UOSInt readingIndex = (UOSInt)-1;
	Int32 readingTypeD;
	Data::LineChart *chart;
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
		SSWR::SMonitor::ISMonitorCore::DevRecord2 *rec;
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
				rec = dev->todayRecs.GetItem(i);
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
				rec = dev->todayRecs.GetItem(i);
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
		if (dev->readingNames[readingIndex])
		{
			sb.AppendSlow(dev->readingNames[readingIndex]);
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
					rec = dev->yesterdayRecs.GetItem(i);
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
					rec = dev->yesterdayRecs.GetItem(i);
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

			NEW_CLASS(chart, Data::LineChart(sb.ToCString()));
			if (dateList2.GetCount() >= 2)
			{
				chart->AddXDataDate(dateList2.GetArray(k), dateList2.GetCount());
				chart->AddYData(CSTR("Yesterday"), valList2.GetArray(k), valList2.GetCount(), 0xffcccccc, Data::LineChart::LS_LINE);
			}
			chart->AddXDataDate(dateList.GetArray(k), dateList.GetCount());
			chart->AddYData(CSTR("Reading"), valList.GetArray(k), valList.GetCount(), 0xffff0000, Data::LineChart::LS_LINE);

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
	Media::ImageList *imgList;

	NEW_CLASS(imgList, Media::ImageList(CSTR("temp.png")));
	imgList->AddImage(dimg->ToStaticImage(), 0);
	deng->DeleteImage(dimg);

	NotNullPtr<IO::MemoryStream> nnmstm;
	NEW_CLASSNN(nnmstm, IO::MemoryStream());
	NEW_CLASS(exporter, Exporter::GUIPNGExporter());
	exporter->ExportFile(nnmstm, CSTR("temp.png"), imgList, 0);
	DEL_CLASS(exporter);
	DEL_CLASS(imgList);

	buff = nnmstm->GetBuff(&buffSize);
	resp->AddDefHeaders(req);
	resp->AddContentType(CSTR("image/png"));
	resp->AddContentLength(buffSize);
	resp->AddHeader(CSTR("Cache-Control"), CSTR("no-cache"));
	resp->Write(buff, buffSize);

	mutUsage.ReplaceMutex(dev->mut, true);
	mstm = dev->imgCaches.Put((sensorId << 16) + (readingId << 8) + (readingType), nnmstm.Ptr());
	mutUsage.EndUse();
	if (mstm)
	{
		DEL_CLASS(mstm);
	}
	return true;
}

Bool __stdcall SSWR::SMonitor::SMonitorWebHandler::DevicePastDataReq(SSWR::SMonitor::SMonitorWebHandler *me, NotNullPtr<Net::WebServer::IWebRequest> req, NotNullPtr<Net::WebServer::IWebResponse> resp)
{
	Text::UTF8Writer *writer;
	Int32 userId = 0;
	Int32 userType = 0;
	UInt8 *buff;
	UOSInt buffSize;
	UTF8Char sbuff[64];
	UTF8Char *sptr;
	Net::WebServer::IWebSession *sess = me->sessMgr->GetSession(req, resp);
	if (sess)
	{
		userId = sess->GetValueInt32(UTF8STRC("UserId"));
		userType = sess->GetValueInt32(UTF8STRC("UserType"));
	}

	UOSInt i;
	UOSInt j;
	UOSInt k;
	UOSInt l;
	Data::ArrayList<SSWR::SMonitor::ISMonitorCore::DeviceInfo *> devList;
	SSWR::SMonitor::ISMonitorCore::DeviceInfo *dev;
	IO::MemoryStream mstm;
	NEW_CLASS(writer, Text::UTF8Writer(mstm));
	WriteHeaderBegin(writer);
	writer->WriteLineC(UTF8STRC("<script type=\"text/javascript\">"));
	writer->WriteLineC(UTF8STRC("var clients = new Object();"));
	writer->WriteLineC(UTF8STRC("var cli;"));
	writer->WriteLineC(UTF8STRC("var reading;"));
	me->core->UserGetDevices(userId, userType, &devList);
	i = 0;
	j = devList.GetCount();
	while (i < j)
	{
		dev = devList.GetItem(i);
		writer->WriteLineC(UTF8STRC("cli = new Object();"));
		writer->WriteStrC(UTF8STRC("cli.cliId = "));
		sptr = Text::StrInt64(sbuff, dev->cliId);
		writer->WriteStrC(sbuff, (UOSInt)(sptr - sbuff));
		writer->WriteLineC(UTF8STRC(";"));
		Sync::RWMutexUsage mutUsage(dev->mut, false);
		writer->WriteStrC(UTF8STRC("cli.name = "));
		if (dev->devName)
		{
			WriteJSText(writer, dev->devName);
		}
		else
		{
			WriteJSText(writer, dev->platformName);
		}
		writer->WriteLineC(UTF8STRC(";"));
		writer->WriteLineC(UTF8STRC("cli.readings = new Array();"));
		k = 0;
		l = dev->nReading;
		while (k < l)
		{
			writer->WriteLineC(UTF8STRC("reading = new Object();"));
			writer->WriteStrC(UTF8STRC("reading.name = "));
			if (dev->readingNames[k])
			{
				WriteJSText(writer, dev->readingNames[k]);
			}
			else
			{
				Text::StrInt32(Text::StrConcatC(Text::StrInt32(Text::StrConcatC(sbuff, UTF8STRC("Sensor ")), ReadInt16(dev->readings[k].status)), UTF8STRC(" Reading ")), ReadInt16(&dev->readings[k].status[4]));
				WriteJSText(writer, sbuff);
			}
			writer->WriteLineC(UTF8STRC(";"));
			writer->WriteStrC(UTF8STRC("reading.sensor = "));
			sptr = Text::StrInt32(sbuff, ReadInt16(dev->readings[k].status));
			writer->WriteStrC(sbuff, (UOSInt)(sptr - sbuff));
			writer->WriteLineC(UTF8STRC(";"));
			writer->WriteStrC(UTF8STRC("reading.reading = "));
			sptr = Text::StrInt32(sbuff, ReadInt16(&dev->readings[k].status[4]));
			writer->WriteStrC(sbuff, (UOSInt)(sptr - sbuff));
			writer->WriteLineC(UTF8STRC(";"));

			writer->WriteLineC(UTF8STRC("cli.readings.push(reading);"));
			k++;
		}
		writer->WriteLineC(UTF8STRC("clients[cli.cliId] = cli;"));
		mutUsage.EndUse();
		i++;
	}

	writer->WriteLineC(UTF8STRC("</script>"));
	writer->WriteLineC(UTF8STRC("<script type=\"text/javascript\" src=\"files/pastdata.js\">"));
	writer->WriteLineC(UTF8STRC("</script>"));
	WriteHeaderEnd(writer);
	writer->WriteLineC(UTF8STRC("<body onload=\"afterLoad()\">"));
	writer->WriteLineC(UTF8STRC("<table width=\"100%\"><tr><td width=\"100\" class=\"menu\">"));
	me->WriteMenu(writer, sess);
	writer->WriteLineC(UTF8STRC("</td><td>"));
	writer->WriteLineC(UTF8STRC("<h2>Past Data</h2>"));
	writer->WriteLineC(UTF8STRC("Device Name<select name=\"cliId\" id=\"cliId\" onchange=\"devChg()\"></select><br/>"));
	writer->WriteLineC(UTF8STRC("Reading<select name=\"reading\" id=\"reading\" onchange=\"selChg()\"></select><br/>"));
	writer->WriteLineC(UTF8STRC("Date<select name=\"year\" id=\"year\" onchange=\"selChg()\"></select><select name=\"month\" id=\"month\" onchange=\"selChg()\"></select><select name=\"day\" id=\"day\" onchange=\"selChg()\"></select><br/>"));
	writer->WriteLineC(UTF8STRC("<img src=\"about:blank\" id=\"dataimg\"/>"));

	writer->WriteLineC(UTF8STRC("</td></tr></table></body>"));
	writer->WriteLineC(UTF8STRC("</html>"));
	if (sess)
	{
		sess->EndUse();
	}

	DEL_CLASS(writer);
	buff = mstm.GetBuff(&buffSize);
	resp->AddDefHeaders(req);
	resp->AddContentType(CSTR("text/html"));
	resp->AddContentLength(buffSize);
	resp->AddHeader(CSTR("Cache-Control"), CSTR("no-cache"));
	resp->Write(buff, buffSize);
	return true;
}

Bool __stdcall SSWR::SMonitor::SMonitorWebHandler::DevicePastDataImgReq(SSWR::SMonitor::SMonitorWebHandler *me, NotNullPtr<Net::WebServer::IWebRequest> req, NotNullPtr<Net::WebServer::IWebResponse> resp)
{
	Text::String *s;
	Int64 cliId = 0;
	Int64 startTime = 0;
	Int32 userId = 0;
	Int32 userType = 0;
	Int32 sensorId = 0;
	Int32 readingId = 0;
	Bool valid = true;
	s = req->GetQueryValue(CSTR("id"));
	if (s == 0)
	{
		valid = false;
	}
	else if (!s->ToInt64(cliId))
	{
		valid = false;
	}

	s = req->GetQueryValue(CSTR("sensor"));
	if (s == 0)
	{
		valid = false;
	}
	else if (!s->ToInt32(sensorId))
	{
		valid = false;
	}

	s = req->GetQueryValue(CSTR("reading"));
	if (s == 0)
	{
		valid = false;
	}
	else if (!s->ToInt32(readingId))
	{
		valid = false;
	}

	s = req->GetQueryValue(CSTR("starttime"));
	if (s == 0)
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

	Net::WebServer::IWebSession *sess = me->sessMgr->GetSession(req, resp);
	if (sess)
	{
		userId = sess->GetValueInt32(UTF8STRC("UserId"));
		userType = sess->GetValueInt32(UTF8STRC("UserType"));
		sess->EndUse();
	}

	if (!me->core->UserHasDevice(userId, userType, cliId))
	{
		resp->ResponseError(req, Net::WebStatus::SC_NOT_FOUND);
		return true;
	}

	SSWR::SMonitor::ISMonitorCore::DeviceInfo *dev = me->core->DeviceGet(cliId);
	Data::ArrayList<SSWR::SMonitor::ISMonitorCore::DevRecord2*> recList;
	UOSInt i;
	UOSInt j;
	UOSInt k;
	UInt8 *buff;
	UOSInt buffSize;

	NotNullPtr<Media::DrawEngine> deng = me->core->GetDrawEngine();
	Media::DrawImage *dimg = deng->CreateImage32(Math::Size2D<UOSInt>(640, 120), Media::AT_NO_ALPHA);
	Media::DrawFont *f;
	Media::DrawBrush *b;
	UOSInt readingIndex = (UOSInt)-1;
	Int32 readingType = 0;
	Data::LineChart *chart;
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
		UTF8Char *sptr;
		Data::ArrayListInt64 dateList;
		Data::ArrayListDbl valList;
		SSWR::SMonitor::ISMonitorCore::DevRecord2 *rec;
		me->core->DeviceQueryRec(cliId, startTime, startTime + 86400000, &recList);
		i = 0;
		j = recList.GetCount();
		while (i < j)
		{
			rec = recList.GetItem(i);
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
			MemFree(rec);
			i++;
		}

		Text::StringBuilderUTF8 sb;
		Sync::RWMutexUsage mutUsage(dev->mut, false);
		if (dev->readingNames[readingIndex])
		{
			sb.AppendSlow(dev->readingNames[readingIndex]);
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
			NEW_CLASS(chart, Data::LineChart(sb.ToCString()));
			chart->AddXDataDate(dateList.GetArray(k), dateList.GetCount());
			chart->AddYData(CSTR("Reading"), valList.GetArray(k), valList.GetCount(), 0xffff0000, Data::LineChart::LS_LINE);
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
	Media::ImageList *imgList;

	NEW_CLASS(imgList, Media::ImageList(CSTR("temp.png")));
	imgList->AddImage(dimg->ToStaticImage(), 0);
	deng->DeleteImage(dimg);

	IO::MemoryStream mstm;
	NEW_CLASS(exporter, Exporter::GUIPNGExporter());
	exporter->ExportFile(mstm, CSTR("temp.png"), imgList, 0);
	DEL_CLASS(exporter);
	DEL_CLASS(imgList);

	buff = mstm.GetBuff(&buffSize);
	resp->AddDefHeaders(req);
	resp->AddContentType(CSTR("image/png"));
	resp->AddContentLength(buffSize);
	resp->AddHeader(CSTR("Cache-Control"), CSTR("no-cache"));
	resp->Write(buff, buffSize);
	return true;
}

Bool __stdcall SSWR::SMonitor::SMonitorWebHandler::UserPasswordReq(SSWR::SMonitor::SMonitorWebHandler *me, NotNullPtr<Net::WebServer::IWebRequest> req, NotNullPtr<Net::WebServer::IWebResponse> resp)
{
	Text::UTF8Writer *writer;
	UInt8 *buff;
	UOSInt buffSize;
	Net::WebServer::IWebSession *sess = me->sessMgr->GetSession(req, resp);
	Text::CString msg = CSTR_NULL;
	if (sess == 0)
	{
		return resp->RedirectURL(req, CSTR("/monitor/index"), 0);
	}

	if (req->GetReqMethod() == Net::WebUtil::RequestMethod::HTTP_POST)
	{
		req->ParseHTTPForm();
		Text::String *pwd = req->GetHTTPFormStr(CSTR("password"));
		Text::String *retype = req->GetHTTPFormStr(CSTR("retype"));
		if (pwd == 0 || pwd->v[0] == 0)
		{
			msg = CSTR("Password is empty");
		}
		else if (retype == 0 || retype->v[0] == 0)
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
				if (me->core->UserSetPassword(sess->GetValueInt32(UTF8STRC("UserId")), pwd->v))
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
	writer->WriteLineC(UTF8STRC("<body onload=\"document.getElementById('password').focus()\">"));
	writer->WriteLineC(UTF8STRC("<table width=\"100%\"><tr><td width=\"100\" class=\"menu\">"));
	me->WriteMenu(writer, sess);
	writer->WriteLineC(UTF8STRC("</td><td>"));
	writer->WriteLineC(UTF8STRC("<h2>Modify Password</h2>"));
	writer->WriteLineC(UTF8STRC("<center>"));
	writer->WriteLineC(UTF8STRC("<form method=\"POST\" action=\"userpassword\">"));
	writer->WriteLineC(UTF8STRC("<table border=\"0\"><tr><td>Password</td><td><input type=\"password\" name=\"password\" id=\"password\"/></td></tr>"));
	writer->WriteLineC(UTF8STRC("<tr><td>Retype</td><td><input type=\"password\" name=\"retype\" id=\"retype\"/></td></tr>"));
	writer->WriteLineC(UTF8STRC("<tr><td></td><td><input type=\"submit\"/></td></tr>"));
	writer->WriteLineC(UTF8STRC("</table>"));
	if (msg.v)
	{
		WriteHTMLText(writer, msg);
	}
	writer->WriteLineC(UTF8STRC("</form></center>"));
	writer->WriteLineC(UTF8STRC("</td></tr></table></body>"));
	writer->WriteLineC(UTF8STRC("</html>"));
	sess->EndUse();

	DEL_CLASS(writer);
	buff = mstm.GetBuff(&buffSize);
	resp->AddDefHeaders(req);
	resp->AddContentType(CSTR("text/html"));
	resp->AddContentLength(buffSize);
	resp->AddHeader(CSTR("Cache-Control"), CSTR("no-cache"));
	resp->Write(buff, buffSize);
	return true;
}

Bool __stdcall SSWR::SMonitor::SMonitorWebHandler::UsersReq(SSWR::SMonitor::SMonitorWebHandler *me, NotNullPtr<Net::WebServer::IWebRequest> req, NotNullPtr<Net::WebServer::IWebResponse> resp)
{
	Text::UTF8Writer *writer;
	UInt8 *buff;
	UOSInt buffSize;
	UTF8Char sbuff[64];
	UTF8Char *sptr;
	Net::WebServer::IWebSession *sess = me->sessMgr->GetSession(req, resp);
	if (sess == 0)
	{
		return resp->RedirectURL(req, CSTR("/monitor/index"), 0);
	}
	if (sess->GetValueInt32(UTF8STRC("UserType")) != 1)
	{
		sess->EndUse();
		return resp->RedirectURL(req, CSTR("/monitor/index"), 0);
	}

	Data::ArrayList<SSWR::SMonitor::ISMonitorCore::WebUser*> userList;
	SSWR::SMonitor::ISMonitorCore::WebUser *user;
	UOSInt i;
	UOSInt j;
	me->core->UserGetList(&userList);
	IO::MemoryStream mstm;
	NEW_CLASS(writer, Text::UTF8Writer(mstm));
	WriteHeaderBegin(writer);
	WriteHeaderEnd(writer);
	writer->WriteLineC(UTF8STRC("<body>"));
	writer->WriteLineC(UTF8STRC("<table width=\"100%\"><tr><td width=\"100\" class=\"menu\">"));
	me->WriteMenu(writer, sess);
	writer->WriteLineC(UTF8STRC("</td><td>"));
	writer->WriteLineC(UTF8STRC("<h2>Users</h2>"));
	writer->WriteLineC(UTF8STRC("<a href=\"useradd\">Add User</a><br/>"));
	writer->WriteLineC(UTF8STRC("<table border=\"1\"><tr><td>User Name</td><td>Action</td></tr>"));
	i = 0;
	j = userList.GetCount();
	while (i < j)
	{
		user = userList.GetItem(i);
		writer->WriteStrC(UTF8STRC("<tr><td>"));
		Sync::RWMutexUsage mutUsage(user->mut, false);
		sptr = Text::StrInt32(sbuff, user->userId);
		WriteHTMLText(writer, user->userName);
		mutUsage.EndUse();
		writer->WriteStrC(UTF8STRC("</td><td><a href=\"userreset?id="));
		writer->WriteStrC(sbuff, (UOSInt)(sptr - sbuff));
		writer->WriteStrC(UTF8STRC("\">Reset Password</a>"));
		writer->WriteStrC(UTF8STRC("<br/><a href=\"userassign?id="));
		writer->WriteStrC(sbuff, (UOSInt)(sptr - sbuff));
		writer->WriteStrC(UTF8STRC("\">Assign Devices</a>"));
		writer->WriteLineC(UTF8STRC("</td></tr>"));
		i++;
	}
	writer->WriteLineC(UTF8STRC("</table>"));
	writer->WriteLineC(UTF8STRC("</td></tr></table></body>"));
	writer->WriteLineC(UTF8STRC("</html>"));
	sess->EndUse();

	DEL_CLASS(writer);
	buff = mstm.GetBuff(&buffSize);
	resp->AddDefHeaders(req);
	resp->AddContentType(CSTR("text/html"));
	resp->AddContentLength(buffSize);
	resp->AddHeader(CSTR("Cache-Control"), CSTR("no-cache"));
	resp->Write(buff, buffSize);
	return true;
}

Bool __stdcall SSWR::SMonitor::SMonitorWebHandler::UserAddReq(SSWR::SMonitor::SMonitorWebHandler *me, NotNullPtr<Net::WebServer::IWebRequest> req, NotNullPtr<Net::WebServer::IWebResponse> resp)
{
	Text::UTF8Writer *writer;
	UInt8 *buff;
	UOSInt buffSize;
	Net::WebServer::IWebSession *sess = me->sessMgr->GetSession(req, resp);
	if (sess == 0)
	{
		return resp->RedirectURL(req, CSTR("/monitor/index"), 0);
	}
	if (sess->GetValueInt32(UTF8STRC("UserType")) != 1)
	{
		sess->EndUse();
		return resp->RedirectURL(req, CSTR("/monitor/index"), 0);
	}

	if (req->GetReqMethod() == Net::WebUtil::RequestMethod::HTTP_POST)
	{
		req->ParseHTTPForm();
		Text::String *action;
		Text::String *userName;
		action = req->GetHTTPFormStr(CSTR("action"));
		userName = req->GetHTTPFormStr(CSTR("username"));
		if (action && userName && action->Equals(UTF8STRC("adduser")))
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
	writer->WriteLineC(UTF8STRC("<body onload=\"document.getElementById('username').focus()\">"));
	writer->WriteLineC(UTF8STRC("<table width=\"100%\"><tr><td width=\"100\" class=\"menu\">"));
	me->WriteMenu(writer, sess);
	writer->WriteLineC(UTF8STRC("</td><td>"));
	writer->WriteLineC(UTF8STRC("<h2>Add User</h2>"));
	writer->WriteLineC(UTF8STRC("<form name=\"adduser\" method=\"POST\" action=\"useradd\">"));
	writer->WriteLineC(UTF8STRC("<input type=\"hidden\" name=\"action\" value=\"adduser\"/>"));
	writer->WriteLineC(UTF8STRC("<table border=\"1\"><tr><td>User Name</td><td><input type=\"text\" name=\"username\" id=\"username\" /></td></tr>"));
	writer->WriteLineC(UTF8STRC("<tr><td></td><td><input type=\"submit\" /></td></tr>"));
	writer->WriteLineC(UTF8STRC("</table>"));
	writer->WriteLineC(UTF8STRC("</td></tr></table></body>"));
	writer->WriteLineC(UTF8STRC("</html>"));
	sess->EndUse();

	DEL_CLASS(writer);
	buff = mstm.GetBuff(&buffSize);
	resp->AddDefHeaders(req);
	resp->AddContentType(CSTR("text/html"));
	resp->AddContentLength(buffSize);
	resp->AddHeader(CSTR("Cache-Control"), CSTR("no-cache"));
	resp->Write(buff, buffSize);
	return true;
}

Bool __stdcall SSWR::SMonitor::SMonitorWebHandler::UserAssignReq(SSWR::SMonitor::SMonitorWebHandler *me, NotNullPtr<Net::WebServer::IWebRequest> req, NotNullPtr<Net::WebServer::IWebResponse> resp)
{
	Text::UTF8Writer *writer;
	UInt8 *buff;
	UOSInt buffSize;
	UTF8Char sbuff[64];
	UTF8Char *sptr;
	Net::WebServer::IWebSession *sess = me->sessMgr->GetSession(req, resp);
	SSWR::SMonitor::ISMonitorCore::WebUser *user;
	Int32 userId;
	UOSInt i;
	UOSInt j;

	if (!req->GetQueryValueI32(CSTR("id"), userId))
	{
		return resp->RedirectURL(req, CSTR("/monitor/users"), 0);
	}
	user = me->core->UserGet(userId);
	if (user == 0 || user->userType != 2)
	{
		return resp->RedirectURL(req, CSTR("/monitor/users"), 0);
	}
	if (sess == 0)
	{
		return resp->RedirectURL(req, CSTR("/monitor/index"), 0);
	}
	if (sess->GetValueInt32(UTF8STRC("UserType")) != 1)
	{
		sess->EndUse();
		return resp->RedirectURL(req, CSTR("/monitor/index"), 0);
	}

	if (req->GetReqMethod() == Net::WebUtil::RequestMethod::HTTP_POST)
	{
		req->ParseHTTPForm();
		Text::String *action;
		Text::String *devicestr;
		action = req->GetHTTPFormStr(CSTR("action"));
		devicestr = req->GetHTTPFormStr(CSTR("device"));
		if (action && devicestr && action->Equals(UTF8STRC("userassign")))
		{
			Data::ArrayListInt64 devIds;
			UTF8Char *sarr[2];
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

			if (valid && me->core->UserAssign(userId, &devIds))
			{
				sess->EndUse();
				return resp->RedirectURL(req, CSTR("/monitor/users"), 0);
			}
		}
	}

	Data::ArrayList<SSWR::SMonitor::ISMonitorCore::DeviceInfo *> devList;
	SSWR::SMonitor::ISMonitorCore::DeviceInfo *dev;
	me->core->UserGetDevices(sess->GetValueInt32(UTF8STRC("UserId")), 1, &devList);

	IO::MemoryStream mstm;
	NEW_CLASS(writer, Text::UTF8Writer(mstm));
	WriteHeaderBegin(writer);
	WriteHeaderEnd(writer);
	writer->WriteLineC(UTF8STRC("<body onload=\"document.getElementById('username').focus()\">"));
	writer->WriteLineC(UTF8STRC("<table width=\"100%\"><tr><td width=\"100\" class=\"menu\">"));
	me->WriteMenu(writer, sess);
	writer->WriteLineC(UTF8STRC("</td><td>"));
	writer->WriteLineC(UTF8STRC("<h2>User Assign</h2>"));
	writer->WriteStrC(UTF8STRC("User Name: "));
	Sync::RWMutexUsage userMutUsage(user->mut, false);
	WriteHTMLText(writer, user->userName);
	writer->WriteLineC(UTF8STRC("<br/>"));
	writer->WriteStrC(UTF8STRC("<form name=\"userassign\" method=\"POST\" action=\"userassign?id="));
	sptr = Text::StrInt32(sbuff, user->userId);
	writer->WriteStrC(sbuff, (UOSInt)(sptr - sbuff));
	writer->WriteLineC(UTF8STRC("\">"));
	writer->WriteLineC(UTF8STRC("<input type=\"hidden\" name=\"action\" value=\"userassign\"/>"));
	writer->WriteLineC(UTF8STRC("Device List<br/>"));
	i = 0;
	j = devList.GetCount();
	while (i < j)
	{
		dev = devList.GetItem(i);
		Sync::RWMutexUsage devMutUsage(dev->mut, false);
		writer->WriteStrC(UTF8STRC("<input type=\"checkbox\" name=\"device\" id=\"device"));
		sptr = Text::StrInt64(sbuff, dev->cliId);
		writer->WriteStrC(sbuff, (UOSInt)(sptr - sbuff));
		writer->WriteStrC(UTF8STRC("\" value=\""));
		writer->WriteStrC(sbuff, (UOSInt)(sptr - sbuff));
		writer->WriteStrC(UTF8STRC("\""));
		if (user->devMap.Get(dev->cliId))
		{
			writer->WriteStrC(UTF8STRC(" checked"));
		}
		writer->WriteStrC(UTF8STRC("/><label for=\"device"));
		writer->WriteStrC(sbuff, (UOSInt)(sptr - sbuff));
		writer->WriteStrC(UTF8STRC("\">"));
		if (dev->devName)
		{
			WriteHTMLText(writer, Text::String::OrEmpty(dev->devName));
		}
		else
		{
			WriteHTMLText(writer, dev->platformName);
		}
		writer->WriteStrC(UTF8STRC("</label><br/>"));
		devMutUsage.EndUse();
		i++;
	}
	userMutUsage.EndUse();
	writer->WriteLineC(UTF8STRC("<input type=\"submit\" />"));
	writer->WriteLineC(UTF8STRC("</td></tr></table></body>"));
	writer->WriteLineC(UTF8STRC("</html>"));
	sess->EndUse();

	DEL_CLASS(writer);
	buff = mstm.GetBuff(&buffSize);
	resp->AddDefHeaders(req);
	resp->AddContentType(CSTR("text/html"));
	resp->AddContentLength(buffSize);
	resp->AddHeader(CSTR("Cache-Control"), CSTR("no-cache"));
	resp->Write(buff, buffSize);
	return true;
}

void __stdcall SSWR::SMonitor::SMonitorWebHandler::WriteHeaderBegin(IO::Writer *writer)
{
	writer->WriteLineC(UTF8STRC("<html><head><title>Monitor</title>"));
	writer->WriteLineC(UTF8STRC("<style>"));
	writer->WriteLineC(UTF8STRC(".menu {"));
	writer->WriteLineC(UTF8STRC("	vertical-align: top;"));
	writer->WriteLineC(UTF8STRC("}"));
	writer->WriteLineC(UTF8STRC("</style>"));
}

void __stdcall SSWR::SMonitor::SMonitorWebHandler::WriteHeaderEnd(IO::Writer *writer)
{
	writer->WriteLineC(UTF8STRC("</head>"));
}

void __stdcall SSWR::SMonitor::SMonitorWebHandler::WriteMenu(IO::Writer *writer, Net::WebServer::IWebSession *sess)
{
	Int32 userType = 0;
	if (sess)
	{
		Data::DateTime dt;
		dt.SetCurrTimeUTC();
		sess->SetValueInt64(UTF8STRC("LastSessTime"), dt.ToTicks());
		userType = sess->GetValueInt32(UTF8STRC("UserType"));
	}
	if (userType == 0)
	{
		writer->WriteLineC(UTF8STRC("<a href=\"login\">Login</a><br/>"));
	}
	if (userType != 0)
	{
		writer->WriteLineC(UTF8STRC("<a href=\"logout\">Logout</a><br/>"));
	}
	writer->WriteLineC(UTF8STRC("<br/>"));
	writer->WriteLineC(UTF8STRC("<a href=\"index\">Home</a><br/>"));
	if (userType != 0)
	{
		writer->WriteLineC(UTF8STRC("<a href=\"device\">Devices</a><br/>"));
		writer->WriteLineC(UTF8STRC("<a href=\"userpassword\">Password</a><br/>"));
	}
	writer->WriteLineC(UTF8STRC("<a href=\"pastdata\">Past Data</a><br/>"));
	if (userType == 1)
	{
		writer->WriteLineC(UTF8STRC("<a href=\"users\">Users</a><br/>"));
	}
}

void __stdcall SSWR::SMonitor::SMonitorWebHandler::WriteHTMLText(IO::Writer *writer, const UTF8Char *txt)
{
	NotNullPtr<Text::String> xmlTxt = Text::XML::ToNewHTMLBodyText(txt);
	writer->WriteStrC(xmlTxt->v, xmlTxt->leng);
	xmlTxt->Release();
}

void __stdcall SSWR::SMonitor::SMonitorWebHandler::WriteHTMLText(IO::Writer *writer, NotNullPtr<Text::String> txt)
{
	NotNullPtr<Text::String> xmlTxt = Text::XML::ToNewHTMLBodyText(txt->v);
	writer->WriteStrC(xmlTxt->v, xmlTxt->leng);
	xmlTxt->Release();
}

void __stdcall SSWR::SMonitor::SMonitorWebHandler::WriteHTMLText(IO::Writer *writer, Text::CString txt)
{
	NotNullPtr<Text::String> xmlTxt = Text::XML::ToNewHTMLBodyText(txt.v);
	writer->WriteStrC(xmlTxt->v, xmlTxt->leng);
	xmlTxt->Release();
}

void __stdcall SSWR::SMonitor::SMonitorWebHandler::WriteAttrText(IO::Writer *writer, const UTF8Char *txt)
{
	NotNullPtr<Text::String> xmlTxt = Text::XML::ToNewAttrText(txt);
	writer->WriteStrC(xmlTxt->v, xmlTxt->leng);
	xmlTxt->Release();
}

void __stdcall SSWR::SMonitor::SMonitorWebHandler::WriteAttrText(IO::Writer *writer, Text::String *txt)
{
	NotNullPtr<Text::String> xmlTxt = Text::XML::ToNewAttrText(STR_PTR(txt));
	writer->WriteStrC(xmlTxt->v, xmlTxt->leng);
	xmlTxt->Release();
}

void __stdcall SSWR::SMonitor::SMonitorWebHandler::WriteAttrText(IO::Writer *writer, NotNullPtr<Text::String> txt)
{
	NotNullPtr<Text::String> xmlTxt = Text::XML::ToNewAttrText(txt->v);
	writer->WriteStrC(xmlTxt->v, xmlTxt->leng);
	xmlTxt->Release();
}

void __stdcall SSWR::SMonitor::SMonitorWebHandler::WriteJSText(IO::Writer *writer, const UTF8Char *txt)
{
	NotNullPtr<Text::String> jsTxt = Text::JSText::ToNewJSText(txt);
	writer->WriteStrC(jsTxt->v, jsTxt->leng);
	jsTxt->Release();
}

void __stdcall SSWR::SMonitor::SMonitorWebHandler::WriteJSText(IO::Writer *writer, Text::String *txt)
{
	NotNullPtr<Text::String> jsTxt = Text::JSText::ToNewJSText(txt);
	writer->WriteStrC(jsTxt->v, jsTxt->leng);
	jsTxt->Release();
}

void __stdcall SSWR::SMonitor::SMonitorWebHandler::WriteJSText(IO::Writer *writer, NotNullPtr<Text::String> txt)
{
	NotNullPtr<Text::String> jsTxt = Text::JSText::ToNewJSText(txt);
	writer->WriteStrC(jsTxt->v, jsTxt->leng);
	jsTxt->Release();
}

Bool SSWR::SMonitor::SMonitorWebHandler::ProcessRequest(NotNullPtr<Net::WebServer::IWebRequest> req, NotNullPtr<Net::WebServer::IWebResponse> resp, Text::CString subReq)
{
	if (this->DoRequest(req, resp, subReq))
	{
		return true;
	}
	RequestHandler reqHdlr = this->reqMap->GetC(subReq);
	if (reqHdlr)
	{
		return reqHdlr(this, req, resp);
	}
	resp->ResponseError(req, Net::WebStatus::SC_NOT_FOUND);
	return true;
}

SSWR::SMonitor::SMonitorWebHandler::SMonitorWebHandler(SSWR::SMonitor::ISMonitorCore *core)
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
