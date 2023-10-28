#include "Stdafx.h"
#include "Core/Core.h"
#include "IO/BTManager.h"
#include "IO/BTUtil.h"
#include "IO/ConsoleWriter.h"
#include "Text/StringBuilderUTF8.h"

Int32 MyMain(NotNullPtr<Core::IProgControl> progCtrl)
{
	UOSInt i;
	UOSInt j;
	UOSInt k;
	UOSInt l;
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
	sb.AppendUOSInt(i);
	sb.AppendC(UTF8STRC(" controller found:"));
	j = 0;
	console.WriteLineC(sb.ToString(), sb.GetLength());
	while (i-- > 0)
	{
		btCtrl = btCtrlList.GetItem(i);
		console.WriteLine();
		sb.ClearStr();
		sb.AppendC(UTF8STRC("Controller "));
		sb.AppendUOSInt(j);
		sb.AppendC(UTF8STRC(":"));
		console.WriteLineC(sb.ToString(), sb.GetLength());
		sb.ClearStr();
		sb.AppendC(UTF8STRC("Name: "));
		sb.Append(btCtrl->GetName());
		console.WriteLineC(sb.ToString(), sb.GetLength());
		sb.ClearStr();
		sb.AppendC(UTF8STRC("Address: "));
		IO::BTUtil::GetAddrText(sb, btCtrl->GetAddress());
		console.WriteLineC(sb.ToString(), sb.GetLength());
		sb.ClearStr();
		sb.AppendC(UTF8STRC("Manufacturer: "));
		sb.Append(IO::BTUtil::GetManufacturerName(btCtrl->GetManufacturer()));
		sb.AppendC(UTF8STRC(" (0x"));
		sb.AppendHex16(btCtrl->GetManufacturer());
		sb.AppendC(UTF8STRC(")"));
		console.WriteLineC(sb.ToString(), sb.GetLength());
		sb.ClearStr();
		sb.AppendC(UTF8STRC("Subversion: 0x"));
		sb.AppendHex16(btCtrl->GetSubversion());
		console.WriteLineC(sb.ToString(), sb.GetLength());
		sb.ClearStr();
		sb.AppendC(UTF8STRC("Class of Device: 0x"));
		sb.AppendHex32(btCtrl->GetDevClass());
		console.WriteLineC(sb.ToString(), sb.GetLength());

		console.WriteLine();
		sb.ClearStr();
		sb.AppendC(UTF8STRC("Controller "));
		sb.AppendUOSInt(j);
		sb.AppendC(UTF8STRC(" - Devices:"));
		console.WriteLineC(sb.ToString(), sb.GetLength());

		btCtrl->CreateDevices(&btDevList, false);
		k = 0;
		l = btDevList.GetCount();
		while (k < l)
		{
			btDev = btDevList.GetItem(k);
			sb.ClearStr();
			sb.AppendC(UTF8STRC("Controller "));
			sb.AppendUOSInt(j);
			sb.AppendC(UTF8STRC(" Device "));
			sb.AppendUOSInt(k);
			console.WriteLineC(sb.ToString(), sb.GetLength());
			sb.ClearStr();
			sb.AppendC(UTF8STRC("Name: "));
			sb.Append(btDev->GetName());
			console.WriteLineC(sb.ToString(), sb.GetLength());
			sb.ClearStr();
			sb.AppendC(UTF8STRC("Address: "));
			IO::BTUtil::GetAddrText(sb, btDev->GetAddress());
			console.WriteLineC(sb.ToString(), sb.GetLength());
			sb.ClearStr();
			sb.AppendC(UTF8STRC("Class of Device: 0x"));
			sb.AppendHex32(btDev->GetDevClass());
			console.WriteLineC(sb.ToString(), sb.GetLength());
			sb.ClearStr();
			sb.AppendC(UTF8STRC("Is Connected: "));
			if (btDev->IsConnected())
			{
				sb.AppendC(UTF8STRC("TRUE"));
			}
			else
			{
				sb.AppendC(UTF8STRC("FALSE"));
			}
			console.WriteLineC(sb.ToString(), sb.GetLength());
			sb.ClearStr();
			sb.AppendC(UTF8STRC("Is Remembered: "));
			if (btDev->IsRemembered())
			{
				sb.AppendC(UTF8STRC("TRUE"));
			}
			else
			{
				sb.AppendC(UTF8STRC("FALSE"));
			}
			console.WriteLineC(sb.ToString(), sb.GetLength());
			sb.ClearStr();
			sb.AppendC(UTF8STRC("Is Authenticated: "));
			if (btDev->IsAuthenticated())
			{
				sb.AppendC(UTF8STRC("TRUE"));
			}
			else
			{
				sb.AppendC(UTF8STRC("FALSE"));
			}
			console.WriteLineC(sb.ToString(), sb.GetLength());
			sb.ClearStr();
			sb.AppendC(UTF8STRC("Last Seen: "));
			btDev->GetLastSeen(&dt);
			dt.ToLocalTime();
			sb.AppendDateTime(dt);
			console.WriteLineC(sb.ToString(), sb.GetLength());
			sb.ClearStr();
			sb.AppendC(UTF8STRC("Last Used: "));
			btDev->GetLastUsed(&dt);
			dt.ToLocalTime();
			sb.AppendDateTime(dt);
			console.WriteLineC(sb.ToString(), sb.GetLength());

			DEL_CLASS(btDev);
			k++;
		}
		btDevList.Clear();

		DEL_CLASS(btCtrl);
		j++;
	}

	return 0;
}
