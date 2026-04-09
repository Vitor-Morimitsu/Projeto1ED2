#include "qry.h"

void comandoRq(FILE* txt, FILE* svg,HashFile hashQuadras,HashFile hashPessoas, int cep);

void comandoPq(FILE* svg, HashFile hashQuadras, HashFile hashPessoas, int cep);

void comandoCenso(FILE* txt, HashFile hashPessoas);

void comandoH(FILE* txt, HashFile hashPessoas);

void comandoNasc(HashFile hashPessoas, int cpf, char* nome, char* sobrenome, char sexo, int diaNasc, int mesNasc, int anoNasc);

void comandoRip(HashFile hashPessoas, int cpf);

void comandoMud(FILE* svg, HashFile hashQuadras, HashFile hashPessoas, int cpf, int cep, char face, int num, char* complemento);

void comandoDspj(FILE* txt, FILE* svg, HashFile hashQuadras, HashFile hashPessoas,int cpf);

void lerQry(FILE* txt, FILE* qry, FILE* svg, HashFile hashQuadras, HashFile hashPessoas);