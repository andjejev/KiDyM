#include "CA.h"
#include "ConvDW.h"
#include "TablW.h"
#include "string.h"
#include <stdio.h>
#include <io.h>
#include <dir.h>
#include <math.h>
#include <locale>

extern FILE *DGSFILE; extern FILE *HTMFILE; extern List *LBeg;
extern int N,NzK,NPV,lmax,StrLong,Lpodv,LpodvD;
extern Cnst *Pust,*Nul;
extern char Inf[],InpFile[],DgsFile[],HtmFile[],HeadCalc[],HeadWork[],
 HeadVersion[],Autor[],WORKDIR[],CurrDir[],AtrForse;
extern Root *Opers; extern int NYear,PUNKT; extern List *L;
extern bool PRINTOP;
List *Lst; bool NEHOL,CHAP=true; Vary *VLast;
void (*PoFormsToReport)(char *,char *);
//---------------------------------------------------------------------------
void TablToHTM(char *File,int CountTabl,double Tn,double Tk,
  bool Godograf,char *Format1,char *Format2,char *Format3,
  double *X,double *Y,int Lpodv,char *Autor){
 FILE *F=fopen(File,"r");
 char S[256],S1[80],S2[80],S3[80],*s,*Head[3],HeadTabl[80];
 double Step; int i,j,k,Row=CountTabl/(3-Godograf);
 if(CountTabl%(3-Godograf)) Row++; Step=(Tk-Tn)/(CountTabl-1);
 if(F){
  if(HTMFILE=fopen(HtmFile,"r+")){
   if(!fseek(HTMFILE,-Lpodv,2)){
    Fgets(S,255,F); if(s=strrchr(S,'\n')) *s='\0';
    for(s=S;*s&&(*s==' ');s++); strcpy(HeadTabl,s);
    if(Fgets(S,255,F)&&Fgets(S,255,F)){
     if(s=strrchr(S,'|')) *s='\0';
     for(s=S;*s&&(*s!='|');s++); Head[0]=++s;
     for(;*s&&(*s!='|');s++); *s++='\0'; Head[1]=++s;
     if(Godograf){
      for(;*s&&(*s!='|');s++); *s++='\0'; Head[2]=++s;
      for(s=Head[2]+strlen(Head[2])-1;(s>Head[2])&&(*s==' ');) s--;
      s++; *s='\0';
     }
     for(s=Head[1]+strlen(Head[1])-1;(s>Head[1])&&(*s==' ');) s--;
     s++; *s='\0';
     for(s=Head[0]+strlen(Head[0])-1;(s>Head[0])&&(*s==' ');) s--;
     s++; *s='\0';
    }
    fprintf(HTMFILE,
     "<h5 align=\"left\"><font color=\"#000099\">"
     "Таблица значений \"%s\" (см. файл \"%s\")</font></h5>\n",
     HeadTabl,File);
    fputs("<p align=\"left\"><table width=\"100%\" border=\"1\""
     " height=\"0\" cellspacing=\"2\" cellpadding=\"4\">\n",HTMFILE);
    fputs("<tr>\n",HTMFILE);
    for(i=0;i<3-Godograf;i++)
     for(j=0;j<2+Godograf;j++)
//      fprintf(HTMFILE,"<td>\n<div align=\"center\">%s</div>\n</td>",
//       Head[j]);
      fprintf(HTMFILE,"<th>%s</th>",Head[j]);
    fputs("\n</tr>\n",HTMFILE);
    sprintf(S1,"<td>\n<div align=\"right\">%s</div>\n</td>",Format1);
    if(Godograf)
     sprintf(S2,"<td>\n<div align=\"right\">%s</div>\n</td>",Format2);
    sprintf(S3,"<td>\n<div align=\"right\">%s</div>\n</td>",Format3);
    for(i=0;i<Row-1;i++){
     fputs("<tr>\n",HTMFILE);
     for(j=0;j<3-Godograf;j++){
      fprintf(HTMFILE,S1,Tn+(j*Row+i)*Step);
      if(Godograf) fprintf(HTMFILE,S2,X[j*Row+i]);
      fprintf(HTMFILE,S3,Y[j*Row+i]);
     }
     fputs("</tr>\n",HTMFILE);
    }
    fputs("<tr>\n",HTMFILE);
    if(CountTabl%(3-Godograf)){
     for(j=0;j<CountTabl%(3-Godograf);j++){
      fprintf(HTMFILE,S1,Tn+(j*Row+i)*Step);
      if(Godograf) fprintf(HTMFILE,S2,X[j*Row+i]);
      fprintf(HTMFILE,S3,Y[j*Row+i]);
     }
     for(;j<3-Godograf;j++){
      fprintf(HTMFILE,"<td>\n<div align=\"right\">&nbsp;</div>\n</td>");
      if(Godograf)
       fprintf(HTMFILE,"<td>\n<div align=\"right\">&nbsp;</div>\n</td>");
      fprintf(HTMFILE,"<td>\n<div align=\"right\">&nbsp;</div>\n</td>");
    }}
    else{
     for(j=0;j<3-Godograf;j++){
      fprintf(HTMFILE,S1,Tn+(j*Row+i)*Step);
      if(Godograf) fprintf(HTMFILE,S2,X[j*Row+i]);
      fprintf(HTMFILE,S3,Y[j*Row+i]);
    }}
    fputs("</tr>\n",HTMFILE);
    fputs("</table>\n</p>",HTMFILE);
    fprintf(HTMFILE,
     "<p align=\"left\"><hr><font face=\"Georgia, Times New Roman, Times, serif\""
     " color=\"#326464\">&copy; %s, %d<br></font></p>\n",Autor,NYear+1900);
    fprintf(HTMFILE,"</body>\n</html>\n");
   }
   fseek(HTMFILE,0,2); fclose(HTMFILE);
  }
  fclose(F);
}}
//---------------------------------------------------------------------------
void ReadAllRight(Parm *P,FILE *F,bool ToGrek,bool Eqv){
 char SName[1024]; int Ln=P->Line;
 for(Parm *p=P;p;p=p->Sled){
  if(p->Line>Ln){ fputs("<br>\n&nbsp;",F); Ln=p->Line; }
  if(p->Nam){
   strcpy(SName,p->Nam); if(ToGrek) Grek(SName); fprintf(F,"%s",SName);
   if(p->R){
    if(Eqv&&(KolElem(p->R)==1)&&!p->R->R) fputc('=',F); else fputc('(',F);
    for(Parm *pp=p->R;pp;pp=pp->Sled){
     if(pp->Line>Ln){ fputs("<br>\n&nbsp;",F); Ln=pp->Line; }
     if(pp->Nam){
      strcpy(SName,pp->Nam); if(ToGrek) Grek(SName); fprintf(F,"%s",SName);
      if(pp->R){
       if(Eqv&&(KolElem(pp->R)==1)&&!pp->R->R) fputc('=',F); else fputc('(',F);
       for(Parm *ppp=pp->R;ppp;ppp=ppp->Sled){
        if(ppp->Line>Ln){ fputs("<br>\n&nbsp;",F); Ln=ppp->Line; }
        if(ppp->Nam){
         strcpy(SName,ppp->Nam); if(ToGrek) Grek(SName); fprintf(F,"%s",SName);
        }
        else fprintf(F,"%g",ppp->Val);
        if(ppp->Sled) fputs(", ",F);
        else if(!Eqv||(KolElem(pp->R)>1)&&pp->R->R) fputc(')',F);
     }}}
     else fprintf(F,"%g",pp->Val);
     if(pp->Sled) fputs(", ",F);
     else if(!Eqv||(KolElem(p->R)>1)||p->R->R) fputc(')',F);
  }}}
  else fprintf(F,"%g",p->Val);
  if(p->Sled) fputs(", ",F);
}}
//---------------------------------------------------------------------------
void StrToHTM(char *S,FILE *HTMFILE){
 bool It=false; int Ks=0,Ind=0,i;
 for(;*S&&(*S!='\n');S++){
  switch(*S){
   case '<': fputs("&#60;",HTMFILE); break;
   case '>': fputs("&#62;",HTMFILE); break;
   case ' ': fputs("&nbsp;",HTMFILE); break;
   case ',':case '|':case '\\':case '(':case ')':
    if(It){ fputs("</i>",HTMFILE); It=false; }
    if(Ind>0){ for(i=Ind;i--;) fprintf(HTMFILE,"</font></sub>"); Ind=0; }
    fputc(*S,HTMFILE);
    break;
   case '$':
    S++;
    if(*S=='('){ Ks++;
      Ind++; fprintf(HTMFILE,"<sub><font size=\"-%d\">%c",Ind,*S);
     for(S++;*S;S++){
      if(*S=='(') Ks++;
      else if(*S==')') Ks--;
      if(It){
       if((isdigit(*S)||isciril(*S))){
        fprintf(HTMFILE,"</i>%c",*S); It=false;
       }
       else fputc(*S,HTMFILE);
      }
      else{
       if(isalpha(*S)){ fputs("<i>",HTMFILE); It=true; fputc(*S,HTMFILE); }
       else fputc(*S,HTMFILE);
      }
      if(!Ks){ fprintf(HTMFILE,"</font></sub>"); Ind--; break; }
    }}
    else{
     Ind++; fprintf(HTMFILE,"<sub><font size=\"-%d\">%c</font></sub>",Ind,*S);
    }
    break;
   default:
    if(It){
     if(isdigit(*S)){
      fprintf(HTMFILE,"</i><sub><font size=\"-1\">%c",*S); It=false; Ind++;
     }
     else if(isciril(*S)){
      if(Ind) fprintf(HTMFILE,"</font></sub>"); Ind++;
      fprintf(HTMFILE,"</i>%c",*S); It=false;
     }
     else fputc(*S,HTMFILE);
    }
    else{
     if(isalpha(*S)){ fputs("<i>",HTMFILE); It=true; fputc(*S,HTMFILE); }
     else fputc(*S,HTMFILE);
    }
 }}
 if(It) fputs("</i>",HTMFILE);
 fputs("<br>\n",HTMFILE);
}
//---------------------------------------------------------------------------
void StrToHTM(char *S){ char Rez[256],*r=Rez;
 for(;*S&&(*S!='\n');S++){
  switch(*S){
   case '<': r+=sprintf(r,"&#60;"); break;
   case '>': r+=sprintf(r,"&#62;"); break;
   case ' ': r+=sprintf(r,"&nbsp;"); break;
   case '$': S++; r+=sprintf(r,"<sub><font size=\"-1\">%c</font></sub>",*S); break;
   default:  *r++=*S;
 }}
 sprintf(r,"<br>\n"); strcpy(S,Rez);
}
//---------------------------------------------------------------------------
bool Number(char *S){
 if(!S) return false;
 for(char *s=S;*s;s++) if(isciril(*s)||isalpha(*s)) return false;
 return true;
}
//---------------------------------------------------------------------------
void GrekCnst(char *Name){
      if(!strcmp(Name,"PI"))        strcpy(Name,"&#960;");
 else if(!strcmp(Name,"2PI"))       strcpy(Name,"2&#960;");
 else if(!strcmp(Name,"3PI"))       strcpy(Name,"3&#960;");
 else if(!strcmp(Name,"4PI"))       strcpy(Name,"4&#960;");
 else if(!strcmp(Name,"5PI"))       strcpy(Name,"5&#960;");
 else if(!strcmp(Name,"6PI"))       strcpy(Name,"6&#960;");
 else if(!strcmp(Name,"7PI"))       strcpy(Name,"7&#960;");
 else if(!strcmp(Name,"PIna2"))     strcpy(Name,"&#960;/2");
 else if(!strcmp(Name,"PIna3"))     strcpy(Name,"&#960;/3");
 else if(!strcmp(Name,"PIna4"))     strcpy(Name,"&#960;/4");
 else if(!strcmp(Name,"PIna6"))     strcpy(Name,"&#960;/6");
 else if(strstr(Name,"sqrt")==Name) sprintf(Name,"&#8730;%s",Name+4);
}
//---------------------------------------------------------------------------
bool Diagnos(TCGauge *CGauge,TStatusBar *StatusBar){
 int l,Np,i,*KO=(int*)calloc(6,SzI),g; char S[50]; Cnst *C; Root *I;
 bool Rez=false;
 DGSFILE=fopen(DgsFile,access(DgsFile,0)?"w":"a");
 if(!DGSFILE) return Rez; Rez=true; lmax=StrLong;
 fprintf(DGSFILE,"<html>\n<head>\n<title>Д И А Г Н О С Т И К А</title>\n"
  "<meta http-equiv=\"Content-Type\" content=\"text/html;"
  "charset=windows-1251\">\n</head>\n"
  "<body bgcolor=\"#FFFFFF\" text=\"#800000\""
  " background=\"%s/fon2.jpg\""
  " font face=\"Times New Roman, Times, serif\">\n",
   WORKDIR);
 if(HeadVersion[0])
  fprintf(DGSFILE,"<h3 align=\"center\"><font color=\"#990000\">"
     "Д И А Г Н О С Т И К А<br>\nПрограмма: \"%s\"<br>\n"
     "Работа: %s<br>\nВариант: %s<br>\nАвторское право: %s</font></h3>\n",
    HeadCalc,HeadWork,HeadVersion,Autor);
 else
  fprintf(DGSFILE,"<h3 align=\"center\"><font color=\"#990000\">"
     "Д И А Г Н О С Т И К А<br>\nПрограмма: \"%s\"<br>\n"
     "Работа: %s<br>\nАвторское право: %s</font></h3>\n",
     HeadCalc,HeadWork,Autor);
 fprintf(DGSFILE,"<h4 align=\"left\"><font color=\"#000099\">"
  "Список констант:</h4>\n</font>");
//  "<font face=\"Courier New, Courier, mono\"><b>");
 fputs("<table width=\"100%\" border=\"1\">\n",DGSFILE);
 fputs("<tr>",DGSFILE);
 for(i=0;i<4;i++)
  fputs("<td><div align=\"center\"><b>Имя</b></div></td>\n"
   "<td><div align=\"center\"><b>Значение</b></div></td>\n",DGSFILE);
 fputs("</tr>",DGSFILE);
 for(C=Pust->Sled,i=0;C;C=C->Sled,i++){ char Name[80];
  if(!(i%4)){ if(i) fputs("</tr>\n",DGSFILE); fputs("<tr>\n <td>",DGSFILE); }
  else fputs("<td>",DGSFILE);
  if(!Number(C->Name)){
   strcpy(Name,C->Name); GrekCnst(Name);
   fprintf(DGSFILE,"%7s</td>\n<td>%10g</td>\n",Name,C->Val);
   sprintf(Inf,"%7s</td>\n<td>%10g</td>\n",Name,C->Val);
  }
  else{
   fprintf(DGSFILE,"&nbsp;</td>\n<td>%10g</td>\n",C->Val);
   sprintf(Inf,"&nbsp;</td>\n<td>%10g</td>\n",C->Val);
 }}
 for(;i%4;i++) fputs("<td>&nbsp;</td>\n<td>&nbsp;</td>\n",DGSFILE);
 fputs("</td>\n<tr></table>",DGSFILE);
 fputs("</b><br>\n</font>",DGSFILE);
 for(Lst=LBeg;Lst;Lst=Lst->Sled){ int Ki=0,Kr=0;
  if(LBeg->Sled)
   fprintf(DGSFILE,"<h3 align=\"center\"><font color=\"#990000\">"
    "ЛИСТ - %s</font></h3>\n",Lst->Name);
//обнулим значения переменных интегрирования и соберем список интегралов
//обнулим значения переменных функций поиска корней и соберем список корней
  for(Vary *V=L->V;V;V=V->Sled){
   Ki=VaryToIntg(Ki,V->Znach,Nul);
   Kr=VaryToSol(Kr,V->Znach,Nul);
  }
  if((N=Coord(Lst))||NPV){ Cord *k;
   if(I=FindInstLast("ОБОБЩЕННЫЕ КООРДИНАТЫ")){
    Parm *P; int NK=KolElem(I->First);
    if(N){
     fprintf(DGSFILE,"<h4 align=\"left\"><font color=\"#000099\">"
      "Неопределенные переменные:</font></h4>\n");
     l=0;
     for(k=Lst->K;k->Sled;k=k->Sled){
      strcpy(Inf,k->V->Name); g=Grek(Inf);
      if((l+g+2)>lmax){ fprintf(DGSFILE,"<br>\n "); l=0; }
      l+=g+2; fprintf(DGSFILE,"%s, ",Inf);
     }
     strcpy(Inf,k->V->Name); g=Grek(Inf);
     if((l+g+1)>lmax) fprintf(DGSFILE,"<br>\n ");
     fprintf(DGSFILE,"%s",Inf);
    }
    fprintf(DGSFILE,"<h4 align=\"left\"><font color=\"#000099\">"
     "Обобщенные координаты:</font></h4>\n");
    l=0; lmax=StrLong;
    for(P=I->First;P->Sled;P=P->Sled){
     strcpy(Inf,P->Nam); g=Grek(Inf);
     if((l+g+2)>lmax){ fprintf(DGSFILE,"<br>\n "); l=0; }
     l+=g+2; fprintf(DGSFILE,"%s, ",Inf);
    }
    strcpy(Inf,P->Nam); g=Grek(Inf);
    if((l+g+1)>lmax) fprintf(DGSFILE,"<br>\n ");
    fprintf(DGSFILE,"%s",Inf);
   }
   else{
    fprintf(DGSFILE,"<h4 align=\"left\"><font color=\"#000099\">"
     "Обобщенные координаты:</font></h4>\n");
    l=0; lmax=StrLong;
    for(k=Lst->K;k->Sled;k=k->Sled){
     strcpy(Inf,k->V->Name); g=Grek(Inf);
     if((l+g+2)>lmax){ fprintf(DGSFILE,"<br>\n "); l=0; }
     l+=g+2; fprintf(DGSFILE,"%s, ",Inf);
    }
    strcpy(Inf,k->V->Name); g=Grek(Inf);
    if((l+g+1)>lmax) fprintf(DGSFILE,"<br>\n ");
    fprintf(DGSFILE,"%s",Inf);
    fprintf(DGSFILE,"<h4 align=\"left\"><font color=\"#000099\">"
     "Число обобщенных координат = </font>%d<font color=\"#000099\">.</font></h4>\n",N);
   }
   if(NPV){
    fprintf(DGSFILE,"<h4 align=\"left\"><font color=\"#000099\">"
     "Псевдоскорости:</font></h4>\n");
    l=0;
    for(k=Lst->W;k->Sled;k=k->Sled){
     strcpy(Inf,k->V->Name); g=Grek(Inf);
     if((l+g+2)>lmax){ fprintf(DGSFILE,"<br>\n "); l=0; }
     l+=g+2; fprintf(DGSFILE,"%s, ",Inf);
    }
    strcpy(Inf,k->V->Name); g=Grek(Inf);
    if((l+g+1)>lmax) fprintf(DGSFILE,"<br>\n ");
    fprintf(DGSFILE,"%s",Inf);
    fprintf(DGSFILE,"<h4 align=\"left\"><font color=\"#000099\">"
     "Число псевдоскоростей = </font>%d<font color=\"#000099\">.</font></h4>\n",NPV);
   }
   fprintf(DGSFILE,"<h4 align=\"left\"><font color=\"#000099\">"
    "Число степеней свободы = </font>%d<font color=\"#000099\">.</font></h4>\n",
    N+NPV);
   if(NzK){
    fprintf(DGSFILE,"<h4 align=\"left\"><font color=\"#000099\">"
     "Независимые координаты с зависимыми вариациями:</font></h4>\n");
    l=0;
    for(k=Lst->Z;k->Sled;k=k->Sled){
     strcpy(Inf,k->V->Name); g=Grek(Inf);
     if((l+g+2)>lmax){ fprintf(DGSFILE,"<br>\n "); l=0; }
     l+=g+2; fprintf(DGSFILE,"%s, ",Inf);
    }
    strcpy(Inf,k->V->Name); g=Grek(Inf);
    if((l+g+1)>lmax) fprintf(DGSFILE,"<br>\n ");
    fprintf(DGSFILE,"%s",Inf);
    fprintf(DGSFILE,"<h4 align=\"left\"><font color=\"#000099\">"
     "Число координат с зависимыми вариациями = </font>%d."
     "<font color=\"#000099\"></font></h4>\n",NzK);
    for(Cord *z=Lst->Z;z;z=z->Sled){
     Form F1,F2,F; Vary *V,*V1,*V2; char S[256],Sv1[256],Sv2[256];
     sprintf(Inf,"%s'%s",z->V->Name,Lst->Time->Name);
     if(FindVary(Inf,&V)){
      for(Cord *K=Lst->K;K;K=K->Sled){
       sprintf(Sv1,"%s'%s",K->V->Name,Lst->Time->Name);
       if(FindVary(Sv1,&V1)){
        for(k=K->Sled;k;k=k->Sled){
         F1=DifBy(DifBy(V->Znach,V1),k->V);
         sprintf(Sv2,"%s'%s",k->V->Name,Lst->Time->Name);
         FindVary(Sv2,&V2);
         F2=DifBy(DifBy(V->Znach,V2),K->V);
         F=F1-F2;
         if(F.C!=Nul){ NEHOL=true; goto N; }
      }}}
      if(Lst->W) NEHOL=true;
    }}
N:  if(NEHOL){
     fprintf(DGSFILE,"<h4 align=\"left\"><font color=\"#000099\">"
      "Система неголономная.</font></h4>\n");
     for(Cord *Z=Lst->Z;Z;Z=Z->Sled){ Vary *V;
      sprintf(Inf,"%s'%s",Z->V->Name,Lst->Time->Name); FindVary(Inf,&V);
      for(Cord *z=Lst->Z;z;z=z->Sled){
       if(VarInForm(true,V->Znach,z->V)){ CHAP=false; goto Ch; }
     }}
     for(Elem *E=Lst->E;E;E=E->Sled){
      for(Cord *z=Lst->Z;z;z=z->Sled){
       if(VarInForm(true,E->Znach,z->V)){ CHAP=false; goto Ch; }
       if(VarInForm(true,E->K->Znach,z->V)){ CHAP=false; goto Ch; }
     }}
Ch:  if(CHAP)
      fprintf(DGSFILE,"<h4 align=\"left\"><font color=\"#000099\">"
       "Это система С.А. Чаплыгина.</font></h4>\n");
    }
    else{
     fprintf(DGSFILE,"<h4 align=\"left\"><font color=\"#009900\">"
      "Система голономная.<br>"
      "Все трехиндексные символы для зависимых скоростей равны нулю.<br>"
      "</font></h4>\n");
/*
     for(Cord *z=Lst->Z;z;z=z->Sled){
      Form F1,F2; Vary *V,*V1,*V2; char S[256],Sv1[256],Sv2[256]; int W;
      sprintf(Inf,"%s'%s",z->V->Name,Lst->Time->Name);
      FindVary(Inf,&V);
      for(Cord *K=Lst->K;K;K=K->Sled){
       sprintf(Sv1,"%s'%s",K->V->Name,Lst->Time->Name);
       FindVary(Sv1,&V1);
       for(k=K->Sled;k;k=k->Sled){
        F1=DifBy(DifBy(V->Znach,V1),k->V);
        sprintf(Sv2,"%s'%s",k->V->Name,Lst->Time->Name);
        FindVary(Sv2,&V2);
        F2=DifBy(DifBy(V->Znach,V2),K->V);
//        F=F1-F2;
//        sprintf(S,
//         "&#8706;&#178;%s/&#8706;%s&#8706;%s",Inf,V1->Name,k->V->Name);
//        W=PrintForm(false,DGSFILE,S,F1)-23;
//        fprintf(DGSFILE,"<font color=\"#009900\"> совпадает с </font>");
//        if(W>50) fputs("<br>",DGSFILE);
//        sprintf(S,
//         "&#8706;&#178;%s/&#8706;%s&#8706;%s",Inf,V2->Name,K->V->Name);
//        PrintForm(false,DGSFILE,S,F2);
        W-=fprintf(DGSFILE,
         "&#8706;&#178;%s/&#8706;%s&#8706;%s",Inf,V1->Name,k->V->Name);
        W+=PrintForm(false,DGSFILE,"",F1);
        fprintf(DGSFILE,"<font color=\"#009900\"> совпадает с </font>");
        if(W>50) fputs("<br>",DGSFILE);
        fprintf(DGSFILE,
         "&#8706;&#178;%s/&#8706;%s&#8706;%s",Inf,V2->Name,K->V->Name);
        PrintForm(false,DGSFILE,"",F2);
        fputs("<font color=\"#009900\"> !<br></font>",DGSFILE);
     }}}*/
  }}}
  else{ Root *I; Parm *p;
   if(I=FindInstLast("ОБОБЩЕННЫЕ КООРДИНАТЫ")){ Vary *V;
    fprintf(DGSFILE,"<h4 align=\"left\"><font color=\"#000099\">"
     "Обобщенные координаты (согласно инструкции):</font></h4>\n");
    l=0;
    for(p=I->First;p->Sled;p=p->Sled){
     if(FindVary(p->Nam,&V)) TakeCord(V);
     else{
      sprintf(Inf,"Обобщенная координата \"%s\" не найдена в списке переменных!",p->Nam);
      fprintf(DGSFILE,
       "<h4 align=\"left\"><font color=\"#000099\">%s:</font></h4>\n",Inf);
      Rez=false;
     }
     strcpy(Inf,p->Nam); g=Grek(Inf); N++;
     if((l+g+2)>lmax){ fprintf(DGSFILE,"<br>\n "); l=0; }
     l+=g+2; fprintf(DGSFILE,"%s, ",Inf);
    }
    if(FindVary(p->Nam,&V)) TakeCord(V);
    else{
     sprintf(Inf,"Обобщенная координата \"%s\" не найдена в списке переменных!",p->Nam);
     fprintf(DGSFILE,
      "<h4 align=\"left\"><font color=\"#000099\">%s:</font></h4>\n",Inf);
     Rez=false;
    }
    strcpy(Inf,p->Nam); g=Grek(Inf); N++;
    if((l+g+1)>lmax) fprintf(DGSFILE,"<br>\n ");
    fprintf(DGSFILE,"%s",Inf);
    fprintf(DGSFILE,"<h4 align=\"left\"><font color=\"#000099\">"
     "Число степеней свободы = %d.</font></h4>\n",N);
    N=0;
   }
   else
    fprintf(DGSFILE,"<h4 align=\"left\"><font color=\"#000099\">"
     "НЕТ ОБОБЩЕННЫХ КООРДИНАТ!</font></h4>\n");
  }
  Np=KolElem(Lst->P);
  if(Np){ int L;
   fprintf(DGSFILE,"<h4 align=\"left\"><font color=\"#000099\">"
    "Неизвестные величины:</font></h4>\n");
   strcpy(Inf,Lst->P->V->Name); L=Grek(Inf); fprintf(DGSFILE,"%s",Inf);
//   l=fprintf(DGSFILE,"%s",Lst->P->V->Name);
   for(Cord *n=Lst->P->Sled;n;n=n->Sled){
    strcpy(Inf,n->V->Name); l=Grek(Inf);
    if((L+2)>lmax){ fputs("<br>\n ",DGSFILE); L=l+2; } else L+=l+2;
    fprintf(DGSFILE,", %s",Inf);
//    if((l=strlen(n->V->Name)+2)>lmax){ fputs("<br>\n ",DGSFILE); l=0; }
//    l+=fprintf(DGSFILE,", %s",n->V->Name);
   }
   fprintf(DGSFILE,"<h4 align=\"left\"><font color=\"#000099\">"
    "Число неизвестных = %d.</font></h4>\n",Np);
  }
  fprintf(DGSFILE,"<b><br>\nПеременная &nbsp;&nbsp; 'Время': </b>");
  sprintf(S,"%s",Lst->Time->Name);
  PrintForm(false,DGSFILE,S,Lst->Time->Znach);
  fprintf(DGSFILE,"<b><br>\nПеременная 'Частота': </b>");
  sprintf(S,"%s",Lst->Freq->Name);
  PrintForm(false,DGSFILE,S,Lst->Freq->Znach);
  if(Lst->E){ Elem *E;
   StatusBar->SimpleText="Диагностика элементов";
   if(CGauge){ CGauge->MaxValue=KolElem(Lst->E); CGauge->Progress=0; }
   fprintf(DGSFILE,"<h4 align=\"left\"><font color=\"#000099\">"
    "Список элементов:</font></h4>\n");
   fputs("<table width=\"100%\" border=\"1\">\n",DGSFILE);
   fputs("<tr>",DGSFILE);
   for(i=0;i<3;i++)
    fputs(" <td><div align=\"center\"><b>Элементы</b></div></td>\n",DGSFILE);
   fputs("</tr>",DGSFILE);
   for(E=Lst->E;E;E=E->Sled){
    if(CGauge) CGauge->Progress++;
    if(!i) fputs("<tr>\n",DGSFILE);
    else if(!(i%3)) fputs("</tr>\n<tr>\n",DGSFILE);
    i++;
    fputs(" <td><div align=\"center\"><b>",DGSFILE);
    PrintForm(false,DGSFILE,E->Name,E->Znach);
    fputs("</b></div></td>\n",DGSFILE);
   }
   fputs("</tr>",DGSFILE);
   fputs("</table>\n",DGSFILE);
/*   for(E=Lst->E;E;E=E->Sled){
    if(*E->Name==AtrForse) continue;
    for(Cord *K=Lst->K;K;K=K->Sled){
     if(VarInForm(true,E->Znach,K->V)){
      sprintf(Inf,"Характеристика элемента \"%s\"\n"
       "зависит от обобщенных координат, что недопустимо!",E->Name);
      Application->MessageBox(Inf,"ОШИБКА!",MB_OK|MB_ICONERROR); Rez=false;
      sprintf(Inf,
       "Характеристика элемента \"%s\" зависит от обобщенных координат,"
       "что недопустимо!",E->Name);
      fprintf(DGSFILE,"<h4 align=\"left\"><font color=\"#000099\">"
       "%s</font></h4>\n",Inf);
   }}} */
  }
  if(Lst->B){
   StatusBar->SimpleText="Диагностика твёрдых тел";
   if(CGauge){ CGauge->MaxValue=KolElem(Lst->B); CGauge->Progress=0; }
   fprintf(DGSFILE,"<h4 align=\"left\"><font color=\"#000099\">"
    "Список твердых тел:</font></h4>\n");
   for(Body *B=Lst->B;B;B=B->Sled){
    if(CGauge) CGauge->Progress++;
    fprintf(DGSFILE,"<br>\nСтрока №%4d: %s",B->Line,B->Name);
    if(B->BaseName){ fprintf(DGSFILE,"~%s",B->BaseName); }
    fputs(" | ",DGSFILE);
    if(B->KoordSys){
     ReadAllRight(B->KoordSys,DGSFILE,true,true); fputs(" | ",DGSFILE);
    }
    if(B->Inert){ ReadAllRight(B->Inert,DGSFILE,true,false); }
    fputc(';',DGSFILE);
  }}
  if(Lst->T){
   StatusBar->SimpleText="Диагностика стержней";
   if(CGauge){ CGauge->MaxValue=KolElem(Lst->T); CGauge->Progress=0; }
   fprintf(DGSFILE,"<h4 align=\"left\"><font color=\"#000099\">"
    "Список стержней:</font></h4>\n");
   for(Stringer *T=Lst->T;T;T=T->Sled){
    if(CGauge) CGauge->Progress++;
    fprintf(DGSFILE,"<br>\nСтрока №%4d: %s",T->Line,T->Name);
    fputs(" \\ ",DGSFILE);
    if(T->Parms)
     ReadAllRight(T->Parms,DGSFILE,true,true);
    fputc(';',DGSFILE);
  }}
  if(Lst->Inst){ char InstName[256];
   StatusBar->SimpleText="Диагностика инструкций";
   if(CGauge){ CGauge->MaxValue=KolElem(Lst->Inst); CGauge->Progress=0; }
   fprintf(DGSFILE,"<h4 align=\"left\"><font color=\"#000099\">"
    "Список инструкций:</font></h4>\n");
   for(Root *I=Lst->Inst;I;I=I->Sled){
    if(CGauge) CGauge->Progress++;
    strcpy(InstName,I->Name); LowToUpp(InstName);
    if(I->First){
     fprintf(DGSFILE,"Строка №%4d: %s := ",I->Line,I->Name);
/*     if(!strcmp(I->Name,"ПОКАЗАТЬ")||!strcmp(I->Name,"ПЕЧАТАТЬ")||
        strstr(I->Name,"ВЫПОЛНИЛ")||strstr(I->Name,"РИСУН")||
        !strcmp(I->Name,"РАБОТА"))
      ReadAllRight(I->First,DGSFILE,false,false);*/
//     if((strstr(InstName,"ОКАЗАТЬ")==InstName+1)||
//        (strstr(InstName,"ЕЧАТАТЬ")==InstName+1)||
      if((strstr(InstName,"ЫПОЛНИЛ")==InstName+1)||
        (strstr(InstName,"ИСУН")==InstName+1)||
        (strstr(InstName,"АБОТА")==InstName+1))
      ReadAllRight(I->First,DGSFILE,false,false);
     else
      ReadAllRight(I->First,DGSFILE,true,true);
     fputs(";<br>\n",DGSFILE);
    }
    else
     fprintf(DGSFILE,"Строка №%4d: %s;<br>\n",I->Line,I->Name);
  }}
  if(Lst->Oper){
   StatusBar->SimpleText="Диагностика операторов";
   if(CGauge){ CGauge->MaxValue=KolElem(Lst->Oper); CGauge->Progress=0; }
   fprintf(DGSFILE,"<h4 align=\"left\"><font color=\"#000099\">"
    "Список операторов:</font></h4>\n");
   for(Root *I=L->Oper;I;I=I->Sled){ int Ln=I->Line;
    if(CGauge) CGauge->Progress++;
    fprintf(DGSFILE,"Строка №%4d: %s : ",I->Line,I->Name);
    if(I->First&&(I->First->Line>Ln)) Ln=I->First->Line;
    strcpy(Inf,I->Name); LowToUpp(Inf);
    if(!strcmp(Inf,"ТЕНЗОР ИНЕРЦИИ"))
     ReadAllRight(I->First,DGSFILE,true,true);
    else
     ReadAllRight(I->First,DGSFILE,true,false);
    fputs(";<br>\n",DGSFILE);
  }}
  if(Lst->L){
   int i,j,k,N; Magazine *MArr=NULL;
   StatusBar->SimpleText="Диагностика линейных интерполяций";
   if(CGauge){ int K=0; Ipll *I;
    for(I=Lst->L;I;I=I->Sled) K++;
    CGauge->MaxValue=K; CGauge->Progress=0;
   }
   fprintf(DGSFILE,"<h4 align=\"left\"><font color=\"#000099\">"
    "%s</font></h4>\n",LBeg->L->Sled?"Линейные интерполяции":"Линейная интерполяция");
   for(Ipll *i=LBeg->L;i;i=i->Sled){ int j;
    if(CGauge) CGauge->Progress++;
    if(!i->A->N){ TakeMagazine(&MArr,i->A->Name); continue; }
    fputs("<table width=\"100%\" border=\"1\">\n",DGSFILE);
    fputs("<tr>\n",DGSFILE);
    fputs(" <td colspan=\"8\">",DGSFILE);
    fprintf(DGSFILE,"<div align=\"center\"><b>ipl(%s, ",i->A->Name);
    PrintForm(false,DGSFILE,NULL,i->F);
    fputs(")</b></div>\n </td>\n",DGSFILE);
    fputs("</tr>\n",DGSFILE);
    fputs("<tr>\n",DGSFILE);
    for(j=0;j<4;j++){
     fputs(" <td><div align=\"center\"><b>",DGSFILE);
     PrintForm(false,DGSFILE,NULL,i->F);
     fputs("</b></div></td>",DGSFILE);
     fprintf(DGSFILE," <td><div align=\"center\"><b>%s</b></div></td>\n",i->A->Name);
    }
    fputs("</tr>\n",DGSFILE);
    fputs("<tr>\n",DGSFILE);
    for(int j=0;j<i->A->N;j+=2){
     if(j&&!(j%8)){
      fputs("</tr>\n",DGSFILE); fputs("<tr>\n",DGSFILE);
     }
     fprintf(DGSFILE," <td>%13g</td><td>%13g</td>\n",i->A->A[j],i->A->A[j+1]);
    }
    fputs("<tr>\n",DGSFILE);
    fputs("</table>",DGSFILE);
   }
   if(MArr){
    if(KolElem(MArr)>1){ char *inf=Inf;
     inf+=sprintf(Inf,"Не заданы массивы, на которые есть ссылки в линейных интерполяциях:\n"
      "%s",MArr->S);
     for(Magazine *m=MArr->Sled;m;m=m->Sled) inf+=sprintf(inf,", %s",m->S);
    }
    else
     sprintf(Inf,"Не задан массив (\"%s\"), на который есть ссылка в линейных интерполяциях",MArr->S);
    Error(0,0,true); Rez=false;
  }}
  if(Lst->S){
   int i,j,k,N; Magazine *MArr=NULL;
   StatusBar->SimpleText="Диагностика сплайнов";
   if(CGauge){ int K=0; Spln *S;
    for(S=Lst->S;S;S=S->Sled) K++;
    CGauge->MaxValue=K; CGauge->Progress=0;
   }
   fprintf(DGSFILE,"<h4 align=\"left\"><font color=\"#000099\">"
    "%s</font></h4>\n",LBeg->S->Sled?"Сплайны":"Сплайн");
   for(Spln *s=LBeg->S;s;s=s->Sled){
    if(CGauge) CGauge->Progress++;
    if(!s->A->N){ TakeMagazine(&MArr,s->A->Name); continue; }
    N=s->A->N/2-1;
    fputs("<table width=\"100%\" border=\"1\">\n",DGSFILE);
    fputs("<tr>\n",DGSFILE);
    fputs(" <td colspan=\"4\">\n",DGSFILE);
    fprintf(DGSFILE,"<div align=\"center\"><b>spln(%s, ",s->A->Name);
    PrintForm(false,DGSFILE,NULL,s->F);
    fprintf(DGSFILE,")=a+b*%s+c*%s^2+d*%s^3</b></div>\n",
     s->F.V->Name,s->F.V->Name,s->F.V->Name);
    fputs(" </td>\n",DGSFILE);
    fputs(" <td colspan=\"4\">\n",DGSFILE);
    fprintf(DGSFILE,"<div align=\"center\"><b>spln(%s, ",s->A->Name);
    PrintForm(false,DGSFILE,NULL,s->F);
    fprintf(DGSFILE,")=a+b*%s+c*%s^2+d*%s^3</b></div>\n",
     s->F.V->Name,s->F.V->Name,s->F.V->Name);
    fputs(" </td>\n",DGSFILE);
    fputs("</tr>\n",DGSFILE);
    fputs("<tr>\n",DGSFILE);
    fputs(" <td><div align=\"center\"><b>a</b></div></td>\n"
     "<td><div align=\"center\"><b>b</b></div></td>\n"
     "<td><div align=\"center\"><b>c</b></div></td>\n"
     "<td><div align=\"center\"><b>d</b></div></td>\n",DGSFILE);
    fputs(" <td><div align=\"center\"><b>a</b></div></td>\n"
     "<td><div align=\"center\"><b>b</b></div></td>\n"
     "<td><div align=\"center\"><b>c</b></div></td>\n"
     "<td><div align=\"center\"><b>d</b></div></td>\n",DGSFILE);
    fputs("</tr>\n",DGSFILE);
    for(i=0;i<N/2;i++){
     fputs("<tr>\n",DGSFILE);
     fprintf(DGSFILE,
      " <td>%13g</td>\n<td>%13g</td>\n<td>%13g</td>\n<td>%13g</td>\n",
      s->a[i],s->b[i],s->c[i],s->d[i]);
     fprintf(DGSFILE,
      " <td>%13g</td>\n<td>%13g</td>\n<td>%13g</td>\n<td>%13g</td>\n",
      s->a[i+N/2],s->b[i+N/2],s->c[i+N/2],s->d[i+N/2]);
     fputs("</tr>\n",DGSFILE);
    }
    if(N%2){
     fputs("<tr>\n",DGSFILE);
     fprintf(DGSFILE,
      " <td>&nbsp;</td>\n<td>&nbsp;</td>\n<td>&nbsp;</td>\n<td>&nbsp;</td>\n");
     fprintf(DGSFILE,
      " <td>%13g</td>\n<td>%13g</td>\n<td>%13g</td>\n<td>%13g</td>\n",
      s->a[i+N/2],s->b[i+N/2],s->c[i+N/2],s->d[i+N/2]);
     fputs("</tr>\n",DGSFILE);
    }
    fputs("</table>",DGSFILE);
   }
   if(MArr){
    if(KolElem(MArr)>1){ char *inf=Inf;
     inf+=sprintf(Inf,"Не заданы массивы, на которые есть ссылки в сплайнах:\n"
      "%s",MArr->S);
     for(Magazine *m=MArr->Sled;m;m=m->Sled) inf+=sprintf(inf,", %s",m->S);
    }
    else
     sprintf(Inf,"Не задан массив (\"%s\"), на который есть ссылки в сплайнах",MArr->S);
    Error(0,0,true); Rez=false;
  }}
  fprintf(DGSFILE,"<h4 align=\"left\"><font color=\"#000099\">"
   "Список переменных:</font></h4>\n");
  StatusBar->SimpleText="Диагностика переменных";
//вернем переменным интегрирования и аргументам функций поиска корней значения Pust
  for(Intg *I=L->I;I;I=I->Sled) I->x->Znach.C=Pust;
  for(Solu *c=L->C;c;c=c->Sled) c->x->Znach.C=Pust;
  if(CGauge){
   int K=0; for(Vary *V=Lst->V;V;V=V->Sled) K++; CGauge->MaxValue=K;
   CGauge->Progress=0;
  }
  for(Vary *V=Lst->V;V;V=V->Sled){
   if(CGauge) CGauge->Progress++;
   sprintf(S,"%s",V->Name); PrintForm(false,DGSFILE,S,V->Znach);
   fputs("<br>\n",DGSFILE); for(int i=0;i<5;i++) KO[i]=0;
   if(PRINTOP){
    ClearOper(); PrintOper(false,DGSFILE,V->Znach,KO);
    fputs("<br>\n",DGSFILE);
   }
   if(!(V->Sled)) VLast=V;
  }
  if(CGauge) CGauge->Progress=0;
  sprintf(Inf,"%s: %s",HeadVersion[0]?HeadVersion:HeadWork,Autor);
  StatusBar->SimpleText=Inf;
  if(Lst->Sled)
   fprintf(DGSFILE,"<h4 align=\"left\"><font color=\"#000099\">"
    "Лист окончен.</font></h4>\n");
 }
 LpodvD=fprintf(DGSFILE,
  "<hr><font face=\"Georgia, Times New Roman, Times, serif\""
  " color=\"#326464\">&copy; %s, %d<br></font>\n",Autor,NYear+1900);
 LpodvD+=fprintf(DGSFILE,"</body>\n</html>\n");
 fclose(DGSFILE);
 return Rez;
}
//---------------------------------------------------------------------------
void Furie(double **FF,int n,int Kt,int Kg,double **Ac,double **As,double *U){
//FF - массив n*Kt (Kt дискретных значений n переменных),
//Kg - число гармоник разложения ряда фурье,
//Ac,As - массивы n*(Kg+1) (0,1,...,Kg гармоники для каждой их n переменных)
//U - рабочий массив размера Kt
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
void TablToReport(char *Name,char *NameArg,int CountTabl,double Tn,double Tk,
  bool Godograf,void (*Funcs)(double,Magazine *,double *),Magazine **NamesInclude,
  Magazine **NamesTAB,Magazine **NamesSpline,Magazine **NamesFurie,
  int Lpodv,char *Autor){
//отабличиваем функции, Name=x|y(x)|z(x,y)|Заголовок(x,y,z,...)
//в первом и четвертом случае Godograf=false,  во втором и третьем - true
//CountTabl - число значений функций, Tn,Tk - диапазон изменения аргумента t,
//Funcs(double t,Magazine *M,double *Z) - процедура расчета массива
//функций Z(t) массива переменных M; NamesInclude, - магазин уже
//включенных в отчет таблиц; Lpodv - размер подвала отчета
//в строках; Autor - авторство задачи
 Magazine *Names=NULL,*m; //bool Period=false;
 char **Format,*s=Name,*s0,*t,*e,Val[256],Head[256],File[256],Form[16],B,Arg[256];
 int i,j,k,*P,*D,*C,*L,p,d,c,l,N,n,W,Wmax=96,Kg=(CountTabl-1)/2; FILE *F;
 double *x,*y,*X=(double *)calloc(CountTabl,sizeof(double)),
  **Y=(double **)calloc(CountTabl,SzV),Step=(Tk-Tn)/(CountTabl-1),w,
  *U=(double *)calloc(CountTabl,SzD);
 strcpy(File,Name);
 strcpy(Arg,NameArg); Grek(Arg);
 if(!(s0=strchr(s,'('))) TakeMagazine(&Names,s);
 else{
  if(Godograf){
   *s0++='\0'; m=TakeMagazine(&Names,s); s=s0;
   if(s0=strchr(s,',')){
    *s0++='\0'; m->Sled=(Magazine *)calloc(1,sizeof(Magazine));
    m->Sled->S=strcpy((char *)calloc(strlen(s)+1,SzC),s); s=s0; m=m->Sled;
   }
   if(s0=strchr(s,')')){
    *s0='\0'; m->Sled=(Magazine *)calloc(1,sizeof(Magazine));
    m->Sled->S=strcpy((char *)calloc(strlen(s)+1,SzC),s);
  }}
  else{
   for(s=s0+1;*s0&&((s0=strchr(s,','))||(s0=strchr(s,')')));s=s0){
    B=*s0; *s0='\0';
    if(Names){
     m->Sled=(Magazine *)calloc(1,sizeof(Magazine));
     m->Sled->S=strcpy((char *)calloc(strlen(s)+1,SzC),s); m=m->Sled;
    }
    else m=TakeMagazine(&Names,s);
    *s0++=B;
 }}}
 n=KolElem(Names);
 for(i=0;i<CountTabl;i++) Y[i]=(double *)calloc(n,SzD);
 Format=(char **)calloc(n+1,SzV);
 for(i=0;i<=n;i++) Format[i]=(char *)calloc(256,SzC);
 P=(int *)calloc(n+1,SzI); D=(int *)calloc(n+1,SzI);
 C=(int *)calloc(n+1,SzI); L=(int *)calloc(n+1,SzI);
 for(i=0,x=X;i<CountTabl;i++){
  *x=Tn+Step*i; Funcs(*x,Names,y=Y[i]);
  l=sprintf(Val,"%g",*x++);
  if(!(e=strchr(Val,'e'))&&!(e=strchr(Val,'E'))) p=0;
  else{ p=strlen(e+2); if(p>P[n]) P[n]=p; *e='\0'; }
  if(!(t=strchr(Val,'.'))) d=0; else{ d=strlen(t+1); if(d>D[n]) D[n]=d; }
  c=l; if(d) c-=d+1; if(p) c-=p+2; if(c>C[n]) C[n]=c;
  if(P[n])
   if(D[n]) sprintf(Format[n],"%%%d.%de",L[n]=max(L[n],D[n]+P[n]+5),D[n]);
   else     sprintf(Format[n],"%%%d.%de",L[n]=max(L[n],C[n]+P[n]+2),C[n]);
  else
   if(D[n]) sprintf(Format[n],"%%%d.%df",L[n]=max(L[n],C[n]+D[n]+2),D[n]);
   else     sprintf(Format[n],"%%%d.0f",L[n]=max(L[n],C[n]));
  if(L[n]<11) L[n]=11; W=L[n]+2;
  for(j=0;j<n;j++){
   l=sprintf(Val,"%g",*y++);
   if(!(e=strchr(Val,'e'))&&!(e=strchr(Val,'E'))) p=0;
   else{ p=strlen(e+2); if(p>P[j]) P[j]=p; *e='\0'; }
   if(!(t=strchr(Val,'.'))) d=0; else{ d=strlen(t+1); if(d>D[j]) D[j]=d; }
   c=l; if(d) c-=d+1; if(p) c-=p+2; if(c>C[j]) C[j]=c;
   if(P[j])
    if(D[j]) sprintf(Format[j],"%%%d.%de",L[j]=max(L[j],D[j]+P[j]+5),D[j]);
    else     sprintf(Format[j],"%%%d.%de",L[j]=max(L[n],C[j]+P[j]+2),C[j]);
   else
    if(D[j]) sprintf(Format[j],"%%%d.%df",L[j]=max(L[j],C[j]+D[j]+2),D[j]);
    else     sprintf(Format[j], "%%%d.0f",L[j]=max(L[j],C[j]));
   W+=L[j]+1;
 }}
 N=Wmax/W; if(!N) N=1;
 if(HTMFILE=fopen(HtmFile,"r+")){
  if(!fseek(HTMFILE,-Lpodv,2)){
   if(!*NamesInclude&&!*NamesTAB&&!*NamesSpline&&!*NamesFurie)
    fprintf(HTMFILE,
     "<h4 align=\"left\"><font color=\"#009900\">"
     "%d. Избранные графики и таблицы:</font></h4>\n",PUNKT++);
   TakeMagazine(NamesTAB,Name);
   if(strchr(Name,'(')) sprintf(Head,Name);
   else sprintf(Head,"%s(%s)",Name,NameArg);
   Grek(Head);
   fprintf(HTMFILE,
    "<h5 align=\"left\"><font color=\"#000099\">"
    "Таблица значений графика \"%s\"</font></h5>\n",Head);
   fputs("<p align=\"left\"><table width=\"100%\" border=\"1\""
    " height=\"0\" cellspacing=\"2\" cellpadding=\"4\">\n",HTMFILE);
   fputs("<tr>\n",HTMFILE);
   for(i=0;i<N;i++){
    fprintf(HTMFILE,"<th>%s</th>\n",Arg);
    for(j=0,m=Names;j<n;j++,m=m->Sled){
     sprintf(Inf,m->S); Grek(Inf);
     fprintf(HTMFILE,"<th>%s</th>\n",Inf);
   }}
   fputs("</tr>\n",HTMFILE);
   for(i=0,x=X;i<CountTabl;){
    fputs("<tr>\n",HTMFILE);
    for(k=0;k<N;k++){
     sprintf(Inf,Format[n],*x++); y=Y[i];
     fputs("<td align=\"right\">\n",HTMFILE);
     for(s=Inf;*s;s++)
      if(*s==' ') fputs("&nbsp;",HTMFILE); else fputc(*s,HTMFILE);
     fputs("</td>\n",HTMFILE);
     for(j=0;j<n;j++){
      sprintf(Inf,Format[j],*y++);
      fputs("<td align=\"right\">\n",HTMFILE);
      for(s=Inf;*s;s++)
       if(*s==' ') fputs("&nbsp;",HTMFILE); else fputc(*s,HTMFILE);
      fputs("</td>\n",HTMFILE);
     }
     i++;
     if(i==CountTabl){
      for(k++;k<N;k++)
       for(j=0;j<n+1;j++) fputs("<td>&nbsp;</td>\n",HTMFILE);
      break;
    }}
    fputs("</tr>\n",HTMFILE);
   }
   fputs("</table>\n</p>",HTMFILE);
   fprintf(HTMFILE,"<hr><font face=\"Georgia, Times New Roman, Times, serif\""
    " color=\"#326464\">&copy; %s, %d<br></font>\n",Autor,NYear+1900);
   fprintf(HTMFILE,"</body>\n</html>\n");
  }
  fseek(HTMFILE,0,2);
  fclose(HTMFILE);
 }
 sprintf(Inf,"%s\\%s.TXT",CurrDir,File);
 for(int i=1;!access(Inf,0);i++){
  if(*CurrDir) sprintf(Inf,"%s\\%s№%d.TXT",CurrDir,File,i);
  else sprintf(Inf,"%s№%d.TXT",File,i);
 }
 strcpy(File,Inf);
 if(F=fopen(File,"w")){
  int l=strlen(::L->Time->Name),LL=atoi(Format[n]+1)+1,f=(LL+l)/2; char Formt[16];
  sprintf(Formt,"%%%ds",f);
  fprintf(F,Formt,::L->Time->Name);
  for(j=0,m=Names;j<n;j++,m=m->Sled){
   f=(LL-l)/2; l=strlen(m->S); LL=atoi(Format[j]+1)+1; f+=(LL+l)/2+1;
   sprintf(Formt,"%%%ds",f);
   fprintf(F,Formt,m->S);
  }
  fputc('\n',F);
  for(i=0,x=X;i<CountTabl;i++){
   fprintf(F,Format[n],*x++); fputc(' ',F);
   for(j=0,y=Y[i];j<n;j++){
    fprintf(F,Format[j],*y++); fputc(' ',F);
   }
   fputc('\n',F);
  }
  fclose(F);
 }
 free(U); free(P); free(D); free(C); free(L);
 for(i=0;i<CountTabl;i++) free(Y[i]); free(Y); free(X);
 for(i=0;i<=n;i++) free(Format[i]); free(Format);
 DelAllMagazine(&Names);
 sprintf(Inf,"График сохранен в виде таблицы в текстовом файле\n\"%s\".\n"
  "Его можно просмотреть и отредактировать соответствующим\n"
  "текстовым редактором, вставить в документ WORD и т.д.",File);
 Application->MessageBox(Inf,"Внимание!",MB_OK|MB_ICONWARNING);
}
//---------------------------------------------------------------------------
void SplToReport(char *Name,int CountTabl,double Tn,double Tk,bool Godograf,
  void (*Funcs)(double,Magazine *,double *),Magazine **NamesInclude,
  Magazine **NamesTAB,Magazine **NamesSpline,Magazine **NamesFurie,
  int Lpodv,char *Autor,TCGauge *CGauge){
//построение интерполяционных формул для графика функции,
//Name=x|y(x)|z(x,y)|Заголовок(x,y,z,...)
//в первом и четвертом случае Godograf=false,  во втором и третьем - true
//CountTabl - дискретность функций, Tn,Tk - диапазон изменения аргумента t,
//Funcs(double t,Magazine *M,double *Z) - процедура расчета массива
//функций Z(t) переменных M; NamesInclude - магазин уже
//включенных в отчет формул; Lpodv - размер подвала отчета
//в строках; Autor - авторство задачи
 Magazine *Names=NULL,*m,*Files=NULL; FILE *F,*Ft;
 char **Format,*s=Name,*s0,*t,*e,Val[256],Head[256],File[256],*LastFile,Dir[256];
 int i,j,k,*P,*D,*C,*L,p,d,c,l,N,n,W,Wmax=96,
  Kinst=KolInst(TextFromFile(WORKDIR,"EditKiDyM",111))+
   KolInst(TextFromFile(WORKDIR,"EditKiDyM",159));
 double *x,*y,*X=(double *)calloc(CountTabl,SzD),
  **Ys=(double **)calloc(CountTabl,SzV),Step=(Tk-Tn)/(CountTabl-1);
 strcpy(Dir,InpFile); if(s0=strrchr(Dir,'.')) *s0='\0';
 sprintf(File,"%s_S.txt",Dir);
 if(!access(File,0)){
  LastFile=TakeMagazine(&Files,File)->S;
  sprintf(File,"%s_S№%d.txt",Dir,i=1);
  while(!access(File,0)){
   LastFile=TakeMagazine(&Files,File)->S;
   sprintf(File,"%s_S№%d.txt",Dir,++i);
  }
  if((i=KolElem(Files))>1)
   sprintf(Inf,"Уничтожить их и писать результат в файл:\n<%s> - <Да>\n"
    "Сохранить их все и писать в новый файл:\n<%s> - <Нет>\n"
    "Продолжать писать в последний из них:\n<%s> - <Отмена>",
    Files->S,File,LastFile);
  else
   sprintf(Inf,"Заменить его и писать результат в файл:\n<%s> - <Да>\n"
    "Сохранить его и писать в новый файл:\n<%s> - <Нет>\n"
    "Продолжать писать в этот файл:\n<%s> - <Отмена>",Files->S,File,LastFile);
  switch(Application->MessageBox(Inf,
         i>1?"Обнаружены файлы со сплайнами":"Обнаружен файл со сплайном",
         MB_YESNOCANCEL|MB_ICONQUESTION)){
   case ID_YES:
    for(m=Files;m;m=m->Sled) if(!access(m->S,0)) unlink(m->S);
    F=fopen(Files->S,"w"); strcpy(File,Files->S); break;
   case ID_NO: F=fopen(File,"w"); break;
   case ID_CANCEL: F=fopen(LastFile,"a"); strcpy(File,LastFile); break;
 }}
 else F=fopen(File,"w");
 DelAllMagazine(&Files);
// if(s0=strrchr(Dir,'\\')) *s0='\0';
 if(!(s0=strchr(s,'('))) TakeMagazine(&Names,s);
 else{
  if(Godograf){
   *s0++='\0'; m=TakeMagazine(&Names,s); s=s0;
   if(s0=strchr(s,',')){
    *s0++='\0'; m->Sled=(Magazine *)calloc(1,sizeof(Magazine));
    m->Sled->S=strcpy((char *)calloc(strlen(s)+1,SzC),s); s=s0; m=m->Sled;
   }
   if(s0=strchr(s,')')){
    *s0='\0'; m->Sled=(Magazine *)calloc(1,sizeof(Magazine));
    m->Sled->S=strcpy((char *)calloc(strlen(s)+1,SzC),s);
  }}
  else{
   for(s=s0+1;*s0&&((s0=strchr(s,','))||(s0=strchr(s,')')));s=s0){
    *s0++='\0';
    if(Names){
     m->Sled=(Magazine *)calloc(1,sizeof(Magazine));
     m->Sled->S=strcpy((char *)calloc(strlen(s)+1,SzC),s); m=m->Sled;
    }
    else m=TakeMagazine(&Names,s);
 }}}
 n=KolElem(Names);
 for(i=0;i<CountTabl;i++) Ys[i]=(double *)calloc(n,SzD);
 Format=(char **)calloc(n+1,SzV);
 for(i=0;i<=n;i++) Format[i]=(char *)calloc(256,SzC);
 P=(int *)calloc(n+1,SzI); D=(int *)calloc(n+1,SzI);
 C=(int *)calloc(n+1,SzI); L=(int *)calloc(n+1,SzI);
 if(CGauge){ CGauge->MaxValue=CountTabl; CGauge->Progress=0; }
 for(i=0,x=X;i<CountTabl;i++){
  if(CGauge) CGauge->Progress++;
  *x=Tn+Step*i;
  Funcs(*x,Names,y=Ys[i]);
  l=sprintf(Val,"%g",*x++);
  if(!(e=strchr(Val,'e'))&&!(e=strchr(Val,'E'))) p=0;
  else{ p=strlen(e+2); if(p>P[n]) P[n]=p; *e='\0'; }
  if(!(t=strchr(Val,'.'))) d=0; else{ d=strlen(t+1); if(d>D[n]) D[n]=d; }
  c=l; if(d) c-=d+1; if(p) c-=p+2; if(c>C[n]) C[n]=c;
  if(P[n])
   if(D[n]) sprintf(Format[n],"%%%d.%de",L[n]=max(L[n],D[n]+P[n]+5),D[n]);
   else     sprintf(Format[n],"%%%d.%de",L[n]=max(L[n],C[n]+P[n]+2),C[n]);
  else
   if(D[n]) sprintf(Format[n],"%%%d.%df",L[n]=max(L[n],C[n]+D[n]+2),D[n]);
   else     sprintf(Format[n],"%%%d.0f",L[n]=max(L[n],C[n]));
  if(L[n]<11) L[n]=11; W=L[n]+2;
  for(j=0;j<n;j++){
   l=sprintf(Val,"%g",*y++);
   if(!(e=strchr(Val,'e'))&&!(e=strchr(Val,'E'))) p=0;
   else{ p=strlen(e+2); if(p>P[j]) P[j]=p; *e='\0'; }
   if(!(t=strchr(Val,'.'))) d=0; else{ d=strlen(t+1); if(d>D[j]) D[j]=d; }
   c=l; if(d) c-=d+1; if(p) c-=p+2; if(c>C[j]) C[j]=c;
   if(P[j])
    if(D[j]) sprintf(Format[j],"%%%d.%de",L[j]=max(L[j],D[j]+P[j]+5),D[j]);
    else     sprintf(Format[j],"%%%d.%de",L[j]=max(L[n],C[j]+P[j]+2),C[j]);
   else
    if(D[j]) sprintf(Format[j],"%%%d.%df",L[j]=max(L[j],C[j]+D[j]+2),D[j]);
    else     sprintf(Format[j], "%%%d.0f",L[j]=max(L[j],C[j]));
   W+=L[j]+1;
 }}
 if(CGauge) CGauge->Progress=0;
 N=Wmax/W; if(!N) N=1;
 if(HTMFILE=fopen(HtmFile,"r+")){
  if(!fseek(HTMFILE,-Lpodv,2)){
   if(!*NamesInclude&&!*NamesTAB&&!*NamesSpline&&!*NamesFurie)
    fprintf(HTMFILE,
     "<h4 align=\"left\"><font color=\"#009900\">"
     "%d. Избранные графики, интерполяции и таблицы:</font></h4>\n",PUNKT++);
   TakeMagazine(NamesSpline,Name);
   if(strchr(Name,'(')) sprintf(Head,Name);
   else sprintf(Head,"%s(%s)",Name,::L->Time->Name);
   fprintf(HTMFILE,
    "<h5 align=\"left\"><font color=\"#000099\"><b>"
    "Сплайн-интерполяция:</b></font></h5>\n"
    "<font face=\"Courier New, Courier, mono\" size=\"-1\">");
   for(i=0,m=Names;i<n;i++,m=m->Sled){
    fprintf(HTMFILE,"%s=spln(%s_arr,%s);<br>\n",
     m->S,m->S,::L->Time->Name);
    if(Kinst>1) sprintf(Inf,"%s\\%s№%d.arr",Dir,m->S,Kinst);
    else sprintf(Inf,"%s\\%s.arr",Dir,m->S);
    if(CountTabl>=100)//массив пишем в другой файл
     fprintf(HTMFILE,"%s_arr[%d]=%s;<br>\n",m->S,2*CountTabl,Inf);
    else{//массив пишем туда же
     fprintf(HTMFILE,"%s_arr[%d]=<br>\n",m->S,2*CountTabl);
     for(j=0,l=0;j<CountTabl-1;j++){
      if(l+L[n]+L[i]+2>Wmax){ l=0; fputs("<br>\n",HTMFILE); }
      l+=sprintf(Inf,Format[n],X[j])+1;
      for(s=Inf;*s;s++)
       if(*s==' ') fputs("&nbsp;",HTMFILE); else fputc(*s,HTMFILE);
      fputc(',',HTMFILE);
      l+=sprintf(Inf,Format[i],Ys[j][i])+1;
      for(s=Inf;*s;s++)
       if(*s==' ') fputs("&nbsp;",HTMFILE); else fputc(*s,HTMFILE);
      fputc(',',HTMFILE);
     }
     if(l+L[n]+L[i]+1>Wmax){ l=0; fputs("<br>\n",HTMFILE); }
     l+=sprintf(Inf,Format[n],X[j])+1;
     for(s=Inf;*s;s++)
      if(*s==' ') fputs("&nbsp;",HTMFILE); else fputc(*s,HTMFILE);
     fputc(',',HTMFILE);
     l+=sprintf(Inf,Format[i],Ys[j][i]);
     for(s=Inf;*s;s++)
      if(*s==' ') fputs("&nbsp;",HTMFILE); else fputc(*s,HTMFILE);
     fputs(";<br>\n",HTMFILE);
   }}
   fputs("</font>\n",HTMFILE);
   fprintf(HTMFILE,
    "<hr><font face=\"Georgia, Times New Roman, Times, serif\""
    " color=\"#326464\">&copy; %s, %d<br></font>\n",Autor,NYear+1900);
   fprintf(HTMFILE,"</body>\n</html>\n");
  }
  fseek(HTMFILE,0,2); fclose(HTMFILE);
 }
 if(F){
  char CurDir[256]; getcwd(CurDir,255);
  if(strchr(Name,'(')) sprintf(Head,Name);
  else sprintf(Head,"%s(%s)",Name,::L->Time->Name);
  for(i=0,m=Names;i<n;i++,m=m->Sled){
   sprintf(Inf,"%s=spln(%s_arr,%s);\n",m->S,m->S,::L->Time->Name);
   Fputs(Inf,F);
   Ft=F;
   if(Kinst>1) sprintf(Inf,"%s\\%s№%d.arr",Dir,m->S,Kinst);
   else sprintf(Inf,"%s\\%s.arr",Dir,m->S);
   mkdir(Dir); chdir(CurDir);
   if((CountTabl<100)||!(F=fopen(Inf,"wb"))){
    F=Ft; fprintf(F,"%s_arr[%d]=\n",m->S,2*CountTabl); //массив пишем туда же
    for(j=0,l=0;j<CountTabl-1;j++){
     if(l+L[n]+L[i]+2>Wmax){ l=0; fputs("\n",F); }
     l+=sprintf(Inf,Format[n],X[j])+1;
     for(s=Inf;*s;s++)
      if(*s==' ') fputs(" ",F); else fputc(*s,F);
     fputc(',',F);
     l+=sprintf(Inf,Format[i],Ys[j][i])+1;
     for(s=Inf;*s;s++)
      if(*s==' ') fputs(" ",F); else fputc(*s,F);
     fputc(',',F);
    }
    if(l+L[n]+L[i]+1>Wmax){ l=0; fputs("\n",F); }
    l+=sprintf(Inf,Format[n],X[j])+1;
    for(s=Inf;*s;s++)
     if(*s==' ') fputs(" ",F); else fputc(*s,F);
    fputc(',',F);
    l+=sprintf(Inf,Format[i],Ys[j][i]);
    for(s=Inf;*s;s++)
     if(*s==' ') fputs(" ",F); else fputc(*s,F);
    fputs(";\n",F);
    if(F!=Ft){ fclose(F); F=Ft; }
   }
   else{ int K; char Buf[256];
    sprintf(Buf,"%s_arr[%s];\n",m->S,Inf);//массив пишем в другой файл
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
 sprintf(Inf,"сохранен в файле\n\"%s\".\n"
  "Этот файл можно просмотреть и отредактировать\n"
  "любым редактором, сплайн вставить в КИДИМ-файл для расчетов и т.д.",
  File);
 Application->MessageBox(Inf,"Сплайн записан в отчет и",MB_OK);
 free(P); free(D); free(C); free(L);
 for(i=0;i<CountTabl;i++) free(Ys[i]); free(Ys); free(X);
 for(i=0;i<=n;i++) free(Format[i]); free(Format);
 DelAllMagazine(&Names);
}
//---------------------------------------------------------------------------
void SplGdrfToReport(char *Name,int CountTabl,double Tn,double Tk,
  void (*Funcs)(double,Magazine *,double *),Magazine **NamesInclude,
  Magazine **NamesTAB,Magazine **NamesSpline,Magazine **NamesFurie,
  int Lpodv,char *Autor,TCGauge *CGauge){
//построение интерполяционных формул для графика функции Name=y(x)
//CountTabl - дискретность функций, Tn,Tk - диапазон изменения аргумента t,
//Funcs(double t,Magazine *M,double *Z) - процедура расчета
//функций x(t) и y(t); NamesInclude,... - магазин уже
//включенных в отчет формул; Lpodv - размер подвала отчета
//в строках; Autor - авторство задачи
 Magazine *Names=NULL,*Files=NULL; FILE *F,*Ft;
 char **Format,*s=Name,*s0,*t,*e,Val[256],Head[256],File[256],*LastFile,Dir[256];
 int i,j,k,*P,*D,*C,*L,p,d,c,l,N,W=0,Wmax=96,K=0,
  Kinst=KolInst(TextFromFile(WORKDIR,"EditKiDyM",111))+
   KolInst(TextFromFile(WORKDIR,"EditKiDyM",159));
 double *x,*y,*X,Step=(Tk-Tn)/(CountTabl-1),**Ys;
 X=(double *)calloc(CountTabl,SzD);
 Ys=(double **)calloc(CountTabl,SzV);
 for(int i=0;i<CountTabl;i++) Ys[i]=(double *)calloc(2,SzD);
 s0=strchr(s=Name,'('); *s0++='\0'; TakeMagazine(&Names,s); s=s0;
 if(s0=strchr(s,')')){
  *s0='\0'; Names->Sled=(Magazine *)calloc(1,sizeof(Magazine));
  Names->Sled->S=strcpy((char *)calloc(strlen(s)+1,SzC),s);
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
   Application->MessageBox("СПЛАЙН НЕ МОЖЕТ БЫТЬ ПОСТРОЕН!",
    "Аргумент сплайна должен быть монотонно\n"
    "убывающий или растущей последовательностью",MB_ICONERROR|MB_OK);
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
 strcpy(Dir,InpFile); if(s0=strrchr(Dir,'.')) *s0='\0';
 sprintf(File,"%s_S.txt",Dir);
 if(!access(File,0)){
  LastFile=TakeMagazine(&Files,File)->S;
  sprintf(File,"%s_S№%d.txt",Dir,i=1);
  while(!access(File,0)){
   LastFile=TakeMagazine(&Files,File)->S;
   sprintf(File,"%s_S№%d.txt",Dir,++i);
  }
  if((i=KolElem(Files))>1)
   sprintf(Inf,"Уничтожить их и писать результат в файл:\n<%s> - <Да>\n"
    "Сохранить их все и писать в новый файл:\n<%s> - <Нет>\n"
    "Продолжать писать в последний из них:\n<%s> - <Отмена>",
    Files->S,File,LastFile);
  else
   sprintf(Inf,"Заменить его и писать результат в файл:\n<%s> - <Да>\n"
    "Сохранить его и писать в новый файл:\n<%s> - <Нет>\n"
    "Продолжать писать в этот файл:\n<%s> - <Отмена>",Files->S,File,LastFile);
  switch(Application->MessageBox(Inf,
         i>1?"Обнаружены файлы со сплайнами":"Обнаружен файл со сплайном",
         MB_YESNOCANCEL|MB_ICONQUESTION)){
   case ID_YES:
    for(Magazine *m=Files;m;m=m->Sled) if(!access(m->S,0)) unlink(m->S);
    F=fopen(Files->S,"w"); strcpy(File,Files->S); break;
   case ID_NO: F=fopen(File,"w"); break;
   case ID_CANCEL: F=fopen(LastFile,"a"); strcpy(File,LastFile); break;
 }}
 else F=fopen(File,"w");
 DelAllMagazine(&Files);
 Format=(char **)calloc(2,SzV);
 for(i=0;i<2;i++) Format[i]=(char *)calloc(256,SzC);
 P=(int *)calloc(2,SzI); D=(int *)calloc(2,SzI);
 C=(int *)calloc(2,SzI); L=(int *)calloc(2,SzI);
 for(i=0,x=X;i<CountTabl;i++){
  *x=Tn+Step*i; y=Ys[i];
  for(j=0;j<2;j++){
   l=sprintf(Val,"%g",*y++);
   if(!(e=strchr(Val,'e'))&&!(e=strchr(Val,'E'))) p=0;
   else{ p=strlen(e+2); if(p>P[j]) P[j]=p; *e='\0'; }
   if(!(t=strchr(Val,'.'))) d=0; else{ d=strlen(t+1); if(d>D[j]) D[j]=d; }
   c=l; if(d) c-=d+1; if(p) c-=p+2; if(c>C[j]) C[j]=c;
   if(P[j])
    if(D[j]) sprintf(Format[j],"%%%d.%de",L[j]=max(L[j],D[j]+P[j]+5),D[j]);
    else     sprintf(Format[j],"%%%d.%de",L[j]=max(L[j],C[j]+P[j]+2),C[j]);
   else
    if(D[j]) sprintf(Format[j],"%%%d.%df",L[j]=max(L[j],C[j]+D[j]+2),D[j]);
    else     sprintf(Format[j], "%%%d.0f",L[j]=max(L[j],C[j]));
   W+=L[j]+1;
 }}
 N=Wmax/W; if(!N) N=1;
 if(HTMFILE=fopen(HtmFile,"r+")){
  if(!fseek(HTMFILE,-Lpodv,2)){
   if(!*NamesInclude&&!*NamesTAB&&!*NamesSpline&&!*NamesFurie)
    fprintf(HTMFILE,
     "<h4 align=\"left\"><font color=\"#009900\">"
     "%d. Избранные графики, интерполяции и таблицы:</font></h4>\n",PUNKT++);
   TakeMagazine(NamesSpline,Name);
   fprintf(HTMFILE,
    "<h5 align=\"left\"><font color=\"#000099\"><b>"
    "Сплайн-интерполяция:</b></font></h5>\n"
    "<font face=\"Courier New, Courier, mono\" size=\"-1\">");
    fprintf(HTMFILE,"%s=spln(%s_arr,%s);<br>\n",
     Names->S,Names->S,Names->Sled->S);
    if(Kinst>1) sprintf(Inf,"%s\\%s№%d.arr",Dir,Names->S,Kinst);
    else sprintf(Inf,"%s\\%s.arr",Dir,Names->S);
    if(CountTabl>=100)//массив пишем в другой файл
     fprintf(HTMFILE,"%s_arr[%d]=%s;<br>\n",Names->S,2*CountTabl,Inf);
    else{//массив пишем туда же
     fprintf(HTMFILE,"%s_arr[%d]=<br>\n",Names->S,2*CountTabl);
     for(j=0,l=0;j<CountTabl-1;j++){
      if(l+L[0]+L[1]+2>Wmax){ l=0; fputs("<br>\n",HTMFILE); }
      l+=sprintf(Inf,Format[1],Ys[j][1])+1;
      for(s=Inf;*s;s++)
       if(*s==' ') fputs("&nbsp;",HTMFILE); else fputc(*s,HTMFILE);
      fputc(',',HTMFILE);
      l+=sprintf(Inf,Format[0],Ys[j][0])+1;
      for(s=Inf;*s;s++)
       if(*s==' ') fputs("&nbsp;",HTMFILE); else fputc(*s,HTMFILE);
      fputc(',',HTMFILE);
     }
     if(l+L[0]+L[1]+1>Wmax){ l=0; fputs("<br>\n",HTMFILE); }
     l+=sprintf(Inf,Format[0],Ys[j][1])+1;
     for(s=Inf;*s;s++)
      if(*s==' ') fputs("&nbsp;",HTMFILE); else fputc(*s,HTMFILE);
     fputc(',',HTMFILE);
     l+=sprintf(Inf,Format[1],Ys[j][0]);
     for(s=Inf;*s;s++)
      if(*s==' ') fputs("&nbsp;",HTMFILE); else fputc(*s,HTMFILE);
     fputs(";<br>\n",HTMFILE);
    }
   fputs("</font>\n",HTMFILE);
   fprintf(HTMFILE,
    "<hr><font face=\"Georgia, Times New Roman, Times, serif\""
    " color=\"#326464\">&copy; %s, %d<br></font>\n",Autor,NYear+1900);
   fprintf(HTMFILE,"</body>\n</html>\n");
  }
  fseek(HTMFILE,0,2); fclose(HTMFILE);
 }
 if(F){
  char CurDir[256]; getcwd(CurDir,255);
  strcpy(Head,Name);
  sprintf(Inf,"%s=spln(%s_arr,%s);\n",Names->S,Names->S,Names->Sled->S);
  Fputs(Inf,F);
  Ft=F;
  if(Kinst>1) sprintf(Inf,"%s\\%s_%s№%d.arr",Dir,Names->S,Names->Sled->S,Kinst);
  else sprintf(Inf,"%s\\%s_%s.arr",Dir,Names->S,Names->Sled->S);
  mkdir(Dir); chdir(CurDir); F=fopen(Inf,"wb");
  if((CountTabl<100)||!F){
   F=Ft; fprintf(F,"%s_arr[%d]=\n",Names->S,2*CountTabl);//массив пишем туда же
   for(j=0,l=0;j<CountTabl-1;j++){
    if(l+L[0]+L[1]+2>Wmax){ l=0; fputs("\n",F); }
    l+=sprintf(Inf,Format[1],Ys[j][1])+1;
    for(s=Inf;*s;s++)
     if(*s==' ') fputs(" ",F); else fputc(*s,F);
    fputc(',',F);
    l+=sprintf(Inf,Format[0],Ys[j][0])+1;
    for(s=Inf;*s;s++)
     if(*s==' ') fputs(" ",F); else fputc(*s,F);
    fputc(',',F);
   }
   if(l+L[0]+L[1]+1>Wmax){ l=0; fputs("\n",F); }
   l+=sprintf(Inf,Format[1],Ys[j][1])+1;
   for(s=Inf;*s;s++)
    if(*s==' ') fputs(" ",F); else fputc(*s,F);
   fputc(',',F);
   l+=sprintf(Inf,Format[0],Ys[j][0]);
   for(s=Inf;*s;s++)
    if(*s==' ') fputs(" ",F); else fputc(*s,F);
   fputs(";\n",F);
   if(F!=Ft){ fclose(F); F=Ft; }
  }
  else{ int K=2*CountTabl; char Buf[256];
   sprintf(Buf,"%s_arr[%s];\n",Names->S,Inf);//массив пишем в другой файл
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
 sprintf(Inf,"сохранен в файле\n\"%s\".\n"
  "Этот файл можно просмотреть и отредактировать\n"
  "любым редактором, сплайн вставить в КИДИМ-файл для расчетов и т.д.",
  File);
 Application->MessageBox(Inf,"Сплайн записан в отчет и",MB_OK);
 free(P); free(D); free(C); free(L);
 for(i=0;i<CountTabl;i++) free(Ys[i]); free(Ys); free(X);
 for(i=0;i<2;i++) free(Format[i]); free(Format);
 DelAllMagazine(&Names);
}
//---------------------------------------------------------------------------
void FurieToReport(char *Name,int CountTabl,double Tn,double Tk,bool Godograf,
  void (*Funcs)(double,Magazine *,double *),Magazine **NamesInclude,
  Magazine **NamesTAB,Magazine **NamesSpline,Magazine **NamesFurie,
  int Lpodv,char *Autor,TCGauge *CGauge){
//построение интерполяционных формул для графика функции,
//Name=x|y(x)|z(x,y)|Заголовок(x,y,z,...)
//в первом и четвертом случае Godograf=false,  во втором и третьем - true
//CountTabl - дискретность функций, Tn,Tk - диапазон изменения аргумента t,
//Funcs(double t,Magazine *M,double *Z) - процедура расчета массива
//функций Z(t) переменных M; NamesInclude - магазин уже
//включенных в отчет формул; Lpodv - размер подвала отчета
//в строках; Autor - авторство задачи
 Magazine *Names=NULL,*m,*Files=NULL; FILE *F; bool Period=false;
 char **Format,*s=Name,*s0,*t,*e,Val[256],Head[256],Form1[80],Form2[80],
  Form3[80],File[256],*LastFile;
 int i,j,k,*P,*D,*C,*L,p,d,c,l,N,n,W,Wmax=96,Kg=(CountTabl-1)/2;
 double *x,*y,*X=(double *)calloc(CountTabl,SzD),**Ac,**As,**Fs,
  **Ys=(double **)calloc(CountTabl,SzV),Step=(Tk-Tn)/(CountTabl-1),
  *U=(double *)calloc(CountTabl,SzD);
 if(s0=strrchr(InpFile,'.')) *s0='\0';
 sprintf(File,"%s_F.txt",InpFile);
 if(!access(File,0)){
  LastFile=TakeMagazine(&Files,File)->S;
  sprintf(File,"%s_F№%d.txt",InpFile,i=1);
  while(!access(File,0)){
   LastFile=TakeMagazine(&Files,File)->S;
   sprintf(File,"%s_F№%d.txt",InpFile,++i);
  }
  if((i=KolElem(Files))>1)
   sprintf(Inf,"Уничтожить их и писать результат в файл:\n<%s> - <Да>\n"
    "Сохранить их все и писать в новый файл:\n<%s> - <Нет>\n"
    "Продолжать писать в последний из них:\n<%s> - <Отмена>",
    Files->S,File,LastFile);
  else
   sprintf(Inf,"Заменить его и писать результат в файл:\n<%s> - <Да>\n"
    "Сохранить его и писать в новый файл:\n<%s> - <Нет>\n"
    "Продолжать писать в этот файл:\n<%s> - <Отмена>",Files->S,File,LastFile);
  switch(Application->MessageBox(Inf,i>1?"Обнаружены файлы с Фурье-разложениями":
   "Обнаружен файл с Фурье-разложением",MB_YESNOCANCEL|MB_ICONQUESTION)){
   case ID_YES:
    for(m=Files;m;m=m->Sled) if(!access(m->S,0)) unlink(m->S);
    F=fopen(Files->S,"w"); strcpy(File,Files->S); break;
   case ID_NO: F=fopen(File,"w"); break;
   case ID_CANCEL: F=fopen(LastFile,"a"); strcpy(File,LastFile); break;
 }}
 else F=fopen(File,"w");
 DelAllMagazine(&Files);
 if(s0) *s0='.';
 if(!(s0=strchr(s,'('))) TakeMagazine(&Names,s);
 else{
  if(Godograf){
   *s0++='\0'; m=TakeMagazine(&Names,s); s=s0;
   if(s0=strchr(s,',')){
    *s0++='\0'; m->Sled=(Magazine *)calloc(1,sizeof(Magazine));
    m->Sled->S=strcpy((char *)calloc(strlen(s)+1,SzC),s); s=s0; m=m->Sled;
   }
   if(s0=strchr(s,')')){
    *s0='\0'; m->Sled=(Magazine *)calloc(1,sizeof(Magazine));
    m->Sled->S=strcpy((char *)calloc(strlen(s)+1,SzC),s);
  }}
  else{
   for(s=s0+1;*s0&&((s0=strchr(s,','))||(s0=strchr(s,')')));s=s0){
    *s0++='\0';
    if(Names){
     m->Sled=(Magazine *)calloc(1,sizeof(Magazine));
     m->Sled->S=strcpy((char *)calloc(strlen(s)+1,SzC),s); m=m->Sled;
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
 Format=(char **)calloc(n+1,SzV);
 for(i=0;i<=n;i++) Format[i]=(char *)calloc(256,SzC);
 P=(int *)calloc(n+1,SzI); D=(int *)calloc(n+1,SzI);
 C=(int *)calloc(n+1,SzI); L=(int *)calloc(n+1,SzI);
 if(CGauge){ CGauge->MaxValue=CountTabl; CGauge->Progress=0; }
 for(i=0,x=X;i<CountTabl;i++){
  if(CGauge) CGauge->Progress++;
  *x=Tn+Step*i;
  Funcs(*x,Names,y=Ys[i]);
  l=sprintf(Val,"%g",*x++);
  if(!(e=strchr(Val,'e'))&&!(e=strchr(Val,'E'))) p=0;
  else{ p=strlen(e+2); if(p>P[n]) P[n]=p; *e='\0'; }
  if(!(t=strchr(Val,'.'))) d=0; else{ d=strlen(t+1); if(d>D[n]) D[n]=d; }
  c=l; if(d) c-=d+1; if(p) c-=p+2; if(c>C[n]) C[n]=c;
  if(P[n])
   if(D[n]) sprintf(Format[n],"%%%d.%de",L[n]=max(L[n],D[n]+P[n]+5),D[n]);
   else     sprintf(Format[n],"%%%d.%de",L[n]=max(L[n],C[n]+P[n]+2),C[n]);
  else
   if(D[n]) sprintf(Format[n],"%%%d.%df",L[n]=max(L[n],C[n]+D[n]+2),D[n]);
   else     sprintf(Format[n],"%%%d.0f",L[n]=max(L[n],C[n]));
  if(L[n]<11) L[n]=11; W=L[n]+2;
  for(j=0;j<n;j++){
   l=sprintf(Val,"%g",*y++);
   if(!(e=strchr(Val,'e'))&&!(e=strchr(Val,'E'))) p=0;
   else{ p=strlen(e+2); if(p>P[j]) P[j]=p; *e='\0'; }
   if(!(t=strchr(Val,'.'))) d=0; else{ d=strlen(t+1); if(d>D[j]) D[j]=d; }
   c=l; if(d) c-=d+1; if(p) c-=p+2; if(c>C[j]) C[j]=c;
   W+=L[j]+1;
 }}
 for(j=0;j<n;j++){
  if(P[j])
   if(D[j]) sprintf(Format[j],"%%%d.%de",L[j]=max(L[j],D[j]+P[j]+5),D[j])-1;
   else     sprintf(Format[j],"%%%d.%de",L[j]=max(L[n],C[j]+P[j]+2),C[j])-1;
  else
   if(D[j]) sprintf(Format[j],"%%%d.%df",L[j]=max(L[j],C[j]+D[j]+2),D[j])-1;
   else     sprintf(Format[j], "%%%d.0f",L[j]=max(L[j],C[j]))-1;
 }
 if(CGauge) CGauge->Progress=0;
 for(i=0;i<CountTabl;){ for(j=0;j<n;){ Fs[j][i]=Ys[i][j]; j++; } i++; }
 if(Application->MessageBox(
  "Если считаете, что раскладываемая функция в ряд Фурье периодическая,\n"
  "то есть ее значения и производные на краях графика одинаковы,\n"
  "жмите - <Да>,\nиначе - <Нет>","Для повышения точности разложения, ответьте",
  MB_YESNO|MB_ICONQUESTION)==ID_YES)
   Period=true;
 Furie(Fs,n,Period?CountTabl-1:CountTabl,Kg,Ac,As,U);
// Furie(Fs,n,CountTabl,Kg,Ac,As,U);
 N=Wmax/W; if(!N) N=1;
 if(HTMFILE=fopen(HtmFile,"r+")){
  if(!fseek(HTMFILE,-Lpodv,2)){
   if(!*NamesInclude&&!*NamesTAB&&!*NamesFurie&&!*NamesSpline)
    fprintf(HTMFILE,
     "<h4 align=\"left\"><font color=\"#009900\">"
     "%d. Избранные графики, интерполяции и таблицы:</font></h4>\n",PUNKT++);
   TakeMagazine(NamesFurie,Name);
   if(strchr(Name,'(')) sprintf(Head,Name);
   else sprintf(Head,"%s(%s)",Name,::L->Time->Name);
   fprintf(HTMFILE,
    "<h5 align=\"left\"><font color=\"#000099\"><b>"
    "Разложение в ряд Фурье:</b>\n<br></font></h5>");
   for(i=0,m=Names;i<n;i++,m=m->Sled){
    if(Kg<10){
     sprintf(Form1,"%s*cos(  w*t)%%c%s*sin(  w*t)",Format[i],Format[i]);
     sprintf(Form2,  "%s*cos(%%1d*w*t)%%c%s*sin(%%1d*w*t)",Format[i],Format[i]);
     sprintf(Form3,"%%c%s*cos(%%1d*w*t)%%c%s*sin(%%1d*w*t)",Format[i],Format[i]);
    }
    else if(Kg<100){
     sprintf(Form1,"%s*cos(   w*t)%%c%s*sin(   w*t)",Format[i],Format[i]);
     sprintf(Form2,  "%s*cos(%%2d*w*t)%%c%s*sin(%%2d*w*t)",Format[i],Format[i]);
     sprintf(Form3,"%%c%s*cos(%%2d*w*t)%%c%s*sin(%%2d*w*t)",Format[i],Format[i]);
    }
    else if(Kg<1000){
     sprintf(Form1,"%s*cos(    w*t)%%c%s*sin(    w*t)",Format[i],Format[i]);
     sprintf(Form2,  "%s*cos(%%3d*w*t)%%c%s*sin(%%3d*w*t)",Format[i],Format[i]);
     sprintf(Form3,"%%c%s*cos(%%3d*w*t)%%c%s*sin(%%3d*w*t)",Format[i],Format[i]);
    }
    else{
     sprintf(Form1,"%s*cos(     w*t)%%c%s*sin(     w*t)",Format[i],Format[i]);
     sprintf(Form2,  "%s*cos(%%4d*w*t)%%c%s*sin(%%4d*w*t)",Format[i],Format[i]);
     sprintf(Form3,"%%c%s*cos(%%4d*w*t)%%c%s*sin(%%4d*w*t)",Format[i],Format[i]);
    }
    fprintf(HTMFILE,"<font face=\"Courier New, Courier, mono\" size=\"-1\">\n%s=",m->S);
    W=l=strlen(m->S)+1;
    fprintf(HTMFILE,"%0.6f%c<br>\n",Ac[i][0],Ac[i][1]>=0.0?'+':'-');
    for(j=0;j<l;j++) fputs("&nbsp;",HTMFILE);
    W+=sprintf(Inf,Form1,fabs(Ac[i][1]),As[i][1]>=0.0?'+':'-',fabs(As[i][1]));
    for(s=Inf;*s;s++)
     if(*s==' ') fputs("&nbsp;",HTMFILE); else fputc(*s,HTMFILE);
    for(k=2;k<Kg;k++){
     if(W+34>Wmax){ W=l;
      fprintf(HTMFILE,"%c<br>\n",Ac[i][k]>=0.0?'+':'-');
      for(j=0;j<l;j++) fputs("&nbsp;",HTMFILE);
      sprintf(Inf,Form2,fabs(Ac[i][k]),k,As[i][k]>=0.0?'+':'-',fabs(As[i][k]),k);
     }
     else
      sprintf(Inf,Form3,Ac[i][k]>=0.0?'+':'-',fabs(Ac[i][k]),k,
       As[i][k]>=0.0?'+':'-',fabs(As[i][k]),k);
     W+=34;
     for(s=Inf;*s;s++)
      if(*s==' ') fputs("&nbsp;",HTMFILE); else fputc(*s,HTMFILE);
    }
    if(W+34>Wmax){
     fprintf(HTMFILE,"%c<br>\n",Ac[i][k]>=0.0?'+':'-');
     for(j=0;j<l;j++) fputs("&nbsp;",HTMFILE);
     sprintf(Inf,Form2,fabs(Ac[i][k]),k,As[i][k]>=0.0?'+':'-',fabs(As[i][k]),k);
    }
    else
     sprintf(Inf,Form3,Ac[i][k]>=0.0?'+':'-',fabs(Ac[i][k]),k,
      As[i][k]>=0.0?'+':'-',fabs(As[i][k]),k);
    for(s=Inf;*s;s++)
     if(*s==' ') fputs("&nbsp;",HTMFILE); else fputc(*s,HTMFILE);
    fputs(";</font></p>\n",HTMFILE);
   }
   fprintf(HTMFILE,"w=%0.6f;</font></h5>\n",/*w=*/M_PI*2/(Tk-Tn));
   fputs("</font>\n",HTMFILE);
   fprintf(HTMFILE,
    "<hr><font face=\"Georgia, Times New Roman, Times, serif\""
    " color=\"#326464\">&copy; %s, %d<br></font>\n",Autor,NYear+1900);
   fprintf(HTMFILE,"</body>\n</html>\n");
  }
  fseek(HTMFILE,0,2); fclose(HTMFILE);
 }
 if(F){
  if(strchr(Name,'(')) sprintf(Head,Name);
  else sprintf(Head,"%s(%s)",Name,::L->Time->Name);
  for(i=0,m=Names;i<n;i++,m=m->Sled){
   if(Kg<10){
    sprintf(Form1,"%s*cos(  w*t)%%c%s*sin(  w*t)",Format[i],Format[i]);
    sprintf(Form2,  "%s*cos(%%1d*w*t)%%c%s*sin(%%1d*w*t)",Format[i],Format[i]);
    sprintf(Form3,"%%c%s*cos(%%1d*w*t)%%c%s*sin(%%1d*w*t)",Format[i],Format[i]);
   }
   else if(Kg<100){
    sprintf(Form1,"%s*cos(   w*t)%%c%s*sin(   w*t)",Format[i],Format[i]);
    sprintf(Form2,  "%s*cos(%%2d*w*t)%%c%s*sin(%%2d*w*t)",Format[i],Format[i]);
    sprintf(Form3,"%%c%s*cos(%%2d*w*t)%%c%s*sin(%%2d*w*t)",Format[i],Format[i]);
   }
   else if(Kg<1000){
    sprintf(Form1,"%s*cos(    w*t)%%c%s*sin(    w*t)",Format[i],Format[i]);
    sprintf(Form2,  "%s*cos(%%3d*w*t)%%c%s*sin(%%3d*w*t)",Format[i],Format[i]);
    sprintf(Form3,"%%c%s*cos(%%3d*w*t)%%c%s*sin(%%3d*w*t)",Format[i],Format[i]);
   }
   else{
    sprintf(Form1,"%s*cos(     w*t)%%c%s*sin(     w*t)",Format[i],Format[i]);
    sprintf(Form2,  "%s*cos(%%4d*w*t)%%c%s*sin(%%4d*w*t)",Format[i],Format[i]);
    sprintf(Form3,"%%c%s*cos(%%4d*w*t)%%c%s*sin(%%4d*w*t)",Format[i],Format[i]);
   }
   fprintf(F,"%s=",m->S);
   W=l=strlen(m->S)+1;
   fprintf(F,"%0.6f%c\n",Ac[i][0],Ac[i][1]>=0.0?'+':'-');
   for(j=0;j<l;j++) fputs(" ",F);
   W+=sprintf(Inf,Form1,fabs(Ac[i][1]),As[i][1]>=0.0?'+':'-',fabs(As[i][1]));
   for(s=Inf;*s;s++)
    if(*s==' ') fputs(" ",F); else fputc(*s,F);
   for(k=2;k<Kg;k++){
    if(W+34>Wmax){ W=l;
     fprintf(F,"%c\n",Ac[i][k]>=0.0?'+':'-');
     for(j=0;j<l;j++) fputs(" ",F);
     sprintf(Inf,Form2,fabs(Ac[i][k]),k,As[i][k]>=0.0?'+':'-',fabs(As[i][k]),k);
    }
    else
     sprintf(Inf,Form3,Ac[i][k]>=0.0?'+':'-',fabs(Ac[i][k]),k,
      As[i][k]>=0.0?'+':'-',fabs(As[i][k]),k);
    W+=34;
    for(s=Inf;*s;s++)
     if(*s==' ') fputs(" ",F); else fputc(*s,F);
   }
   if(W+34>Wmax){
    fprintf(F,"%c\n",Ac[i][k]>=0.0?'+':'-');
    for(j=0;j<l;j++) fputs(" ",F);
    sprintf(Inf,Form2,fabs(Ac[i][k]),k,As[i][k]>=0.0?'+':'-',fabs(As[i][k]),k);
   }
   else
    sprintf(Inf,Form3,Ac[i][k]>=0.0?'+':'-',fabs(Ac[i][k]),k,
     As[i][k]>=0.0?'+':'-',fabs(As[i][k]),k);
   for(s=Inf;*s;s++)
    if(*s==' ') fputs(" ",F); else fputc(*s,F);
   fputs(";\n",F);
  }
  fprintf(F,"w=%0.6f;\n",M_PI*2/(Tk-Tn));
  fclose(F);
 }
 if(PoFormsToReport){
  PoFormsToReport(Name,File);
  sprintf(Inf,"Ряд Фурье и разложение по собственным формам\n"
   "записаны в отчет и сохранены в файле\n\"%s\".\n"
   "Этот файл можно просмотреть и отредактировать\n"
   "любым редактором, сплайн вставить в КИДИМ-файл для расчетов и т.д.",
   File);
 }
 else
  sprintf(Inf,"Ряд Фурье записан в отчет и сохранен в файле\n\"%s\".\n"
   "Этот файл можно просмотреть и отредактировать\n"
   "любым редактором, сплайн вставить в КИДИМ-файл для расчетов и т.д.",
   File);
 Application->MessageBox(Inf,"Внимание!",MB_OK);
 for(i=0;i<n;i++){ free(Fs[i]); free(Ac[i]); free(As[i]); }
 free(Fs); free(Ac); free(As); free(U);
 free(P); free(D); free(C); free(L);
 for(i=0;i<CountTabl;i++) free(Ys[i]); free(Ys); free(X);
 for(i=0;i<=n;i++) free(Format[i]); free(Format);
 DelAllMagazine(&Names);
}
//---------------------------------------------------------------------------
void ToTable(char *Name,int CountTabl,double Tn,double Tk,int Lpodv,
  char *Autor,void (*Funcs)(double,Magazine *,double *),char *File){
 char *s,*s0,**Format; Magazine *Names=NULL,*m; AnsiString Head;
 double **Mas=(double **)calloc(CountTabl,SzV),Step=(Tk-Tn)/(CountTabl-1),t;
 int i,*l,L,K,d,*D,j; FILE *F;
 s=strchr(Name,'('); *s++='\0'; Head=Name; Head+=": ";
 while(s0=strchr(s,',')){ *s0++='\0'; TakeMagazine(&Names,s); s=s0; }
 if(s0=strchr(s,')')){ *s0='\0'; TakeMagazine(&Names,s); }
 for(m=Names;m;m=m->Sled){ Head+=m->S; Head+="(t)"; if(m->Sled) Head+=", "; }
 K=KolElem(Names); l=(int *)calloc(K+1,SzI);
 D=(int *)calloc(K+1,SzI); L=Head.Length();
 Format=(char **)calloc(K+1,SzV);
 for(i=0;i<=K;i++) Format[i]=(char *)calloc(16,SzC);
 for(i=0;i<CountTabl;i++){
  Mas[i]=(double *)calloc(K,SzD);
  t=Tn+i*Step; Funcs(t,Names,Mas[i]);
  d=sprintf(Inf,"%0.5f",t); if(d>l[0]) l[0]=d;
  for(j=1;j<=K;j++){ d=sprintf(Inf,"%0.5f",Mas[i][j-1]); if(d>l[j]) l[j]=d; }
 }
 sprintf(Format[0],"%%%d.5f",l[0]);
 D[0]=sprintf(Inf,"время t,сек"); if(l[0]>D[0]) D[0]=l[0];
 for(m=Names,j=1;m;m=m->Sled,j++){
  sprintf(Format[j],"%%%d.5f",l[j]);
  D[j]=sprintf(Inf,m->S); if(l[j]>D[j]) D[j]=l[j];
 }
 d=1; for(j=0;j<=K;j++) d+=1+D[j];
 while(d<L){ for(j=0;j<=K;j++) D[j]++; d+=K+1; }
 if(F=fopen(File,"w")){
  for(j=0;j<(d-L)/2;j++) fputc(' ',F); fputs(Head.c_str(),F); fputc('\n',F);
  fputc('+',F); for(j=0;j<D[0];j++) fputc('-',F); fputc('+',F);
  for(i=1;i<=K;i++){ for(j=0;j<D[i];j++) fputc('-',F); fputc('+',F); }
  fputc('\n',F);
  fputc('|',F);
  for(j=0;j<(D[0]-12)/2;j++) fputc(' ',F);
  j+=fprintf(F,"время t,сек"); for(;j<D[0];j++) fputc(' ',F);
  fputc('|',F);
  for(m=Names,i=1;m;i++,m=m->Sled){
   for(j=0;j<(D[i]-strlen(m->S))/2;j++) fputc(' ',F);
   j+=fprintf(F,m->S); for(;j<D[i];j++) fputc(' ',F); fputc('|',F);
  }
  fputc('\n',F);
  fputc('+',F); for(j=0;j<D[0];j++) fputc('-',F); fputc('+',F);
  for(i=1;i<=K;i++){ for(j=0;j<D[i];j++) fputc('-',F); fputc('+',F); }
  fputc('\n',F);
  for(i=0;i<CountTabl;i++){
   fputc('|',F); for(j=0;j<(D[0]-l[0])/2;j++) fputc(' ',F);
   t=Tn+i*Step;
   j+=fprintf(F,Format[0],t); for(;j<D[0];j++) fputc(' ',F);
   fputc('|',F);
   for(j=1;j<=K;j++){ int k;
    for(k=0;k<(D[j]-l[j])/2;k++) fputc(' ',F);
    k+=fprintf(F,Format[j],Mas[i][j-1]);
    for(;k<D[j];k++) fputc(' ',F); fputc('|',F);
   }
   fputc('\n',F);
  }
  fputc('+',F); for(j=0;j<D[0];j++) fputc('-',F); fputc('+',F);
  for(i=1;i<=K;i++){ for(j=0;j<D[i];j++) fputc('-',F); fputc('+',F); }
  fputc('\n',F);
 }
 fclose(F);
 if(HTMFILE=fopen(HtmFile,"r+")){
  if(!fseek(HTMFILE,-Lpodv,2)){
   fprintf(HTMFILE,"<h5 align=\"left\"><font color=\"#000099\">"
    "Таблица значений \"%s\"</font></h5>\n",Head);
   fputs("<table align=\"center\" width=\"100%\" border=\"1\""
    " height=\"0\" cellspacing=\"2\" cellpadding=\"4\">\n",HTMFILE);
   fputs("<tr>\n",HTMFILE);
   fprintf(HTMFILE,"<td>\n<div align=\"center\">время t, сек</div>\n</td>");
   for(m=Names;m;m=m->Sled)
    fprintf(HTMFILE,"<td>\n<div align=\"center\">%s</div>\n</td>",m->S);
   fputs("\n</tr>\n",HTMFILE);
   for(i=0;i<CountTabl;i++){
    fprintf(HTMFILE,"<td>\n<div align=\"right\">%0.5f</div>\n</td>",Tn+i*Step);
    for(j=1;j<=K;j++)
     fprintf(HTMFILE,"<td>\n<div align=\"right\">%0.5f</div>\n</td>",Mas[i][j-1]);
    fputs("</tr>\n",HTMFILE);
   }
   fputs("</table>\n",HTMFILE);
   fprintf(HTMFILE,"<hr><font face=\"Georgia, Times New Roman, Times, serif\""
    " color=\"#326464\">&copy; %s, %d<br></font><br>\n",Autor,NYear+1900);
   fprintf(HTMFILE,"</body>\n</html>\n");
  }
  fseek(HTMFILE,0,2); fclose(HTMFILE);
 }
 fclose(HTMFILE);
 for(i=0;i<=K;i++) free(Format[i]); free(Format); free(D); free(l);
 for(i=0;i<CountTabl;i++) free(Mas[i]); free(Mas); DelAllMagazine(&Names);
}
//---------------------------------------------------------------------------


