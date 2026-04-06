#include "pessoa.h"
#include <stdio.h>
#include <stdlib.h>

int main(){
    Pessoa novaPessoa = criarPessoa();
    if(novaPessoa == NULL){
        printf("Erro ao criar a pessoa\n");
        return 1;
    }

    int CPF = 1234567;
    setCPF(novaPessoa,CPF);

    char* nome = "Vitor";
    setNome(novaPessoa, nome);

    char* sobrenome = "Morimitsu";
    setSobrenome(novaPessoa, sobrenome);

    char sexo = 'm';
    setSexo(novaPessoa,sexo);

    int dia = 12;    
    int mes = 3;
    int ano = 2004;

    setNascimento(novaPessoa, dia, mes, ano);
    
    printf("Dados da pessoa:\n CPF: %d\n Nome: %s\n Sobrenome: %s\n Sexo: %c\n Data de aniver:%d/%d/%d\n", getCPF(novaPessoa), getNome(novaPessoa),
    getSobrenome(novaPessoa), getSexo(novaPessoa), getDiaNascimento(novaPessoa), getMesNascimento(novaPessoa), getAnoNascimento(novaPessoa));
    return 0;
}