#include "CalcStr.h"

#define MINUS   -1
#define DOUBLE   1
#define SKOBKA1 40
#define SKOBKA2 41

#include <math.h>
//static int MAXZNAC=4;
//static wchar_t B,*sp,*SP,S[256],INF[256],ZNAC[]=L" +-*/";
static int MAXZNAC=6;
static wchar_t B,*sp,*SP,S[256],INF[256],ZNAC[]=L" +-*/^%";

int Nznaca (void) {
 int i; for(i=MAXZNAC;i>0&&B!=ZNAC[i];i--); return i;
}

wchar_t *Short(wchar_t *S) { wchar_t *s;
 for(s=S;*s;s++); for(--s;*s==' ';s--); *(s+1)='\0'; return S;
}

int ChtoStr (void) { int Chto,Numb,i=0;
M:switch(B=*sp++) {
  case  ' ':case  '+': goto M;
  case '\0': S[i]='\0'; return 0;
  case  '-': S[i++]=B; S[i]='\0'; return MINUS;
  case  '(': S[i++]=B; S[i]='\0'; return SKOBKA1;
  case  ')': S[i++]=B; S[i]='\0'; return SKOBKA2;
  case  '.': case  'ю': case  'Ю': case  ',': Chto=DOUBLE; S[i++]=*(sp-1)='.'; Numb=1; break;
  default:
   if(isdigit(B)) { Chto=DOUBLE; S[i++]=B; break; }
   else {
//    swprintf(INF,L"Запрещенный символ - '%c' в формуле \"%s\".",B,SP);
    swprintf(INF,L"Запрещенный символ - '%c'.",B,SP);
    Application->MessageBox(INF,L"Ошибка!",MB_OK|MB_ICONWARNING);
    return 0;
 } }
L:if(B=*sp++) switch(B) {
  case  '(':
   if(Numb) {
    swprintf(INF,L"Вместо \"%s\" в формуле \"%s\" ожидалось имя.",sp-1,SP);
    Application->MessageBox(INF,L"Ошибка!",MB_OK|MB_ICONWARNING);
    return 0;
   }
  case  '-': S[i]='\0'; Short(S);
   if(((B=S[i-1])=='E'||B=='e')&&Chto==DOUBLE) { S[i++]='-'; goto L; } B='-';
  case  ')': case  '+': case  '*': case  '/': case  '%': break;
  case  '.': case  'ю': case  'Ю': case  ',': S[i++]=B; Chto=DOUBLE; Numb=1;
  case  ' ': goto L;
  default  :
   if(((isalpha(B)||isciril(B))&&(B!='E')&&(B!='e'))&&Numb) {
    swprintf(INF,L"Вместо \"%s\" в формуле \"%s\" ожидалась: цифра,L"
		" 'E', 'e', знак операции.",sp-1,SP);
    Application->MessageBox(INF,L"Ошибка!",MB_OK|MB_ICONWARNING);
    return 0;
   }
   S[i++]=B; goto L;
 }
 S[i]='\0'; Short(S); return Chto;
}

double CalcForm (int Prio) { int Oper,KS=0; double F;
 switch(ChtoStr()) {
  case MINUS  : F=-CalcForm(6); break;
  case SKOBKA1: KS++; F=CalcForm(0); break;
  case  DOUBLE: F=_wtof(S); break;
  default: return 0.0;
 }
 for(;;) switch(B) {
  case  ' ': if(!(B=*sp++)) return 0.0; else break;
  case SKOBKA2: if(KS) { KS--; B=*sp++; break; } return F;
  case  '\0':
   if(KS) {
    swprintf(INF,L"Вместо \"%c\" в формуле \"%s\" ожидалась ')'.",B,SP);
    Application->MessageBox(INF,L"Ошибка!",MB_OK|MB_ICONWARNING);
    F=0.0;
   } return F;
  default:
   if(!(Oper=Nznaca())) return F=0.0;
   else {
    if(Prio>=Oper) return F;
    switch(Oper) { double f;
     case  1: F+=CalcForm(Oper); break;
     case  2: F-=CalcForm(Oper); break;
     case  3: F*=CalcForm(Oper); break;
     case  4: f=CalcForm(Oper); if(f!=0.0) F/=f; else  F=0.0; break;
     case  6:
      f=CalcForm(Oper); if(f!=0.0) F-=f*floor(F/f); else F=0.0; break;
}}}}

double CalcStr (wchar_t *Sp) { sp=SP=Sp; return (SP&&*SP)?CalcForm(0):0.0; }

