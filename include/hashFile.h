#ifndef HASHFILE_H
#define HASHFILE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
    HashFile extensível genérica.
    Serve para armazenar os dados das quadras e das pessoas
    que serão manipuladas no trabalho.

    Arquivos criados por este módulo:
      ─ Pessoas : HASHFILE_PESSOAS_DIR  e  HASHFILE_PESSOAS_DADOS
      ─ Quadras : HASHFILE_QUADRAS_DIR  e  HASHFILE_QUADRAS_DADOS

    Arquivo de diretório (*.dir) — BINÁRIO:
        [4 bytes] int  profundidade global
        [N*8 bytes] long offsets dos buckets no arquivo de dados

    Arquivo de dados (*.txt) — TEXTO com blocos de tamanho FIXO:
        Cada bucket ocupa exatamente TAM_BUCKET_BYTES bytes,
        permitindo fseek direto via offset do diretório.
*/

/** Tamanho máximo (em bytes) da string serializada de um dado. */
#define HASHFILE_TAM_BUF 256

typedef void* HashFile;

/// @brief Cria ou reabre a HashFile com os arquivos especificados.
/// @param dirArq   Nome do arquivo de diretório binário (use as constantes HASHFILE_*_DIR)
/// @param dadosArq Nome do arquivo de dados texto      (use as constantes HASHFILE_*_DADOS)
/// @return HashFile pronta para uso, ou NULL em caso de erro.
HashFile criarHashFile(const char* dirArq, const char* dadosArq);

/// @brief Retorna a profundidade global da HashFile.
int getProfundidadeHash(HashFile hash);

/// @brief Insere ou atualiza o registro completo na HashFile. Se o bucket ficar cheio, realiza split automático.
/// @param hashFile HashFile já existente
/// @param chave Chave inteira (CPF para Pessoa, CEP para Quadra)
/// @param dado String serializada do registro completo
void inserirDadoHashFile(HashFile hashFile, char* chave, const char* dado);

/// @brief Busca uma chave na HashFile e preenche 'buf' com o registro completo.
/// @param hashFile HashFile já existente
/// @param chave Chave a buscar
/// @param buf Buffer de saída (deve ter pelo menos HASHFILE_TAM_BUF bytes)
/// @param tamBuf Tamanho do buffer (use HASHFILE_TAM_BUF)
/// @return 1 se encontrado, 0 se não encontrado
int buscarDadosHashFile(HashFile hashFile, char* chave, char* buf, int tamBuf);

/// @brief Remove logicamente um registro da HashFile (marcação de removido).
/// @param hashFile HashFile já existente
/// @param chave Chave do registro a remover
void removerDadosHashFile(HashFile hashFile, char* chave);

/// @brief Retorna o offset do bucket para uma chave no arquivo de dados.
/// @return Offset em bytes, ou -1 em caso de erro.
long getEnderecoDiretorioHashFile(HashFile hash, int chave);

/// @brief Fecha os arquivos e libera a HashFile.
void fecharHashFile(HashFile hashFile);

#endif