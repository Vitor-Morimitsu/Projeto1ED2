#include "pessoa.h"
#include <string.h>
#include <stdlib.h>

typedef struct{
    int CPF;
    char nome[32];
    char sobrenome[50];
    char sexo;
    int diaNascimento;
    int mesNascimento;
    int anoNascimento;
    int CEP;
    char face;
    int numero;
    char complemento[50];
}stPessoa;

Pessoa criarPessoa(){
    stPessoa *novaPessoa = malloc(sizeof(stPessoa));
    if(novaPessoa == NULL){
        printf("Erro ao criar pessoa\n");
        return NULL;
    }

    novaPessoa->CPF = 0;
    novaPessoa->diaNascimento = 0;
    novaPessoa->mesNascimento = 0;
    novaPessoa->anoNascimento = 0;
    novaPessoa->sexo = -1;
    novaPessoa->CEP = -1;
    novaPessoa->face = 'i'; //indefinido
    novaPessoa->numero = -1;
    
    return (Pessoa)novaPessoa;
}

void setCPF(Pessoa p, int cpf){
    if(p == NULL){
        printf("Erro ao settar o cpf\n");
    }

    stPessoa *pessoa =(stPessoa*)p;
    pessoa->CPF = cpf;
}

void setNome(Pessoa p, char* nome){
    if(p == NULL){
        printf("Erro ao settar nome\n");
        return;
    }
    stPessoa *pessoa =(stPessoa*)p;
    strcpy(pessoa->nome, nome);
}

void setSobrenome(Pessoa p, char* sobrenome){
    if(p == NULL){
        printf("Erro em settar o sobrenome\n");
        return;
    }

    stPessoa* pessoa = (stPessoa*)p;
    strcpy(pessoa->sobrenome, sobrenome);
}

void setSexo(Pessoa p, char sexo){
    if(p == NULL){
        printf("Erro em settar o sexo\n");
    }
    stPessoa* pessoa = (stPessoa*)p;
    pessoa->sexo = sexo;
}

void setNascimento(Pessoa p, int dia, int mes, int ano){
    if(p == NULL){
        printf("Erro em settar o nascimento\n");
    }
    stPessoa* pessoa = (stPessoa*)p;
    pessoa->diaNascimento = dia;
    pessoa->mesNascimento = mes;
    pessoa->anoNascimento = ano;
}

void setCEP(Pessoa p, int CEP){
    if(p == NULL){
        printf("Erro em setCEP\n");
        return;
    }

    stPessoa *pessoa = (stPessoa*)p;
    pessoa->CEP = CEP;
}

void setFace(Pessoa p, char face){
    if(p == NULL){
        printf("Erro em setFace\n");
        return;
    }
    stPessoa* pessoa = (stPessoa*)p;
    pessoa->face = face;
}

void setNum(Pessoa p, int num){
    if(p == NULL){
        printf("Erro em setNum\n");
        return;
    }
    stPessoa* pessoa = (stPessoa*)p;
    pessoa->numero = num;
}

void setComplemento(Pessoa p, char* complemento){
    if(p == NULL){
        printf("Erro em setComplemento\n");
        return;
    }

    stPessoa* pessoa =(stPessoa*)p;
    strcpy(pessoa->complemento, complemento);
}

int getCPF(Pessoa p){
    stPessoa* pessoa = (stPessoa*)p;

    return pessoa->CPF;
}

char* getNome(Pessoa p){
    stPessoa* pessoa = (stPessoa*)p;
    return pessoa->nome;
}

char* getSobrenome(Pessoa p){
    stPessoa* pessoa = (stPessoa*)p;
    return pessoa->sobrenome;
}

char getSexo(Pessoa p){
    stPessoa* pessoa = (stPessoa*)p;
    return pessoa->sexo;
}

int getDiaNascimento(Pessoa p){
    stPessoa* pessoa = (stPessoa*)p;
    return pessoa->diaNascimento;
}

int getMesNascimento(Pessoa p){
    stPessoa* pessoa = (stPessoa*)p;
    return pessoa->mesNascimento;
}

int getAnoNascimento(Pessoa p){
    stPessoa* pessoa = (stPessoa*)p;
    return pessoa->anoNascimento;
}

int getCEP(Pessoa p){
    return ((stPessoa*)p)->CEP;
}

char getFace(Pessoa p){
    return ((stPessoa*)p)->face;
}

int getNum(Pessoa p){
    return ((stPessoa*)p)->numero;
}

char* getComplemento(Pessoa p){
    return ((stPessoa*)p)->complemento;
}

void liberarPessoa(Pessoa p){
    if(p == NULL){
        printf("Erro em liberarPessoa\n");
        return;
    }
    stPessoa* pessoa = (stPessoa*)p;
    free(pessoa);
}

void escreverPessoaArquivo(Pessoa p, FILE* arquivo){
    if(p == NULL || arquivo == NULL){
        printf("Erro em escreverPessoaArquivo\n");
        return;
    }
    stPessoa* pessoa = (stPessoa*)p;
    fwrite(pessoa, sizeof(stPessoa), 1, arquivo);
}

Pessoa lerPessoaArquivo(FILE* arquivo, long offset){
    if(arquivo == NULL){
        printf("Erro em lerPessoaArquivo\n");
        return NULL;
    }
    stPessoa* pessoa = malloc(sizeof(stPessoa));
    if(pessoa == NULL) return NULL;

    fseek(arquivo, offset, SEEK_SET);
    int lidos = fread(pessoa, sizeof(stPessoa), 1, arquivo);
    if(lidos != 1){
        free(pessoa);
        return NULL;
    }
    return (Pessoa)pessoa;
}