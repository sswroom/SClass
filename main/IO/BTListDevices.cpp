#include "Stdafx.h"
#include "Core/Core.h"
#include "IO/BTManager.h"
#include "IO/BTUtil.h"
#include "IO/ConsoleWriter.h"
#include "Text/StringBuilderUTF8.h"

Int32 MyMain(Core::IProgControl *progCtrl)
{
	OSInt i;
	OSInt j;
	OSInt k;
	OSInt l;
	Data::ArrayList<IO::BTController*> btCtrlList;
	Data::ArrayList<IO::BTController::BTDevice*> btDevList;
	IO::BTController *btCtrl;
	IO::BTController::BTDevice  *btDev;
	IO::ConsoleWriter console;
	Text::StringBuilderUTF8 sb;
	Data::DateTime dt;

	IO::BTManager btMgr;
	btMgr.CreateControllers(&btCtrlList);
	i = btCtrlList.GetCount();
	sb.AppendOSInt(i);
	sb.Append((const UTF8Char*)" controller found:");
	j = 0;
	console.WriteLine(sb.ToString());
	while (i-- > 0)
	{
		btCtrl = btCtrlList.GetItem(i);
		console.WriteLine();
		sb.ClearStr();
		sb.Append((const UTF8Char*)"Controller ");
		sb.AppendOSInt(j);
		sb.Append((const UTF8Char*)":");
		console.WriteLine(sb.ToString());
		sb.ClearStr();
		sb.Append((const UTF8Char*)"Name: ");
		sb.Append(btCtrl->GetName());
		console.WriteLine(sb.ToString());
		sb.ClearStr();
		sb.Append((const UTF8Char*)"Address: ");
		IO::BTUtil::GetAddrText(&sb, btCtrl->GetAddress());
		console.WriteLine(sb.ToString());
		sb.ClearStr();
		sb.Append((const UTF8Char*)"Manufacturer: ");
		sb.Append(IO::BTUtil::GetManufacturerName(btCtrl->GetManufacturer()));
		sb.Append((const UTF8Char*)" (0x");
		sb.AppendHex16(btCtrl->GetManufacturer());
		sb.Append((const UTF8Char*)")");
		console.WriteLine(sb.ToString());
		sb.ClearStr();
		sb.Append((const UTF8Char*)"Subversion: 0x");
		sb.AppendHex16(btCtrl->GetSubversion());
		console.WriteLine(sb.ToString());
		sb.ClearStr();
		sb.Append((const UTF8Char*)"Class of Device: 0x");
		sb.AppendHex32(btCtrl->GetDevClass());
		console.WriteLine(sb.ToString());

		console.WriteLine();
		sb.ClearStr();
		sb.Append((const UTF8Char*)"Controller ");
		sb.AppendOSInt(j);
		sb.Append((const UTF8Char*)" - Devices:");
		console.WriteLine(sb.ToString());

		btCtrl->CreateDevices(&btDevList, false);
		k = 0;
		l = btDevList.GetCount();
		while (k < l)
		{
			btDev = btDevList.GetItem(k);
			sb.ClearStr();
			sb.Append((const UTF8Char*)"Controller ");
			sb.AppendOSInt(j);
			sb.Append((const UTF8Char*)" Device ");
			sb.AppendOSInt(k);
			console.WriteLine(sb.ToString());
			sb.ClearStr();
			sb.Append((const UTF8Char*)"Name: ");
			sb.Append(btDev->GetName());
			console.WriteLine(sb.ToString());
			sb.ClearStr();
			sb.Append((const UTF8Char*)"Address: ");
			IO::BTUtil::GetAddrText(&sb, btDev->GetAddress());
			console.WriteLine(sb.ToString());
			sb.ClearStr();
			sb.Append((const UTF8Char*)"Class of Device: 0x");
			sb.AppendHex32(btDev->GetDevClass());
			console.WriteLine(sb.ToString());
			sb.ClearStr();
			sb.Append((const UTF8Char*)"Is Connected: ");
			sb.Append(btDev->IsConnected()?(const UTF8Char*)"TRUE":(const UTF8Char*)"FALSE");
			console.WriteLine(sb.ToString());
			sb.ClearStr();
			sb.Append((const UTF8Char*)"Is Remembered: ");
			sb.Append(btDev->IsRemembered()?(const UTF8Char*)"TRUE":(const UTF8Char*)"FALSE");
			console.WriteLine(sb.ToString());
			sb.ClearStr();
			sb.Append((const UTF8Char*)"Is Authenticated: ");
			sb.Append(btDev->IsAuthenticated()?(const UTF8Char*)"TRUE":(const UTF8Char*)"FALSE");
			console.WriteLine(sb.ToString());
			sb.ClearStr();
			sb.Append((const UTF8Char*)"Last Seen: ");
			btDev->GetLastSeen(&dt);
			dt.ToLocalTime();
			sb.AppendDate(&dt);
			console.WriteLine(sb.ToString());
			sb.ClearStr();
			sb.Append((const UTF8Char*)"Last Used: ");
			btDev->GetLastUsed(&dt);
			dt.ToLocalTime();
			sb.AppendDate(&dt);
			console.WriteLine(sb.ToString());

			DEL_CLASS(btDev);
			k++;
		}
		btDevList.Clear();

		DEL_CLASS(btCtrl);
		j++;
	}

	return 0;
}
