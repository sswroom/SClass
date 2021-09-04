#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/FileStream.h"
#include "IO/Path.h"
#include "Net/ASN1MIB.h"
#include "Net/ASN1Util.h"
#include "Net/MIBReader.h"
#include "Text/CharUtil.h"
#include "Text/StringBuilderUTF8.h"
#define DEBUGOBJ "id-dsa"

UOSInt Net::ASN1MIB::CalcLineSpace(const UTF8Char *txt)
{
	UOSInt ret = 0;
	UTF8Char c;
	while ((c = *txt++))
	{
		if (c == ' ')
		{
			ret++;
		}
		else if (c == '\t')
		{
			ret += 4;
		}
		else
		{
			return ret;
		}
	}
	return ret;
}

void Net::ASN1MIB::ModuleAppendOID(Net::ASN1MIB::ModuleInfo *module, ObjectInfo *obj)
{
	OSInt i;
	OSInt j;
	OSInt k;
	OSInt l;
	ObjectInfo *obj2;
	if (obj->oidLen <= 0)
		return;
	i = 0;
	j = (OSInt)module->oidList->GetCount() - 1;
	while (i <= j)
	{
		k = (i + j) >> 1;
		obj2 = module->oidList->GetItem((UOSInt)k);
		l = Net::ASN1Util::OIDCompare(obj2->oid, obj2->oidLen, obj->oid, obj->oidLen);
		if (l > 0)
		{
			j = k - 1;
		}
		else if (l < 0)
		{
			i = k + 1;
		}
		else
		{
			return;
		}
	}
	module->oidList->Insert((UOSInt)i, obj);
}

Bool Net::ASN1MIB::ParseObjectOID(ModuleInfo *module, ObjectInfo *obj, const UTF8Char *s, Text::StringBuilderUTF *errMessage)
{
	const UTF8Char *oriS = s;
	UTF8Char c;
	const UTF8Char *oidName;
	UOSInt oidNameLen;
	const UTF8Char *oidNextLev;
	UOSInt oidNextLen;
	Bool isFirst = false;
	OSInt i;
	Text::StringBuilderUTF8 sb;
	while (true)
	{
		c = *s++;
		if (c == ' ')
		{

		}
		else if (c == '{')
		{
			break;
		}
		else if ((i = module->objKeys->SortedIndexOf(s - 1)) >= 0)
		{
			ObjectInfo *refObj = module->objValues->GetItem((UOSInt)i);
			if (!refObj->parsed)
			{
				if (!ApplyModuleOID(module, refObj, errMessage))
				{
					return false;
				}
			}
			s = refObj->typeVal;
		}
		else
		{
			errMessage->Append(obj->objectName);
			errMessage->Append((const UTF8Char*)": OID Format error 1: \"");
			errMessage->Append(oriS);
			errMessage->Append((const UTF8Char*)"\"");
			return false;
		}
	}
	while (true)
	{
		c = *s++;
		if (c == ' ')
		{

		}
		else if (c == '}' || c == 0)
		{
			errMessage->Append(obj->objectName);
			errMessage->Append((const UTF8Char*)": OID Format error 2: \"");
			errMessage->Append(oriS);
			errMessage->Append((const UTF8Char*)"\"");
			return false;
		}
		else
		{
			oidName = s - 1;
			break;
		}
	}
	while (true)
	{
		c = *s++;
		if (c == ' ')
		{
			oidNameLen = (UOSInt)(s - oidName - 1);
			break;
		}
		else if (c == '}' || c == 0)
		{
			errMessage->Append(obj->objectName);
			errMessage->Append((const UTF8Char*)": OID Format error 3: \"");
			errMessage->Append(oriS);
			errMessage->Append((const UTF8Char*)"\"");
			return false;
		}
	}
	sb.ClearStr();
	sb.AppendC(oidName, oidNameLen);
	if (sb.Equals((const UTF8Char*)"iso"))
	{
		obj->oid[0] = 40;
		obj->oidLen = 1;
		isFirst = true;
	}
	else if (sb.Equals((const UTF8Char*)"iso(1)"))
	{
		obj->oid[0] = 40;
		obj->oidLen = 1;
		isFirst = true;
	}
	else if (sb.Equals((const UTF8Char*)"joint-iso-ccitt(2)") || sb.Equals((const UTF8Char*)"joint-iso-itu-t(2)") || sb.Equals((const UTF8Char*)"joint-iso-itu-t") || sb.Equals((const UTF8Char*)"2"))
	{
		obj->oid[0] = 80;
		obj->oidLen = 1;
		isFirst = true;
	}
	else if (sb.Equals((const UTF8Char*)"0"))
	{
		obj->oid[0] = 0;
		obj->oidLen = 1;
		isFirst = true;
	}
	else
	{
		OSInt i = module->objKeys->SortedIndexOf(sb.ToString());
		ObjectInfo *obj2;
		if (i < 0)
		{
//			i = this->globalModule.objKeys->SortedIndexOf(sb.ToString());
//			if (i < 0)
//			{
				errMessage->Append(obj->objectName);
				errMessage->Append((const UTF8Char*)": OID Name \"");
				errMessage->Append(sb.ToString());
				errMessage->Append((const UTF8Char*)"\" not found");
				return false;
//			}
//			else
//			{
//				obj2 = this->globalModule.objValues->GetItem(i);
//			}
		}
		else
		{
			obj2 = module->objValues->GetItem((UOSInt)i);
		}
		if (!obj2->parsed)
		{
			if (!ApplyModuleOID(module, obj2, errMessage))
			{
				return false;
			}
		}
		if (obj2->oidLen == 0)
		{
			if (obj2->typeVal != 0)
			{
				if (!ParseObjectOID(module, obj2, obj2->typeVal, errMessage))
				{
					return false;
				}
			}
		}
		if (obj2->oidLen == 0)
		{
			errMessage->Append(obj->objectName);
			errMessage->Append((const UTF8Char*)": OID Name \"");
			errMessage->Append(sb.ToString());
			errMessage->Append((const UTF8Char*)"\" is not OID");
			return false;
		}
		MemCopyNO(obj->oid, obj2->oid, obj2->oidLen);
		obj->oidLen = obj2->oidLen;
	}
	
	while (true)
	{
		while (true)
		{
			c = *s++;
			if (c == ' ')
			{

			}
			else if (c == '}')
			{
				while (true)
				{
					c = *s++;
					if (c == ' ')
					{
					}
					else if (c == 0)
					{
						return true;
					}
					else
					{
						errMessage->Append(obj->objectName);
						errMessage->Append((const UTF8Char*)": OID Format error 4: \"");
						errMessage->Append(oriS);
						errMessage->Append((const UTF8Char*)"\"");
						return false;
					}
				}
			}
			else
			{
				oidNextLev = s - 1;
				break;
			}
		}
		while (true)
		{
			c = *s++;
			if (c == ' ' || c == '}')
			{
				i = 0;
				while (s[i] == ' ')
				{
					i++;
				}
				if (s[i] != '(')
				{
					oidNextLen = (UOSInt)(s - oidNextLev - 1);
					if (c == '}')
					{
						s--;
					}
					break;
				}
			}
		}

		UInt32 v;
		UOSInt i;
		UOSInt j;
		sb.ClearStr();
		sb.AppendC(oidNextLev, oidNextLen);
		i = sb.IndexOf('(');
		j = sb.IndexOf(')');
		if (i != INVALID_INDEX && j != INVALID_INDEX && j > i)
		{
			sb.SetSubstr(i + 1);
			sb.TrimToLength(j - i - 1);
			if (!sb.ToUInt32(&v))
			{
				errMessage->Append(obj->objectName);
				errMessage->Append((const UTF8Char*)": OID Format error 5: \"");
				errMessage->Append(oriS);
				errMessage->Append((const UTF8Char*)"\"");
				return false;
			}
		}
		else if (sb.Equals((const UTF8Char*)"standard"))
		{
			v = 0;
		}
		else
		{
			if (!sb.ToUInt32(&v))
			{
				errMessage->Append(obj->objectName);
				errMessage->Append((const UTF8Char*)": OID Format error 6: \"");
				errMessage->Append(oriS);
				errMessage->Append((const UTF8Char*)"\"");
				return false;
			}
		}
		
		if (v < 128)
		{
			if (isFirst)
			{
				obj->oid[0] = (UInt8)(obj->oid[0] + v);
				isFirst = false;
			}
			else
			{
				obj->oid[obj->oidLen] = (UInt8)v;
				obj->oidLen = obj->oidLen + 1;
			}
			
		}
		else if (v < 0x4000)
		{
			obj->oid[obj->oidLen] = (UInt8)(0x80 | (v >> 7));
			obj->oid[obj->oidLen + 1] = (UInt8)(v & 0x7f);
			obj->oidLen = obj->oidLen + 2;
		}
		else if (v < 0x200000)
		{
			obj->oid[obj->oidLen] = (UInt8)(0x80 | (v >> 14));
			obj->oid[obj->oidLen + 1] = (UInt8)(0x80 | ((v >> 7) & 0x7f));
			obj->oid[obj->oidLen + 2] = (UInt8)(v & 0x7f);
			obj->oidLen = obj->oidLen + 3;
		}
		else if (v < 0x10000000)
		{
			obj->oid[obj->oidLen] = (UInt8)(0x80 | (v >> 21));
			obj->oid[obj->oidLen + 1] = (UInt8)(0x80 | ((v >> 14) & 0x7f));
			obj->oid[obj->oidLen + 2] = (UInt8)(0x80 | ((v >> 7) & 0x7f));
			obj->oid[obj->oidLen + 3] = (UInt8)(v & 0x7f);
			obj->oidLen = obj->oidLen + 4;
		}
		else
		{
			obj->oid[obj->oidLen] = (UInt8)(0x80 | (v >> 28));
			obj->oid[obj->oidLen + 1] = (UInt8)(0x80 | ((v >> 21) & 0x7f));
			obj->oid[obj->oidLen + 2] = (UInt8)(0x80 | ((v >> 14) & 0x7f));
			obj->oid[obj->oidLen + 3] = (UInt8)(0x80 | ((v >> 7) & 0x7f));
			obj->oid[obj->oidLen + 4] = (UInt8)(v & 0x7f);
			obj->oidLen = obj->oidLen + 5;
		}
	}

	return true;
}

Bool Net::ASN1MIB::ParseObjectBegin(Net::MIBReader *reader, ObjectInfo *obj, Text::StringBuilderUTF *errMessage)
{
	Text::StringBuilderUTF8 sb;
	while (true)
	{
		sb.ClearStr();
		if (!reader->ReadLine(&sb))
		{
			errMessage->Append((const UTF8Char*)"Object end not found");
			return false;
		}
		if (sb.GetLength() > 0)
		{
			if (sb.EndsWith((const UTF8Char*)"BEGIN"))
			{
				errMessage->Append((const UTF8Char*)"Nested begin found");
				return false;
			}
			else if (sb.EndsWith((const UTF8Char*)"END"))
			{
				return true;
			}
		}
	}
}

Bool Net::ASN1MIB::ParseModule(Net::MIBReader *reader, ModuleInfo *module, Text::StringBuilderUTF *errMessage)
{
	Text::StringBuilderUTF8 sb;
	UOSInt i;
	UOSInt j;
	UOSInt lineSpace;
	ObjectInfo *obj;
	ObjectInfo *currObj = 0;
	Text::StringBuilderUTF8 sbObjValName;
	Text::StringBuilderUTF8 sbObjValCont;
	UOSInt objLineSpace = 0;
	Bool objIsEqual = false;
	UTF8Char objBrkType = 0;
	Bool succ;
	Bool isQuotedText = false;
	
	while (true)
	{
		sb.ClearStr();
		if (!reader->ReadLine(&sb))
		{
			errMessage->Append((const UTF8Char*)"Module end not found");
			return false;
		}

		if (currObj && currObj->objectName && Text::StrEquals(currObj->objectName, (const UTF8Char*)"id-sha1"))
		{
			i = 0;
		}

		if (sb.GetLength() > 0)
		{
			if (sb.EndsWith((const UTF8Char*)"BEGIN"))
			{
				succ = ParseObjectBegin(reader, 0, errMessage);
				if (!succ)
				{
					return succ;
				}
				currObj = 0;
			}
			else if (sb.EndsWith((const UTF8Char*)"END"))
			{
				return true;
			}
			else if (isQuotedText)
			{
				if (currObj)
				{
					sbObjValCont.Append(sb.ToString());
				}
				if (sb.EndsWith('"'))
				{
					isQuotedText = false;
					if (currObj)
					{
						if (sbObjValName.GetLength() > 0 && sbObjValCont.GetLength() > 0)
						{
							currObj->valName->Add(Text::StrCopyNew(sbObjValName.ToString()));
							currObj->valCont->Add(Text::StrCopyNew(sbObjValCont.ToString()));
						}
						sbObjValName.ClearStr();
						sbObjValCont.ClearStr();
					}
				}
				else
				{
					reader->GetLastLineBreak(&sbObjValCont);
				}
				
			}
			else
			{
				lineSpace = CalcLineSpace(sb.ToString());
				sb.Trim();
				if (currObj && (objBrkType != 0 || objIsEqual || (lineSpace > objLineSpace && sb.ToString()[0] >= 'A' && sb.ToString()[0] <= 'Z') || sb.StartsWith((const UTF8Char*)"::=") || sb.StartsWith((const UTF8Char*)"{") || sb.StartsWith((const UTF8Char*)"\"")))
				{
					if (objBrkType)
					{
						Char brkEndChar;
						if (objBrkType == '{')
						{
							brkEndChar = '}';
						}
						else if (objBrkType == '(')
						{
							brkEndChar = ')';
						}
						else
						{
							brkEndChar = '}';
						}

						if (objIsEqual)
						{
							Text::StringBuilderUTF8 sbTmp;
							sbTmp.Append(currObj->typeVal);
							sbTmp.AppendChar(' ', 1);
							sbTmp.Append(sb.ToString());
							Text::StrDelNew(currObj->typeVal);
							currObj->typeVal = Text::StrCopyNew(sbTmp.ToString());

							OSInt brkEndIndex = BranketEnd(currObj->typeVal, (UTF8Char*)&brkEndChar);
							if (brkEndIndex >= 0)
							{
								objBrkType = 0;
								objIsEqual = false;
								if (sbObjValName.GetLength() > 0 && sbObjValCont.GetLength() > 0)
								{
									currObj->valName->Add(Text::StrCopyNew(sbObjValName.ToString()));
									currObj->valCont->Add(Text::StrCopyNew(sbObjValCont.ToString()));
								}
								sbObjValName.ClearStr();
								sbObjValCont.ClearStr();
								if (Text::StrEquals(currObj->objectName, (const UTF8Char*)DEBUGOBJ))
								{
									sbObjValCont.ClearStr();
								}

								if (currObj->typeName == 0 || currObj->typeName[0] == '{')
								{
									const UTF8Char *sptr = SkipWS(&currObj->typeVal[brkEndIndex]);
									if (sptr[0] == '(')
									{
										OSInt nextEndIndex = BranketEnd(&currObj->typeVal[brkEndIndex], 0);
										while (nextEndIndex < 0)
										{
											sb.ClearStr();
											sb.Append(currObj->typeVal);
											sb.AppendChar(' ', 1);
											if (!reader->ReadLine(&sb))
											{
												errMessage->Append((const UTF8Char*)"Unexpected end of file: ");
												errMessage->Append(sb.ToString());
												return false;
											}
											Text::StrDelNew(currObj->typeVal);
											currObj->typeVal = Text::StrCopyNew(sb.ToString());
											nextEndIndex = BranketEnd(&currObj->typeVal[brkEndIndex], 0);
										}
										RemoveSpace((UTF8Char*)currObj->typeVal);
									}
									else if (Text::StrStartsWith(sptr, (const UTF8Char*)"WITH SYNTAX") || Text::StrStartsWith(sptr, (const UTF8Char*)"WITH COMPONENTS"))
									{
										if (Text::StrIndexOf(sptr, (const UTF8Char*)"{") != INVALID_INDEX)
										{
											OSInt nextEndIndex = BranketEnd(&currObj->typeVal[brkEndIndex], 0);
											while (nextEndIndex < 0)
											{
												sb.ClearStr();
												sb.Append(currObj->typeVal);
												sb.AppendChar(' ', 1);
												if (!reader->ReadLine(&sb))
												{
													errMessage->Append((const UTF8Char*)"Unexpected end of file: ");
													errMessage->Append(sb.ToString());
													return false;
												}
												Text::StrDelNew(currObj->typeVal);
												currObj->typeVal = Text::StrCopyNew(sb.ToString());
												nextEndIndex = BranketEnd(&currObj->typeVal[brkEndIndex], 0);
											}
											RemoveSpace((UTF8Char*)currObj->typeVal);
										}
										else
										{
											sb.ClearStr();
											sb.Append(currObj->typeVal);
											sb.AppendChar(' ', 1);
											if (!reader->ReadLine(&sb))
											{
												errMessage->Append((const UTF8Char*)"Unexpected end of file: ");
												errMessage->Append(sb.ToString());
												return false;
											}
											Text::StrDelNew(currObj->typeVal);
											currObj->typeVal = Text::StrCopyNew(sb.ToString());
										}
									}
									else
									{
										
										RemoveSpace((UTF8Char*)currObj->typeVal);
										sb.ClearStr();
										if (reader->PeekWord(&sb))
										{
											RemoveSpace(sb.ToString());
											if (sb.Equals((const UTF8Char*)"WITH"))
											{
												sb.ClearStr();
												reader->NextWord(&sb);
												sb.AppendChar(' ', 1);
												reader->NextWord(&sb);
												if (sb.Equals((const UTF8Char*)"WITH SYNTAX") || sb.Equals((const UTF8Char*)"WITH COMPONENTS"))
												{
													sb.AppendChar(' ', 1);
													if (!reader->NextWord(&sb))
													{
														errMessage->Append((const UTF8Char*)"WITH SYNTAX error: ");
														errMessage->Append(sb.ToString());
														return false;
													}
													Text::StringBuilderUTF8 sbTmp;
													sbTmp.Append(currObj->typeVal);
													sbTmp.AppendChar(' ', 1);
													sbTmp.Append(sb.ToString());
													Text::StrDelNew(currObj->typeVal);
													currObj->typeVal = Text::StrCopyNew(sbTmp.ToString());
													RemoveSpace((UTF8Char*)currObj->typeVal);
												}
												else
												{
													errMessage->Append((const UTF8Char*)"Unexpected word after WITH: ");
													errMessage->Append(sb.ToString());
													return false;
												}
											}
											else
											{
												while (sb.StartsWith((const UTF8Char*)"(WITH") ||
													sb.StartsWith((const UTF8Char*)"( WITH") ||
													sb.StartsWith((const UTF8Char*)"(CONSTRAINED") ||
													sb.StartsWith((const UTF8Char*)"(ALL") ||
													sb.StartsWith((const UTF8Char*)"( ALL"))
												{
													sb.ClearStr();
													sb.Append(currObj->typeVal);
													sb.AppendChar(' ', 1);
													reader->NextWord(&sb);
													Text::StrDelNew(currObj->typeVal);
													currObj->typeVal = Text::StrCopyNew(sb.ToString());
													RemoveSpace((UTF8Char*)currObj->typeVal);
													sb.ClearStr();
													reader->PeekWord(&sb);
													RemoveSpace(sb.ToString());
												}
											}
										}
									}
									currObj = 0;
								}
								else if (Text::StrEquals(currObj->typeName, (const UTF8Char*)"OBJECT IDENTIFIER"))
								{
									currObj = 0;
								}
							}
						}
						else
						{
							sbObjValCont.Append(sb.ToString());
						}
					}
					else if (objIsEqual)
					{
						if (sb.StartsWith((const UTF8Char*)"[") && sb.EndsWith((const UTF8Char*)"]"))
						{

						}
						else if (sb.Equals((const UTF8Char*)"OCTET") || sb.Equals((const UTF8Char*)"BIT"))
						{
							sb.AppendChar(' ', 1);
							reader->NextWord(&sb);
							if (sb.Equals((const UTF8Char*)"OCTET STRING") || sb.Equals((const UTF8Char*)"BIT STRING"))
							{
								Text::StringBuilderUTF8 sbTmp;
								reader->PeekWord(&sbTmp);
								if (sbTmp.StartsWith((const UTF8Char*)"{") || sbTmp.StartsWith((const UTF8Char*)"("))
								{
									sb.AppendChar(' ', 1);
									reader->NextWord(&sb);
									sbTmp.ClearStr();
									reader->PeekWord(&sbTmp);
								}
								currObj->typeVal = Text::StrCopyNew(sb.ToString());
								currObj = 0;
								objIsEqual = false;
							}
							else
							{
								errMessage->Append((const UTF8Char*)"Unexpected words: ");
								errMessage->Append(sb.ToString());
								return false;
							}
						}
						else
						{
							currObj->typeVal = Text::StrCopyNew(sb.ToString());
							i = Text::StrIndexOf(currObj->typeVal, '{');
							if (i != INVALID_INDEX)
							{
								if (Text::StrCountChar(currObj->typeVal, '{') <= Text::StrCountChar(currObj->typeVal, '}'))
								{
									currObj = 0;
									objIsEqual = false;
								}
								else
								{
									objBrkType = '{';
								}
							}
							else if (Text::StrEndsWith(currObj->typeVal, (const UTF8Char*)" OF"))
							{
								sb.ClearStr();
								sb.Append(currObj->typeVal);
								sb.AppendChar(' ', 1);
								if (!reader->NextWord(&sb))
								{
									errMessage->Append((const UTF8Char*)"Unexpected end of file after OF");
									return false;
								}

								Text::StringBuilderUTF8 sbTmp;
								reader->PeekWord(&sbTmp);
								if (sbTmp.StartsWith((const UTF8Char*)"{"))
								{
									reader->NextWord(&sb);
								}
								Text::StrDelNew(currObj->typeVal);
								currObj->typeVal = Text::StrCopyNew(sb.ToString());
								sb.ClearStr();
								currObj = 0;
							}
							else
							{
								sb.ClearStr();
								if (reader->PeekWord(&sb) && sb.StartsWith((const UTF8Char*)"{"))
								{
									Text::StringBuilderUTF8 sbTmp;
									sbTmp.Append(currObj->typeVal);
									sbTmp.AppendChar(' ', 1);
									reader->NextWord(&sbTmp);
									Text::StrDelNew(currObj->typeVal);
									currObj->typeVal = Text::StrCopyNew(sbTmp.ToString());
								}
								currObj = 0;
								objIsEqual = false;
							}
						}
					}
					else if (sb.StartsWith((const UTF8Char*)"::="))
					{
						if (sbObjValName.GetLength() > 0 && sbObjValCont.GetLength() > 0)
						{
							currObj->valName->Add(Text::StrCopyNew(sbObjValName.ToString()));
							currObj->valCont->Add(Text::StrCopyNew(sbObjValCont.ToString()));
						}
						sbObjValName.ClearStr();
						sbObjValCont.ClearStr();

						i = 3;
						while (sb.ToString()[i] == ' ' || sb.ToString()[i] == '\t')
						{
							i++;
						}
						currObj->typeVal = Text::StrCopyNew(sb.ToString() + i);
						UOSInt startCnt = Text::StrCountChar(currObj->typeVal, '{');
						UOSInt endCnt = Text::StrCountChar(currObj->typeVal, '}');
						if (endCnt >= startCnt)
						{
							currObj = 0;
							objBrkType = 0;
							objIsEqual = false;
						}							
						else
						{
							objBrkType = '{';
							objIsEqual = true;
						}
					}
					else if (sb.StartsWith((const UTF8Char*)"{"))
					{
						sbObjValCont.Append(sb.ToString());
						if (sb.EndsWith((const UTF8Char*)"}"))
						{
							objBrkType = 0;
							objIsEqual = false;
							if (sbObjValName.GetLength() > 0 && sbObjValCont.GetLength() > 0)
							{
								currObj->valName->Add(Text::StrCopyNew(sbObjValName.ToString()));
								currObj->valCont->Add(Text::StrCopyNew(sbObjValCont.ToString()));
							}
							sbObjValName.ClearStr();
							sbObjValCont.ClearStr();
						}
						else
						{
							objBrkType = '{';
							objIsEqual = false;
						}
					}
					else if (sb.StartsWith((const UTF8Char*)"\""))
					{
						sbObjValCont.Append(sb.ToString());
						if (sb.GetLength() > 1 && sb.EndsWith((const UTF8Char*)"\""))
						{
							if (sbObjValName.GetLength() > 0 && sbObjValCont.GetLength() > 0)
							{
								currObj->valName->Add(Text::StrCopyNew(sbObjValName.ToString()));
								currObj->valCont->Add(Text::StrCopyNew(sbObjValCont.ToString()));
							}
							sbObjValName.ClearStr();
							sbObjValCont.ClearStr();
						}
						else
						{
							isQuotedText = true;
						}
					}
					else
					{
						Bool proc = false;
						if (sbObjValName.GetLength() > 0 && sbObjValCont.GetLength() > 0)
						{
							if (sb.StartsWith((const UTF8Char*)"{") || sb.StartsWith((const UTF8Char*)"\""))
							{
								sbObjValCont.AppendChar(' ', 1);
								sbObjValCont.Append(sb.ToString());
								proc = true;
							}
							else
							{
								currObj->valName->Add(Text::StrCopyNew(sbObjValName.ToString()));
								currObj->valCont->Add(Text::StrCopyNew(sbObjValCont.ToString()));
								sbObjValName.ClearStr();
								sbObjValCont.ClearStr();
							}
						}
						else if (sbObjValName.GetLength() > 0)
						{
							sbObjValCont.Append(sb.ToString());
							proc = true;
						}

						if (!proc)
						{
							i = sb.IndexOf(' ');
							j = sb.IndexOf('\t');
							if (i == INVALID_INDEX)
							{
								i = j;
							}
							else if (j != INVALID_INDEX && j < i)
							{
								i = j;
							}
							
							if (i == INVALID_INDEX)
							{
								sbObjValName.Append(sb.ToString());
							}
							else
							{
								sbObjValName.AppendC(sb.ToString(), i);
								sbObjValCont.Append(sb.ToString() + i + 1);
								sbObjValCont.Trim();
							}
							
						}

						if ((i = sb.IndexOf('\"')) != INVALID_INDEX)
						{
							j = sb.IndexOf((const UTF8Char*)"\"", i + 1);
							if (j == INVALID_INDEX)
							{
								reader->GetLastLineBreak(&sbObjValCont);
								isQuotedText = true;
							}
						}
						else if ((i = sb.IndexOf((const UTF8Char*)"{")) != INVALID_INDEX)
						{
							j = sb.IndexOf((const UTF8Char*)"}");
							if (j != INVALID_INDEX && j > i)
							{
								if (sbObjValName.GetLength() > 0 && sbObjValCont.GetLength() > 0)
								{
									currObj->valName->Add(Text::StrCopyNew(sbObjValName.ToString()));
									currObj->valCont->Add(Text::StrCopyNew(sbObjValCont.ToString()));
								}
								sbObjValName.ClearStr();
								sbObjValCont.ClearStr();
							}
							else
							{
								objBrkType = '{';
								objIsEqual = false;
							}
						}
						else if ((i = sb.IndexOf((const UTF8Char*)"(")) != INVALID_INDEX)
						{
							j = sb.IndexOf((const UTF8Char*)")", (UOSInt)i);
							if (j != INVALID_INDEX && j > i)
							{
								if (sbObjValName.GetLength() > 0 && sbObjValCont.GetLength() > 0)
								{
									currObj->valName->Add(Text::StrCopyNew(sbObjValName.ToString()));
									currObj->valCont->Add(Text::StrCopyNew(sbObjValCont.ToString()));
								}
								sbObjValName.ClearStr();
								sbObjValCont.ClearStr();
							}
							else
							{
								objBrkType = '(';
								objIsEqual = false;
							}
						}
					}
				}
				else if (sb.StartsWith((const UTF8Char*)"IMPORTS"))
				{
					Bool isEnd = false;
					Text::StringBuilderUTF8 impObjNames;
					Net::ASN1MIB::ModuleInfo *impModule;
					Net::ASN1MIB::ObjectInfo *impObj;
					UTF8Char *impSarr[2];
					UOSInt impCnt;
					sb.SetSubstr(7);
					sb.Trim();

					while (!isEnd)
					{
						if (sb.EndsWith(';'))
						{
							isEnd = true;
							sb.RemoveChars(1);
						}
						i = sb.IndexOf((const UTF8Char*)"FROM ");
						if (i != INVALID_INDEX)
						{
							impObjNames.AppendC(sb.ToString(), (UOSInt)i);
							impObjNames.TrimRight();

							sb.SetSubstr((UOSInt)i + 5);
							sb.Trim();
							if ((i = sb.IndexOf('{')) != INVALID_INDEX)
							{
								while (true)
								{
									j = sb.IndexOf('}');
									if (j != INVALID_INDEX)
									{
										break;
									}
									if (!reader->ReadLine(&sb))
									{
										errMessage->Append((const UTF8Char*)"Import module error: ");
										errMessage->Append(sb.ToString());
										return false;
									}
								}
								if (sb.EndsWith(';'))
								{
									isEnd = true;
									sb.RemoveChars(1);
								}
								sb.TrimToLength((UOSInt)i);
								sb.Trim();
							}
							else if ((i = sb.IndexOf(' ')) != INVALID_INDEX)
							{
								if (sb.EndsWith(';'))
								{
									isEnd = true;
									sb.RemoveChars(1);
								}
								sb.TrimToLength((UOSInt)i);
							}

							if ((impModule = this->moduleMap->Get(sb.ToString())) != 0)
							{
								
							}
							else
							{
								UTF8Char sbuff[512];
								Text::StrConcat(sbuff, module->moduleFileName);
								j = Text::StrLastIndexOf(sbuff, IO::Path::PATH_SEPERATOR);
								Text::StrConcat(&sbuff[j + 1], sb.ToString());
								succ = LoadFileInner(sbuff, errMessage, false);
								if (!succ)
								{
									return false;
								}
								impModule = this->moduleMap->Get(sb.ToString());
							}
							
							if (impModule == 0)
							{
								errMessage->Append((const UTF8Char*)"IMPORTS module ");
								errMessage->Append(sb.ToString());
								errMessage->Append((const UTF8Char *)" not found");
								return false;
							}
							impSarr[1] = impObjNames.ToString();
							while (true)
							{
								UOSInt ui;
								impCnt = Text::StrSplitTrim(impSarr, 2, impSarr[1], ',');

								i = Text::StrIndexOf(impSarr[0], '{');
								if (i != INVALID_INDEX && Text::StrEndsWith(impSarr[0], (const UTF8Char*)"}"))
								{
									impSarr[0][i] = 0;
									while (i > 0 && Text::CharUtil::IsWS(&impSarr[0][i - 1]))
									{
										impSarr[0][--i] = 0;
									}
								}
								impObj = MemAlloc(ObjectInfo, 1);
								impObj->objectName = Text::StrCopyNew(impSarr[0]);
								impObj->typeName = Text::StrCopyNew(sb.ToString());
								impObj->typeVal = Text::StrCopyNew((const UTF8Char*)"Imported Value");
								impObj->oidLen = 0;
								NEW_CLASS(impObj->valName, Data::ArrayList<const UTF8Char*>());
								NEW_CLASS(impObj->valCont, Data::ArrayList<const UTF8Char*>());
								impObj->impModule = impModule;
								impObj->parsed = false;
								ui = module->objKeys->SortedInsert(impObj->objectName);
								module->objValues->Insert(ui, impObj);

//								OSInt impInd;
//								impInd = impModule->objKeys->SortedIndexOf(impSarr[0]);
/*								if (impInd >= 0)
								{
									impObj = impModule->objValues->GetItem((UOSInt)impInd);
									impObj2 = MemAlloc(ObjectInfo, 1);
									impObj2->objectName = Text::StrCopyNew(impSarr[0]);
									if (impObj->typeName)
									{
										impObj2->typeName = Text::StrCopyNew(impObj->typeName);
									}
									else
									{
										impObj2->typeName = 0;
									}
									impObj2->typeVal = Text::StrCopyNew((const UTF8Char*)"Imported Value");
									impObj2->oidLen = impObj->oidLen;
									if (impObj->oidLen > 0)
									{
										MemCopyNO(impObj2->oid, impObj->oid, impObj->oidLen);
									}
									NEW_CLASS(impObj2->valName, Data::ArrayList<const UTF8Char*>());
									NEW_CLASS(impObj2->valCont, Data::ArrayList<const UTF8Char*>());
									impObj2->impModule = impModule;
									impObj2->parsed = false;
									UOSInt ui = 0;
									UOSInt uj = impObj->valName->GetCount();
									while (ui < uj)
									{
										impObj2->valName->Add(Text::StrCopyNew(impObj->valName->GetItem(ui)));
										impObj2->valCont->Add(Text::StrCopyNew(impObj->valCont->GetItem(ui)));
										ui++;
									}
									
									ui = module->objKeys->SortedInsert(impObj2->objectName);
									module->objValues->Insert(ui, impObj2);
								}
								else
								{
									errMessage->Append((const UTF8Char*)"IMPORTS object ");
									errMessage->Append(impSarr[0]);
									errMessage->Append((const UTF8Char*)" in module ");
									errMessage->Append(sb.ToString());
									errMessage->Append((const UTF8Char *)" not found");
									return false;
								}*/
								if (impCnt != 2)
								{
									break;
								}
							}
							impObjNames.ClearStr();
						}
						else
						{
							impObjNames.Append(sb.ToString());
						}
						if (isEnd)
						{
							break;
						}
						sb.ClearStr();
						if (!reader->ReadLine(&sb))
						{
							errMessage->Append((const UTF8Char*)"IMPORTS end not found");
							return false;
						}
						sb.Trim();
					}
				}
				else if (sb.StartsWith((const UTF8Char*)"EXPORTS"))
				{
					while (true)
					{
						if (sb.EndsWith(';'))
						{
							break;
						}

						sb.ClearStr();
						if (!reader->ReadLine(&sb))
						{
							errMessage->Append((const UTF8Char*)"EXPORTS end not found");
							return false;
						}
						sb.Trim();
					}
				}
				else
				{
					if (currObj)
					{
						if (sbObjValName.GetLength() > 0 && sbObjValCont.GetLength() > 0)
						{
							currObj->valName->Add(Text::StrCopyNew(sbObjValName.ToString()));
							currObj->valCont->Add(Text::StrCopyNew(sbObjValCont.ToString()));
						}
						sbObjValName.ClearStr();
						sbObjValCont.ClearStr();
					}
					currObj = 0;
					i = sb.IndexOf((const UTF8Char*)"::=");
					if (i == 0)
					{
						errMessage->Append((const UTF8Char*)"::= found at non object location");
						return false;
					}
					if (i == INVALID_INDEX)
					{
						i = sb.IndexOf(' ');
						j = sb.IndexOf('\t');
						if (i == INVALID_INDEX)
						{
							i = j;
						}
						else if (j != INVALID_INDEX && j < i)
						{
							i = j;
						}
						if (i != INVALID_INDEX)
						{
							i = INVALID_INDEX;
						}
						else
						{
							i = sb.GetLength();
							if (!reader->ReadLine(&sb))
							{
								errMessage->Append((const UTF8Char*)"Unknown format 1: ");
								errMessage->Append(sb.ToString());
								return false;
							}
							if (sb.ToString()[i] == ' ' || sb.ToString()[i] == '\t')
							{
								i = sb.IndexOf((const UTF8Char*)"::=");
							}
							else
							{
								errMessage->Append((const UTF8Char*)"Unknown format 2: ");
								errMessage->Append(sb.ToString());
								return false;
							}
						}
					}
					if (i != INVALID_INDEX)
					{
						j = i;
						UOSInt k;
						while (sb.ToString()[j - 1] == ' ' || sb.ToString()[j - 1] == '\t')
						{
							j--;
						}
						k = sb.IndexOf(' ');
						UOSInt l = sb.IndexOf('\t');
						if (k == INVALID_INDEX)
						{
							k = l;
						}
						else if (l != INVALID_INDEX && l < k)
						{
							k = l;
						}
						l = sb.IndexOf('{');
						if (k == INVALID_INDEX || (l != INVALID_INDEX && l < k))
						{
							k = l;
						}
						obj = MemAlloc(ObjectInfo, 1);
						obj->objectName = Text::StrCopyNewC(sb.ToString(), k);
						if (j > k)
						{
							while (sb.ToString()[k] == ' ' || sb.ToString()[k] == '\t')
							{
								k++;
							}
							obj->typeName = Text::StrCopyNewC(sb.ToString() + k, (UOSInt)(j - k));
						}
						else
						{
							obj->typeName = 0;
						}
						obj->typeVal = 0;
						obj->oidLen = 0;
						NEW_CLASS(obj->valName, Data::ArrayList<const UTF8Char*>());
						NEW_CLASS(obj->valCont, Data::ArrayList<const UTF8Char*>());
						obj->impModule = 0;
						obj->parsed = false;
						UOSInt ui = module->objKeys->SortedInsert(obj->objectName);
						module->objValues->Insert(ui, obj);
						ui = this->globalModule.objKeys->SortedInsert(obj->objectName);
						this->globalModule.objValues->Insert(ui, obj);
						if (Text::StrEquals(obj->objectName, (const UTF8Char*)DEBUGOBJ))
						{
							currObj = 0;
						}

						if (sb.EndsWith((const UTF8Char*)"::="))
						{
							currObj = obj;
							objLineSpace = lineSpace;
							objIsEqual = true;
							sbObjValName.ClearStr();
							sbObjValCont.ClearStr();
						}
						else
						{
							i += 3;
							while (sb.ToString()[i] == ' ' || sb.ToString()[i] == '\t')
							{
								i++;
							}
							obj->typeVal = Text::StrCopyNew(sb.ToString() + i);
							currObj = obj;
							sbObjValName.ClearStr();
							sbObjValCont.ClearStr();
							objLineSpace = lineSpace;
							objIsEqual = false;
							objBrkType = 0;

							if (Text::StrIndexOf(obj->typeVal, (const UTF8Char*)"{") != INVALID_INDEX)
							{
								UOSInt startCnt = Text::StrCountChar(obj->typeVal, '{');
								UOSInt endCnt = Text::StrCountChar(obj->typeVal, '}');
								if (endCnt >= startCnt)
								{
								}
								else
								{
									objBrkType = '{';
									objIsEqual = true;
								}
							}
							else if ((i = Text::StrIndexOf(obj->typeVal, (const UTF8Char*)"(")) != INVALID_INDEX)
							{
								UOSInt startCnt = Text::StrCountChar(obj->typeVal, '(');
								UOSInt endCnt = Text::StrCountChar(obj->typeVal, ')');
								if (endCnt >= startCnt)
								{
								}
								else
								{
									objBrkType = '(';
									objIsEqual = true;
								}
							}

							if (!objIsEqual)
							{
								if (Text::StrEndsWith(obj->typeVal, (const UTF8Char*)"SIZE"))
								{
									sb.ClearStr();
									sb.Append(obj->typeVal);
									sbObjValCont.ClearStr();
									reader->NextWord(&sbObjValCont);
									if (sbObjValCont.ToString()[0] != '(')
									{
										errMessage->Append((const UTF8Char*)"Unexpected SIZE format: ");
										errMessage->Append(sbObjValCont.ToString());
										return false;
									}
									sb.Append(sbObjValCont.ToString());

									sbObjValCont.ClearStr();
									reader->PeekWord(&sbObjValCont);
									if (sbObjValCont.Equals((const UTF8Char*)"OF"))
									{
										sb.AppendChar(' ', 1);
										reader->NextWord(&sb);
										sb.AppendChar(' ', 1);
										reader->NextWord(&sb);
									}
									sbObjValCont.ClearStr();
									Text::StrDelNew(obj->typeVal);
									obj->typeVal = Text::StrCopyNew(sb.ToString());
								}
								else if (Text::StrEndsWith(obj->typeVal, (const UTF8Char*)"TEXTUAL-CONVENTION"))
								{
									while (true)
									{
										sbObjValName.ClearStr();
										reader->PeekWord(&sbObjValName);
										if (IsUnknownType(sbObjValName.ToString()))
										{
											sbObjValName.ClearStr();
											reader->NextWord(&sbObjValName);
											sbObjValCont.ClearStr();
											reader->NextWord(&sbObjValCont);
											sb.ClearStr();
											reader->PeekWord(&sb);
											if (sb.StartsWith((const UTF8Char*)"{") || sb.StartsWith((const UTF8Char*)"("))
											{
												reader->NextWord(&sbObjValCont);
											}
											obj->valName->Add(Text::StrCopyNew(sbObjValName.ToString()));
											obj->valCont->Add(Text::StrCopyNew(sbObjValCont.ToString()));
											sbObjValCont.ClearStr();
										}
										else
										{
											sbObjValName.ClearStr();
											break;
										}
									}
								}
								else if (Text::StrEndsWith(obj->typeVal, (const UTF8Char*)" OF"))
								{
									sb.ClearStr();
									sb.Append(obj->typeVal);
									sb.AppendChar(' ', 1);
									reader->NextWord(&sb);

									Text::StringBuilderUTF8 sbTmp;
									reader->PeekWord(&sbTmp);
									if (sbTmp.StartsWith((const UTF8Char*)"{"))
									{
										reader->NextWord(&sb);
									}
									Text::StrDelNew(obj->typeVal);
									obj->typeVal = Text::StrCopyNew(sb.ToString());
								}
								else
								{
									sbObjValCont.ClearStr();
									reader->PeekWord(&sbObjValCont);
									if (sbObjValCont.Equals((const UTF8Char*)"OF"))
									{
										sb.ClearStr();
										sb.Append(obj->typeVal);
										sb.AppendChar(' ', 1);
										reader->NextWord(&sb);
										sb.AppendChar(' ', 1);
										reader->NextWord(&sb);
										Text::StrDelNew(obj->typeVal);
										obj->typeVal = Text::StrCopyNew(sb.ToString());
									}
									else if (sbObjValCont.StartsWith((const UTF8Char*)"("))
									{
										sb.ClearStr();
										sb.Append(obj->typeVal);
										sb.AppendChar(' ', 1);
										reader->NextWord(&sb);

										sbObjValCont.ClearStr();
										reader->PeekWord(&sbObjValCont);
										if (sbObjValCont.Equals((const UTF8Char*)"OF"))
										{
											sb.AppendChar(' ', 1);
											reader->NextWord(&sb);
											sb.AppendChar(' ', 1);
											reader->NextWord(&sb);
										}
										Text::StrDelNew(obj->typeVal);
										obj->typeVal = Text::StrCopyNew(sb.ToString());
									}
									sbObjValCont.ClearStr();
								}
							}
						}
					}
					else
					{
						i = sb.IndexOf(' ');
						j = sb.IndexOf('\t');
						if (i == INVALID_INDEX)
						{
							i = j;
						}
						else if (j != INVALID_INDEX && j < i)
						{
							i = j;
						}
						if (i == INVALID_INDEX)
						{
							i = sb.GetLength();
							if (!reader->ReadLine(&sb))
							{
								errMessage->Append((const UTF8Char*)"Unknown format 3: ");
								errMessage->Append(sb.ToString());
								return false;
							}
							if (sb.ToString()[i] == ' ' || sb.ToString()[i] == '\t')
							{
							}
							else
							{
								errMessage->Append((const UTF8Char*)"Unknown format 4: ");
								errMessage->Append(sb.ToString());
								return false;
							}
						}

						obj = MemAlloc(ObjectInfo, 1);
						obj->objectName = Text::StrCopyNewC(sb.ToString(), (UOSInt)i);
						while (sb.ToString()[i] == ' ' || sb.ToString()[i] == '\t')
						{
							i++;
						}
						obj->typeName = Text::StrCopyNew(sb.ToString() + i);
						obj->typeVal = 0;
						obj->oidLen = 0;
						NEW_CLASS(obj->valName, Data::ArrayList<const UTF8Char*>());
						NEW_CLASS(obj->valCont, Data::ArrayList<const UTF8Char*>());
						obj->impModule = 0;
						obj->parsed = false;
						UOSInt ui = module->objKeys->SortedInsert(obj->objectName);
						module->objValues->Insert(ui, obj);
						ui = this->globalModule.objKeys->SortedInsert(obj->objectName);
						this->globalModule.objValues->Insert(ui, obj);
						if (Text::StrEquals(obj->objectName, (const UTF8Char*)DEBUGOBJ))
						{
							currObj = obj;
						}

						while (true)
						{
							sbObjValName.ClearStr();
							reader->PeekWord(&sbObjValName);
							if (IsUnknownType(sbObjValName.ToString()))
							{
								sbObjValName.ClearStr();
								reader->NextWord(&sbObjValName);
								sbObjValCont.ClearStr();
								reader->NextWord(&sbObjValCont);
								sb.ClearStr();
								reader->PeekWord(&sb);
								if (sb.StartsWith((const UTF8Char*)"{") || sb.StartsWith((const UTF8Char*)"("))
								{
									reader->NextWord(&sbObjValCont);
								}
								obj->valName->Add(Text::StrCopyNew(sbObjValName.ToString()));
								obj->valCont->Add(Text::StrCopyNew(sbObjValCont.ToString()));
								sbObjValCont.ClearStr();
							}
							else if (sbObjValCont.Equals(((const UTF8Char*)"::=")))
							{
								reader->NextWord(&sbObjValCont);
								currObj = obj;
								objLineSpace = lineSpace;
								objIsEqual = true;
								sbObjValName.ClearStr();
								sbObjValCont.ClearStr();
								break;
							}
							else
							{
								currObj = obj;
								objLineSpace = lineSpace;
								objIsEqual = false;
								sbObjValName.ClearStr();
								sbObjValCont.ClearStr();
								break;
							}
						}						
					}
				}
			}
		}
	}
}

Bool Net::ASN1MIB::ApplyModuleOID(ModuleInfo *module, ObjectInfo *obj, Text::StringBuilderUTF *errMessage)
{
	Bool valid = false;
	if (obj->parsed)
	{
		return true;
	}
	if (obj->impModule != 0)
	{
		OSInt i = obj->impModule->objKeys->SortedIndexOf(obj->objectName);
		if (i < 0)
		{
			errMessage->Append((const UTF8Char*)"IMPORTS object ");
			errMessage->Append(obj->objectName);
			errMessage->Append((const UTF8Char*)" in module ");
			errMessage->Append(obj->typeName);
			errMessage->Append((const UTF8Char*)" not found");
			return false;
		}
		ObjectInfo *impObj = obj->impModule->objValues->GetItem((UOSInt)i);
		if (!impObj->parsed)
		{
			if (!ApplyModuleOID(obj->impModule, impObj, errMessage))
			{
				return false;
			}
		}

		SDEL_TEXT(obj->typeName);
		if (impObj->typeName)
		{
			obj->typeName = Text::StrCopyNew(impObj->typeName);
		}
		SDEL_TEXT(obj->typeVal);
		obj->typeVal = SCOPY_TEXT(impObj->typeVal);
		obj->oidLen = impObj->oidLen;
		if (impObj->oidLen > 0)
		{
			MemCopyNO(obj->oid, impObj->oid, impObj->oidLen);
		}

		UOSInt uk = 0;
		UOSInt ul = impObj->valName->GetCount();
		while (uk < ul)
		{
			obj->valName->Add(Text::StrCopyNew(impObj->valName->GetItem(uk)));
			obj->valCont->Add(Text::StrCopyNew(impObj->valCont->GetItem(uk)));
			uk++;
		}
	}

	if (obj->typeName && obj->typeVal && obj->oidLen == 0)
	{
		valid = true;
		if (Text::StrEquals(obj->typeName, (const UTF8Char*)"TRAP-TYPE")) valid = false;
		if (Text::StrEquals(obj->typeName, (const UTF8Char*)"ATTRIBUTE")) valid = false;
		if (Text::StrEquals(obj->typeName, (const UTF8Char*)"INTEGER")) valid = false;
		if (Text::StrEquals(obj->typeName, (const UTF8Char*)"NULL")) valid = false;
		if (Text::StrStartsWith(obj->typeName, (const UTF8Char*)"OCTET STRING")) valid = false;

		if (Text::StrEquals(obj->typeVal, (const UTF8Char*)"Imported Value")) valid = false;
		if (Text::StrIndexOf(obj->typeVal, (const UTF8Char*)",") != INVALID_INDEX) valid = false;
		if (Text::StrIndexOf(obj->typeVal, (const UTF8Char*)"...") != INVALID_INDEX) valid = false;
	}
	obj->parsed = true;
	if (valid)
	{
		if (Text::StrEquals(obj->typeName, (const UTF8Char*)"OBJECT IDENTIFIER"))
		{
			if (!this->ParseObjectOID(module, obj, obj->typeVal, errMessage))
			{
				return false;
			}
		}
		else
		{
			Text::StringBuilderUTF8 sb;
			if (!this->ParseObjectOID(module, obj, obj->typeVal, &sb))
			{
				obj->oidLen = 0;
			}
		}
		ModuleAppendOID(module, obj);
		ModuleAppendOID(&this->globalModule, obj);
	}
	else if (obj->oidLen > 0)
	{
		ModuleAppendOID(module, obj);
		ModuleAppendOID(&this->globalModule, obj);
	}
	return true;
}

Bool Net::ASN1MIB::ApplyModuleOIDs(ModuleInfo *module, Text::StringBuilderUTF *errMessage)
{
	Data::ArrayList<ObjectInfo*> *objList = module->objValues;
	ObjectInfo *obj;
	UOSInt ui = 0;
	UOSInt uj = objList->GetCount();
	while (ui < uj)
	{
		obj = objList->GetItem(ui);
		if (!ApplyModuleOID(module, obj, errMessage))
		{
			return false;
		}
		ui++;
	}
	return true;
}

Bool Net::ASN1MIB::ApplyOIDs(Text::StringBuilderUTF *errMessage)
{
	Data::ArrayList<ModuleInfo*> *moduleList = this->moduleMap->GetValues();
	UOSInt i = moduleList->GetCount();
	while (i-- > 0)
	{
		if (!ApplyModuleOIDs(moduleList->GetItem(i), errMessage))
		{
			return false;
		}
	}
	return true;
}

/*Bool Net::ASN1MIB::ApplyModuleImports(ModuleInfo *module, Text::StringBuilderUTF *errMessage)
{
	Data::ArrayList<ObjectInfo*> *objList = module->objValues;
	ObjectInfo *obj;
	UOSInt ui = 0;
	UOSInt uj = objList->GetCount();
	while (ui < uj)
	{
		obj = objList->GetItem(ui);
		if (obj->typeName && obj->typeVal && Text::StrEquals(obj->typeVal, (const UTF8Char*)"Imported Value"))
		{
			ModuleInfo *impModule = this->moduleMap->Get(obj->typeName);
			ObjectInfo *impObj;
			OSInt impInd;
			impInd = impModule->objKeys->SortedIndexOf(obj->objectName);
			if (impInd >= 0)
			{
				impObj = impModule->objValues->GetItem((UOSInt)impInd);
				if (!impObj->parsed)
				{
					if (!ApplyModuleOID(impModule, impObj, errMessage))
					{
						return false;
					}
				}
				SDEL_TEXT(obj->typeName);
				if (impObj->typeName)
				{
					obj->typeName = Text::StrCopyNew(impObj->typeName);
				}
				SDEL_TEXT(obj->typeVal);
				obj->typeVal = SCOPY_TEXT(impObj->typeVal);
				obj->oidLen = impObj->oidLen;
				if (impObj->oidLen > 0)
				{
					MemCopyNO(obj->oid, impObj->oid, impObj->oidLen);
				}

				UOSInt uk = 0;
				UOSInt ul = impObj->valName->GetCount();
				while (uk < ul)
				{
					obj->valName->Add(Text::StrCopyNew(impObj->valName->GetItem(uk)));
					obj->valCont->Add(Text::StrCopyNew(impObj->valCont->GetItem(uk)));
					uk++;
				}
			}
			else
			{
				errMessage->Append((const UTF8Char*)"IMPORTS object ");
				errMessage->Append(obj->objectName);
				errMessage->Append((const UTF8Char*)" in module ");
				errMessage->Append(obj->typeName);
				errMessage->Append((const UTF8Char *)" not found");
				return false;
			}
		}
		ui++;
	}
	return true;
}*/

Bool Net::ASN1MIB::ApplyImports(Text::StringBuilderUTF *errMessage)
{
/*	Data::ArrayList<ModuleInfo*> *moduleList = this->moduleMap->GetValues();
	UOSInt i = moduleList->GetCount();
	while (i-- > 0)
	{
		if (!ApplyModuleImports(moduleList->GetItem(i), errMessage))
		{
			return false;
		}
	}*/
	return true;
}

Bool Net::ASN1MIB::LoadFileInner(const UTF8Char *fileName, Text::StringBuilderUTF *errMessage, Bool postApply)
{
	Text::StringBuilderUTF8 sbFileName;
	IO::FileStream *fs;
	Net::MIBReader *reader;
	ModuleInfo *module;
	Bool succ;
	if (IO::Path::GetPathType(fileName) != IO::Path::PT_FILE)
	{
		sbFileName.ClearStr();
		sbFileName.Append(fileName);
		sbFileName.Append((const UTF8Char*)".asn");
		if (IO::Path::GetPathType(sbFileName.ToString()) == IO::Path::PT_FILE)
		{
			fileName = sbFileName.ToString();
		}
		else
		{
			sbFileName.ClearStr();
			sbFileName.Append(fileName);
			sbFileName.Append((const UTF8Char*)".mib");
			fileName = sbFileName.ToString();
		}
	}
	NEW_CLASS(fs, IO::FileStream(fileName, IO::FileStream::FILE_MODE_READONLY, IO::FileStream::FILE_SHARE_DENY_NONE, IO::FileStream::BT_NORMAL));
	if (fs->IsError())
	{
		DEL_CLASS(fs);
		errMessage->Append((const UTF8Char*)"Error in opening file ");
		errMessage->Append(fileName);
		return false;
	}
	succ = false;
	NEW_CLASS(reader, Net::MIBReader(fs));
	Text::StringBuilderUTF8 sb;
	Text::StringBuilderUTF8 sbModuleName;
	Text::StringBuilderUTF8 sbOID;
	sb.ClearStr();
	if (!reader->NextWord(&sbModuleName))
	{
		errMessage->Append((const UTF8Char*)"Module definition not found");
	}
	else if (!Text::CharUtil::IsAlphaNumeric(sbModuleName.ToString()[0]))
	{
		errMessage->Append((const UTF8Char*)"Module name not found");
	}
	else if (!reader->NextWord(&sbOID))
	{
		errMessage->Append((const UTF8Char*)"Invalid file format: Unexpected end of file 1");
	}
	else
	{
		succ = true;
		if (sbOID.ToString()[0] == '{')
		{
			if (!reader->NextWord(&sb))
			{
				errMessage->Append((const UTF8Char*)"Invalid file format: Unexpected end of file 2");
				succ = false;
			}
		}
		else
		{
			sb.Append(sbOID.ToString());
			sbOID.ClearStr();
		}
		if (succ && !sb.Equals((const UTF8Char*)"DEFINITIONS"))
		{
			errMessage->Append((const UTF8Char*)"Invalid file format: Expected DEFINITIONS: ");
			errMessage->Append(sb.ToString());
			succ = false;
		}
		sb.ClearStr();
		if (succ && !reader->NextWord(&sb))
		{
			errMessage->Append((const UTF8Char*)"Invalid file format: Unexpected end of file 3");
			succ = false;
		}
		if (succ && (sb.Equals((const UTF8Char*)"IMPLICIT") || sb.Equals((const UTF8Char*)"EXPLICIT")))
		{
			sb.ClearStr();
			if (!reader->NextWord(&sb))
			{
				errMessage->Append((const UTF8Char*)"Invalid file format: Unexpected end of file 4");
				succ = false;
			}
			else if (!sb.Equals((const UTF8Char*)"TAGS"))
			{
				errMessage->Append((const UTF8Char*)"Invalid file format: Expected TAGS: ");
				errMessage->Append(sb.ToString());
				succ = false;
			}
			else
			{
				sb.ClearStr();
				if (!reader->NextWord(&sb))
				{
					errMessage->Append((const UTF8Char*)"Invalid file format: Unexpected end of file 5");
					succ = false;
				}
			}
		}
		if (succ && !sb.Equals((const UTF8Char*)"::="))
		{
			errMessage->Append((const UTF8Char*)"Invalid file format: Expected ::= : ");
			errMessage->Append(sb.ToString());
			succ = false;
		}
		if (succ)
		{
			sb.ClearStr();
			if (!reader->NextWord(&sb))
			{
				errMessage->Append((const UTF8Char*)"Invalid file format: Unexpected end of file 6");
				succ = false;
			}
			else if (!sb.Equals((const UTF8Char*)"BEGIN"))
			{
				errMessage->Append((const UTF8Char*)"Invalid file format: Expected BEGIN: ");
				errMessage->Append(sb.ToString());
				succ = false;
			}
		}
	}

	if (succ)
	{
		module = MemAlloc(ModuleInfo, 1);
		module->moduleName = Text::StrCopyNew(sbModuleName.ToString());
		module->moduleFileName = Text::StrCopyNew(fileName);
		NEW_CLASS(module->objKeys, Data::ArrayListStrUTF8());
		NEW_CLASS(module->objValues, Data::ArrayList<ObjectInfo*>());
		NEW_CLASS(module->oidList, Data::ArrayList<ObjectInfo*>());
		this->moduleMap->Put(module->moduleName, module);
		if ((succ = ParseModule(reader, module, errMessage)) == true)
		{
			if (postApply)
			{
				succ = ApplyImports(errMessage) && ApplyModuleOIDs(module, errMessage);
			}
		}
	}

/*	
	Bool moduleFound = false;
	OSInt i;
	while (true)
	{
		sb.ClearStr();
		if (!reader->ReadLine(&sb))
		{
			if (!moduleFound)
			{
				errMessage->Append((const UTF8Char*)"Module definition not found");
			}
			break;
		}
		sb.Trim();
		if (sb.GetLength() > 0)
		{
			if (moduleFound)
			{
				errMessage->Append((const UTF8Char*)"Object found after Module definition");
				succ = false;
				break;
			}
			i = sb.IndexOf((const UTF8Char*)" DEFINITIONS ::= BEGIN");
			if (i == INVALID_INDEX)
			{
				succ = false;
				errMessage->Append((const UTF8Char*)"Wrong Module definition format");
				break;
			}
			sb.ToString()[i] = 0;
			if (this->moduleMap->Get(sb.ToString()))
			{
				errMessage->Append((const UTF8Char*)"Module ");
				errMessage->Append(sb.ToString());
				errMessage->Append((const UTF8Char*)" already loaded");
				break;
			}
			module = MemAlloc(ModuleInfo, 1);
			module->moduleName = Text::StrCopyNew(sb.ToString());
			module->moduleFileName = Text::StrCopyNew(fileName);
			NEW_CLASS(module->objKeys, Data::ArrayListStrUTF8());
			NEW_CLASS(module->objValues, Data::ArrayList<ObjectInfo*>());
			NEW_CLASS(module->oidList, Data::ArrayList<ObjectInfo*>());
			this->moduleMap->Put(module->moduleName, module);
			succ = ParseModule(reader, module, errMessage);
			moduleFound = true;
			if (!succ)
			{
				break;
			}
		}
	}*/
	DEL_CLASS(reader);
	DEL_CLASS(fs);
	return succ;
}

void Net::ASN1MIB::RemoveSpace(UTF8Char *s)
{
	if (s)
	{
		UOSInt strLen = Text::StrCharCnt(s);
		UOSInt wsCnt = 0;
		while (strLen-- > 0)
		{
			if (s[strLen] == '\r' || s[strLen] == '\n' || s[strLen] == '\t' || s[strLen] == ' ')
			{
				s[strLen] = ' ';
				wsCnt++;
			}
			else if (wsCnt > 0)
			{
				if (wsCnt > 1)
				{
					Text::StrLTrim(&s[strLen + 2]);
				}
				wsCnt = 0;
			}
		}
	}
}

Bool Net::ASN1MIB::IsType(const UTF8Char *s)
{
	UTF8Char c;
	if (s[0] == 0)
	{
		return false;
	}
	while ((c = *s++) != 0)
	{
		if (c != ' ' && c != '-' && (c < 'A' || c > 'Z'))
		{
			return false;
		}
	}
	return true;
}

Bool Net::ASN1MIB::IsKnownType(const UTF8Char *s)
{
	if (Text::StrEquals(s, (const UTF8Char*)"OCTET STRING") ||
		Text::StrEquals(s, (const UTF8Char*)"END") ||
		Text::StrEquals(s, (const UTF8Char*)"INTEGER") ||
		Text::StrEquals(s, (const UTF8Char*)"OBJECT IDENTIFIER"))
	{
		return true;
	}
	return false;
}

Bool Net::ASN1MIB::IsUnknownType(const UTF8Char *s)
{
	return IsType(s) && !IsKnownType(s);
}

OSInt Net::ASN1MIB::BranketEnd(const UTF8Char *s, UTF8Char *brkType)
{
	OSInt i = 0;
	UTF8Char c;
	UTF8Char brkStart;
	UTF8Char brkEnd;
	UOSInt level;
	while (true)
	{
		c = s[i++];
		if (c == 0)
		{
			return -1;
		}
		if (c == '{')
		{
			brkStart = '{';
			brkEnd = '}';
			break;
		}
		else if (c == '(')
		{
			brkStart = '(';
			brkEnd = ')';
			break;
		}
	}
	level = 1;
	while ((c = s[i++]) != 0)
	{
		if (c == brkStart)
		{
			level++;
		}
		else if (c == brkEnd)
		{
			level--;
			if (level == 0)
			{
				if (brkType)
				{
					*brkType = brkStart;
				}
				return i;
			}
		}
	}
	return -1;
}

const UTF8Char *Net::ASN1MIB::SkipWS(const UTF8Char *s)
{
	UTF8Char c;
	while (true)
	{
		c = *s;
		if (c == 0)
		{
			return s;
		}
		else if (c != ' ' && c != '\t' && c != '\r' && c != '\n')
		{
			return s;
		}
		s++;
	}
}

UTF8Char Net::ASN1MIB::NextChar(const UTF8Char *s)
{
	return SkipWS(s)[0];
}

Net::ASN1MIB::ASN1MIB()
{
	NEW_CLASS(this->moduleMap, Data::StringUTF8Map<ModuleInfo *>());
	NEW_CLASS(this->globalModule.objKeys, Data::ArrayListStrUTF8());
	NEW_CLASS(this->globalModule.objValues, Data::ArrayList<ObjectInfo*>());
	NEW_CLASS(this->globalModule.oidList, Data::ArrayList<ObjectInfo*>());
}

Net::ASN1MIB::~ASN1MIB()
{
	this->UnloadAll();
	DEL_CLASS(this->moduleMap);
	DEL_CLASS(this->globalModule.objKeys);
	DEL_CLASS(this->globalModule.objValues);
	DEL_CLASS(this->globalModule.oidList);
}

Net::ASN1MIB::ModuleInfo *Net::ASN1MIB::GetGlobalModule()
{
	return &this->globalModule;	
}

Net::ASN1MIB::ModuleInfo *Net::ASN1MIB::GetModuleByFileName(const UTF8Char *fileName)
{
	Data::ArrayList<ModuleInfo*> *moduleList = this->moduleMap->GetValues();
	ModuleInfo *module;
	UOSInt i = moduleList->GetCount();
	while (i-- > 0)
	{
		module = moduleList->GetItem(i);
		if (module->moduleFileName && Text::StrEquals(module->moduleFileName, fileName))
			return module;
	}
	return 0;
}

void Net::ASN1MIB::UnloadAll()
{
	Data::ArrayList<ModuleInfo*> *moduleList = this->moduleMap->GetValues();
	Data::ArrayList<ObjectInfo*> *objList;
	ObjectInfo *obj;
	ModuleInfo *module;
	UOSInt i = moduleList->GetCount();
	UOSInt j;
	UOSInt k;
	while (i-- > 0)
	{
		module = moduleList->GetItem(i);
		objList = module->objValues;
		j = objList->GetCount();
		while (j-- > 0)
		{
			obj = objList->GetItem(j);
			Text::StrDelNew(obj->objectName);
			SDEL_TEXT(obj->typeName);
			SDEL_TEXT(obj->typeVal);
			k = obj->valName->GetCount();
			while (k-- > 0)
			{
				Text::StrDelNew(obj->valName->GetItem(k));
				Text::StrDelNew(obj->valCont->GetItem(k));
			}
			DEL_CLASS(obj->valName);
			DEL_CLASS(obj->valCont);
			MemFree(obj);
		}

		DEL_CLASS(module->objKeys);
		DEL_CLASS(module->objValues);
		SDEL_TEXT(module->moduleName);
		SDEL_TEXT(module->moduleFileName);
		DEL_CLASS(module->oidList);
		MemFree(module);
	}
	this->moduleMap->Clear();

	this->globalModule.oidList->Clear();
	this->globalModule.objKeys->Clear();
	this->globalModule.objValues->Clear();
}

Bool Net::ASN1MIB::LoadFile(const UTF8Char *fileName, Text::StringBuilderUTF *errMessage)
{
	return LoadFileInner(fileName, errMessage, true);
}
