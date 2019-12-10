#include "CA.h"
#include <string.h>
#include <math.h>

extern List *L,*LBeg;  extern Vary *V;  extern Elem *E;
extern int Line;       extern Arra *A;  extern Root *I,*O;
extern Cnst *C,*Pust,/**MinusTri,*MinusDva,*MinusOdin,*/*Nul,*Halb,*Odin,*Dva,
 *Tri,*Exp,*PIna6,*PIna4,*PIna3,*PIna2,*PI,*pi,*DvaPI,*Sqrt2,*Sqrt3,*Sqrt5;
extern FILE *ERRFILE;
extern char Inf[],ErrFile[];

		/* Перегружаемые операции */

bool operator == (Form A,Form  B){
// return A.C==B.C;
 if(A.C==B.C) return true;
 if(A.C->Atr!=B.C->Atr) return false;
 switch(A.C->Atr){
  case CNST: return A.C->Val==B.C->Val;
  case VARY: return !strcmp(A.V->Name,B.V->Name);
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

bool IsMins (Form F){ return ((F.C->Atr==UNOP)&&(F.U->Func==1)); }
bool IsSqrt (Form F){ return ((F.C->Atr==UNOP)&&(F.U->Func==2)); }
bool IsSin  (Form F){ return ((F.C->Atr==UNOP)&&(F.U->Func==3)); }
bool IsCos  (Form F){ return ((F.C->Atr==UNOP)&&(F.U->Func==4)); }
bool IsPust (Form F){ return ((F.C->Atr==PUST)&&(F.C==Pust)); }
bool IsCnst (Form F){ return ((F.C->Atr==CNST)&&!IsPust(F)); }
bool IsVary (Form F){ return (F.C->Atr==VARY); }
bool IsSumm (Form F){ return ((F.C->Atr==BNOP)&&(F.B->Oper==1)); }
bool IsDiff (Form F){ return ((F.C->Atr==BNOP)&&(F.B->Oper==2)); }
bool IsMult (Form F){ return ((F.C->Atr==BNOP)&&(F.B->Oper==3)); }
bool IsDivd (Form F){ return ((F.C->Atr==BNOP)&&(F.B->Oper==4)); }
bool IsPowr (Form F){ return ((F.C->Atr==BNOP)&&(F.B->Oper==5)); }

Form operator + (Form A,Form B){ Form F;
 if(A==Nul) return B;
 if(IsVary(A)&&IsCnst(A.V->Znach)&&(A.V->Znach==Nul)) return B;
 if(B==Nul) return   A;
 if(IsVary(B)&&IsCnst(B.V->Znach)&&(B.V->Znach==Nul)) return A;
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
 if(IsVary(B)&&IsCnst(B.V->Znach)&&(B.V->Znach==Nul)) return A;
 if(A==Nul)  return -B;
 if(IsVary(A)&&IsCnst(A.V->Znach)&&(A.V->Znach==Nul)) return -B;
 if(A==B)     return  F.C=Nul,F;
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
 if(IsVary(A)&&IsCnst(A.V->Znach)&&(A.V->Znach==Nul)) return A.V->Znach;
 if(B==Nul)  return B;
 if(IsVary(B)&&IsCnst(B.V->Znach)&&(B.V->Znach==Nul)) return B.V->Znach;
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
 if((A.C->Atr==6)&&(A.B->Oper==5)&&(A.B->L==B))
  return B^(A.B->R+1);
 if((B.C->Atr==6)&&(B.B->Oper==5)&&(B.B->L==A))
  return A^(B.B->R+1);
 F.B=TakeBnop(3,A,B); return F;
}

Form operator / (Form A,Form B){ Form F,La,Lb,Ra,Rb;
 if((B==Nul)||(IsVary(B)&&IsCnst(B.V->Znach)&&(B.V->Znach==Nul))){
  strcpy(Inf,"Формульное деление на ноль (Form A/Form B)."); Error(0,0,true);
  if(ERRFILE=fopen(ErrFile,"a")){
   PrintForm(true,ERRFILE,"Form A",A); PrintForm(true,ERRFILE,"Form B",B);
   fclose(ERRFILE);
  }
  F.C=Pust; return F;
 }
 if(A==Nul) return A;
 if(IsVary(A)&&IsCnst(A.V->Znach)&&(A.V->Znach==Nul)){ F.C=Nul; return F; }
 if(B==Odin) return A;
 if(IsVary(B)&&IsCnst(B.V->Znach)&&(B.V->Znach==Odin)) return A;
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
 if(IsMult(A)&&B==A.B->L) return B;
 if(IsMult(A)&&B==A.B->R) return A;
 if(IsDivd(B)&&IsCnst(B.B->L)){
  F.C=TakeCnst(1/B.B->L.C->Val); return F*(A*B.B->R);
 }
 if(IsDivd(B)&&IsCnst(B.B->R)) return B.B->R*(A/B.B->L);
 if(IsDivd(A)&&A.B->L==B) { F.C=Odin; return F/A.B->R; }
 if(IsDivd(B)) return A*B.B->R/B.B->L;
 return F.B=TakeBnop(4,A,B),F;
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
  if((A.C->Atr==6)&&(A.B->Oper==5)&&(A.B->R.C->Atr==3))
   if(Round(A.B->R.C->Val*B.C->Val,0.01)==1.0) return F=A.B->L;
   else{
    B.C=TakeCnst(A.B->R.C->Val*B.C->Val);
    return F=A.B->L^B;
   }
  if((A.C->Atr==4)&&(A.V->Znach.C->Atr==6)&&(A.V->Znach.B->Oper==5)&&
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
 return F.B=TakeBnop(5,A,B),F;
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
 if(F.C->Atr==BNOP&&F.B->Oper==5&&F.B->R==Dva) return(F.B->L);
 if((F==Nul)||(IsVary(F)&&IsCnst(F.V->Znach)&&(F.V->Znach==Nul))||
    (F==Odin)||(IsVary(F)&&IsCnst(F.V->Znach)&&(F.V->Znach==Odin))) return F;
 if(F==Dva) return F.C=Sqrt2,F;
 if(F==Tri) return F.C=Sqrt3,F;
 if(F.C->Atr==CNST&&F.C->Val==5) return F.C=Sqrt5,F;
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
 if((F==Nul)||(IsVary(F)&&IsCnst(F.V->Znach)&&(F.V->Znach==Nul))) return F;
 else if(F==pi||F==PI||F==DvaPI) F.C=Nul;
 else if(F==PIna6)        F.C=Halb;
 else if(F==PIna4)        F.C=TakeCnst(1/sqrt(2));
 else if(F==PIna3)        F.C=TakeCnst(sqrt(3)/2);
 else if(F==PIna2)        F.C=Odin;
 else                     F.U=TakeUnop(3,F);
 return F;
}

Form cos(Form F){
// if(F.C->Atr==UNOP&&F.U->Func==3) return cos(F.U->Arg);
 if(F.C->Atr==UNOP&&F.U->Func==17) return F.U->Arg;
 if(F.C->Atr==UNOP&&F.U->Func==6){
//  Form f; f.C=Odin; F=sqrt(f-((F.U->Arg)^2)); return F;
  return sqrt(1-((F.U->Arg)^2));
 }
 else if((F==Nul)||(IsVary(F)&&IsCnst(F.V->Znach)&&(F.V->Znach==Nul))||
         (F==DvaPI)) F.C=Odin;
 else if(F==pi||F==PI){ F.C=Odin; TakeUnop(1,F); }
 else if(F==PIna6) F.C=TakeCnst(sqrt(3)/2);
 else if(F==PIna4) F.C=TakeCnst(1/sqrt(2));
 else if(F==PIna3) F.C=Halb;
 else if(F==PIna2) F.C=Nul;
 else F.U=TakeUnop(4,F);
 return F;
}

Form tg(Form F){
// if(F.C->Atr==UNOP&&F.U->Func==3) return F.U->Arg;
 if(F.C->Atr==UNOP&&F.U->Func==7) return F.U->Arg;
 if(F==Nul||F==pi||F==PI||F==DvaPI) return F.C=Nul,F;
 if(F==PIna6)                return F.C=TakeCnst(sqrt(3)/3),F;
 if(F==PIna4)                return F.C=Odin,F;
 if(F==PIna3)                return F.C=TakeCnst(sqrt(3)),F;
		             return F.U=TakeUnop(5,F),F;
}

Form arcsin(Form F){
// if(F.C->Atr==UNOP&&F.U->Func==3) return F.U->Arg;
// if(F.C->Atr==UNOP&&F.U->Func==4){
//  Form f; f.C=PIna2; return f+F.U->Arg;
// }
 if((F==Nul)||(IsVary(F)&&IsCnst(F.V->Znach)&&(F.V->Znach==Nul))) return F;
 if((F==Halb)||(IsVary(F)&&IsCnst(F.V->Znach)&&(F.V->Znach==Halb))) return F.C=PIna6,F;
 if((F==Odin)||(IsVary(F)&&IsCnst(F.V->Znach)&&(F.V->Znach==Odin))) return F.C=PIna2,F;
 return F.U=TakeUnop(6,F),F;
}

Form arccos(Form F){
// if(F.C->Atr==UNOP&&F.U->Func==3) return F.U->Arg;
// if(F.C->Atr==UNOP&&F.U->Func==4){
//  Form f; f.C=PIna2; return f+F.U->Arg;
// }
 if((F==Nul)||(IsVary(F)&&IsCnst(F.V->Znach)&&(F.V->Znach==Nul))) return F.C=PIna2,F;
 if((F==Halb)||(IsVary(F)&&IsCnst(F.V->Znach)&&(F.V->Znach==Halb))) return F.C=PIna3,F;
 if((F==Odin)||(IsVary(F)&&IsCnst(F.V->Znach)&&(F.V->Znach==Odin))) return F.C=Nul,F;
 return F.U=TakeUnop(17,F),F;
}

Form arctg(Form F){
 if(F.C->Atr==UNOP&&F.U->Func==3) return F.U->Arg;
 if(F.C->Atr==UNOP&&F.U->Func==5) return F.U->Arg;
 if((F==Nul)||(IsVary(F)&&IsCnst(F.V->Znach)&&(F.V->Znach==Nul))) return F;
 if((F==Odin)||(IsVary(F)&&IsCnst(F.V->Znach)&&(F.V->Znach==Odin))) return F.C=PIna4,F;
 return F.U=TakeUnop(7,F),F;
}

Form exp(Form F){
 if(F.C->Atr==UNOP&&F.U->Func==3){ F.C=Exp; return 1/F; }
 if(F.C->Atr==UNOP&&F.U->Func==9) return F.U->Arg;
 if((F==Nul)||(IsVary(F)&&IsCnst(F.V->Znach)&&(F.V->Znach==Nul))) return F.C=Odin,F;
 if((F==Odin)||(IsVary(F)&&IsCnst(F.V->Znach)&&(F.V->Znach==Odin))) return F.C=Exp,F;
 return F.U=TakeUnop(8,F),F;
}

Form ln(Form F){
 if(F.C->Atr==CNST&&!strcmp(F.C->Name,"exp")) return F.C=Odin,F;
 if(F.C->Atr==UNOP&&F.U->Func==8) return F.U->Arg;
 if((F==Odin)||(IsVary(F)&&IsCnst(F.V->Znach)&&(F.V->Znach==Odin))) return F.C=Nul,F;
 else return F.U=TakeUnop(9,F),F;
}

Form sh(Form F){
 if(F.C->Atr==UNOP&&F.U->Func==12) return F.U->Arg;
 if((F==Nul)||(IsVary(F)&&IsCnst(F.V->Znach)&&(F.V->Znach==Nul))) return F;
 else return F.U=TakeUnop(10,F),F;
}

Form ch(Form F){
 if(F.C->Atr==UNOP&&F.U->Func==12){
  Form f; f.C=Odin; F=sqrt(f+((F.U->Arg)^2)); return F;
 }
 if((F==Nul)||(IsVary(F)&&IsCnst(F.V->Znach)&&(F.V->Znach==Nul))) return F.C=Odin,F;
 else return F.U=TakeUnop(11,F),F;
}

Form arsh(Form F){
 if(F.C->Atr==UNOP&&F.U->Func==10) return F.U->Arg;
 if((F==Nul)||(IsVary(F)&&IsCnst(F.V->Znach)&&(F.V->Znach==Nul))) return F;
 else return F.U=TakeUnop(12,F),F;
}

Form abs (Form F){ if(!(F==Nul)) F.U=TakeUnop(13,F); return F; }

Form rand (Form F){ if(!(F==Nul)) F.U=TakeUnop(14,F); return F; }

Form ceil (Form F){ if(!(F==Nul)) F.U=TakeUnop(15,F); return F; }

Form floor(Form F){ if(!(F==Nul)) F.U=TakeUnop(16,F); return F; }

