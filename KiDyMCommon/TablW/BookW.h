#ifndef BookWH
#define BookWH
struct Book {Book *Sled; char *NameCex, *NameProd;};

//---------------------------------------------------------------------------
//Book
int FindBook(Book *Rout, char *NameCex,char *NameProd, Book **B);
Book *TakeBook (Book **Rout, char *NameCex, char *NameProd);
int FileToBook(Book **Rout,char *NameFile);
int BookToFile(Book *B,char *NameFile);
int BookToFileA(Book *B,char *NameFile);
int FindeIndexBook(char *Name,Book *B);
char *FinedNameCexInBook(int In,Book *B);
#endif
