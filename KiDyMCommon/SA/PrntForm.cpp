#include "CA.h"
#include <string.h>
#include <io.h>
#include "ConvDW.h"

			/*  Объявления объектов:  */
extern wchar_t Inf[],*ZNAC,LOGZNAC[],SimbInd,*NameFunc[],
 CurrDir[];
extern List *L; extern bool ISRAZM;
extern int N,NPV,Line,Pos,StrLong; extern Cnst *Pust;
extern TRichEdit *RichEdit;
static int k,l,Ksk; extern bool CPP,REPVARNACNST;
wchar_t Sk[11];

int GrekToHTM(wchar_t *Buf,wchar_t *Kod,wchar_t *s){
 wchar_t *b; int L,l;
 L=swprintf(b=Buf,
	L"<font face=\"Times New Roman, Times, serif\">%s</font>",Kod); b+=L;
 if(*s){
  l=swprintf(b,L"<sub><font size=\"-1\">%s</font></sub>",s); L+=l;
 }
 return L;
}
//---------------------------------------------------------------------------
int LatToHTM(wchar_t *Buf,wchar_t *S){ wchar_t C='\0',*s,*b=Buf; int L=0,l;
 if((S[0]=='x'||S[0]=='y'||S[0]=='z'||S[0]=='X'||S[0]=='Y'||S[0]=='Z')){
  L+=(l=swprintf(b,L"<i>%c</i><sub><font size=\"-1\">",S[0])); b+=l;
  l=LatToHTM(b,S+1); L+=l; b+=l;
  L+=swprintf(b,L"</font></sub>");
  return L;
 }
 if(!isdigit(S[0])){
  for(s=S;*s;s++)
   if(isdigit(*s)&&(s[-1]!='[')&&!isdigit(s[-1])){ C=*s; *s='\0'; break; }
  for(s=S;*s;){
   if(isalpha(*s)){
    b+=swprintf(b,L"<i>"); while(*s&&isalpha(*s)){ *b++=*s++; L++; }
    b+=swprintf(b,L"</i>"); L+=7;
   }
   else while(*s&&!isalpha(*s)){ *b++=*s++; L++; }
  }
  *b='\0';
  if(C){ *s=C; L+=swprintf(b,L"<sub><font size=\"-1\">%s</font></sub>",s); }
  return L;
 }
 return swprintf(b,L"%s",S);
}
//---------------------------------------------------------------------------
int Grek(wchar_t *S,int &Kod){//возвращает код и длину греческой буквы
 if(wcsstr(S,   L"alfa")==S){ Kod=945; return 4; }
 if(wcsstr(S,  L"alpha")==S){ Kod=945; return 5; }
 if(wcsstr(S,   L"beta")==S){ Kod=946; return 4; }
 if(wcsstr(S,  L"betta")==S){ Kod=946; return 5; }
 if(wcsstr(S,  L"gamma")==S){ Kod=947; return 5; }
 if(wcsstr(S,   L"gama")==S){ Kod=947; return 5; }
 if(wcsstr(S,  L"delta")==S){ Kod=948; return 5; }
 if(wcsstr(S,L"epsilon")==S){ Kod=949; return 7; }
 if(wcsstr(S,  L"dzeta")==S){ Kod=950; return 5; }
 if(wcsstr(S,  L"dseta")==S){ Kod=950; return 5; }
 if(wcsstr(S,   L"teta")==S){ Kod=952; return 4; }
 if(wcsstr(S,    L"eta")==S){ Kod=951; return 3; }
 if(wcsstr(S,   L"jota")==S){ Kod=953; return 4; }
 if(wcsstr(S,   L"iota")==S){ Kod=953; return 4; }
 if(wcsstr(S,  L"kappa")==S){ Kod=954; return 5; }
 if(wcsstr(S,  L"lamda")==S){ Kod=955; return 5; }
 if(wcsstr(S, L"lambda")==S){ Kod=955; return 6; }
 if(wcsstr(S,     L"mu")==S){ Kod=956; return 2; }
 if(wcsstr(S,     L"nu")==S){ Kod=957; return 2; }
 if(wcsstr(S,    L"ksi")==S){ Kod=958; return 3; }
 if(wcsstr(S,L"omicron")==S){ Kod=959; return 7; }
 if(wcsstr(S,L"omikron")==S){ Kod=959; return 7; }
 if(wcsstr(S,     L"pi")==S){ Kod=960; return 2; }
 if(wcsstr(S,     L"ro")==S){ Kod=961; return 2; }
 if(wcsstr(S,  L"sigma")==S){ Kod=963; return 5; }
 if(wcsstr(S,    L"tau")==S){ Kod=964; return 3; }
 if(wcsstr(S,L"ipsilon")==S){ Kod=965; return 7; }
 if(wcsstr(S,     L"fi")==S){ Kod=966; return 2; }
 if(wcsstr(S,     L"hi")==S){ Kod=967; return 2; }
 if(wcsstr(S,    L"psi")==S){ Kod=968; return 3; }
 if(wcsstr(S,  L"omega")==S){ Kod=969; return 5; }
 if(wcsstr(S,   L"ALFA")==S){ Kod=913; return 4; }
 if(wcsstr(S,  L"ALPHA")==S){ Kod=913; return 5; }
 if(wcsstr(S,   L"BETA")==S){ Kod=914; return 4; }
 if(wcsstr(S,  L"BETTA")==S){ Kod=914; return 5; }
 if(wcsstr(S,  L"GAMMA")==S){ Kod=915; return 5; }
 if(wcsstr(S,  L"DELTA")==S){ Kod=916; return 5; }
 if(wcsstr(S,L"EPSILON")==S){ Kod=917; return 7; }
 if(wcsstr(S,  L"DZETA")==S){ Kod=918; return 5; }
 if(wcsstr(S,  L"DSETA")==S){ Kod=918; return 5; }
 if(wcsstr(S,   L"TETA")==S){ Kod=920; return 4; }
 if(wcsstr(S,    L"ETA")==S){ Kod=919; return 3; }
 if(wcsstr(S,   L"JOTA")==S){ Kod=921; return 4; }
 if(wcsstr(S,   L"IOTA")==S){ Kod=921; return 4; }
 if(wcsstr(S,  L"KAPPA")==S){ Kod=922; return 5; }
 if(wcsstr(S,  L"LAMDA")==S){ Kod=923; return 5; }
 if(wcsstr(S, L"LAMBDA")==S){ Kod=923; return 6; }
 if(wcsstr(S,     L"MU")==S){ Kod=924; return 2; }
 if(wcsstr(S,     L"NU")==S){ Kod=925; return 2; }
 if(wcsstr(S,    L"KSI")==S){ Kod=926; return 3; }
 if(wcsstr(S,L"OMICRON")==S){ Kod=927; return 7; }
 if(wcsstr(S,L"OMIKRON")==S){ Kod=927; return 7; }
 if(wcsstr(S,     L"PI")==S){ Kod=960; return 2; }
 if(wcsstr(S,     L"RO")==S){ Kod=929; return 2; }
 if(wcsstr(S,  L"SIGMA")==S){ Kod=931; return 5; }
 if(wcsstr(S,    L"TAU")==S){ Kod=932; return 3; }
 if(wcsstr(S,L"IPSILON")==S){ Kod=933; return 7; }
 if(wcsstr(S,     L"FI")==S){ Kod=934; return 2; }
 if(wcsstr(S,     L"HI")==S){ Kod=935; return 2; }
 if(wcsstr(S,    L"PSI")==S){ Kod=936; return 3; }
 if(wcsstr(S,  L"OMEGA")==S){ Kod=937; return 5; }
 return 0;
}
//---------------------------------------------------------------------------
wchar_t *LiterGrekToHtm(wchar_t *S){
 int d,Kod,i; bool Index=false,Lat=false; String Buf;
 for(wchar_t *s=S;*s;){
  if(d=Grek(s,Kod)){
   if(Lat){ Buf+=L"</i>"; Lat=false; }
   Buf+=L"&#"; Buf+=Kod; Buf+=L";";
   s+=d; Lat=false;
  }
  else switch(*s){
   case '<': Buf+=L"&lt;"; s++; break;
   case '>': Buf+=L"&gt;"; s++; break;
   case '\'': case '[':
	if(Index){ Buf+=L"</sub>"; Index=false; }
	if(Lat)  { Buf+=L"</i>";   Lat=false;   }
	Buf+=*s++;
	break;
   default:
	if(*s==SimbInd){
	 s++;
	 if(!Index){
	  if(Lat){ Buf+=L"</i>"; Lat=false; }
	  Buf+=L"<sub>"; Index=true;
	}}
	if(d=Grek(s,Kod)){
	 if(Lat){ Buf+=L"</i>"; Lat=false; }
	 Buf+=L"&#"; Buf+=Kod; Buf+=L";";
	 s+=d; Lat=false;
	}
	if(Lat&&(isciril(*s)||IsCifra(*s))){
	 Buf+=L"</i>"; Lat=false;
	}
	else if(!Lat&&isalpha(*s)){
	 Buf+=L"<i>"; Lat=true;
	}
	if(*s) Buf+=*s++;
	if(Index){
	 if(Lat){ Buf+=L"</i>"; Lat=false; }
	 Buf+=L"</sub>"; Index=false;
 }}}
 if(Lat) Buf+=L"</i>";
 if(Index) Buf+=L"</sub>";
 S=(wchar_t *)realloc(S,(Buf.Length()+1)*SzC);
 wcscpy(S,Buf.w_str());
 return S;
}
//---------------------------------------------------------------------------
bool IsGrek(wchar_t *S){
 wchar_t Name[80]; wcscpy(Name,S); LowToUpp(Name);
 if(wcsstr(Name,   L"ALFA")==Name) return true;
 if(wcsstr(Name,  L"ALPHA")==Name) return true;
 if(wcsstr(Name,   L"BETA")==Name) return true;
 if(wcsstr(Name,  L"BETTA")==Name) return true;
 if(wcsstr(Name,  L"БЕТТА")==Name) return true;
 if(wcsstr(Name,  L"GAMMA")==Name) return true;
 if(wcsstr(Name,  L"DELTA")==Name) return true;
 if(wcsstr(Name,L"EPSILON")==Name) return true;
 if(wcsstr(Name,  L"DZETA")==Name) return true;
 if(wcsstr(Name,  L"DSETA")==Name) return true;
 if(wcsstr(Name,    L"ETA")==Name) return true;
 if(wcsstr(Name,   L"TETA")==Name) return true;
 if(wcsstr(Name,   L"JOTA")==Name) return true;
 if(wcsstr(Name,   L"IOTA")==Name) return true;
 if(wcsstr(Name,  L"KAPPA")==Name) return true;
 if(wcsstr(Name,  L"LAMDA")==Name) return true;
 if(wcsstr(Name, L"LAMBDA")==Name) return true;
 if(wcsstr(Name,     L"MU")==Name) return true;
 if(wcsstr(Name,     L"NU")==Name) return true;
 if(wcsstr(Name,    L"KSI")==Name) return true;
 if(wcsstr(Name,L"OMICRON")==Name) return true;
 if(wcsstr(Name,L"OMIKRON")==Name) return true;
 if(wcsstr(Name,     L"PI")==Name) return true;
 if(wcsstr(Name,     L"RO")==Name) return true;
 if(wcsstr(Name,  L"SIGMA")==Name) return true;
 if(wcsstr(Name,    L"TAU")==Name) return true;
 if(wcsstr(Name,L"IPSILON")==Name) return true;
 if(wcsstr(Name,     L"FI")==Name) return true;
 if(wcsstr(Name,     L"HI")==Name) return true;
 if(wcsstr(Name,    L"PSI")==Name) return true;
 if(wcsstr(Name,  L"OMEGA")==Name) return true;
 return false;
}
//---------------------------------------------------------------------------
void PFRez(FILE *OUTFILE,int a,int Prio,Form F) {
//ядро процедуры печати формул с заменой апострофа в именах переменных на L"_po_"
 wchar_t *s,*b,Beg,End,Buf[1024]; int i;
 switch(F.C->Atr){
  case PUST: putwc('$',OUTFILE); k++; break;
  case CNST:
   if(F.C->Name){
	wcscpy(Buf,F.C->Name); for(s=Buf;*s&&*s==' ';s++); StrWINtoDOS(s);
	fwprintf(OUTFILE,s);
   }
   else k+=fwprintf(OUTFILE,L"%g",F.C->Val); break;
  case VARY:
   for(s=F.V->Name,b=Buf;*s;s++)
    if(*s=='\'') b+=swprintf(b,L"_po_"); else *b++=*s;
   *b='\0';
   StrWINtoDOS(Buf); k+=fwprintf(OUTFILE,Buf);
   break;
  case UNOP:
   if(F.U->Func==18){
    if((F.U->Arg.C->Atr!=CNST)&&(F.U->Arg.C->Atr!=VARY)){
     fputwc('(',OUTFILE); k++;
    }
    PFRez(OUTFILE,a,0,F.U->Arg);
    if((F.U->Arg.C->Atr!=CNST)&&(F.U->Arg.C->Atr!=VARY)){
     fputwc(')',OUTFILE); k++;
    }
    k+=fwprintf(OUTFILE,NameFunc[F.U->Func]);
   }
   else{
    k+=fwprintf(OUTFILE,NameFunc[F.U->Func]);
    if((F.U->Func==1)&&
       ((F.U->Arg.C->Atr==CNST)||(F.U->Arg.C->Atr==VARY)||
        ((F.U->Arg.C->Atr==UNOP)&&(F.U->Arg.U->Func!=1))||
        ((F.U->Arg.C->Atr==BNOP)&&(F.U->Arg.B->Oper>2))||
        (F.U->Arg.C->Atr==IPLL)||(F.U->Arg.C->Atr==SPLN))){
     PFRez(OUTFILE,a,0,F.U->Arg);
    }
    else{
     switch((++Ksk)%3){
      case 1: putwc('(',OUTFILE); break;
      case 2: putwc('[',OUTFILE); break;
      case 0: putwc('{',OUTFILE); break;
     }
     k++; PFRez(OUTFILE,a,0,F.U->Arg);
     switch((Ksk--)%3){
      case 1: putwc(')',OUTFILE); break;
      case 2: putwc(']',OUTFILE); break;
      case 0: putwc('}',OUTFILE); break;
     }
     k++;
   }}
   break;
  case BNOP : i=F.B->Oper;
   if((i-1)/2<(Prio-1)/2){//24.02.2013
    switch((++Ksk)%3){
     case 2: putwc('[',OUTFILE); break;
     case 0: putwc('{',OUTFILE); break;
    }
    k++;
   }
   PFRez(OUTFILE,a,i,F.B->L);
   if(i<3) putwc(' ',OUTFILE); putwc(ZNAC[i],OUTFILE);
   if(i<3) putwc(' ',OUTFILE);
   k++;
   if(F.B->R.C->Atr==INTG){
    switch((++Ksk)%3){
     case 1: putwc('(',OUTFILE); break;
     case 2: putwc('[',OUTFILE); break;
     case 0: putwc('{',OUTFILE); break;
    }
    k++;
   }
   PFRez(OUTFILE,a,i,F.B->R);
   if(F.B->R.C->Atr==INTG){
    switch((Ksk--)%3){
     case 1: putwc(')',OUTFILE); break;
     case 2: putwc(']',OUTFILE); break;
     case 0: putwc('}',OUTFILE); break;
    }
    k++;
   }
   if((i-1)/2<(Prio-1)/2){
    switch((Ksk--)%3){
     case 1: putwc(')',OUTFILE); break;
     case 2: putwc(']',OUTFILE); break;
     case 0: putwc('}',OUTFILE); break;
    }
    k++;
   }
   break;
  case IFOP:
   switch((++Ksk)%3){
    case 1: putwc('(',OUTFILE); break;
    case 2: putwc('[',OUTFILE); break;
    case 0: putwc('{',OUTFILE); break;
   }
   k++; PFRez(OUTFILE,a,0,F.L->U);
   switch(F.L->Oper){
    case 1:// <
     putwc(LOGZNAC[F.L->Oper],OUTFILE);
     break;
    case 2:// >
     putwc(LOGZNAC[F.L->Oper],OUTFILE);
     break;
    case 3:// <=
	 fwprintf(OUTFILE,L"%c=",LOGZNAC[F.L->Oper-2]);
     break;
    case 4:// >=
	 fwprintf(OUTFILE,L"%c=",LOGZNAC[F.L->Oper-2]);
     break;
    case 5:// \=
	 fwprintf(OUTFILE,L"%c=",LOGZNAC[F.L->Oper-2]);
     break;
    case 6:// ==
     fwprintf(OUTFILE,L"%c=",LOGZNAC[F.L->Oper-2]);
     break;
    case 7:// <<
     fwprintf(OUTFILE,L"%c%c",LOGZNAC[1],LOGZNAC[1]);
     break;
    case 8:// >>
     fwprintf(OUTFILE,L"%c%c",LOGZNAC[2],LOGZNAC[2]);
     break;
   }
   PFRez(OUTFILE,a,0,F.L->V);  fputws(L" ? ",OUTFILE); k+=3;
   PFRez(OUTFILE,a,0,F.L->FT);
   if(F.L->FF.C!=Pust){
    fputws(L" ! ",OUTFILE); k+=3;
    PFRez(OUTFILE,a,0,F.L->FF);
   }
   switch((Ksk--)%3){
    case 1: putwc(')',OUTFILE); break;
    case 2: putwc(']',OUTFILE); break;
    case 0: putwc('}',OUTFILE); break;
   }
   k++; break;
  case IPLL:
   switch(F.I->Type) {
    case 1: fputws(L"ipl(",OUTFILE); Beg=',';  End=')'; break;
    case 2: Beg='['; End=']';
   }
   wcscpy(Buf,F.I->A->Name);
   StrWINtoDOS(Buf); k+=fwprintf(OUTFILE,Buf);
   putwc(Beg,OUTFILE); PFRez(OUTFILE,a,0,F.I->F); putwc(End,OUTFILE); break;
  case SPLN:
   switch(F.P->Type) {
    case 1: fputws(L"spln(",OUTFILE); Beg=',';  End=')'; break;
    default: Beg='['; End=']';
   }
   wcscpy(Buf,F.P->A->Name);
   StrWINtoDOS(Buf); k+=fwprintf(OUTFILE,Buf);
   putwc(Beg,OUTFILE); PFRez(OUTFILE,a,0,F.P->F); putwc(End,OUTFILE); break;
  case SUMM:
   fputws(L"summ(",OUTFILE);
   PFRez(OUTFILE,a,0,F.S->F); putwc(',',OUTFILE);
   wcscpy(Buf,F.S->i->Name);
   StrWINtoDOS(Buf); k+=fwprintf(OUTFILE,Buf);
   putwc(',',OUTFILE); PFRez(OUTFILE,a,0,F.S->a);
   putwc(',',OUTFILE); PFRez(OUTFILE,a,0,F.S->b);
   putwc(')',OUTFILE); break;
  case INTG:
   fputws(L"intg(",OUTFILE);
   PFRez(OUTFILE,a,0,F.G->f); fputws(L", ",OUTFILE);
   wcscpy(Buf,F.G->x->Name);
   StrWINtoDOS(Buf); k+=fwprintf(OUTFILE,Buf);
   if(F.G->a!=Pust){
	fputws(L" , ",OUTFILE); PFRez(OUTFILE,a,0,F.G->a);
	fputws(L" , ",OUTFILE); PFRez(OUTFILE,a,0,F.G->b);
   }
   fputws(L" ) ",OUTFILE);
   if(F.G->F!=Pust){
	fputws(L" = ",OUTFILE); PFRez(OUTFILE,a,0,F.G->F);
    if(F.G->Rez!=Pust){
     fputws(L" = ",OUTFILE); PFRez(OUTFILE,a,0,F.G->Rez);
   }}
   break;
  case RUUT:
   fputws(L"root( ",OUTFILE);
   PFRez(OUTFILE,a,0,F.R->f); fputws(L", ",OUTFILE);
   wcscpy(Buf,F.R->x->Name);
   StrWINtoDOS(Buf); k+=fwprintf(OUTFILE,Buf);
   if(F.R->a!=Pust){
	fputws(L" , ",OUTFILE); PFRez(OUTFILE,a,0,F.R->a);
	fputws(L" , ",OUTFILE); PFRez(OUTFILE,a,0,F.R->b);
   }
   fputws(L" ) ",OUTFILE);
   break;
  case REST:
   fputws(L"rest(",OUTFILE);
   PFRez(OUTFILE,a,0,F.O->a); fputws(L", ",OUTFILE);
   PFRez(OUTFILE,a,0,F.O->b);
   fputws(L" ) ",OUTFILE);
   break;
  case ARRI:
   fwprintf(OUTFILE,L"%s[",F.a->A->Name);
   if(F.a->FInd.C->Atr==CNST){
    if(F.a->FInd.C->Name)
     fwprintf(OUTFILE,L"%s",F.a->FInd.C->Name);
    else
     fwprintf(OUTFILE,L"%d",Round(F.a->FInd.C->Val));
   }
   else PFRez(OUTFILE,a,0,F.a->FInd);
   fputwc(']',OUTFILE);
   break;
}}
//---------------------------------------------------------------------------
int PrintRez(FILE *OUTFILE,wchar_t *NameF,Form F){
//процедура печати формул с заменой апострофа в именах переменных на L"_po_"
 wchar_t Buf[1024],*b,*s; k=0; Ksk=0; l=60;
 if(NameF){
  for(s=NameF,b=Buf;*s;s++)
   if(*s=='\'') b+=swprintf(b,L"_po_"); else *b++=*s;
  *b='\0';
  Fputs(Buf,OUTFILE); k=wcslen(Buf);
  fputws(L" = ",OUTFILE); k+=3;
 }
 PFRez(OUTFILE,k,0,F);
 if(NameF){ putwc(';',OUTFILE); k++; }
 return k;
}
//---------------------------------------------------------------------------
void FullPrntForm(FILE *OUTFILE,Form F,Magazine **M){ Magazine *m;
//процедура вывода в файл формул переменных, входящих в данную формулу
 switch(F.C->Atr){
  case CNST: return;
  case VARY:
   if((F.V->Znach.C->Atr!=PUST)&&!FindMagazine(*M,F.V->Name,&m)){
    if(F.V->Znach.C->Atr!=CNST)//переменная не равна константе
     FullPrntForm(OUTFILE,F.V->Znach,M);
    PrintRez(OUTFILE,F.V->Name,F.V->Znach); fputwc('\n',OUTFILE);
    TakeMagazine(M,F.V->Name);
   }
   return;
  case UNOP: //унарная операция
   FullPrntForm(OUTFILE,F.U->Arg,M);
   return;
  case BNOP: //бинарная операция
   FullPrntForm(OUTFILE,F.B->L,M);
   FullPrntForm(OUTFILE,F.B->R,M);
   return;
  case IFOP: //условное предложение
   FullPrntForm(OUTFILE,F.L->U,M);
   FullPrntForm(OUTFILE,F.L->V,M);
   FullPrntForm(OUTFILE,F.L->FT,M);
   FullPrntForm(OUTFILE,F.L->FF,M);
   return;
  case SUMM: //сумма
   FullPrntForm(OUTFILE,F.S->a,M);
   FullPrntForm(OUTFILE,F.S->b,M);
   FullPrntForm(OUTFILE,F.S->F,M);
   return;
  case INTG: //интеграл
   FullPrntForm(OUTFILE,F.G->a,M);
   FullPrntForm(OUTFILE,F.G->b,M);
   FullPrntForm(OUTFILE,F.G->f,M);
   FullPrntForm(OUTFILE,F.G->F,M);
   FullPrntForm(OUTFILE,F.G->Rez,M);
   return;
  case ARRI:
   fwprintf(OUTFILE,L"%s[",F.a->A->Name);
   if(F.a->FInd.C->Atr==CNST){
    if(F.a->FInd.C->Name)
     fwprintf(OUTFILE,L"%s",F.a->FInd.C->Name);
    else
     fwprintf(OUTFILE,L"%d",Round(F.a->FInd.C->Val));
   }
   else FullPrntForm(OUTFILE,F.a->FInd,M);
   fputwc(']',OUTFILE);
   break;
}}
//---------------------------------------------------------------------------
bool OpenSk(int i,int Prio){
 if(i<Prio){
  switch(i){
   case 1: case 2:
	if(Prio>2) return true;
	break;
   case 3: case 4:
	if(Prio>4) return true;
	break;
   case 5: return true;
 }}
 return false;
}
//---------------------------------------------------------------------------
void PF(bool DOS,FILE *OUTFILE,int a,int Prio,Form F) {
 wchar_t *s,Beg,End,*Buf; int i;
 switch(F.C->Atr){
  case PUST: putwc('$',OUTFILE); k++; break;
  case CNST:
   if(F.C->Name){
	if(DOS){
	 if(CPP){
	  if(!wcscmp(F.C->Name,L"2PI"))        s=wcscpy(Buf,L"2*M_PI");
	  else if(!wcscmp(F.C->Name,L"3PI"))   s=wcscpy(Buf,L"3*M_PI");
	  else if(!wcscmp(F.C->Name,L"4PI"))   s=wcscpy(Buf,L"4*M_PI");
	  else if(!wcscmp(F.C->Name,L"5PI"))   s=wcscpy(Buf,L"5*M_PI");
	  else if(!wcscmp(F.C->Name,L"6PI"))   s=wcscpy(Buf,L"6*M_PI");
	  else if(!wcscmp(F.C->Name,L"7PI"))   s=wcscpy(Buf,L"7*M_PI");
	  else if(!wcscmp(F.C->Name,L"PI"))    s=wcscpy(Buf,L"M_PI");
	  else if(!wcscmp(F.C->Name,L"pi"))    s=wcscpy(Buf,L"M_PI");
	  else if(!wcscmp(F.C->Name,L"PIna2")) s=wcscpy(Buf,L"M_PI_2");
	  else if(!wcscmp(F.C->Name,L"PIna3")) s=wcscpy(Buf,L"M_PI/3.0");
	  else if(!wcscmp(F.C->Name,L"PIna4")) s=wcscpy(Buf,L"M_PI_4");
	  else if(!wcscmp(F.C->Name,L"PIna6")) s=wcscpy(Buf,L"M_PI/6.0");
	  else s=F.C->Name;
	 }
	 else{
	  wcscpy(Buf,F.C->Name); for(s=Buf;*s&&*s==' ';s++); StrWINtoDOS(s);
	}}
	else{
	 if(!wcscmp(F.C->Name,L"2PI"))
	  s=wcscpy(Buf,L"<font face=\"Times New Roman, Times, serif\">2&#960;</font>");
	 else if(!wcscmp(F.C->Name,L"3PI"))
	  s=wcscpy(Buf,L"<font face=\"Times New Roman, Times, serif\">3&#960;</font>");
	 else if(!wcscmp(F.C->Name,L"4PI"))
	  s=wcscpy(Buf,L"<font face=\"Times New Roman, Times, serif\">4&#960;</font>");
	 else if(!wcscmp(F.C->Name,L"5PI"))
	  s=wcscpy(Buf,L"<font face=\"Times New Roman, Times, serif\">5&#960;</font>");
	 else if(!wcscmp(F.C->Name,L"6PI"))
	  s=wcscpy(Buf,L"<font face=\"Times New Roman, Times, serif\">6&#960;</font>");
	 else if(!wcscmp(F.C->Name,L"7PI"))
	  s=wcscpy(Buf,L"<font face=\"Times New Roman, Times, serif\">7&#960;</font>");
	 else if(!wcscmp(F.C->Name,L"PI"))
	  s=wcscpy(Buf,L"<font face=\"Times New Roman, Times, serif\">&#960;</font>");
	 else if(!wcscmp(F.C->Name,L"pi"))
	  s=wcscpy(Buf,L"<font face=\"Times New Roman, Times, serif\">&#960;</font>");
	 else if(!wcscmp(F.C->Name,L"PIna2"))
	  s=wcscpy(Buf,L"<font face=\"Times New Roman, Times, serif\">&#960;/2</font>");
	 else if(!wcscmp(F.C->Name,L"PIna3"))
	  s=wcscpy(Buf,L"<font face=\"Times New Roman, Times, serif\">&#960;/3</font>");
	 else if(!wcscmp(F.C->Name,L"PIna4"))
	  s=wcscpy(Buf,L"<font face=\"Times New Roman, Times, serif\">&#960;/4</font>");
	 else if(!wcscmp(F.C->Name,L"PIna6"))
	  s=wcscpy(Buf,L"<font face=\"Times New Roman, Times, serif\">&#960;/6</font>");
	 else s=F.C->Name;
   }}
   else
	swprintf(s=Buf,L"%g",F.C->Val);
   a+=wcslen(s);
   if(a>l){ fputwc('\n',OUTFILE); a=0; }
   fputws(s,OUTFILE);
   break;
  case VARY:
   Buf=(wchar_t *)calloc(wcslen(F.V->Name)+1,SzC);
   wcscpy(Buf,F.V->Name);
   if(DOS)
	StrWINtoDOS(Buf);
   else
	Buf=LiterGrekToHtm(Buf);
   a+=wcslen(Buf);
   if(a>l){ fputwc('\n',OUTFILE); a=0; }
   fputws(Buf,OUTFILE);
   free(Buf);
   break;
  case UNOP:
   if(F.U->Func==18){ //факториал
	if((F.U->Arg.C->Atr!=3)&&(F.U->Arg.C->Atr!=4)){
	 a++;
	 if(DOS){ a++; if(a>l){ fputwc('\n',OUTFILE); a=0; }}
	 fputwc('(',OUTFILE);
	}
	PF(DOS,OUTFILE,a,0,F.U->Arg);
	if((F.U->Arg.C->Atr!=3)&&(F.U->Arg.C->Atr!=4)){
	 a++;
	 if(DOS){ if(a>l){ fputwc('\n',OUTFILE); a=0; }}
	 fputwc(')',OUTFILE);
	}

	a+=wcslen(NameFunc[F.U->Func]);
	if(a>l){ fputwc('\n',OUTFILE); a=0; }
	fputws(NameFunc[F.U->Func],OUTFILE);
   }
   else{
	if(DOS)
	 a+=fwprintf(OUTFILE,NameFunc[F.U->Func]);
	else{
	 if(F.U->Func==1){ fwprintf(OUTFILE,L"&#9472;"); a++; }//минус
	 else if(F.U->Func==2){ fwprintf(OUTFILE,L"&#8730;"); a++; }//корень квадратный
	 else a+=fwprintf(OUTFILE,NameFunc[F.U->Func]);
	}
	if(a>l){ fputwc('\n',OUTFILE);a=0; }
	if((F.U->Func==1)&&
	   ((F.U->Arg.C->Atr==CNST)||(F.U->Arg.C->Atr==VARY)||
	   ((F.U->Arg.C->Atr==UNOP)&&(F.U->Arg.U->Func!=1))||
	   ((F.U->Arg.C->Atr==BNOP)&&(F.U->Arg.B->Oper>2))||
	   (F.U->Arg.C->Atr==IPLL)||(F.U->Arg.C->Atr==SPLN))){
	 PF(DOS,OUTFILE,a,0,F.U->Arg);
	}
	else
	 if(!DOS&&(F.U->Func==2)&&
		((F.U->Arg.C->Atr==CNST)||(F.U->Arg.C->Atr==VARY)||(F.U->Arg.C->Atr==UNOP)))
	  PF(DOS,OUTFILE,a,0,F.U->Arg);
	else
	 if(!DOS&&(F.U->Func>2)&&(F.U->Func<6)&&
		(F.U->Arg.C->Atr==VARY)&&IsGrek(F.U->Arg.V->Name)){
	  fputws(L"<font size=-2> </font>",OUTFILE);
	  PF(DOS,OUTFILE,a,0,F.U->Arg);
	  fputws(L"<font size=-2> </font>",OUTFILE);
	}
	else{
	 if(DOS) putwc('(',OUTFILE);
	 else switch(Ksk%3){
	  case 1: putwc('(',OUTFILE); break;
	  case 2: putwc('(',OUTFILE); break;//[
	  case 0: putwc('(',OUTFILE); break;//{
	 }
	 Ksk++; a++; PF(DOS,OUTFILE,a,0,F.U->Arg);
	 if(DOS) putwc(')',OUTFILE);
	 else switch((Ksk)%3){
	  case 1: putwc(')',OUTFILE); break;
	  case 2: putwc(')',OUTFILE); break;//]
	  case 0: putwc(')',OUTFILE); break;//}
	 }
	 Ksk--; a++;
   }}
   break;
  case BNOP : i=F.B->Oper;
   if((i<Prio)){//||((i==Prio)&&(i==2))){
	if(DOS) putwc('(',OUTFILE);
	else switch((Ksk)%3){
	 case 1: putwc('(',OUTFILE); break;
	 case 2: putwc('(',OUTFILE); break;//[
	 case 0: putwc('(',OUTFILE); break;//{
	}
	Ksk++; a++;
   }//18.01.2019
/*   if(OpenSk(i,Prio)){//18.01.2019
	if(DOS) putwc('(',OUTFILE);
	else switch((Ksk)%3){
	 case 1: putwc('(',OUTFILE); break;
	 case 2: putwc('(',OUTFILE); break;//[
	 case 0: putwc('(',OUTFILE); break;//{
	}
	Ksk++; a++;
   }*/
   if(!DOS&&(i==6)){
	if((F.B->L.C->Atr==5)&&(F.B->L.U->Func>2)&&(F.B->L.U->Func<18)){
	 fputws(NameFunc[F.B->L.U->Func],OUTFILE);
	 fputws(L"<sup><font size=\"-1\">",OUTFILE);
	 PF(false,OUTFILE,a,i,F.B->R);
	 fputws(L"</font></sup>",OUTFILE);
	 PF(false,OUTFILE,a,i,F.B->L.U->Arg);
	}
	else /*if(F.B->L.C->Atr==4)*/{
	 PF(false,OUTFILE,a,i,F.B->L);
	 fputws(L"<sup><font size=\"-1\">",OUTFILE);
	 PF(false,OUTFILE,a,i,F.B->R);
	 fputws(L"</font></sup>",OUTFILE);
   }}
   else{
	if(DOS||i<6){
//     if(i!=6)
	  PF(DOS,OUTFILE,a,i,F.B->L);
	 if(DOS||(i!=2)) {
	  if(i<3) putwc(' ',OUTFILE);
	  if((i==6)&&CPP){
	   fputws(L"pow(",OUTFILE);
	   PF(DOS,OUTFILE,a,i,F.B->L);
	   putwc(',',OUTFILE);
	  }
	  else putwc(ZNAC[i],OUTFILE);
	  if(i<3) putwc(' ',OUTFILE);
	 }
	 else fputws(L" &#9472; ",OUTFILE); // 8722 8211
	 a++;
	}
	else if((F.B->L.C->Atr==4)&&!wcschr(F.B->L.V->Name,'\'')){
	 PF(DOS,OUTFILE,a,i,F.B->L);
	 if(!DOS) fputws(L"<sup><font size=\"-1\">",OUTFILE);
	}
	else{
	 if(F.B->L.C->Atr==INTG){
	  if(DOS) putwc('(',OUTFILE);
	  else switch((Ksk)%3){
	   case 1: putwc('(',OUTFILE); break;
	   case 2: putwc('(',OUTFILE); break;//[
	   case 0: putwc('(',OUTFILE); break;//{
	  }
	  Ksk++; a++;
	 }
	 PF(DOS,OUTFILE,a,i,F.B->L);
	 if(F.B->L.C->Atr==INTG){
	  if(DOS) putwc(')',OUTFILE);
	  else{
	   switch((Ksk)%3){
		case 1: putwc(')',OUTFILE); break;
		case 2: putwc(')',OUTFILE); break;//]
		case 0: putwc(')',OUTFILE); break;//]
	   }
	   fputws(L"<sup><font size=\"-1\">",OUTFILE);
	  }
	  Ksk--; a++;
	 }
	}
	if(F.B->R.C->Atr==INTG){
	 if(DOS) putwc('(',OUTFILE);
	 else switch((Ksk)%3){
	  case 1: putwc('(',OUTFILE); break;
	  case 2: putwc('(',OUTFILE); break;//[
	  case 0: putwc('(',OUTFILE); break;//{
	 }
	 Ksk++; a++;
	}
	PF(DOS,OUTFILE,a,i,F.B->R);
	if(i==6)
	 if(DOS)
	  putwc(')',OUTFILE);
	 else
	  fputws(L"</font></sup>",OUTFILE);
	if(F.B->R.C->Atr==INTG){
	 if(DOS) putwc(')',OUTFILE);
	 else switch((Ksk)%3){
	  case 1: putwc(')',OUTFILE); break;
	  case 2: putwc(')',OUTFILE); break;//]
	  case 0: putwc(')',OUTFILE); break;//}
	 }
	 Ksk--; a++;
   }}
   if((i<Prio)){//||((i==Prio)&&(i==2))){
	if(DOS) putwc(')',OUTFILE);
	else switch((Ksk)%3){
	 case 1: putwc(')',OUTFILE); break;
	 case 2: putwc(')',OUTFILE); break;//]
	 case 0: putwc(')',OUTFILE); break;//}
	}
	Ksk--; a++;
   }
/*   if(OpenSk(i,Prio)){//18.01.2019
	if(DOS) putwc(')',OUTFILE);
	else switch((Ksk)%3){
	 case 1: putwc(')',OUTFILE); break;
	 case 2: putwc(')',OUTFILE); break;//]
	 case 0: putwc(')',OUTFILE); break;//}
	}
	Ksk--; a++;
   }*/
   break;
  case IFOP:
   if(DOS) putwc('(',OUTFILE);
   else switch((++Ksk)%3){
	case 1: putwc('(',OUTFILE); break;
	case 2: putwc('(',OUTFILE); break;//[
	case 0: putwc('(',OUTFILE); break;//{
   }
   a++; PF(DOS,OUTFILE,a,0,F.L->U);
   switch(F.L->Oper){
	case 1:// <
	 if(DOS) putwc(LOGZNAC[F.L->Oper],OUTFILE);
	 else fputws(L"&#60;",OUTFILE);
	 break;
	case 2:// >
	 if(DOS) putwc(LOGZNAC[F.L->Oper],OUTFILE);
	 else fputws(L"&#62;",OUTFILE);
	 break;
	case 3:// <=
	 if(DOS) fwprintf(OUTFILE,L"%c=",LOGZNAC[F.L->Oper-2]);
	 else fputws(L"&#8804;",OUTFILE);
	 break;
	case 4:// >=
	 if(DOS) fwprintf(OUTFILE,L"%c=",LOGZNAC[F.L->Oper-2]);
	 else fputws(L"&#8805;",OUTFILE);
	 break;
	case 5:// /=
	 if(DOS) fwprintf(OUTFILE,L"%c=",LOGZNAC[F.L->Oper-2]);
	 else fputws(L"&#8800;",OUTFILE);
	 break;
	case 6:// ==
	 if(DOS) fwprintf(OUTFILE,L"%c=",LOGZNAC[F.L->Oper-2]);
	 else fputws(L"&#8801;",OUTFILE);
	 break;
	case 7:// <<
	 if(DOS) fwprintf(OUTFILE,L"%c%c",LOGZNAC[1],LOGZNAC[1]);
	 else fputws(L"&#60;&#60;",OUTFILE);
	 break;
	case 8:// >>
	 if(DOS) fwprintf(OUTFILE,L"%c=",LOGZNAC[2],LOGZNAC[2]);
	 else fputws(L"&#62;&#62;",OUTFILE);
	 break;
   }
   PF(DOS,OUTFILE,a,0,F.L->V); fputws(L" ? ",OUTFILE); a+=3;
   PF(DOS,OUTFILE,a,0,F.L->FT);
   if(F.L->FF.C!=Pust){
	fputws(DOS&&CPP ? L" : " : L" ! ",OUTFILE);
	a+=3; PF(DOS,OUTFILE,a,0,F.L->FF);
   }
   if(DOS) putwc(')',OUTFILE);
   else switch((Ksk--)%3){
	case 1: putwc(')',OUTFILE); break;
	case 2: putwc(')',OUTFILE); break;//]
	case 0: putwc(')',OUTFILE); break;//}
   }
   a++; break;
  case IPLL:
   switch(F.I->Type){
	case 1: fputws(L"ipl(",OUTFILE); Beg=',';  End=')'; break;
	case 2: Beg='['; End=']';
   }
   Buf=(wchar_t *)calloc(wcslen(F.I->A->Name)+1,SzC);
   wcscpy(Buf,F.I->A->Name);
   if(DOS) StrWINtoDOS(Buf);
   else
	Buf=LiterGrekToHtm(Buf);
   a+=wcslen(Buf);
   if(a>l){ fputwc('\n',OUTFILE); a=0; }
   a=fwprintf(OUTFILE,Buf);
   free(Buf);
   putwc(Beg,OUTFILE); PF(DOS,OUTFILE,a,0,F.I->F); putwc(End,OUTFILE);
   break;
  case SPLN:
   switch(F.P->Type) {
	case 1: fputws(L"spln(",OUTFILE); Beg=',';  End=')'; break;
	default: Beg='['; End=']';
   }
   Buf=(wchar_t *)calloc(wcslen(F.P->A->Name)+1,SzC);
   wcscpy(Buf,F.P->A->Name);
   if(DOS)
	StrWINtoDOS(Buf);
   else
	Buf=LiterGrekToHtm(Buf);
   a+=wcslen(Buf);
   if(a>l){ fputwc('\n',OUTFILE); a=0; }
   a=fwprintf(OUTFILE,Buf);
   free(Buf);
   putwc(Beg,OUTFILE); PF(DOS,OUTFILE,a,0,F.P->F); putwc(End,OUTFILE); break;
  case SUMM:
   fputws(L"summ(",OUTFILE);
   PF(DOS,OUTFILE,a,0,F.S->F); putwc(',',OUTFILE);
   Buf=(wchar_t *)calloc(wcslen(F.S->i->Name)+1,SzC);
   wcscpy(Buf,F.S->i->Name);
   if(DOS)
	StrWINtoDOS(Buf);
   else
	Buf=LiterGrekToHtm(Buf);
   k+=wcslen(Buf);
   fwprintf(OUTFILE,Buf);
   free(Buf);
   putwc(',',OUTFILE); PF(DOS,OUTFILE,a,0,F.S->a);
   putwc(',',OUTFILE); PF(DOS,OUTFILE,a,0,F.S->b);
   putwc(')',OUTFILE); break;
  case INTG:
   fputws(L"intg(",OUTFILE);
   PF(DOS,OUTFILE,a,0,F.G->f); fputws(L", ",OUTFILE);
   Buf=(wchar_t *)calloc(wcslen(F.G->x->Name)+1,SzC);
   wcscpy(Buf,F.G->x->Name);
   if(DOS)
	StrWINtoDOS(Buf);
   else
	Buf=LiterGrekToHtm(Buf);
   k+=wcslen(Buf);
   fwprintf(OUTFILE,Buf);
   free(Buf);
   if(F.G->a!=Pust){
	fputws(L" , ",OUTFILE); PF(DOS,OUTFILE,a,0,F.G->a);
	fputws(L" , ",OUTFILE); PF(DOS,OUTFILE,a,0,F.G->b);
   }
   fputws(L" ) ",OUTFILE);
   if(F.G->F!=Pust){
	fputws(L" = ",OUTFILE); PF(DOS,OUTFILE,a,0,F.G->F);
	if(F.G->Rez!=Pust){
	 fputws(L" = ",OUTFILE); PF(DOS,OUTFILE,a,0,F.G->Rez);
   }}
   break;
  case RUUT:
   fputws(L"root(",OUTFILE);
   PF(DOS,OUTFILE,a,0,F.R->f); fputws(L", ",OUTFILE);
   Buf=(wchar_t *)calloc(wcslen(F.R->x->Name)+1,SzC);
   wcscpy(Buf,F.R->x->Name);
   if(DOS)
	StrWINtoDOS(Buf);
   else
	Buf=LiterGrekToHtm(Buf);
   k+=wcslen(Buf);
   fwprintf(OUTFILE,Buf);
   free(Buf);
   if(F.R->a!=Pust){
	fputws(L" , ",OUTFILE); PF(DOS,OUTFILE,a,0,F.R->a);
	fputws(L" , ",OUTFILE); PF(DOS,OUTFILE,a,0,F.R->b);
   }
   fputws(L" ) ",OUTFILE);
   break;
  case REST:
   fputws(L"rest( ",OUTFILE);
   PF(DOS,OUTFILE,a,0,F.O->a); fputws(L", ",OUTFILE);
   PF(DOS,OUTFILE,a,0,F.O->b);
   fputws(L" ) ",OUTFILE);
   break;
  case ARRA:
   if(F.A->F){
	if(F.A->F[0].C->Atr==VARY){
	 fwprintf(OUTFILE,L"%s = ",F.A->F[0].V->Name);
	 PF(DOS,OUTFILE,a,0,F.A->F[0].V->Znach);
	}
	else
	 PF(DOS,OUTFILE,a,0,F.A->F[0]);
	for(int i=1;i<F.A->N;i++){
	 fputws(L", ",OUTFILE);
	 if(F.A->F[i].C->Atr==VARY){
	  fwprintf(OUTFILE,L"%s = ",F.A->F[i].V->Name);
	  PF(DOS,OUTFILE,a,0,F.A->F[i].V->Znach);
	 }
	 else
	  PF(DOS,OUTFILE,a,0,F.A->F[i]);
   }}
   else if(F.A->A){
	for(int i=0;i<F.A->N-1;i++)
	 fwprintf(OUTFILE,L"%g, ",F.A->A[i]);
	fwprintf(OUTFILE,L"%g",F.A->A[F.A->N-1]);
   }
   fputws(L";\n",OUTFILE); if(!DOS) fputws(L"<br />",OUTFILE);
   break;
  case ARRI:
   fwprintf(OUTFILE,L"%s[",F.a->A->Name);
   if(F.a->FInd.C->Atr==CNST){
	if(F.a->FInd.C->Name)
	 fwprintf(OUTFILE,L"%s",F.a->FInd.C->Name);
	else
	 fwprintf(OUTFILE,L"%d",Round(F.a->FInd.C->Val));
   }
   else PF(DOS,OUTFILE,a,0,F.a->FInd);
   fputwc(']',OUTFILE);
   break;
}}
//---------------------------------------------------------------------------
void UtilVars(Form F,Magazine **VarsInForm,bool ViewVal){
 Form FF; wchar_t *s,NameV[256]; Vary *V; int Ind;
//Процедура заполнения магазина VarsInForm именами переменных, входящих
//в формулу F и в переменные и операции, входящие в эту формулу
 switch(F.C->Atr) {
  case PUST:
  case CNST: break;
  case VARY:
   if(s=wcschr(F.V->Name,SimbInd)){
	*s='\0';
	if(FindVary(s+1,&V)){
	 Ind=Round(V->P>=0?V->Val:Val(V->Znach));
	 swprintf(NameV,L"%s%d",F.V->Name,Ind);
	 if(FindVary(NameV,&V)){
	  if(ViewVal) swprintf(Inf,L"%s=%s=%g",F.V->Name,V->Name,V->Val);
	  else swprintf(Inf,L"%s=%s",F.V->Name,V->Name);
	 }
	 else swprintf(Inf,L"%s=%s???",F.V->Name,NameV);
	}
	else swprintf(Inf,L"%s???",F.V->Name);
	*s=SimbInd;
   }
   else{
	if(ViewVal) swprintf(Inf,L"%s=%g",F.V->Name,F.V->Val);
	else swprintf(Inf,L"%s",F.V->Name);
   }
   TakeMagazine(VarsInForm,Inf);
   UtilVars(F.V->Znach,VarsInForm,ViewVal); break;
  case UNOP: UtilVars(F.U->Arg,VarsInForm,ViewVal); break;
  case BNOP:
   UtilVars(F.B->L,VarsInForm,ViewVal); UtilVars(F.B->R,VarsInForm,ViewVal);
   break;
  case IFOP:
   UtilVars(F.L->U,VarsInForm,ViewVal);  UtilVars(F.L->V,VarsInForm,ViewVal);
   UtilVars(F.L->FT,VarsInForm,ViewVal); UtilVars(F.L->FF,VarsInForm,ViewVal);
   break;
  case IPLL:
   UtilVars(F.I->F,VarsInForm,ViewVal); break;
  case SPLN:
   UtilVars(F.P->F,VarsInForm,ViewVal); break;
  case SUMM:
   FF.V=F.S->i; UtilVars(FF,VarsInForm,ViewVal);
   UtilVars(F.S->a,VarsInForm,ViewVal); UtilVars(F.S->b,VarsInForm,ViewVal);
   UtilVars(F.S->F,VarsInForm,ViewVal); break;
  case INTG:
   FF.V=F.G->x; UtilVars(FF,VarsInForm,ViewVal);
   UtilVars(F.G->a,VarsInForm,ViewVal); UtilVars(F.G->b,VarsInForm,ViewVal);
   UtilVars(F.G->f,VarsInForm,ViewVal); break;
  case ARRI:
   if(F.a->FInd.C->Atr!=PUST)
	UtilVars(F.a->FInd,VarsInForm,ViewVal);
   break;
}}
//---------------------------------------------------------------------------
int FindCordDefVars(Cord *R,Vary *V,Cord **C){
 if(!(*C=R)) return 0; *C=NULL;
 for(Cord *c=R;c;c=c->Sled){
  if((c->V==V)||!wcscmp(c->V->Name,V->Name)){ *C=c; return 1; }
  if(VarInForm(true,V->Znach,c->V)) *C=c;
 }
 return 0;
}
//---------------------------------------------------------------------------
void TakeCordDefVars(Cord **R,Vary *V){
 Cord *C,*c;
 if(FindCordDefVars(*R,V,&C)) return;
 c=(Cord *)calloc(1,sizeof(Cord)); c->V=V;
 if(!C){ c->Sled=*R;           *R=c; }
 else  { c->Sled=C->Sled; C->Sled=c; }
}
//---------------------------------------------------------------------------
void VarsFromFormDefVars(Form F,Cord **VarsInForm,Cord *Exclus){
 Form FF; wchar_t *s,NameV[256]; Vary *V; int Ind; Cord *K;
//Процедура заполнения списка VarsInForm именами переменных, входящих
//в формулу F и в переменные и операции, входящие в эту формулу
 switch(F.C->Atr) {
  case PUST: case CNST: break;
  case VARY:
//исключаем обобщенные координаты, независимые обобщенные скорости и псевдоскорости
   if((F.V->Znach.C!=Pust)&&!FindCordDefVars(Exclus,F.V,&K)){
	VarsFromFormDefVars(F.V->Znach,VarsInForm,Exclus);
	TakeCordDefVars(VarsInForm,F.V);
   }
   break;
  case UNOP: VarsFromFormDefVars(F.U->Arg,VarsInForm,Exclus); break;
  case BNOP:
   VarsFromFormDefVars(F.B->L,VarsInForm,Exclus);
   VarsFromFormDefVars(F.B->R,VarsInForm,Exclus);
   break;
  case IFOP:
   VarsFromFormDefVars(F.L->U,VarsInForm,Exclus);
   VarsFromFormDefVars(F.L->V,VarsInForm,Exclus);
   VarsFromFormDefVars(F.L->FT,VarsInForm,Exclus);
   VarsFromFormDefVars(F.L->FF,VarsInForm,Exclus);
   break;
  case IPLL:
   VarsFromFormDefVars(F.I->F,VarsInForm,Exclus); break;
  case SPLN:
   VarsFromFormDefVars(F.P->F,VarsInForm,Exclus); break;
  case SUMM:
   FF.V=F.S->i; VarsFromFormDefVars(FF,VarsInForm,Exclus);
   VarsFromFormDefVars(F.S->a,VarsInForm,Exclus);
   VarsFromFormDefVars(F.S->b,VarsInForm,Exclus);
   VarsFromFormDefVars(F.S->F,VarsInForm,Exclus); break;
  case INTG:
   FF.V=F.G->x; VarsFromFormDefVars(FF,VarsInForm,Exclus);
   VarsFromFormDefVars(F.G->a,VarsInForm,Exclus);
   VarsFromFormDefVars(F.G->b,VarsInForm,Exclus);
   VarsFromFormDefVars(F.G->f,VarsInForm,Exclus); break;
  case ARRI:
   if(F.a->FInd.C->Atr!=PUST)
	VarsFromFormDefVars(F.a->FInd,VarsInForm,Exclus);
   break;
}}
//---------------------------------------------------------------------------
void VarsFromForm(Form F,Magazine **VarsInForm){
 Form FF; wchar_t *s,NameV[256]; Vary *V; int Ind;
//Процедура заполнения магазина VarsInForm именами переменных, входящих
//в формулу F и в переменные и операции, входящие в эту формулу
 switch(F.C->Atr) {
  case PUST: case CNST: break;
  case VARY:
/*   if(s=wcschr(F.V->Name,SimbInd)){
	*s='\0';
	if(FindVary(s+1,&V)){
	 Ind=Round(V->P>=0?V->Val:Val(V->Znach));
	 swprintf(NameV,L"%s%d",F.V->Name,Ind);
	 if(FindVary(NameV,&V))
	  swprintf(Inf,L"%s=%s",F.V->Name,V->Name);
	 else swprintf(Inf,L"%s=%s",F.V->Name,NameV);
	}
	else swprintf(Inf,L"%s",F.V->Name);
	*s=SimbInd;
   }
   else
	swprintf(Inf,L"%s",F.V->Name);
   TakeMagazine(VarsInForm,Inf);*/
//исключаем обобщенные координаты, независимые обобенные скорости и псевдоскорости
   if(F.V->Znach.C!=Pust){
	TakeMagazine(VarsInForm,F.V->Name);
	VarsFromForm(F.V->Znach,VarsInForm);
   }
   break;
  case UNOP: VarsFromForm(F.U->Arg,VarsInForm); break;
  case BNOP:
   VarsFromForm(F.B->L,VarsInForm); VarsFromForm(F.B->R,VarsInForm);
   break;
  case IFOP:
   VarsFromForm(F.L->U,VarsInForm);  VarsFromForm(F.L->V,VarsInForm);
   VarsFromForm(F.L->FT,VarsInForm); VarsFromForm(F.L->FF,VarsInForm);
   break;
  case IPLL:
   VarsFromForm(F.I->F,VarsInForm); break;
  case SPLN:
   VarsFromForm(F.P->F,VarsInForm); break;
  case SUMM:
   FF.V=F.S->i; VarsFromForm(FF,VarsInForm);
   VarsFromForm(F.S->a,VarsInForm); VarsFromForm(F.S->b,VarsInForm);
   VarsFromForm(F.S->F,VarsInForm); break;
  case INTG:
   FF.V=F.G->x; VarsFromForm(FF,VarsInForm);
   VarsFromForm(F.G->a,VarsInForm); VarsFromForm(F.G->b,VarsInForm);
   VarsFromForm(F.G->f,VarsInForm); break;
  case ARRI:
   if(F.a->FInd.C->Atr!=PUST)
	VarsFromForm(F.a->FInd,VarsInForm);
   break;
}}
//---------------------------------------------------------------------------
void MagVars(Form F,Magazine **VarsInForm){
 Form FF; wchar_t *s,NameV[256]; Vary *V; int Ind;
//Процедура заполнения магазина VarsInForm именами переменных,
//непосредственно входящих в формулу F без анализа переменных,
//входящих в эту формулу
 switch(F.C->Atr) {
  case PUST:
  case CNST: break;
  case VARY: TakeMagazine(VarsInForm,F.V->Name); break;
  case UNOP: MagVars(F.U->Arg,VarsInForm); break;
  case BNOP:
   MagVars(F.B->L,VarsInForm); MagVars(F.B->R,VarsInForm);
   break;
  case IFOP:
   MagVars(F.L->U,VarsInForm);  MagVars(F.L->V,VarsInForm);
   MagVars(F.L->FT,VarsInForm); MagVars(F.L->FF,VarsInForm);
   break;
  case IPLL:
   MagVars(F.I->F,VarsInForm); break;
  case SPLN:
   MagVars(F.P->F,VarsInForm); break;
  case SUMM:
   FF.V=F.S->i; MagVars(FF,VarsInForm);
   MagVars(F.S->a,VarsInForm); MagVars(F.S->b,VarsInForm);
   MagVars(F.S->F,VarsInForm);
   break;
  case INTG:
   FF.V=F.G->x; MagVars(FF,VarsInForm);
   MagVars(F.G->a,VarsInForm); MagVars(F.G->b,VarsInForm);
   MagVars(F.G->f,VarsInForm); break;
  case ARRI:
   if(F.a->FInd.C->Atr!=PUST)
	MagVars(F.a->FInd,VarsInForm);
   break;
}}
//---------------------------------------------------------------------------
int PrintForm(bool DOS,FILE *OUTFILE,wchar_t *NameF,Form F){
 Vary *V; k=0; Ksk=0; l=DOS?72:StrLong; //FILE *FFF;
//if(FFF=_w_wfopen(L"PrintForm.txt",L"w")){ fputws(NameF,FFF); fclose(FFF); }
 if(NameF){
  if(DOS){ Fputs(NameF,OUTFILE); k=wcslen(NameF); }
  else{
   wchar_t *Buf=(wchar_t *)calloc(wcslen(NameF)+1,SzC);
   wcscpy(Buf,NameF); Buf=LiterGrekToHtm(Buf);
   fwprintf(OUTFILE,L"<b>%s</b>",Buf);
   free(Buf);
  }
  if(F.C->Atr==ARRA)
   k+=fwprintf(OUTFILE,L"[%d]",F.A->N);
  fputws(L" = ",OUTFILE); k+=3;
 }
 PF(DOS,OUTFILE,k,0,F);
 if(NameF){
  if(FindVary(NameF,&V)&&(F==V->Znach)&&(V->Razm.C!=Pust)){
   fputws(L", ",OUTFILE); k+=2;
   PF(DOS,OUTFILE,k,0,V->Razm);
  }
  if(F.C->Atr!=ARRA)
   putwc(';',OUTFILE);
  k++;
 }
 return k;
}
//---------------------------------------------------------------------------
int PrintFormToVars(FILE *OUTFILE,wchar_t *NameF,Form F,Magazine *ToV){
// процедура пишет в файл OUTFILE строковый образ формулы F;
// печатает ее в виде <Name> = <формула>;, если Name&&*Name!=0,
// если Name!=NULL и *Name='\0', то в виде \n<формула>,
// иначе в виде <формула>.
// затем выводятся формулы для всех переменных, вошедших в формулу F
// и ее формулы и операции кроме переменных из списка Tov                  */
 wchar_t Buf[1024]; Vary *V; k=0; Ksk=0; l=72; FILE *FFF; Magazine *Mv=NULL;
 if(NameF){
  Fputs(NameF,OUTFILE); fputws(L" = ",OUTFILE); k=wcslen(NameF)+3;
 }
 VarsFromForm(F,&Mv);
 for(Magazine *m=ToV;m;m=m->Sled) DelMagazineCmp(&Mv,m->S);
 PF(true,OUTFILE,k,0,F);
 if(NameF){
  fputws(L";\n",OUTFILE); k++;
 }
 for(Magazine *m=Mv;m;m=m->Sled)
  if(FindVary(m->S,&V)){
   PrintForm(true,OUTFILE,m->S,V->Znach);
   putwc('\n',OUTFILE); k++;
  } 
 return k;
}
//---------------------------------------------------------------------------
void KOp(Form F,int *KolOp){
 switch(F.C->Atr){
  case PUST: case CNST: break;
  case VARY:
   if(!F.V->O){ KOp(F.V->Znach,KolOp); F.V->O=1; } break;
  case UNOP:
   if(!F.U->O){
    if(F.U->Func!=1) (KolOp[5])++;
    KOp(F.U->Arg,KolOp); F.U->O=1;
   }
   break;
  case BNOP :
   switch(F.B->Oper){
    case 1:         (KolOp[0])++; break;
    case 2:         (KolOp[1])++; break;
    case 3:         (KolOp[2])++; break;
    case 4: case 5: (KolOp[3])++; break;
    case 6:         (KolOp[4])++; break;
   }
   KOp(F.B->L,KolOp);
   KOp(F.B->R,KolOp);
//  case IFOP: case IPLL: case SPLN: case SUMM: case INTG: break;
  default: break;
}}
//---------------------------------------------------------------------------
void ClearOper(void){
 L->Time->O=L->Freq->O=0;
 for(Vary *V=L->V;V;V=V->Sled) V->O=0;
 for(Unop *U=L->U;U;U=U->Sled) U->O=0;
}
//---------------------------------------------------------------------------
void PrintOper(bool DOS,FILE *OUTFILE,Form F,int *KolOp){
 wchar_t Buf[256]; int KO[]={0,0,0,0,0,0};
 KOp(F,KO);
 swprintf(Buf,
  L"\nОпераций: \"+\"=%d, \"-\"=%d, \"*\"=%d, \":\"=%d, \"^\"=%d, \"функций\"=%d",
  KO[0],KO[1],KO[2],KO[3],KO[4],KO[5]);
 if(DOS){ Fputs(Buf,OUTFILE); /*fputwc('\n',OUTFILE);*/ }
 else{ fputws(Buf,OUTFILE); /*fputws(L"<br>",OUTFILE);*/ }
 if(KolOp) for(int i=0;i<6;i++) KolOp[i]+=KO[i];
}
//---------------------------------------------------------------------------
void PrintOper(TStringList *SLH,Form F,int *KolOp){
 wchar_t Buf[256]; int KO[]={0,0,0,0,0,0};
 KOp(F,KO);
 swprintf(Buf,L"Операций: \"+\"=%d, \"-\"=%d, \"*\"=%d, \":\"=%d,"
  L" \"^\"=%d, \"функций\"=%d",KO[0],KO[1],KO[2],KO[3],KO[4],KO[5]);
 SLH->Add(Buf);
 if(KolOp) for(int i=0;i<6;i++) KolOp[i]+=KO[i];
}
//---------------------------------------------------------------------------
bool FindCord(Cord *VaryList,wchar_t *S,Cord **C){
 if(!(*C=VaryList)) return false;
 for(;;*C=(*C)->Sled){
  if(!wcscmp((*C)->V->Name,S)) return true;
  if(!(*C)->Sled) return false;
}}
//---------------------------------------------------------------------------
Cord *TakeCord(Cord **VaryList,wchar_t *S,Form F){
 Cord *V,*W; Vary *v;
 if(!S||!*S) return NULL;
 Compress(S);
 if(FindCord(*VaryList,S,&V)) return V;
 v=TakeVary(S); v->Znach=F;
 W=(Cord *)calloc(1,sizeof(Cord)); W->V=v;
 return (V?V->Sled:*VaryList)=W;
}
//---------------------------------------------------------------------------
wchar_t *UnOpToNameVary(wchar_t *NameVary,wchar_t *NameFunc,wchar_t *Arg){//делаем имя
 wchar_t *inf=Inf,*a;
 for(a=Arg;*a;a++){
  switch(*a){
   case '+': inf+=swprintf(inf,L"_plus_"); break;
   case '-': inf+=swprintf(inf,L"_minus_"); break;
   case '*': inf+=swprintf(inf,L"_multi_"); break;
   case '/': inf+=swprintf(inf,L"_divide_"); break;
   case '^': inf+=swprintf(inf,L"_elevate_"); break;
   default: *inf++=*a;
  }
  *inf='\0';
 }
 swprintf(NameVary,L"%s_%s",NameFunc,Inf);
 return NameVary;
}
//---------------------------------------------------------------------------
void UnOpToVary(Form F,Cord **UnOps){
//Если встречаем унарную операцию (кроме минуса), генерируем переменную с именем
//состоящем из имени операции и аргумента в виде строки. Таким образом, после
//обработки всех формул будем иметь список разных унарных операций
//Дальше нужно в этих формулах заменить унарные операции на эти переменные
//со значениями соответствующих унарных операций
 switch(F.C->Atr){
  case PUST: case CNST: break;
  case VARY:
   if(!F.V->O){//если переменная еще не обрабатывалась
    UnOpToVary(F.V->Znach,UnOps);
    F.V->O=1;
   }
   break;
  case UNOP:
   if(!F.U->O){//если эта унарная операция еще не обрабатывалась
    UnOpToVary(F.U->Arg,UnOps);
    if(F.U->Func>1){//если это не унарный минус
	 String Arg; wchar_t NameVary[256]; //Vary *V;
     FormToStr(F.U->Arg,&Arg);
	 UnOpToNameVary(NameVary,NameFunc[F.U->Func],Arg.w_str());//делаем имя
     TakeCord(UnOps,NameVary,F);//порождаем переменную в списке UnOps
    }
    F.U->O=1;
   }
   break;
  case BNOP :
   UnOpToVary(F.B->L,UnOps);
   UnOpToVary(F.B->R,UnOps);
//  case IFOP: case IPLL: case SPLN: case SUMM: case INTG: break;
  default: break;
}}
//---------------------------------------------------------------------------
Form UnOpRepVary(Form F,Cord *UnOps){ Form f;
//Замена в F унарные операции на переменные, равные этим операциям
 switch(F.C->Atr){
  case CNST: case PUST: break;
  case VARY:
   if(!F.V->U){//еще не обрабатывалась
    F.V->Znach=UnOpRepVary(F.V->Znach,UnOps);
    F.V->U=1;
   }
   break;
  case UNOP:
   if(F.U->Func>1){
	String Arg; wchar_t NameVary[256]; Vary *V; Cord *q;
	FormToStr(F.U->Arg,&Arg);
    UnOpToNameVary(NameVary,NameFunc[F.U->Func],Arg.c_str());//делаем имя
    if(FindCord(UnOps,NameVary,&q))
     F.V=q->V;
   }
   else{
    F.U->Arg=UnOpRepVary(F.U->Arg,UnOps);
    F.U->O=1;
   }
   break;
  case BNOP:
   F.B->L=UnOpRepVary(F.B->L,UnOps);
   F.B->R=UnOpRepVary(F.B->R,UnOps);
   break;
  case IFOP:
   F.L->U=UnOpRepVary(F.L->U,UnOps);
   F.L->V=UnOpRepVary(F.L->V,UnOps);
   F.L->FT=UnOpRepVary(F.L->FT,UnOps);
   F.L->FF=UnOpRepVary(F.L->FF,UnOps);
   break;
  case SUMM:
   f.V=F.S->i; f=UnOpRepVary(f,UnOps); //F.S->i=f.V;
   F.S->a=UnOpRepVary(F.S->a,UnOps);
   F.S->b=UnOpRepVary(F.S->b,UnOps);
   F.S->F=UnOpRepVary(F.S->F,UnOps);
   break;
  case INTG:
   f.V=F.G->x; f=UnOpRepVary(f,UnOps); //F.G->x=f.V;
   F.G->a=UnOpRepVary(F.G->a,UnOps);
   F.G->b=UnOpRepVary(F.G->b,UnOps);
   F.G->f=UnOpRepVary(F.G->f,UnOps);
   F.G->F=UnOpRepVary(F.G->F,UnOps);
   F.G->Rez=UnOpRepVary(F.G->Rez,UnOps);
   break;
  case IPLL:
   F.I->F=UnOpRepVary(F.I->F,UnOps);
   break;
  case SPLN:
   F.P->F=UnOpRepVary(F.P->F,UnOps);
   break;
  case ROUT:
   f.V=F.R->x; f=UnOpRepVary(f,UnOps); //F.R->x=f.V;
   F.R->f=UnOpRepVary(F.R->f,UnOps);
   F.R->a=UnOpRepVary(F.R->a,UnOps);
   F.R->b=UnOpRepVary(F.R->b,UnOps);
   break;
  case REST:
   F.O->a=UnOpRepVary(F.O->a,UnOps);
   F.O->b=UnOpRepVary(F.O->b,UnOps);
   break;
  case ARRI:
   F.a->FInd=UnOpRepVary(F.a->FInd,UnOps);
   break;
  case DIFF:
   f.V=F.D->Y; f=UnOpRepVary(f,UnOps); //F.D->Y=f.V;
   f.V=F.D->X; f=UnOpRepVary(f,UnOps); //F.D->X=f.V;
   break;
  case ARRA:
   F.A->Znach=UnOpRepVary(F.A->Znach,UnOps);
   if(F.A->F)
    for(int i=0;i<F.A->N;i++)
     F.A->F[i]=UnOpRepVary(F.A->F[i],UnOps);
   break;
 }
 return F;
}
//---------------------------------------------------------------------------
void FormToStr(Form F,int Prio,String &S){
 int i; wchar_t Beg,End; wchar_t *Buf;
 switch(F.C->Atr){
  case PUST: S+=L"$"; break;
  case CNST:
   if(F.C->Name){
	Buf=(wchar_t *)calloc(wcslen(F.C->Name)+1,SzC);
	wcscpy(Buf,F.C->Name); Buf=LiterGrekToHtm(Buf);
	S+=Buf;	free(Buf);
   }
   else{ swprintf(Inf,L"%g",F.C->Val); S+=Inf; }
   break;
  case VARY:
   Buf=(wchar_t *)calloc(wcslen(F.V->Name)+1,SzC);
   wcscpy(Buf,F.V->Name); Buf=LiterGrekToHtm(Buf);
   S+=Buf; free(Buf);
   break;
  case UNOP:
   if(F.U->Func==2){ swprintf(Inf,L"&#8730;"); S+=Inf; }
   else S+=NameFunc[F.U->Func];
   if((F.U->Func==1)&&
	  ((F.U->Arg.C->Atr==CNST)||(F.U->Arg.C->Atr==VARY)||
	   ((F.U->Arg.C->Atr==UNOP)&&(F.U->Arg.U->Func!=1))||
	   (F.U->Arg.C->Atr==IPLL)||(F.U->Arg.C->Atr==SPLN))){
	FormToStr(F.U->Arg,0,S);
   }
   else{
	switch((++Ksk)%3){
	 case 1: S+=L"("; break;
	 case 2: S+=L"["; break;
	 case 0: S+=L"{"; break;
	}
	FormToStr(F.U->Arg,0,S);
	switch((Ksk--)%3){
	 case 1: S+=L")"; break;
	 case 2: S+=L"]"; break;
	 case 0: S+=L"}"; break;
   }}
   break;
  case BNOP : i=F.B->Oper;
   if((i<Prio)||((i==Prio)&&((i==2)||(i==4)||(i==5)))){
	switch((++Ksk)%3){
	 case 1: S+=L"("; break;
	 case 2: S+=L"["; break;
	 case 0: S+=L"{"; break;
   }}
   FormToStr(F.B->L,i,S);
   swprintf(Inf,i<3?L" %c ":L"%c",ZNAC[i]); S+=Inf;
   FormToStr(F.B->R,i,S);
   if((i<Prio)||((i==Prio)&&((i==2)||(i==4)||(i==5)))){
	switch((Ksk--)%3){
	 case 1: S+=L")"; break; case 2: S+=L"]"; break; case 0:
	  S+=L"}"; break;
   }}
   break;
  case IFOP:
   switch((++Ksk)%3){
	case 1: S+=L"("; break;
	case 2: S+=L"["; break;
	case 0: S+=L"{"; break;
   }
   FormToStr(F.L->U,0,S);
   if(F.L->Oper<3){
	swprintf(Inf,L"%c",LOGZNAC[F.L->Oper]); S+=Inf;
   }
   else if(F.L->Oper<7){
	swprintf(Inf,L"%c=L",LOGZNAC[F.L->Oper-2]); S+=Inf;
   }
   else{
	swprintf(Inf,L"%c%c",
	 LOGZNAC[F.L->Oper-6],LOGZNAC[F.L->Oper-6]);
	S+=Inf;
   }
   FormToStr(F.L->V,0,S); S+=L" ? ";
   FormToStr(F.L->FT,0,S);
   if(F.L->FF.C!=Pust){ S+=L" ! "; FormToStr(F.L->FF,0,S); }
   switch((Ksk--)%3){
	case 1: S+=L")"; break;
	case 2: S+=L"]"; break;
	case 0: S+=L"}"; break;
   }
   break;
  case IPLL:
   switch(F.I->Type){
	case 1: S+=L"ipl("; Beg=',';  End=')'; break;
	case 2: Beg='['; End=']';
   }
   S+=F.I->A->Name; swprintf(Inf,L"%c",Beg); S+=Inf;
   FormToStr(F.I->F,0,S); swprintf(Inf,L"%c",End); S+=Inf;
   break;
  case SPLN:
   switch(F.P->Type){
	case 1: S+=L"spln("; Beg=',';  End=')'; break;
	case 2: Beg='['; End=']';
   }
   S+=F.P->A->Name; swprintf(Inf,L"%c",Beg); S+=Inf;
   FormToStr(F.P->F,0,S); swprintf(Inf,L"%c",End); S+=Inf;
   break;
  case SUMM:
   S+=L"summ("; S+=F.S->i->Name;
   S+=L","; FormToStr(F.S->a,0,S);
   S+=L","; FormToStr(F.S->b,0,S);
   S+=L","; FormToStr(F.S->F,0,S);
   S+=L")";
   break;
  case INTG:
   S+=L"intg("; FormToStr(F.G->f,0,S);
   S+=L","; S+=F.G->x->Name;
   S+=L","; FormToStr(F.G->a,0,S);
   S+=L","; FormToStr(F.G->b,0,S);
   S+=L")";
   break;
  case RUUT:
   S+=L"root("; FormToStr(F.R->f,0,S);
   S+=L","; S+=F.R->x->Name;
   S+=L","; FormToStr(F.R->a,0,S);
   S+=L","; FormToStr(F.R->b,0,S);
   S+=L")";
   break;
  case REST:
   S+=L"rest("; FormToStr(F.O->a,0,S);
   S+=L","; FormToStr(F.O->b,0,S);
   S+=L")";
   break;
  case ARRA:
//   if(F.A->Znach.C!C=Pust){
//	FormToStr(F.A->Znach,0,S); S+=L"=";
//   }
   S+=L"{";
   if(F.A->F){
	if(F.A->F[0].C->Atr==VARY){
	 swprintf(Inf,L"%s=",F.A->F[0].V->Name); S+=Inf;
	 FormToStr(F.A->F[0].V->Znach,0,S);
	}
	else
	 FormToStr(F.A->F[0],0,S);
	for(int i=1;i<F.A->N;i++){
	 S+=L", ";
	 if(F.A->F[i].C->Atr==VARY){
	  swprintf(Inf,L"%s=",F.A->F[i].V->Name); S+=Inf;
	  FormToStr(F.A->F[i].V->Znach,0,S);
	 }
	 else
	  FormToStr(F.A->F[i],0,S);
   }}
   else if(F.A->A){
	for(int i=0;i<F.A->N-1;i++){
	 swprintf(Inf,L"%g, ",F.A->A[i]); S+=Inf;
	}
	swprintf(Inf,L"%g",F.A->A[F.A->N-1]); S+=Inf;
   }
   S+=L"}";
   break;
  case ARRI:
   S+=F.a->A->Name;
   if(F.a->FInd.C->Atr==CNST){
	if(F.a->FInd.C->Name)
	 S+=F.a->FInd.C->Name;
	else
	 S+=Round(F.a->FInd.C->Val);
   }
   else FormToStr(F.a->FInd,0,S);
   S+=L"]";
   break;
}}
//---------------------------------------------------------------------------
String *FormToStr(Form F,String *S){
 String aS; Ksk=0;
 FormToStr(F,0,aS);
 *S=aS;
 return S;
}
//---------------------------------------------------------------------------
wchar_t *ZamStrich(wchar_t *S){
 static wchar_t Buf[256]; wchar_t *b=Buf;
 for(;*S;S++)
  switch(*S){
   case '\'': b+=swprintf(b,L"_po_"); break;
   case '@':  b+=swprintf(b,L"_at_"); break;
   case '°':  b+=swprintf(b,L"_g"); break;
   case '$':                        break;
//   case '~':  *b++='_';             break;
   case '~':  b+=swprintf(b,L"_td_"); break;
   default:   *b++=*S;              break;
  }
 *b='\0'; return Buf;
}
//---------------------------------------------------------------------------
void WF(FILE *Fo,int a,int Prio,Form F) {
//процедура печати формул с заменой имен обобщенныхх координат и скоростей,
//псевдоскоростей на индексированные элементы массива RK_w
//обобщенные координаты с независимыми скоростями - RK_w[i+N+NPV],
//обобщенные независимые скорости - RK_w[i],
//псевдоскорости - RK_w[i+N],
//обобщенные координаты с зависимыми скоростями - RK_w[i+2*N+NPV].
 wchar_t *s,*b,Beg,End,Buf[1024]; int i;
 switch(F.C->Atr){
  case PUST: putwc('$',Fo); k++; break;
  case CNST:
   if(F.C->Name)
	wcscpy(Buf,F.C->Name);
   else
	swprintf(Buf,L"%g",F.C->Val);
   if(!wcschr(Buf,'.')&&!wcschr(Buf,'E')&&!wcschr(Buf,'e'))
	k+=fwprintf(Fo,L"%0.1f",_wtof(Buf));
   else k+=fwprintf(Fo,Buf);
   break;
  case VARY:
   if((i=IndexOK(F.V->Name))>-1) i+=N+NPV;       //ОК с независимыми скоростями
   else if((i=IndexOV(F.V->Name))>-1) i=i;       //независимые скорости
   else if((i=IndexW(F.V->Name))>-1) i+=N;       //псевдоскорости
   else if((i=IndexZ(F.V->Name))>-1) i+=2*N+NPV; //ОК с зависимыми скоростями
   else i=-1;
   if(i>-1) k+=fwprintf(Fo,L"RK_w[%d]",i);
   else if(REPVARNACNST&&(F.V->Znach.C->Atr==CNST))
	k+=fwprintf(Fo,L"%g",F.V->Znach.C->Val);
//   else if(F.V->Znach.C->Atr==VARY)
//    k+=fwprintf(Fo,F.V->Znach.V->Name);
   else
	k+=fwprintf(Fo,ZamStrich(F.V->Name));
   break;
  case UNOP:
   if(F.U->Func==18){//факториал!
	if((F.U->Arg.C->Atr!=CNST)&&(F.U->Arg.C->Atr!=VARY)){
	 fputwc('(',Fo); k++;
	}
	WF(Fo,a,0,F.U->Arg);
	if((F.U->Arg.C->Atr!=CNST)&&(F.U->Arg.C->Atr!=VARY)){
	 fputwc(')',Fo); k++;
	}
	k+=fwprintf(Fo,NameFunc[F.U->Func]);
   }
   else{
	k+=fwprintf(Fo,NameFunc[F.U->Func]);
	if((F.U->Func==1)&&
	   ((F.U->Arg.C->Atr==CNST)||(F.U->Arg.C->Atr==VARY)||
		((F.U->Arg.C->Atr==UNOP)&&(F.U->Arg.U->Func!=1))||
		((F.U->Arg.C->Atr==BNOP)&&(F.U->Arg.B->Oper>2))||
		(F.U->Arg.C->Atr==IPLL)||(F.U->Arg.C->Atr==SPLN))){
	 WF(Fo,a,0,F.U->Arg);
	}
	else{
	 fputwc('(',Fo); Ksk++;
	 WF(Fo,a,0,F.U->Arg);
	 fputwc(')',Fo); k+=2; Ksk--;
   }}
   break;
  case BNOP : i=F.B->Oper;
   if((i-1)/2<(Prio-1)/2){//24.02.2013
	fputwc('(',Fo); k++; Ksk++;
   }
   if(i==6)
	k+=fwprintf(Fo,L"pow(");
   WF(Fo,a,i,F.B->L);
   if(i==6) fputwc(',',Fo);
   if(i<3){ fputwc(' ',Fo); k++; }
   if(i!=6){ fputwc(ZNAC[i],Fo); k++; }
   if(i<3){ fputwc(' ',Fo); k++; }
   if(F.B->R.C->Atr==INTG){
	fputwc('(',Fo); Ksk++; k++;
   }
   if((i==1)||IsCnst(F.B->R)||IsVary(F.B->R)||(IsUnop(F.B->R)&&!IsMins(F.B->R)))
	WF(Fo,a,i,F.B->R);
   else{
	fputwc('(',Fo); k++; Ksk++;
	WF(Fo,a,i,F.B->R);
	fputwc(')',Fo); k++; Ksk--;
   }
   if(F.B->R.C->Atr==INTG){
	fputwc(')',Fo); Ksk--; k++;
   }
   if(i==6) k+=fwprintf(Fo,L")");
   if((i-1)/2<(Prio-1)/2){
	fputwc(')',Fo); Ksk--; k++;
   }
   break;
  case IFOP:
   Ksk++; fputwc('(',Fo); k++;
   WF(Fo,a,0,F.L->U);
   switch(F.L->Oper){
	case 1:// <
	 fputwc(LOGZNAC[F.L->Oper],Fo); k++;
	 break;
	case 2:// >
	 fputwc(LOGZNAC[F.L->Oper],Fo); k++;
	 break;
	case 3:// <=
	 fwprintf(Fo,L"%c=",LOGZNAC[F.L->Oper-2]); k+=2;
	 break;
	case 4:// >=
	 fwprintf(Fo,L"%c=",LOGZNAC[F.L->Oper-2]); k+=2;
	 break;
	case 5:// \=
	 fwprintf(Fo,L"%c=",LOGZNAC[F.L->Oper-2]); k+=2;
	 break;
	case 6:// ==
	 fwprintf(Fo,L"%c=",LOGZNAC[F.L->Oper-2]); k+=2;
	 break;
	case 7:// <<
	 fwprintf(Fo,L"%c%c",LOGZNAC[1],LOGZNAC[1]); k+=2;
	 break;
	case 8:// >>
	 fwprintf(Fo,L"%c%c",LOGZNAC[2],LOGZNAC[2]); k+=2;
	 break;
   }
   WF(Fo,a,0,F.L->V);  fputws(L" ? ",Fo); k+=3;
   WF(Fo,a,0,F.L->FT);
   if(F.L->FF.C!=Pust){ fputws(L" : ",Fo); k+=3; WF(Fo,a,0,F.L->FF); }
   Ksk--; fputwc(')',Fo); k++;
   break;
}}
//---------------------------------------------------------------------------
int FormToFileWithRep(FILE *Fo,wchar_t *NameF,Form F){
 wchar_t Buf[1024]; Vary *V; k=0; Ksk=0; l=60;
 if(NameF){
  fputws(NameF,Fo); k=wcslen(NameF); fputws(L" = ",Fo); k+=3;
 }
 WF(Fo,k,0,F);
 if(NameF){ fputwc(';',Fo); k++; }
 return k;
}
//---------------------------------------------------------------------------
void PrintMatr(wchar_t *File,wchar_t *Comm,wchar_t *NameMatr,double **M,int N){
 FILE *F=_wfopen(File,L"a"); wchar_t Name[256],*s; int Do=0,Po=0,d,p;
 if(Comm){ swprintf(Name,L"\n%s",Comm); fputws(StrWINtoDOS(Name),F); }
 fwprintf(F,L"%s=\n",NameMatr);
 for(int i=0;i<N;i++){
  for(int j=0;j<N;j++){
   swprintf(Name,L"%g",M[i][j]);
   if(s=wcschr(Name,'.')){
	*s++='\0'; d=wcslen(Name); p=wcslen(s);
	if(d>Do) Do=d;
	if(p>Po) Po=p;
 }}}
 Do+=2+Po; swprintf(Name,L"%%%d.%df ",Do,Po);
 for(int i=0;i<N;i++){
  for(int j=0;j<N;j++)
   fwprintf(F,Name,M[i][j]);
  fputwc('\n',F);
 }
 fclose(F);
}
//---------------------------------------------------------------------------
void PrintVect(wchar_t *File,wchar_t *Comm,wchar_t *NameVect,double *V,int N){
 FILE *F=_wfopen(File,L"a"); wchar_t Name[256],*s; int Do=0,Po=0,d,p;
 if(Comm){ swprintf(Name,L"\n%s",Comm); fputws(StrWINtoDOS(Name),F); }
 fwprintf(F,L"%s=\n",NameVect);
 for(int i=0;i<N;i++){
  swprintf(Name,L"%g",V[i]);
  if(s=wcschr(Name,'.')){
   *s++='\0'; d=wcslen(Name); p=wcslen(s);
   if(d>Do) Do=d;
   if(p>Po) Po=p;
 }}
 Do+=2+Po; swprintf(Name,L"%%%d.%df ",Do,Po);
 for(int i=0;i<N;i++)
  fwprintf(F,Name,V[i]);
 fputwc('\n',F);
 fclose(F);
}
//---------------------------------------------------------------------------
void PrintVectF(wchar_t *File,wchar_t *Comm,wchar_t *NameVect,Form *V,int N){
 FILE *F=_wfopen(File,L"a"); wchar_t Name[256];
 if(Comm){ swprintf(Name,L"\n%s",Comm); fputws(StrWINtoDOS(Name),F); }
 for(int i=0;i<N;i++){
  swprintf(Name,L"%s[%d]",NameVect,i);
  PrintForm(true,F,Name,V[i]);
  fputwc('\n',F);
 }
 fclose(F);
}
//---------------------------------------------------------------------------
void PrintMatrF(wchar_t *File,wchar_t *Comm,wchar_t *NameMatr,
  Form **M,int N){
 FILE *F=_wfopen(File,L"a"); wchar_t Name[256];
 if(Comm){ swprintf(Name,L"\n%s\n",Comm); fputws(StrWINtoDOS(Name),F); }
 for(int i=0;i<N;i++){
  for(int j=0;j<N;j++){
   swprintf(Name,L"%s[%d][%d]",NameMatr,i,j);
   PrintForm(true,F,Name,M[i][j]);
   fputwc('\n',F);
  }
  fputwc('\n',F);
 }
 fclose(F);
}
//---------------------------------------------------------------------------
wchar_t *FormToStringList(Form F,wchar_t *NameVary,wchar_t *Ln,
  wchar_t *l,TStringList *SL,bool EndLn,String *SF){
//F - формула для строкового представления
//NameVary - имя переменной слева от формулы или NULL
//Ln - Накапливаемая строка
//l - текущая позиция в строке Ln для записи
//SL - массив строк для накопления информации
//EndLn = true, если завершать строку и включать в массив строк
//SF - строка - накопитель для формулы
 FormToStr(F,SF);
 if(NameVary){
	Vary *V;
  wchar_t *Buf=(wchar_t *)calloc(wcslen(NameVary)+1,SzC);
  wcscpy(Buf,NameVary);
  Buf=LiterGrekToHtm(Buf);
  if(ISRAZM&&FindVary(NameVary, &V)&&(V->Razm.C!=Pust)) {
   String SR;
   FormToStr(V->Razm,&SR);
   l+=swprintf(l,L"<b>%s</b>=%s, %s;",Buf,SF->w_str(),SR.w_str());
  }
  else
   l+=swprintf(l,L"<b>%s</b>=%s;",Buf,SF->w_str());
  free(Buf);
 }
 else
  l+=swprintf(l,L"%s",SF->w_str());
 if(EndLn){
  l+=swprintf(l,L"<br>"); SL->Add(Ln); *Ln='\0'; l=Ln;
 }
 return l;
}
//---------------------------------------------------------------------------

