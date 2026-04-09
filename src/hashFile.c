#include "hashFile.h"

#define registro_vazio    0
#define registro_valido   1
#define registro_removido 2
#define tamBucket         4


typedef struct {
    int numero;
    long offset;  
    int  valido;  // 0=vazio, 1=válido, 2=removido
} Registro;

typedef struct {
    int       profundidadeLocal;
    int       qtdRegistro;
    Registro  registros[tamBucket];
} Bucket;

typedef struct {
    FILE* diretorio; 
    FILE* dados;     
    int   profundidade;
} stHashFile;

//pega os bits menos significativos do numero para determinar em qaul bucket o registro deve ir
int getBits(int numero, int profundidade) {
    int mascara = (1 << profundidade) - 1;
    return numero & mascara;
}

HashFile criarHashFile(const char* dirArq, const char* dadosArq) {
    FILE* diretorio = fopen(dirArq,  "r+b");
    FILE* dados     = fopen(dadosArq, "r+b");
    if (diretorio == NULL || dados == NULL) {
        if (diretorio) fclose(diretorio);
        if (dados)     fclose(dados);

        diretorio = fopen(dirArq, "w+b");
        if (diretorio == NULL) {
            printf("Erro ao criar %s\n", dirArq);
            return NULL;
        }
        dados = fopen(dadosArq, "w+b");
        if (dados == NULL) {
            printf("Erro ao criar %s\n", dadosArq);
            fclose(diretorio);
            return NULL;
        }

        int profundidadeInicial = 1;
        fwrite(&profundidadeInicial, sizeof(int), 1, diretorio);

        Bucket bucketVazio;
        memset(&bucketVazio, 0, sizeof(Bucket));
        bucketVazio.profundidadeLocal = 1;

        long offsetBucket0 = 0;
        long offsetBucket1 = (long)sizeof(Bucket);
        fwrite(&bucketVazio, sizeof(Bucket), 1, dados); /* bucket índice 0 */
        fwrite(&bucketVazio, sizeof(Bucket), 1, dados); /* bucket índice 1 */

        /* Registrar os offsets no diretório */
        fwrite(&offsetBucket0, sizeof(long), 1, diretorio);
        fwrite(&offsetBucket1, sizeof(long), 1, diretorio);
    }

    stHashFile* hash = malloc(sizeof(stHashFile));
    if (hash == NULL) {
        fclose(diretorio);
        fclose(dados);
        return NULL;
    }

    hash->diretorio = diretorio;
    hash->dados     = dados;

    /* Lê a profundidade do cabeçalho do arquivo */
    fseek(diretorio, 0, SEEK_SET);
    fread(&hash->profundidade, sizeof(int), 1, diretorio);

    return (HashFile)hash;
}

int getProfundidadeHash(HashFile hash) {
    return ((stHashFile*)hash)->profundidade;
}

long getEnderecoDiretorioHashFile(HashFile hash, int numero) {
    if (hash == NULL) {
        printf("Erro em getEnderecoDiretorioHashFile\n");
        return -1;
    }

    stHashFile* hashFile = (stHashFile*)hash;
    int   indice     = getBits(numero, hashFile->profundidade);
    long  posicao    = sizeof(int) + (long)indice * sizeof(long);

    fseek(hashFile->diretorio, posicao, SEEK_SET);

    long offsetBucket = -1;
    fread(&offsetBucket, sizeof(long), 1, hashFile->diretorio);
    return offsetBucket;
}

void inserirDadoHashFile(HashFile hashFile, int numero, long offset) {
    if (hashFile == NULL) {
        printf("Erro em inserirDadoHashFile\n");
        return;
    }

    stHashFile* hash = (stHashFile*)hashFile;

    long   offsetBucket = getEnderecoDiretorioHashFile(hashFile, numero);
    if (offsetBucket == -1) return;

    Bucket bucket;
    fseek(hash->dados, offsetBucket, SEEK_SET);
    fread(&bucket, sizeof(Bucket), 1, hash->dados);

    if (bucket.qtdRegistro < tamBucket) {
        int slot = bucket.qtdRegistro;
        bucket.registros[slot].numero = numero;
        bucket.registros[slot].offset = offset;
        bucket.registros[slot].valido = registro_valido;
        bucket.qtdRegistro++;

        fseek(hash->dados, offsetBucket, SEEK_SET);
        fwrite(&bucket, sizeof(Bucket), 1, hash->dados);
    } else {
        /* Split não implementado ainda */
        printf("Bucket cheio! Split não implementado. numero %d não foi inserido.\n", numero);
    }
}

long buscarDadosHashFile(HashFile hashFile, int numero) {
    if (hashFile == NULL) return -1;

    stHashFile* hash = (stHashFile*)hashFile;

    long offsetBucket = getEnderecoDiretorioHashFile(hashFile, numero);
    if (offsetBucket == -1) return -1;

    Bucket bucket;
    fseek(hash->dados, offsetBucket, SEEK_SET);
    fread(&bucket, sizeof(Bucket), 1, hash->dados);

    for (int i = 0; i < tamBucket; i++) {
        if (bucket.registros[i].valido == registro_valido &&
            bucket.registros[i].numero == numero) {
            return bucket.registros[i].offset;
        }
    }
    return -1;
}

void removerDadosHashFile(HashFile hashFile, int numero) {
    if (hashFile == NULL) {
        printf("Erro em removerDadosHashFile\n");
        return;
    }

    stHashFile* hash = (stHashFile*)hashFile;

    long offsetBucket = getEnderecoDiretorioHashFile(hashFile, numero);
    if (offsetBucket == -1) return;

    Bucket bucket;
    fseek(hash->dados, offsetBucket, SEEK_SET);
    fread(&bucket, sizeof(Bucket), 1, hash->dados);

    for (int i = 0; i < tamBucket; i++) {
        if (bucket.registros[i].valido == registro_valido &&
            bucket.registros[i].numero  == numero) {
            bucket.registros[i].valido = registro_removido;
            bucket.qtdRegistro--;

            fseek(hash->dados, offsetBucket, SEEK_SET);
            fwrite(&bucket, sizeof(Bucket), 1, hash->dados);
            return;
        }
    }
    printf("numero %d não encontrado para remoção.\n", numero);
}

void fecharHashFile(HashFile hashFile) {
    if (hashFile == NULL) {
        printf("Erro em fecharHashFile\n");
        return;
    }

    stHashFile* hash = (stHashFile*)hashFile;
    fclose(hash->diretorio);
    fclose(hash->dados);
    free(hash);
}