#include "hashFile.h"

#define VAZIO    0
#define VALIDO   1
#define REMOVIDO 2

#define TAM_BUCKET 10
#define TAM_DADO   HASHFILE_TAM_BUF   // 256 é o tamanho máximo da string

#define REG_CONTENT_SIZE  (3 + 3 + 10 + 3 + TAM_DADO)  
#define REG_LINE_SIZE     (REG_CONTENT_SIZE + 1)        

#define HEADER_CONTENT_SIZE  30
#define HEADER_LINE_SIZE     (HEADER_CONTENT_SIZE + 1)  

#define TAM_BUCKET_BYTES  (HEADER_LINE_SIZE + TAM_BUCKET * REG_LINE_SIZE)

typedef struct {
    int  status;          /* VAZIO / VALIDO / REMOVIDO */
    int  chave;
    char dado[TAM_DADO];  /* string serializada do dado */
} Registro;

typedef struct {
    int      profLocal;
    int      qtd;         /* número de registros VALIDO no bucket */
    Registro registros[TAM_BUCKET];
} Bucket;

typedef struct {
    FILE* dir;
    FILE* dados;
    int   profundidade;   /* profundidade global */
} stHashFile;

/* ── Funções privadas ─────────────────────────────────────── */

static int getBits(int numero, int prof) {
    return numero & ((1 << prof) - 1);
}

/* Retorna o offset do bucket para 'chave' lendo o arquivo diretório */
static long _lerOffsetBucket(stHashFile* h, int chave) {
    int  idx = getBits(chave, h->profundidade);
    long pos = sizeof(int) + (long)idx * sizeof(long);
    fseek(h->dir, pos, SEEK_SET);
    long off = -1;
    fread(&off, sizeof(long), 1, h->dir);
    return off;
}

/* Escreve um offset no diretório para o índice 'idx' */
static void _escreverOffsetDir(stHashFile* h, int idx, long offset) {
    long pos = sizeof(int) + (long)idx * sizeof(long);
    fseek(h->dir, pos, SEEK_SET);
    fwrite(&offset, sizeof(long), 1, h->dir);
}

/* Lê um bucket completo a partir do offset no arquivo de dados */
static void _lerBucket(stHashFile* h, long offset, Bucket* b) {
    fseek(h->dados, offset, SEEK_SET);

    /* Lê cabeçalho (linha de tamanho fixo) */
    char linhaH[HEADER_LINE_SIZE + 1];
    memset(linhaH, 0, sizeof(linhaH));
    fread(linhaH, 1, HEADER_LINE_SIZE, h->dados);
    b->profLocal = 0;
    b->qtd       = 0;
    sscanf(linhaH, "prof_local=%d|qtd=%d", &b->profLocal, &b->qtd);

    /* Lê cada linha de registro */
    for (int i = 0; i < TAM_BUCKET; i++) {
        char linhaR[REG_LINE_SIZE + 1];
        memset(linhaR, 0, sizeof(linhaR));
        fread(linhaR, 1, REG_LINE_SIZE, h->dados);

        int  s = 0, k = 0;
        char dado[TAM_DADO + 1];
        memset(dado, 0, sizeof(dado));

        /* %256[^\n] lê todos os chars até '\n', incluindo espaços de padding */
        sscanf(linhaR, "s=%d|k=%d|d=%256[^\n]", &s, &k, dado);

        /* Remove espaços de padding do final */
        int len = (int)strlen(dado);
        while (len > 0 && dado[len - 1] == ' ') dado[--len] = '\0';

        b->registros[i].status = s;
        b->registros[i].chave  = k;
        strncpy(b->registros[i].dado, dado, TAM_DADO - 1);
        b->registros[i].dado[TAM_DADO - 1] = '\0';
    }
}

/* Escreve um bucket completo no offset especificado do arquivo de dados */
static void _escreverBucket(stHashFile* h, long offset, Bucket* b) {
    fseek(h->dados, offset, SEEK_SET);

    /* ── Cabeçalho ── */
    char linhaH[HEADER_LINE_SIZE + 1];
    int n = snprintf(linhaH, HEADER_LINE_SIZE + 1,
                     "prof_local=%d|qtd=%d", b->profLocal, b->qtd);
    for (int i = n; i < HEADER_CONTENT_SIZE; i++) linhaH[i] = ' ';
    linhaH[HEADER_CONTENT_SIZE] = '\n';
    fwrite(linhaH, 1, HEADER_LINE_SIZE, h->dados);

    /* ── Registros ── */
    for (int i = 0; i < TAM_BUCKET; i++) {
        char linhaR[REG_LINE_SIZE + 1];
        int n2 = snprintf(linhaR, REG_LINE_SIZE + 1,
                          "s=%d|k=%010d|d=%s",
                          b->registros[i].status,
                          b->registros[i].chave,
                          b->registros[i].dado);
        /* Preenche com espaços até REG_CONTENT_SIZE, depois '\n' */
        for (int j = n2; j < REG_CONTENT_SIZE; j++) linhaR[j] = ' ';
        linhaR[REG_CONTENT_SIZE] = '\n';
        fwrite(linhaR, 1, REG_LINE_SIZE, h->dados);
    }
    fflush(h->dados);
}

/* Retorna o offset do próximo byte livre no arquivo de dados */
static long _proximaPosFim(stHashFile* h) {
    fseek(h->dados, 0, SEEK_END);
    return ftell(h->dados);
}

/* Dobra o diretório: duplica o número de entradas */
static void _dobrarDiretorio(stHashFile* h) {
    int tamanhoAtual  = 1 << h->profundidade;
    int novaTamanho   = tamanhoAtual * 2;
    int novaProfund   = h->profundidade + 1;

    /* Lê todos os offsets atuais */
    long* offsets = malloc(tamanhoAtual * sizeof(long));
    if (!offsets) return;
    fseek(h->dir, sizeof(int), SEEK_SET);
    fread(offsets, sizeof(long), tamanhoAtual, h->dir);

    /* Atualiza profundidade no cabeçalho do diretório */
    fseek(h->dir, 0, SEEK_SET);
    fwrite(&novaProfund, sizeof(int), 1, h->dir);

    /* Escreve os novos offsets: entrada i aponta ao mesmo bucket de i % tamanhoAtual */
    for (int i = 0; i < novaTamanho; i++) {
        long off = offsets[i % tamanhoAtual];
        fwrite(&off, sizeof(long), 1, h->dir);
    }
    fflush(h->dir);
    free(offsets);
    h->profundidade = novaProfund;
}

/* Faz o split do bucket que contém 'chave' */
static void _splitBucket(stHashFile* h, int chave) {
    long   offsetAntigo = _lerOffsetBucket(h, chave);
    Bucket bucketAntigo;
    _lerBucket(h, offsetAntigo, &bucketAntigo);

    /* Se profLocal == profGlobal, dobrar o diretório antes */
    if (bucketAntigo.profLocal == h->profundidade) {
        _dobrarDiretorio(h);
    }

    int novoProfLocal = bucketAntigo.profLocal + 1;

    /* Criar dois novos buckets vazios */
    Bucket b0, b1;
    memset(&b0, 0, sizeof(Bucket));
    memset(&b1, 0, sizeof(Bucket));
    b0.profLocal = b1.profLocal = novoProfLocal;

    /* Redistribuir os registros válidos */
    for (int i = 0; i < TAM_BUCKET; i++) {
        if (bucketAntigo.registros[i].status == VALIDO) {
            /* O bit discriminante é o bit na posição bucketAntigo.profLocal */
            int novoBit = (getBits(bucketAntigo.registros[i].chave, novoProfLocal)
                          >> bucketAntigo.profLocal) & 1;
            Bucket* dest = (novoBit == 0) ? &b0 : &b1;
            dest->registros[dest->qtd++] = bucketAntigo.registros[i];
        }
    }

    /* b0 reutiliza offsetAntigo; b1 é alocado no final do arquivo */
    long offsetB1 = _proximaPosFim(h);
    _escreverBucket(h, offsetAntigo, &b0);
    _escreverBucket(h, offsetB1,     &b1);

    /* Atualizar entradas do diretório que apontavam para o bucket antigo */
    int mascAntiga    = (1 << bucketAntigo.profLocal) - 1;
    int baseAntiga    = getBits(chave, bucketAntigo.profLocal);
    int totalEntradas = 1 << h->profundidade;

    for (int i = 0; i < totalEntradas; i++) {
        if ((i & mascAntiga) == baseAntiga) {
            int novoBit = (i >> bucketAntigo.profLocal) & 1;
            _escreverOffsetDir(h, i, (novoBit == 0) ? offsetAntigo : offsetB1);
        }
    }
    fflush(h->dir);
}

HashFile criarHashFile(const char* dirArq, const char* dadosArq) {
    /* Tenta abrir arquivos existentes */
    FILE* dir   = fopen(dirArq,   "r+b");
    FILE* dados = fopen(dadosArq, "r+b");

    if (dir == NULL || dados == NULL) {
        /* Algum arquivo não existe — fecha o que porventura abriu e cria tudo do zero */
        if (dir)   fclose(dir);
        if (dados) fclose(dados);

        dir = fopen(dirArq, "w+b");
        if (!dir) { printf("Erro ao criar %s\n", dirArq); return NULL; }

        dados = fopen(dadosArq, "w+b");
        if (!dados) {
            printf("Erro ao criar %s\n", dadosArq);
            fclose(dir);
            return NULL;
        }

        /* ── Inicializar diretório binário ── */
        int profInicial = 1;
        fwrite(&profInicial, sizeof(int), 1, dir);

        long off0 = 0;
        long off1 = (long)TAM_BUCKET_BYTES;
        fwrite(&off0, sizeof(long), 1, dir);   /* bucket índice 0 */
        fwrite(&off1, sizeof(long), 1, dir);   /* bucket índice 1 */
        fflush(dir);

        /* ── Inicializar dois buckets vazios no arquivo de dados ── */
        stHashFile htmp = { dir, dados, profInicial };
        Bucket bVazio;
        memset(&bVazio, 0, sizeof(Bucket));
        bVazio.profLocal = 1;
        _escreverBucket(&htmp, off0, &bVazio);
        _escreverBucket(&htmp, off1, &bVazio);
    }

    stHashFile* hash = malloc(sizeof(stHashFile));
    if (!hash) { fclose(dir); fclose(dados); return NULL; }

    hash->dir   = dir;
    hash->dados = dados;

    /* Lê a profundidade global do cabeçalho do diretório */
    fseek(dir, 0, SEEK_SET);
    fread(&hash->profundidade, sizeof(int), 1, dir);

    return (HashFile)hash;
}

int getProfundidadeHash(HashFile hf) {
    if (!hf) return -1;
    return ((stHashFile*)hf)->profundidade;
}

long getEnderecoDiretorioHashFile(HashFile hash, int chave) {
    if (!hash) return -1;
    return _lerOffsetBucket((stHashFile*)hash, chave);
}

void inserirDadoHashFile(HashFile hashFile, int chave, const char* dado) {
    if (!hashFile || !dado) return;
    stHashFile* h = (stHashFile*)hashFile;

    /* Tenta inserir; se o bucket estiver cheio, faz split e repete */
    for (int tentativas = 0; tentativas < 64; tentativas++) {
        long   offsetBucket = _lerOffsetBucket(h, chave);
        Bucket b;
        _lerBucket(h, offsetBucket, &b);

        /* Se a chave já existe, apenas atualiza o dado */
        for (int i = 0; i < TAM_BUCKET; i++) {
            if (b.registros[i].status == VALIDO && b.registros[i].chave == chave) {
                strncpy(b.registros[i].dado, dado, TAM_DADO - 1);
                b.registros[i].dado[TAM_DADO - 1] = '\0';
                _escreverBucket(h, offsetBucket, &b);
                return;
            }
        }

        /* Procura slot vazio ou removido para inserção */
        for (int i = 0; i < TAM_BUCKET; i++) {
            if (b.registros[i].status != VALIDO) {
                b.registros[i].status = VALIDO;
                b.registros[i].chave  = chave;
                strncpy(b.registros[i].dado, dado, TAM_DADO - 1);
                b.registros[i].dado[TAM_DADO - 1] = '\0';
                b.qtd++;
                _escreverBucket(h, offsetBucket, &b);
                return;
            }
        }

        /* Bucket cheio → split e tenta novamente */
        _splitBucket(h, chave);
    }
    printf("Erro: nao foi possivel inserir chave %d apos multiplos splits.\n", chave);
}

int buscarDadosHashFile(HashFile hashFile, int chave, char* buf, int tamBuf) {
    if (!hashFile || !buf) return 0;
    stHashFile* h = (stHashFile*)hashFile;

    long offsetBucket = _lerOffsetBucket(h, chave);
    if (offsetBucket < 0) return 0;

    Bucket b;
    _lerBucket(h, offsetBucket, &b);

    for (int i = 0; i < TAM_BUCKET; i++) {
        if (b.registros[i].status == VALIDO && b.registros[i].chave == chave) {
            strncpy(buf, b.registros[i].dado, tamBuf - 1);
            buf[tamBuf - 1] = '\0';
            return 1;
        }
    }
    return 0;
}

void removerDadosHashFile(HashFile hashFile, int chave) {
    if (!hashFile) return;
    stHashFile* h = (stHashFile*)hashFile;

    long offsetBucket = _lerOffsetBucket(h, chave);
    if (offsetBucket < 0) return;

    Bucket b;
    _lerBucket(h, offsetBucket, &b);

    for (int i = 0; i < TAM_BUCKET; i++) {
        if (b.registros[i].status == VALIDO && b.registros[i].chave == chave) {
            b.registros[i].status = REMOVIDO;
            b.qtd--;
            _escreverBucket(h, offsetBucket, &b);
            return;
        }
    }
    printf("Chave %d nao encontrada para remocao.\n", chave);
}

void fecharHashFile(HashFile hashFile) {
    if (!hashFile) return;
    stHashFile* h = (stHashFile*)hashFile;
    fclose(h->dir);
    fclose(h->dados);
    free(h);
}