#include <vcl.h>
#pragma hdrstop
// ---------------------------------------------------------------------------
#include "ConvDW.h"
#include <io.h>
#include <math.h>
#include <string.h>
#include <stdio.h>
#include <dir.h>
#include <stdlib.h>
#include <sys\stat.h>
#include <process.h>

wchar_t LANGUAGELITERA = L'R';
extern wchar_t Inf[], LHA[];

// ---------------------------------------------------------------------------
char ChConvChrDOStoWIN(unsigned char B) {
 if(B>127&&B<176) B+=64;
 else if(B>223&&B<240) B+=16;
 else switch(B) {
  case 179: B='|'; break; //¦
  case 180: B='+'; break; //¦
  case 191: B='+'; break; //¦
  case 192: B='+'; break; //¦
  case 193: B='+'; break; //¦
  case 194: B='+'; break; //¦
  case 195: B='+'; break; //¦
  case 196: B='-'; break; //¦
  case 197: B='+'; break; //¦
  case 217: B='+'; break; //¦
  case 218: B='+'; break; //¦
  case 240: B=168; break; //Ё
  case 241: B=184; break; //ё
  case 242: B=170; break; //Є
  case 243: B=186; break; //є
  case 244: B=175; break; //Ї
  case 245: B=191; break; //ї
  case 248: B=176; break; //градус
// case 251: B=1??; break; //корень
  case 252: B=185; break; //номер
  case 253: B=164; break; //солнышко
 }
 return B;
}
//---------------------------------------------------------------------------
char *ChStrDOStoWIN(char *S) {
 for(char *s=S;*s;s++) *s=ChConvChrDOStoWIN(*s); return S;
}
//---------------------------------------------------------------------------
wchar_t ConvChrDOStoWIN(wchar_t B) { // unsigned wchar_t ???
	if (B > 127 && B < 176)
		B += 64;
	else if (B > 223 && B < 240)
		B += 16;
	else
		switch(B) {
		case 179:
			B = L'|';
			break; // ¦
		case 180:
			B = L'+';
			break; // ¦
		case 191:
			B = L'+';
			break; // ¦
		case 192:
			B = L'+';
			break; // ¦
		case 193:
			B = L'+';
			break; // ¦
		case 194:
			B = L'+';
			break; // ¦
		case 195:
			B = L'+';
			break; // ¦
		case 196:
			B = L'-';
			break; // ¦
		case 197:
			B = L'+';
			break; // ¦
		case 217:
			B = L'+';
			break; // ¦
		case 218:
			B = L'+';
			break; // ¦
		case 240:
			B = 168;
			break; // Ё
		case 241:
			B = 184;
			break; // ё
		case 242:
			B = 170;
			break; // Є
		case 243:
			B = 186;
			break; // є
		case 244:
			B = 175;
			break; // Ї
		case 245:
			B = 191;
			break; // ї
		case 248:
			B = 176;
			break; // градус
			// case 251: B=1??; break; //корень
		case 252:
			B = 185;
			break; // номер
		case 253:
			B = 164;
			break; // солнышко
		}
	return B;
}

// ---------------------------------------------------------------------------
wchar_t ConvChrWINtoDOS(wchar_t B) {
	if (B > 191 && B < 240)
		B -= 64;
	else if (B > 239)
		B -= 16;
	else
		switch(B) {
		case 178:
			B = 73;
			break; // I
		case 179:
			B = 105;
			break; // i
		case 168:
			B = 240;
			break; // Ё
		case 184:
			B = 241;
			break; // ё
		case 170:
			B = 242;
			break; // Є
		case 186:
			B = 243;
			break; // є
		case 175:
			B = 244;
			break; // Ї
		case 191:
			B = 245;
			break; // ї
		case 176:
			B = 248;
			break; // градус
		case 185:
			B = 252;
			break; // номер
		case 164:
			B = 253;
			break; // солнышко
		}
	return B;
}

// ---------------------------------------------------------------------------
wchar_t ConvChrWINtoPRN(wchar_t B) {
	if (B > 191 && B < 240)
		B -= 64;
	else if (B > 239)
		B -= 16;
	else
		switch(B) {
		case 178:
			B = 73;
			break; // I
		case 179:
			B = 105;
			break; // i
		case 168:
			B = 240;
			break; // Ё
		case 184:
			B = 241;
			break; // ё
		case 170:
			B = 242;
			break; // Є
		case 186:
			B = 243;
			break; // є
		case 175:
			B = 244;
			break; // Ї
		case 191:
			B = 245;
			break; // ї
		case 176:
			B = 248;
			break; // градус
		case 185:
			B = 252;
			break; // номер
		case 164:
			B = 253;
			break; // солнышко
		}
	return B;
}

// ---------------------------------------------------------------------------
wchar_t *StrDOStoWIN(wchar_t *S) {
	for (wchar_t*s = S; *s; s++)
		* s = ConvChrDOStoWIN(*s);
	return S;
}

// ---------------------------------------------------------------------------
wchar_t *StrWINtoDOS(wchar_t *S) {
	for (wchar_t*s = S; *s; s++)
		* s = ConvChrWINtoDOS(*s);
	return S;
}

// ---------------------------------------------------------------------------
wchar_t *StrWINtoPRN(wchar_t *S) {
	for (wchar_t*s = S; *s; s++)
		* s = ConvChrWINtoPRN(*s);
	return S;
}

// ---------------------------------------------------------------------------
double Round(double D, double R) {
	return R*(D >= 0.0 ? floor(D / R + 0.5) : ceil(D / R - 0.5));
}

// ---------------------------------------------------------------------------
double Round(double D) {
	return Round(D, 1.0);
}

// ---------------------------------------------------------------------------
wchar_t *Fgets(wchar_t *s, int n, FILE *stream) {
	wchar_t *Dest = fgetws(s, n, stream);
	return Dest ? StrDOStoWIN(s) : NULL;
}

// ---------------------------------------------------------------------------
int Fputs(wchar_t *s, FILE *stream) {
	int Rez = 0;
	wchar_t Buf[2048];
	if (s && *s) {
		wcscpy(Buf, s);
		Rez = fputws(StrWINtoDOS(Buf), stream);
	}
	return Rez;
}

// ---------------------------------------------------------------------------
int Fgetc(FILE *F) {
	return(int)ConvChrDOStoWIN((wchar_t)fgetwc(F));
}

// ---------------------------------------------------------------------------
int Fputc(wchar_t B, FILE *F) {
	return fputwc((int)ConvChrWINtoDOS(B), F);
}

// ---------------------------------------------------------------------------
wchar_t IntToChar(int i) {
	if (i < 36)
		return(wchar_t)(i + (i < 10 ? 48 : 87));
	switch(i) {
	case 36:
		return L'#';
	case 37:
		return L'$';
	case 38:
		return L'%';
	case 39:
		return L'^';
	case 40:
		return L'&';
	case 41:
		return L'(';
	case 42:
		return L')';
	case 43:
		return L'-';
	case 44:
		return L'_';
	case 45:
		return L'@';
	case 46:
		return L'!';
	case 47:
		return L'"';
	case 48:
		return L'~';
	default:
		return L'0';
	}
}

// ---------------------------------------------------------------------------
int CharToInt(wchar_t C) {
	if (C >= L'0' && C <= L'9')
		return(int)(C - L'0');
	if (C >= L'a' && C <= L'z')
		return(int)(C - L'W');
	if (C >= L'A' && C <= L'Z')
		return(int)(C - L'A') + 10;
	switch(C) {
	case L'#':
		return 36;
	case L'$':
		return 37;
	case L'%':
		return 38;
	case L'^':
		return 39;
	case L'&':
		return 40;
	case L'(':
		return 41;
	case L')':
		return 42;
	case L'-':
		return 43;
	case L'_':
		return 44;
	case L'@':
		return 45;
	case L'!':
		return 46;
	case L'"':
		return 47;
	case L'~':
		return 48;
	default:
		return 0;
	}
}

// ---------------------------------------------------------------------------
int KSimbStr(wchar_t *S, wchar_t C) {
	int K = 0;
	while (*S)
		if (*S++ == C)
			K++;
	return K;
}

// ---------------------------------------------------------------------------
bool isLargeCiril(wchar_t c) {
	switch(c) {
	case L'А':
		return true;
	case L'Б':
		return true;
	case L'В':
		return true;
	case L'Г':
		return true;
	case L'Д':
		return true;
	case L'Е':
		return true;
	case L'Ё':
		return true;
	case L'Ж':
		return true;
	case L'З':
		return true;
	case L'И':
		return true;
	case L'Й':
		return true;
	case L'К':
		return true;
	case L'Л':
		return true;
	case L'М':
		return true;
	case L'Н':
		return true;
	case L'О':
		return true;
	case L'П':
		return true;
	case L'Р':
		return true;
	case L'С':
		return true;
	case L'Т':
		return true;
	case L'У':
		return true;
	case L'Ф':
		return true;
	case L'Х':
		return true;
	case L'Ц':
		return true;
	case L'Ч':
		return true;
	case L'Ш':
		return true;
	case L'Щ':
		return true;
	case L'Ъ':
		return true;
	case L'Ы':
		return true;
	case L'Ь':
		return true;
	case L'Э':
		return true;
	case L'Ю':
		return true;
	case L'Я':
		return true;
	case L'І':
		return true;
	case L'Є':
		return true;
	case L'Ї':
		return true;
	}
	return false;
}

// ---------------------------------------------------------------------------
bool isLargeLatin(wchar_t c) {
	switch(c) {
	case L'A':
		return true;
	case L'B':
		return true;
	case L'C':
		return true;
	case L'D':
		return true;
	case L'E':
		return true;
	case L'F':
		return true;
	case L'G':
		return true;
	case L'H':
		return true;
	case L'I':
		return true;
	case L'J':
		return true;
	case L'K':
		return true;
	case L'L':
		return true;
	case L'M':
		return true;
	case L'N':
		return true;
	case L'O':
		return true;
	case L'P':
		return true;
	case L'Q':
		return true;
	case L'R':
		return true;
	case L'S':
		return true;
	case L'T':
		return true;
	case L'U':
		return true;
	case L'V':
		return true;
	case L'W':
		return true;
	case L'X':
		return true;
	case L'Y':
		return true;
	case L'Z':
		return true;
	}
	return false;
}

// ---------------------------------------------------------------------------
bool isLarge(wchar_t c) {
	return isLargeLatin(c) || isLargeCiril(c);
}

// ---------------------------------------------------------------------------
bool isciril(wchar_t c) {
	switch(c) {
	case L'А':
		return true;
	case L'Б':
		return true;
	case L'В':
		return true;
	case L'Г':
		return true;
	case L'Д':
		return true;
	case L'Е':
		return true;
	case L'Ё':
		return true;
	case L'Ж':
		return true;
	case L'З':
		return true;
	case L'И':
		return true;
	case L'Й':
		return true;
	case L'К':
		return true;
	case L'Л':
		return true;
	case L'М':
		return true;
	case L'Н':
		return true;
	case L'О':
		return true;
	case L'П':
		return true;
	case L'Р':
		return true;
	case L'С':
		return true;
	case L'Т':
		return true;
	case L'У':
		return true;
	case L'Ф':
		return true;
	case L'Х':
		return true;
	case L'Ц':
		return true;
	case L'Ч':
		return true;
	case L'Ш':
		return true;
	case L'Щ':
		return true;
	case L'Ъ':
		return true;
	case L'Ы':
		return true;
	case L'Ь':
		return true;
	case L'Э':
		return true;
	case L'Ю':
		return true;
	case L'Я':
		return true;
	case L'І':
		return true;
	case L'Є':
		return true;
	case L'Ї':
		return true;
	case L'а':
		return true;
	case L'б':
		return true;
	case L'в':
		return true;
	case L'г':
		return true;
	case L'д':
		return true;
	case L'е':
		return true;
	case L'ё':
		return true;
	case L'ж':
		return true;
	case L'з':
		return true;
	case L'и':
		return true;
	case L'й':
		return true;
	case L'к':
		return true;
	case L'л':
		return true;
	case L'м':
		return true;
	case L'н':
		return true;
	case L'о':
		return true;
	case L'п':
		return true;
	case L'р':
		return true;
	case L'с':
		return true;
	case L'т':
		return true;
	case L'у':
		return true;
	case L'ф':
		return true;
	case L'х':
		return true;
	case L'ц':
		return true;
	case L'ч':
		return true;
	case L'ш':
		return true;
	case L'щ':
		return true;
	case L'ъ':
		return true;
	case L'ы':
		return true;
	case L'ь':
		return true;
	case L'э':
		return true;
	case L'ю':
		return true;
	case L'я':
		return true;
	case L'№':
		return true;
	case L'і':
		return true;
	case L'є':
		return true;
	case L'ї':
		return true; // case L'\'': return true;
	default:
		return false;
	}
}

// ---------------------------------------------------------------------------
struct FileMasStr {
	FileMasStr *Sled;
	wchar_t *File, **MasStr;
	int K;
}*RootFMS;

// ---------------------------------------------------------------------------
int FindFile(wchar_t *NameFile, FileMasStr **F) {
	if (!(*F = RootFMS))
		return 0;
	if (wcscmp(NameFile, RootFMS->File) < 0) {
		*F = NULL;
		return 0;
	}
	for (; ; *F = (*F)->Sled) {
		if (!wcscmp((*F)->File, NameFile))
			return 1;
		if (!((*F)->Sled))
			return 0;
		if (wcscmp((*F)->Sled->File, NameFile) > 0)
			return 0;
	}
}

// ---------------------------------------------------------------------------
FileMasStr *TakeFileMasStr(wchar_t *NameFile) {
	FileMasStr *F, *f;
	int i = 0, K = 0;
	FILE *Fi;
	wchar_t S[1024], *s;
	if (FindFile(NameFile, &F))
		return F;
	f = (FileMasStr*)calloc(1, sizeof(FileMasStr));
	f->File = (wchar_t*)calloc(wcslen(NameFile)+1,sizeof(wchar_t));
	wcscpy(f->File,NameFile);
	if (_waccess(NameFile, 0)) {
		switch(LANGUAGELITERA) {
		case L'U':
			swprintf(Inf, L"Не знайдено файлу текстових повідомлень\n<%s>",
				NameFile);
			Application->MessageBox(Inf, L"Системна помилка!", MB_ICONERROR);
			break;
		case L'E':
			swprintf(Inf, L"The file of text messages is not found\n<%s>",
				NameFile);
			Application->MessageBox(Inf, L"System error!", MB_ICONERROR);
			break;
		default:
			swprintf(Inf, L"Не найден файл текстовых сообщений\n<%s>",
				NameFile);
			Application->MessageBox(Inf, L"Системная ошибка!", MB_ICONERROR);
		}
		return NULL;
	}
	if (!(Fi = _wfopen(NameFile, L"r"))) {
		switch(LANGUAGELITERA) {
		case L'U':
			swprintf(Inf, L"Не відкрито файлу текстових повідомлень\n<%s>",
				NameFile);
			Application->MessageBox(Inf, L"Системна помилка!", MB_ICONERROR);
			break;
		case L'E':
			swprintf(Inf, L"The file of text messages is not opened\n<%s>",
				NameFile);
			Application->MessageBox(Inf, L"System error!", MB_ICONERROR);
			break;
		default:
			swprintf(Inf, L"Не открыт файл текстовых сообщений\n<%s>",
				NameFile);
			Application->MessageBox(Inf, L"Системная ошибка!", MB_ICONERROR);
		}
		return NULL;
	}
	while (fgetws(S, 1023, Fi))
		K++;
	f->MasStr = (wchar_t**)calloc(K, sizeof(wchar_t*));
	f->K = K;
	rewind(Fi);
	while (fgetws(S, 1023, Fi)) {
	 if (s = wcsrchr(S, L'\n')) *s = L'\0';
	 f->MasStr[i++]=wcscpy((wchar_t*)calloc(wcslen(S)+1,sizeof(wchar_t)),S);
	}
	fclose(Fi);
	if (!F) {
	 f->Sled = RootFMS;
	 return RootFMS = f;
	}
	else {
		f->Sled = F->Sled;
		return F->Sled = f;
	}
}

// ---------------------------------------------------------------------------
wchar_t *TextFromFile(wchar_t *Dir,wchar_t *NameFile,
  TStringList **SL,int N){
 if(!(*SL)){
  wchar_t File[256];
  *SL = new TStringList;
  if(Dir&&*Dir&&!_waccess(Dir,0))
   swprintf(File,L"%s\\%s%c.txt",Dir,NameFile,LANGUAGELITERA);
  else
   swprintf(File,L"%s%c.txt",NameFile,LANGUAGELITERA);
  if(!_waccess(File,0))
   (*SL)->LoadFromFile(File);
  else{
   switch(LANGUAGELITERA){
	case 'U':
	 swprintf(Inf,L"Файлу <%sU> не існує",NameFile);
	 Application->MessageBox(Inf,L"Системна помилка!",MB_OK);
	 break;
	case 'E':
	 swprintf(Inf,L"File <%sE> does not exist",NameFile,N);
	 Application->MessageBox(Inf,L"System error!",MB_OK);
	 break;
	default:
	 swprintf(Inf,L"Файл <%sR> не существует",NameFile);
	 Application->MessageBox(Inf,L"!",MB_OK);
   }
   return NULL;
  }
  if((*SL)->Count<N){
   switch(LANGUAGELITERA){
	case 'U':
	 swprintf(Inf,L"В файлі <%sU> немає стрічки за номером - <%d>",NameFile,N);
	 Application->MessageBox(Inf,L"Системна помилка!",MB_OK);
	 break;
	case 'E':
	 swprintf(Inf,L"In the file <%sE> there is no line number - <%d>",NameFile,N);
	 Application->MessageBox(Inf,L"System error!",MB_OK);
	 break;
	default:
	 swprintf(Inf,L"В файле <%sR> нет строки номер - <%d>",NameFile,N);
	 Application->MessageBox(Inf,L"Системная ошибка!",MB_OK);
   }
   return NULL;
 }}
 return (*SL)->Strings[N-1].w_str();
}
// ---------------------------------------------------------------------------
wchar_t *ToLat(wchar_t *S) {
	wchar_t *s = S;
	while (*s) {
		if (isciril(*s))
			switch(*s) {
			case L'А':
				*s = L'A';
				break;
			case L'П':
				*s = L'P';
				break;
			case L'Б':
				*s = L'B';
				break;
			case L'Р':
				*s = L'R';
				break;
			case L'В':
				*s = L'V';
				break;
			case L'С':
				*s = L'S';
				break;
			case L'Г':
				*s = L'G';
				break;
			case L'Т':
				*s = L'T';
				break;
			case L'Д':
				*s = L'D';
				break;
			case L'У':
				*s = L'U';
				break;
			case L'Е':
				*s = L'E';
				break;
			case L'Ф':
				*s = L'F';
				break;
			case L'Ё':
				*s = L'E';
				break;
			case L'Х':
				*s = L'H';
				break;
			case L'Ж':
				*s = L'G';
				break;
			case L'Ц':
				*s = L'C';
				break;
			case L'З':
				*s = L'Z';
				break;
			case L'Ч':
				*s = L'C';
				break;
			case L'И':
				*s = L'I';
				break;
			case L'Ш':
				*s = L'S';
				break;
			case L'Й':
				*s = L'J';
				break;
			case L'Щ':
				*s = L'S';
				break;
			case L'К':
				*s = L'K';
				break;
			case L'Ы':
				*s = L'Y';
				break;
			case L'Л':
				*s = L'L';
				break;
			case L'Ь':
				*s = L'Y';
				break;
			case L'М':
				*s = L'M';
				break;
			case L'Ъ':
				*s = L'X';
				break;
			case L'Н':
				*s = L'N';
				break;
			case L'Э':
				*s = L'E';
				break;
			case L'О':
				*s = L'O';
				break;
			case L'Ю':
				*s = L'U';
				break;
			case L'Я':
				*s = L'A';
				break;
			case L'І':
				*s = L'I';
				break;
			case L'Ї':
				*s = L'I';
				break;
			case L'Є':
				*s = L'E';
				break;
			case L'а':
				*s = L'a';
				break;
			case L'п':
				*s = L'p';
				break;
			case L'б':
				*s = L'b';
				break;
			case L'р':
				*s = L'r';
				break;
			case L'в':
				*s = L'v';
				break;
			case L'с':
				*s = L's';
				break;
			case L'г':
				*s = L'g';
				break;
			case L'т':
				*s = L't';
				break;
			case L'д':
				*s = L'd';
				break;
			case L'у':
				*s = L'u';
				break;
			case L'е':
				*s = L'e';
				break;
			case L'ф':
				*s = L'f';
				break;
			case L'ё':
				*s = L'e';
				break;
			case L'х':
				*s = L'h';
				break;
			case L'ж':
				*s = L'g';
				break;
			case L'ц':
				*s = L'c';
				break;
			case L'з':
				*s = L'z';
				break;
			case L'ч':
				*s = L'c';
				break;
			case L'и':
				*s = L'i';
				break;
			case L'ш':
				*s = L's';
				break;
			case L'й':
				*s = L'j';
				break;
			case L'щ':
				*s = L's';
				break;
			case L'к':
				*s = L'k';
				break;
			case L'ы':
				*s = L'y';
				break;
			case L'л':
				*s = L'l';
				break;
			case L'ь':
				*s = L'y';
				break;
			case L'м':
				*s = L'm';
				break;
			case L'ъ':
				*s = L'x';
				break;
			case L'н':
				*s = L'n';
				break;
			case L'э':
				*s = L'e';
				break;
			case L'о':
				*s = L'o';
				break;
			case L'ю':
				*s = L'u';
				break;
			case L'я':
				*s = L'a';
				break;
			case L'і':
				*s = L'i';
				break;
			case L'ї':
				*s = L'i';
				break;
			case L'є':
				*s = L'e';
				break;
			}
		s++;
	}
	return S;
}

// ---------------------------------------------------------------------------
wchar_t *LowToUpp(wchar_t *S) {
	for (wchar_t*s = S; *s; s++) {
		if (isalpha(*s)) {
			if (*s > 96)
				* s -= 32;
		}
		else {
			if (isciril(*s))
				switch(*s) {
				case L'а':
					*s = L'А';
					break;
				case L'п':
					*s = L'П';
					break;
				case L'б':
					*s = L'Б';
					break;
				case L'р':
					*s = L'Р';
					break;
				case L'в':
					*s = L'В';
					break;
				case L'с':
					*s = L'С';
					break;
				case L'г':
					*s = L'Г';
					break;
				case L'т':
					*s = L'Т';
					break;
				case L'д':
					*s = L'Д';
					break;
				case L'у':
					*s = L'У';
					break;
				case L'е':
					*s = L'Е';
					break;
				case L'ф':
					*s = L'Ф';
					break;
				case L'ё':
					*s = L'Ё';
					break;
				case L'х':
					*s = L'Х';
					break;
				case L'ж':
					*s = L'Ж';
					break;
				case L'ц':
					*s = L'Ц';
					break;
				case L'з':
					*s = L'З';
					break;
				case L'ч':
					*s = L'Ч';
					break;
				case L'и':
					*s = L'И';
					break;
				case L'ш':
					*s = L'Ш';
					break;
				case L'й':
					*s = L'Й';
					break;
				case L'щ':
					*s = L'Щ';
					break;
				case L'к':
					*s = L'К';
					break;
				case L'ы':
					*s = L'Ы';
					break;
				case L'л':
					*s = L'Л';
					break;
				case L'ь':
					*s = L'Ь';
					break;
				case L'м':
					*s = L'М';
					break;
				case L'ъ':
					*s = L'Ъ';
					break;
				case L'н':
					*s = L'Н';
					break;
				case L'э':
					*s = L'Э';
					break;
				case L'о':
					*s = L'О';
					break;
				case L'ю':
					*s = L'Ю';
					break;
				case L'я':
					*s = L'Я';
					break;
				}
		}
	}
	return S;
}

// ---------------------------------------------------------------------------
wchar_t *UppToLow(wchar_t *S) {
	for (wchar_t*s = S; *s; s++) {
		if (isalpha(*s)) {
			if (*s <= 96)
				* s += 32;
		}
		else {
			if (isciril(*s))
				switch(*s) {
				case L'А':
					*s = L'а';
					break;
				case L'П':
					*s = L'п';
					break;
				case L'Б':
					*s = L'б';
					break;
				case L'Р':
					*s = L'р';
					break;
				case L'В':
					*s = L'в';
					break;
				case L'С':
					*s = L'с';
					break;
				case L'Г':
					*s = L'г';
					break;
				case L'Т':
					*s = L'т';
					break;
				case L'Д':
					*s = L'д';
					break;
				case L'У':
					*s = L'у';
					break;
				case L'Е':
					*s = L'е';
					break;
				case L'Ф':
					*s = L'ф';
					break;
				case L'Ё':
					*s = L'ё';
					break;
				case L'Х':
					*s = L'х';
					break;
				case L'Ж':
					*s = L'ж';
					break;
				case L'Ц':
					*s = L'ц';
					break;
				case L'З':
					*s = L'з';
					break;
				case L'Ч':
					*s = L'ч';
					break;
				case L'И':
					*s = L'и';
					break;
				case L'Ш':
					*s = L'ш';
					break;
				case L'Й':
					*s = L'й';
					break;
				case L'Щ':
					*s = L'щ';
					break;
				case L'К':
					*s = L'к';
					break;
				case L'Ы':
					*s = L'ы';
					break;
				case L'Л':
					*s = L'л';
					break;
				case L'Ь':
					*s = L'ь';
					break;
				case L'М':
					*s = L'м';
					break;
				case L'Ъ':
					*s = L'ъ';
					break;
				case L'Н':
					*s = L'н';
					break;
				case L'Э':
					*s = L'э';
					break;
				case L'О':
					*s = L'о';
					break;
				case L'Ю':
					*s = L'ю';
					break;
				case L'Я':
					*s = L'я';
					break;
				}
		}
	}
	return S;
}
// ---------------------------------------------------------------------------
int max(int a, int b) {
	return a > b ? a : b;
}

// ---------------------------------------------------------------------------
int min(int a, int b) {
	return a > b ? b : a;
}

// ---------------------------------------------------------------------------
wchar_t *NameToFile(wchar_t *S) {
	wchar_t Buf[256], *b = Buf, *s = S;
	for (; *s; s++) {
		switch(*s) {
		case L'.':
			*b++ = L'_';
		case L'/':
			*b++ = L'_';
		case L' ':
		case L',':
			break;
		default:
			*b++ = *s;
		}
	}
	*b = L'\0';
	ToLat(Buf);
	wcscpy(S, Buf);
	return S;
}

// ---------------------------------------------------------------------------
wchar_t *NameToFileRus(wchar_t *S) {
	wchar_t Buf[256], *b = Buf, *s = S;
	for (; *s; s++) {
		switch(*s) {
		case L'.':
			*b++ = L'_';
		case L'/':
			*b++ = L'_';
		case L' ':
		case L',':
			break;
		default:
			*b++ = *s;
		}
	}
	*b = L'\0';
	wcscpy(S, Buf);
	return S;
}

// ---------------------------------------------------------------------------
void Unlink(wchar_t *File) {
	int Rez;
	FILE *F = _wfopen(File, L"w");
	if (!F) {
		swprintf(Inf,
			L"Файл <%s> не может быть очищен.\n" L"Информация будет добавлена.", File);
		Application->MessageBox(Inf, L"Системная ошибка!",
			MB_OK | MB_ICONWARNING);
	}
	fclose(F);
}

// ---------------------------------------------------------------------------
bool GrandToLitlSimbKiril(wchar_t &B) { // возврат true, если была замена
	switch(B) {
	case L'А':
		B = L'а';
		break;
	case L'К':
		B = L'к';
		break;
	case L'Х':
		B = L'х';
		break;
	case L'Б':
		B = L'б';
		break;
	case L'Л':
		B = L'л';
		break;
	case L'Ц':
		B = L'ц';
		break;
	case L'В':
		B = L'в';
		break;
	case L'М':
		B = L'м';
		break;
	case L'Ч':
		B = L'ч';
		break;
	case L'Г':
		B = L'г';
		break;
	case L'Н':
		B = L'н';
		break;
	case L'Ш':
		B = L'ш';
		break;
	case L'Д':
		B = L'д';
		break;
	case L'О':
		B = L'о';
		break;
	case L'Щ':
		B = L'щ';
		break;
	case L'Е':
		B = L'е';
		break;
	case L'П':
		B = L'п';
		break;
	case L'Ы':
		B = L'ы';
		break;
	case L'Ё':
		B = L'ё';
		break;
	case L'Р':
		B = L'р';
		break;
	case L'Ъ':
		B = L'ъ';
		break;
	case L'Ж':
		B = L'ж';
		break;
	case L'С':
		B = L'с';
		break;
	case L'Ь':
		B = L'ь';
		break;
	case L'З':
		B = L'з';
		break;
	case L'Т':
		B = L'т';
		break;
	case L'Э':
		B = L'э';
		break;
	case L'И':
		B = L'и';
		break;
	case L'У':
		B = L'у';
		break;
	case L'Ю':
		B = L'ю';
		break;
	case L'Й':
		B = L'й';
		break;
	case L'Ф':
		B = L'ф';
		break;
	case L'Я':
		B = L'я';
		break;
	default:
		return false;
	}
	return true;
}

// ---------------------------------------------------------------------------
bool LitlToGrandSimbKiril(wchar_t &B) { // возврат true, если была замена
	switch(B) {
	case L'а':
		B = L'А';
		break;
	case L'к':
		B = L'К';
		break;
	case L'х':
		B = L'Х';
		break;
	case L'б':
		B = L'Б';
		break;
	case L'л':
		B = L'Л';
		break;
	case L'ц':
		B = L'Ц';
		break;
	case L'в':
		B = L'В';
		break;
	case L'м':
		B = L'М';
		break;
	case L'ч':
		B = L'Ч';
		break;
	case L'г':
		B = L'Г';
		break;
	case L'н':
		B = L'Н';
		break;
	case L'ш':
		B = L'Ш';
		break;
	case L'д':
		B = L'Д';
		break;
	case L'о':
		B = L'О';
		break;
	case L'щ':
		B = L'Щ';
		break;
	case L'е':
		B = L'Е';
		break;
	case L'п':
		B = L'П';
		break;
	case L'ы':
		B = L'Ы';
		break;
	case L'ё':
		B = L'Ё';
		break;
	case L'р':
		B = L'Р';
		break;
	case L'ъ':
		B = L'Ъ';
		break;
	case L'ж':
		B = L'Ж';
		break;
	case L'с':
		B = L'С';
		break;
	case L'ь':
		B = L'Ь';
		break;
	case L'з':
		B = L'З';
		break;
	case L'т':
		B = L'Т';
		break;
	case L'э':
		B = L'Э';
		break;
	case L'и':
		B = L'И';
		break;
	case L'у':
		B = L'У';
		break;
	case L'ю':
		B = L'Ю';
		break;
	case L'й':
		B = L'Й';
		break;
	case L'ф':
		B = L'Ф';
		break;
	case L'я':
		B = L'Я';
		break;
	default:
		return false;
	}
	return true;
}

// ---------------------------------------------------------------------------
void CopyFromArhiv(wchar_t *Arhiv, wchar_t *File) {
	long Ta = 0l, Tf = 0l; AnsiString ArhFileA,FileA;
	struct _stat St; wchar_t ArhFile[256], *t, Bak[256];
	swprintf(ArhFile, L"%s\\%s", Arhiv, File);
	if (!_waccess(ArhFile, 0)) {
		_wstat(ArhFile, &St);
		Ta = St.st_mtime;
	}
	if (!_waccess(File, 0)) {
		_wstat(File, &St);
		Tf = St.st_mtime;
	}
	if (Ta > Tf) {
		if (!_waccess(File, 0)) {
			wcscpy(Bak, File);
			if (t = wcsrchr(Bak, L'.'))
				wcscpy(t + 1, L"bak");
			else
				wcscat(Bak, L".bak");
			_wunlink(Bak);
			_wrename(File, Bak);
		}
		ArhFileA=ArhFile; FileA=File;
		CopyFile(ArhFileA.c_str(),FileA.c_str(),false);
	}
}

// ---------------------------------------------------------------------------
void CopyToArhiv(wchar_t *Arhiv, wchar_t *File) {
	long Ta = 0l, Tf = 0l; AnsiString ArhFileA,FileA;
	struct _stat St; wchar_t ArhFile[256], *t, Bak[256];
	swprintf(ArhFile, L"%s\\%s", Arhiv, File);
	if (!_waccess(ArhFile, 0)) {
		_wstat(ArhFile, &St);
		Ta = St.st_mtime;
	}
	if (!_waccess(File, 0)) {
		_wstat(File, &St);
		Tf = St.st_mtime;
	}
	if (Tf > Ta) {
		if (!_waccess(ArhFile, 0)) {
			wcscpy(Bak, ArhFile);
			if (t = wcsrchr(Bak, L'.'))
				wcscpy(t + 1, L"bak");
			else
				wcscat(Bak, L".bak");
			_wunlink(Bak);
			_wrename(ArhFile, Bak);
		}
		ArhFileA=ArhFile; FileA=File;
		CopyFile(ArhFileA.c_str(),FileA.c_str(),false);
	}
}

// ---------------------------------------------------------------------------
bool CreateDir(wchar_t *Dir) {
	if (_waccess(Dir, 0)) {
		wchar_t NadDir[256], *s;
		wcscpy(NadDir, Dir);
		if (s = wcsrchr(NadDir, L'\\')) {
			*s = L'\0';
			if (CreateDir(NadDir)) {
				*s = L'\\';
				return !_wmkdir(Dir);
			}
			*s = L'\\';
		}
		return false;
	}
	return true;
}

// ---------------------------------------------------------------------------
void Pack(wchar_t *Kod, wchar_t *Path, wchar_t *Arc, wchar_t *File) {
	wchar_t Arhiv[256], Dir[256];
	if (_waccess(Path, 0)) {
		_wgetcwd(Dir, 255);
		_wmkdir(Path);
		_wchdir(Dir);
	}
	swprintf(Arhiv, L"%s\\%s", Path, Arc);
	_wspawnl(P_WAIT, LHA, LHA, Kod, Arhiv, File, NULL);
}

// ---------------------------------------------------------------------------
int LLat(TCanvas *Canvas, wchar_t *S, wchar_t b) {
	int Size = Canvas->Font->Size, L = 0;
	wchar_t *s, C = L'\0', B[2];
	if (!isdigit(S[0])) {
		for (s = S; *s; s++)
			if (isdigit(*s) && (s[-1] != L'[') && !isdigit(s[-1])) {
				C = *s;
				*s = L'\0';
				break;
			}
	}
	if (!Canvas->Font->Style.Contains(fsItalic)) {
		L += Canvas->TextWidth(S);
	}
	else
		L += Canvas->TextWidth(S);
	if (C) {
		Canvas->Font->Size = Round(3 * Canvas->Font->Size / 5);
		*s = C;
		L += Canvas->TextWidth(s);
		Canvas->Font->Size = Size;
	}
	B[0] = b;
	B[1] = L'\0';
	L += Canvas->TextWidth(B);
	return L;
}

// ---------------------------------------------------------------------------
int LGrekLat(TCanvas *Canvas, int Kod, wchar_t *s, wchar_t B) {
	AnsiString NameFont = Canvas->Font->Name, S = wchar_t(Kod);
	int L;
	Canvas->Font->Name = "Symbol";
	L = Canvas->TextWidth(S);
	Canvas->Font->Name = NameFont;
	S = s;
	S += B;
	L += Canvas->TextWidth(S);
	return L;
}

// ---------------------------------------------------------------------------
int LGrek(TCanvas *Canvas, int Kod, bool Index) {
	AnsiString NameFont = Canvas->Font->Name, S = wchar_t(Kod);
	int L, Size = Canvas->Font->Size;
	bool ExFont = Canvas->Font->Style.Contains(fsBold),
	ItFont = Canvas->Font->Style.Contains(fsItalic),
	UnFont = Canvas->Font->Style.Contains(fsUnderline);
	if (ExFont)
		Canvas->Font->Style = TFontStyles() >> fsBold;
	if (ItFont)
		Canvas->Font->Style = TFontStyles() >> fsItalic;
	if (UnFont)
		Canvas->Font->Style = TFontStyles() >> fsUnderline;
	Canvas->Font->Name = "Symbol";
	if (Index)
		Canvas->Font->Size = Round(2.0 * Size / 3.0);
	L = Canvas->TextWidth(S);
	if (ExFont)
		Canvas->Font->Style = TFontStyles() << fsBold;
	if (ItFont)
		Canvas->Font->Style = TFontStyles() << fsItalic;
	if (UnFont)
		Canvas->Font->Style = TFontStyles() << fsUnderline;
	Canvas->Font->Name = NameFont;
	if (Index)
		Canvas->Font->Size = Size;
	return L;
}

// ---------------------------------------------------------------------------
int LLiter(TCanvas *Canvas, wchar_t B, bool Index) {
	AnsiString S = B;
	int L, Size = Canvas->Font->Size;
	bool ExFont = Canvas->Font->Style.Contains(fsBold),
	ItFont = Canvas->Font->Style.Contains(fsItalic),
	UnFont = Canvas->Font->Style.Contains(fsUnderline);
	if (ExFont)
		Canvas->Font->Style = TFontStyles() >> fsBold;
	if (ItFont && (isciril(B) || IsCifra(B)))
		Canvas->Font->Style = TFontStyles() >> fsItalic;
	if (UnFont)
		Canvas->Font->Style = TFontStyles() >> fsUnderline;
	if (Index)
		Canvas->Font->Size = Round(2.0 * Size / 3.0);
	L = Canvas->TextWidth(S);
	if (ExFont)
		Canvas->Font->Style = TFontStyles() << fsBold;
	if (ItFont && (isciril(B) || IsCifra(B)))
		Canvas->Font->Style = TFontStyles() << fsItalic;
	if (UnFont)
		Canvas->Font->Style = TFontStyles() << fsUnderline;
	if (Index)
		Canvas->Font->Size = Size;
	return L;
}

// ---------------------------------------------------------------------------
int DrawLat(TCanvas *Canvas, int Left, int Top, wchar_t *S, wchar_t b) {
	int Size = Canvas->Font->Size, H = -Canvas->Font->Height;
	wchar_t *s, C = L'\0', B[2];
	if (!isdigit(S[0]) && ((S[0] != L'-') || !isdigit(S[1]))) {
		for (s = S; *s; s++)
			if (isdigit(*s) && (s[-1] != L'[') && !isdigit(s[-1]) || (*s == L'$')
				) {
				C = *s;
				*s = L'\0';
				break;
			}
	}
	/* if(!Canvas->Font->Style.Contains(fsItalic)){  //латиницу кусивом
	Canvas->Font->Style=TFontStyles()<<fsItalic;
	Canvas->TextOut(Left,Top,S); Left+=Canvas->TextWidth(S);
	Canvas->Font->Style=TFontStyles()>>fsItalic;
	}
	else{
	Canvas->TextOut(Left,Top,S); Left+=Canvas->TextWidth(S);
	} */
	Canvas->TextOut(Left, Top, S);
	Left += Canvas->TextWidth(S);
	if (C) {
		Canvas->Font->Size = Round(3 * Canvas->Font->Size / 5);
		*s = C;
		if (*s == L'$')
			s++;
		Canvas->TextOut(Left, Top + Round(H / 2), s);
		Left += Canvas->TextWidth(s);
		Canvas->Font->Size = Size;
	}
	B[0] = b;
	B[1] = L'\0';
	Canvas->TextOut(Left, Top, B);
	Left += Canvas->TextWidth(B);
	return Left;
}

// ---------------------------------------------------------------------------
int KodGrek(wchar_t *S, int &Kod) { // возвращает код и длину греческой буквы
	if (wcsstr(S, L"alfa") == S) {
		Kod = 97;
		return 4;
	}
	if (wcsstr(S, L"alpha") == S) {
		Kod = 97;
		return 5;
	}
	if (wcsstr(S, L"beta") == S) {
		Kod = 98;
		return 4;
	}
	if (wcsstr(S, L"betta") == S) {
		Kod = 98;
		return 5;
	}
	if (wcsstr(S, L"gamma") == S) {
		Kod = 103;
		return 5;
	}
	if (wcsstr(S, L"gama") == S) {
		Kod = 103;
		return 5;
	}
	if (wcsstr(S, L"delta") == S) {
		Kod = 100;
		return 5;
	}
	if (wcsstr(S, L"epsilon") == S) {
		Kod = 101;
		return 7;
	}
	if (wcsstr(S, L"dzeta") == S) {
		Kod = 122;
		return 5;
	}
	if (wcsstr(S, L"dseta") == S) {
		Kod = 122;
		return 5;
	}
	if (wcsstr(S, L"teta") == S) {
		Kod = 113;
		return 4;
	}
	if (wcsstr(S, L"eta") == S) {
		Kod = 104;
		return 3;
	}
	if (wcsstr(S, L"jota") == S) {
		Kod = 105;
		return 4;
	}
	if (wcsstr(S, L"iota") == S) {
		Kod = 105;
		return 4;
	}
	if (wcsstr(S, L"kappa") == S) {
		Kod = 107;
		return 5;
	}
	if (wcsstr(S, L"lamda") == S) {
		Kod = 108;
		return 5;
	}
	if (wcsstr(S, L"lambda") == S) {
		Kod = 108;
		return 6;
	}
	if (wcsstr(S, L"mu") == S) {
		Kod = 109;
		return 2;
	}
	if (wcsstr(S, L"nu") == S) {
		Kod = 110;
		return 2;
	}
	if (wcsstr(S, L"ksi") == S) {
		Kod = 120;
		return 3;
	}
	if (wcsstr(S, L"omicron") == S) {
		Kod = 111;
		return 7;
	}
	if (wcsstr(S, L"omikron") == S) {
		Kod = 111;
		return 7;
	}
	if (wcsstr(S, L"pi") == S) {
		Kod = 112;
		return 2;
	}
	if (wcsstr(S, L"ro") == S) {
		Kod = 114;
		return 2;
	}
	if (wcsstr(S, L"sigma") == S) {
		Kod = 115;
		return 5;
	}
	if (wcsstr(S, L"tau") == S) {
		Kod = 116;
		return 3;
	}
	if (wcsstr(S, L"ipsilon") == S) {
		Kod = 117;
		return 7;
	}
	if (wcsstr(S, L"fi") == S) {
		Kod = 106;
		return 2;
	}
	if (wcsstr(S, L"hi") == S) {
		Kod = 99;
		return 2;
	}
	if (wcsstr(S, L"psi") == S) {
		Kod = 121;
		return 3;
	}
	if (wcsstr(S, L"omega") == S) {
		Kod = 119;
		return 5;
	}
	if (wcsstr(S, L"ALFA") == S) {
		Kod = 65;
		return 4;
	}
	if (wcsstr(S, L"ALPHA") == S) {
		Kod = 65;
		return 5;
	}
	if (wcsstr(S, L"BETA") == S) {
		Kod = 66;
		return 4;
	}
	if (wcsstr(S, L"BETTA") == S) {
		Kod = 66;
		return 5;
	}
	if (wcsstr(S, L"GAMMA") == S) {
		Kod = 71;
		return 5;
	}
	if (wcsstr(S, L"DELTA") == S) {
		Kod = 68;
		return 5;
	}
	if (wcsstr(S, L"EPSILON") == S) {
		Kod = 69;
		return 7;
	}
	if (wcsstr(S, L"DZETA") == S) {
		Kod = 90;
		return 5;
	}
	if (wcsstr(S, L"DSETA") == S) {
		Kod = 90;
		return 5;
	}
	if (wcsstr(S, L"ETA") == S) {
		Kod = 72;
		return 3;
	}
	if (wcsstr(S, L"TETA") == S) {
		Kod = 81;
		return 4;
	}
	if (wcsstr(S, L"JOTA") == S) {
		Kod = 73;
		return 4;
	}
	if (wcsstr(S, L"IOTA") == S) {
		Kod = 73;
		return 4;
	}
	if (wcsstr(S, L"KAPPA") == S) {
		Kod = 75;
		return 5;
	}
	if (wcsstr(S, L"LAMDA") == S) {
		Kod = 76;
		return 5;
	}
	if (wcsstr(S, L"LAMBDA") == S) {
		Kod = 76;
		return 6;
	}
	if (wcsstr(S, L"MU") == S) {
		Kod = 77;
		return 2;
	}
	if (wcsstr(S, L"NU") == S) {
		Kod = 78;
		return 2;
	}
	if (wcsstr(S, L"KSI") == S) {
		Kod = 88;
		return 3;
	}
	if (wcsstr(S, L"OMICRON") == S) {
		Kod = 79;
		return 7;
	}
	if (wcsstr(S, L"OMIKRON") == S) {
		Kod = 79;
		return 7;
	}
	if (wcsstr(S, L"PI") == S) {
		Kod = 80;
		return 2;
	}
	if (wcsstr(S, L"RO") == S) {
		Kod = 82;
		return 2;
	}
	if (wcsstr(S, L"SIGMA") == S) {
		Kod = 83;
		return 5;
	}
	if (wcsstr(S, L"TAU") == S) {
		Kod = 84;
		return 3;
	}
	if (wcsstr(S, L"IPSILON") == S) {
		Kod = 85;
		return 7;
	}
	if (wcsstr(S, L"FI") == S) {
		Kod = 70;
		return 2;
	}
	if (wcsstr(S, L"HI") == S) {
		Kod = 67;
		return 2;
	}
	if (wcsstr(S, L"PSI") == S) {
		Kod = 89;
		return 3;
	}
	if (wcsstr(S, L"OMEGA") == S) {
		Kod = 87;
		return 5;
	}
	Kod = 0;
	return 0;
}

// ---------------------------------------------------------------------------
int LengthGrek(TCanvas *Canvas, wchar_t *S, wchar_t SimbInd) {
	// возвращает длину строки в пикселях
	int Rez = 0, d, Kod;
	bool Index = false;
	for (wchar_t*s = S; *s; ) {
		if (d = KodGrek(s, Kod)) {
			Rez += LGrek(Canvas, Kod, Index);
			s += d;
		}
		else {
			if (*s == SimbInd)
				Index = true;
			else if (*s == L'\'') {
				Index = false;
				Rez += LLiter(Canvas, SimbInd, Index);
			}
			else
				Rez += LLiter(Canvas, *s, Index);
			s++;
		}
	}
	return Rez;
}

// ---------------------------------------------------------------------------
int DrawGrek(TCanvas *Canvas, int Left, int Top, int Kod, bool Index) {
	AnsiString S, NameFont = Canvas->Font->Name;
	int Size = Canvas->Font->Size, L;
	bool ItFont = Canvas->Font->Style.Contains(fsItalic);
	if (ItFont)
		Canvas->Font->Style = TFontStyles() >> fsItalic;
	Canvas->Font->Name = "Symbol";
	if (Index) {
		Top -= Canvas->Font->Height / 2;
		Canvas->Font->Size = Round(2.0 * Size / 3.0);
	}
	else
		Top -= Canvas->Font->Height / 12;
	S = wchar_t(Kod);
	L = Canvas->TextWidth(S);
	Canvas->TextOut(Left, Top, S);
	Canvas->Font->Name = NameFont;
	if (Index)
		Canvas->Font->Size = Size;
	if (ItFont)
		Canvas->Font->Style = TFontStyles() << fsItalic;
	return L;
}

// ---------------------------------------------------------------------------
int DrawCiril(TCanvas *Canvas, int Left, int Top, wchar_t *S, bool Index) {
	int Size = Canvas->Font->Size, L;
	bool ItFont = Canvas->Font->Style.Contains(fsItalic);
	if (ItFont)
		Canvas->Font->Style = TFontStyles() >> fsItalic;
	if (Index) {
		Top -= Round(3.0 * Canvas->Font->Height / 4.0);
		Canvas->Font->Size = Round(3.0 * Size / 4.0);
	}
	L = Canvas->TextWidth(S);
	Canvas->TextOut(Left, Top, S);
	if (ItFont)
		Canvas->Font->Style = TFontStyles() << fsItalic;
	if (Index)
		Canvas->Font->Size = Size;
	return L;
}

// ---------------------------------------------------------------------------
int DrawLat(TCanvas *Canvas, int Left, int Top, wchar_t *S, bool Index) {
	int Size = Canvas->Font->Size, L;
	if (Index) {
		Top -= Canvas->Font->Height / 2;
		Canvas->Font->Size = Round(2.0 * Size / 3.0);
	}
	L = Canvas->TextWidth(S);
	Canvas->TextOut(Left, Top, S);
	if (Index)
		Canvas->Font->Size = Size;
	return L;
}

// ---------------------------------------------------------------------------
void DrawGrekText(TCanvas *Canvas, int Left, int Top, int Width, wchar_t *S,
	wchar_t SimbInd) {
	wchar_t *s, B, Buf[256], *b = Buf;
	int d, Kod, L;
	bool Index = false, Grek = false;
	if (Width > 0) {
		L = LengthGrek(Canvas, S, SimbInd);
		Left += (Width - L) / 2;
	}
	else
		Left += Round(0.5 * Canvas->TextWidth(L"M"));
	for (wchar_t*s = S; *s; ) {
		if (d = KodGrek(s, Kod)) {
			L = DrawGrek(Canvas, Left, Top, Kod, Index);
			Left += Round(5.0 * L / 4.0);
			s += d;
			Grek = true;
		}
		else {
			if (*s == SimbInd)
				Index = true;
			else {
				if (*s == L'\'' || *s == L'(' || *s == L',' || *s == L')') {
					if (Index)
						Index = false;
					B = s[1];
					s[1] = L'\0';
					L = DrawLat(Canvas, Left, Top, s, Index);
					s[1] = B;
				}
				else {
					if (isciril(*s)) {
						while (!KodGrek(s, Kod) && (isciril(*s) || IsCifra(*s)))
							* b++ = *s++;
						*b = L'\0';
						s--;
						L = DrawCiril(Canvas, Left, Top, Buf, Index);
						b = Buf;
					}
					else if (IsCifra(*s)) {
						while (!KodGrek(s, Kod) && IsCifra(*s))
							* b++ = *s++;
						*b = L'\0';
						s--;
						if (Grek) {
							Index = true;
							Grek = false;
						}
						L = DrawCiril(Canvas, Left, Top, Buf, Index);
						Index = false;
						b = Buf;
					}
					else if (isalpha(*s)) {
						while (!KodGrek(s, Kod) && isalpha(*s))
							* b++ = *s++;
						*b = L'\0';
						s--;
						L = DrawLat(Canvas, Left, Top, Buf, Index);
						if (IsCifra(s[1]))
							Index = true;
						b = Buf;
					}
					else {
						B = s[1];
						s[1] = L'\0';
						Grek = false;
						L = DrawCiril(Canvas, Left, Top, s, false);
						s[1] = B;
					}
				}
				Left += L;
			}
			s++;
		}
	}
}

// ---------------------------------------------------------------------------
void DrawGrekText(TCanvas *Canvas, const TRect &Rect, wchar_t *S,
	wchar_t SimbInd) {
	DrawGrekText(Canvas, Rect.Left, Rect.Top, Rect.Width(), S, SimbInd);
}

// ---------------------------------------------------------------------------
wchar_t *CleanStr(wchar_t *SIn, wchar_t *SOut) {
	// очистка строки от пробелов, символов табуляции и перевода строк в начале и в конце строки
	wchar_t *s;
	*SOut = L'\0'; // ДЛЯ ПУЩЕЙ ВАЖНОСТИ ОБНУЛИМ ВЫХОДЯЩУЮ СТРОКУ
	if (!SOut)
		return NULL;
	if (!SIn)
		return SOut;
	while (*SIn&&((*SIn==L' ')||(*SIn==L'\t')||
		  (*SIn==L'\n')))
		SIn++;
	wcscpy(SOut, SIn);
	for (s = SOut + wcslen(SOut) - 1;
		(*s == L' ') || (*s == L'\t') || (*s == L'\n'); s--);
	*++s = L'\0';
	return SOut;
}

// ---------------------------------------------------------------------------
int StrCmp(wchar_t *s1, wchar_t *s2) {
	wchar_t S1[256], S2[256];
	if (!s1)
		if (!s2 || !*s2)
			return 0;
		else
			return -1;
	if (!s2)
		if (!*s1)
			return 0;
		else
			return 1;
	CleanStr(s1, S1);
	CleanStr(s2, S2);
	return wcscmp(S1, S2);
}

// ---------------------------------------------------------------------------
bool CopyFiles(wchar_t *Fi, wchar_t *Fo) {
	wchar_t Sys[1024];
	int Rez=0;
//	Rez = CopyFile(Fi, Fo, false);
	if (!Rez) {
		swprintf(Sys,
			L"Копирование файла <%s>\nв файл <%s> прошло с ошибкой\n" "Код ошибки - %d", Fi, Fo, GetLastError());
		Application->MessageBox(Sys, L"Ошибка!", MB_OK | MB_ICONERROR);
		return false;
	}
	return true;
}

// ---------------------------------------------------------------------------
wchar_t *Pole(wchar_t *S) {
	wchar_t *s;
	for (s = S; *s && (*s != L'\xb3') && (*s != L'\n'); s++);
	*s++ = L'\0';
	return s;
}

// ---------------------------------------------------------------------------
bool AllCifry(wchar_t *S) {
	if (!S)
		return true;
	while (*S && isdigit(*S))
		S++;
	if (*S)
		return false;
	return true;
}

// ---------------------------------------------------------------------------
bool Whole(wchar_t *S) {
	if ((*S == L'+') || (*S == L'-'))
		S++;
	if (!isdigit(*S) || ((*S == L'0') && S[1]))
		return false;
	if (!S[1] || AllCifry(S + 1))
		return true;
	return false;
}

// ---------------------------------------------------------------------------
bool Fractional(wchar_t *S) {
	wchar_t *s = wcschr(S, L'.');
	if (KSimbStr(S, L'.') != 1)
		return false;
	*s = L'\0';
	if ((Whole(S)) && (!s[1] || AllCifry(s + 1))) {
		*s = L'.';
		return true;
	}
	*s = L'.';
	return false;
}

// ---------------------------------------------------------------------------
bool Number(wchar_t *S) {
	wchar_t *s, E;
	bool W, F, w;
	if ((s = wcschr(S, L'e')) || (s = wcschr(S, L'E'))) {
		E = *s;
		*s = L'\0';
		if ((Whole(S) || Fractional(S)) && Whole(s + 1)) {
			*s = E;
			return true;
		}
		*s = E;
		return false;
	}
	if (Whole(S) || Fractional(S))
		return true;
	return false;
}

// ---------------------------------------------------------------------------
bool IsLiters(wchar_t *S) {
	for (; *S; S++)
		if ((isciril(*S) || isalpha(*S)) && (*S != L'e') && (*S != L'e'))
			return true;
	return false;
}

// ---------------------------------------------------------------------------
bool IsCifra(wchar_t B) {
	return(B > 47 && B < 58);
}

// ---------------------------------------------------------------------------
void FormSizes(int &Left, int &Top, int &Width, int &Height) {
	HWND Window;
	TRect TaskBarWind;
	Window = FindWindow("Shell_trayWnd", NULL);
	GetWindowRect(Window, &TaskBarWind);
	if (TaskBarWind.Left <= 0 && (TaskBarWind.Width() >= Screen->Width)) {
		// вверху или внизу
		Width = Screen->Width;
		Height = Screen->Height - TaskBarWind.Height();
		Left = 0;
		if (TaskBarWind.Top)
			Top = 0; // TaskBar внизу
		else
			Top = TaskBarWind.Bottom + 1; // TaskBar вверху
	}
	else { // справа или слева
		Width = Screen->Width - TaskBarWind.Width();
		Height = Screen->Height;
		Top = 0;
		if (TaskBarWind.Left > 0)
			Left = 0;
		else
			Left = TaskBarWind.Right + 1;
	}
}

// ---------------------------------------------------------------------------
void CopyZam(FILE *F, wchar_t *Target) {
	int l = 0, Lmax = 72;
	bool NewLine = false;
	for (wchar_t *t = Target; *t; t++) {
		switch(*t) {
		case L'@':
			l += fwprintf(F, L"_dog_");
			break;
		case L'$':
			l += fwprintf(F, L"_bax_");
			break;
		case L'\'':
			l += fwprintf(F, L"_po_");
			break;
		case L'°':
			l += fwprintf(F, L"_g");
			break;
		case L'+':
		case L'-':
		case L'*':
		case L'/':
		case L';':
		case L',':
			l++;
			fputwc(*t, F);
			if (NewLine && (t[1] != L'\n')) {
				l = 0;
				fputws(L"\n   ", F);
				NewLine = false;
			}
			break;
		default:
			l++;
			fputwc(*t, F);
		}
		if (l > Lmax)
			NewLine = true;
	}
}
// ---------------------------------------------------------------------------
