#include "pessoa.h"

typedef struct{
    int CPF;
    char nome[32];
    char sobrenome[50];
    int sexo;
    int diaNascimento;
    int mesNascimento;
    int anoNascimento;
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
    }
    stPessoa *pessoa =(stPessoa*)p;
    char* nomePassado = malloc(sizeof(strlen(nome)));
    if(nomePassado == NULL) return NULL;

    strcpy(pessoa->nome,nomePassado);
}

void setSobrenome(Pessoa p, char* sobrenome){
    if(p == NULL){
        printf("Erro em settar o sobrenome\n");
    }

    stPessoa* pessoa = (stPessoa*)p;

    char* sobrenomePassado = malloc(sizeof(strlen(sobrenome)));
    if(sobrenomePassado == NULL) return NULL;

    strcpy(pessoa->sobrenome, sobrenomePassado);
}

void setSexo(Pessoa p, int sexo){
    //0-homem e 1-mulher
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

int getSexo(Pessoa p){
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