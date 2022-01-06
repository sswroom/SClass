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
#include "Text/JSText.h"
#include "Text/MyStringFloat.h"
#include "Text/StringBuilderUTF8.h"
#include "Text/XML.h"
#include "Text/UTF8Writer.h"
#include <wchar.h>

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
	Int64 t = dt.ToTicks() - sess->GetValueInt64("LastSessTime");
	if (t > 900000)
	{
		return true;
	}
	return false;
}

Bool __stdcall SSWR::SMonitor::SMonitorWebHandler::DefaultReq(SSWR::SMonitor::SMonitorWebHandler *me, Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp)
{
	return resp->RedirectURL(req, (const UTF8Char*)"/monitor/index", -2);
}

Bool __stdcall SSWR::SMonitor::SMonitorWebHandler::IndexReq(SSWR::SMonitor::SMonitorWebHandler *me, Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp)
{
	IO::MemoryStream *mstm;
	Text::UTF8Writer *writer;
	UInt8 *buff;
	UOSInt buffSize;
	UTF8Char sbuff[64];
	UTF8Char *sptr;

	if (!me->core->UserExist())
	{
		if (req->GetReqMethod() == Net::WebServer::IWebRequest::RequestMethod::HTTP_POST)
		{
			req->ParseHTTPForm();
			Text::String *pwd = req->GetHTTPFormStr(UTF8STRC("password"));
			Text::String *retype = req->GetHTTPFormStr(UTF8STRC("password"));
			if (pwd && retype)
			{
				if (pwd->leng >= 3 && pwd->Equals(retype))
				{
					me->core->UserAdd((const UTF8Char*)"admin", pwd->v, 1);
					return resp->RedirectURL(req, (const UTF8Char*)"index", 0);
				}
			}
		}
		NEW_CLASS(mstm, IO::MemoryStream((const UTF8Char*)"SSWR.SMonitor.SMonitorWebHandler.IndexReq"));
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
		NEW_CLASS(mstm, IO::MemoryStream((const UTF8Char*)"SSWR.SMonitor.SMonitorWebHandler.IndexReq"));
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
			userId = sess->GetValueInt32("UserId");
			userType = sess->GetValueInt32("UserType");
		}
		else
		{
			userId = 0;
			userType = 0;
		}

		Text::String *reqDevId = req->GetQueryValue(UTF8STRC("devid"));
		Text::String *reqOutput = req->GetQueryValue(UTF8STRC("output"));
		if (reqDevId && reqOutput)
		{
			Int64 idevId = reqDevId->ToInt64();
			if (me->core->UserHasDevice(userId, userType, idevId))
			{
				Text::StringBuilderUTF8 sb;
				sb.Append(reqOutput);
				UTF8Char *sarr[3];
				UOSInt i;
				i = Text::StrSplit(sarr, 3, sb.ToString(), ',');
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
			dev->mut->LockRead();
			writer->WriteStrC(UTF8STRC("<tr><td>"));
			if (dev->devName)
			{
				WriteHTMLText(writer, dev->devName);
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

			dev->mut->UnlockRead();
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
	buff = mstm->GetBuff(&buffSize);
	resp->AddDefHeaders(req);
	resp->AddContentType((const UTF8Char*)"text/html");
	resp->AddContentLength(buffSize);
	resp->AddHeader((const UTF8Char*)"Cache-Control", (const UTF8Char*)"no-cache");
	resp->Write(buff, buffSize);
	DEL_CLASS(mstm);
	return true;
}

Bool __stdcall SSWR::SMonitor::SMonitorWebHandler::LoginReq(SSWR::SMonitor::SMonitorWebHandler *me, Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp)
{
	IO::MemoryStream *mstm;
	Text::UTF8Writer *writer;
	UInt8 *buff;
	UOSInt buffSize;
	Net::WebServer::IWebSession *sess = me->sessMgr->GetSession(req, resp);
	const UTF8Char *msg = 0;
	if (sess)
	{
		sess->EndUse();
		return resp->RedirectURL(req, (const UTF8Char*)"index", 0);
	}
	if (req->GetReqMethod() == Net::WebServer::IWebRequest::RequestMethod::HTTP_POST)
	{
		req->ParseHTTPForm();
		Text::String *s = req->GetHTTPFormStr(UTF8STRC("action"));
		Text::String *s2 = req->GetHTTPFormStr(UTF8STRC("user"));
		Text::String *s3 = req->GetHTTPFormStr(UTF8STRC("pwd"));
		if (s && s2 && s3 && s->Equals((const UTF8Char*)"login"))
		{
			if (s2->v[0])
			{
				SSWR::SMonitor::ISMonitorCore::LoginInfo *login = me->core->UserLogin(s2->v, s3->v);
				if (login != 0)
				{
					sess = me->sessMgr->CreateSession(req, resp);
					sess->SetValueInt32("UserId", login->userId);
					sess->SetValueInt32("LoginId", login->loginId);
					sess->SetValueInt32("UserType", login->userType);
					sess->EndUse();
					me->core->UserFreeLogin(login);

					return resp->RedirectURL(req, (const UTF8Char*)"index", 0);
				}
				else
				{
					msg = (const UTF8Char*)"User name or password not correct";
				}
			}
		}
	}

	NEW_CLASS(mstm, IO::MemoryStream((const UTF8Char*)"SSWR.SMonitor.SMonitorWebHandler.LoginReq"));
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
	buff = mstm->GetBuff(&buffSize);
	resp->AddDefHeaders(req);
	resp->AddContentType((const UTF8Char*)"text/html");
	resp->AddContentLength(buffSize);
	resp->AddHeader((const UTF8Char*)"Cache-Control", (const UTF8Char*)"no-cache");
	resp->Write(buff, buffSize);
	DEL_CLASS(mstm);
	return true;
}

Bool __stdcall SSWR::SMonitor::SMonitorWebHandler::LogoutReq(SSWR::SMonitor::SMonitorWebHandler *me, Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp)
{
	Net::WebServer::IWebSession *sess = me->sessMgr->GetSession(req, resp);
	if (sess)
	{
		sess->EndUse();
		me->sessMgr->DeleteSession(req, resp);
	}
	return resp->RedirectURL(req, (const UTF8Char*)"index", 0);
}

Bool __stdcall SSWR::SMonitor::SMonitorWebHandler::DeviceReq(SSWR::SMonitor::SMonitorWebHandler *me, Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp)
{
	IO::MemoryStream *mstm;
	Text::UTF8Writer *writer;
	UInt8 *buff;
	UOSInt buffSize;
	UTF8Char sbuff[64];
	UTF8Char *sptr;
	Int64 devId;
	Net::WebServer::IWebSession *sess = me->sessMgr->GetSession(req, resp);
	if (sess == 0)
	{
		return resp->RedirectURL(req, (const UTF8Char*)"index", 0);
	}
	if (req->GetQueryValueI64(UTF8STRC("photo"), &devId))
	{
		if (me->core->UserHasDevice(sess->GetValueInt32("UserId"), sess->GetValueInt32("UserType"), devId))
		{
			me->core->SendCapturePhoto(devId);
		}
	}

	NEW_CLASS(mstm, IO::MemoryStream((const UTF8Char*)"SSWR.SMonitor.SMonitorWebHandler.DeviceReq"));
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
	me->core->UserGetDevices(sess->GetValueInt32("UserId"), sess->GetValueInt32("UserType"), &devList);
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
		dev->mut->LockRead();
		if (dev->devName)
		{
			WriteHTMLText(writer, dev->devName);
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
		dev->mut->UnlockRead();
		writer->WriteLineC(UTF8STRC("</td></tr>"));
		
		i++;
	}
	writer->WriteLineC(UTF8STRC("</table>"));

	writer->WriteLineC(UTF8STRC("</td></tr></table></body>"));
	writer->WriteLineC(UTF8STRC("</html>"));
	sess->EndUse();

	DEL_CLASS(writer);
	buff = mstm->GetBuff(&buffSize);
	resp->AddDefHeaders(req);
	resp->AddContentType((const UTF8Char*)"text/html");
	resp->AddContentLength(buffSize);
	resp->AddHeader((const UTF8Char*)"Cache-Control", (const UTF8Char*)"no-cache");
	resp->Write(buff, buffSize);
	DEL_CLASS(mstm);
	return true;
}

Bool __stdcall SSWR::SMonitor::SMonitorWebHandler::DeviceEditReq(SSWR::SMonitor::SMonitorWebHandler *me, Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp)
{
	IO::MemoryStream *mstm;
	Text::UTF8Writer *writer;
	UInt8 *buff;
	UOSInt buffSize;
	UTF8Char sbuff[64];
	UTF8Char *sptr;
	Net::WebServer::IWebSession *sess = me->sessMgr->GetSession(req, resp);
	if (sess == 0)
	{
		return resp->RedirectURL(req, (const UTF8Char*)"index", 0);
	}
	Text::String *cid = req->GetQueryValue(UTF8STRC("id"));
	Int64 cliId = 0;
	if (cid)
	{
		cliId = cid->ToInt64();
	}
	if (cliId == 0)
	{
		sess->EndUse();
		return resp->RedirectURL(req, (const UTF8Char*)"device", 0);
	}
	if (!me->core->UserHasDevice(sess->GetValueInt32("UserId"), sess->GetValueInt32("UserType"), cliId))
	{
		sess->EndUse();
		return resp->RedirectURL(req, (const UTF8Char*)"device", 0);
	}

	if (req->GetReqMethod() == Net::WebServer::IWebRequest::RequestMethod::HTTP_POST)
	{
		req->ParseHTTPForm();
		Text::String *action = req->GetHTTPFormStr(UTF8STRC("action"));
		if (action && action->Equals((const UTF8Char*)"modify"))
		{
			Text::String *devName = req->GetHTTPFormStr(UTF8STRC("devName"));
			Int32 flags = 0;
			Text::String *s;
			s = req->GetHTTPFormStr(UTF8STRC("anonymous"));
			if (s && s->v[0] == '1')
			{
				flags |= 1;
			}
			s = req->GetHTTPFormStr(UTF8STRC("removed"));
			if (s && s->v[0] == '1')
			{
				flags |= 2;
			}
			if (devName)
			{
				if (me->core->DeviceModify(cliId, devName->v, flags))
				{
					sess->EndUse();
					return resp->RedirectURL(req, (const UTF8Char*)"device", 0);
				}
			}
		}
	}

	NEW_CLASS(mstm, IO::MemoryStream((const UTF8Char*)"SSWR.SMonitor.SMonitorWebHandler.DeviceEditReq"));
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
	dev->mut->LockRead();
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
	dev->mut->UnlockRead();

	writer->WriteLineC(UTF8STRC("</td></tr></table></body>"));
	writer->WriteLineC(UTF8STRC("</html>"));
	sess->EndUse();

	DEL_CLASS(writer);
	buff = mstm->GetBuff(&buffSize);
	resp->AddDefHeaders(req);
	resp->AddContentType((const UTF8Char*)"text/html");
	resp->AddContentLength(buffSize);
	resp->AddHeader((const UTF8Char*)"Cache-Control", (const UTF8Char*)"no-cache");
	resp->Write(buff, buffSize);
	DEL_CLASS(mstm);
	return true;
}

Bool __stdcall SSWR::SMonitor::SMonitorWebHandler::DeviceReadingReq(SSWR::SMonitor::SMonitorWebHandler *me, Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp)
{
	IO::MemoryStream *mstm;
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
		return resp->RedirectURL(req, (const UTF8Char*)"index", 0);
	}
	Text::String *cid = req->GetQueryValue(UTF8STRC("id"));
	Int64 cliId = 0;
	if (cid)
	{
		cliId = cid->ToInt64();
	}
	if (cliId == 0)
	{
		sess->EndUse();
		return resp->RedirectURL(req, (const UTF8Char*)"device", 0);
	}
	if (!me->core->UserHasDevice(sess->GetValueInt32("UserId"), sess->GetValueInt32("UserType"), cliId))
	{
		sess->EndUse();
		return resp->RedirectURL(req, (const UTF8Char*)"device", 0);
	}

	dev = me->core->DeviceGet(cliId);
	if (req->GetReqMethod() == Net::WebServer::IWebRequest::RequestMethod::HTTP_POST)
	{
		req->ParseHTTPForm();
		Text::String *action = req->GetHTTPFormStr(UTF8STRC("action"));
		if (action && action->Equals((const UTF8Char *)"reading"))
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
				s = req->GetHTTPFormStr(sbuff, (UOSInt)(sptr - sbuff));
				if (s)
				{
					sb.Append(s);
				}
				i++;
			}
			if (me->core->DeviceSetReadings(dev, sb.ToString()))
			{
				sess->EndUse();
				return resp->RedirectURL(req, (const UTF8Char*)"device", 0);
			}
		}
	}

	if (dev->nReading <= 0)
	{
		sess->EndUse();
		return resp->RedirectURL(req, (const UTF8Char*)"device", 0);
	}

	NEW_CLASS(mstm, IO::MemoryStream((const UTF8Char*)"SSWR.SMonitor.SMonitorWebHandler.DeviceReadingReq"));
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
	dev->mut->LockRead();
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
	dev->mut->UnlockRead();

	writer->WriteLineC(UTF8STRC("</td></tr></table></body>"));
	writer->WriteLineC(UTF8STRC("</html>"));
	sess->EndUse();

	DEL_CLASS(writer);
	buff = mstm->GetBuff(&buffSize);
	resp->AddDefHeaders(req);
	resp->AddContentType((const UTF8Char*)"text/html");
	resp->AddContentLength(buffSize);
	resp->AddHeader((const UTF8Char*)"Cache-Control", (const UTF8Char*)"no-cache");
	resp->Write(buff, buffSize);
	DEL_CLASS(mstm);
	return true;
}

Bool __stdcall SSWR::SMonitor::SMonitorWebHandler::DeviceDigitalsReq(SSWR::SMonitor::SMonitorWebHandler *me, Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp)
{
	IO::MemoryStream *mstm;
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
		return resp->RedirectURL(req, (const UTF8Char*)"index", 0);
	}
	Text::String *cid = req->GetQueryValue(UTF8STRC("id"));
	Int64 cliId = 0;
	if (cid)
	{
		cliId = cid->ToInt64();
	}
	if (cliId == 0)
	{
		sess->EndUse();
		return resp->RedirectURL(req, (const UTF8Char*)"device", 0);
	}
	if (!me->core->UserHasDevice(sess->GetValueInt32("UserId"), sess->GetValueInt32("UserType"), cliId))
	{
		sess->EndUse();
		return resp->RedirectURL(req, (const UTF8Char*)"device", 0);
	}

	dev = me->core->DeviceGet(cliId);
	if (req->GetReqMethod() == Net::WebServer::IWebRequest::RequestMethod::HTTP_POST)
	{
		req->ParseHTTPForm();
		Text::String *action = req->GetHTTPFormStr(UTF8STRC("action"));
		if (action && action->Equals((const UTF8Char *)"digitals"))
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
				s = req->GetHTTPFormStr(sbuff, (UOSInt)(sptr - sbuff));
				if (s)
				{
					sb.Append(s);
				}
				i++;
			}
			if (me->core->DeviceSetDigitals(dev, sb.ToString()))
			{
				sess->EndUse();
				return resp->RedirectURL(req, (const UTF8Char*)"device", 0);
			}
		}
	}

	if (dev->ndigital <= 0)
	{
		sess->EndUse();
		return resp->RedirectURL(req, (const UTF8Char*)"device", 0);
	}

	NEW_CLASS(mstm, IO::MemoryStream((const UTF8Char*)"SSWR.SMonitor.SMonitorWebHandler.DeviceDigitalsReq"));
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
	dev->mut->LockRead();
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
	dev->mut->UnlockRead();

	writer->WriteLineC(UTF8STRC("</td></tr></table></body>"));
	writer->WriteLineC(UTF8STRC("</html>"));
	sess->EndUse();

	DEL_CLASS(writer);
	buff = mstm->GetBuff(&buffSize);
	resp->AddDefHeaders(req);
	resp->AddContentType((const UTF8Char*)"text/html");
	resp->AddContentLength(buffSize);
	resp->AddHeader((const UTF8Char*)"Cache-Control", (const UTF8Char*)"no-cache");
	resp->Write(buff, buffSize);
	DEL_CLASS(mstm);
	return true;
}

Bool __stdcall SSWR::SMonitor::SMonitorWebHandler::DeviceReadingImgReq(SSWR::SMonitor::SMonitorWebHandler *me, Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp)
{
	Text::String *s;
	Int64 cliId = 0;
	Int32 userId = 0;
	Int32 userType = 0;
	Int32 sensorId = 0;
	Int32 readingId = 0;
	Int32 readingType = 0;
	Bool valid = true;
	s = req->GetQueryValue(UTF8STRC("id"));
	if (s == 0)
	{
		valid = false;
	}
	else if (!s->ToInt64(&cliId))
	{
		valid = false;
	}

	s = req->GetQueryValue(UTF8STRC("sensor"));
	if (s == 0)
	{
		valid = false;
	}
	else if (!s->ToInt32(&sensorId))
	{
		valid = false;
	}

	s = req->GetQueryValue(UTF8STRC("reading"));
	if (s == 0)
	{
		valid = false;
	}
	else if (!s->ToInt32(&readingId))
	{
		valid = false;
	}

	s = req->GetQueryValue(UTF8STRC("readingType"));
	if (s == 0)
	{
		valid = false;
	}
	else if (!s->ToInt32(&readingType))
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
		userId = sess->GetValueInt32("UserId");
		userType = sess->GetValueInt32("UserType");
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
		Data::ArrayList<IO::MemoryStream *> *cacheList;
		dev->mut->LockWrite();
		dev->valUpdated = false;
		cacheList = dev->imgCaches->GetValues();
		i = cacheList->GetCount();
		while (i-- > 0)
		{
			mstm = cacheList->GetItem(i);
			DEL_CLASS(mstm);
		}
		dev->imgCaches->Clear();
		dev->mut->UnlockWrite();
	}
	
	dev->mut->LockRead();
	mstm = dev->imgCaches->Get((sensorId << 16) + (readingId << 8) + (readingType));
	if (mstm)
	{
		buff = mstm->GetBuff(&buffSize);
		resp->AddDefHeaders(req);
		resp->AddContentType((const UTF8Char*)"image/png");
		resp->AddContentLength(buffSize);
		resp->AddHeader((const UTF8Char*)"Cache-Control", (const UTF8Char*)"no-cache");
		resp->Write(buff, buffSize);
		dev->mut->UnlockRead();
		return true;
	}

	Media::DrawEngine *deng = me->core->GetDrawEngine();
	Media::DrawImage *dimg = deng->CreateImage32(640, 120, Media::AT_NO_ALPHA);
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
		f = dimg->NewFontPx((const UTF8Char*)"Arial", 12, Media::DrawEngine::DFS_ANTIALIAS, 0);
		b = dimg->NewBrushARGB(0xffffffff);
		dimg->DrawRect(0, 0, Math::UOSInt2Double(dimg->GetWidth()), Math::UOSInt2Double(dimg->GetHeight()), 0, b);
		dimg->DelBrush(b);
		b = dimg->NewBrushARGB(0xff000000);
		dimg->DrawString(0, 0, (const UTF8Char*)"Sensor not found", f, b);
		dimg->DelBrush(b);
		dimg->DelFont(f);
	}
	else
	{
		Data::ArrayListInt64 dateList;
		Data::ArrayListDbl valList;
		Data::ArrayList<SSWR::SMonitor::ISMonitorCore::DevRecord2 *> *recList;
		SSWR::SMonitor::ISMonitorCore::DevRecord2 *rec;
		recList = dev->todayRecs->GetValues();
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
			j = recList->GetCount();
			while (i < j)
			{
				rec = recList->GetItem(i);
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
			j = recList->GetCount();
			while (i < j)
			{
				rec = recList->GetItem(i);
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
			sb.Append(dev->readingNames[readingIndex]);
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
		Text::SBAppendF64(&sb, currVal);
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
			recList = dev->yesterdayRecs->GetValues();
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
				j = recList->GetCount();
				while (i < j)
				{
					rec = recList->GetItem(i);
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
					Text::SBAppendF64(&sb, currVal - yesterdayVal);
				}
			}
			else
			{
				i = 0;
				j = recList->GetCount();
				while (i < j)
				{
					rec = recList->GetItem(i);
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
					Text::SBAppendF64(&sb, dev->readings[readingIndex].reading - yesterdayVal);
				}
			}

			const UTF8Char *u8ptr = Text::StrCopyNew(sb.ToString());
			NEW_CLASS(chart, Data::LineChart(u8ptr));
			Text::StrDelNew(u8ptr);
			if (dateList2.GetCount() >= 2)
			{
				chart->AddXDataDate(dateList2.GetArray(&k), dateList2.GetCount());
				chart->AddYData((const UTF8Char*)"Yesterday", valList2.GetArray(&k), valList2.GetCount(), 0xffcccccc, Data::LineChart::LS_LINE);
			}
			chart->AddXDataDate(dateList.GetArray(&k), dateList.GetCount());
			chart->AddYData((const UTF8Char*)"Reading", valList.GetArray(&k), valList.GetCount(), 0xffff0000, Data::LineChart::LS_LINE);

			chart->SetDateFormat("");
			chart->SetFontHeightPt(10);
			chart->SetTimeZoneQHR(32);
			chart->SetTimeFormat("HH:mm");
			chart->Plot(dimg, 0, 0, Math::UOSInt2Double(dimg->GetWidth()), Math::UOSInt2Double(dimg->GetHeight()));
			DEL_CLASS(chart);
		}
		else
		{
			f = dimg->NewFontPx((const UTF8Char*)"Arial", 12, Media::DrawEngine::DFS_ANTIALIAS, 0);
			b = dimg->NewBrushARGB(0xffffffff);
			dimg->DrawRect(0, 0, Math::UOSInt2Double(dimg->GetWidth()), Math::UOSInt2Double(dimg->GetHeight()), 0, b);
			dimg->DelBrush(b);
			b = dimg->NewBrushARGB(0xff000000);
			dimg->DrawString(0, 0, sb.ToString(), f, b);
			dimg->DrawString(0, 12, (const UTF8Char*)"Data not enough", f, b);
			dimg->DelBrush(b);
			dimg->DelFont(f);
		}
	}
	dev->mut->UnlockRead();
	
	Exporter::GUIPNGExporter *exporter;
	Media::ImageList *imgList;

	NEW_CLASS(imgList, Media::ImageList((const UTF8Char*)"temp.png"));
	imgList->AddImage(dimg->ToStaticImage(), 0);
	deng->DeleteImage(dimg);

	NEW_CLASS(mstm, IO::MemoryStream((const UTF8Char*)"SSWR.SMonitor.SMonitorWebHandler.DeviceReadingImgReq"));
	NEW_CLASS(exporter, Exporter::GUIPNGExporter());
	exporter->ExportFile(mstm, (const UTF8Char*)"temp.png", imgList, 0);
	DEL_CLASS(exporter);
	DEL_CLASS(imgList);

	buff = mstm->GetBuff(&buffSize);
	resp->AddDefHeaders(req);
	resp->AddContentType((const UTF8Char*)"image/png");
	resp->AddContentLength(buffSize);
	resp->AddHeader((const UTF8Char*)"Cache-Control", (const UTF8Char*)"no-cache");
	resp->Write(buff, buffSize);

	dev->mut->LockWrite();
	mstm = dev->imgCaches->Put((sensorId << 16) + (readingId << 8) + (readingType), mstm);
	dev->mut->UnlockWrite();
	if (mstm)
	{
		DEL_CLASS(mstm);
	}
	return true;
}

Bool __stdcall SSWR::SMonitor::SMonitorWebHandler::DevicePastDataReq(SSWR::SMonitor::SMonitorWebHandler *me, Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp)
{
	IO::MemoryStream *mstm;
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
		userId = sess->GetValueInt32("UserId");
		userType = sess->GetValueInt32("UserType");
	}

	UOSInt i;
	UOSInt j;
	UOSInt k;
	UOSInt l;
	Data::ArrayList<SSWR::SMonitor::ISMonitorCore::DeviceInfo *> devList;
	SSWR::SMonitor::ISMonitorCore::DeviceInfo *dev;
	NEW_CLASS(mstm, IO::MemoryStream((const UTF8Char*)"SSWR.SMonitor.SMonitorWebHandler.DevicePastDataReq"));
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
		dev->mut->LockRead();
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
				Text::StrInt32(Text::StrConcat(Text::StrInt32(Text::StrConcat(sbuff, (const UTF8Char*)"Sensor "), ReadInt16(dev->readings[k].status)), (const UTF8Char*)" Reading "), ReadInt16(&dev->readings[k].status[4]));
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
		dev->mut->UnlockRead();
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
	buff = mstm->GetBuff(&buffSize);
	resp->AddDefHeaders(req);
	resp->AddContentType((const UTF8Char*)"text/html");
	resp->AddContentLength(buffSize);
	resp->AddHeader((const UTF8Char*)"Cache-Control", (const UTF8Char*)"no-cache");
	resp->Write(buff, buffSize);
	DEL_CLASS(mstm);
	return true;
}

Bool __stdcall SSWR::SMonitor::SMonitorWebHandler::DevicePastDataImgReq(SSWR::SMonitor::SMonitorWebHandler *me, Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp)
{
	Text::String *s;
	Int64 cliId = 0;
	Int64 startTime = 0;
	Int32 userId = 0;
	Int32 userType = 0;
	Int32 sensorId = 0;
	Int32 readingId = 0;
	Bool valid = true;
	s = req->GetQueryValue(UTF8STRC("id"));
	if (s == 0)
	{
		valid = false;
	}
	else if (!s->ToInt64(&cliId))
	{
		valid = false;
	}

	s = req->GetQueryValue(UTF8STRC("sensor"));
	if (s == 0)
	{
		valid = false;
	}
	else if (!s->ToInt32(&sensorId))
	{
		valid = false;
	}

	s = req->GetQueryValue(UTF8STRC("reading"));
	if (s == 0)
	{
		valid = false;
	}
	else if (!s->ToInt32(&readingId))
	{
		valid = false;
	}

	s = req->GetQueryValue(UTF8STRC("starttime"));
	if (s == 0)
	{
		valid = false;
	}
	else if (!s->ToInt64(&startTime))
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
		userId = sess->GetValueInt32("UserId");
		userType = sess->GetValueInt32("UserType");
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
	IO::MemoryStream *mstm;
	UInt8 *buff;
	UOSInt buffSize;

	Media::DrawEngine *deng = me->core->GetDrawEngine();
	Media::DrawImage *dimg = deng->CreateImage32(640, 120, Media::AT_NO_ALPHA);
	Media::DrawFont *f;
	Media::DrawBrush *b;
	UOSInt readingIndex = (UOSInt)-1;
	Int32 readingType = 0;
	Data::LineChart *chart;
	dev->mut->LockRead();
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
	dev->mut->UnlockRead();
	if (readingIndex == (UOSInt)-1)
	{
		f = dimg->NewFontPx((const UTF8Char*)"Arial", 12, Media::DrawEngine::DFS_ANTIALIAS, 0);
		b = dimg->NewBrushARGB(0xffffffff);
		dimg->DrawRect(0, 0, Math::UOSInt2Double(dimg->GetWidth()), Math::UOSInt2Double(dimg->GetHeight()), 0, b);
		dimg->DelBrush(b);
		b = dimg->NewBrushARGB(0xff000000);
		dimg->DrawString(0, 0, (const UTF8Char*)"Sensor not found", f, b);
		dimg->DelBrush(b);
		dimg->DelFont(f);
	}
	else
	{
		UTF8Char sbuff[64];
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
		dev->mut->LockRead();
		if (dev->readingNames[readingIndex])
		{
			sb.Append(dev->readingNames[readingIndex]);
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
		dev->mut->UnlockRead();
		Data::DateTime dt;
		dt.SetTicks(startTime);
		dt.ToLocalTime();
		dt.ToString(sbuff, "yyyy-MM-dd");
		sb.AppendC(UTF8STRC(" "));
		sb.Append(sbuff);

		if (dateList.GetCount() >= 2)
		{
			const UTF8Char *u8ptr = Text::StrCopyNew(sb.ToString());
			NEW_CLASS(chart, Data::LineChart(u8ptr));
			Text::StrDelNew(u8ptr);
			chart->AddXDataDate(dateList.GetArray(&k), dateList.GetCount());
			chart->AddYData((const UTF8Char*)"Reading", valList.GetArray(&k), valList.GetCount(), 0xffff0000, Data::LineChart::LS_LINE);
			chart->SetDateFormat("");
			chart->SetFontHeightPt(10);
			chart->SetTimeZoneQHR(32);
			chart->SetTimeFormat("HH:mm");
			chart->Plot(dimg, 0, 0, Math::UOSInt2Double(dimg->GetWidth()), Math::UOSInt2Double(dimg->GetHeight()));
			DEL_CLASS(chart);
		}
		else
		{
			f = dimg->NewFontPx((const UTF8Char*)"Arial", 12, Media::DrawEngine::DFS_ANTIALIAS, 0);
			b = dimg->NewBrushARGB(0xffffffff);
			dimg->DrawRect(0, 0, Math::UOSInt2Double(dimg->GetWidth()), Math::UOSInt2Double(dimg->GetHeight()), 0, b);
			dimg->DelBrush(b);
			b = dimg->NewBrushARGB(0xff000000);
			dimg->DrawString(0, 0, sb.ToString(), f, b);
			dimg->DrawString(0, 12, (const UTF8Char*)"Data not enough", f, b);
			dimg->DelBrush(b);
			dimg->DelFont(f);
		}
	}
	
	Exporter::GUIPNGExporter *exporter;
	Media::ImageList *imgList;

	NEW_CLASS(imgList, Media::ImageList((const UTF8Char*)"temp.png"));
	imgList->AddImage(dimg->ToStaticImage(), 0);
	deng->DeleteImage(dimg);

	NEW_CLASS(mstm, IO::MemoryStream((const UTF8Char*)"SSWR.SMonitor.SMonitorWebHandler.DevicePastDataImgReq"));
	NEW_CLASS(exporter, Exporter::GUIPNGExporter());
	exporter->ExportFile(mstm, (const UTF8Char*)"temp.png", imgList, 0);
	DEL_CLASS(exporter);
	DEL_CLASS(imgList);

	buff = mstm->GetBuff(&buffSize);
	resp->AddDefHeaders(req);
	resp->AddContentType((const UTF8Char*)"image/png");
	resp->AddContentLength(buffSize);
	resp->AddHeader((const UTF8Char*)"Cache-Control", (const UTF8Char*)"no-cache");
	resp->Write(buff, buffSize);
	DEL_CLASS(mstm);
	return true;
}

Bool __stdcall SSWR::SMonitor::SMonitorWebHandler::UserPasswordReq(SSWR::SMonitor::SMonitorWebHandler *me, Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp)
{
	IO::MemoryStream *mstm;
	Text::UTF8Writer *writer;
	UInt8 *buff;
	UOSInt buffSize;
	Net::WebServer::IWebSession *sess = me->sessMgr->GetSession(req, resp);
	const UTF8Char *msg = 0;
	if (sess == 0)
	{
		return resp->RedirectURL(req, (const UTF8Char*)"index", 0);
	}

	if (req->GetReqMethod() == Net::WebServer::IWebRequest::RequestMethod::HTTP_POST)
	{
		req->ParseHTTPForm();
		Text::String *pwd = req->GetHTTPFormStr(UTF8STRC("password"));
		Text::String *retype = req->GetHTTPFormStr(UTF8STRC("retype"));
		if (pwd == 0 || pwd->v[0] == 0)
		{
			msg = (const UTF8Char*)"Password is empty";
		}
		else if (retype == 0 || retype->v[0] == 0)
		{
			msg = (const UTF8Char*)"Retype is empty";
		}
		else if (!pwd->Equals(retype))
		{
			msg = (const UTF8Char*)"Password and retype do not match";
		}
		else
		{
			UOSInt len = pwd->leng;
			if (len < 3)
			{
				msg = (const UTF8Char*)"Password is too short";
			}
			else
			{
				if (me->core->UserSetPassword(sess->GetValueInt32("UserId"), pwd->v))
				{
					msg = (const UTF8Char*)"Password is changed successfully";
				}
				else
				{
					msg = (const UTF8Char*)"Error in changing password";
				}
			}
		}
	}


	NEW_CLASS(mstm, IO::MemoryStream((const UTF8Char*)"SSWR.SMonitor.SMonitorWebHandler.UserPasswordReq"));
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
	if (msg)
	{
		WriteHTMLText(writer, msg);
	}
	writer->WriteLineC(UTF8STRC("</form></center>"));
	writer->WriteLineC(UTF8STRC("</td></tr></table></body>"));
	writer->WriteLineC(UTF8STRC("</html>"));
	sess->EndUse();

	DEL_CLASS(writer);
	buff = mstm->GetBuff(&buffSize);
	resp->AddDefHeaders(req);
	resp->AddContentType((const UTF8Char*)"text/html");
	resp->AddContentLength(buffSize);
	resp->AddHeader((const UTF8Char*)"Cache-Control", (const UTF8Char*)"no-cache");
	resp->Write(buff, buffSize);
	DEL_CLASS(mstm);
	return true;
}

Bool __stdcall SSWR::SMonitor::SMonitorWebHandler::UsersReq(SSWR::SMonitor::SMonitorWebHandler *me, Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp)
{
	IO::MemoryStream *mstm;
	Text::UTF8Writer *writer;
	UInt8 *buff;
	UOSInt buffSize;
	UTF8Char sbuff[64];
	UTF8Char *sptr;
	Net::WebServer::IWebSession *sess = me->sessMgr->GetSession(req, resp);
	if (sess == 0)
	{
		return resp->RedirectURL(req, (const UTF8Char*)"index", 0);
	}
	if (sess->GetValueInt32("UserType") != 1)
	{
		sess->EndUse();
		return resp->RedirectURL(req, (const UTF8Char*)"index", 0);
	}

	Data::ArrayList<SSWR::SMonitor::ISMonitorCore::WebUser*> userList;
	SSWR::SMonitor::ISMonitorCore::WebUser *user;
	UOSInt i;
	UOSInt j;
	me->core->UserGetList(&userList);
	NEW_CLASS(mstm, IO::MemoryStream((const UTF8Char*)"SSWR.SMonitor.SMonitorWebHandler.UsersReq"));
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
		user->mut->LockRead();
		sptr = Text::StrInt32(sbuff, user->userId);
		WriteHTMLText(writer, user->userName);
		user->mut->UnlockRead();
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
	buff = mstm->GetBuff(&buffSize);
	resp->AddDefHeaders(req);
	resp->AddContentType((const UTF8Char*)"text/html");
	resp->AddContentLength(buffSize);
	resp->AddHeader((const UTF8Char*)"Cache-Control", (const UTF8Char*)"no-cache");
	resp->Write(buff, buffSize);
	DEL_CLASS(mstm);
	return true;
}

Bool __stdcall SSWR::SMonitor::SMonitorWebHandler::UserAddReq(SSWR::SMonitor::SMonitorWebHandler *me, Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp)
{
	IO::MemoryStream *mstm;
	Text::UTF8Writer *writer;
	UInt8 *buff;
	UOSInt buffSize;
	Net::WebServer::IWebSession *sess = me->sessMgr->GetSession(req, resp);
	if (sess == 0)
	{
		return resp->RedirectURL(req, (const UTF8Char*)"index", 0);
	}
	if (sess->GetValueInt32("UserType") != 1)
	{
		sess->EndUse();
		return resp->RedirectURL(req, (const UTF8Char*)"index", 0);
	}

	if (req->GetReqMethod() == Net::WebServer::IWebRequest::RequestMethod::HTTP_POST)
	{
		req->ParseHTTPForm();
		Text::String *action;
		Text::String *userName;
		action = req->GetHTTPFormStr(UTF8STRC("action"));
		userName = req->GetHTTPFormStr(UTF8STRC("username"));
		if (action && userName && action->Equals((const UTF8Char*)"adduser"))
		{
			UOSInt len = userName->leng;
			if (len >= 3 && len < 128)
			{
				if (me->core->UserAdd(userName->v, userName->v, 2))
				{
					sess->EndUse();
					return resp->RedirectURL(req, (const UTF8Char*)"users", 0);
				}
			}
		}
	}

	NEW_CLASS(mstm, IO::MemoryStream((const UTF8Char*)"SSWR.SMonitor.SMonitorWebHandler.UserAddReq"));
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
	buff = mstm->GetBuff(&buffSize);
	resp->AddDefHeaders(req);
	resp->AddContentType((const UTF8Char*)"text/html");
	resp->AddContentLength(buffSize);
	resp->AddHeader((const UTF8Char*)"Cache-Control", (const UTF8Char*)"no-cache");
	resp->Write(buff, buffSize);
	DEL_CLASS(mstm);
	return true;
}

Bool __stdcall SSWR::SMonitor::SMonitorWebHandler::UserAssignReq(SSWR::SMonitor::SMonitorWebHandler *me, Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp)
{
	IO::MemoryStream *mstm;
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

	if (!req->GetQueryValueI32(UTF8STRC("id"), &userId))
	{
		return resp->RedirectURL(req, (const UTF8Char*)"users", 0);
	}
	user = me->core->UserGet(userId);
	if (user == 0 || user->userType != 2)
	{
		return resp->RedirectURL(req, (const UTF8Char*)"users", 0);
	}
	if (sess == 0)
	{
		return resp->RedirectURL(req, (const UTF8Char*)"index", 0);
	}
	if (sess->GetValueInt32("UserType") != 1)
	{
		sess->EndUse();
		return resp->RedirectURL(req, (const UTF8Char*)"index", 0);
	}

	if (req->GetReqMethod() == Net::WebServer::IWebRequest::RequestMethod::HTTP_POST)
	{
		req->ParseHTTPForm();
		Text::String *action;
		Text::String *devicestr;
		action = req->GetHTTPFormStr(UTF8STRC("action"));
		devicestr = req->GetHTTPFormStr(UTF8STRC("device"));
		if (action && devicestr && action->Equals((const UTF8Char*)"userassign"))
		{
			Data::ArrayListInt64 devIds;
			UTF8Char *sarr[2];
			Text::StringBuilderUTF8 sb;
			Int64 cliId;
			Bool valid = true;
			sb.Append(devicestr);
			sarr[1] = sb.ToString();
			while (true)
			{
				i = Text::StrSplit(sarr, 2, sarr[1], ',');
				if (!Text::StrToInt64(sarr[0], &cliId))
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
				return resp->RedirectURL(req, (const UTF8Char*)"users", 0);
			}
		}
	}

	Data::ArrayList<SSWR::SMonitor::ISMonitorCore::DeviceInfo *> devList;
	SSWR::SMonitor::ISMonitorCore::DeviceInfo *dev;
	me->core->UserGetDevices(sess->GetValueInt32("UserId"), 1, &devList);

	NEW_CLASS(mstm, IO::MemoryStream((const UTF8Char*)"SSWR.SMonitor.SMonitorWebHandler.UserAssignReq"));
	NEW_CLASS(writer, Text::UTF8Writer(mstm));
	WriteHeaderBegin(writer);
	WriteHeaderEnd(writer);
	writer->WriteLineC(UTF8STRC("<body onload=\"document.getElementById('username').focus()\">"));
	writer->WriteLineC(UTF8STRC("<table width=\"100%\"><tr><td width=\"100\" class=\"menu\">"));
	me->WriteMenu(writer, sess);
	writer->WriteLineC(UTF8STRC("</td><td>"));
	writer->WriteLineC(UTF8STRC("<h2>User Assign</h2>"));
	writer->WriteStrC(UTF8STRC("User Name: "));
	user->mut->LockRead();
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
		dev->mut->LockRead();
		writer->WriteStrC(UTF8STRC("<input type=\"checkbox\" name=\"device\" id=\"device"));
		sptr = Text::StrInt64(sbuff, dev->cliId);
		writer->WriteStrC(sbuff, (UOSInt)(sptr - sbuff));
		writer->WriteStrC(UTF8STRC("\" value=\""));
		writer->WriteStrC(sbuff, (UOSInt)(sptr - sbuff));
		writer->WriteStrC(UTF8STRC("\""));
		if (user->devMap->Get(dev->cliId))
		{
			writer->WriteStrC(UTF8STRC(" checked"));
		}
		writer->WriteStrC(UTF8STRC("/><label for=\"device"));
		writer->WriteStrC(sbuff, (UOSInt)(sptr - sbuff));
		writer->WriteStrC(UTF8STRC("\">"));
		if (dev->devName)
		{
			WriteHTMLText(writer, dev->devName);
		}
		else
		{
			WriteHTMLText(writer, dev->platformName);
		}
		writer->WriteStrC(UTF8STRC("</label><br/>"));
		dev->mut->UnlockRead();
		i++;
	}
	user->mut->UnlockRead();
	writer->WriteLineC(UTF8STRC("<input type=\"submit\" />"));
	writer->WriteLineC(UTF8STRC("</td></tr></table></body>"));
	writer->WriteLineC(UTF8STRC("</html>"));
	sess->EndUse();

	DEL_CLASS(writer);
	buff = mstm->GetBuff(&buffSize);
	resp->AddDefHeaders(req);
	resp->AddContentType((const UTF8Char*)"text/html");
	resp->AddContentLength(buffSize);
	resp->AddHeader((const UTF8Char*)"Cache-Control", (const UTF8Char*)"no-cache");
	resp->Write(buff, buffSize);
	DEL_CLASS(mstm);
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
		sess->SetValueInt64("LastSessTime", dt.ToTicks());
		userType = sess->GetValueInt32("UserType");
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
	Text::String *xmlTxt = Text::XML::ToNewHTMLText(txt);
	writer->WriteStrC(xmlTxt->v, xmlTxt->leng);
	xmlTxt->Release();
}

void __stdcall SSWR::SMonitor::SMonitorWebHandler::WriteAttrText(IO::Writer *writer, const UTF8Char *txt)
{
	Text::String *xmlTxt = Text::XML::ToNewAttrText(txt);
	writer->WriteStrC(xmlTxt->v, xmlTxt->leng);
	xmlTxt->Release();
}

void __stdcall SSWR::SMonitor::SMonitorWebHandler::WriteJSText(IO::Writer *writer, const UTF8Char *txt)
{
	Text::String *jsTxt = Text::JSText::ToNewJSText(txt);
	writer->WriteStrC(jsTxt->v, jsTxt->leng);
	jsTxt->Release();
}

Bool SSWR::SMonitor::SMonitorWebHandler::ProcessRequest(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, const UTF8Char *subReq)
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

SSWR::SMonitor::SMonitorWebHandler::SMonitorWebHandler(SSWR::SMonitor::ISMonitorCore *core)
{
	this->core = core;
	NEW_CLASS(this->reqMap, Data::StringUTF8Map<RequestHandler>());
	NEW_CLASS(this->sessMgr, Net::WebServer::MemoryWebSessionManager((const UTF8Char*)"/monitor", OnSessDeleted, this, 60000, OnSessCheck, this));
	this->reqMap->Put((const UTF8Char*)"", DefaultReq);
	this->reqMap->Put((const UTF8Char*)"/index", IndexReq);
	this->reqMap->Put((const UTF8Char*)"/login", LoginReq);
	this->reqMap->Put((const UTF8Char*)"/logout", LogoutReq);
	this->reqMap->Put((const UTF8Char*)"/device", DeviceReq);
	this->reqMap->Put((const UTF8Char*)"/devedit", DeviceEditReq);
	this->reqMap->Put((const UTF8Char*)"/devreading", DeviceReadingReq);
	this->reqMap->Put((const UTF8Char*)"/devdigitals", DeviceDigitalsReq);
	this->reqMap->Put((const UTF8Char*)"/devreadingimg", DeviceReadingImgReq);
	this->reqMap->Put((const UTF8Char*)"/pastdata", DevicePastDataReq);
	this->reqMap->Put((const UTF8Char*)"/pastdataimg", DevicePastDataImgReq);
	this->reqMap->Put((const UTF8Char*)"/userpassword", UserPasswordReq);
	this->reqMap->Put((const UTF8Char*)"/users", UsersReq);
	this->reqMap->Put((const UTF8Char*)"/useradd", UserAddReq);
//	this->reqMap->Put((const UTF8Char*)"/userreset", UserResetReq);
	this->reqMap->Put((const UTF8Char*)"/userassign", UserAssignReq);
}

SSWR::SMonitor::SMonitorWebHandler::~SMonitorWebHandler()
{
	DEL_CLASS(this->sessMgr);
	DEL_CLASS(this->reqMap);
}
