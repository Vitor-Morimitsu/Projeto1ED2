#ifndef HASHFILE_H
#define HASHFILE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
    Arquivo responsável por criar e operar uma Hash File extensível
*/

typedef void* HashFile;

/// @brief Cria ou reabre a HashFile (arquivos diretorio.bin e dados.bin)
/// @return HashFile pronta para uso, ou NULL em caso de erro
HashFile criarHashFile();

/// @brief Retorna o índice do diretório para um dado CPF
/// @param CPF CPF da pessoa
/// @param profundidade Profundidade global atual do diretório
/// @return Índice entre 0 e 2^profundidade - 1
int getBits(int CPF, int profundidade);

/// @brief Retorna a profundidade global da HashFile
/// @param hash HashFile já existente
/// @return Profundidade global
int getProfundidadeHash(HashFile hash);

/// @brief Insere o mapeamento CPF → offset no arquivo pessoas.bin
/// @param hashFile HashFile já existente
/// @param CPF CPF da pessoa
/// @param offset Offset da Pessoa no arquivo pessoas.bin
void inserirDadoHashFile(HashFile hashFile, int CPF, long offset);

/// @brief Busca um CPF na HashFile e retorna o offset em pessoas.bin
/// @param hashFile HashFile já existente
/// @param CPF CPF da pessoa a buscar
/// @return Offset em pessoas.bin, ou -1 se não encontrado
long buscarDadosHashFile(HashFile hashFile, int CPF);

/// @brief Remove um CPF da HashFile (remoção lógica por marcação)
/// @param hashFile HashFile já existente
/// @param CPF CPF da pessoa a remover
void removerDadosHashFile(HashFile hashFile, int CPF);

/// @brief Retorna o offset do bucket em dados.bin para um dado CPF
/// @param hash HashFile já existente
/// @param CPF CPF da pessoa
/// @return Offset do bucket em dados.bin, ou -1 em erro
long getEnderecoDiretorioHashFile(HashFile hash, int CPF);

/// @brief Fecha e libera a HashFile
/// @param hashFile HashFile a fechar
void fecharHashFile(HashFile hashFile);

#endif