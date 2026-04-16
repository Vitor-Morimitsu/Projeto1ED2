#ifndef QRY_H
#define QRY_H
/*
    Arquivo responsável por processar os comandos qry do primeiro trabalho de ED2
*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "quadra.h"
#include "pessoa.h"
#include "hashFile.h"
#include "svg.h"

/// @brief Remove a quadra com o cep passado. Moradores da quadra passam a ser sem-tetos
/// @param txt Arquivo txt 
/// @param svg Arquivo svg 
/// @param hashQuadras HashFile que contém as quadras 
/// @param hashPessoas Hash File que contém as pessoas 
/// @param cep CEP da quadra CEP  
void comandoRq(FILE* txt, FILE* svg,HashFile hashQuadras,HashFile hashPessoas, char* cep);

/// @brief Calcula o número de moradores da quadra(por face e total)
/// @param svg Arquivo svg Arquivo svg 
/// @param hashQuadras HashFile que contém as quadras 
/// @param hashPessoas Hash File que contém as pessoas 
/// @param cep CEP da quadra  
void comandoPq(FILE* svg, HashFile hashQuadras, HashFile hashPessoas, char* cep);

/// @brief Produz várias estatísticas sobre habitantes de Bitnópolis.
/// @param txt Arquivo txt 
/// @param hashPessoas Hash File que contém as pessoas 
void comandoCenso(FILE* txt, HashFile hashPessoas);

/// @brief Dados sobre habitante identificado por cpf.
/// @param txt Arquivo txt 
/// @param cpf Cpf que será buscado e informado
/// @param hashPessoas Hash File que contém as pessoas 
void comandoH(FILE* txt, HashFile hashPessoas, char* cpf);

/// @brief Pessoa nasce.
/// @param hashPessoas Hash File que contém as pessoas 
/// @param cpf CPF da pessoa 
/// @param nome Nome da pessoa
/// @param sobrenome Sobrenome da pessoa
/// @param sexo Sexo da pessoa
/// @param diaNasc Dia do nascimento
/// @param mesNasc Mês do nascimento
/// @param anoNasc Ano do nascimento
void comandoNasc(HashFile hashPessoas, char* cpf, char* nome, char* sobrenome, char sexo, int diaNasc, int mesNasc, int anoNasc);

/// @brief Pessoa falece.
/// @param txt Arquivo txt
/// @param svg Arquivo svg
/// @param hashPessoas Hash File que contém as pessoas 
/// @param hashQuadras HashFile que contém as quadras
/// @param cpf CPF da pessoa 
void comandoRip(FILE* txt, FILE* svg, HashFile hashPessoas, HashFile hashQuadras, char* cpf);

/// @brief Morador identificado por cpf muda-se para novo endereço.
/// @param svg Arquivo svg 
/// @param hashQuadras HashFile que contém as quadras 
/// @param hashPessoas Hash File que contém as pessoas 
/// @param cpf CPF da pessoa 
/// @param cep CEP da quadra 
/// @param face Face da quadra
/// @param num Numéro da moradia
/// @param complemento Complemento do endereço
void comandoMud(FILE* svg, HashFile hashQuadras, HashFile hashPessoas, char* cpf, char* cep, char face, int num, char* complemento);

/// @brief Morador identificado por cpf é despejado.
/// @param txt Arquivo txt 
/// @param svg Arquivo svg 
/// @param hashQuadras HashFile que contém as quadras 
/// @param hashPessoas Hash File que contém as pessoas 
/// @param cpf CPF da pessoa 
void comandoDspj(FILE* txt, FILE* svg, HashFile hashQuadras, HashFile hashPessoas,char* cpf);

/// @brief Função principal para ler os comando passados no arquivo .qry  
/// @param txt Arquivo txt 
/// @param svg Arquivo svg 
/// @param hashQuadras HashFile que contém as quadras 
/// @param hashPessoas Hash File que contém as pessoas 
void lerQry(FILE* txt, FILE* qry, FILE* svg, HashFile hashQuadras, HashFile hashPessoas);

#endif