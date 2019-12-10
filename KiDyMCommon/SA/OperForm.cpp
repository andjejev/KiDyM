#include "CA.h"
#include <string.h>
#include <math.h>

extern List *L;
extern Cnst *C,*Pust,*Nul,*Halb,*Odin,*Dva,*Tri,*Exp,*pi;
extern FILE *ERRFILE;
extern TStringList *SLD,*SLH,*SLO,*SLE; extern String SF;
extern wchar_t Inf[],ErrFile[],Serr[],*serr;

bool VaryNoOK(Vary *V){
 Parm *P; Cord *K; Root *I;
 if((I=FindInstLast(L"ОБОБЩЕННЫЕ КООРДИНАТЫ"))||
    (I=FindInstLast(L"ОБОБЩЁННЫЕ КООРДИНАТЫ"))){
  for(P=I->First;P;P=P->Sled) if(!wcscmp(P->Nam,V->Name)) break;
  return !P;
 }
 for(K=L->K;K;K=K->Sled) if(K->V==V) break;
 return !K;
}

		/* Перегружаемые операции */

bool operator == (Form A,Form  B){
 if(A.C->Atr!=B.C->Atr) return false;
 switch(A.C->Atr){
  case CNST: return A.C->Val==B.C->Val;
  case VARY: return !wcscmp(A.V->Name,B.V->Name);
  case UNOP:
   if(A.U->Func!=B.U->Func) return false;
   return A.U->Arg==B.U->Arg;
  case BNOP:
   if(A.B->Oper!=B.B->Oper) return false;
   if(A.B->L==B.B->L) return A.B->R==B.B->R;
   if((A.B->Oper==3)||(A.B->Oper==1))
    if(A.B->L==B.B->R) return A.B->R==B.B->L;
   break;
  case IFOP:
   if(A.L->Oper!=B.L->Oper) return false;
   if((A.L->U==B.L->U)&&(A.L->V==B.L->V)&&(A.L->FF==B.L->FF))
    return A.L->FT==B.L->FT;
 }
 return false;
}
bool operator != (Form A,Form  B){ return A.C!=B.C; }
bool operator == (Form F,Cnst *C){
 if(F.C==C) return true;
// if((F.C->Atr==VARY)&&(F.V->Znach.C==C)) return true;
 return false;
}
bool operator != (Form F,Cnst *C){
 if(F.C!=C) return true;
// if((F.C->Atr!==VARY)||(F.V->Znach.C!=C)) return true;
 return false;
}
bool operator == (Form F,Vary *V){ return F.V==V;   }

Form operator  + (Form F,double D){ Form f; return F+(f.C=TakeCnst(D),f); }
Form operator  - (Form F,double D){ Form f; return F-(f.C=TakeCnst(D),f); }
Form operator  * (Form F,double D){ Form f; return F*(f.C=TakeCnst(D),f); }
Form operator  / (Form F,double D){ Form f; return F/(f.C=TakeCnst(D),f); }
Form operator  ^ (Form F,double D){ Form f; return F^(f.C=TakeCnst(D),f); }

Form operator  + (double D,Form F){ Form f; return (f.C=TakeCnst(D),f)+F; }
Form operator  - (double D,Form F){ Form f; return (f.C=TakeCnst(D),f)-F; }
Form operator  * (double D,Form F){ Form f; return (f.C=TakeCnst(D),f)*F; }
Form operator  / (double D,Form F){ Form f; return (f.C=TakeCnst(D),f)/F; }
Form operator  ^ (double D,Form F){ Form f; return (f.C=TakeCnst(D),f)^F; }

//Form Form::operator  = (double D){ C=TakeCnst(D); return *this; }
//Form Form::operator += (double D){ Form F; return *this+=(F.C=TakeCnst(D),F); }
//Form Form::operator -= (double D){ Form F; return *this-=(F.C=TakeCnst(D),F); }
//Form Form::operator *= (double D){ Form F; return *this*=(F.C=TakeCnst(D),F); }
//Form Form::operator /= (double D){ Form F; return *this/=(F.C=TakeCnst(D),F); }
//Form Form::operator ^= (double D){ Form F; return *this^=(F.C=TakeCnst(D),F); }

Form operator + (Form A,Form B){ Form F;
 if(A==Nul) return B;
//  IsVary(B)&&IsCnst(B.V->Znach)?B.V->Znach:B;
// if(IsVary(A)&&IsCnst(A.V->Znach)&&(A.V->Znach==Nul)&&VaryNoOK(A.V)) return// B;
//  IsVary(B)&&IsCnst(B.V->Znach)?B.V->Znach:B;
 if(B==Nul) return A;
//  IsVary(A)&&IsCnst(A.V->Znach)?A.V->Znach:A;
// if(IsVary(B)&&IsCnst(B.V->Znach)&&(B.V->Znach==Nul)&&VaryNoOK(B.V)) return// A;
//  IsVary(A)&&IsCnst(A.V->Znach)?A.V->Znach:A;
 if(A==B)    return 2*A;
 if(IsCnst(A)&&IsCnst(B)){
  F.C=TakeCnst(A.C->Val+B.C->Val);
  return F;
 }

 if(IsCnst(A)&&(A.C->Val<0)){
  F.C=TakeCnst(fabs(A.C->Val));
  return B-F;
 }
 if(IsCnst(B)&&(B.C->Val<0)){
  F.C=TakeCnst(fabs(B.C->Val));
  return A-F;
 }
 if(IsMins(A))
  if(IsMins(B)) return -(A.U->Arg+B.U->Arg);
  else return (B-A.U->Arg);
 else
  if(IsMins(B)) return (A-B.U->Arg);
 if(IsMult(A)){
  if(IsMult(B)){
   if(A.B->L==B.B->L) return A.B->L*(A.B->R+B.B->R);
   if(A.B->L==B.B->R) return A.B->L*(A.B->R+B.B->L);
   if(A.B->R==B.B->L) return (A.B->L+B.B->R)*A.B->R;
   if(A.B->R==B.B->R) return (A.B->L+B.B->L)*A.B->R;
  }
  else if(IsDivd(B)){
   if(A.B->L==B.B->L) return A.B->L*(A.B->R+(1/B.B->R));
   if(A.B->R==B.B->L) return (A.B->L+(1/B.B->R))*A.B->R;
 }}
 else if(IsDivd(A)){
  if(IsDivd(B)){
//   if((A.B->L==B.B->L)&&(A.B->L!=Odin)) return A.B->L*((1/A.B->R)+(1/B.B->R));
   if(A.B->L==B.B->L) return A.B->L*(A.B->R+B.B->R)/(A.B->R*B.B->R);
   if(A.B->R==B.B->R) return (A.B->L+B.B->L)/A.B->R;
  }
  if(IsMult(B)){
   if(A.B->L==B.B->L) return A.B->L*((1/A.B->R)+B.B->R);
   if(A.B->L==B.B->R) return A.B->L*((1/A.B->R)+B.B->L);
 }}
 F.B=TakeBnop(1,A,B); return F;
}

Form operator += (Form A,Form B){
 Form F=A+B;
 A=F;
 return A;
// return A=A+B;
};

Form operator - (Form A,Form B){ Form F;
 if(B==Nul)  return  A;
// if(IsVary(B)&&IsCnst(B.V->Znach)&&(B.V->Znach==Nul)&&VaryNoOK(B.V)) return A;
 if(A==Nul)  return -B;
// if(IsVary(A)&&IsCnst(A.V->Znach)&&(A.V->Znach==Nul)&&VaryNoOK(A.V)) return -B;

 if(A==B)    return  F.C=Nul,F;
 if((A.C->Atr==VARY)&&(A.V->Znach==B)) return  F.C=Nul,F;
 if((B.C->Atr==VARY)&&(B.V->Znach==A)) return  F.C=Nul,F;
 if((A.C->Atr==VARY)&&(B.C->Atr==VARY)){
  if(A.V->Znach==B.V->Znach) return F.C=Nul,F;
//  else{
//   Train TA(A.V->Znach),TB(B.V->Znach); AnsiString SA,SB,Rez="Составы:\r\n";
//   TA.ToStr(SA); Rez+=SA; Rez+="\r\n";
//   TB.ToStr(SB); Rez+=SB; Rez+="\r\n";
//   TA.ToFile(L"TA.prn",true);
//   TB.ToFile(L"TB.prn",true);
//   if(TA.Eqv(&TB))
//    Rez+="эквивалентны!";
//   else
//    Rez+="не эквивалентны!";
//   Application->MessageBoxA(Rez.c_str(),L"TA==TB?",MB_OK);
//  }
 }
 if(IsCnst(A)&&IsCnst(B)){
  F.C=TakeCnst(A.C->Val-B.C->Val);
  return F;
 }
 if(IsCnst(A)&&(A.C->Val<0)){
  F.C=TakeCnst(fabs(A.C->Val));
  return -(F+B);
 }
 if(IsCnst(B)&&(B.C->Val<0)){
  F.C=TakeCnst(fabs(B.C->Val));
  return (A+F);
 }
 if(IsMins(A))
  if(IsMins(B)) return (B.U->Arg-A.U->Arg);
  else return -(A.U->Arg+B);
 else
  if(IsMins(B)) return (A+B.U->Arg);
 if(IsMult(A)){
  if(IsMult(B)){
   if(A.B->L==B.B->L) return A.B->L*(A.B->R-B.B->R);
   if(A.B->L==B.B->R) return A.B->L*(A.B->R-B.B->L);
   if(A.B->R==B.B->L) return (A.B->L-B.B->R)*A.B->R;
   if(A.B->R==B.B->R) return (A.B->L-B.B->L)*A.B->R;
  }
  else if(IsDivd(B)){
   if(A.B->L==B.B->L) return A.B->L*(A.B->R-(1/B.B->R));
   if(A.B->R==B.B->L) return (A.B->L-(1/B.B->R))*A.B->R;
 }}
 else if(IsDivd(A)){
  if(IsDivd(B)){
   if(A.B->L==B.B->L) return A.B->L*(A.B->R-B.B->R)/(A.B->R*B.B->R);
   if(A.B->R==B.B->R) return (A.B->L-B.B->L)/A.B->R;
  }
  if(IsMult(B)){
   if(A.B->L==B.B->L)
    return
     A.B->L*((1/A.B->R)-B.B->R);
   if(A.B->L==B.B->R) return A.B->L*((1/A.B->R)-B.B->L);
 }}
 F.B=TakeBnop(2,A,B); return F;
}

Form operator * (Form A,Form B){ Form F,La,Ra,Lb,Rb; 
 if(A==Nul)  return A;
// if(IsVary(A)&&IsCnst(A.V->Znach)&&(A.V->Znach==Nul)&&VaryNoOK(A.V))
//  return A.V->Znach;
 if(B==Nul)  return B;
// if(IsVary(B)&&IsCnst(B.V->Znach)&&(B.V->Znach==Nul)&&VaryNoOK(B.V))
//  return B.V->Znach;
 if(A==Odin) return B;
 if(B==Odin) return A;
 if(A==B)    return A^2;
 if(IsCnst(A)&&(A.C->Val<0)&&IsCnst(B)&&(B.C->Val<0)){
  F.C=TakeCnst((A.C->Val)*(B.C->Val));
  return F;
 }
 if(IsCnst(A)&&(A.C->Val<0)){
  F.C=TakeCnst(fabs(A.C->Val));
  return -(F*B);
 }
 if(IsCnst(B)&&(B.C->Val<0)){
  F.C=TakeCnst(fabs(B.C->Val));
  return (-A)*F;
 }
 if(IsMins(A))
  if(IsMins(B)) return (A.U->Arg*B.U->Arg);
  else return -(A.U->Arg*B);
 else
  if(IsMins(B))
       return -(A*B.U->Arg);
 if(IsCnst(A)&&IsCnst(B)){ F.C=TakeCnst(A.C->Val*B.C->Val); return F; }
 if(IsCnst(A)){
  if(IsMult(B)){
   if(IsCnst(B.B->L)){
    F.C=TakeCnst(A.C->Val*B.B->L.C->Val); return F*B.B->R;
   }
   if(IsCnst(B.B->R)){
    F.C=TakeCnst(A.C->Val*B.B->R.C->Val); return F*B.B->L;
  }}
  if(IsDivd(B)){
   if(IsCnst(B.B->L)){
    F.C=TakeCnst(A.C->Val*B.B->L.C->Val); return F/B.B->R;
   }
   if(IsCnst(B.B->R)){
    F.C=TakeCnst(A.C->Val/B.B->R.C->Val); return F*B.B->L;
 }}}
 if(IsCnst(B)){
  if(IsMult(A)){
   if(IsCnst(A.B->L)){
    F.C=TakeCnst(B.C->Val*A.B->L.C->Val); return F*A.B->R;
   }
   if(IsCnst(A.B->R)){
    F.C=TakeCnst(B.C->Val*A.B->R.C->Val); return F*A.B->L;
  }}
  if(IsDivd(A)){
   if(IsCnst(A.B->L)){
    F.C=TakeCnst(B.C->Val*A.B->L.C->Val); return F/A.B->R;
   }
   if(IsCnst(A.B->R)){
    F.C=TakeCnst(B.C->Val/A.B->R.C->Val); return F*A.B->L;
 }}}
 if(IsMult(A)){ La=A.B->L; Ra=A.B->R;
  if(IsMult(B)){ Lb=B.B->L; Rb=B.B->R;
   if(IsCnst(La)){
    if(IsCnst(Lb)){
     F.C=TakeCnst(La.C->Val*Lb.C->Val); return F*(Ra*Rb);
    }
    if(IsCnst(Rb)){
     F.C=TakeCnst(La.C->Val*Rb.C->Val); return F*(Ra*Lb);
   }}
   else if(IsCnst(Ra)){
    if(IsCnst(Lb)){
     F.C=TakeCnst(Ra.C->Val*Lb.C->Val); return F*(La*Rb);
    }
    if(IsCnst(Rb)){
     F.C=TakeCnst(Ra.C->Val*Rb.C->Val); return F*(La*Lb);
  }}}
  else if(IsDivd(B)){ Lb=B.B->L; Rb=B.B->R;
   if(IsCnst(La)){
    if(IsCnst(Lb)){
     F.C=TakeCnst(La.C->Val*Lb.C->Val); return F*(Ra/Rb);
    }
    if(IsCnst(Rb)){
     F.C=TakeCnst(La.C->Val/Rb.C->Val); return F*(Ra*Lb);
   }}
   else if(IsCnst(Ra)){
    if(IsCnst(Lb)){
     F.C=TakeCnst(Ra.C->Val*Lb.C->Val); return F*(La/Rb);
    }
    if(IsCnst(Rb)){
     F.C=TakeCnst(Ra.C->Val/Rb.C->Val); return F*(La*Lb);
 }}}}
 else if(IsDivd(A)){ La=A.B->L; Ra=A.B->R;
  if(IsMult(B)){ Lb=B.B->L; Rb=B.B->R;
   if(IsCnst(La)){
    if(IsCnst(Lb)){ F.C=TakeCnst(La.C->Val*Lb.C->Val); return F*(Rb/Ra); }
    if(IsCnst(Rb)){ F.C=TakeCnst(La.C->Val*Rb.C->Val); return F*(Lb/Ra); }
   }
   else if(IsCnst(Ra)){
    if(IsCnst(Lb)){ F.C=TakeCnst(Lb.C->Val/Ra.C->Val); return F*(La*Rb); }
    if(IsCnst(Rb)){ F.C=TakeCnst(Rb.C->Val/Ra.C->Val); return F*(La*Lb); }
  }}
  else if(IsDivd(B)){ Lb=B.B->L; Rb=B.B->R;
   if(IsCnst(La)){
    if(IsCnst(Lb)){ F.C=TakeCnst(La.C->Val*Lb.C->Val); return F/(Ra*Rb); }
    if(IsCnst(Rb)){ F.C=TakeCnst(La.C->Val/Rb.C->Val); return F*(Lb/Ra); }
   }
   else if(IsCnst(Ra)){
    if(IsCnst(Lb)){ F.C=TakeCnst(Lb.C->Val/Ra.C->Val); return F*(La/Rb); }
    if(IsCnst(Rb)){ F.C=TakeCnst(1/Ra.C->Val/Rb.C->Val); return F*(La*Lb); }
 }}}
 if(IsMult(A)){
  if(IsCnst(A.B->L)) return A.B->L.C->Val*(A.B->R*B);
  if(IsCnst(A.B->R)) return A.B->R.C->Val*(A.B->L*B);
 }
 if(IsDivd(A)){
  if(IsCnst(A.B->L)) return A.B->L.C->Val*(B/A.B->R);
  if(IsCnst(A.B->R)){
   F.C=TakeCnst(1/A.B->R.C->Val); return F*(A.B->L*B);
 }}
 if(IsMult(B)){
  if(IsCnst(B.B->L)) return B.B->L.C->Val*(A*B.B->R);
  if(IsCnst(B.B->R)) return B.B->R.C->Val*(A*B.B->L);
 }
 if(IsDivd(B)){
  if(IsCnst(B.B->L))
   return B.B->L.C->Val*(A/B.B->R);
  if(IsCnst(B.B->R)){
   F.C=TakeCnst(1/B.B->R.C->Val); return F*(A*B.B->L);
 }}
 if((A.C->Atr==6)&&(A.B->Oper==6)&&(A.B->L==B))
  return B^(A.B->R+1);
 if((B.C->Atr==6)&&(B.B->Oper==6)&&(B.B->L==A))
  return A^(B.B->R+1);
//Умножение массива на константу и наоборот
 if((A.C->Atr==ARRA)&&((B.C->Atr==CNST)||(B.C->Atr==VARY))){
  F.A=(Arra *)calloc(1,sizeof(Arra));
  F.A->Type=A.A->Type; F.A->Znach.C=Pust; F.A->Atr=ARRA; F.A->N=A.A->N;
  if(A.A->F){//формульный массив
   F.A->F=(Form *)calloc(A.A->N,sizeof(Form));
   for(int i=0;i<A.A->N;i++)
	F.A->F[i]=A.A->F[i]*B;
  }
  else if(A.A->A){//числовой массив
   if(B.C->Atr==CNST){
	F.A->A=(double *)calloc(A.A->N,SzD);
	for(int i=0;i<A.A->N;i++)
	 F.A->A[i]=A.A->A[i]*(B.C->Val);
   }
   else{
	F.A->F=(Form *)calloc(A.A->N,sizeof(Form));
	for(int i=0;i<A.A->N;i++)
	 F.A->F[i]=A.A->A[i]*B;
  }}
  return F;
 }
 if(((A.C->Atr==CNST)||(A.C->Atr==VARY))&&(B.C->Atr==ARRA)){
  F.A=(Arra *)calloc(1,sizeof(Arra));
  F.A->Type=B.A->Type; F.A->Znach.C=Pust; F.A->Atr=ARRA; F.A->N=B.A->N;
  if(B.A->F){//формульный массив
   F.A->F=(Form *)calloc(B.A->N,sizeof(Form));
   for(int i=0;i<B.A->N;i++)
	F.A->F[i]=A*B.A->F[i];
  }
  else if(B.A->A){//числовой массив
   if(A.C->Atr==CNST){
	F.A->A=(double *)calloc(B.A->N,SzD);
	for(int i=0;i<B.A->N;i++)
	 F.A->A[i]=B.A->A[i]*(A.C->Val);
   }
   else{
	F.A->F=(Form *)calloc(B.A->N,sizeof(Form));
	for(int i=0;i<B.A->N;i++)
	 F.A->F[i]=B.A->A[i]*A;
  }}
  return F;
 }
 if((A.C->Atr==ARRA)&&(B.C->Atr==ARRA)){
  if((A.A->Type==QUAT)&&(B.A->Type==QUAT)){
   F.A=(Arra *)calloc(1,sizeof(Arra));
   if(A.A->F&&B.A->F){
	Form *FA=A.A->F,*FB=B.A->F,*MF=(Form *)calloc(4,sizeof(Form));
	MF[0]=FA[0]*FB[0]-FA[1]*FB[1]-FA[2]*FB[2]-FA[3]*FB[3];
	MF[1]=FA[0]*FB[1]+FA[1]*FB[0]+FA[2]*FB[3]-FA[3]*FB[2];
	MF[2]=FA[0]*FB[2]+FA[2]*FB[0]+FA[3]*FB[1]-FA[1]*FB[3];
	MF[3]=FA[0]*FB[3]+FA[3]*FB[0]+FA[1]*FB[2]-FA[2]*FB[1];
	F.A->Type=QUAT; F.A->Znach.C=Pust; F.A->Atr=ARRA; F.A->N=4; F.A->F=MF;
   }
   else if(A.A->F&&B.A->A){
	Form *FA=A.A->F,*MF=(Form *)calloc(4,sizeof(Form)); double *MB=B.A->A;
	MF[0]=FA[0]*MB[0]-FA[1]*MB[1]-FA[2]*MB[2]-FA[3]*MB[3];
	MF[1]=FA[0]*MB[1]+FA[1]*MB[0]+FA[2]*MB[3]-FA[3]*MB[2];
	MF[2]=FA[0]*MB[2]+FA[2]*MB[0]+FA[3]*MB[1]-FA[1]*MB[3];
	MF[3]=FA[0]*MB[3]+FA[3]*MB[0]+FA[1]*MB[2]-FA[2]*MB[1];
	F.A->Type=QUAT; F.A->Znach.C=Pust; F.A->Atr=ARRA; F.A->N=4; F.A->F=MF;
   }
   else if(A.A->A&&B.A->F){
	Form *FB=B.A->F,*MF=(Form *)calloc(4,sizeof(Form)); double *MA=A.A->A;
	MF[0]=MA[0]*FB[0]-MA[1]*FB[1]-MA[2]*FB[2]-MA[3]*FB[3];
	MF[1]=MA[0]*FB[1]+MA[1]*FB[0]+MA[2]*FB[3]-MA[3]*FB[2];
	MF[2]=MA[0]*FB[2]+MA[2]*FB[0]+MA[3]*FB[1]-MA[1]*FB[3];
	MF[3]=MA[0]*FB[3]+MA[3]*FB[0]+MA[1]*FB[2]-MA[2]*FB[1];
	F.A->Type=QUAT; F.A->Znach.C=Pust; F.A->Atr=ARRA; F.A->N=4; F.A->F=MF;
   }
   else if(A.A->A&&B.A->A){
	double *MA=A.A->A,*MB=B.A->A,*MD=(double *)calloc(4,sizeof(double));
	MD[0]=MA[0]*MB[0]-MA[1]*MB[1]-MA[2]*MB[2]-MA[3]*MB[3];
	MD[1]=MA[0]*MB[1]+MA[1]*MB[0]+MA[2]*MB[3]-MA[3]*MB[2];
	MD[2]=MA[0]*MB[2]+MA[2]*MB[0]+MA[3]*MB[1]-MA[1]*MB[3];
	MD[3]=MA[0]*MB[3]+MA[3]*MB[0]+MA[1]*MB[2]-MA[2]*MB[1];
	F.A->Type=QUAT; F.A->Znach.C=Pust; F.A->Atr=ARRA; F.A->N=4; F.A->A=MD;
   }
   else F.C=Pust;
  }
  else if((A.A->Type==VECT)&&(B.A->Type==VECT)){
   F.A=(Arra *)calloc(1,sizeof(Arra));
   if(A.A->F&&B.A->F){
	Form *FA=A.A->F,*FB=B.A->F,*MF=(Form *)calloc(3,sizeof(Form));
	MF[0]=FA[1]*FB[2]-FA[2]*FB[1]; MF[1]=FA[2]*FB[0]-FA[0]*FB[2];
	MF[2]=FA[0]*FB[1]-FA[1]*FB[0];
	F.A->Type=VECT; F.A->Znach.C=Pust; F.A->Atr=ARRA; F.A->N=3; F.A->F=MF;
   }
   else if(A.A->F&&B.A->A){
	Form *FA=A.A->F,*MF=(Form *)calloc(3,sizeof(Form)); double *MB=B.A->A;
	MF[0]=FA[1]*MB[2]-FA[2]*MB[1]; MF[1]=FA[2]*MB[0]-FA[0]*MB[2];
	MF[2]=FA[0]*MB[1]-FA[1]*MB[0];
	F.A->Type=VECT; F.A->Znach.C=Pust; F.A->Atr=ARRA; F.A->N=3; F.A->F=MF;
   }
   else if(A.A->A&&B.A->F){
	Form *FB=B.A->F,*MF=(Form *)calloc(3,sizeof(Form)); double *MA=A.A->A;
	MF[0]=MA[1]*FB[2]-MA[2]*FB[1]; MF[1]=MA[2]*FB[0]-MA[0]*FB[2];
	MF[2]=MA[0]*FB[1]-MA[1]*FB[0];
	F.A->Type=VECT; F.A->Znach.C=Pust; F.A->Atr=ARRA; F.A->N=3; F.A->F=MF;
   }
   else if(A.A->A&&B.A->A){
	double *MA=A.A->A,*MB=B.A->A,*MD=(double *)calloc(3,sizeof(double));
	MD[0]=MA[1]*MB[2]-MA[2]*MB[1]; MD[1]=MA[2]*MB[0]-MA[0]*MB[2];
	MD[2]=MA[0]*MB[1]-MA[1]*MB[0];
	F.A->Type=VECT; F.A->Znach.C=Pust; F.A->Atr=ARRA; F.A->N=3; F.A->A=MD;
   }
   else F.C=Pust;
  }
  else if((A.A->Type==CLMN)&&(B.A->Type==CLMN)){
   F.C=Nul;
   if(A.A->F&&B.A->F){
	for(int i=0;i<A.A->N;i++)
	 F=F+A.A->F[i]*B.A->F[i];
   }
   else if(A.A->F&&B.A->A){
	for(int i=0;i<A.A->N;i++)
	 F=F+A.A->F[i]*B.A->A[i];
   }
   else if(A.A->A&&B.A->F){
	for(int i=0;i<A.A->N;i++)
	 F=F+A.A->A[i]*B.A->F[i];
   }
   else if(A.A->A&&B.A->A){
	for(int i=0;i<A.A->N;i++)
	 F=F+A.A->A[i]*B.A->A[i];
   }
   else F.C=Pust;
  }
  else if((A.A->Type==MATR)&&(B.A->Type==CLMN)){
   F.A=(Arra *)calloc(1,sizeof(Arra));
   int Str=A.A->N/B.A->N;
   if(A.A->F&&B.A->F){
	Form *MF=(Form *)calloc(B.A->N,sizeof(Form)),*FA=A.A->F,*FB=B.A->F;
	for(int i=0;i<Str;i++){
	 MF[i].C=Nul;
	 for(int j=0;j<B.A->N;j++)
	  MF[i]=MF[i]+(*FA++)*FB[j];
	}
	F.A->Type=CLMN; F.A->Znach.C=Pust; F.A->Atr=ARRA; F.A->N=Str; F.A->F=MF;
   }
   else if(A.A->F&&B.A->A){
	Form *MF=(Form *)calloc(B.A->N,sizeof(Form)),*FA=A.A->F; double *MB=B.A->A;
	for(int i=0;i<Str;i++){
	 MF[i].C=Nul;
	 for(int j=0;j<B.A->N;j++)
	  MF[i]=MF[i]+(*FA++)*MB[j];
	}
	F.A->Type=CLMN; F.A->Znach.C=Pust; F.A->Atr=ARRA; F.A->N=Str; F.A->F=MF;
   }
   else if(A.A->A&&B.A->F){
	Form *MF=(Form *)calloc(B.A->N,sizeof(Form)),*FB=B.A->F; double *MA=A.A->A;
	for(int i=0;i<Str;i++){
	 MF[i].C=Nul;
	 for(int j=0;j<B.A->N;j++)
	  MF[i]=MF[i]+(*MA++)*FB[j];
	}
	F.A->Type=CLMN; F.A->Znach.C=Pust; F.A->Atr=ARRA; F.A->N=Str; F.A->F=MF;
   }
   else if(A.A->A&&B.A->A){
	double *MD=(double *)calloc(B.A->N,SzD),*MA=A.A->A,*MB=B.A->A;
	for(int i=0;i<Str;i++){
	 for(int j=0;j<B.A->N;j++)
	  MD[i]+=(*MA++)*MB[j];
	}
	F.A->Type=CLMN; F.A->Znach.C=Pust; F.A->Atr=ARRA; F.A->N=Str; F.A->A=MD;
   }
   else F.C=Pust;
  }
  return F;
 }
 else if(A.C->Atr==ARRA){
  for(int i=0;i<A.A->N;i++)
   if(A.A->F) A.A->F[i]=A.A->F[i]*B;
//А если массив чисел???
 }
 else if(B.C->Atr==ARRA){
  for(int i=0;i<B.A->N;i++)
   if(B.A->F) B.A->F[i]=A*B.A->F[i];
//А если массив чисел???
 }
 F.B=TakeBnop(3,A,B); return F;
}

Form operator / (Form A,Form B){ Form F,La,Lb,Ra,Rb;
 if((B==Nul)||(IsVary(B)&&IsCnst(B.V->Znach)&&(B.V->Znach==Nul))){
  wcscpy(Inf,L"Формульное деление на ноль (Form A/Form B).");
  Error(0,0,true);
//  if(ERRFILE=_wfopen(ErrFile,L"a")){
  if(SLE){
   //PrintForm(true,ERRFILE,L"Form A",A);
   serr=FormToStringList(A,L"Form A",Serr,serr,SLE,true,&SF);
   //PrintForm(true,ERRFILE,L"Form B",B);
   serr=FormToStringList(B,L"Form B",Serr,serr,SLE,true,&SF);
   //fclose(ERRFILE);
   SLE->SaveToFile(ErrFile);
  }
  F.C=Pust; return F;
 }
 if(A==Nul) return A;
 if(B==Odin) return A;
 if(A==B){ F.C=Odin; return F; }
 if(IsCnst(A)&&(A.C->Val<0)&&IsCnst(B)&&(B.C->Val<0)){
  F.C=TakeCnst(A.C->Val/B.C->Val);
  return F;
 }
 if(IsCnst(A)&&(A.C->Val<0)){
  F.C=TakeCnst(fabs(A.C->Val));
  return -(F/B);
 }
 if(IsCnst(B)&&(B.C->Val<0)){
  F.C=TakeCnst(fabs(B.C->Val));
  return -(A/F);
 }
 if(IsMins(A))
  if(IsMins(B)) return (A.U->Arg/B.U->Arg);
  else return -(A.U->Arg/B);
 else if(IsMins(B)) return -(A/B.U->Arg);
 if(IsCnst(A)&&IsCnst(B)){ F.C=TakeCnst(A.C->Val/B.C->Val); return F; }
 if(IsCnst(A)){
  if(IsMult(B)){
   if(IsCnst(B.B->L)){ F.C=TakeCnst(A.C->Val/B.B->L.C->Val); return F/B.B->R; }
   if(IsCnst(B.B->R)){ F.C=TakeCnst(A.C->Val/B.B->R.C->Val); return F/B.B->L; }
  }
  if(IsDivd(B)){
   if(IsCnst(B.B->L)){ F.C=TakeCnst(A.C->Val/B.B->L.C->Val); return F*B.B->R; }
   if(IsCnst(B.B->R)){ F.C=TakeCnst(A.C->Val*B.B->R.C->Val); return F/B.B->L; }
 }}
 if(IsCnst(B)){
  if(IsMult(A)){
   if(IsCnst(A.B->L)){ F.C=TakeCnst(A.B->L.C->Val/B.C->Val); return F*A.B->R; }
   if(IsCnst(A.B->R)){ F.C=TakeCnst(A.B->R.C->Val/B.C->Val); return F*A.B->L; }
  }
  if(IsDivd(A)){
   if(IsCnst(A.B->L)){ F.C=TakeCnst(A.B->L.C->Val/B.C->Val); return F/A.B->R; }
   if(IsCnst(A.B->R)){ F.C=TakeCnst(1/A.B->R.C->Val/B.C->Val); return F*A.B->L; }
 }}
 if(IsMult(A)){ La=A.B->L; Ra=A.B->R;
  if(IsMult(B)){ Lb=B.B->L; Rb=B.B->R;
   if(IsCnst(La)){
    if(IsCnst(Lb)){ F.C=TakeCnst(La.C->Val/Lb.C->Val); return F*(Ra/Rb); }
    if(IsCnst(Rb)){ F.C=TakeCnst(La.C->Val/Rb.C->Val); return F*(Ra/Lb); }
   }
   else if(IsCnst(Ra)){
    if(IsCnst(Lb)){ F.C=TakeCnst(Ra.C->Val/Lb.C->Val); return F*(La/Rb); }
    if(IsCnst(Rb)){ F.C=TakeCnst(Ra.C->Val/Rb.C->Val); return F*(La/Lb); }
  }}
  else if(IsDivd(B)){ Lb=B.B->L; Rb=B.B->R;
   if(IsCnst(La)){
    if(IsCnst(Lb)){ F.C=TakeCnst(La.C->Val/Lb.C->Val); return F*(Ra*Rb); }
    if(IsCnst(Rb)){ F.C=TakeCnst(La.C->Val*Rb.C->Val); return F*(Ra/Lb); }
   }
   else if(IsCnst(Ra)){
    if(IsCnst(Lb)){ F.C=TakeCnst(Ra.C->Val/Lb.C->Val); return F*(La*Rb); }
    if(IsCnst(Rb)){ F.C=TakeCnst(Ra.C->Val*Rb.C->Val); return F*(La/Lb); }
 }}}
 else if(IsDivd(A)){ La=A.B->L; Ra=A.B->R;
  if(B==Ra){ F=Ra*Ra; return La/F; }
  if(IsMult(B)){ Lb=B.B->L; Rb=B.B->R;
   if(IsCnst(La)){
    if(IsCnst(Lb)){ F.C=TakeCnst(La.C->Val/Lb.C->Val); return F/(Ra*Rb); }
    if(IsCnst(Rb)){ F.C=TakeCnst(La.C->Val/Rb.C->Val); return F/(Ra/Lb); }
   }
   else if(IsCnst(Ra)){
    if(IsCnst(Lb)){ F.C=TakeCnst(1/Ra.C->Val/Lb.C->Val); return F*(La/Rb); }
    if(IsCnst(Rb)){ F.C=TakeCnst(1/Ra.C->Val/Rb.C->Val); return F*(La/Lb); }
  }}
  else if(IsDivd(B)){ Lb=B.B->L; Rb=B.B->R;
   if(IsCnst(La)){
    if(IsCnst(Lb)){ F.C=TakeCnst(La.C->Val/Lb.C->Val); return F*(Rb/Ra); }
    if(IsCnst(Rb)){ F.C=TakeCnst(La.C->Val*Rb.C->Val); return F/(Ra*Lb); }
   }
   else if(IsCnst(Ra)){
    if(IsCnst(Lb)){ F.C=TakeCnst(1/Ra.C->Val/Lb.C->Val); return F*(Rb/La); }
    if(IsCnst(Rb)){ F.C=TakeCnst(Rb.C->Val/Ra.C->Val); return F*(La/Lb); }
 }}}
 if(IsMult(A)&&IsCnst(A.B->L)) return A.B->L*(A.B->R/B);
 if(IsMult(A)&&IsCnst(A.B->R)) return A.B->R*(A.B->L/B);
 if(IsDivd(A)&&IsCnst(A.B->L)) return A.B->L/(A.B->R*B);
 if(IsDivd(A)&&IsCnst(A.B->R)){
  F.C=TakeCnst(1/A.B->R.C->Val); return F*(A.B->L/B);
 }
 if(IsMult(B)&&IsCnst(B.B->L)){
  F.C=TakeCnst(1/B.B->L.C->Val); return F*(A/B.B->R);
 }
 if(IsMult(B)&&IsCnst(B.B->R)){
  F.C=TakeCnst(1/B.B->R.C->Val); return F*(A/B.B->L);
 }
 if(IsMult(B)&&A==B.B->L){ F.C=Odin; return F/B.B->R; }
 if(IsMult(B)&&A==B.B->R){ F.C=Odin; return F/B.B->L; }
 if(IsMult(A)&&B==A.B->L) return A.B->R;
 if(IsMult(A)&&B==A.B->R) return A.B->L;
 if(IsDivd(B)&&IsCnst(B.B->L)){
  F.C=TakeCnst(1/B.B->L.C->Val); return F*(A*B.B->R);
 }
 if(IsDivd(B)&&IsCnst(B.B->R)) return B.B->R*(A/B.B->L);
 if(IsDivd(A)&&A.B->R==B) return A.B->L; 
 if(IsDivd(B)) return A*B.B->R/B.B->L;
 return F.B=TakeBnop(4,A,B),F;
}

Form operator % (Form A,Form B){ Form F,La,Lb,Ra,Rb;
 if(A==B){ F.C=Nul; return F; }
 if((A==Nul)||(B==Nul)) return A;
 if(IsVary(A)&&IsCnst(A.V->Znach)&&(A.V->Znach==Nul)&&VaryNoOK(A.V))
  return A.V->Znach;
 return F.B=TakeBnop(5,A,B),F;
}

Form operator ^ (Form A,Form B){ Form F;
 if(A==Nul) return F.C=Nul,F;
 if(A==Odin || B==Odin) return A;
 if(IsCnst(B)&&(B.C->Val<0)){
  F.C=TakeCnst(fabs(B.C->Val));
  return 1/(A^F);
 }
 if(B==Nul) return F.C=Odin,F;
 if(B==Dva){
  if((A.C->Atr==5)&&(A.U->Func==2)) return F=A.U->Arg;
  if((A.C->Atr==4)&&(A.V->Znach.C->Atr==5)&&(A.V->Znach.U->Func==2))
   return F=A.V->Znach.U->Arg;
 }
 if(B.C->Atr==3){
  if((A.C->Atr==6)&&(A.B->Oper==6)&&(A.B->R.C->Atr==3))
   if(Round(A.B->R.C->Val*B.C->Val,0.01)==1.0) return F=A.B->L;
   else{
    B.C=TakeCnst(A.B->R.C->Val*B.C->Val);
    return F=A.B->L^B;
   }
  if((A.C->Atr==4)&&(A.V->Znach.C->Atr==6)&&(A.V->Znach.B->Oper==6)&&
     (A.V->Znach.B->R.C->Atr==3)&&
     (Round(A.V->Znach.B->R.C->Val*B.C->Val,0.01)==1.0))
   return F=A.V->Znach.B->L;
  if((A.C->Atr==6)&&(A.B->Oper==3||A.B->Oper==4)&&IsCnst(B)){
   if(IsCnst(A.B->L)){
    F.C=TakeCnst(pow(A.B->L.C->Val,B.C->Val));
    return A.B->Oper==3?F*(A.B->R^B):F/(A.B->R^B);
   }
   if(IsCnst(A.B->R)){
    F.C=TakeCnst(pow(A.B->R.C->Val,B.C->Val));
    return A.B->Oper==3?F*(A.B->L^B):(1/F)*(A.B->L^B);
 }}}
 if(A.C->Atr==CNST&&B.C->Atr==CNST)
			return F.C=TakeCnst(pow(A.C->Val,B.C->Val)),F;
 if(IsMins(A)){
  if(B.C->Atr==CNST){ int v=ceil(B.C->Val);
   if(v==B.C->Val){
    F=(A.U->Arg)^B;     return v%2 ? -F : F;
 }}}
 return F.B=TakeBnop(6,A,B),F;
}

Form operator - (Form F){
 if(IsMins(F)) return F.U->Arg;
 if(F.C->Atr==CNST){
  if(F==Nul) return F;
  else       return F.C=TakeCnst(-F.C->Val),F;
 }
 return F.U=TakeUnop(1,F),F;
}
		 /* Перегружаемые функции */

Form sqrt (Form F){
 if(F.C->Atr==BNOP&&F.B->Oper==6&&F.B->R==Dva) return(F.B->L);
 if((F==Nul)||//(IsVary(F)&&IsCnst(F.V->Znach)&&(F.V->Znach==Nul)&&VaryNoOK(F.V))||
    (F==Odin)//||(IsVary(F)&&IsCnst(F.V->Znach)&&(F.V->Znach==Odin)&&VaryNoOK(F.V))
   ) return F;
 if(F.C->Atr==CNST) return F.C=TakeCnst(sqrt(F.C->Val)),F;
 F.U=TakeUnop(2,F),F; return F;
}

Form sin(Form F){
// if(F.C->Atr==UNOP&&F.U->Func==3) return F.U->Arg;
 if(F.C->Atr==UNOP&&F.U->Func==6) return F.U->Arg;
 if(F.C->Atr==UNOP&&F.U->Func==17){
//  Form f; f.C=Odin; F=sqrt(f-((F.U->Arg)^2));
  return sqrt(1-((F.U->Arg)^2));
 }
 if((F==Nul))//||(IsVary(F)&&IsCnst(F.V->Znach)&&(F.V->Znach==Nul)&&VaryNoOK(F.V)))
  return F;
 else if(F.C->Atr==UNOP&&F.U->Func==1){ F.U=TakeUnop(3,F.U->Arg); F=-F; }
 else F.U=TakeUnop(3,F);
 return F;
}

Form cos(Form F){
// if(F.C->Atr==UNOP&&F.U->Func==3) return cos(F.U->Arg);
 if(F.C->Atr==UNOP&&F.U->Func==17) return F.U->Arg;
 if(F.C->Atr==UNOP&&F.U->Func==6){
//  Form f; f.C=Odin; F=sqrt(f-((F.U->Arg)^2)); return F;
  return sqrt(1-((F.U->Arg)^2));
 }
 else if(F==Nul) F.C=Odin;
 else if(F==pi){ F.C=Odin; F.U=TakeUnop(1,F); }
 else if(F.C->Atr==UNOP&&F.U->Func==1) F.U=TakeUnop(4,F.U->Arg);
 else F.U=TakeUnop(4,F);
 return F;
}

Form tg(Form F){
// if(F.C->Atr==UNOP&&F.U->Func==3) return F.U->Arg;
 if(F.C->Atr==UNOP&&F.U->Func==7) return F.U->Arg;
 if(F==Nul||F==pi) return F.C=Nul,F;
 return F.U=TakeUnop(5,F),F;
}

Form arcsin(Form F){
// if(F.C->Atr==UNOP&&F.U->Func==3) return F.U->Arg;
// if(F.C->Atr==UNOP&&F.U->Func==4){
//  Form f; f.C=PIna2; return f+F.U->Arg;
// }
 if((F==Nul))//||(IsVary(F)&&IsCnst(F.V->Znach)&&(F.V->Znach==Nul)&&VaryNoOK(F.V)))
  return F;
 return F.U=TakeUnop(6,F),F;
}

Form arccos(Form F){
 return F.U=TakeUnop(17,F),F;
}

Form arctg(Form F){
 if(F.C->Atr==UNOP&&F.U->Func==3) return F.U->Arg;
 if(F.C->Atr==UNOP&&F.U->Func==5) return F.U->Arg;
 if((F==Nul))//||(IsVary(F)&&IsCnst(F.V->Znach)&&(F.V->Znach==Nul)&&VaryNoOK(F.V)))
  return F;
 return F.U=TakeUnop(7,F),F;
}

Form exp(Form F){
 if(F.C->Atr==UNOP&&F.U->Func==3){ F.C=::Exp; return 1/F; }
 if(F.C->Atr==UNOP&&F.U->Func==9) return F.U->Arg;
 if((F==Nul))//||(IsVary(F)&&IsCnst(F.V->Znach)&&(F.V->Znach==Nul)&&VaryNoOK(F.V)))
  return F.C=Odin,F;
 if((F==Odin))//||(IsVary(F)&&IsCnst(F.V->Znach)&&(F.V->Znach==Odin)&&VaryNoOK(F.V)))
  return F.C=::Exp,F;
 return F.U=TakeUnop(8,F),F;
}

Form ln(Form F){
 if(F.C->Atr==CNST&&!wcscmp(F.C->Name,L"exp")) return F.C=Odin,F;
 if(F.C->Atr==UNOP&&F.U->Func==8) return F.U->Arg;
 if((F==Odin))//||(IsVary(F)&&IsCnst(F.V->Znach)&&(F.V->Znach==Odin)&&VaryNoOK(F.V)))
  return F.C=Nul,F;
 else return F.U=TakeUnop(9,F),F;
}

Form sh(Form F){
 if(F.C->Atr==UNOP&&F.U->Func==12) return F.U->Arg;
 if((F==Nul))//||(IsVary(F)&&IsCnst(F.V->Znach)&&(F.V->Znach==Nul)&&VaryNoOK(F.V)))
  return F;
 else return F.U=TakeUnop(10,F),F;
}

Form ch(Form F){
 if(F.C->Atr==UNOP&&F.U->Func==12){
  Form f; f.C=Odin; F=sqrt(f+((F.U->Arg)^2)); return F;
 }
 if((F==Nul))//||(IsVary(F)&&IsCnst(F.V->Znach)&&(F.V->Znach==Nul)&&VaryNoOK(F.V)))
  return F.C=Odin,F;
 else return F.U=TakeUnop(11,F),F;
}

Form arsh(Form F){
 if(F.C->Atr==UNOP&&F.U->Func==10) return F.U->Arg;
 if((F==Nul))//||(IsVary(F)&&IsCnst(F.V->Znach)&&(F.V->Znach==Nul)&&VaryNoOK(F.V)))
  return F;
 else return F.U=TakeUnop(12,F),F;
}

Form abs (Form F){ if(!(F==Nul)) F.U=TakeUnop(13,F); return F; }

Form rand (Form F){ if(!(F==Nul)) F.U=TakeUnop(14,F); return F; }

Form ceil (Form F){ if(!(F==Nul)) F.U=TakeUnop(15,F); return F; }

Form floor(Form F){ if(!(F==Nul)) F.U=TakeUnop(16,F); return F; }

