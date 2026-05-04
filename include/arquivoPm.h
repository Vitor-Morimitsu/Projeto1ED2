#ifndef ARQUIVOPM_H
#define ARQUIVOPM_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pessoa.h"
#include "hashFile.h"

/*
    Módulo responsável pela leitura e processamento de um arquivo .pm.
*/

/// @brief Lê o arquivo .pm e processa os comandos p e m
/// @param arquivoPm Arquivo .pm já aberto para leitura
/// @param hashFile  HashFile já criada
/// @param pessoasArq Arquivo pessoas.bin já aberto para leitura e escrita
void lerPm(FILE* arquivoPm, HashFile hashFile, FILE* pessoasArq);

/// @brief Cria uma Pessoa a partir dos dados do comando 'p', grava em pessoas.bin e insere o mapeamento CPF→offset na HashFile
/// @param hashFile   HashFile já criada
/// @param pessoasArq Arquivo pessoas.bin aberto para escrita
/// @param CPF CPF da pessoa
/// @param nome Nome da pessoa
/// @param sobrenome Sobrenome da pessoa
/// @param sexo Sexo da pessoa
/// @param diaNascimento Dia de nascimento
/// @param mesNascimento Mês de nascimento
/// @param anoNascimento Ano de nascimento
void comandoP(HashFile hashFile, FILE* pessoasArq,char* CPF, char* nome, char* sobrenome, char sexo,int diaNascimento, int mesNascimento, int anoNascimento);

/// @brief Atualiza os dados de endereço de uma Pessoa já existente em pessoas.bin
/// @param hashFile   HashFile já criada (para localizar a Pessoa pelo CPF)
/// @param pessoasArq Arquivo pessoas.bin aberto para leitura e escrita
/// @param CPF CPF da pessoa a atualizar
/// @param CEP CEP do endereço
/// @param face Face da quadra
/// @param num Número do imóvel
/// @param complemento Complemento do endereço
void comandoM(HashFile hashFile, FILE* pessoasArq,char* CPF, char* CEP, char face, int num, char* complemento);

#endif