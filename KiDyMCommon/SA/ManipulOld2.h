//---------------------------------------------------------------------------

#ifndef ManipulOld2H
#define ManipulOld2H
//---------------------------------------------------------------------------
#include "CA.h"

void TakeCoSinFi(Parm *P);
void MultTransMatrVect(Form M[3][3],Form V[3]);
void MultMatrVect(Form M[3][3],Form V[3]);
void AddVect(Form A[3],Form B[3],Form C[3]);
void MultMatrMatr(Form A[3][3],Form B[3][3],Form C[3][3]);
void MultMatrMatr(Form A[3][3],Form B[3][3]);
void MatrMultMatr(Form A[4][4],Form B[4][4]);
void PrintFormVect(char *File,char *Comm,char *NameVect,Form V[3]);
void PrintFormMatr(char *File,char *Comm,char *NameMatr,Form M[3][3]);

#endif
