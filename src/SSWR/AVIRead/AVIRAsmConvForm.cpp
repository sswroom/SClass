#include "Stdafx.h"
#include "Data/ArrayListICaseStrUTF8.h"
#include "SSWR/AVIRead/AVIRAsmConvForm.h"
#include "Text/MyString.h"
#include "Win32/Clipboard.h"

void __stdcall SSWR::AVIRead::AVIRAsmConvForm::OnConvClicked(void *userObj)
{
	SSWR::AVIRead::AVIRAsmConvForm *me = (SSWR::AVIRead::AVIRAsmConvForm*)userObj;
	me->ConvAsm();
}

void __stdcall SSWR::AVIRead::AVIRAsmConvForm::OnConv2Clicked(void *userObj)
{
	SSWR::AVIRead::AVIRAsmConvForm *me = (SSWR::AVIRead::AVIRAsmConvForm*)userObj;
	Text::StringBuilderUTF8 sb;
	Win32::Clipboard clipboard(me->GetHandle());
	if (clipboard.GetString(me->GetHandle(), &sb))
	{
		me->txtIntelAsm->SetText(sb.ToString());
	}
	else
	{
		return;
	}
	me->ConvAsm();
	sb.ClearStr();
	me->txtGNUAsm->GetText(&sb);
	clipboard.SetString(me->GetHandle(), sb.ToString());
}

void SSWR::AVIRead::AVIRAsmConvForm::ConvAsm()
{
	Text::StringBuilderUTF8 srcSb;
	Text::StringBuilderUTF8 destSb;
	UTF8Char *sarr2[4];
	UTF8Char *sarr[5];
	UOSInt sarrCnt;
	UOSInt i;
	UOSInt j;
	UOSInt k;
	UOSInt l;
	OSInt si;
	OSInt sj;
	UOSInt thisTabCnt = 0;
	UOSInt allTabCnt = 0;
	UTF8Char *lineStart;
	UTF8Char c;
	UTF8Char c2;
	UTF8Char endC;
	Data::ArrayListICaseStrUTF8 regKey;
	Data::ArrayList<OSInt> regSize;
	Bool found;

	regSize.Insert(regKey.SortedInsert((const UTF8Char*)"eax"), 4);
	regSize.Insert(regKey.SortedInsert((const UTF8Char*)"ebx"), 4);
	regSize.Insert(regKey.SortedInsert((const UTF8Char*)"ecx"), 4);
	regSize.Insert(regKey.SortedInsert((const UTF8Char*)"edx"), 4);
	regSize.Insert(regKey.SortedInsert((const UTF8Char*)"esi"), 4);
	regSize.Insert(regKey.SortedInsert((const UTF8Char*)"edi"), 4);
	regSize.Insert(regKey.SortedInsert((const UTF8Char*)"esp"), 4);
	regSize.Insert(regKey.SortedInsert((const UTF8Char*)"ebp"), 4);
	regSize.Insert(regKey.SortedInsert((const UTF8Char*)"al"), 1);
	regSize.Insert(regKey.SortedInsert((const UTF8Char*)"bl"), 1);
	regSize.Insert(regKey.SortedInsert((const UTF8Char*)"cl"), 1);
	regSize.Insert(regKey.SortedInsert((const UTF8Char*)"dl"), 1);
	regSize.Insert(regKey.SortedInsert((const UTF8Char*)"ah"), 1);
	regSize.Insert(regKey.SortedInsert((const UTF8Char*)"bh"), 1);
	regSize.Insert(regKey.SortedInsert((const UTF8Char*)"ch"), 1);
	regSize.Insert(regKey.SortedInsert((const UTF8Char*)"dh"), 1);
	regSize.Insert(regKey.SortedInsert((const UTF8Char*)"ax"), 2);
	regSize.Insert(regKey.SortedInsert((const UTF8Char*)"bx"), 2);
	regSize.Insert(regKey.SortedInsert((const UTF8Char*)"cx"), 2);
	regSize.Insert(regKey.SortedInsert((const UTF8Char*)"dx"), 2);
	regSize.Insert(regKey.SortedInsert((const UTF8Char*)"si"), 2);
	regSize.Insert(regKey.SortedInsert((const UTF8Char*)"di"), 2);
	regSize.Insert(regKey.SortedInsert((const UTF8Char*)"sp"), 2);
	regSize.Insert(regKey.SortedInsert((const UTF8Char*)"bp"), 2);
	regSize.Insert(regKey.SortedInsert((const UTF8Char*)"rax"), 8);
	regSize.Insert(regKey.SortedInsert((const UTF8Char*)"rbx"), 8);
	regSize.Insert(regKey.SortedInsert((const UTF8Char*)"rcx"), 8);
	regSize.Insert(regKey.SortedInsert((const UTF8Char*)"rdx"), 8);
	regSize.Insert(regKey.SortedInsert((const UTF8Char*)"rsi"), 8);
	regSize.Insert(regKey.SortedInsert((const UTF8Char*)"rdi"), 8);
	regSize.Insert(regKey.SortedInsert((const UTF8Char*)"rsp"), 8);
	regSize.Insert(regKey.SortedInsert((const UTF8Char*)"rbp"), 8);
	regSize.Insert(regKey.SortedInsert((const UTF8Char*)"xmm0"), 16);
	regSize.Insert(regKey.SortedInsert((const UTF8Char*)"xmm1"), 16);
	regSize.Insert(regKey.SortedInsert((const UTF8Char*)"xmm2"), 16);
	regSize.Insert(regKey.SortedInsert((const UTF8Char*)"xmm3"), 16);
	regSize.Insert(regKey.SortedInsert((const UTF8Char*)"xmm4"), 16);
	regSize.Insert(regKey.SortedInsert((const UTF8Char*)"xmm5"), 16);
	regSize.Insert(regKey.SortedInsert((const UTF8Char*)"xmm6"), 16);
	regSize.Insert(regKey.SortedInsert((const UTF8Char*)"xmm7"), 16);

	this->txtIntelAsm->GetText(&srcSb);
	lineStart = srcSb.ToString();


	while (true)
	{
		i = 0;
		while (true)
		{
			c = lineStart[i];
			if (c == '\r' || c == '\n' || c == 0)
			{
				endC = c;
				break;
			}
			else
			{
				i++;
			}
		}

		thisTabCnt = 0;
		lineStart[i] = 0;
		while (true)
		{
			c = *lineStart;
			if (c == ' ')
			{
				lineStart++;
				i--;
			}
			else if (c == '\t')
			{
				thisTabCnt++;
				lineStart++;
				i--;
			}
			else
				break;
		}
		if (c == 0)
		{
			if (endC == 0)
				break;
			destSb.Append((const UTF8Char*)"\r\n");
			lineStart += i + 1;
			if (endC == '\r')
			{
				if (*lineStart == '\n')
				{
					lineStart++;
				}
			}
			else if (endC == '\n')
			{
				if (*lineStart == '\r')
				{
					lineStart++;
				}
			}
			continue;
		}

		j = 0;
		while (true)
		{
			c = lineStart[j];
			if (c == ' ' || c == '\t' || c == 0)
			{
				break;
			}
			j++;
		}
		c2 = lineStart[j - 1];
		if (c2 == ':')
		{
			lineStart[j] = 0;
			destSb.AppendChar('\t', allTabCnt);
			destSb.Append((const UTF8Char*)"\"");
			destSb.Append(lineStart);
			destSb.Append((const UTF8Char*)"\\n\"\r\n");
		}
		else
		{
			Bool skipLine = false;
			lineStart[j] = 0;
			Text::StrToLower(lineStart, lineStart);
			if (Text::StrEquals(lineStart, (const UTF8Char*)"align") && c != 0)
			{
				destSb.AppendChar('\t', allTabCnt);
				destSb.Append((const UTF8Char*)"\"\t");
				destSb.Append((const UTF8Char*)".balign ");
				destSb.Append(&lineStart[j + 1]);
			}
			else if (c == 0)
			{
				if (Text::StrCompareICase(lineStart, (const UTF8Char*)"_asm") == 0)
				{
					allTabCnt = thisTabCnt;
					destSb.AppendChar('\t', allTabCnt);
					destSb.Append((const UTF8Char*)"asm(");
				}
				else if (Text::StrCompareICase(lineStart, (const UTF8Char*)"stosd") == 0)
				{
					destSb.AppendChar('\t', allTabCnt);
					destSb.Append((const UTF8Char*)"\"\t");
					destSb.Append((const UTF8Char*)"stosl");
				}
				else if (Text::StrEquals(lineStart, (const UTF8Char*)"{") == 0)
				{
					skipLine = true;
				}
				else if (Text::StrEquals(lineStart, (const UTF8Char*)"}") == 0)
				{
					destSb.AppendChar('\t', allTabCnt);
					destSb.Append((const UTF8Char*)");\r\n");
					skipLine = true;
				}
				else
				{
					destSb.AppendChar('\t', allTabCnt);
					destSb.Append((const UTF8Char*)"\"\t");
					destSb.Append(lineStart);
				}
			}
			else
			{
				destSb.AppendChar('\t', allTabCnt);
				destSb.Append((const UTF8Char*)"\"\t");
				destSb.Append(lineStart);
				sarrCnt = Text::StrSplitTrim(sarr, 5, &lineStart[j + 1], ',');
				if (sarrCnt == 1 && sarr[0][0] == 0)
				{
				}
				else
				{
					j = sarrCnt;
					while (j-- > 0)
					{
						si = Text::StrIndexOfICase(sarr[j], (const UTF8Char*)" ptr ");
						if (si >= 0)
						{
							sarr[j][si] = 0;
							if (Text::StrStartsWithICase(sarr[j], (const UTF8Char*)"BYTE"))
							{
								destSb.Append((const UTF8Char*)"b");
							}
							else if (Text::StrStartsWithICase(sarr[j], (const UTF8Char*)"WORD"))
							{
								destSb.Append((const UTF8Char*)"w");
							}
							else if (Text::StrStartsWithICase(sarr[j], (const UTF8Char*)"DWORD"))
							{
								destSb.Append((const UTF8Char*)"l");
							}
							else if (Text::StrStartsWithICase(sarr[j], (const UTF8Char*)"QWORD") && lineStart[0] != 'f' && lineStart[0] != 'F')
							{
								destSb.Append((const UTF8Char*)"q");
							}
							sarr[j] = &sarr[j][k + 5];
							Text::StrTrim(sarr[j]);
						}
					}
					destSb.Append((const UTF8Char*)" ");
					j = sarrCnt;
					while (j-- > 0)
					{
						if (sarr[j][0] >= 0x30 && sarr[j][0] <= 0x39)
						{
							destSb.Append((const UTF8Char*)"$");
							destSb.Append(sarr[j]);
						}
						else if (sarr[j][0] == '\'')
						{
							destSb.Append(sarr[j]);
						}
						else if (regKey.SortedIndexOf(sarr[j]) >= 0)
						{
							destSb.Append((const UTF8Char*)"%");
							destSb.Append(sarr[j]);
						}
						else
						{
							si = Text::StrIndexOf(sarr[j], '[');
							if (si >= 0)
							{
								sj = Text::StrIndexOf(&sarr[j][si+1], ']');
								if (sj >= 0)
								{
									sarr[j][si+sj+1] = 0;

									found = false;
									sj = Text::StrIndexOf(&sarr[j][si+1], '-');
									if (sj == 0)
									{
										sarr[j][si+1] = ' ';
										found = true;
									}
									else if (sj > 0)
									{
										sarr[j][si+sj+1] = '+';
										found = true;
									}

									if (si > 0)
									{
										sarr[j][si] = 0;
										destSb.Append(sarr[si]);
										destSb.Append((const UTF8Char*)"(");
										if (found)
										{
											destSb.Append((const UTF8Char*)"-");
										}
										found = false;
										k = Text::StrSplitTrim(sarr2, 4, &sarr[j][si+1], '+');
										l = k;
										while (l-- > 0)
										{
											if (sarr2[l] && sarr2[l][0] >= 0x30 && sarr2[l][0] <= 0x39)
											{
												if (found)
												{
													destSb.Append((const UTF8Char*)",");
												}
												destSb.Append(sarr2[l]);
												found = true;
												sarr2[l] = 0;
											}
										}
									}
									else
									{
										if (found)
										{
											destSb.Append((const UTF8Char*)"-");
										}
										k = Text::StrSplitTrim(sarr2, 4, &sarr[j][k+1], '+');
										l = k;
										while (l-- > 0)
										{
											if (sarr2[l] && sarr2[l][0] >= 0x30 && sarr2[l][0] <= 0x39)
											{
												destSb.Append(sarr2[l]);
												sarr2[l] = 0;
											}
										}
										destSb.Append((const UTF8Char*)"(");
										found = false;
									}

									l = 0;
									while (l < k)
									{
										if (sarr2[l] && Text::StrIndexOf(sarr2[l], '*') < 0)
										{
											if (found)
											{
												destSb.Append((const UTF8Char*)",");
											}
											destSb.Append((const UTF8Char*)"%");
											destSb.Append(sarr2[l]);
											found = true;
											sarr2[l] = 0;
										}
										l++;
									}
									l = k;
									while (l-- > 0)
									{
										if (sarr2[l])
										{
											destSb.Append((const UTF8Char*)",");
											si = Text::StrIndexOf(sarr2[l], '*');
											sarr2[l][si] = 0;
											Text::StrTrim(sarr2[l]);
											destSb.Append((const UTF8Char*)"%");
											destSb.Append(sarr2[l]);
											Text::StrTrim(&sarr2[l][k+1]);
											destSb.Append((const UTF8Char*)",");
											destSb.Append(&sarr2[l][k+1]);
											found = true;
											sarr2[l] = 0;
										}
									}
									destSb.Append((const UTF8Char*)")");
								}
								else
								{
									sarr[j][k] = '[';
									destSb.Append(sarr[j]);
								}
							}
							else
							{
								destSb.Append(sarr[j]);
							}
						}
						if (j != 0)
						{
							destSb.Append((const UTF8Char*)",");
						}
					}
				}
			}
			if (!skipLine)
			{
				destSb.Append((const UTF8Char*)"\\n\"\r\n");
			}
		}
		lineStart += i + 1;
		if (endC == 0)
			break;
		else if (endC == '\r')
		{
			if (*lineStart == '\n')
			{
				lineStart++;
			}
		}
		else if (endC == '\n')
		{
			if (*lineStart == '\r')
			{
				lineStart++;
			}
		}
	}
	this->txtGNUAsm->SetText(destSb.ToString());
}

SSWR::AVIRead::AVIRAsmConvForm::AVIRAsmConvForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetText((const UTF8Char*)"Assembly Converter");
	this->SetFont(0, 8.25, false);

	this->core = core;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	NEW_CLASS(this->pnlCtrl, UI::GUIPanel(ui, this));
	this->pnlCtrl->SetRect(0, 0, 100, 31, false);
	this->pnlCtrl->SetDockType(UI::GUIControl::DOCK_TOP);
	NEW_CLASS(this->btnConv, UI::GUIButton(ui, this->pnlCtrl, (const UTF8Char*)"Convert"));
	this->btnConv->SetRect(4, 4, 75, 23, false);
	this->btnConv->HandleButtonClick(OnConvClicked, this);
	NEW_CLASS(this->btnConv2, UI::GUIButton(ui, this->pnlCtrl, (const UTF8Char*)"Paste-Conv-Copy"));
	this->btnConv2->SetRect(84, 4, 150, 23, false);
	this->btnConv2->HandleButtonClick(OnConv2Clicked, this);
	NEW_CLASS(this->txtIntelAsm, UI::GUITextBox(ui, this, (const UTF8Char*)"", true));
	this->txtIntelAsm->SetRect(0, 0, 512, 100, false);
	this->txtIntelAsm->SetDockType(UI::GUIControl::DOCK_LEFT);
	NEW_CLASS(this->hspAsm, UI::GUIHSplitter(ui, this, 3, false));
	NEW_CLASS(this->txtGNUAsm, UI::GUITextBox(ui, this, (const UTF8Char*)"", true));
	this->txtGNUAsm->SetDockType(UI::GUIControl::DOCK_FILL);
	this->txtGNUAsm->SetReadOnly(true);
}

SSWR::AVIRead::AVIRAsmConvForm::~AVIRAsmConvForm()
{
}

void SSWR::AVIRead::AVIRAsmConvForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
