//---------------------------------------------------------------------------

#ifndef CodyTMCH
#define CodyTMCH
//---------------------------------------------------------------------------

class TCodTMC{
 public:  TCodTMC *Sled;
 private: char *Name,*CodOld,*CodNew;
 public:
//  TCodTMC(){ Sled=NULL; Name=CodOld=CodNew=NULL; };
  TCodTMC(char *N,char *NN);
  char *GetName(){ return Name; };
  char *GetCodNew(){ return CodNew; };
//  void SetName(char *S){ Name=S; };
//  void SetCodOld(char *N){ CodOld=N; };
  void SetCodNew(char *N);
  void Fill(char *NN);
 ~TCodTMC();
};

class TListCodyTMC{
 private: TCodTMC *R;
 public:
  TListCodyTMC();
  int FindTMC(char *Name,TCodTMC **t);
  TCodTMC *TakeTMC(char *Name,char *NN);
  void Load(int Ac,int Nk);
  void LoadSklad(int Ac,int Nk);
  void Edit();
  void Replace();
  void ChangeFile(char *File);
  void ChangeKeeps();
  void ChangeProducts(char *File);
 ~TListCodyTMC();
};
#endif
