#include "qry.h"

void comandoRq(FILE* txt, FILE* svg,HashFile hashQuadras,HashFile hashPessoas, int cep){
    if(txt == NULL || svg == NULL || hashQuadras == NULL || hashPessoas == NULL){
        printf("Erro em comandoRq\n");
        return;
    }
    //ver as pessoas que moram no cep

    
    //comparar com os endereços das pessoas para ver quem mora lá

    //transformar as pessoas em sem teto

    //remover a quadra
}

void comandoPq(FILE* svg, HashFile hashQuadras, HashFile hashPessoas, int cep);

void comandoCenso(FILE* txt, HashFile hashPessoas);

void comandoH(FILE* txt, HashFile hashPessoas);

void comandoNasc(HashFile hashPessoas, int cpf, char* nome, char* sobrenome, char sexo, int diaNasc, int mesNasc, int anoNasc){
    if(hashPessoas == NULL){
        printf("Erro em comandoNasc\n");
        return;
    }

    Pessoa nova = criarPessoa();
    setCPF(nova,cpf);
    setNome(nova, nome);
    setSobrenome(nova, sobrenome);
    setSexo(nova,sexo);
    setNascimento(nova, diaNasc,mesNasc,anoNasc);
}

void comandoRip(FILE* txt, HashFile hashPessoas, int cpf){
    if(hashPessoas == NULL){
        printf("Erro em comandoRip\n");
        return;
    }
    removerDadosHashFile(hashPessoas,cpf);
    
    fprintf(txt, "cpf do falecido: %d, nome do falecido: %s, sobrenome do falecido: %s, sexo do falecido: %c, data de nascimento: %d/%d/%d\n", cpf,getNome)
}

void comandoMud(FILE* svg, HashFile hashQuadras, HashFile hashPessoas, int cpf, int cep, char face, int num, char* complemento);

void comandoDspj(FILE* txt, FILE* svg, HashFile hashQuadras, HashFile hashPessoas,int cpf);

void lerQry(FILE* txt, FILE* qry, FILE* svg, HashFile hashQuadras, HashFile hashPessoas);