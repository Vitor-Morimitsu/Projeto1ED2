#include "pessoa.h"
#include <string.h>
#include <stdlib.h>

typedef struct{
    char CPF[32];
    char nome[32];
    char sobrenome[50];
    char sexo;
    int diaNascimento;
    int mesNascimento;
    int anoNascimento;
    char CEP[32];
    char face;
    int numero;
    char complemento[50];
}stPessoa;

Pessoa criarPessoa(){
    stPessoa *novaPessoa = calloc(1, sizeof(stPessoa));
    if(novaPessoa == NULL){
        printf("Erro ao criar pessoa\n");
        return NULL;
    }

    novaPessoa->sexo = -1;
    novaPessoa->face = 'i'; //indefinido
    novaPessoa->numero = -1;
    
    return (Pessoa)novaPessoa;
}

void setCPF(Pessoa p, char* cpf){
    if(p == NULL){
        printf("Erro ao settar o cpf\n");
    }

    stPessoa *pessoa =(stPessoa*)p;
    strncpy(pessoa->CPF, cpf, 31);
    pessoa->CPF[31] = '\0';
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

void setCEP(Pessoa p, char* CEP){
    if(p == NULL){
        printf("Erro em setCEP\n");
        return;
    }

    stPessoa *pessoa = (stPessoa*)p;
    strncpy(pessoa->CEP, CEP, 31);
    pessoa->CEP[31] = '\0';
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

char* getCPF(Pessoa p){
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

char* getCEP(Pessoa p){
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

char* serializarPessoa(Pessoa p) {
    if (p == NULL) return NULL;
    stPessoa* pessoa = (stPessoa*)p;
    char* buf = malloc(256);
    if (!buf) return NULL;
    /* Formato: CPF|nome|sobrenome|sexo|dia|mes|ano|CEP|face|num|complemento */
    snprintf(buf, 256, "%s|%s|%s|%c|%d|%d|%d|%s|%c|%d|%s",
             pessoa->CPF, pessoa->nome, pessoa->sobrenome, pessoa->sexo,
             pessoa->diaNascimento, pessoa->mesNascimento, pessoa->anoNascimento,
             pessoa->CEP, pessoa->face, pessoa->numero, pessoa->complemento);
    return buf;
}

Pessoa desserializarPessoa(const char* s) {
    if (s == NULL) return NULL;
    stPessoa* p = malloc(sizeof(stPessoa));
    if (!p) return NULL;
    memset(p, 0, sizeof(stPessoa));
    /* %[^|] le ate o proximo '|', preservando espacos internos (ex: "Apto 5") */
    sscanf(s, "%31[^|]|%31[^|]|%49[^|]|%c|%d|%d|%d|%31[^|]|%c|%d|%49[^|]",
           p->CPF, p->nome, p->sobrenome, &p->sexo,
           &p->diaNascimento, &p->mesNascimento, &p->anoNascimento,
           p->CEP, &p->face, &p->numero, p->complemento);
    return (Pessoa)p;
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