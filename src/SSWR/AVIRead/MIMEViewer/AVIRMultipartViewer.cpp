#include "Stdafx.h"
#include "Crypto/Cert/X509PKCS7.h"
#include "Crypto/Hash/HashCreator.h"
#include "IO/MemoryStream.h"
#include "Net/ASN1PDUBuilder.h"
#include "Net/SSLEngineFactory.h"
#include "Parser/FileParser/X509Parser.h"
#include "SSWR/AVIRead/MIMEViewer/AVIRMultipartViewer.h"
#include "Text/MIMEObj/UnknownMIMEObj.h"
#include "UI/GUITabPage.h"
#include "UI/GUITextBox.h"

#include <stdio.h>

SSWR::AVIRead::MIMEViewer::AVIRMultipartViewer::AVIRMultipartViewer(NotNullPtr<SSWR::AVIRead::AVIRCore> core, NotNullPtr<UI::GUICore> ui, NotNullPtr<UI::GUIClientControl> ctrl, NotNullPtr<Media::ColorManagerSess> sess, Text::MIMEObj::MultipartMIMEObj *obj) : SSWR::AVIRead::MIMEViewer::AVIRMIMEViewer(core, ctrl, obj)
{
	UInt8 hashBuff[64];
	UOSInt i;
	UOSInt j;
	Text::IMIMEObj *subObj;
	Text::MIMEObj::MIMEMessage *part;
	this->obj = obj;

	if (obj->GetContentType().StartsWith(UTF8STRC("multipart/signed")))
	{
		NEW_CLASSNN(this->pnlSMIME, UI::GUIPanel(ui, ctrl));
		this->pnlSMIME->SetRect(0, 0, 100, 31, false);
		this->pnlSMIME->SetDockType(UI::GUIControl::DOCK_BOTTOM);
		NEW_CLASS(this->lblSignState, UI::GUILabel(ui, this->pnlSMIME, CSTR("Signature")));
		this->lblSignState->SetRect(4, 4, 100, 23, false);
		NEW_CLASS(this->txtSignState, UI::GUITextBox(ui, this->pnlSMIME, CSTR("")));
		this->txtSignState->SetRect(104, 4, 200, 23, false);
		this->txtSignState->SetReadOnly(true);

		j = this->obj->GetPartCount();
		if (j == 2)
		{
			part = this->obj->GetPart(0);
			IO::MemoryStream mstm;
			part->WriteStream(&mstm);
			
			subObj = this->obj->GetPartContent(1);
			if (subObj->GetContentType().StartsWith(UTF8STRC("application/pkcs7-signature")))
			{
				UOSInt dataSize;
				const UInt8 *data = ((Text::MIMEObj::UnknownMIMEObj*)subObj)->GetRAWData(&dataSize);
				Net::ASN1Data *asn1 = Parser::FileParser::X509Parser::ParseBuff(Data::ByteArrayR(data, dataSize), subObj->GetSourceNameObj());
				if (asn1)
				{
					if (asn1->GetASN1Type() == Net::ASN1Data::ASN1Type::X509)
					{
						Crypto::Cert::X509File *x509 = (Crypto::Cert::X509File*)asn1;
						if (x509->GetFileType() == Crypto::Cert::X509File::FileType::PKCS7)
						{
							Crypto::Cert::X509PKCS7 *pkcs7 = (Crypto::Cert::X509PKCS7*)x509;
							Crypto::Hash::HashType hashType = pkcs7->GetDigestType();
							data = pkcs7->GetMessageDigest(&dataSize);
							if (data == 0)
							{
								this->txtSignState->SetText(CSTR("Message Digest not found"));
							}
							else if (hashType == Crypto::Hash::HashType::Unknown)
							{
								this->txtSignState->SetText(CSTR("Unknown Digest Type"));
							}
							else
							{
								Crypto::Hash::IHash *hash = Crypto::Hash::HashCreator::CreateHash(hashType);
								UOSInt buffSize;
								const UInt8 *dataBuff = mstm.GetBuff(buffSize);
								if (buffSize > 2 && dataBuff[buffSize - 2] == 13 && dataBuff[buffSize - 1] == 10)
								{
									buffSize -= 2;
								}
								hash->Calc(dataBuff, buffSize);
								buffSize = hash->GetResultSize();
								hash->GetValue(hashBuff);
								if (!Text::StrEqualsC(data, dataSize, hashBuff, buffSize))
								{
									this->txtSignState->SetText(CSTR("Digest Mismatch"));
								}
								else
								{
									UOSInt encLen;
									const UInt8 *encDigestData = pkcs7->GetEncryptedDigest(&encLen);
									if (encDigestData == 0)
									{
										this->txtSignState->SetText(CSTR("Signature data not found"));
									}
									else
									{
										Crypto::Cert::X509Cert *crt = pkcs7->GetNewCert(0);
										if (crt == 0)
										{
											this->txtSignState->SetText(CSTR("Cert not found"));
										}
										else
										{
											NotNullPtr<Crypto::Cert::X509Key> key;
											if (!key.Set(crt->GetNewPublicKey()))
											{
												this->txtSignState->SetText(CSTR("Public key not found"));
											}
											else
											{
												Net::SSLEngine *ssl = Net::SSLEngineFactory::Create(core->GetSocketFactory(), false);
												if (ssl == 0)
												{
													this->txtSignState->SetText(CSTR("Error in initializing SSL Engine"));
												}
												else
												{
													Text::StringBuilderUTF8 sb;
													sb.AppendC(UTF8STRC("Hash:\r\n"));
													sb.AppendHexBuff(hashBuff, buffSize, ' ', Text::LineBreakType::CRLF);

													Crypto::Cert::DigestInfo digestInfo;
													Bool match = false;
													UInt8 decBuff[256];
													UOSInt decLen = ssl->Decrypt(key, decBuff, encDigestData, encLen, Crypto::Encrypt::RSACipher::Padding::PKCS1);
													if (decLen > 0)
													{
														if (Crypto::Cert::X509File::ParseDigestType(&digestInfo, decBuff, decBuff + decLen))
														{
															sb.AppendC(UTF8STRC("\r\nHash Type: "));
															sb.Append(Crypto::Hash::HashTypeGetName(digestInfo.hashType));
															sb.AppendC(UTF8STRC("\r\nHash Value:\r\n"));
															sb.AppendHexBuff(digestInfo.hashVal, digestInfo.hashLen, ' ', Text::LineBreakType::CRLF);
															match = Text::StrEqualsC(digestInfo.hashVal, digestInfo.hashLen, hashBuff, buffSize);
														}
													}
													printf("%s\r\n", sb.ToString());
													if (match)
													{
														this->txtSignState->SetText(CSTR("Signature valid"));
													}
													else
													{
														this->txtSignState->SetText(CSTR("Signature invalid"));
													}
													DEL_CLASS(ssl);
												}

												key.Delete();
											}
											DEL_CLASS(crt);
										}
									}
								}
							}
						}
						else
						{
							this->txtSignState->SetText(CSTR("Signature is not in PKCS7 format"));
						}
					}
					else
					{
						this->txtSignState->SetText(CSTR("Signature is not a X.509 file"));
					}
					DEL_CLASS(asn1);
				}
				else
				{
					this->txtSignState->SetText(CSTR("Error in parsing Signature file"));
				}
			}
			else
			{
				this->txtSignState->SetText(CSTR("Signature file not found"));
			}
		}
		else
		{
			this->txtSignState->SetText(CSTR("Unexpected Multipart format"));
		}
	}
	NotNullPtr<UI::GUITabPage> tp;
	SSWR::AVIRead::MIMEViewer::AVIRMIMEViewer *viewer;
	NEW_CLASS(this->tcParts, UI::GUITabControl(ui, ctrl));
	this->tcParts->SetDockType(UI::GUIControl::DOCK_FILL);

	Text::String *defMsg = obj->GetDefMsg();
	if (defMsg && defMsg->v[0])
	{
		UI::GUITextBox *txt;
		tp = this->tcParts->AddTabPage(CSTR("Default"));
		NEW_CLASS(txt, UI::GUITextBox(ui, tp, obj->GetDefMsg()->ToCString(), true));
		txt->SetDockType(UI::GUIControl::DOCK_FILL);
		txt->SetReadOnly(true);
	}

	i = 0;
	j = obj->GetPartCount();
	while (i < j)
	{
		subObj = obj->GetPartContent(i);
		tp = this->tcParts->AddTabPage(subObj->GetSourceNameObj());
		viewer = SSWR::AVIRead::MIMEViewer::AVIRMIMEViewer::CreateViewer(core, ui, tp, sess, subObj);
		this->subViewers.Add(viewer);
		i++;
	}
}

SSWR::AVIRead::MIMEViewer::AVIRMultipartViewer::~AVIRMultipartViewer()
{
	SSWR::AVIRead::MIMEViewer::AVIRMIMEViewer *viewer;
	UOSInt i;
	i = this->subViewers.GetCount();
	while (i-- > 0)
	{
		viewer = this->subViewers.GetItem(i);
		DEL_CLASS(viewer);
	}
}
