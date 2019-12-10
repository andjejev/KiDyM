#include "CA.h"
#include "ConvDW.h"
#include "TablW.h"
#include "string.h"
#include <stdio.h>
#include <io.h>
#include <dir.h>
#include <math.h>
#include <locale>
#include "ControlCOFs.h"

extern List *LBeg; extern FILE *DGSFILE;//,*HTMFILE,*OPRFILE;
extern int N,NzK,NPV,lmax,StrLong,Kpodv,KColumn;
extern Cnst *Pust,*Nul;
extern wchar_t Inf[],InpFile[],DgsFile[],HtmFile[],OprFile[],HeadCalc[],HeadWork[],
 HeadVersion[],Autor[],WORKDIR[],CurrDir[],AtrForse,COD[],NameCalc[],
 Sdgs[],Shtm[],Sopr[],*sdgs,*shtm,*sopr;
extern TStringList *SLD,*SLH,*SLO,*SLP,*SLK; extern String SF;
extern Root *Opers; extern int NYear,PUNKT; extern List *L;
extern bool SMALLFONT,IsSyntError,IsError,INVERSE;

bool NEHOL,CHAP=true,ArrayDiagnostik=false,SplineDiagnostik=false;
void (*PoFormsToReport)(wchar_t *,wchar_t *);

void BuildCOF(Root *I,TStatusBar *StatusBar,TTabControl *TC);
//---------------------------------------------------------------------------
void TablToHTM(wchar_t *File,int CountTabl,double Tn,double Tk,
  bool Godograf,wchar_t *Format1,wchar_t *Format2,wchar_t *Format3,
  double *X,double *Y){
 FILE *F=_wfopen(File,L"r");
 wchar_t S[256],S1[80],S2[80],S3[80],*s,*Head[3],HeadTabl[80];
 double Step; int i,j,k,Row=CountTabl/(3-Godograf);
 if(CountTabl%(3-Godograf)) Row++; Step=(Tk-Tn)/(CountTabl-1);
 if(F){
//  if(HTMFILE=_wfopen(HtmFile,L"r+")){
  if(SLH){
//   if(!fseek(HTMFILE,-Lpodv,2)){
   for(int i=0;i<Kpodv;i++) SLH->Delete(SLH->Count-1);
   Fgets(S,255,F); if(s=wcsrchr(S,'\n')) *s='\0';
   for(s=S;*s&&(*s==' ');s++); wcscpy(HeadTabl,s);
   if(Fgets(S,255,F)&&Fgets(S,255,F)){
	if(s=wcsrchr(S,'|')) *s='\0';
	for(s=S;*s&&(*s!='|');s++); Head[0]=++s;
	for(;*s&&(*s!='|');s++); *s++='\0'; Head[1]=++s;
	if(Godograf){
	 for(;*s&&(*s!='|');s++); *s++='\0'; Head[2]=++s;
	 for(s=Head[2]+wcslen(Head[2])-1;(s>Head[2])&&(*s==' ');) s--;
	 s++; *s='\0';
	}
	for(s=Head[1]+wcslen(Head[1])-1;(s>Head[1])&&(*s==' ');) s--;
	s++; *s='\0';
	for(s=Head[0]+wcslen(Head[0])-1;(s>Head[0])&&(*s==' ');) s--;
	s++; *s='\0';
   }
//   fwprintf(HTMFILE,
//	L"<h5 align=\"left\"><font color=\"#000099\">"
//	L"Таблица значений \"%s\" (см. файл \"%s\")</font></h5>\n",
//	HeadTabl,File);
   swprintf(Shtm,
	L"<h5 align=\"left\"><font color=\"#000099\">"
	L"Таблица значений \"%s\" (см. файл \"%s\")</font></h5>",
	HeadTabl,File);
   SLH->Add(Shtm);
//   fputws(L"<p align=\"left\"><table width=\"100%\" border=\"1\""
//	L" height=\"0\" cellspacing=\"2\" cellpadding=\"4\">\n",HTMFILE);
//   fputws(L"<tr>\n",HTMFILE);
   SLH->Add(L"<p align=\"left\"><table width=\"100%"
	L"\" border=\"1\" height=\"0\" cellspacing=\"2\""
	L" cellpadding=\"4\">");
//   fputws(L"<tr>\n",HTMFILE);
   SLH->Add(L"<tr>");
   for(i=0;i<3-Godograf;i++){
	for(j=0;j<2+Godograf;j++){
//      fwprintf(HTMFILE,L"<td>\n<div align=\"center\">%s</div>\n</td>",
//       Head[j]);
//	 fwprintf(HTMFILE,L"<th>%s</th>",Head[j]);
	 swprintf(Shtm,L"<th>%s</th>",Head[j]); SLH->Add(Shtm);
   }}
//   fputws(L"\n</tr>\n",HTMFILE);
   SLH->Add(L"</tr>");
   swprintf(S1,L"<td>\n<div align=\"right\">%s</div>",Format1);
   if(Godograf)
	swprintf(S2,L"<td>\n<div align=\"right\">%s</div>\n</td>",Format2);
   swprintf(S3,L"<td>\n<div align=\"right\">%s</div>\n</td>",Format3);
   for(i=0;i<Row-1;i++){
//	fputws(L"<tr>\n",HTMFILE);
	SLH->Add(L"<tr>");
	for(j=0;j<3-Godograf;j++){
//	 fwprintf(HTMFILE,S1,Tn+(j*Row+i)*Step);
	 SLH->Add(L"<td>"); SLH->Add(L"<div align=\"right\">");
	 swprintf(Shtm,Format1,Tn+(j*Row+i)*Step); SLH->Add(Shtm);
	 SLH->Add(L"</div>"); SLH->Add(L"</td>");
	 if(Godograf){
//	  fwprintf(HTMFILE,S2,X[j*Row+i]);
	  SLH->Add(L"<td>"); SLH->Add(L"<div align=\"right\">");
	  swprintf(Shtm,Format2,X[j*Row+i]); SLH->Add(Shtm);
	  SLH->Add(L"</div>"); SLH->Add("</td>");
	 }
//	 fwprintf(HTMFILE,S3,Y[j*Row+i]);
	 SLH->Add(L"<td>"); SLH->Add(L"<div align=\"right\">");
	 swprintf(Shtm,Format3,Y[j*Row+i]); SLH->Add(Shtm);
	 SLH->Add(L"</div>"); SLH->Add(L"</td>");
	}
//	fputws(L"</tr>\n",HTMFILE);
	SLH->Add(L"</tr>");
   }
//   fputws(L"<tr>\n",HTMFILE);
	SLH->Add(L"<tr>");
   if(CountTabl%(3-Godograf)){
	for(j=0;j<CountTabl%(3-Godograf);j++){
//	 fwprintf(HTMFILE,S1,Tn+(j*Row+i)*Step);
	 SLH->Add(L"<td>"); SLH->Add(L"<div align=\"right\">");
	 swprintf(Shtm,Format1,Tn+(j*Row+i)*Step); SLH->Add(Shtm);
	 SLH->Add(L"</div>"); SLH->Add(L"</td>");
	 if(Godograf){
//	  fwprintf(HTMFILE,S2,X[j*Row+i]);
	  SLH->Add(L"<td>"); SLH->Add(L"<div align=\"right\">");
	  swprintf(Shtm,Format2,X[j*Row+i]); SLH->Add(Shtm);
	  SLH->Add(L"</div>"); SLH->Add("</td>");
	 }
//	 fwprintf(HTMFILE,S3,Y[j*Row+i]);
	 SLH->Add(L"<td>"); SLH->Add(L"<div align=\"right\">");
	 swprintf(Shtm,Format3,Y[j*Row+i]); SLH->Add(Shtm);
	 SLH->Add(L"</div>"); SLH->Add(L"</td>");
	}
	for(;j<3-Godograf;j++){
//	 fwprintf(HTMFILE,L"<td>\n<div align=\"right\">&nbsp;</div>\n</td>");
	 SLH->Add(L"<td>");
	 SLH->Add(L"<div align=\"right\">&nbsp;</div>");
	 SLH->Add(L"</td>");
	 if(Godograf){
//	  fwprintf(HTMFILE,L"<td>\n<div align=\"right\">&nbsp;</div>\n</td>");
	  SLH->Add(L"<td>");
	  SLH->Add(L"<div align=\"right\">&nbsp;</div>");
	  SLH->Add(L"</td>");
	 }
//	 fwprintf(HTMFILE,L"<td>\n<div align=\"right\">&nbsp;</div>\n</td>");
	 SLH->Add(L"<td>");
	 SLH->Add(L"<div align=\"right\">&nbsp;</div>");
	 SLH->Add(L"</td>");
   }}
   else{
	for(j=0;j<3-Godograf;j++){
//	 fwprintf(HTMFILE,S1,Tn+(j*Row+i)*Step);
	 SLH->Add(L"<td>"); SLH->Add(L"<div align=\"right\">");
	 swprintf(Shtm,Format1,Tn+(j*Row+i)*Step); SLH->Add(Shtm);
	 SLH->Add(L"</div>"); SLH->Add(L"</td>");
	 if(Godograf){
//	  fwprintf(HTMFILE,S2,X[j*Row+i]);
	  SLH->Add(L"<td>"); SLH->Add(L"<div align=\"right\">");
	  swprintf(Shtm,Format2,X[j*Row+i]); SLH->Add(Shtm);
	  SLH->Add(L"</div>"); SLH->Add("</td>");
	 }
//	 fwprintf(HTMFILE,S3,Y[j*Row+i]);
	 SLH->Add(L"<td>"); SLH->Add(L"<div align=\"right\">");
	 swprintf(Shtm,Format3,Y[j*Row+i]); SLH->Add(Shtm);
	 SLH->Add(L"</div>"); SLH->Add(L"</td>");
   }}
//   fputws(L"</tr>",HTMFILE);
//   fputws(L"</table>\n</p>",HTMFILE);
   SLH->Add(L"</tr>"); SLH->Add(L"</table>"); SLH->Add(L"</p>");
//	fwprintf(HTMFILE,
//	 L"<p align=\"left\"><hr><font face=\"Georgia, Times New Roman, Times, serif\""
//	 L" color=\"#326464\">&copy; %s, %d<br></font></p>\n",Autor,NYear+1900);
//	fwprintf(HTMFILE,L"</body>\n</html>\n");
//   fseek(HTMFILE,0,2);
//   fclose(HTMFILE);
   for(int i=0;i<Kpodv;i++) SLH->Add(SLP->Strings[i]);
   SLH->SaveToFile(HtmFile);
  }
  fclose(F);
}}
//---------------------------------------------------------------------------
wchar_t *ReadAllRight(Parm *P,wchar_t *Ln,wchar_t *l,
  TStringList *SL,String *SF,bool ToGrek,bool Eqv){
 wchar_t *SName; int PLn=P->Line;
 for(Parm *p=P;p;p=p->Sled){
//  if(p->Line>Ln){ fputws(L"<br>\n&nbsp;",F); Ln=p->Line; }
  if(p->Line>PLn){
   l+=swprintf(l,L"<br>");
   SL->Add(Ln); *Ln='\0'; l=Ln;
   l+=swprintf(l,L"&nbsp;");
   PLn=p->Line;
  }
  if(p->Nam){
   SName=(wchar_t *)calloc(wcslen(p->Nam)+1,SzC);
   wcscpy(SName,p->Nam);
   if(ToGrek) SName=LiterGrekToHtm(SName);//Grek(SName);
//   fwprintf(F,L"%s",SName);
   l+=swprintf(l,L"%s",SName);
   free(SName);
   if(p->R){
//	fputwc('(',F);
	*l++='('; *l='\0';
	for(Parm *pp=p->R;pp;pp=pp->Sled){
//	 if(pp->Line>Ln){ fputws(L"<br>\n&nbsp;",F); Ln=pp->Line; }
	 if(pp->Line>PLn){
	  l+=swprintf(l,L"<br>");
	  SL->Add(Ln); *Ln='\0'; l=Ln;
	  l+=swprintf(l,L"&nbsp;");
	  PLn=pp->Line;
	 }
	 if(pp->Nam){
	  SName=(wchar_t *)calloc(wcslen(pp->Nam)+1,SzC);
	  wcscpy(SName,pp->Nam);
	  if(ToGrek) SName=LiterGrekToHtm(SName);//Grek(SName);
//    fwprintf(F,L"%s",SName);
	  l+=swprintf(l,L"%s",SName);
	  if(pp->R){
//	   fputwc('(',F);
	   *l++='('; *l='\0';
	   for(Parm *ppp=pp->R;ppp;ppp=ppp->Sled){
		if(ppp->Line>PLn){
//		 fputws(L"<br>\n&nbsp;",F); Ln=ppp->Line;
		 l+=swprintf(l,L"<br>");
		 SL->Add(Ln); *Ln='\0'; l=Ln;
		 l+=swprintf(l,L"&nbsp;");
		 PLn=ppp->Line;
		}
		if(ppp->Nam){
		 SName=(wchar_t *)calloc(wcslen(ppp->Nam)+1,SzC);
		 wcscpy(SName,ppp->Nam);
		 if(ToGrek) SName=LiterGrekToHtm(SName);//Grek(SName);
//		 fwprintf(F,L"%s",SName);
		 l+=swprintf(l,L"%s",SName);
		 free(SName);
		}
		else if(ppp->F.C!=Pust)
//		 PrintForm(false,F,NULL,ppp->F);
		 l=FormToStringList(ppp->F,NULL,Ln,l,SL,false,SF);
//		else fwprintf(F,L"%g",ppp->Val);
		else l+=swprintf(l,L"%g",ppp->Val);
//		if(ppp->Sled) fputws(L", ",F);
		if(ppp->Sled) l+=swprintf(l,L", ");
//		else
//		 fputwc(')',F);
		else{
		 *l++=')'; *l='\0';
	 }}}}
	 else if(pp->F.C&&(pp->F.C!=Pust))
//	  PrintForm(false,F,NULL,pp->F);
	  l=FormToStringList(pp->F,NULL,Ln,l,SL,false,SF);
	 else
//	  fwprintf(F,L"%g",pp->Val);
	  l+=swprintf(l,L"%g",pp->Val);
	 if(pp->Sled)
//	  fputws(L", ",F);
	  l+=swprintf(l,L", ");
	 else{
//	  fputwc(')',F);
	  *l++=')'; *l='\0';
  }}}}
  else if(p->F.C!=Pust)
//   PrintForm(false,F,NULL,p->F);
   l=FormToStringList(p->F,NULL,Ln,l,SL,false,SF);
  else
//   fwprintf(F,L"%g",p->Val);
	 l+=swprintf(l,L"%g",p->Val);
  if(p->Sled)
//   fputws(L", ",F);
   l+=swprintf(l,L", ");
 }
 return l;
}
//---------------------------------------------------------------------------
void StrToHTM(wchar_t *S,TStringList *SL){
 bool It=false; int Ks=0,Ind=0,i; String St;
 for(St=L"\0";*S&&(*S!='\n');S++){
  switch(*S){
   case '<': /*fputws(L"&#60;",HTMFILE);*/ St=St+L"&#60;"; break;
   case '>': /*fputws(L"&#62;",HTMFILE);*/ St=St+L"&#62;"; break;
   case ' ': /*fputws(L"&nbsp;",HTMFILE);*/ St=St+L"&nbsp;"; break;
   case ',':case '|':case '\\':case '(':case ')':
	if(It){
	 /*fputws(L"</i>",HTMFILE);*/ St=St+L"</i>"; It=false;
	}
	if(Ind>0){
	 for(i=Ind;i--;)
	  /*fwprintf(HTMFILE,L"</font></sub>");*/
	  St=St+L"</font></sub>";
	 Ind=0;
	}
	/*fputwc(*S,HTMFILE);*/ St=St+S[0];
	break;
   case '$':
	S++;
	if(*S=='('){ Ks++;
	 Ind++;
//	 fwprintf(HTMFILE,L"<sub><font size=\"-%d\">%c",Ind,*S);
	 swprintf(Inf,L"<sub><font size=\"-%d\">%c",Ind,*S);
	 St=St+Inf;
	 for(S++;*S;S++){
	  if(*S=='(') Ks++;
	  else if(*S==')') Ks--;
	  if(It){
	   if((isdigit(*S)||isciril(*S))){
		/*fwprintf(HTMFILE,L"</i>%c",*S);*/
		St=St+L"</i>%c"; St=St+S[0];
		It=false;
	   }
	   else /*fputwc(*S,HTMFILE);*/ St=St+S[0];
	  }
	  else{
	   if(isalpha(*S)){
		/*fputws(L"<i>",HTMFILE);*/ St=St+L"<i>";
		It=true;
		/*fputwc(*S,HTMFILE);*/ St=St+S[0];
	   }
	   else /*fputwc(*S,HTMFILE);*/ St=St+S[0];
	  }
	  if(!Ks){
	   /*fwprintf(HTMFILE,L"</font></sub>");*/
	   St=St+L"</font></sub>";
	   Ind--; break;
	  }
	}}
	else{
	 Ind++;
	 /*fwprintf(HTMFILE,L"<sub><font size=\"-%d\">%c</font>"
	  L"</sub>",Ind,*S);*/
	 swprintf(Inf,L"<sub><font size=\"-%d\">%c</font>"
	  L"</sub>",Ind,*S);
	 St=St+Inf;
	}
	break;
   default:
	if(It){
	 if(isdigit(*S)){
//	  fwprintf(HTMFILE,L"</i><sub><font size=\"-1\">%c",*S);
	  swprintf(Inf,L"</i><sub><font size=\"-1\">%c",*S);
	  St=St+Inf;
	  It=false; Ind++;
	 }
	 else if(isciril(*S)){
//	  if(Ind) fwprintf(HTMFILE,L"</font></sub>");
	  if(Ind) St=St+L"</font></sub>";
	  Ind++;
//	  fwprintf(HTMFILE,L"</i>%c",*S);
	  swprintf(Inf,L"</i>%c",*S); St=St+Inf;
	  It=false;
	 }
	 else /*fputwc(*S,HTMFILE);*/ St=St+S[0];
	}
	else{
	 if(isalpha(*S)){
	  /*fputws(L"<i>",HTMFILE);*/ St=St+L"<i>";
	  It=true;
	  /*fputwc(*S,HTMFILE);*/ St=St+S[0];
	 }
	 else /*fputwc(*S,HTMFILE);*/ St=St+S[0];
	}
 }}
 if(It) /*fputws(L"</i>",HTMFILE);*/ St=St+L"</i>";
 /*fputws(L"<br>\n",HTMFILE);*/ St=St+L"<br>"; SL->Add(St);
}
//---------------------------------------------------------------------------
void StrToHTM(wchar_t *S){ wchar_t Rez[256],*r=Rez;
 for(;*S&&(*S!='\n');S++){
  switch(*S){
   case '<': r+=swprintf(r,L"&#60;"); break;
   case '>': r+=swprintf(r,L"&#62;"); break;
   case ' ': r+=swprintf(r,L"&nbsp;"); break;
   case '$': S++; r+=swprintf(r,L"<sub><font size=\"-1\">%c</font></sub>",*S); break;
   default:  *r++=*S;
 }}
 swprintf(r,L"<br>\n"); wcscpy(S,Rez);
}
//---------------------------------------------------------------------------
bool Number(wchar_t *S){
 if(!S) return false;
 for(wchar_t *s=S;*s;s++) if(isciril(*s)||isalpha(*s)) return false;
 return true;
}
//---------------------------------------------------------------------------
void GrekCnst(wchar_t *Name){
      if(!wcscmp(Name,L"PI"))        wcscpy(Name,L"&#960;");
 else if(!wcscmp(Name,L"2PI"))       wcscpy(Name,L"2&#960;");
 else if(!wcscmp(Name,L"3PI"))       wcscpy(Name,L"3&#960;");
 else if(!wcscmp(Name,L"4PI"))       wcscpy(Name,L"4&#960;");
 else if(!wcscmp(Name,L"5PI"))       wcscpy(Name,L"5&#960;");
 else if(!wcscmp(Name,L"6PI"))       wcscpy(Name,L"6&#960;");
 else if(!wcscmp(Name,L"7PI"))       wcscpy(Name,L"7&#960;");
 else if(!wcscmp(Name,L"PIna2"))     wcscpy(Name,L"&#960;/2");
 else if(!wcscmp(Name,L"PIna3"))     wcscpy(Name,L"&#960;/3");
 else if(!wcscmp(Name,L"PIna4"))     wcscpy(Name,L"&#960;/4");
 else if(!wcscmp(Name,L"PIna6"))     wcscpy(Name,L"&#960;/6");
 else if(wcsstr(Name,L"sqrt")==Name) swprintf(Name,L"&#8730;%s",Name+4);
}
//---------------------------------------------------------------------------
bool FirstLetterCorrect(wchar_t B){
 for(int i=0;i<6;i++) if(B==COD[i]) return true;
 if(B=='@') return true;
 return false;
}
//---------------------------------------------------------------------------
void PrintVars(){
 FILE *Fo=_wfopen(L"OtladkaV.txt",L"w");
 for(Vary *V=L->V;V;V=V->Sled){
  PrintForm(true,Fo,V->Name,V->Znach);
  fputwc('\n',Fo);
 }
 fclose(Fo);
}
//---------------------------------------------------------------------------
void CalcVWBodys(TCGauge *CGauge,TStatusBar *StatusBar){//Сформируем скорости и ускорения тел
 wchar_t SBold[256];
// OPRFILE=_wfopen(OprFile,L"a");
// fwprintf(OPRFILE,L"<h3 align=\"center\"><font color=\"#800000\">"
//  L"Дополнительно сгенерированы дифференциальные структуры для следующих тел.</font></h3>\n");
 if(!SLO) return;
 swprintf(Sopr,L"<h4 align=\"center\"><font color=\"#000080\">"
  L"Дополнительно сгенерированы дифференциальные структуры "
  L"для следующих тел.</font></h4>");
 SLO->Add(Sopr);
 if(StatusBar) wcscpy(SBold,StatusBar->SimpleText.c_str());
 if(CGauge){ CGauge->MaxValue=KolElem(L->B); CGauge->Progress=0; }
 for(Body *B=L->B;B;B=B->Sled){ Vary *V; wchar_t Name[256];
//  B->vOk[3].C=B->vCk[3].C=B->aOk[3].C=B->aCk[3].C=B->wkk[3].C=B->ekk[3].C=Nul;
  swprintf(Inf,L"Формируем скорости и ускорения тела: %s",B->Name);
  if(StatusBar) StatusBar->SimpleText=Inf;
  if(CGauge) CGauge->Progress++;
//  fwprintf(OPRFILE,L"<h4 align=\"center\"><font color=\"#800000\">"
//   L"\"%s\" строки %d:</font></h4>\n",B->Name,B->Line);
  swprintf(Sopr,L"<h4 align=\"center\"><font color=\"#800000\">"
   L"\"%s\" строки %d:</font></h4>",B->Name,B->Line);
  SLO->Add(Sopr);
  for(int i=0;i<3;i++){ Parm *p; Vary *W,*A,*E;
   swprintf(Name,L"%cO@%s",i==2?'Z':i==1?'Y':'X',B->Name);
   if(FindVary(Name,&V)){
	swprintf(Name,L"VO%c@%s",i==2?'Z':i==1?'Y':'X',B->Name); W=TakeVary(Name);
	W->Znach=DifByT(V); B->vOk[i].V=W;
	if(W->Znach.C->Atr==VARY){
//	 fwprintf(OPRFILE,L"<b>%s=%s=</b>",Name,W->Znach.V->Name);
//	 PrintForm(false,OPRFILE,NULL,W->Znach.V->Znach);
//	 fputws(L"<br>\n",OPRFILE);
	 sopr+=swprintf(sopr,L"<b>%s=%s=</b>",Name,W->Znach.V->Name);
	 sopr=FormToStringList(W->Znach.V->Znach,NULL,Sopr,sopr,
	  SLO,false,&SF);
	}
	else{
//	 PrintForm(false,OPRFILE,Name,W->Znach);
//	 fputws(L"<br>\n",OPRFILE);
	 sopr=FormToStringList(W->Znach,Name,Sopr,sopr,SLO,false,&SF);
	}
	wcscpy(sopr,L"<br>");
	SLO->Add(Sopr); *Sopr='\0'; sopr=Sopr;
	swprintf(Name,L"AO%c@%s",i==2?'Z':i==1?'Y':'X',B->Name); A=TakeVary(Name);
	A->Znach=DifByT(W); B->aOk[i].V=A;
	if(A->Znach.C->Atr==VARY){
	 if(A->Znach.V->Znach.C->Atr==VARY){
//	  fwprintf(OPRFILE,L"<b>%s=%s=%s=</b>",
//	   Name,A->Znach.V->Name,A->Znach.V->Znach.V->Name);
//	  PrintForm(false,OPRFILE,NULL,A->Znach.V->Znach.V->Znach);
//	  fputws(L"<br>\n",OPRFILE);
	  sopr+=swprintf(sopr,L"<b>%s=%s=%s=</b>",
	   Name,A->Znach.V->Name,A->Znach.V->Znach.V->Name);
	  sopr=FormToStringList(A->Znach.V->Znach,NULL,Sopr,sopr,
	   SLO,false,&SF);
	  sopr+=swprintf(sopr,L"<br>");
	  SLO->Add(Sopr); *Sopr='\0'; sopr=Sopr;
	 }
	 else{
//	  fwprintf(OPRFILE,L"<b>%s=%s=</b>",Name,A->Znach.V->Name);
//	  PrintForm(false,OPRFILE,NULL,A->Znach.V->Znach);
//	  fputws(L"<br>\n",OPRFILE);
	  sopr+=swprintf(sopr,L"<b>%s=%s=</b>",Name,A->Znach.V->Name);
	  sopr=FormToStringList(A->Znach.V->Znach,NULL,Sopr,sopr,
	   SLO,false,&SF);
	  sopr+=swprintf(sopr,L"<br>");
	  SLO->Add(Sopr); *Sopr='\0'; sopr=Sopr;
	  swprintf(Name,L"%cO@%s'%s'%s",
	   i==2?'Z':i==1?'Y':'X',B->Name,L->Time->Name,L->Time->Name);
	  if(FindVary(Name,&V)){
//	   PrintForm(false,OPRFILE,Name,V->Znach);
//	   fputws(L"<br>\n",OPRFILE);
	   sopr=FormToStringList(V->Znach,Name,Sopr,sopr,SLO,
		false,&SF);
	   sopr+=swprintf(sopr,L"<br>");
	   SLO->Add(Sopr); *Sopr='\0'; sopr=Sopr;
	}}}
	else{
//	 PrintForm(false,OPRFILE,Name,A->Znach);
//	 fputws(L"<br>\n",OPRFILE);
	 sopr=FormToStringList(A->Znach,Name,Sopr,sopr,SLO,
	  false,&SF);
	 sopr+=swprintf(sopr,L"<br>");
	 SLO->Add(Sopr); *Sopr='\0'; sopr=Sopr;
   }}
   swprintf(Name,L"%cC@%s",i==2?'Z':i==1?'Y':'X',B->Name);
   if(FindVary(Name,&V)){
	swprintf(Name,L"VC%c@%s",i==2?'Z':i==1?'Y':'X',B->Name); W=TakeVary(Name);
	W->Znach=DifByT(V); B->vCk[i].V=W;
	if(W->Znach.C->Atr==VARY){
//	 fwprintf(OPRFILE,L"<b>%s=%s=</b>",Name,W->Znach.V->Name);
//	 PrintForm(false,OPRFILE,NULL,W->Znach.V->Znach);
//	 fputws(L"<br>\n",OPRFILE);
	 sopr+=swprintf(sopr,L"<b>%s=%s=</b>",Name,W->Znach.V->Name);
	 sopr=FormToStringList(W->Znach.V->Znach,NULL,Sopr,sopr,
	  SLO,false,&SF);
	}
	else{
//	 PrintForm(false,OPRFILE,Name,W->Znach);
//	 fputws(L"<br>\n",OPRFILE);
	 sopr=FormToStringList(W->Znach,Name,Sopr,sopr,SLO,false,&SF);
	}
	sopr+=swprintf(sopr,L"<br>");
	SLO->Add(Sopr); *Sopr='\0'; sopr=Sopr;
	swprintf(Name,L"AC%c@%s",i==2?'Z':i==1?'Y':'X',B->Name); A=TakeVary(Name);
	A->Znach=DifByT(W); B->aCk[i].V=A;
	if(A->Znach.C->Atr==VARY){
	 if(A->Znach.V->Znach.C->Atr==VARY){
//	  swprintf(OPRFILE,L"<b>%s=%s=%s=</b>",
//	   Name,A->Znach.V->Name,A->Znach.V->Znach.V->Name);
//	  PrintForm(false,OPRFILE,NULL,A->Znach.V->Znach.V->Znach);
//	  fputws(L"<br>\n",OPRFILE);
	  sopr+=swprintf(sopr,L"<b>%s=%s=%s=</b>",
	   Name,A->Znach.V->Name,A->Znach.V->Znach.V->Name);
	  sopr=FormToStringList(A->Znach.V->Znach.V->Znach,NULL,
	   Sopr,sopr,SLO,false,&SF);
	  sopr+=swprintf(sopr,L"<br>");
	  SLO->Add(Sopr); *Sopr='\0'; sopr=Sopr;
	 }
	 else{
//	  fwprintf(OPRFILE,L"<b>%s=%s=</b>",Name,A->Znach.V->Name);
//	  PrintForm(false,OPRFILE,NULL,A->Znach.V->Znach);
//	  fputws(L"<br>\n",OPRFILE);
	  sopr+=swprintf(sopr,L"<b>%s=%s=</b>",
	   Name,A->Znach.V->Name);
	  sopr=FormToStringList(A->Znach.V->Znach,NULL,Sopr,sopr,
	   SLO,false,&SF);
	  sopr+=swprintf(sopr,L"<br>");
	  SLO->Add(Sopr); *Sopr='\0'; sopr=Sopr;
	  swprintf(Name,L"%cC@%s'%s'%s",
	   i==2?'Z':i==1?'Y':'X',B->Name,L->Time->Name,L->Time->Name);
	  if(FindVary(Name,&V)){
//	   PrintForm(false,OPRFILE,Name,V->Znach);
//	   fputws(L"<br>\n",OPRFILE);
	   sopr=FormToStringList(V->Znach,Name,Sopr,sopr,
		SLO,false,&SF);
	   sopr+=swprintf(sopr,L"<br>");
	   SLO->Add(Sopr); *Sopr='\0'; sopr=Sopr;
	}}}
	else{
//	 PrintForm(false,OPRFILE,Name,A->Znach);
//	 fputws(L"<br>\n",OPRFILE);
	 sopr=FormToStringList(A->Znach,Name,Sopr,sopr,
	  SLO,false,&SF);
	 sopr+=swprintf(sopr,L"<br>");
	 SLO->Add(Sopr); *Sopr='\0'; sopr=Sopr;
   }}
   swprintf(Name,L"epsilon%c@%s",i==2?'z':i==1?'y':'x',B->Name); E=TakeVary(Name);
   E->Znach=DifByT(B->wkk[i].V->Znach); B->ekk[i].V=E;
//   PrintForm(false,OPRFILE,Name,E->Znach);
//   fputws(L"<br>\n",OPRFILE);
   sopr=FormToStringList(E->Znach,Name,Sopr,sopr,
	SLO,false,&SF);
   sopr+=swprintf(sopr,L"<br>");
   SLO->Add(Sopr); *Sopr='\0'; sopr=Sopr;
 }}
 if(StatusBar) StatusBar->SimpleText=SBold;
 if(CGauge) CGauge->Progress=0;
// fwprintf(OPRFILE,L"</body>\n</html>\n");
// fclose(OPRFILE);
 SLO->Add(L"</body>"); SLO->Add(L"</html>");
 SLO->SaveToFile(OprFile);
}
//---------------------------------------------------------------------------
bool Diagnos(TCGauge *CGauge,TStatusBar *StatusBar,TTabControl *TC){
 int Ki=0,Kr=0,Ks=0,l,Np,i,g; Cord *k;//,*KO=(int*)calloc(6,SzI);
 bool Rez=false; Vary *V; Cnst *C; Root *I;
 wchar_t S[50],SBold[256],*Buf;
// DGSFILE=_wfopen(DgsFile,_waccess(DgsFile,0)?L"w":L"a");
 if(!SLD) SLD=new TStringList;
// if(!DGSFILE) return Rez; Rez=true; lmax=StrLong;
 if(!SLD) return Rez; Rez=true; lmax=StrLong;
// fwprintf(DGSFILE,L"<html>\n
//  L"<head>\n<title>Д И А Г Н О С Т И К А</title>\n"
//  L"<meta http-equiv=\"Content-Type\" content=\"text/html;"
//  L"wchar_tset=windows-1251\">\n</head>\n"
//  L"<body bgcolor=\"#FFFFFF\" text=\"#800000\""
//  L" background=\"%s/fon2.jpg\""
//  L" font face=\"Times New Roman, Times, serif\">\n",WORKDIR);
 SLD->Add(L"<html>");
 SLD->Add(L"<head>\n<title>Д И А Г Н О С Т И К А</title>\n");
 SLD->Add(L"<meta http-equiv=\"Content-Type\" "
  L"content=\"text/html; charset=windows-1251\">");
 SLD->Add(L"</head>");
 sdgs+=swprintf(sdgs=Sdgs,
  L"<body bgcolor=\"#FFFFFF\" text=\"#800000\""
  L" background=\"%s/fon2.jpg\""
  L" font face=\"Times New Roman, Times, serif\">",WORKDIR);
 SLD->Add(Sdgs); *Sdgs='\0'; sdgs=Sdgs;
// if(HeadVersion[0])
//  fwprintf(DGSFILE,L"<h3 align=\"center\"><font color=\"#990000\">"
//	 L"Д И А Г Н О С Т И К А<br>\nПрограмма: \"%s\"<br>\n"
//	 L"Работа: %s<br>\nВариант: %s<br>\nАвторское право: %s</font></h3>\n",
//	HeadCalc,HeadWork,HeadVersion,Autor);
// else
//  fwprintf(DGSFILE,L"<h3 align=\"center\"><font color=\"#990000\">"
//	 L"Д И А Г Н О С Т И К А<br>\nПрограмма: \"%s\"<br>\n"
//	 L"Работа: %s<br>\nАвторское право: %s</font></h3>\n",
//	 HeadCalc,HeadWork,Autor);
 SLD->Add(L"<h3 align=\"center\"><font color=\"#990000\">"
  L"Д И А Г Н О С Т И К А<br>");
 swprintf(Sdgs,L"Программа: \"%s\"<br>",HeadCalc);
 SLD->Add(Sdgs);
 swprintf(Sdgs,L"Работа: %s<br>",HeadWork); SLD->Add(Sdgs);
 if(HeadVersion[0]){
  swprintf(Sdgs,L"Вариант: %s<br>",HeadVersion); SLD->Add(Sdgs);
 }
 swprintf(Sdgs,L"Авторское право: %s</font></h3>",Autor);
 SLD->Add(Sdgs);
// fwprintf(DGSFILE,L"<h4 align=\"left\"><font color=\"#000099\">"
//  L"Список констант:</font></h4>\n");
 SLD->Add(L"<h4 align=\"left\"><font color=\"#000099\">"
  L"Список констант:</font></h4>");
//  L"<font face=\"Courier New, Courier, mono\"><b>");
// fputws(L"<table width=\"100%\" border=\"1\">\n",DGSFILE);
// fputws(L"<tr>",DGSFILE);
 SLD->Add(L"<table width=\"100%\" border=\"1\">");
 sdgs+=swprintf(sdgs,L"<tr>");
 for(i=0;i<4;i++){
//  fputws(L"<td><div align=\"center\"><b>Имя</b></div></td>\n"
//   L"<td><div align=\"center\"><b>Значение</b></div></td>\n",DGSFILE);
  sdgs+=swprintf(sdgs,L"<td><div align=\"center\"><b>Имя</b></div></td>");
  SLD->Add(Sdgs); sdgs=Sdgs; *Sdgs='\0';
  SLD->Add(L"<td><div align=\"center\"><b>Значение</b></div></td>");
 }
// fputws(L"</tr>",DGSFILE);
 sdgs+=swprintf(sdgs,L"</tr>");
 if(CGauge){
  CGauge->Progress=0; for(C=Pust->Sled,i=0;C;C=C->Sled,i++);
  CGauge->MaxValue=i;
 }
 for(C=Pust->Sled,i=0;C;C=C->Sled,i++){ wchar_t Name[80];
  if(CGauge) CGauge->Progress++;
//  if(!(i%4)){ if(i) fputws(L"</tr>\n",DGSFILE);
//  fputws(L"<tr>\n <td>",DGSFILE); }
//  else fputws(L"<td>",DGSFILE);
  if(i%4==0){
   if(i) sdgs+=swprintf(sdgs,L"</tr>");
   SLD->Add(Sdgs); sdgs=Sdgs; *Sdgs='\0';
   SLD->Add(L"<tr>");
  }
  sdgs+=swprintf(sdgs,L"<td>");
  if(C->Name&&!Number(C->Name)){
   wcscpy(Name,C->Name); GrekCnst(Name);
//   fwprintf(DGSFILE,L"%7s</td>\n<td>%10g</td>\n",Name,C->Val);
   sdgs+=swprintf(sdgs,L"%7s</td>\n<td>%10g</td>",Name,C->Val);
  }
  else
//   fwprintf(DGSFILE,L"&nbsp;</td>\n<td>%10g</td>\n",C->Val);
   sdgs+=swprintf(sdgs,L"&nbsp;</td>\n<td>%10g</td>\n",C->Val);
  SLD->Add(Sdgs); sdgs=Sdgs; *Sdgs='\0';
 }
 for(;i%4;i++)
//  fputws(L"<td>&nbsp;</td>\n<td>&nbsp;</td>\n",DGSFILE);
  SLD->Add(L"<td>&nbsp;</td>"); SLD->Add(L"<td>&nbsp;</td>");
// fputws(L"</td>\n<tr></table>",DGSFILE);
 SLD->Add(L"</td>"); SLD->Add(L"<tr></table>");
// fputws(L"</b><br>\n</font>",DGSFILE);
 SLD->Add(L"</b><br>"); SLD->Add(L"</font>");
//обнулим значения переменных интегрирования и соберем список интегралов
//обнулим значения переменных функций поиска корней и соберем список корней
 for(Vary *V=L->V;V;V=V->Sled){
  Ki=VaryToIntg(Ki,V->Znach,Nul);
  Ks=VaryToSumm(Ks,V->Znach,Nul);
  Kr=VaryToSol(Kr,V->Znach,Nul);
 }
 TakeDifVary(CGauge,StatusBar);// Сделаем производные не по времени
//PrintVars();
 if((N=Coord(L,CGauge,StatusBar))||NPV){
//PrintVars();
  if(I=FindInstLast(L"ОБОБЩЕННЫЕ КООРДИНАТЫ")){
   Parm *P; Cord *K;
   Magazine *Mnodef=NULL,*Mdef=NULL,*m;//магазины неопределенных переменных,
//но не обобщенных координат и определенных переменных, но перечисленных среди
//обобщенных координат в инструкции ОБОБЩЕННЫЕ КООРДИНАТЫ
   if(N&&!wcscmp(NameCalc,L"КИНЕМАТИКА")){
	wchar_t *Buf;
//	fwprintf(DGSFILE,
//	 L"<h4 align=\"left\"><font color=\"#000099\">"
//	 L"Неопределенные переменные:</font></h4>\n");
	SLD->Add(L"<h4 align=\"left\"><font color=\"#000099\">"
	 L"Неопределенные переменные:</font></h4>");
	l=0;
	for(k=L->K;k->Sled;k=k->Sled){
	 Buf=(wchar_t *)calloc(wcslen(k->V->Name)+1,SzC);
	 wcscpy(Buf,k->V->Name); Buf=LiterGrekToHtm(Buf);//Grek(Inf);
	 g=wcslen(Buf);
	 if((l+g+2)>lmax){
//	  fwprintf(DGSFILE,L"<br>\n "); l=0;
	  SLD->Add(Sdgs); sdgs=Sdgs; *Sdgs='\0';
	  SLD->Add(L"<br>"); l=0;
	 }
//	 l+=g+2; fwprintf(DGSFILE,L"%s, ",Buf);
	 l+=g+2; sdgs+=swprintf(sdgs,L" %s, ",Buf);
	 free(Buf);
	}
	Buf=(wchar_t *)calloc(wcslen(k->V->Name)+1,SzC);
	wcscpy(Buf,k->V->Name);	Buf=LiterGrekToHtm(Buf);//Grek(Inf);
//	if((l+g+1)>lmax) fwprintf(DGSFILE,L"<br>\n ");
	if((l+g+1)>lmax){
	 sdgs+=swprintf(sdgs,L"<br>");
	 SLD->Add(Sdgs); *Sdgs='\0'; sdgs=Sdgs;
	}
	g=wcslen(Buf);
//	fwprintf(DGSFILE,L"%s",Buf);
	sdgs+=swprintf(sdgs,Buf);
	free(Buf);
   }
   for(P=I->First;P;P=P->Sled)
	if(!FindCord(P->Nam,&K)) TakeMagazine(&Mdef,P->Nam);
   if(Mdef){
	swprintf(Inf,L"Среди переменных, перечисленных в инструкции\n"
	 L"\"ОБОБЩЕННЫЕ КООРДИНАТЫ\" есть определенные.\n"
	 L"См. список в закладке \"Диагностика\"");
	Application->MessageBox(Inf,L"Проверка списка обобщенных координат",
	 MB_OK||MB_ICONERROR);
//	fwprintf(DGSFILE,L"<h4 align=\"left\"><font color=\"#000099\">"
//	 L"Определенные переменные, перечисленные в инструкции \"Обобщенные координаты:\"</font></h4>");
	SLD->Add(L"<h4 align=\"left\"><font color=\"#000099\">"
	 L"Определенные переменные, перечисленные в инструкции "
	 L"\"Обобщенные координаты:\"</font></h4>");
	l=0; lmax=StrLong;
	for(m=Mdef;m->Sled;m=m->Sled){
	 Buf=(wchar_t *)calloc(wcslen(m->S)+1,SzC);
	 wcscpy(Buf,m->S); Buf=LiterGrekToHtm(Buf);//Grek(Inf);
	 g=wcslen(Buf);
//	 if((l+g+2)>lmax){ fwprintf(DGSFILE,L"<br>\n "); l=0; }
//	 l+=g+2; fwprintf(DGSFILE,L"%s, ",Buf);
	 if((l+g+2)>lmax){
	  sdgs+=swprintf(sdgs,L"<br>"); l=0;
	  SLD->Add(Sdgs); Sdgs[0]=' '; Sdgs[1]='\0'; sdgs=Sdgs+1;
	 }
	 l+=g+2; sdgs+=swprintf(sdgs,L"%s, ",Buf);
	 free(Buf);
	}
	Buf=(wchar_t *)calloc(wcslen(m->S)+1,SzC);
	wcscpy(Buf,m->S); Buf=LiterGrekToHtm(Buf);//Grek(Inf);
	g=wcslen(Buf);
//	if((l+g+1)>lmax) fwprintf(DGSFILE,L"<br>\n ");
//	fwprintf(DGSFILE,L"%s",Buf);
	if((l+g+1)>lmax){
	 sdgs+=swprintf(sdgs,L"<br>"); l=0;
	 SLD->Add(Sdgs); Sdgs[0]=' '; Sdgs[1]='\0'; sdgs=Sdgs+1;
	}
	sdgs+=swprintf(sdgs,L"%s",Buf);
	free(Buf);
   }
   for(K=L->K;K;K=K->Sled)
	if(!FindParm(I->First,K->V->Name,&P)) TakeMagazine(&Mnodef,K->V->Name);
   if(Mnodef){
	swprintf(Inf,L"Кроме переменных, перечисленных в инструкции\n"
	 L"\"ОБОБЩЕННЫЕ КООРДИНАТЫ\" есть еще неопределенные.\n"
	 L"См. список в закладке \"Диагностика\"");
	Application->MessageBox(Inf,L"Проверка списка обобщенных координат",
	 MB_OK||MB_ICONERROR);
//	fwprintf(DGSFILE,L"<h4 align=\"left\"><font color=\"#000099\">"
//	 L"Неопределенные переменные, не указанные в инструкции \"Обобщенные координаты:\"</font></h4>\n");
	SLD->Add(L"<h4 align=\"left\"><font color=\"#000099\">"
	 L"Непределенные переменные, не указанные в инструкции "
	 L"\"Обобщенные координаты:\"</font></h4>");
	l=0; lmax=StrLong;
	for(m=Mnodef;m->Sled;m=m->Sled){
	 Buf=(wchar_t *)calloc(wcslen(m->S)+1,SzC);
	 wcscpy(Buf,m->S); Buf=LiterGrekToHtm(Buf);//Grek(Inf);
//	 if((l+g+2)>lmax){ fwprintf(DGSFILE,L"<br>\n "); l=0; }
//	 l+=g+2; fwprintf(DGSFILE,L"%s, ",Buf);
	 if((l+g+2)>lmax){
	  sdgs+=swprintf(sdgs,L"<br>"); l=0;
	  SLD->Add(Sdgs); Sdgs[0]=' '; Sdgs[1]='\0'; sdgs=Sdgs+1;
	 }
	 l+=g+2; sdgs+=swprintf(sdgs,L"%s, ",Buf);
	 free(Buf);
	}
	Buf=(wchar_t *)calloc(wcslen(m->S)+1,SzC);
	wcscpy(Buf,m->S); Buf=LiterGrekToHtm(Buf);//Grek(Inf);
//	if((l+g+1)>lmax) fwprintf(DGSFILE,L"<br>\n ");
//	fwprintf(DGSFILE,L"%s",Buf);
	if((l+g+1)>lmax){
	 sdgs+=swprintf(sdgs,L"<br>"); l=0;
	 SLD->Add(Sdgs); Sdgs[0]=' '; Sdgs[1]='\0'; sdgs=Sdgs+1;
	}
	sdgs+=swprintf(sdgs,L"%s",Buf);
	free(Buf);
   }
   if(!Mdef&&!Mnodef){ Vary *V;
	DelAllList((void **)&(L->K));
	for(P=I->First;P;P=P->Sled){
	 FindVary(P->Nam,&V);
	 TakeCord(V);
   }}
   else{
	DelAllMagazine(&Mnodef);
	DelAllMagazine(&Mdef);
   }
//   fwprintf(DGSFILE,L"<h4 align=\"left\"><font color=\"#000099\">"
//	L"Обобщенные координаты:</font></h4>\n");
   SLD->Add(L"<h4 align=\"left\"><font color=\"#000099\">"
	L"Обобщенные координаты:</font></h4>");
   l=0; lmax=StrLong;
   for(P=I->First;P->Sled;P=P->Sled){
	Buf=(wchar_t *)calloc(wcslen(P->Nam)+1,SzC);
	wcscpy(Buf,P->Nam); Buf=LiterGrekToHtm(Buf);//Grek(Inf);
	g=wcslen(Buf);
//	 if((l+g+2)>lmax){ fwprintf(DGSFILE,L"<br>\n "); l=0; }
//	 l+=g+2; fwprintf(DGSFILE,L"%s, ",Buf);
	if((l+g+2)>lmax){
	 sdgs+=swprintf(sdgs,L"<br>"); l=0;
	 SLD->Add(Sdgs); Sdgs[0]=' '; Sdgs[1]='\0'; sdgs=Sdgs+1;
	}
	l+=g+2; sdgs+=swprintf(sdgs,L"%s, ",Buf);
	free(Buf);
   }
   Buf=(wchar_t *)calloc(wcslen(P->Nam)+1,SzC);
   wcscpy(Buf,P->Nam); Buf=LiterGrekToHtm(Buf);//Grek(Inf);
   g=wcslen(Buf);
// if((l+g+1)>lmax) fwprintf(DGSFILE,L"<br>\n ");
// fwprintf(DGSFILE,L"%s",Buf);
   if((l+g+1)>lmax){
	sdgs+=swprintf(sdgs,L"<br>"); l=0;
	SLD->Add(Sdgs); Sdgs[0]=' '; Sdgs[1]='\0'; sdgs=Sdgs+1;
   }
   sdgs+=swprintf(sdgs,L"%s",Buf);
   free(Buf);
  }
  else if(N){
//   fwprintf(DGSFILE,L"<h4 align=\"left\"><font color=\"#000099\">"
//	L"Обобщенные координаты:</font></h4>\n");
   SLD->Add(L"<h4 align=\"left\"><font color=\"#000099\">"
	L"Обобщенные координаты:</font></h4>");
   l=0; lmax=StrLong;
   for(k=L->K;k->Sled;k=k->Sled){
	Buf=(wchar_t *)calloc(wcslen(k->V->Name)+1,SzC);
	wcscpy(Buf,k->V->Name); Buf=LiterGrekToHtm(Buf);//Grek(Inf);
	g=wcslen(Buf);
//	 if((l+g+2)>lmax){ fwprintf(DGSFILE,L"<br>\n "); l=0; }
//	 l+=g+2; fwprintf(DGSFILE,L"%s, ",Buf);
	if((l+g+2)>lmax){
	 sdgs+=swprintf(sdgs,L"<br>"); l=0;
	 SLD->Add(Sdgs); Sdgs[0]=' '; Sdgs[1]='\0'; sdgs=Sdgs+1;
	}
	l+=g+2; sdgs+=swprintf(sdgs,L"%s, ",Buf);
	free(Buf);
   }
   Buf=(wchar_t *)calloc(wcslen(k->V->Name)+1,SzC);
   wcscpy(Buf,k->V->Name); Buf=LiterGrekToHtm(Buf);//Grek(Inf);
   g=wcslen(Buf);
// if((l+g+1)>lmax) fwprintf(DGSFILE,L"<br>\n ");
// fwprintf(DGSFILE,L"%s",Buf);
   if((l+g+1)>lmax){
	sdgs+=swprintf(sdgs,L"<br>"); l=0;
	SLD->Add(Sdgs); Sdgs[0]=' '; Sdgs[1]='\0'; sdgs=Sdgs+1;
   }
   sdgs+=swprintf(sdgs,L"%s",Buf);
   free(Buf);
//   fwprintf(DGSFILE,L"<h4 align=\"left\"><font color=\"#000099\">"
//	L"Число обобщенных координат = </font>%d<font color=\"#000099\">.</font></h4>\n",N);
   sdgs+=swprintf(sdgs,
	L"<h4 align=\"left\"><font color=\"#000099\">"
	L"Число обобщенных координат = </font>%d"
	L"<font color=\"#000099\">.</font></h4>",N);
   SLD->Add(Sdgs); Sdgs[0]='\0'; sdgs=Sdgs;
  }
  if(NPV){
//   fwprintf(DGSFILE,L"<h4 align=\"left\"><font color=\"#000099\">"
//	L"Псевдоскорости:</font></h4>\n");
   SLD->Add(L"<h4 align=\"left\"><font color=\"#000099\">"
	L"Псевдоскорости:</font></h4>");
   l=0;
   for(k=L->W;k->Sled;k=k->Sled){
	Buf=(wchar_t *)calloc(wcslen(k->V->Name)+1,SzC);
	wcscpy(Buf,k->V->Name); Buf=LiterGrekToHtm(Buf);//Grek(Inf);
	g=wcslen(Buf);
//	 if((l+g+2)>lmax){ fwprintf(DGSFILE,L"<br>\n "); l=0; }
//	 l+=g+2; fwprintf(DGSFILE,L"%s, ",Buf);
	if((l+g+2)>lmax){
	 sdgs+=swprintf(sdgs,L"<br>"); l=0;
	 SLD->Add(Sdgs); Sdgs[0]=' '; Sdgs[1]='\0'; sdgs=Sdgs+1;
	}
	l+=g+2; sdgs+=swprintf(sdgs,L"%s, ",Buf);
	free(Buf);
   }
   Buf=(wchar_t *)calloc(wcslen(k->V->Name)+1,SzC);
   wcscpy(Buf,k->V->Name); Buf=LiterGrekToHtm(Buf);//Grek(Inf);
   g=wcslen(Buf);
//   if((l+g+1)>lmax) fwprintf(DGSFILE,L"<br>\n ");
//   fwprintf(DGSFILE,L"%s",Buf);
   if((l+g+1)>lmax){
	sdgs+=swprintf(sdgs,L"<br>"); l=0;
	SLD->Add(Sdgs); Sdgs[0]=' '; Sdgs[1]='\0'; sdgs=Sdgs+1;
   }
   sdgs+=swprintf(sdgs,L"%s",Buf);
   free(Buf);
//   fwprintf(DGSFILE,L"<h4 align=\"left\"><font color=\"#000099\">"
//	L"Число псевдоскоростей = </font>%d<font color=\"#000099\">.</font></h4>\n",NPV);
   sdgs+=swprintf(sdgs,
	L"<h4 align=\"left\"><font color=\"#000099\">"
	L"Число псевдоскоростей = </font>%d<font color=\"#000099\">.</font></h4>",NPV);
   SLD->Add(Sdgs); Sdgs[0]='\0'; sdgs=Sdgs;
  }
//  fwprintf(DGSFILE,L"<h4 align=\"left\"><font color=\"#000099\">"
//   L"Число степеней свободы = </font>%d<font color=\"#000099\">.</font></h4>\n",
//   N+NPV);
  sdgs+=swprintf(sdgs,
   L"<h4 align=\"left\"><font color=\"#000099\">"
   L"Число псевдоскоростей = </font>%d<font "
   L"color=\"#000099\">.</font></h4>",N+NPV);
  SLD->Add(Sdgs); Sdgs[0]='\0'; sdgs=Sdgs;
  if(NzK){
//   fwprintf(DGSFILE,L"<h4 align=\"left\"><font color=\"#000099\">"
//	L"Независимые координаты с зависимыми вариациями:"
//	L"</font></h4>\n");
   SLD->Add(L"<h4 align=\"left\"><font color=\"#000099\">"
	L"Независимые координаты с зависимыми вариациями:"
	L"</font></h4>");
   l=0;
   for(k=L->Z;k->Sled;k=k->Sled){
	Buf=(wchar_t *)calloc(wcslen(k->V->Name)+1,SzC);
	wcscpy(Buf,k->V->Name); Buf=LiterGrekToHtm(Buf);//Grek(Inf);
	g=wcslen(Buf);
//	if((l+g+2)>lmax){ fwprintf(DGSFILE,L"<br>\n "); l=0; }
//	l+=g+2; fwprintf(DGSFILE,L"%s, ",Buf);
	if((l+g+2)>lmax){
	 sdgs+=swprintf(sdgs,L"<br>"); l=0;
	 SLD->Add(Sdgs); Sdgs[0]=' '; Sdgs[1]='\0'; sdgs=Sdgs+1;
	}
	l+=g+2; sdgs+=swprintf(sdgs,L"%s, ",Buf);
	free(Buf);
   }
   Buf=(wchar_t *)calloc(wcslen(k->V->Name)+1,SzC);
   wcscpy(Inf,k->V->Name); Buf=LiterGrekToHtm(Buf);//Grek(Inf);
   g=wcslen(Buf);
//   if((l+g+1)>lmax) fwprintf(DGSFILE,L"<br>\n ");
//   fwprintf(DGSFILE,L"%s",Buf);
   if((l+g+1)>lmax){
	sdgs+=swprintf(sdgs,L"<br>"); l=0;
	SLD->Add(Sdgs); Sdgs[0]=' '; Sdgs[1]='\0'; sdgs=Sdgs+1;
   }
   free(Buf);
//   fwprintf(DGSFILE,L"<h4 align=\"left\"><font color=\"#000099\">"
//	L"Число координат с зависимыми вариациями = </font>%d."
//	L"<font color=\"#000099\"></font></h4>\n",NzK);
   sdgs+=swprintf(sdgs,
	L"<h4 align=\"left\"><font color=\"#000099\">"
	L"Число координат с зависимыми вариациями = </font>%d."
	L"<font color=\"#000099\"></font></h4>",NzK);
   SLD->Add(Sdgs); Sdgs[0]='\0'; sdgs=Sdgs;
   for(Cord *z=L->Z;z;z=z->Sled){
	Form F1,F2,F; Vary *V,*V1,*V2; wchar_t S[256],Sv1[256],Sv2[256];
	swprintf(Inf,L"%s'%s",z->V->Name,L->Time->Name);
	if(FindVary(Inf,&V)){
	 for(Cord *K=L->K;K;K=K->Sled){
	  swprintf(Sv1,L"%s'%s",K->V->Name,L->Time->Name);
	  if(FindVary(Sv1,&V1)){
	   for(k=L->K;k;k=k->Sled){
		if(k==K) continue;
		F1=DifBy(DifBy(V->Znach,V1),k->V);
		swprintf(Sv2,L"%s'%s",k->V->Name,L->Time->Name);
		if(FindVary(Sv2,&V2)){
		 Train TA,TB; AnsiString SA,SB,Rez=L"Составы:\r\n";
		 F2=DifBy(DifBy(V->Znach,V2),K->V);
		 TA.TrainFromForm(F1); TB.TrainFromForm(F2);
		 TA.ToStr(SA); Rez+=SA; Rez+=L"\r\n";
		 TB.ToStr(SB); Rez+=SB; Rez+=L"\r\n";
		 TA.ToFile(L"F1.prn",false);
		 TB.ToFile(L"F2.prn",false);
		 if(TA.Eqv(&TB)){
		  Rez+=L"эквивалентны!";
//          Application->MessageBoxA(Rez.c_str(),L"TA==TB?L",MB_OK);
		  continue;
		 }
		 else{
		  Rez+=L"не эквивалентны!";
//          Application->MessageBoxA(Rez.c_str(),L"TA==TB?L",MB_OK);
		  NEHOL=true; goto N;
		 }
//         if(F1==F2)
//          continue;
//         else{
//          NEHOL=true; goto N;
//         }
	 }}}}
	 if(L->W) NEHOL=true;
   }}
N: if(NEHOL){
//	fwprintf(DGSFILE,L"<h4 align=\"left\"><font color=\"#000099\">"
//	 L"Система неголономная.</font></h4>\n");
	SLD->Add(L"<h4 align=\"left\"><font color=\"#000099\">"
	 L"Система неголономная.</font></h4>");
	for(Cord *Z=L->Z;Z;Z=Z->Sled){ Vary *V;
	 swprintf(Inf,L"%s'%s",Z->V->Name,L->Time->Name); FindVary(Inf,&V);
	 for(Cord *z=L->Z;z;z=z->Sled){
	  if(VarInForm(true,V->Znach,z->V)){ CHAP=false; goto Ch; }
	}}
	for(Elem *E=L->E;E;E=E->Sled){
	 for(Cord *z=L->Z;z;z=z->Sled){
	  if(VarInForm(true,E->Znach,z->V)){ CHAP=false; goto Ch; }
	  if(VarInForm(true,E->K->Znach,z->V)){ CHAP=false; goto Ch; }
	}}
Ch: if(CHAP)
//	 fwprintf(DGSFILE,L"<h4 align=\"left\"><font color=\"#000099\">"
//	  L"Это система С.А. Чаплыгина.</font></h4>\n");
	 SLD->Add(L"<h4 align=\"left\"><font color=\"#000099\">"
	  L"Это система С.А. Чаплыгина.</font></h4>");
   }
   else{
//	fwprintf(DGSFILE,L"<h4 align=\"left\"><font color=\"#009900\">"
//	 L"Система голономная.<br>"
//	 L"Все трехиндексные символы для зависимых скоростей равны нулю.<br>"
//	 L"</font></h4>\n");
	SLD->Add(L"<h4 align=\"left\"><font color=\"#009900\">"
	 L"Система голономная.<br>"
	 L"Все трехиндексные символы для зависимых скоростей равны нулю.<br>"
	 L"</font></h4>");
 }}}
 else{
//  if(INVERSE&&(I=FindInstLast(L"ОБОБЩЕННЫЕ КООРДИНАТЫ"))){
  if(I=FindInstLast(L"ОБОБЩЕННЫЕ КООРДИНАТЫ")){ wchar_t *Buf;
   Vary *V; Parm *p;
//   fwprintf(DGSFILE,L"<h4 align=\"left\"><font color=\"#000099\">"
//	L"Обобщенные координаты (согласно инструкции):</font></h4>\n");
   SLD->Add(L"<h4 align=\"left\"><font color=\"#000099\">"
	L"Обобщенные координаты (согласно инструкции):</font></h4>");
   l=0;
   for(p=I->First;p->Sled;p=p->Sled){
	if(FindVary(p->Nam,&V)) TakeCord(V);
	else{
	 swprintf(Inf,L"Обобщенная координата \"%s\" не найдена в списке переменных!",p->Nam);
//	 fwprintf(DGSFILE,
//	  L"<h4 align=\"left\"><font color=\"#000099\">%s:</font></h4>\n",Inf);
	 swprintf(Sdgs,L"<h4 align=\"left\">"
	  L"<font color=\"#000099\">%s:</font></h4>",Inf);
	 SLD->Add(Sdgs);
	 Rez=false;
	}
	Buf=(wchar_t *)calloc(wcslen(p->Nam)+1,SzC);
	wcscpy(Buf,p->Nam); Buf=LiterGrekToHtm(Buf);//Grek(Inf);
	N++; g=wcslen(Buf);
//	if((l+g+2)>lmax){ fwprintf(DGSFILE,L"<br>\n "); l=0; }
//	l+=g+2; fwprintf(DGSFILE,L"%s, ",Buf);
	if((l+g+2)>lmax){
	 sdgs+=swprintf(sdgs,L"<br>"); l=0;
	 SLD->Add(Sdgs); Sdgs[0]=' '; Sdgs[1]='\0'; sdgs=Sdgs+1;
	}
	l+=g+2; sdgs+=swprintf(sdgs,L"%s, ",Buf);
	free(Buf);
   }
   if(FindVary(p->Nam,&V)) TakeCord(V);
   else{
	swprintf(Inf,L"Обобщенная координата \"%s\" не найдена в списке переменных!",p->Nam);
//	fwprintf(DGSFILE,
//	 L"<h4 align=\"left\"><font color=\"#000099\">%s:</font></h4>\n",Inf);
	swprintf(Sdgs,L"<h4 align=\"left\">"
	 L"<font color=\"#000099\">%s:</font></h4>",Inf);
	SLD->Add(Sdgs);
	Rez=false;
   }
   Buf=(wchar_t *)calloc(wcslen(p->Nam)+1,SzC);
   wcscpy(Buf,p->Nam); Buf=LiterGrekToHtm(Buf);//Grek(Inf);
   N++; g=wcslen(Buf);
//   if((l+g+1)>lmax) fwprintf(DGSFILE,L"<br>\n ");
//   fwprintf(DGSFILE,L"%s",Buf);
   if((l+g+1)>lmax){
	sdgs+=swprintf(sdgs,L"<br>"); l=0;
	SLD->Add(Sdgs); Sdgs[0]=' '; Sdgs[1]='\0'; sdgs=Sdgs+1;
   }
   free(Buf);
//   fwprintf(DGSFILE,L"<h4 align=\"left\"><font color=\"#000099\">"
//	L"Число степеней свободы = %d.</font></h4>\n",N);
   swprintf(Sdgs,L"<h4 align=\"left\"><font color=\"#000099\">"
	L"Число степеней свободы = %d.</font></h4>\n",N);
   SLD->Add(Sdgs);
   N=0;
  }
  else
//   fwprintf(DGSFILE,L"<h4 align=\"left\"><font color=\"#000099\">"
//	L"НЕТ ОБОБЩЕННЫХ КООРДИНАТ!</font></h4>\n");
   SLD->Add(L"<h4 align=\"left\"><font color=\"#000099\">"
	L"НЕТ ОБОБЩЕННЫХ КООРДИНАТ!</font></h4>");
 }
 if(L->B){
  CalcVWBodys(CGauge,StatusBar);
  if(I=FindInstLast(L"УПРАВЛЕНИЕ COF"))
   ControlCOF(I,StatusBar,TC);
  else if(I=FindInstLast(L"РАССЧИТАТЬ COF"))
   BuildCOF(I,StatusBar,TC);
 }
 Np=KolElem(L->P);
 if(Np){ int LL;
//  fwprintf(DGSFILE,L"<h4 align=\"left\"><font color=\"#000099\">"
//   L"Неизвестные величины:</font></h4>\n");
  SLD->Add(L"<h4 align=\"left\"><font color=\"#000099\">"
   L"Неизвестные величины:</font></h4>\n");
  Buf=(wchar_t *)calloc(wcslen(L->P->V->Name)+1,SzC);
  wcscpy(Buf,L->P->V->Name); Buf=LiterGrekToHtm(Buf);//Grek(Inf);
  LL=wcslen(Buf);
//  fwprintf(DGSFILE,L"%s",Buf);
  sdgs+=swprintf(sdgs,L"%s",Buf);
  free(Buf);
//   l=fwprintf(DGSFILE,L"%s",L->P->V->Name);
  for(Cord *n=L->P->Sled;n;n=n->Sled){
   Buf=(wchar_t *)calloc(wcslen(n->V->Name)+1,SzC);
   wcscpy(Buf,n->V->Name); Buf=LiterGrekToHtm(Buf);//l=Grek(Inf);
   l=wcslen(Buf);
   if((LL+2)>lmax){
//	fputws(L"<br>\n ",DGSFILE); LL=l+2;
	sdgs+=swprintf(sdgs,L"<br>\n ",DGSFILE); LL=l+2;
   }
   else LL+=l+2;
//   fwprintf(DGSFILE,L", %s",Buf);
   sdgs+=swprintf(sdgs,L", %s",Buf);
   free(Buf);
//    if((l=wcslen(n->V->Name)+2)>lmax){ fputws(L"<br>\n ",DGSFILE); l=0; }
//    l+=fwprintf(DGSFILE,L", %s",n->V->Name);
  }
//  fwprintf(DGSFILE,L"<h4 align=\"left\"><font color=\"#000099\">"
//   L"Число неизвестных = %d.</font></h4>\n",Np);
  sdgs+=swprintf(sdgs,L"<h4 align=\"left\"><font color=\"#000099\">"
   L"Число неизвестных = %d.</font></h4>",Np);
  SLD->Add(Sdgs); *Sdgs='\0'; sdgs=Sdgs;
 }
// fwprintf(DGSFILE,L"<b><br>\nПеременная &nbsp;&nbsp; 'Время': </b>");
 sdgs+=swprintf(sdgs,L"<br>");
 SLD->Add(Sdgs); *Sdgs='\0'; sdgs=Sdgs;
 sdgs+=swprintf(sdgs,L"<b>Переменная &nbsp;&nbsp; 'Время': </b>");
// swprintf(S,L"%s",L->Time->Name);
 sdgs+=swprintf(sdgs,L"%s",L->Time->Name);
// PrintForm(false,DGSFILE,S,L->Time->Znach);
 sdgs=FormToStringList(L->Time->Znach,S,Sdgs,sdgs,SLD,false,&SF);
 sdgs+=swprintf(sdgs,L"<br>");
 SLD->Add(Sdgs); *Sdgs='\0'; sdgs=Sdgs;
// fwprintf(DGSFILE,L"<b><br>\nПеременная 'Частота': </b>");
 sdgs+=swprintf(sdgs,L"<b>Переменная &nbsp;&nbsp; 'Частота': </b>");
// swprintf(S,L"%s",L->Freq->Name);
 sdgs+=swprintf(sdgs,L"%s",L->Freq->Name);
// PrintForm(false,DGSFILE,S,L->Freq->Znach);
// swprintf(S,L"%s",L->Time->Name);
 sdgs=FormToStringList(L->Freq->Znach,S,Sdgs,sdgs,SLD,false,&SF);
 sdgs+=swprintf(sdgs,L"<br>");
 SLD->Add(Sdgs); *Sdgs='\0'; sdgs=Sdgs;
 if(L->E){
  Elem *E; Magazine *MN=NULL;
  if(StatusBar){
   wcscpy(SBold,StatusBar->SimpleText.c_str());
   StatusBar->SimpleText=L"Диагностика элементов";
  }
  if(CGauge){ CGauge->MaxValue=KolElem(L->E); CGauge->Progress=0; }
//  fwprintf(DGSFILE,L"<h4 align=\"left\"><font color=\"#000099\">"
//   L"Список элементов:</font></h4>\n");
  sdgs+=swprintf(sdgs,L"<h4 align=\"left\"><font color=\"#000099\">"
   L"Список элементов:</font></h4>");
  SLD->Add(Sdgs); *Sdgs='\0'; sdgs=Sdgs;
//  fputws(L"<table width=\"100%\" border=\"0\">\n",DGSFILE);
  SLD->Add(L"<table width=\"100%\" border=\"0\">");
  for(E=L->E,i=0;E;E=E->Sled){
   if(CGauge) CGauge->Progress++;
   if(FirstLetterCorrect(E->Name[0])){
//	if(!i) fputws(L"<tr>\n",DGSFILE);
//	else if(!(i%KColumn)) fputws(L"</tr>\n<tr>\n",DGSFILE);
	if(!i) SLD->Add(L"<tr>");
	else if(!(i%KColumn)){
	 SLD->Add(L"</tr>"); SLD->Add(L"<tr>");
	}
	i++;
//	fputws(L" <td><div align=\"left\"><b>",DGSFILE);
	sdgs+=swprintf(sdgs,L" <td><div align=\"left\"><b>");
	if(SMALLFONT)
//	 fwprintf(DGSFILE,L"<font size=-%d>",1);
	 sdgs+=swprintf(sdgs,L"<font size=-%d>",1);
//	PrintForm(false,DGSFILE,E->Name,E->Znach);
	sdgs=FormToStringList(E->Znach,E->Name,Sdgs,sdgs,SLD,false,&SF);
	if(SMALLFONT) sdgs+=swprintf(sdgs,L"</font>");
	sdgs+=swprintf(sdgs,L"</b></div></td>");
	SLD->Add(Sdgs); *Sdgs='\0'; sdgs=Sdgs;
   }
   else TakeMagazine(&MN,E->Name);
  }
//  fputws(L"</tr>",DGSFILE);
//  fputws(L"</table>\n",DGSFILE);
  sdgs+=swprintf(sdgs,L"</tr></table>");
  SLD->Add(Sdgs); *Sdgs='\0'; sdgs=Sdgs;
  if(MN){ Magazine *m;
   IsError=true;
//   fwprintf(DGSFILE,L"<h4 align=\"left\"><font color=\"#000099\">"
//	L"Список нераспознанных элементов:</font></h4>\n");
//   fputws(L"<table width=\"100%\" border=\"0\">\n",DGSFILE);
//   fputws(L"<tr>",DGSFILE);
   SLD->Add(L"<h4 align=\"left\"><font color=\"#000099\">"
	L"Список нераспознанных элементов:</font></h4>");
   SLD->Add(L"<table width=\"100%\" border=\"0\">");
   sdgs+=swprintf(sdgs,L"<tr>");
   for(m=MN,i=0;m;m=m->Sled){
//	if(!i) fputws(L"<tr>\n",DGSFILE);
//	else if(!(i%(2*KColumn))) fputws(L"</tr>\n<tr>\n",DGSFILE);
	if(!i){
	 sdgs+=swprintf(sdgs,L"<tr>");
	}
	else if(!(i%(2*KColumn))){
	 sdgs+=swprintf(sdgs,L"</tr>");
	 SLD->Add(Sdgs); *Sdgs='\0'; sdgs=Sdgs;
	 sdgs+=swprintf(sdgs,L"<tr>");
	}
	SLD->Add(Sdgs); *Sdgs='\0'; sdgs=Sdgs;
//	i++; fputws(L" <td><div align=\"left\">",DGSFILE);
	i++; sdgs+=swprintf(sdgs,L" <td><div align=\"left\">");
	Buf=(wchar_t *)calloc(wcslen(m->S)+1,SzC);
	wcscpy(Buf,m->S); Buf=LiterGrekToHtm(Buf);
//	fwprintf(DGSFILE,L"<b>%s</b></div></td>\n",Buf);
	sdgs+=swprintf(sdgs,L"<b>%s</b></div></td>",Buf);
	SLD->Add(Sdgs); *Sdgs='\0'; sdgs=Sdgs;
	free(Buf);
   }
//   fputws(L"</tr>",DGSFILE);
///   fputws(L"</table>\n",DGSFILE);
   SLD->Add(L"</tr></table>");
   DelAllMagazine(&MN);
   swprintf(Inf,L"   В исходных данных записаны элементы, которые по\n"
	L"первой букве имени нельзя идентифицировать как\n"
	L"инерционные, диссипативные, упругие, силовые,\n"
	L"нелинейные. См. закладку \"ДИАГНОСТИКА\"");
   Error(0,0,true);
  }
  StatusBar->SimpleText=SBold;
 }
 if(L->B){
  wcscpy(SBold,StatusBar->SimpleText.c_str());
  StatusBar->SimpleText=L"Диагностика твёрдых тел";
  if(CGauge){ CGauge->MaxValue=KolElem(L->B); CGauge->Progress=0; }
//  fwprintf(DGSFILE,L"<h4 align=\"left\"><font color=\"#000099\">"
//   L"Список твердых тел:</font></h4>\n");
  SLD->Add(L"<h4 align=\"left\"><font color=\"#000099\">"
   L"Список твердых тел:</font></h4>");
  for(Body *B=L->B;B;B=B->Sled){
   if(CGauge) CGauge->Progress++;
//   fwprintf(DGSFILE,L"<br>\nСтрока №%4d: %s",B->Line,B->Name);
//   if(B->BaseName){ fwprintf(DGSFILE,L"~%s",B->BaseName); }
//   fputws(L" | ",DGSFILE);
   sdgs+=swprintf(sdgs,L"<br>Строка №%4d: %s",B->Line,B->Name);
   if(B->BaseName) sdgs+=swprintf(sdgs,L"~%s",B->BaseName);
   sdgs+=swprintf(sdgs,L" | ");
   if(B->KoordSys){
//	ReadAllRight(B->KoordSys,DGSFILE,true,true);
	sdgs=ReadAllRight(B->KoordSys,Sdgs,sdgs,SLD,&SF,true,true);
//	fputws(L" | ",DGSFILE);
	sdgs+=swprintf(sdgs,L" | ");
   }
   if(B->Inert){
//	ReadAllRight(B->Inert,DGSFILE,true,false);
	sdgs=ReadAllRight(B->Inert,Sdgs,sdgs,SLD,&SF,true,false);
   }
 //  fputwc(';',DGSFILE);
   *sdgs++=';'; *sdgs='\0';
   SLD->Add(Sdgs); *Sdgs='\0'; sdgs=Sdgs;
  }
  StatusBar->SimpleText=SBold;
 }
 if(L->T){
  wcscpy(SBold,StatusBar->SimpleText.c_str());
  StatusBar->SimpleText=L"Диагностика стержней";
  if(CGauge){
   CGauge->MaxValue=KolElem(L->T); CGauge->Progress=0;
  }
//  fwprintf(DGSFILE,L"<h4 align=\"left\"><font color=\"#000099\">"
//   L"Список стержней:</font></h4>\n");
  SLD->Add(L"<h4 align=\"left\"><font color=\"#000099\">"
   L"Список стержней:</font></h4>");
  for(Stringer *T=L->T;T;T=T->Sled){
   if(CGauge) CGauge->Progress++;
//   fwprintf(DGSFILE,L"<br>\nСтрока №%4d: %s",T->Line,T->Name);
//   fputws(L" \\ ",DGSFILE);
   SLD->Add(L"<br>");
   sdgs+=swprintf(sdgs,L"Строка №%4d: %s \\ ",T->Line,T->Name);
   if(T->Parms)
//	ReadAllRight(T->Parms,DGSFILE,true,true);
	sdgs=ReadAllRight(T->Parms,Sdgs,sdgs,SLD,&SF,true,true);
//   fputwc(';',DGSFILE);
   *sdgs++=';'; *sdgs='\0';
  }
  StatusBar->SimpleText=SBold;
 }
 if(L->Inst){ wchar_t InstName[256];
  wcscpy(SBold,StatusBar->SimpleText.c_str());
  StatusBar->SimpleText=L"Диагностика инструкций";
  if(CGauge){ CGauge->MaxValue=KolElem(L->Inst); CGauge->Progress=0; }
//  fwprintf(DGSFILE,L"<h4 align=\"left\"><font color=\"#000099\">"
//   L"Список инструкций:</font></h4>\n");
  SLD->Add(L"<h4 align=\"left\"><font color=\"#000099\">"
   L"Список инструкций:</font></h4>");
  for(Root *I=L->Inst;I;I=I->Sled){
   if(CGauge) CGauge->Progress++;
   wcscpy(InstName,I->Name); LowToUpp(InstName);
   if(I->First){
//	fwprintf(DGSFILE,L"Строка №%4d: %s := ",I->Line,I->Name);
	sdgs+=swprintf(sdgs=Sdgs,L"Строка №%4d: %s := ",I->Line,I->Name);
	if((wcsstr(InstName,L"ЫПОЛНИЛ")==InstName+1)||
	   (wcsstr(InstName,L"ИСУН")==InstName+1)||
	   (wcsstr(InstName,L"АБОТА")==InstName+1))
//	 ReadAllRight(I->First,DGSFILE,false,false);
	 sdgs=ReadAllRight(I->First,Sdgs,sdgs,SLD,&SF,false,false);
	else
//	 ReadAllRight(I->First,DGSFILE,true,true);
	 sdgs=ReadAllRight(I->First,Sdgs,sdgs,SLD,&SF,true,true);
//	fputws(L";<br>\n",DGSFILE);
	sdgs+=swprintf(sdgs,L";<br>");
	SLD->Add(Sdgs); *Sdgs='\0'; sdgs=Sdgs;
   }
   else
//	fwprintf(DGSFILE,L"Строка №%4d: %s;<br>\n",I->Line,I->Name);
   sdgs+=swprintf(sdgs,L"Строка №%4d: %s;<br>",I->Line,I->Name);
  }
  StatusBar->SimpleText=SBold;
 }
 if(L->A&&ArrayDiagnostik){
  StatusBar->SimpleText=L"Диагностика массивов";
  if(CGauge){
   CGauge->MaxValue=KolElem(L->A); CGauge->Progress=0;
  }
//  fwprintf(DGSFILE,L"<h4 align=\"left\"><font color=\"#000099\">"
//   L"Список массивов:</font></h4>\n");
  SLD->Add(L"<h4 align=\"left\"><font color=\"#000099\">"
   L"Список массивов:</font></h4>");
  for(Arra *a=L->A;a;a=a->Sled){
   if(CGauge) CGauge->Progress++;
//   fwprintf(DGSFILE,L"Строка №%4d: %s[%d] = ",a->Line,a->Name,a->N);
   sdgs+=swprintf(sdgs,L"Строка №%4d: %s[%d] = ",
	a->Line,a->Name,a->N);
   for(int i=0;i<a->N;i++){
	if(a->F)
	 PrintForm(false,DGSFILE,NULL,a->F[i]);
//	else if(a->A) fwprintf(DGSFILE,L"%g",a->A[i]);
//	if(i<a->N-1) fputws(L", ",DGSFILE);
//	else fputws(L";<br>\n",DGSFILE);
	else if(a->A) sdgs+=swprintf(sdgs,L"%g",a->A[i]);
	if(i<a->N-1) sdgs+=swprintf(sdgs,L", ");
	else{
	 sdgs+=swprintf(sdgs,L";<br>");
	 SLD->Add(Sdgs); *Sdgs='\0'; sdgs=Sdgs;
  }}}
  StatusBar->SimpleText=SBold;
 }
 if(L->Oper){
  wcscpy(SBold,StatusBar->SimpleText.c_str());
  StatusBar->SimpleText=L"Диагностика операторов";
  if(CGauge){ CGauge->MaxValue=KolElem(L->Oper); CGauge->Progress=0; }
//  fwprintf(DGSFILE,L"<h4 align=\"left\"><font color=\"#000099\">"
//   L"Список операторов:</font></h4>\n");
  SLD->Add(L"<h4 align=\"left\"><font color=\"#000099\">"
   L"Список операторов:</font></h4>");
  for(Root *I=L->Oper;I;I=I->Sled){ int Ln=I->Line;
   if(CGauge) CGauge->Progress++;
//   fwprintf(DGSFILE,L"Строка №%4d: %s : ",I->Line,I->Name);
   sdgs+=swprintf(sdgs,L"Строка №%4d: %s : ",I->Line,I->Name);
   if(I->First&&(I->First->Line>Ln)) Ln=I->First->Line;
   wcscpy(Inf,I->Name); LowToUpp(Inf);
   if(!wcscmp(Inf,L"ТЕНЗОР ИНЕРЦИИ"))
//	ReadAllRight(I->First,DGSFILE,true,true);
	sdgs=ReadAllRight(I->First,Sdgs,sdgs,SLD,&SF,true,true);
   else
//	ReadAllRight(I->First,DGSFILE,true,false);
	sdgs=ReadAllRight(I->First,Sdgs,sdgs,SLD,&SF,true,false);
//   fputws(L";<br>\n",DGSFILE);
   sdgs+=swprintf(sdgs,L";<br>");
   SLD->Add(Sdgs); *Sdgs='\0'; sdgs=Sdgs;
  }
  StatusBar->SimpleText=SBold;
 }
 if(L->L){
  int i,j,k,N; Magazine *MArr=NULL;
  wcscpy(SBold,StatusBar->SimpleText.c_str());
  StatusBar->SimpleText=L"Диагностика линейных интерполяций";
  if(CGauge){ int K=0; Ipll *I;
   for(I=L->L;I;I=I->Sled) K++;
   CGauge->MaxValue=K; CGauge->Progress=0;
  }
//  fwprintf(DGSFILE,L"<h4 align=\"left\"><font color=\"#000099\">"
//   L"%s</font></h4>\n",L"Линейная интерполяция");
  swprintf(Sdgs,L"<h4 align=\"left\"><font color=\"#000099\">"
   L"%s</font></h4>",L"Линейная интерполяция");
  for(Ipll *i=L->L;i;i=i->Sled){ int j;
   if(CGauge) CGauge->Progress++;
   if(!i->A->N){ TakeMagazine(&MArr,i->A->Name); continue; }
//   fputws(L"<table width=\"100%\" border=\"1\">\n",DGSFILE);
//   fputws(L"<tr>\n",DGSFILE);
//   fputws(L" <td colspan=\"8\">",DGSFILE);
//   fwprintf(DGSFILE,L"<div align=\"center\"><b>ipl(%s, ",i->A->Name);
   SLD->Add(L"<table width=\"100%\" border=\"1\">");
   SLD->Add(L"<tr>");
   sdgs+=swprintf(sdgs,L" <td colspan=\"8\">");
   sdgs+=swprintf(sdgs,L"<div align=\"center\"><b>ipl(%s, ",
	i->A->Name);
//   PrintForm(false,DGSFILE,NULL,i->F);
   sdgs=FormToStringList(i->F,NULL,Sdgs,sdgs,SLD,false,&SF);
//   fputws(L")</b></div>\n </td>\n",DGSFILE);
//   fputws(L"</tr>\n",DGSFILE);
//   fputws(L"<tr>\n",DGSFILE);
   SLD->Add(L")</b></div>"); SLD->Add(L"</td>");
   SLD->Add(L"</tr>"); SLD->Add(L"<tr>");
   for(j=0;j<4;j++){
//	fputws(L" <td><div align=\"center\"><b>",DGSFILE);
	sdgs+=swprintf(sdgs,L" <td><div align=\"center\"><b>");
//	PrintForm(false,DGSFILE,NULL,i->F);
	sdgs=FormToStringList(i->F,NULL,Sdgs,sdgs,SLD,false,&SF);
//	fputws(L"</b></div></td>",DGSFILE);
//	fwprintf(DGSFILE,L" <td><div align=\"center\"><b>%s</b></div></td>\n",i->A->Name);
	sdgs+=swprintf(sdgs,L"</b></div></td>",DGSFILE);
	sdgs+=swprintf(sdgs,L" <td><div align=\"center\"><b>%s"
	 L"</b></div></td>",i->A->Name);
	SLD->Add(Sdgs); *Sdgs='\0'; sdgs=Sdgs;
   }
//   fputws(L"</tr>\n",DGSFILE);
//   fputws(L"<tr>\n",DGSFILE);
   SLD->Add(L"</tr>"); SLD->Add(L"<tr>");
   for(int j=0;j<i->A->N;j+=2){
	if(j&&!(j%8)){
//	 fputws(L"</tr>\n",DGSFILE); fputws(L"<tr>\n",DGSFILE);
	 SLD->Add(L"</tr>"); SLD->Add(L"<tr>");
	}
	if(i->A->A)
//	 fwprintf(DGSFILE,L" <td>%13g</td><td>%13g</td>\n",
//	  i->A->A[j],i->A->A[j+1]);
	 swprintf(Sdgs,L" <td>%13g</td><td>%13g</td>",
	  i->A->A[j],i->A->A[j+1]);
	else if(i->A->F)
//	 fwprintf(DGSFILE,L" <td>%13g</td><td>%13g</td>\n",
//	  Val(i->A->F[j]),Val(i->A->F[j+1]));
	 swprintf(Sdgs,L" <td>%13g</td><td>%13g</td>",
	  Val(i->A->F[j]),Val(i->A->F[j+1]));
   }
   SLD->Add(Sdgs); *Sdgs='\0'; sdgs=Sdgs;
//   fputws(L"<tr>\n",DGSFILE);
//   fputws(L"</table>",DGSFILE);
   SLD->Add(L"<tr>"); SLD->Add(L"</table>");
  }
  if(MArr){
   if(KolElem(MArr)>1){ wchar_t *inf=Inf;
	inf+=swprintf(Inf,L"Не заданы массивы, на которые есть ссылки в линейных интерполяциях:\n"
	 L"%s",MArr->S);
	for(Magazine *m=MArr->Sled;m;m=m->Sled) inf+=swprintf(inf,L", %s",m->S);
   }
   else
	swprintf(Inf,L"Не задан массив (\"%s\"), на который есть ссылка в линейных интерполяциях",MArr->S);
   Error(0,0,true); Rez=false;
  }
  StatusBar->SimpleText=SBold;
 }
 if(L->S&&SplineDiagnostik){
  int i,j,k,N; Magazine *MArr=NULL;
  wcscpy(SBold,StatusBar->SimpleText.c_str());
  StatusBar->SimpleText=L"Диагностика сплайнов";
  if(CGauge){ int K=0; Spln *S;
   for(S=L->S;S;S=S->Sled) K++;
   CGauge->MaxValue=K; CGauge->Progress=0;
  }
//  fwprintf(DGSFILE,L"<h4 align=\"left\"><font color=\"#000099\">"
//   L"%s</font></h4>\n",L"Сплайн");
  SLD->Add(L"<h4 align=\"left\"><font color=\"#000099\">"
   L"Сплайн</font></h4>");
  for(Spln *s=L->S;s;s=s->Sled){
   if(CGauge) CGauge->Progress++;
   if(s->A&&!s->A->N){ TakeMagazine(&MArr,s->A->Name); continue; }
   N=s->A->N/2-1;
//   fputws(L"<table width=\"100%\" border=\"1\">\n",DGSFILE);
//   fputws(L"<tr>\n",DGSFILE);
//   fputws(L" <td colspan=\"4\">\n",DGSFILE);
//   fwprintf(DGSFILE,L"<div align=\"center\"><b>spln(%s, ",s->A->Name);
//   PrintForm(false,DGSFILE,NULL,s->F);
   SLD->Add(L"<table width=\"100%\" border=\"1\">");
   SLD->Add(L"<tr>");
   sdgs+=swprintf(sdgs,L" <td colspan=\"4\">");
   sdgs+=swprintf(sdgs,L"<div align=\"center\"><b>ipl(%s, ",
	s->A->Name);
//   PrintForm(false,DGSFILE,NULL,i->F);
   sdgs=FormToStringList(s->F,NULL,Sdgs,sdgs,SLD,false,&SF);
   sdgs+=swprintf(sdgs,L")=a+b*%s+c*%s^2+d*%s^3</b></div>",
	s->F.V->Name,s->F.V->Name,s->F.V->Name);
   SLD->Add(Sdgs); *Sdgs='\0'; sdgs=Sdgs;
   SLD->Add(L" </td>");
   SLD->Add(L" <td colspan=\"4\">");
   sdgs+=swprintf(sdgs,L"<div align=\"center\"><b>spln(%s, ",
	s->A->Name);
//   PrintForm(false,DGSFILE,NULL,s->F);
   sdgs=FormToStringList(s->F,NULL,Sdgs,sdgs,SLD,false,&SF);
//   fwprintf(DGSFILE,L")=a+b*%s+c*%s^2+d*%s^3</b></div>\n",
//	s->F.V->Name,s->F.V->Name,s->F.V->Name);
//   fputws(L" </td>\n",DGSFILE);
   sdgs+=swprintf(sdgs,L")=a+b*%s+c*%s^2+d*%s^3</b></div>",
	s->F.V->Name,s->F.V->Name,s->F.V->Name);
   SLD->Add(Sdgs); *Sdgs='\0'; sdgs=Sdgs;
   SLD->Add(L" </td>"); SLD->Add(L"</tr>");
   SLD->Add(L"<tr>");
//   fputws(L" <td><div align=\"center\"><b>a</b></div></td>\n"
//	L"<td><div align=\"center\"><b>b</b></div></td>\n"
//	L"<td><div align=\"center\"><b>c</b></div></td>\n"
//	L"<td><div align=\"center\"><b>d</b></div></td>\n",DGSFILE);
//   fputws(L" <td><div align=\"center\"><b>a</b></div></td>\n"
//	L"<td><div align=\"center\"><b>b</b></div></td>\n"
//	L"<td><div align=\"center\"><b>c</b></div></td>\n"
//	L"<td><div align=\"center\"><b>d</b></div></td>\n",DGSFILE);
//   fputws(L"</tr>\n",DGSFILE);
   SLD->Add(L" <td><div align=\"center\"><b>a</b></div></td>");
   SLD->Add(L"<td><div align=\"center\"><b>b</b></div></td>");
   SLD->Add(L"<td><div align=\"center\"><b>c</b></div></td>");
   SLD->Add(L"<td><div align=\"center\"><b>d</b></div></td>");
   SLD->Add(L" <td><div align=\"center\"><b>a</b></div></td>");
   SLD->Add(L"<td><div align=\"center\"><b>b</b></div></td>");
   SLD->Add(L"<td><div align=\"center\"><b>c</b></div></td>");
   SLD->Add(L"<td><div align=\"center\"><b>d</b></div></td>");
   SLD->Add(L"</tr>");
   for(i=0;i<N/2;i++){
//	fputws(L"<tr>\n",DGSFILE);
	SLD->Add(L"<tr>");
	swprintf(Sdgs,L"<td>%13g</td>",s->a[i]); SLD->Add(Sdgs);
	swprintf(Sdgs,L"<td>%13g</td>",s->b[i]); SLD->Add(Sdgs);
	swprintf(Sdgs,L"<td>%13g</td>",s->c[i]); SLD->Add(Sdgs);
	swprintf(Sdgs,L"<td>%13g</td>",s->d[i]); SLD->Add(Sdgs);
	swprintf(Sdgs,L"<td>%13g</td>",s->a[i+N/2]); SLD->Add(Sdgs);
	swprintf(Sdgs,L"<td>%13g</td>",s->b[i+N/2]); SLD->Add(Sdgs);
	swprintf(Sdgs,L"<td>%13g</td>",s->c[i+N/2]); SLD->Add(Sdgs);
	swprintf(Sdgs,L"<td>%13g</td>",s->d[i+N/2]); SLD->Add(Sdgs);
//	fputws(L"</tr>\n",DGSFILE);
	SLD->Add(L"</tr>");
   }
   if(N%2){
//	fputws(L"<tr>\n",DGSFILE);
	SLD->Add(L"<tr>");
	SLD->Add(L"<td>&nbsp;</td>");
	SLD->Add(L"<td>&nbsp;</td>");
	SLD->Add(L"<td>&nbsp;</td>");
	SLD->Add(L"<td>&nbsp;</td>");
	swprintf(Sdgs,L"<td>%13g</td>",s->a[i+N/2]); SLD->Add(Sdgs);
	swprintf(Sdgs,L"<td>%13g</td>",s->b[i+N/2]); SLD->Add(Sdgs);
	swprintf(Sdgs,L"<td>%13g</td>",s->c[i+N/2]); SLD->Add(Sdgs);
	swprintf(Sdgs,L"<td>%13g</td>",s->d[i+N/2]); SLD->Add(Sdgs);
//	fputws(L"</tr>\n",DGSFILE);
	SLD->Add(L"</tr>");
   }
   SLD->Add(L"</table>");
  }
  if(MArr){
   if(KolElem(MArr)>1){ wchar_t *inf=Inf;
	inf+=swprintf(Inf,L"Не заданы массивы, на которые есть ссылки в сплайнах:\n"
	 L"%s",MArr->S);
	for(Magazine *m=MArr->Sled;m;m=m->Sled) inf+=swprintf(inf,L", %s",m->S);
   }
   else
	swprintf(Inf,L"Не задан массив (\"%s\"), на который есть ссылки в сплайнах",MArr->S);
   Error(0,0,true); Rez=false;
  }
  StatusBar->SimpleText=SBold;
 }
// fwprintf(DGSFILE,L"<h4 align=\"left\"><font color=\"#000099\">"
//  L"Список переменных:</font></h4>\n");
 SLD->Add(L"<h4 align=\"left\"><font color=\"#000099\">"
  L"Список переменных:</font></h4>");
 wcscpy(SBold,StatusBar->SimpleText.c_str());
 StatusBar->SimpleText=L"Диагностика переменных";
//вернем переменным интегрирования и аргументам функций поиска корней значения Pust
 for(Intg *I=L->I;I;I=I->Sled) I->x->Znach.C=Pust;
 for(Solu *c=L->C;c;c=c->Sled) c->x->Znach.C=Pust;
 if(CGauge){
  int K=0; for(Vary *V=L->V;V;V=V->Sled) K++; CGauge->MaxValue=K;
  CGauge->Progress=0;
 }
 sdgs+=swprintf(sdgs,L"<table width=\"100%\" border=\"0\">");
 for(i=0,V=L->V;V;i++,V=V->Sled){
  if(CGauge) CGauge->Progress++;
//  if(i>KColumn-1){ i=0; fwprintf(DGSFILE,L"</tr>\n"); }
//  if(!i) fwprintf(DGSFILE,L"<tr>\n");
  if(i>KColumn-1){ i=0; sdgs+=swprintf(sdgs,L"</tr>"); }
  if(!i) sdgs+=swprintf(sdgs,L"<tr>");
  SLD->Add(Sdgs); *Sdgs='\0'; sdgs=Sdgs;
  sdgs+=swprintf(sdgs,L"<td width=\"%0.0f%%\">",Round(100/KColumn));
  if(SMALLFONT) sdgs+=swprintf(sdgs,L"<font size=-1>");
  swprintf(S,L"%s",V->Name);
//  PrintForm(false,DGSFILE,S,V->Znach);
  if(V->Znach.C->Atr==CNST){
   CalcKRazm(V);
   V->Znach=V->Krazm*V->Razm;
   ConvOldRazm(V);
  }
  sdgs=FormToStringList(V->Znach,S,Sdgs,sdgs,SLD,false,&SF);
  if(SMALLFONT) sdgs+=swprintf(sdgs,L"</font>");
//  for(int i=0;i<5;i++) KO[i]=0;
  sdgs+=swprintf(sdgs,L"</td>\n");
  SLD->Add(Sdgs); *Sdgs='\0'; sdgs=Sdgs;
 }
// fwprintf(DGSFILE,L"</tr></table>\n");
 SLD->Add(L"</tr></table>");
 if(CGauge) CGauge->Progress=0;
 swprintf(Inf,L"%s: %s",HeadVersion[0]?HeadVersion:HeadWork,Autor);
// StatusBar->SimpleText=Inf;
 StatusBar->SimpleText=SBold;
// LpodvD=fwprintf(DGSFILE,
//  L"<hr><font face=\"Georgia, Times New Roman, Times, serif\""
//  L" color=\"#326464\">&copy; %s, %d<br></font>\n",Autor,NYear+1900);
// LpodvD+=fwprintf(DGSFILE,L"</body>\n</html>\n");
// fclose(DGSFILE);
 for(int i=0;i<Kpodv;i++) SLD->Add(SLP->Strings[i]);
 SLD->SaveToFile(DgsFile);
 return Rez;
}
//---------------------------------------------------------------------------
void TablToReport(wchar_t *Name,wchar_t *NameArg,int CountTabl,
  double Tn,double Tk,bool Godograf,
  void (*Funcs)(double,Magazine *,double *),
  Magazine **NamesInclude,Magazine **NamesTAB,
  Magazine **NamesSpline,Magazine **NamesFurie){
//отабличиваем функции, Name=x|y(x)|z(x,y)|Заголовок(x,y,z,...)
//в первом и четвертом случае Godograf=false,  во втором и третьем - true
//CountTabl - число значений функций, Tn,Tk - диапазон изменения аргумента t,
//Funcs(double t,Magazine *M,double *Z) - процедура расчета массива
//функций Z(t) массива переменных M; NamesInclude, - магазин уже
//включенных в отчет таблиц;
 Magazine *Names=NULL,*m; //bool Period=false;
 wchar_t **Format,*s=Name,*s0,*t,*e,Val[256],*Head,File[256],Form[16],B,*Arg;
 int i,j,k,*P,*D,*C,*L,p,d,c,l,N,n,W,Wmax=96; FILE *F; //,Kg=(CountTabl-1)/2
 double *x,*y,*X=(double *)calloc(CountTabl,SzD),
  **Y=(double **)calloc(CountTabl,SzV),Step=(Tk-Tn)/(CountTabl-1),w,
  *U=(double *)calloc(CountTabl,SzD);
 wcscpy(File,Name);
 Arg=(wchar_t *)calloc(wcslen(NameArg)+1,SzC);
 wcscpy(Arg,NameArg); Arg=LiterGrekToHtm(Arg);//Grek(Arg);
 if(!(s0=wcschr(s,'('))) TakeMagazine(&Names,s);
 else{
  if(Godograf){
   *s0++='\0'; m=TakeMagazine(&Names,s); s=s0;
   if(s0=wcschr(s,',')){
	*s0++='\0'; m->Sled=(Magazine *)calloc(1,sizeof(Magazine));
	m->Sled->S=wcscpy((wchar_t *)calloc(wcslen(s)+1,SzC),s); s=s0; m=m->Sled;
   }
   if(s0=wcschr(s,')')){
	*s0='\0'; m->Sled=(Magazine *)calloc(1,sizeof(Magazine));
	m->Sled->S=wcscpy((wchar_t *)calloc(wcslen(s)+1,SzC),s);
  }}
  else{
   for(s=s0+1;*s0&&((s0=wcschr(s,','))||(s0=wcschr(s,')')));s=s0){
	B=*s0; *s0='\0';
	if(Names){
	 m->Sled=(Magazine *)calloc(1,sizeof(Magazine));
	 m->Sled->S=wcscpy((wchar_t *)calloc(wcslen(s)+1,SzC),s); m=m->Sled;
	}
	else m=TakeMagazine(&Names,s);
	*s0++=B;
 }}}
 n=KolElem(Names);
 for(i=0;i<CountTabl;i++) Y[i]=(double *)calloc(n,SzD);
 Format=(wchar_t **)calloc(n+1,SzV);
 for(i=0;i<=n;i++) Format[i]=(wchar_t *)calloc(256,SzC);
 P=(int *)calloc(n+1,SzI); D=(int *)calloc(n+1,SzI);
 C=(int *)calloc(n+1,SzI); L=(int *)calloc(n+1,SzI);
 for(i=0,x=X;i<CountTabl;i++){
  *x=Tn+Step*i; Funcs(*x,Names,y=Y[i]);
  l=swprintf(Val,L"%g",*x++);
  if(!(e=wcschr(Val,'e'))&&!(e=wcschr(Val,'E'))) p=0;
  else{ p=wcslen(e+2); if(p>P[n]) P[n]=p; *e='\0'; }
  if(!(t=wcschr(Val,'.'))) d=0; else{ d=wcslen(t+1); if(d>D[n]) D[n]=d; }
  c=l; if(d) c-=d+1; if(p) c-=p+2; if(c>C[n]) C[n]=c;
  if(P[n])
   if(D[n]) swprintf(Format[n],L"%%%d.%de",L[n]=max(L[n],D[n]+P[n]+5),D[n]);
   else     swprintf(Format[n],L"%%%d.%de",L[n]=max(L[n],C[n]+P[n]+2),C[n]);
  else
   if(D[n]) swprintf(Format[n],L"%%%d.%df",L[n]=max(L[n],C[n]+D[n]+2),D[n]);
   else     swprintf(Format[n],L"%%%d.0f",L[n]=max(L[n],C[n]));
  if(L[n]<11) L[n]=11; W=L[n]+2;
  for(j=0;j<n;j++){
   l=swprintf(Val,L"%g",*y++);
   if(!(e=wcschr(Val,'e'))&&!(e=wcschr(Val,'E'))) p=0;
   else{ p=wcslen(e+2); if(p>P[j]) P[j]=p; *e='\0'; }
   if(!(t=wcschr(Val,'.'))) d=0; else{ d=wcslen(t+1); if(d>D[j]) D[j]=d; }
   c=l; if(d) c-=d+1; if(p) c-=p+2; if(c>C[j]) C[j]=c;
   if(P[j])
	if(D[j]) swprintf(Format[j],L"%%%d.%de",L[j]=max(L[j],D[j]+P[j]+5),D[j]);
	else     swprintf(Format[j],L"%%%d.%de",L[j]=max(L[n],C[j]+P[j]+2),C[j]);
   else
	if(D[j]) swprintf(Format[j],L"%%%d.%df",L[j]=max(L[j],C[j]+D[j]+2),D[j]);
	else     swprintf(Format[j], L"%%%d.0f",L[j]=max(L[j],C[j]));
   W+=L[j]+1;
 }}
 N=Wmax/W; if(!N) N=1;
// if(HTMFILE=_wfopen(HtmFile,L"r+")){
//  if(!fseek(HTMFILE,-Lpodv,2)){
 if(SLH){
  for(int i=0;i<Kpodv;i++) SLH->Delete(SLH->Count-1);
  if(!*NamesInclude&&!*NamesTAB&&!*NamesSpline&&!*NamesFurie){
//   fwprintf(HTMFILE,
//	L"<h4 align=\"left\"><font color=\"#009900\">"
//	L"%d. Избранные графики и таблицы:</font></h4>\n",PUNKT++);
   swprintf(Shtm,
	L"<h4 align=\"left\"><font color=\"#009900\">"
	L"%d. Избранные графики и таблицы:</font></h4>\n",PUNKT++);
   SLH->Add(shtm);
  }
  TakeMagazine(NamesTAB,Name);
  Head=(wchar_t *)calloc(wcslen(Name)+wcslen(NameArg)+3,SzC);
  if(wcschr(Name,'(')) swprintf(Head,Name);
  else swprintf(Head,L"%s(%s)",Name,NameArg);
  Head=LiterGrekToHtm(Head);//Grek(Head);
//  fwprintf(HTMFILE,
//   L"<h5 align=\"left\"><font color=\"#000099\">"
//   L"Таблица значений графика \"%s\"</font></h5>\n",Head);
  swprintf(Shtm,
   L"<h5 align=\"left\"><font color=\"#000099\">"
   L"Таблица значений графика \"%s\"</font></h5>\n",Head);
  SLH->Add(Shtm);
  free(Head);
//  fputws(L"<p align=\"left\"><table width=\"100%\" border=\"1\""
//   L" height=\"0\" cellspacing=\"2\" cellpadding=\"4\">\n",HTMFILE);
//  fputws(L"<tr>\n",HTMFILE);
  SLH->Add(L"<p align=\"left\"><table width=\"100%\" border=\"1\""
   L" height=\"0\" cellspacing=\"2\" cellpadding=\"4\">");
  SLH->Add(L"<tr>");
  for(i=0;i<N;i++){
//   fwprintf(HTMFILE,L"<th>%s</th>\n",Arg);
   swprintf(Shtm,L"<th>%s</th>\n",Arg); SLH->Add(Shtm);
   for(j=0,m=Names;j<n;j++,m=m->Sled){
	wchar_t *Buf=(wchar_t *)calloc(wcslen(m->S)+1,SzC);
	swprintf(Buf,m->S); Buf=LiterGrekToHtm(Buf);//Grek(Inf);
//	fwprintf(HTMFILE,L"<th>%s</th>\n",Buf);
	swprintf(Shtm,L"<th>%s</th>\n",Buf); SLH->Add(Shtm);
	free(Buf);
  }}
//  fputws(L"</tr>\n",HTMFILE);
   SLH->Add(L"</tr>");
  for(i=0,x=X;i<CountTabl;){
//   fputws(L"<tr>\n",HTMFILE);
   SLH->Add(L"<tr>");
   for(k=0;k<N;k++){
	swprintf(Inf,Format[n],*x++); y=Y[i];
//	fputws(L"<td align=\"right\">\n",HTMFILE);
	SLH->Add(L"<td align=\"right\">");
	for(s=Inf;*s;s++)
	 if(*s==' ')
//	  fputws(L"&nbsp;",HTMFILE);
	  shtm+=swprintf(shtm,L"&nbsp;");
	 else
//	  fputwc(*s,HTMFILE);
	  shtm+=swprintf(shtm,L"%c",*s);
//	fputws(L"</td>\n",HTMFILE);
	shtm+=swprintf(shtm,L"</td>");
	SLH->Add(Shtm); *Shtm='\0'; shtm=Shtm;
	for(j=0;j<n;j++){
	 swprintf(Inf,Format[j],*y++);
//	 fputws(L"<td align=\"right\">\n",HTMFILE);
	 SLH->Add(L"<td align=\"right\">");
	 if(*s==' ')
//	  fputws(L"&nbsp;",HTMFILE);
	  shtm+=swprintf(shtm,L"&nbsp;");
	 else
//	  fputwc(*s,HTMFILE);
	  shtm+=swprintf(shtm,L"%c",*s);
//	fputws(L"</td>\n",HTMFILE);
	 shtm+=swprintf(shtm,L"</td>");
	 SLH->Add(Shtm); *Shtm='\0'; shtm=Shtm;
	}
	i++;
	if(i==CountTabl){
	 for(k++;k<N;k++)
	  for(j=0;j<n+1;j++)
//	   fputws(L"<td>&nbsp;</td>\n",HTMFILE);
	   SLH->Add(L"<td>&nbsp;</td>");
	 break;
   }}
//   fputws(L"</tr>\n",HTMFILE);
   SLH->Add(L"</tr>");
  }
//  fputws(L"</table>\n</p>",HTMFILE);
  SLH->Add(L"</table>"); SLH->Add(L"</p>");
//   fwprintf(HTMFILE,L"<hr><font face=\"Georgia, Times New Roman, Times, serif\""
//	L" color=\"#326464\">&copy; %s, %d<br></font>\n",Autor,NYear+1900);
//   fwprintf(HTMFILE,L"</body>\n</html>\n");
  for(int i=0;i<Kpodv;i++) SLH->Add(SLP->Strings[i]);
//  }
//  fseek(HTMFILE,0,2);
//  fclose(HTMFILE);
  SLH->SaveToFile(HtmFile);
 }
 swprintf(Inf,L"%s\\%s.TXT",CurrDir,File);
 for(int i=1;!_waccess(Inf,0);i++){
  if(*CurrDir) swprintf(Inf,L"%s\\%s№%d.TXT",CurrDir,File,i);
  else swprintf(Inf,L"%s№%d.TXT",File,i);
 }
 wcscpy(File,Inf);
 if(F=_wfopen(File,L"w")){
  int l=wcslen(::L->Time->Name),LL=_wtoi(Format[n]+1)+1,f=(LL+l)/2; wchar_t Formt[16];
  swprintf(Formt,L"%%%ds",f);
  fwprintf(F,Formt,::L->Time->Name);
  for(j=0,m=Names;j<n;j++,m=m->Sled){
   f=(LL-l)/2; l=wcslen(m->S); LL=_wtoi(Format[j]+1)+1; f+=(LL+l)/2+1;
   swprintf(Formt,L"%%%ds",f);
   fwprintf(F,Formt,m->S);
  }
  fputwc('\n',F);
  for(i=0,x=X;i<CountTabl;i++){
   fwprintf(F,Format[n],*x++); fputwc(' ',F);
   for(j=0,y=Y[i];j<n;j++){
	fwprintf(F,Format[j],*y++); fputwc(' ',F);
   }
   fputwc('\n',F);
  }
  fclose(F);
 }
 free(U); free(P); free(D); free(C); free(L); free(Arg);
 for(i=0;i<CountTabl;i++) free(Y[i]); free(Y); free(X);
 for(i=0;i<=n;i++) free(Format[i]); free(Format);
 DelAllMagazine(&Names);
 swprintf(Inf,L"График сохранен в виде таблицы в текстовом файле\n\"%s\".\n"
  L"Его можно просмотреть и отредактировать соответствующим\n"
  L"текстовым редактором, вставить в документ WORD и т.д.",File);
 Application->MessageBox(Inf,L"Внимание!",MB_OK|MB_ICONWARNING);
}
//---------------------------------------------------------------------------
void SplToReport(wchar_t *Name,int CountTabl,double Tn,double Tk,
  bool Godograf,void (*Funcs)(double,Magazine *,double *),
  Magazine **NamesInclude,Magazine **NamesTAB,
  Magazine **NamesSpline,Magazine **NamesFurie,TCGauge *CGauge){
//построение интерполяционных формул для графика функции,
//Name=x|y(x)|z(x,y)|Заголовок(x,y,z,...)
//в первом и четвертом случае Godograf=false,  во втором и третьем - true
//CountTabl - дискретность функций, Tn,Tk - диапазон изменения аргумента t,
//Funcs(double t,Magazine *M,double *Z) - процедура расчета массива
//функций Z(t) переменных M; NamesInclude - магазин уже
//включенных в отчет формул;
 Magazine *Names=NULL,*m,*Files=NULL; FILE *F,*Ft;
 wchar_t **Format,*s=Name,*s0,*t,*e,Val[256],Head[256],File[256],*LastFile,Dir[256];
 int i,j,k,*P,*D,*C,*L,p,d,c,l,N,n,W,Wmax=96,
  Kinst=KolInst(TextFromFile(WORKDIR,L"EditKiDyM",&SLK,111))+
   KolInst(TextFromFile(WORKDIR,L"EditKiDyM",&SLK,159));
 double *x,*y,*X=(double *)calloc(CountTabl,SzD),
  **Ys=(double **)calloc(CountTabl,SzV),Step=(Tk-Tn)/(CountTabl-1);
 wcscpy(Dir,InpFile); if(s0=wcsrchr(Dir,'.')) *s0='\0';
 swprintf(File,L"%s_S.txt",Dir);
 if(_waccess(File,0))
  F=_wfopen(File,L"w");
 else{
  LastFile=TakeMagazine(&Files,File)->S;
  swprintf(File,L"%s_S№1.txt",Dir); i=1;
  while(!_waccess(File,0)){
   LastFile=TakeMagazine(&Files,File)->S;
   swprintf(File,L"%s_S№%d.txt",Dir,++i);
  }
  if((i=KolElem(Files))>1)
   swprintf(Inf,L"Уничтожить их и писать результат в файл:\n<%s> - <Да>\n"
	L"Сохранить их все и писать в новый файл:\n<%s> - <Нет>\n"
	L"Продолжать писать в последний из них:\n<%s> - <Отмена>",
	Files->S,File,LastFile);
  else
   swprintf(Inf,L"Заменить его и писать результат в файл:\n<%s> - <Да>\n"
	L"Сохранить его и писать в новый файл:\n<%s> - <Нет>\n"
	L"Продолжать писать в этот файл:\n<%s> - <Отмена>",Files->S,File,LastFile);
  switch(Application->MessageBox(Inf,
		 i>1?L"Обнаружены файлы со сплайнами":L"Обнаружен файл со сплайном",
		 MB_YESNOCANCEL|MB_ICONQUESTION)){
   case ID_YES:
	for(m=Files;m;m=m->Sled) if(!_waccess(m->S,0)) _wunlink(m->S);
	F=_wfopen(Files->S,L"w"); wcscpy(File,Files->S); break;
   case ID_NO: F=_wfopen(File,L"w"); break;
   case ID_CANCEL: F=_wfopen(LastFile,L"a"); wcscpy(File,LastFile); break;
 }}
 DelAllMagazine(&Files);
// if(s0=wcsrchr(Dir,'\\')) *s0='\0';
 if(!(s0=wcschr(s,'('))) TakeMagazine(&Names,s);
 else{
  if(Godograf){
   *s0++='\0'; m=TakeMagazine(&Names,s); s=s0;
   if(s0=wcschr(s,',')){
	*s0++='\0'; m->Sled=(Magazine *)calloc(1,sizeof(Magazine));
	m->Sled->S=wcscpy((wchar_t *)calloc(wcslen(s)+1,SzC),s); s=s0; m=m->Sled;
   }
   if(s0=wcschr(s,')')){
	*s0='\0'; m->Sled=(Magazine *)calloc(1,sizeof(Magazine));
	m->Sled->S=wcscpy((wchar_t *)calloc(wcslen(s)+1,SzC),s);
  }}
  else{
   for(s=s0+1;*s0&&((s0=wcschr(s,','))||(s0=wcschr(s,')')));s=s0){
	*s0++='\0';
	if(Names){
	 m->Sled=(Magazine *)calloc(1,sizeof(Magazine));
	 m->Sled->S=wcscpy((wchar_t *)calloc(wcslen(s)+1,SzC),s); m=m->Sled;
	}
	else m=TakeMagazine(&Names,s);
 }}}
 n=KolElem(Names);
 for(i=0;i<CountTabl;i++) Ys[i]=(double *)calloc(n,SzD);
 Format=(wchar_t **)calloc(n+1,SzV);
 for(i=0;i<=n;i++) Format[i]=(wchar_t *)calloc(256,SzC);
 P=(int *)calloc(n+1,SzI); D=(int *)calloc(n+1,SzI);
 C=(int *)calloc(n+1,SzI); L=(int *)calloc(n+1,SzI);
 if(CGauge){ CGauge->MaxValue=CountTabl; CGauge->Progress=0; }
 for(i=0,x=X;i<CountTabl;i++){
  if(CGauge) CGauge->Progress++;
  *x=Tn+Step*i;
  Funcs(*x,Names,y=Ys[i]);
  l=swprintf(Val,L"%g",*x++);
  if(!(e=wcschr(Val,'e'))&&!(e=wcschr(Val,'E'))) p=0;
  else{ p=wcslen(e+2); if(p>P[n]) P[n]=p; *e='\0'; }
  if(!(t=wcschr(Val,'.'))) d=0; else{ d=wcslen(t+1); if(d>D[n]) D[n]=d; }
  c=l; if(d) c-=d+1; if(p) c-=p+2; if(c>C[n]) C[n]=c;
  if(P[n])
   if(D[n]) swprintf(Format[n],L"%%%d.%de",L[n]=max(L[n],D[n]+P[n]+5),D[n]);
   else     swprintf(Format[n],L"%%%d.%de",L[n]=max(L[n],C[n]+P[n]+2),C[n]);
  else
   if(D[n]) swprintf(Format[n],L"%%%d.%df",L[n]=max(L[n],C[n]+D[n]+2),D[n]);
   else     swprintf(Format[n],L"%%%d.0f",L[n]=max(L[n],C[n]));
  if(L[n]<11) L[n]=11; W=L[n]+2;
  for(j=0;j<n;j++){
   l=swprintf(Val,L"%g",*y++);
   if(!(e=wcschr(Val,'e'))&&!(e=wcschr(Val,'E'))) p=0;
   else{ p=wcslen(e+2); if(p>P[j]) P[j]=p; *e='\0'; }
   if(!(t=wcschr(Val,'.'))) d=0; else{ d=wcslen(t+1); if(d>D[j]) D[j]=d; }
   c=l; if(d) c-=d+1; if(p) c-=p+2; if(c>C[j]) C[j]=c;
   if(P[j])
	if(D[j]) swprintf(Format[j],L"%%%d.%de",L[j]=max(L[j],D[j]+P[j]+5),D[j]);
	else     swprintf(Format[j],L"%%%d.%de",L[j]=max(L[n],C[j]+P[j]+2),C[j]);
   else
	if(D[j]) swprintf(Format[j],L"%%%d.%df",L[j]=max(L[j],C[j]+D[j]+2),D[j]);
	else     swprintf(Format[j], L"%%%d.0f",L[j]=max(L[j],C[j]));
   W+=L[j]+1;
 }}
 if(CGauge) CGauge->Progress=0;
 N=Wmax/W; if(!N) N=1;
// if(HTMFILE=_wfopen(HtmFile,L"r+")){
 if(SLH){
//  if(!fseek(HTMFILE,-Lpodv,2)){
  for(int i=0;i<Kpodv;i++) SLH->Delete(SLH->Count-1);
  if(!*NamesInclude&&!*NamesTAB&&!*NamesSpline&&!*NamesFurie){
//   fwprintf(HTMFILE,
//	L"<h4 align=\"left\"><font color=\"#009900\">"
//	L"%d. Избранные графики, интерполяции и таблицы:</font></h4>\n",PUNKT++);
   swprintf(Shtm,
	L"<h4 align=\"left\"><font color=\"#009900\">"
	L"%d. Избранные графики, интерполяции и таблицы:</font></h4>",PUNKT++);
   SLH->Add(Shtm);
  }
  TakeMagazine(NamesSpline,Name);
  if(wcschr(Name,'(')) swprintf(Head,Name);
  else swprintf(Head,L"%s(%s)",Name,::L->Time->Name);
//  fwprintf(HTMFILE,
//   L"<h5 align=\"left\"><font color=\"#000099\"><b>"
//   L"Сплайн-интерполяция:</b></font></h5>\n"
//   L"<font face=\"Courier New, Courier, mono\" size=\"-1\">");
  SLH->Add(L"<h5 align=\"left\"><font color=\"#000099\"><b>"
   L"Сплайн-интерполяция:</b></font></h5>");
  SLH->Add(L"<font face=\"Courier New, Courier, mono\" size=\"-1\">");
  for(i=0,m=Names;i<n;i++,m=m->Sled){
//   fwprintf(HTMFILE,L"%s=spln(%s_arr,%s);<br>\n",
//	m->S,m->S,::L->Time->Name);
   swprintf(Shtm,L"%s=spln(%s_arr,%s);<br>\n",
	m->S,m->S,::L->Time->Name);
   SLH->Add(Shtm);
   if(Kinst>1) swprintf(Inf,L"%s\\%s№%d.arr",Dir,m->S,Kinst);
   else swprintf(Inf,L"%s\\%s.arr",Dir,m->S);
   if(CountTabl>=100){//массив пишем в другой файл
//	fwprintf(HTMFILE,L"%s_arr[\"%s\"];<br>\n",m->S,Inf);
	swprintf(Shtm,L"%s_arr[\"%s\"];<br>\n",m->S,Inf);
	SLH->Add(Shtm);
   }
   else{//массив пишем туда же
//	fwprintf(HTMFILE,L"%s_arr[%d]=<br>\n",m->S,2*CountTabl);
	swprintf(Shtm,L"%s_arr[%d]=<br>\n",m->S,2*CountTabl);
	SLH->Add(Shtm);
	for(j=0,l=0;j<CountTabl-1;j++){
	 if(l+L[n]+L[i]+2>Wmax){
	  l=0;
//	  fputws(L"<br>\n",HTMFILE);
	  SLH->Add(L"<br>");
	 }
	 l+=swprintf(Inf,Format[n],X[j])+1;
	 for(s=Inf;*s;s++)
	  if(*s==' ')
//	   fputws(L"&nbsp;",HTMFILE);
	   shtm+=swprintf(shtm,L"&nbsp;");
	  else
//	   fputwc(*s,HTMFILE);
	   shtm+=swprintf(shtm,L"%s",*s);
//	 fputwc(',',HTMFILE);
	 shtm+=swprintf(shtm,L",");
	 l+=swprintf(Inf,Format[i],Ys[j][i])+1;
	 for(s=Inf;*s;s++)
	  if(*s==' ')
//	   fputws(L"&nbsp;",HTMFILE);
	   shtm+=swprintf(shtm,L"&nbsp;");
	  else
//	   fputwc(*s,HTMFILE);
	   shtm+=swprintf(shtm,L"%s",*s);
//	 fputwc(',',HTMFILE);
	 shtm+=swprintf(shtm,L",");
	}
	if(l+L[n]+L[i]+1>Wmax){
//	 l=0; fputws(L"<br>\n",HTMFILE);
	 l=0; shtm+=swprintf(shtm,L"<br>");
	 SLH->Add(Shtm); *Shtm='\0'; shtm=Shtm;
	}
	l+=swprintf(Inf,Format[n],X[j])+1;
	for(s=Inf;*s;s++)
	 if(*s==' ')
//	   fputws(L"&nbsp;",HTMFILE);
	  shtm+=swprintf(shtm,L"&nbsp;");
	 else
//	   fputwc(*s,HTMFILE);
	  shtm+=swprintf(shtm,L"%s",*s);
//	 fputwc(',',HTMFILE);
	shtm+=swprintf(shtm,L",");
	l+=swprintf(Inf,Format[i],Ys[j][i]);
	for(s=Inf;*s;s++)
	 if(*s==' ')
//	   fputws(L"&nbsp;",HTMFILE);
	  shtm+=swprintf(shtm,L"&nbsp;");
	 else
//	   fputwc(*s,HTMFILE);
	  shtm+=swprintf(shtm,L"%s",*s);
//	fputws(L";<br>\n",HTMFILE);
	shtm+=swprintf(shtm,L";<br>");
	SLH->Add(Shtm); *Shtm='\0'; shtm=Shtm;
  }}
//  fputws(L"</font>\n",HTMFILE);
  SLH->Add(L"</font>");
//   fwprintf(HTMFILE,
//	L"<hr><font face=\"Georgia, Times New Roman, Times, serif\""
//	L" color=\"#326464\">&copy; %s, %d<br></font>\n",Autor,NYear+1900);
//   fwprintf(HTMFILE,L"</body>\n</html>\n");
//  fseek(HTMFILE,0,2); fclose(HTMFILE);
  for(int i=0;i<Kpodv;i++) SLH->Add(SLP->Strings[i]);
  SLH->SaveToFile(HtmFile);
 }
 if(F){
  wchar_t CurDir[256]; _wgetcwd(CurDir,255);
  if(wcschr(Name,'(')) swprintf(Head,Name);
  else swprintf(Head,L"%s(%s)",Name,::L->Time->Name);
  for(i=0,m=Names;i<n;i++,m=m->Sled){
   swprintf(Inf,L"%s=spln(%s_arr,%s);\n",m->S,m->S,::L->Time->Name);
   Fputs(Inf,F);
   Ft=F;
   if(Kinst) swprintf(Inf,L"%s\\%s№%d.arr",Dir,m->S,Kinst);
   else swprintf(Inf,L"%s\\%s.arr",Dir,m->S);
   _wmkdir(Dir); _wchdir(CurDir);
   if((CountTabl<100)||!(F=_wfopen(Inf,L"wb"))){
	F=Ft; fwprintf(F,L"%s_arr[%d]=\n",m->S,2*CountTabl); //массив пишем туда же
	for(j=0,l=0;j<CountTabl-1;j++){
	 if(l+L[n]+L[i]+2>Wmax){ l=0; fputws(L"\n",F); }
	 l+=swprintf(Inf,Format[n],X[j])+1;
	 for(s=Inf;*s;s++)
	  if(*s==' ') fputws(L" ",F); else fputwc(*s,F);
	 fputwc(',',F);
	 l+=swprintf(Inf,Format[i],Ys[j][i])+1;
	 for(s=Inf;*s;s++)
	  if(*s==' ') fputws(L" ",F); else fputwc(*s,F);
	 fputwc(',',F);
	}
	if(l+L[n]+L[i]+1>Wmax){ l=0; fputws(L"\n",F); }
	l+=swprintf(Inf,Format[n],X[j])+1;
	for(s=Inf;*s;s++)
	 if(*s==' ') fputws(L" ",F); else fputwc(*s,F);
	fputwc(',',F);
	l+=swprintf(Inf,Format[i],Ys[j][i]);
	for(s=Inf;*s;s++)
	 if(*s==' ') fputws(L" ",F); else fputwc(*s,F);
	fputws(L";\n",F);
	if(F!=Ft){ fclose(F); F=Ft; }
   }
   else{ int K; wchar_t Buf[256];
	swprintf(Buf,L"%s_arr[\"%s\"];\n",m->S,Inf);//массив пишем в другой файл
	Fputs(Buf,Ft);
	K=2*CountTabl;
	fwrite(&K,SzI,1,F);//сначала пишем, размер массива, затем его самого
	for(j=0;j<CountTabl-1;j++){
	 fwrite(X+j,SzD,1,F); fwrite(&(Ys[j][i]),SzD,1,F);
	}
	fwrite(X+j,SzD,1,F); fwrite(&(Ys[j][i]),SzD,1,F);
	if(F!=Ft){ fclose(F); F=Ft; }
  }}
  fclose(F);
 }
 swprintf(Inf,L"сохранен в файле\n\"%s\".\n"
  L"Этот файл можно просмотреть и отредактировать\n"
  L"любым редактором, сплайн вставить в КИДИМ-файл для расчетов и т.д.",
  File);
 Application->MessageBox(Inf,L"Сплайн записан в отчет и",MB_OK);
 free(P); free(D); free(C); free(L);
 for(i=0;i<CountTabl;i++) free(Ys[i]); free(Ys); free(X);
 for(i=0;i<=n;i++) free(Format[i]); free(Format);
 DelAllMagazine(&Names);
}
//---------------------------------------------------------------------------
void SplGdrfToReport(wchar_t *Name,int CountTabl,double Tn,
  double Tk,void (*Funcs)(double,Magazine *,double *),
  Magazine **NamesInclude,Magazine **NamesTAB,
  Magazine **NamesSpline,Magazine **NamesFurie,TCGauge *CGauge){
//построение интерполяционных формул для графика функции Name=y(x)
//CountTabl - дискретность функций, Tn,Tk - диапазон изменения аргумента t,
//Funcs(double t,Magazine *M,double *Z) - процедура расчета
//функций x(t) и y(t); NamesInclude,... - магазин уже
//включенных в отчет формул;
 Magazine *Names=NULL,*Files=NULL; FILE *F,*Ft;
 wchar_t **Format,*s=Name,*s0,*t,*e,Val[256],Head[256],File[256],*LastFile,Dir[256];
 int i,j,k,*P,*D,*C,*L,p,d,c,l,N,W=0,Wmax=96,K=0,
  Kinst=KolInst(TextFromFile(WORKDIR,L"EditKiDyM",&SLK,111))+
   KolInst(TextFromFile(WORKDIR,L"EditKiDyM",&SLK,159));
 double *x,*y,*X,Step=(Tk-Tn)/(CountTabl-1),**Ys;
 X=(double *)calloc(CountTabl,SzD);
 Ys=(double **)calloc(CountTabl,SzV);
 for(int i=0;i<CountTabl;i++) Ys[i]=(double *)calloc(2,SzD);
 s0=wcschr(s=Name,'('); *s0++='\0'; TakeMagazine(&Names,s); s=s0;
 if(s0=wcschr(s,')')){
  *s0='\0'; Names->Sled=(Magazine *)calloc(1,sizeof(Magazine));
  Names->Sled->S=wcscpy((wchar_t *)calloc(wcslen(s)+1,SzC),s);
 }
 if(CGauge){ CGauge->MaxValue=CountTabl; CGauge->Progress=0; }
 if(CGauge) CGauge->Progress++;
 x=X; *x=Tn;
 Funcs(*x++,Names,Ys[0]);
 if(CGauge) CGauge->Progress++;
 *x=Tn+Step;
 Funcs(*x++,Names,Ys[1]);
 if(Ys[1][1]>Ys[0][1]) K=1; else K=-1;
 for(i=2;i<CountTabl;i++){
  if(CGauge) CGauge->Progress++; *x=Tn+Step*i;
  Funcs(*x,Names,Ys[i]);
  if((K*(Ys[i][1]-Ys[i-1][1]))<0){
   Application->MessageBox(L"СПЛАЙН НЕ МОЖЕТ БЫТЬ ПОСТРОЕН!",
	L"Аргумент сплайна должен быть монотонно\n"
	L"убывающей или растущей последовательностью",MB_ICONERROR|MB_OK);
   free(X); for(i=0;i<CountTabl;i++) free(Ys[i]); free(Ys); i=0;
   DelAllMagazine(&Names);
   break;
 }}
 if(CGauge) CGauge->Progress=0;
 if(!i) return;
 if(K<1){//перевернем массивы
  double D;
  for(i=0;i<CountTabl/2;i++){
   D=Ys[i][0]; Ys[i][0]=Ys[CountTabl-i-1][0]; Ys[CountTabl-i-1][0]=D;
   D=Ys[i][1]; Ys[i][1]=Ys[CountTabl-i-1][1]; Ys[CountTabl-i-1][1]=D;
 }}
 wcscpy(Dir,InpFile); if(s0=wcsrchr(Dir,'.')) *s0='\0';
 swprintf(File,L"%s_S.txt",Dir);
 if(!_waccess(File,0)){
  LastFile=TakeMagazine(&Files,File)->S;
  swprintf(File,L"%s_S№%d.txt",Dir,i=1);
  while(!_waccess(File,0)){
   LastFile=TakeMagazine(&Files,File)->S;
   swprintf(File,L"%s_S№%d.txt",Dir,++i);
  }
  if((i=KolElem(Files))>1)
   swprintf(Inf,L"Уничтожить их и писать результат в файл:\n<%s> - <Да>\n"
	L"Сохранить их все и писать в новый файл:\n<%s> - <Нет>\n"
	L"Продолжать писать в последний из них:\n<%s> - <Отмена>",
	Files->S,File,LastFile);
  else
   swprintf(Inf,L"Заменить его и писать результат в файл:\n<%s> - <Да>\n"
	L"Сохранить его и писать в новый файл:\n<%s> - <Нет>\n"
	L"Продолжать писать в этот файл:\n<%s> - <Отмена>",Files->S,File,LastFile);
  switch(Application->MessageBox(Inf,
		 i>1?L"Обнаружены файлы со сплайнами":L"Обнаружен файл со сплайном",
		 MB_YESNOCANCEL|MB_ICONQUESTION)){
   case ID_YES:
	for(Magazine *m=Files;m;m=m->Sled) if(!_waccess(m->S,0)) _wunlink(m->S);
	F=_wfopen(Files->S,L"w"); wcscpy(File,Files->S); break;
   case ID_NO: F=_wfopen(File,L"w"); break;
   case ID_CANCEL: F=_wfopen(LastFile,L"a"); wcscpy(File,LastFile); break;
 }}
 else F=_wfopen(File,L"w");
 DelAllMagazine(&Files);
 Format=(wchar_t **)calloc(2,SzV);
 for(i=0;i<2;i++) Format[i]=(wchar_t *)calloc(256,SzC);
 P=(int *)calloc(2,SzI); D=(int *)calloc(2,SzI);
 C=(int *)calloc(2,SzI); L=(int *)calloc(2,SzI);
 for(i=0,x=X;i<CountTabl;i++){
  *x=Tn+Step*i; y=Ys[i];
  for(j=0;j<2;j++){
   l=swprintf(Val,L"%g",*y++);
   if(!(e=wcschr(Val,'e'))&&!(e=wcschr(Val,'E'))) p=0;
   else{ p=wcslen(e+2); if(p>P[j]) P[j]=p; *e='\0'; }
   if(!(t=wcschr(Val,'.'))) d=0; else{ d=wcslen(t+1); if(d>D[j]) D[j]=d; }
   c=l; if(d) c-=d+1; if(p) c-=p+2; if(c>C[j]) C[j]=c;
   if(P[j])
	if(D[j]) swprintf(Format[j],L"%%%d.%de",L[j]=max(L[j],D[j]+P[j]+5),D[j]);
	else     swprintf(Format[j],L"%%%d.%de",L[j]=max(L[j],C[j]+P[j]+2),C[j]);
   else
	if(D[j]) swprintf(Format[j],L"%%%d.%df",L[j]=max(L[j],C[j]+D[j]+2),D[j]);
	else     swprintf(Format[j], L"%%%d.0f",L[j]=max(L[j],C[j]));
   W+=L[j]+1;
 }}
 N=Wmax/W; if(!N) N=1;
// if(HTMFILE=_wfopen(HtmFile,L"r+")){
 if(SLH){
//  if(!fseek(HTMFILE,-Lpodv,2)){
  for(int i=0;i<Kpodv;i++) SLH->Delete(SLH->Count-1);
  if(!*NamesInclude&&!*NamesTAB&&!*NamesSpline&&!*NamesFurie){
//   fwprintf(HTMFILE,
//	L"<h4 align=\"left\"><font color=\"#009900\">"
//	L"%d. Избранные графики, интерполяции и таблицы:</font></h4>\n",PUNKT++);
   swprintf(Shtm,
	L"<h4 align=\"left\"><font color=\"#009900\">"
	L"%d. Избранные графики, интерполяции и таблицы:"
	L"</font></h4>\n",PUNKT++);
   SLH->Add(Shtm);
  }
  TakeMagazine(NamesSpline,Name);
//  fwprintf(HTMFILE,
//   L"<h5 align=\"left\"><font color=\"#000099\"><b>"
//   L"Сплайн-интерполяция:</b></font></h5>\n"
//   L"<font face=\"Courier New, Courier, mono\" size=\"-1\">");
  SLH->Add(L"<h5 align=\"left\"><font color=\"#000099\"><b>"
   L"Сплайн-интерполяция:</b></font></h5>");
  SLH->Add(L"<font face=\"Courier New, Courier, mono\" size=\"-1\">");
//  fwprintf(HTMFILE,L"%s=spln(%s_arr,%s);<br>\n",
//   Names->S,Names->S,Names->Sled->S);
  swprintf(Shtm,L"%s=spln(%s_arr,%s);<br>\n",
   Names->S,Names->S,Names->Sled->S);
  SLH->Add(Shtm);
  if(Kinst>1) swprintf(Inf,L"%s\\%s№%d.arr",Dir,Names->S,Kinst);
  else swprintf(Inf,L"%s\\%s.arr",Dir,Names->S);
  if(CountTabl>=100){//массив пишем в другой файл
//   fwprintf(HTMFILE,L"%s_arr[%d]=%s;<br>\n",Names->S,2*CountTabl,Inf);
   swprintf(Shtm,L"%s_arr[%d]=%s;<br>\n",
	Names->S,2*CountTabl,Inf);
   SLH->Add(Shtm);
  }
  else{//массив пишем туда же
//   fwprintf(HTMFILE,L"%s_arr[%d]=<br>\n",Names->S,2*CountTabl);
   swprintf(Shtm,L"%s_arr[%d]=<br>\n",Names->S,2*CountTabl);
   SLH->Add(Shtm);
   for(j=0,l=0;j<CountTabl-1;j++){
	if(l+L[0]+L[1]+2>Wmax){
//	 l=0; fputws(L"<br>\n",HTMFILE);
	 l=0; SLH->Add(L"<br>");
	}
	l+=swprintf(Inf,Format[1],Ys[j][1])+1;
	for(s=Inf;*s;s++)
	 if(*s==' ')
//	   fputws(L"&nbsp;",HTMFILE);
	  shtm+=swprintf(shtm,L"&nbsp;");
	 else
//	   fputwc(*s,HTMFILE);
	  shtm+=swprintf(shtm,L"%s",*s);
//	 fputwc(',',HTMFILE);
	shtm+=swprintf(shtm,L",");
	l+=swprintf(Inf,Format[0],Ys[j][0])+1;
	for(s=Inf;*s;s++)
	 if(*s==' ')
//	   fputws(L"&nbsp;",HTMFILE);
	  shtm+=swprintf(shtm,L"&nbsp;");
	 else
//	   fputwc(*s,HTMFILE);
	  shtm+=swprintf(shtm,L"%s",*s);
//	 fputwc(',',HTMFILE);
	shtm+=swprintf(shtm,L",");
   }
   if(l+L[0]+L[1]+1>Wmax){
//	 l=0; fputws(L"<br>\n",HTMFILE);
	l=0; SLH->Add(L"<br>");
   }
   l+=swprintf(Inf,Format[0],Ys[j][1])+1;
   for(s=Inf;*s;s++)
	if(*s==' ')
//	   fputws(L"&nbsp;",HTMFILE);
	 shtm+=swprintf(shtm,L"&nbsp;");
	else
//	   fputwc(*s,HTMFILE);
	 shtm+=swprintf(shtm,L"%s",*s);
//	 fputwc(',',HTMFILE);
   shtm+=swprintf(shtm,L",");
   l+=swprintf(Inf,Format[1],Ys[j][0]);
   for(s=Inf;*s;s++)
	if(*s==' ')
//	   fputws(L"&nbsp;",HTMFILE);
	 shtm+=swprintf(shtm,L"&nbsp;");
	else
//	   fputwc(*s,HTMFILE);
	 shtm+=swprintf(shtm,L"%s",*s);
//	fputws(L";<br>\n",HTMFILE);
   shtm+=swprintf(shtm,L";<br>\n");
  }
//  fputws(L"</font>\n",HTMFILE);
  SLH->Add(L"</font>");
//  fwprintf(HTMFILE,
//   L"<hr><font face=\"Georgia, Times New Roman, Times, serif\""
//   L" color=\"#326464\">&copy; %s, %d<br></font>\n",Autor,NYear+1900);
//  fwprintf(HTMFILE,L"</body>\n</html>\n");
//  fseek(HTMFILE,0,2); fclose(HTMFILE);
  for(int i=0;i<Kpodv;i++) SLH->Add(SLP->Strings[i]);
  SLH->SaveToFile(HtmFile);
 }
 if(F){
  wchar_t CurDir[256]; _wgetcwd(CurDir,255);
  wcscpy(Head,Name);
  swprintf(Inf,L"%s=spln(%s_arr,%s);\n",Names->S,Names->S,Names->Sled->S);
  Fputs(Inf,F);
  Ft=F;
  if(Kinst>1) swprintf(Inf,L"%s\\%s_%s№%d.arr",Dir,Names->S,Names->Sled->S,Kinst);
  else swprintf(Inf,L"%s\\%s_%s.arr",Dir,Names->S,Names->Sled->S);
  _wmkdir(Dir); _wchdir(CurDir); F=_wfopen(Inf,L"wb");
  if((CountTabl<100)||!F){
   F=Ft; fwprintf(F,L"%s_arr[%d]=\n",Names->S,2*CountTabl);//массив пишем туда же
   for(j=0,l=0;j<CountTabl-1;j++){
	if(l+L[0]+L[1]+2>Wmax){ l=0; fputws(L"\n",F); }
	l+=swprintf(Inf,Format[1],Ys[j][1])+1;
	for(s=Inf;*s;s++)
	 if(*s==' ') fputws(L" ",F); else fputwc(*s,F);
	fputwc(',',F);
	l+=swprintf(Inf,Format[0],Ys[j][0])+1;
	for(s=Inf;*s;s++)
	 if(*s==' ') fputws(L" ",F); else fputwc(*s,F);
	fputwc(',',F);
   }
   if(l+L[0]+L[1]+1>Wmax){ l=0; fputws(L"\n",F); }
   l+=swprintf(Inf,Format[1],Ys[j][1])+1;
   for(s=Inf;*s;s++)
	if(*s==' ') fputws(L" ",F); else fputwc(*s,F);
   fputwc(',',F);
   l+=swprintf(Inf,Format[0],Ys[j][0]);
   for(s=Inf;*s;s++)
	if(*s==' ') fputws(L" ",F); else fputwc(*s,F);
   fputws(L";\n",F);
   if(F!=Ft){ fclose(F); F=Ft; }
  }
  else{ int K=2*CountTabl; wchar_t Buf[256];
   swprintf(Buf,L"%s_arr[%s];\n",Names->S,Inf);//массив пишем в другой файл
   Fputs(Buf,Ft);
   fwrite(&K,SzI,1,F);//сначала пишем, размер массива, затем его самого
   for(j=0;j<CountTabl-1;j++){
	fwrite(Ys[j]+1,SzD,1,F); fwrite(Ys[j],SzD,1,F);
   }
   fwrite(Ys[j]+1,SzD,1,F); fwrite(Ys[j],SzD,1,F);
   if(F!=Ft){ fclose(F); F=Ft; }
  }
  fclose(F);
 }
 swprintf(Inf,L"сохранен в файле\n\"%s\".\n"
  L"Этот файл можно просмотреть и отредактировать\n"
  L"любым редактором, сплайн вставить в КИДИМ-файл для расчетов и т.д.",
  File);
 Application->MessageBox(Inf,L"Сплайн записан в отчет и",MB_OK);
 free(P); free(D); free(C); free(L);
 for(i=0;i<CountTabl;i++) free(Ys[i]); free(Ys); free(X);
 for(i=0;i<2;i++) free(Format[i]); free(Format);
 DelAllMagazine(&Names);
}
//---------------------------------------------------------------------------
void Furie(double **FF,int n,int Kt,int Kg,double **Ac,double **As,double *U){
//FF - массив [0:n-1][0:Kt-1] (Kt дискретных значений n переменных),
//Kg - число гармоник разложения ряда фурье,
//Ac,As - массивы [0:n-1][0:Kg] (0,1,...,Kg - е амплитуды гармоник для каждой
//из n переменных)
//U[0:Kt-1] - рабочий массив размера Kt
 double Max=FF[0][0],Min=Max;
 for(int j=1;j<Kt;j++){
  if(FF[0][j]>Max) Max=FF[0][j]; if(FF[0][j]<Min) Min=FF[0][j];
 }
 Max-=Min;
 if((fabs(FF[0][0]-FF[0][Kt-1])<1e-5*Max)&&
  fabs(FF[0][Kt-1]-FF[0][0])<1e-5*fabs(FF[0][1]-FF[0][Kt-2]))
  Kt--;//периодическая функция!*/
 for(int i=0;i<n;i++){
  double *ac=Ac[i],*as=As[i],*F=FF[i]; U[1]=F[Kt-1]; U[0]=0.0;
  for(int k=0;k<=Kg;k++,ac++,as++){
   double w=2*cos(M_PI*2*k/Kt);
   for(int j=2;j<Kt;j++) U[j]=w*U[j-1]-U[j-2]+F[Kt-j];
	*ac=(U[Kt-1]*w/2-U[Kt-2]+(*F))*2/Kt;
	if(k) *as=(U[Kt-1]*sin(M_PI*2*k/Kt))*2/Kt;
	else{ *ac/=2; *as=0; }
}}}
//---------------------------------------------------------------------------
void FurieToReport(wchar_t *Name,int CountTabl,double Tn,
  double Tk,bool Godograf,
  void (*Funcs)(double,Magazine *,double *),
  Magazine **NamesInclude,Magazine **NamesTAB,
  Magazine **NamesSpline,Magazine **NamesFurie,TCGauge *CGauge){
//построение интерполяционных формул на основе ряда Фурье для графика функции,
//Name=x|y(x)|z(x,y)|Заголовок(x,y,z,...)
//в первом и четвертом случае Godograf=false,  во втором и третьем - true
//CountTabl - дискретность функций, Tn,Tk - диапазон изменения аргумента t,
//Funcs(double t,Magazine *M,double *Z) - процедура расчета массива
//функций Z(t) переменных M; NamesInclude - магазин уже
//включенных в отчет формул; 
 Magazine *Names=NULL,*m,*Files=NULL; FILE *F; bool Period=false,CONT=false;
 wchar_t **Format,*s=Name,*s0,*t,*e,Val[256],Head[256],Form1[80],Form2[80],tt[3],
  Form3[80],File[256],*LastFile,AC[256],AS[256];
 if(CountTabl<10){
  Application->MessageBox(L"Задайте в Настройке дискретность таблиц целым числом,\n"
   L"большим, чем у Вас сейчас (%d), например, больше 10!",L"Ошибка в настройках",
   MB_OK|MB_ICONERROR);
  return;
 }
 int i,j,k,*P,*D,*C,*L,p,d,c,l,N,n,W,Wmax=96,Kg=(CountTabl-1)/2;
 double *x,*y,*X=(double *)calloc(CountTabl,SzD),**Ac,**As,**Fs,
  **Ys=(double **)calloc(CountTabl,SzV),Step=(Tk-Tn)/(CountTabl-1),
  *U=(double *)calloc(CountTabl,SzD),A,Psi;
 if(s0=wcsrchr(InpFile,'.')) *s0='\0';
 swprintf(File,L"%s_F.txt",InpFile);
 if(!_waccess(File,0)){
  LastFile=TakeMagazine(&Files,File)->S;
  swprintf(File,L"%s_F№%d.txt",InpFile,i=1);
  while(!_waccess(File,0)){
   LastFile=TakeMagazine(&Files,File)->S;
   swprintf(File,L"%s_F№%d.txt",InpFile,++i);
  }
  if((i=KolElem(Files))>1)
   swprintf(Inf,L"Уничтожить их и писать результат в файл:\n<%s> - <Да>\n"
    L"Сохранить их все и писать в новый файл:\n<%s> - <Нет>\n"
    L"Продолжать писать в последний из них:\n<%s> - <Отмена>",
    Files->S,File,LastFile);
  else
   swprintf(Inf,L"Заменить его и писать результат в файл:\n<%s> - <Да>\n"
    L"Сохранить его и писать в новый файл:\n<%s> - <Нет>\n"
    L"Продолжать писать в этот файл:\n<%s> - <Отмена>",Files->S,File,LastFile);
  switch(Application->MessageBox(Inf,i>1?L"Обнаружены файлы с Фурье-разложениями":
   L"Обнаружен файл с Фурье-разложением",MB_YESNOCANCEL|MB_ICONQUESTION)){
   case ID_YES:
    for(m=Files;m;m=m->Sled) if(!_waccess(m->S,0)) _wunlink(m->S);
    F=_wfopen(Files->S,L"w"); wcscpy(File,Files->S); break;
   case ID_NO: F=_wfopen(File,L"w"); break;
   case ID_CANCEL: F=_wfopen(LastFile,L"a"); wcscpy(File,LastFile); CONT=true; break;
 }}
 else F=_wfopen(File,L"w");
 DelAllMagazine(&Files);
 if(s0) *s0='.';
 if(!(s0=wcschr(s,'('))) TakeMagazine(&Names,s);
 else{
  if(Godograf){
   *s0++='\0'; m=TakeMagazine(&Names,s); s=s0;
   if(s0=wcschr(s,',')){
    *s0++='\0'; m->Sled=(Magazine *)calloc(1,sizeof(Magazine));
    m->Sled->S=wcscpy((wchar_t *)calloc(wcslen(s)+1,SzC),s); s=s0; m=m->Sled;
   }
   if(s0=wcschr(s,')')){
    *s0='\0'; m->Sled=(Magazine *)calloc(1,sizeof(Magazine));
    m->Sled->S=wcscpy((wchar_t *)calloc(wcslen(s)+1,SzC),s);
  }}
  else{
   for(s=s0+1;*s0&&((s0=wcschr(s,','))||(s0=wcschr(s,')')));s=s0){
    *s0++='\0';
    if(Names){
     m->Sled=(Magazine *)calloc(1,sizeof(Magazine));
     m->Sled->S=wcscpy((wchar_t *)calloc(wcslen(s)+1,SzC),s); m=m->Sled;
    }
    else m=TakeMagazine(&Names,s);
 }}}
 n=KolElem(Names);
 for(i=0;i<CountTabl;i++) Ys[i]=(double *)calloc(n,SzD);
 Fs=(double **)calloc(n,SzV);
 Ac=(double **)calloc(n,SzV); As=(double **)calloc(n,SzV);
 for(i=0;i<n;i++){
  Fs[i]=(double *)calloc(CountTabl,SzD);
  Ac[i]=(double *)calloc(Kg+1,SzD); As[i]=(double *)calloc(Kg+1,SzD);
 }
 Format=(wchar_t **)calloc(n+1,SzV);
 for(i=0;i<=n;i++) Format[i]=(wchar_t *)calloc(256,SzC);
 P=(int *)calloc(n+1,SzI); D=(int *)calloc(n+1,SzI);
 C=(int *)calloc(n+1,SzI); L=(int *)calloc(n+1,SzI);
 if(CGauge){ CGauge->MaxValue=CountTabl; CGauge->Progress=0; }
 for(i=0,x=X;i<CountTabl;i++){
  if(CGauge) CGauge->Progress++;
  *x=Tn+Step*i;
  Funcs(*x,Names,y=Ys[i]);
  l=swprintf(Val,L"%g",*x++);
  if(!(e=wcschr(Val,'e'))&&!(e=wcschr(Val,'E'))) p=0;
  else{ p=wcslen(e+2); if(p>P[n]) P[n]=p; *e='\0'; }
  if(!(t=wcschr(Val,'.'))) d=0; else{ d=wcslen(t+1); if(d>D[n]) D[n]=d; }
  c=l; if(d) c-=d+1; if(p) c-=p+2; if(c>C[n]) C[n]=c;
  if(P[n])
   if(D[n]) swprintf(Format[n],L"%%%d.%de",L[n]=max(L[n],D[n]+P[n]+5),D[n]);
   else     swprintf(Format[n],L"%%%d.%de",L[n]=max(L[n],C[n]+P[n]+2),C[n]);
  else
   if(D[n]) swprintf(Format[n],L"%%%d.%df",L[n]=max(L[n],C[n]+D[n]+2),D[n]);
   else     swprintf(Format[n],L"%%%d.0f",L[n]=max(L[n],C[n]));
  if(L[n]<11) L[n]=11; W=L[n]+2;
  for(j=0;j<n;j++){
   l=swprintf(Val,L"%g",*y++);
   if(!(e=wcschr(Val,'e'))&&!(e=wcschr(Val,'E'))) p=0;
   else{ p=wcslen(e+2); if(p>P[j]) P[j]=p; *e='\0'; }
   if(!(t=wcschr(Val,'.'))) d=0; else{ d=wcslen(t+1); if(d>D[j]) D[j]=d; }
   c=l; if(d) c-=d+1; if(p) c-=p+2; if(c>C[j]) C[j]=c;
   W+=L[j]+1;
 }}
 for(j=0;j<n;j++){
  if(P[j])
   if(D[j]) swprintf(Format[j],L"%%%d.%de",L[j]=max(L[j],D[j]+P[j]+5),D[j])-1;
   else     swprintf(Format[j],L"%%%d.%de",L[j]=max(L[n],C[j]+P[j]+2),C[j])-1;
  else
   if(D[j]) swprintf(Format[j],L"%%%d.%df",L[j]=max(L[j],C[j]+D[j]+2),D[j])-1;
   else     swprintf(Format[j], L"%%%d.0f",L[j]=max(L[j],C[j]))-1;
 }
 if(CGauge) CGauge->Progress=0;
 for(i=0;i<CountTabl;){ for(j=0;j<n;){ Fs[j][i]=Ys[i][j]; j++; } i++; }
 if(Application->MessageBox(
  L"Если считаете, что раскладываемая функция в ряд Фурье периодическая,\n"
  L"то есть ее значения и производные на краях графика одинаковы,\n"
  L"жмите - <Да>,\nиначе - <Нет>",L"Для повышения точности разложения, ответьте",
  MB_YESNO|MB_ICONQUESTION)==ID_YES)
   Period=true;
 Furie(Fs,n,Period?CountTabl-1:CountTabl,Kg,Ac,As,U);
// Furie(Fs,n,CountTabl,Kg,Ac,As,U);
 N=Wmax/W; if(!N) N=1;
// if(HTMFILE=_wfopen(HtmFile,L"r+")){
//  if(!fseek(HTMFILE,-Lpodv-2,2)){
 if(SLH){
  for(int i=0;i<Kpodv;i++) SLH->Delete(SLH->Count-1);
  if(!*NamesInclude&&!*NamesTAB&&!*NamesFurie&&!*NamesSpline){
//   fwprintf(HTMFILE,L"<h4 align=\"left\"><font color=\"#009900\">"
//	 fwprintf(HTMFILE,L"%d. Избранные графики, интерполяции и таблицы:</font></h4>\n",PUNKT++);
   shtm+=swprintf(shtm,L"<h4 align=\"left\"><font color=\"#009900\">");
//   fwprintf(HTMFILE,L"%d. Избранные графики, интерполяции и таблицы:</font></h4>\n",PUNKT++);
   shtm+=swprintf(shtm,L"%d. Избранные графики, интерполяции и таблицы:</font></h4>",PUNKT++);
   SLH->Add(Shtm); *Shtm='\0'; shtm=Shtm;
  }
  TakeMagazine(NamesFurie,Name);
  if(wcschr(Name,'(')) swprintf(Head,Name);
  else swprintf(Head,L"%s(%s)",Name,::L->Time->Name);
  if(Tn!=0.0) swprintf(tt,L"%s~",::L->Time->Name);
  else wcscpy(tt,::L->Time->Name);
  if(!CONT){
//   fwprintf(HTMFILE,
//	L"<h5 align=\"left\"><font color=\"#000099\"><b>"
//	L"Разложение в ряд Фурье по косинусным и синусным амплитудам:</b>\n<br></font></h5>");
   SLH->Add(L"<h5 align=\"left\"><font color=\"#000099\"><b>"
	L"Разложение в ряд Фурье по косинусным и синусным амплитудам:</b>");
   SLH->Add(L"<br></font></h5>");
//   fwprintf(HTMFILE,L"w=%0.6f;",M_PI*2/(Tk-Tn));
//   fwprintf(HTMFILE,L"w=%0.6f;",M_PI*2/(Tk-Tn));
   shtm+=swprintf(shtm,L"w=%0.6f;",M_PI*2/(Tk-Tn));
   if(Tn!=0.0)
//	fwprintf(HTMFILE,L" %s=%s-%g;",tt,::L->Time->Name,Tn);
	shtm+=swprintf(shtm,L" %s=%s-%g;",tt,::L->Time->Name,Tn);
//   fputws(L"<br>\n",HTMFILE);
   shtm+=swprintf(shtm,L"<br>");
   SLH->Add(Shtm); *Shtm='\0'; shtm=Shtm;
  }
  else
   SLH->Add(L"<h5 align=\"left\"><font color=\"#000099\">"
	L"<b></b></font></h5>");
  for(i=0,m=Names;i<n;i++,m=m->Sled){
//   fwprintf(HTMFILE,L"<font face=\"Courier New, Courier, mono\" size=\"-1\">");
//   fwprintf(HTMFILE,L"<br>\n%s=L",m->S);
   SLH->Add(L"<font face=\"Courier New, Courier,"
	L" mono\" size=\"-1\">");
   SLH->Add(L"<br>"); SLH->Add(m->S);
   if(Kg<10){
	swprintf(Form1,L"%s*cos(  w*%s)%%c%s*sin(  w*%s)",Format[i],tt,Format[i],tt);
	swprintf(Form2,L"%s*cos(%%1d*w*%s)%%c%s*sin(%%1d*w*%s)",Format[i],tt,Format[i],tt);
	swprintf(Form3,L"%%c%s*cos(%%1d*w*%s)%%c%s*sin(%%1d*w*%s)",Format[i],tt,Format[i],tt);
   }
   else if(Kg<100){
	swprintf(Form1,L"%s*cos(   w*%s)%%c%s*sin(   w*%s)",Format[i],tt,Format[i],tt);
	swprintf(Form2,L"%s*cos(%%2d*w*%s)%%c%s*sin(%%2d*w*%s)",Format[i],tt,Format[i],tt);
	swprintf(Form3,L"%%c%s*cos(%%2d*w*%s)%%c%s*sin(%%2d*w*%s)",Format[i],tt,Format[i],tt);
   }
   else if(Kg<1000){
	swprintf(Form1,L"%s*cos(    w*%s)%%c%s*sin(    w*%s)",Format[i],tt,Format[i],tt);
	swprintf(Form2,  L"%s*cos(%%3d*w*%s)%%c%s*sin(%%3d*w*%s)",Format[i],tt,Format[i],tt);
	swprintf(Form3,L"%%c%s*cos(%%3d*w*%s)%%c%s*sin(%%3d*w*%s)",Format[i],tt,Format[i],tt);
   }
   else{
	swprintf(Form1,L"%s*cos(     w*%s)%%c%s*sin(     w*%s)",Format[i],tt,Format[i],tt);
	swprintf(Form2,  L"%s*cos(%%4d*w*%s)%%c%s*sin(%%4d*w*%s)",Format[i],tt,Format[i],tt);
	swprintf(Form3,L"%%c%s*cos(%%4d*w*%s)%%c%s*sin(%%4d*w*%s)",Format[i],tt,Format[i],tt);
   }
   W=l=wcslen(m->S)+1;
//   fwprintf(HTMFILE,L"%0.6f%c<br>\n",Ac[i][0],Ac[i][1]>=0.0?'+':'-');
   swprintf(Shtm,L"%0.6f%c<br>",Ac[i][0],Ac[i][1]>=0.0?'+':'-');
   SLH->Add(Shtm);
   for(j=0;j<l;j++)
  //	fputws(L"&nbsp;",HTMFILE);
	shtm+=swprintf(shtm,L"&nbsp;");
   W+=swprintf(Inf,Form1,fabs(Ac[i][1]),As[i][1]>=0.0?'+':'-',
	fabs(As[i][1]));
   for(s=Inf;*s;s++)
	if(*s==' ')
//	 fputws(L"&nbsp;",HTMFILE);
	 shtm+=swprintf(shtm,L"&nbsp;");
	else
//	 fputwc(*s,HTMFILE);
	 shtm+=swprintf(shtm,L"%c",*s);
   for(k=2;k<Kg;k++){
	swprintf(AC,Format[i],Ac[i][k]);
	swprintf(AS,Format[i],As[i][k]);
	if((_wtof(AC)==0.0)&&(_wtof(AS)==0.0)) continue;
	if(W+34>Wmax){ W=l;
//	 fwprintf(HTMFILE,L"%c<br>\n",Ac[i][k]>=0.0?'+':'-');
	 shtm+=swprintf(shtm,L"%c<br>",Ac[i][k]>=0.0?'+':'-');
	 SLH->Add(Shtm); *Shtm='\0'; shtm=Shtm;
	 for(j=0;j<l;j++)
//	  fputws(L"&nbsp;",HTMFILE);
	  shtm+=swprintf(shtm,L"&nbsp;");
	 swprintf(Inf,Form2,fabs(Ac[i][k]),k,As[i][k]>=0.0?'+':'-',fabs(As[i][k]),k);
	}
	else
	 swprintf(Inf,Form3,Ac[i][k]>=0.0?'+':'-',fabs(Ac[i][k]),k,
	  As[i][k]>=0.0?'+':'-',fabs(As[i][k]),k);
	W+=34;
	for(s=Inf;*s;s++)
	 if(*s==' ')
//	 fputws(L"&nbsp;",HTMFILE);
	  shtm+=swprintf(shtm,L"&nbsp;");
	 else
//	 fputwc(*s,HTMFILE);
	  shtm+=swprintf(shtm,L"%c",*s);
   }
   swprintf(AC,Format[i],Ac[i][k]); swprintf(AS,Format[i],As[i][k]);
   if((_wtof(AC)!=0.0)||(_wtof(AS)!=0.0)){
	if(W+34>Wmax){
//	 fwprintf(HTMFILE,L"%c<br>\n",Ac[i][k]>=0.0?'+':'-');
	 swprintf(Shtm,L"%c<br>\n",Ac[i][k]>=0.0?'+':'-');
	 SLH->Add(Shtm);
	 for(j=0;j<l;j++)
//	  fputws(L"&nbsp;",HTMFILE);
	  shtm+=swprintf(shtm,L"&nbsp;");
	 swprintf(Inf,Form2,fabs(Ac[i][k]),k,As[i][k]>=0.0?'+':'-',fabs(As[i][k]),k);
	}
	else
	 swprintf(Inf,Form3,Ac[i][k]>=0.0?'+':'-',fabs(Ac[i][k]),k,
	  As[i][k]>=0.0?'+':'-',fabs(As[i][k]),k);
	for(s=Inf;*s;s++)
	 if(*s==' ')
//	 fputws(L"&nbsp;",HTMFILE);
	  shtm+=swprintf(shtm,L"&nbsp;");
	 else
//	 fputwc(*s,HTMFILE);
	  shtm+=swprintf(shtm,L"%c",*s);
   }
//   fputws(L";</font>\n",HTMFILE);
   shtm+=swprintf(shtm,L";</font>");
   SLH->Add(Shtm); *Shtm='\0'; shtm=Shtm;
  }
//  fwprintf(HTMFILE,L"<font face=\"Courier New, Courier, mono\" size=\"-1\">");
  SLH->Add(L"<font face=\"Courier New, Courier, mono\" size=\"-1\">");
  for(i=0,m=Names;i<n;i++,m=m->Sled){
   if(Kg<10){
	swprintf(Form1,L"%s*sin(  w*%s%%c%s)",Format[i],tt,Format[i]);
	swprintf(Form2,L"%s*sin(%%1d*w*%s%%c%s)",Format[i],tt,Format[i]);
	swprintf(Form3,L"%s*sin(%%1d*w*%s%%c%s)",Format[i],tt,Format[i]);
   }
   else if(Kg<100){
	swprintf(Form1,L"%s*sin(   w*%s%%c%s)",Format[i],tt,Format[i]);
	swprintf(Form2,L"%s*sin(%%2d*w*%s%%c%s)",Format[i],tt,Format[i]);
	swprintf(Form3,L"%s*sin(%%2d*w*%s%%c%s)",Format[i],tt,Format[i]);
   }
   else if(Kg<1000){
	swprintf(Form1,L"%s*sin(    w*%s%%c%s)",Format[i],tt,Format[i]);
	swprintf(Form2,L"%s*sin(%%3d*w*%s%%c%s)",Format[i],tt,Format[i]);
	swprintf(Form3,L"%s*sin(%%3d*w*%s%%c%s)",Format[i],tt,Format[i]);
   }
   else{
	swprintf(Form1,L"%s*sin(     w*%s%%c%s)",Format[i],tt,Format[i]);
	swprintf(Form2,L"%s*sin(%%4d*w*%s%%c%s)",Format[i],tt,Format[i]);
	swprintf(Form3,L"%s*sin(%%4d*w*%s%%c%s)",Format[i],tt,Format[i]);
   }
//   fwprintf(HTMFILE,L"<br>\n%s=L",m->S);
   SLH->Add(L"<br>"); SLH->Add(m->S);
   W=l=wcslen(m->S)+1;
   shtm+=swprintf(shtm,L"%0.6f+<br>",Ac[i][0]);
   for(j=0;j<l;j++)
//	fputws(L"&nbsp;",HTMFILE);
	shtm+=swprintf(shtm,L"&nbsp;");
   A=sqrt(Ac[i][1]*Ac[i][1]+As[i][1]*As[i][1]);
   Psi=atan2(Ac[i][1],As[i][1]);
   W+=swprintf(Inf,Form1,A,Psi>=0.0?'+':'-',fabs(Psi));
   for(s=Inf;*s;s++)
	if(*s==' ')
//	 fputws(L"&nbsp;",HTMFILE);
	 shtm+=swprintf(shtm,L"&nbsp;");
	else
//	 fputwc(*s,HTMFILE);
	shtm+=swprintf(shtm,L"%c",*s);
	for(k=2;k<Kg;k++){
	A=sqrt(Ac[i][k]*Ac[i][k]+As[i][k]*As[i][k]);
	swprintf(Inf,Format[i],A);
	if(_wtof(Inf)==0.0) continue;
	Psi=atan2(Ac[i][k],As[i][k]);
	if(W+34>Wmax){ W=l;
//	 fwprintf(HTMFILE,L"+<br>\n");
	 shtm+=swprintf(shtm,L"+<br>");
	 for(j=0;j<l;j++)
//	  fputws(L"&nbsp;",HTMFILE);
	  shtm+=swprintf(shtm,L"&nbsp;");
	 swprintf(Inf,Form2,A,k,Psi>=0.0?'+':'-',fabs(Psi));
	}
	else{
	 shtm+=swprintf(shtm,L"+");
	 swprintf(Inf,Form3,A,k,Psi>=0.0?'+':'-',fabs(Psi));
	}
	W+=34;
	for(s=Inf;*s;s++)
	 if(*s==' ')
//	  fputws(L"&nbsp;",HTMFILE);
	  shtm+=swprintf(shtm,L"&nbsp;");
	 else
//	  fputwc(*s,HTMFILE);
	  shtm+=swprintf(shtm,L"%c",*s);
   }
   A=sqrt(Ac[i][k]*Ac[i][k]+As[i][k]*As[i][k]);
   swprintf(Inf,Format[i],A);
   if(_wtof(Inf)>0.0){
	Psi=atan2(Ac[i][k],As[i][k]);
	if(W+34>Wmax){
//	 fwprintf(HTMFILE,L"+<br>\n");
	 shtm+=swprintf(shtm,L"+<br>");
	 SLH->Add(Shtm); *Shtm='\0'; shtm=Shtm;
	 for(j=0;j<l;j++)
//	  fputws(L"&nbsp;",HTMFILE);
	  shtm+=swprintf(shtm,L"&nbsp;");
	 swprintf(Inf,Form2,A,k,Psi>=0.0?'+':'-',fabs(Psi));
	}
	else{
//	 fwprintf(HTMFILE,L"+\n");
	 shtm+=swprintf(shtm,L"+");
	 swprintf(Inf,Form3,A,k,Psi>=0.0?'+':'-',fabs(Psi));
	}
	for(s=Inf;*s;s++)
	 if(*s==' ')
//	  fputws(L"&nbsp;",HTMFILE);
	  shtm+=swprintf(shtm,L"&nbsp;");
	 else
//	  fputwc(*s,HTMFILE);
	  shtm+=swprintf(shtm,L"%c",*s);
   }
//   fputws(L";\n",HTMFILE);
   shtm+=swprintf(shtm,L";");
   SLH->Add(Shtm); *Shtm='\0'; shtm=Shtm;
  }
//  fputws(L"</font><br>\n",HTMFILE);
  SLH->Add(L"</font><br>");
//  Lpodv=fwprintf(HTMFILE,
//   L"<hr><font face=\"Georgia, Times New Roman, Times, serif\""
//   L" color=\"#326464\">&copy; %s, %d<br></font>\n",Autor,NYear+1900);
//  fwprintf(HTMFILE,L"</body>\n</html>\n");
//  fseek(HTMFILE,0,2);
//  fclose(HTMFILE);
  for(int i=0;i<Kpodv;i++) SLH->Add(SLP->Strings[i]);
  SLH->SaveToFile(HtmFile);
 }
 if(F){
  if(Tn!=0.0) swprintf(tt,L"%s~",::L->Time->Name);
  else wcscpy(tt,::L->Time->Name);
  if(!CONT){
   fwprintf(F,L"w=%0.6f;",M_PI*2/(Tk-Tn));
   if(Tn!=0.0) fwprintf(F,L" %s=%s-%g;",tt,::L->Time->Name,Tn);
   fputwc('\n',F);
  }
  if(wcschr(Name,'(')) swprintf(Head,Name);
  else swprintf(Head,L"%s(%s)",Name,::L->Time->Name);
  for(i=0,m=Names;i<n;i++,m=m->Sled){
   if(Kg<10){
	swprintf(Form1,L"%s*cos(  w*%s)%%c%s*sin(  w*%s)",Format[i],tt,Format[i],tt);
	swprintf(Form2,L"%s*cos(%%1d*w*%s)%%c%s*sin(%%1d*w*%s)",Format[i],tt,Format[i],tt);
	swprintf(Form3,L"%%c%s*cos(%%1d*w*%s)%%c%s*sin(%%1d*w*%s)",Format[i],tt,Format[i],tt);
   }
   else if(Kg<100){
	swprintf(Form1,L"%s*cos(   w*%s)%%c%s*sin(   w*%s)",Format[i],tt,Format[i],tt);
	swprintf(Form2,L"%s*cos(%%2d*w*%s)%%c%s*sin(%%2d*w*%s)",Format[i],tt,Format[i],tt);
	swprintf(Form3,L"%%c%s*cos(%%2d*w*%s)%%c%s*sin(%%2d*w*%s)",Format[i],tt,Format[i],tt);
   }
   else if(Kg<1000){
	swprintf(Form1,L"%s*cos(    w*%s)%%c%s*sin(    w*%s)",Format[i],tt,Format[i],tt);
	swprintf(Form2,L"%s*cos(%%3d*w*%s)%%c%s*sin(%%3d*w*%s)",Format[i],tt,Format[i],tt);
	swprintf(Form3,L"%%c%s*cos(%%3d*w*%s)%%c%s*sin(%%3d*w*%s)",Format[i],tt,Format[i],tt);
   }
   else{
	swprintf(Form1,L"%s*cos(     w*%s)%%c%s*sin(     w*%s)",Format[i],tt,Format[i],tt);
	swprintf(Form2,  L"%s*cos(%%4d*w*%s)%%c%s*sin(%%4d*w*%s)",Format[i],tt,Format[i],tt);
	swprintf(Form3,L"%%c%s*cos(%%4d*w*%s)%%c%s*sin(%%4d*w*%s)",Format[i],tt,Format[i],tt);
   }
   fwprintf(F,L"%s=L",m->S);
   W=l=wcslen(m->S)+1;
   fwprintf(F,L"%0.6f%c\n",Ac[i][0],Ac[i][1]>=0.0?'+':'-');
   for(j=0;j<l;j++) fputwc(' ',F);
   W+=swprintf(Inf,Form1,fabs(Ac[i][1]),As[i][1]>=0.0?'+':'-',fabs(As[i][1]));
   for(s=Inf;*s;s++)
	if(*s==' ') fputwc(' ',F); else fputwc(*s,F);
   for(k=2;k<Kg;k++){
	swprintf(AC,Format[i],Ac[i][k]); swprintf(AS,Format[i],As[i][k]);
	if((_wtof(AC)==0.0)&&(_wtof(AS)==0.0)) continue;
	if(W+34>Wmax){ W=l;
	 fwprintf(F,L"%c\n",Ac[i][k]>=0.0?'+':'-');
	 for(j=0;j<l;j++) fputws(L" ",F);
	 swprintf(Inf,Form2,fabs(Ac[i][k]),k,As[i][k]>=0.0?'+':'-',fabs(As[i][k]),k);
	}
	else
	 swprintf(Inf,Form3,Ac[i][k]>=0.0?'+':'-',fabs(Ac[i][k]),k,
	  As[i][k]>=0.0?'+':'-',fabs(As[i][k]),k);
	W+=34;
	for(s=Inf;*s;s++)
	 if(*s==' ') fputwc(' ',F); else fputwc(*s,F);
   }
   swprintf(AC,Format[i],Ac[i][k]); swprintf(AS,Format[i],As[i][k]);
   if((_wtof(AC)!=0.0)||(_wtof(AS)!=0.0)){
	if(W+34>Wmax){
	 fwprintf(F,L"%c\n",Ac[i][k]>=0.0?'+':'-');
	 for(j=0;j<l;j++) fputwc(' ',F);
	 swprintf(Inf,Form2,fabs(Ac[i][k]),k,As[i][k]>=0.0?'+':'-',fabs(As[i][k]),k);
	}
	else
	 swprintf(Inf,Form3,Ac[i][k]>=0.0?'+':'-',fabs(Ac[i][k]),k,
	  As[i][k]>=0.0?'+':'-',fabs(As[i][k]),k);
	for(s=Inf;*s;s++)
	 if(*s==' ') fputwc(' ',F); else fputwc(*s,F);
   }
   fputws(L";\n",F);
  }
  for(i=0,m=Names;i<n;i++,m=m->Sled){
   if(Kg<10){
	swprintf(Form1,L"%s*sin(  w*%s%%c%s)",Format[i],tt,Format[i]);
	swprintf(Form2,L"%s*sin(%%1d*w*%s%%c%s)",Format[i],tt,Format[i]);
	swprintf(Form3,L"%s*sin(%%1d*w*%s%%c%s)",Format[i],tt,Format[i]);
   }
   else if(Kg<100){
	swprintf(Form1,L"%s*sin(   w*%s%%c%s)",Format[i],tt,Format[i]);
	swprintf(Form2,L"%s*sin(%%2d*w*%s%%c%s)",Format[i],tt,Format[i]);
	swprintf(Form3,L"%s*sin(%%2d*w*%s%%c%s)",Format[i],tt,Format[i]);
   }
   else if(Kg<1000){
	swprintf(Form1,L"%s*sin(    w*%s%%c%s)",Format[i],tt,Format[i]);
	swprintf(Form2,L"%s*sin(%%3d*w*%s%%c%s)",Format[i],tt,Format[i]);
	swprintf(Form3,L"%s*sin(%%3d*w*%s%%c%s)",Format[i],tt,Format[i]);
   }
   else{
	swprintf(Form1,L"%s*sin(     w*%s%%c%s)",Format[i],tt,Format[i]);
	swprintf(Form2,L"%s*sin(%%4d*w*%s%%c%s)",Format[i],tt,Format[i]);
	swprintf(Form3,L"%s*sin(%%4d*w*%s%%c%s)",Format[i],tt,Format[i]);
   }
   fwprintf(F,L"%s=L",m->S);
   W=l=wcslen(m->S)+1;
   fwprintf(F,L"%0.6f+\n",Ac[i][0]);
   for(j=0;j<l;j++) fputws(L" ",F);
   A=sqrt(Ac[i][1]*Ac[i][1]+As[i][1]*As[i][1]);
   Psi=atan2(Ac[i][1],As[i][1]);
   W+=swprintf(Inf,Form1,A,Psi>=0.0?'+':'-',fabs(Psi));
   for(s=Inf;*s;s++)
	if(*s==' ') fputwc(' ',F); else fputwc(*s,F);
   for(k=2;k<Kg;k++){
	A=sqrt(Ac[i][k]*Ac[i][k]+As[i][k]*As[i][k]);
	swprintf(Inf,Format[i],A);
	if(_wtof(Inf)==0.0) continue;
	Psi=atan2(Ac[i][k],As[i][k]);
	if(W+34>Wmax){ W=l;
	 fputws(L"+\n",F);
	 for(j=0;j<l;j++) fputws(L" ",F);
	 swprintf(Inf,Form2,A,k,Psi>=0.0?'+':'-',fabs(Psi));
	}
	else{
	 fwprintf(F,L"+");
	 swprintf(Inf,Form3,A,k,Psi>=0.0?'+':'-',fabs(Psi));
	}
	W+=34;
	for(s=Inf;*s;s++)
	 if(*s==' ') fputwc(' ',F); else fputwc(*s,F);
   }
   A=sqrt(Ac[i][k]*Ac[i][k]+As[i][k]*As[i][k]);
   swprintf(Inf,Format[i],A);
   if(_wtof(Inf)>0.0){
	Psi=atan2(Ac[i][k],As[i][k]);
	if(W+34>Wmax){
	 fputws(L"+\n",F);
	 for(j=0;j<l;j++) fputws(L" ",F);
	 swprintf(Inf,Form2,A,k,Psi>=0.0?'+':'-',fabs(Psi));
	}
	else{
	 fputws(L"+\n",F);
	 swprintf(Inf,Form3,A,k,Psi>=0.0?'+':'-',fabs(Psi));
	}
	for(s=Inf;*s;s++)
	 if(*s==' ') fputwc(' ',F); else fputwc(*s,F);
   }
  }
  fputws(L";\n",F);
  fclose(F);
 }
 if(PoFormsToReport){
  PoFormsToReport(Name,File);
  swprintf(Inf,L"Ряд Фурье и разложение по собственным формам\n"
   L"записаны в отчет и сохранены в файле\n\"%s\".\n"
   L"Этот файл можно просмотреть и отредактировать\n"
   L"любым редактором, сплайн вставить в КИДИМ-файл для расчетов и т.д.",
   File);
 }
 else
  swprintf(Inf,L"Ряд Фурье записан в отчет и сохранен в файле\n\"%s\".\n"
   L"Этот файл можно просмотреть и отредактировать\n"
   L"любым редактором, содержимое его вставить в КИДИМ-файл для расчетов и т.д.",
   File);
 Application->MessageBox(Inf,L"Внимание!",MB_OK);
 for(i=0;i<n;i++){ free(Fs[i]); free(Ac[i]); free(As[i]); }
 free(Fs); free(Ac); free(As); free(U);
 free(P); free(D); free(C); free(L);
 for(i=0;i<CountTabl;i++) free(Ys[i]); free(Ys); free(X);
 for(i=0;i<=n;i++) free(Format[i]); free(Format);
 DelAllMagazine(&Names);
}
//---------------------------------------------------------------------------
void ToTable(wchar_t *Name,int CountTabl,double Tn,double Tk,
 void (*Funcs)(double,Magazine *,double *),wchar_t *File){
 wchar_t *s,*s0,**Format; Magazine *Names=NULL,*m; String Head;
 double **Mas=(double **)calloc(CountTabl,SzV),Step=(Tk-Tn)/(CountTabl-1),t;
 int i,*l,L,K,d,*D,j; FILE *F;
 s=wcschr(Name,'('); *s++='\0'; Head=Name; Head+=L": L";
 while(s0=wcschr(s,',')){ *s0++='\0'; TakeMagazine(&Names,s); s=s0; }
 if(s0=wcschr(s,')')){ *s0='\0'; TakeMagazine(&Names,s); }
 for(m=Names;m;m=m->Sled){ Head+=m->S; Head+=L"(t)"; if(m->Sled) Head+=L", L"; }
 K=KolElem(Names); l=(int *)calloc(K+1,SzI);
 D=(int *)calloc(K+1,SzI); L=Head.Length();
 Format=(wchar_t **)calloc(K+1,SzV);
 for(i=0;i<=K;i++) Format[i]=(wchar_t *)calloc(16,SzC);
 for(i=0;i<CountTabl;i++){
  Mas[i]=(double *)calloc(K,SzD);
  t=Tn+i*Step; Funcs(t,Names,Mas[i]);
  d=swprintf(Inf,L"%0.5f",t); if(d>l[0]) l[0]=d;
  for(j=1;j<=K;j++){ d=swprintf(Inf,L"%0.5f",Mas[i][j-1]); if(d>l[j]) l[j]=d; }
 }
 swprintf(Format[0],L"%%%d.5f",l[0]);
 D[0]=swprintf(Inf,L"время t,сек"); if(l[0]>D[0]) D[0]=l[0];
 for(m=Names,j=1;m;m=m->Sled,j++){
  swprintf(Format[j],L"%%%d.5f",l[j]);
  D[j]=swprintf(Inf,m->S); if(l[j]>D[j]) D[j]=l[j];
 }
 d=1; for(j=0;j<=K;j++) d+=1+D[j];
 while(d<L){ for(j=0;j<=K;j++) D[j]++; d+=K+1; }
 if(F=_wfopen(File,L"w")){
  for(j=0;j<(d-L)/2;j++) fputwc(' ',F); fputws(Head.w_str(),F); fputwc('\n',F);
  fputwc('+',F); for(j=0;j<D[0];j++) fputwc('-',F); fputwc('+',F);
  for(i=1;i<=K;i++){ for(j=0;j<D[i];j++) fputwc('-',F); fputwc('+',F); }
  fputwc('\n',F);
  fputwc('|',F);
  for(j=0;j<(D[0]-12)/2;j++) fputwc(' ',F);
  j+=fwprintf(F,L"время t,сек"); for(;j<D[0];j++) fputwc(' ',F);
  fputwc('|',F);
  for(m=Names,i=1;m;i++,m=m->Sled){
   for(j=0;j<(D[i]-wcslen(m->S))/2;j++) fputwc(' ',F);
   j+=fwprintf(F,m->S); for(;j<D[i];j++) fputwc(' ',F); fputwc('|',F);
  }
  fputwc('\n',F);
  fputwc('+',F); for(j=0;j<D[0];j++) fputwc('-',F); fputwc('+',F);
  for(i=1;i<=K;i++){ for(j=0;j<D[i];j++) fputwc('-',F); fputwc('+',F); }
  fputwc('\n',F);
  for(i=0;i<CountTabl;i++){
   fputwc('|',F); for(j=0;j<(D[0]-l[0])/2;j++) fputwc(' ',F);
   t=Tn+i*Step;
   j+=fwprintf(F,Format[0],t); for(;j<D[0];j++) fputwc(' ',F);
   fputwc('|',F);
   for(j=1;j<=K;j++){ int k;
	for(k=0;k<(D[j]-l[j])/2;k++) fputwc(' ',F);
	k+=fwprintf(F,Format[j],Mas[i][j-1]);
	for(;k<D[j];k++) fputwc(' ',F); fputwc('|',F);
   }
   fputwc('\n',F);
  }
  fputwc('+',F); for(j=0;j<D[0];j++) fputwc('-',F); fputwc('+',F);
  for(i=1;i<=K;i++){ for(j=0;j<D[i];j++) fputwc('-',F); fputwc('+',F); }
  fputwc('\n',F);
 }
 fclose(F);
// if(HTMFILE=_wfopen(HtmFile,L"r+")){
//  if(!fseek(HTMFILE,-Lpodv,2)){
 if(SLH){
  for(int i=0;i<Kpodv;i++) SLH->Delete(SLH->Count-1);
//  fwprintf(HTMFILE,L"<h5 align=\"left\"><font color=\"#000099\">"
//   L"Таблица значений \"%s\"</font></h5>\n",Head);
  swprintf(Shtm,L"<h5 align=\"left\"><font color=\"#000099\">"
   L"Таблица значений \"%s\"</font></h5>",Head);
  SLH->Add(Shtm);
//  fputws(L"<table align=\"center\" width=\"100%\" border=\"1\""
//   L" height=\"0\" cellspacing=\"2\" cellpadding=\"4\">\n",HTMFILE);
//  fputws(L"<tr>\n",HTMFILE);
  SLH->Add(L"<table align=\"center\" width=\"100%\" border=\"1\""
   L" height=\"0\" cellspacing=\"2\" cellpadding=\"4\"><tr>");
  SLH->Add(L"<td>\n<div align=\"center\">время t, сек</div>\n</td>");
  for(m=Names;m;m=m->Sled){
//   fwprintf(HTMFILE,L"<td>\n<div align=\"center\">%s</div>\n</td>",m->S);
   SLH->Add(L"<td>");
   shtm+=swprintf(shtm,L"<div align=\"center\">");
   shtm+=swprintf(shtm,m->S);
   shtm+=swprintf(shtm,L"</div>");
   SLH->Add(Shtm); *Shtm='\0'; shtm=Shtm;
   shtm+=swprintf(shtm,L"</td>");
  }
//  fputws(L"\n</tr>\n",HTMFILE);
  *shtm++='\n'; *shtm='\0'; SLH->Add(Shtm); *Shtm='\0'; shtm=Shtm;
  SLH->Add(L"</tr>");
  for(i=0;i<CountTabl;i++){
//   fwprintf(HTMFILE,L"<td>\n<div align=\"right\">%0.5f</div>\n</td>",Tn+i*Step);
   shtm+=swprintf(shtm,L"<td>\n<div align=\"right\">%0.5f</div>",Tn+i*Step);
   SLH->Add(Shtm); *Shtm='\0'; shtm=Shtm;
   shtm+=swprintf(shtm,L"</td>");
   for(j=1;j<=K;j++){
//	fwprintf(HTMFILE,L"<td>\n<div align=\"right\">%0.5f</div>\n</td>",Mas[i][j-1]);
	shtm+=swprintf(shtm,L"<td>\n<div align=\"right\">%0.5f</div>",Mas[i][j-1]);
	SLH->Add(Shtm); *Shtm='\0'; shtm=Shtm;
	shtm+=swprintf(shtm,L"</td>");
   }
//   fputws(L"</tr>\n",HTMFILE);
   SLH->Add(L"</tr>");
  }
//  fputws(L"</table>\n",HTMFILE);
  SLH->Add(L"</table>");
//  fwprintf(HTMFILE,L"<hr><font face=\"Georgia, Times New Roman, Times, serif\""
//   L" color=\"#326464\">&copy; %s, %d<br></font><br>\n",Autor,NYear+1900);
//  fwprintf(HTMFILE,L"</body>\n</html>\n");
//  fseek(HTMFILE,0,2);
//  fclose(HTMFILE);
  for(int i=0;i<Kpodv;i++) SLH->Add(SLP->Strings[i]);
  SLH->SaveToFile(HtmFile);
 }
// fclose(HTMFILE);
 for(i=0;i<=K;i++) free(Format[i]); free(Format); free(D); free(l);
 for(i=0;i<CountTabl;i++) free(Mas[i]); free(Mas); DelAllMagazine(&Names);
}
//---------------------------------------------------------------------------


