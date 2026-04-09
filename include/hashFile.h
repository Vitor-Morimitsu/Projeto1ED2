#ifndef HASHFILE_H
#define HASHFILE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
    Arquivo responsável por criar e operar uma Hash File extensível
*/

typedef void* HashFile;

/// @brief Cria ou reabre a HashFile com os arquivos especificados
/// @param dirArq  Nome do arquivo de diretório (ex: "dir_pessoas.bin")
/// @param dadosArq Nome do arquivo de dados   (ex: "dados_pessoas.bin")
/// @return HashFile pronta para uso, ou NULL em caso de erro
HashFile criarHashFile(const char* dirArq, const char* dadosArq);

/// @brief Retorna o índice do diretório para um dado numero
/// @param numero numero da pessoa
/// @param profundidade Profundidade global atual do diretório
/// @return Índice entre 0 e 2^profundidade - 1
int getBits(int numero, int profundidade);

/// @brief Retorna a profundidade global da HashFile
/// @param hash HashFile já existente
/// @return Profundidade global
int getProfundidadeHash(HashFile hash);

/// @brief Insere o mapeamento numero → offset no arquivo pessoas.bin
/// @param hashFile HashFile já existente
/// @param numero numero da pessoa
/// @param offset Offset da Pessoa no arquivo pessoas.bin
void inserirDadoHashFile(HashFile hashFile, int numero, long offset);

/// @brief Busca um numero na HashFile e retorna o offset em pessoas.bin
/// @param hashFile HashFile já existente
/// @param numero
/// @return Offset em pessoas.bin, ou -1 se não encontrado
long buscarDadosHashFile(HashFile hashFile, int numero);

/// @brief Remove um numero da HashFile (remoção lógica por marcação)
/// @param hashFile HashFile já existente
/// @param numero numero da pessoa a remover
void removerDadosHashFile(HashFile hashFile, int numero);

/// @brief Retorna o offset do bucket em dados.bin para um dado numero
/// @param hash HashFile já existente
/// @param numero numero da pessoa
/// @return Offset do bucket em dados.bin, ou -1 em erro
long getEnderecoDiretorioHashFile(HashFile hash, int numero);

/// @brief Fecha e libera a HashFile
/// @param hashFile HashFile a fechar
void fecharHashFile(HashFile hashFile);

#endif