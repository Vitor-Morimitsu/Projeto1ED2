#include "hashFile.h"

#define VAZIO    0
#define VALIDO   1
#define REMOVIDO 2

#define TAM_BUCKET 10
#define TAM_DADO   HASHFILE_TAM_BUF   // 256 é o tamanho máximo da string

/* O formato do registro passa a ser: s=%d|k=%-31s|d=%s */
#define REG_CONTENT_SIZE  300  
#define REG_LINE_SIZE     (REG_CONTENT_SIZE + 1)        

#define HEADER_CONTENT_SIZE  30
#define HEADER_LINE_SIZE     (HEADER_CONTENT_SIZE + 1)  

#define TAM_BUCKET_BYTES  (HEADER_LINE_SIZE + TAM_BUCKET * REG_LINE_SIZE)

typedef struct {
    int  status;          /* VAZIO-0 / VALIDO-1 / REMOVIDO-2 */
    char chave[32];
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
    FILE* dumpArq;
    int   profundidade;   /* profundidade global */
} stHashFile;

/* ── Funções privadas ─────────────────────────────────────── */

static unsigned int hashString(const char *str) {
    if (!str) return 0;
    unsigned long hash = 5381;
    int c;
    while ((c = *str++))
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
    return (unsigned int)hash;
}

static int getBits(unsigned int numero, int prof) {
    return numero & ((1 << prof) - 1);
}

/* Retorna o offset do bucket para 'chave' lendo o arquivo diretório */
static long _lerOffsetBucket(stHashFile* h, const char* chave) {
    int  idx = getBits(hashString(chave), h->profundidade);
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

        int  s = 0;
        char k[32];
        char dado[TAM_DADO + 1];
        memset(k, 0, sizeof(k));
        memset(dado, 0, sizeof(dado));

        /* %31[^|] le ate pipe, %256[^\n] le ate o fim */
        sscanf(linhaR, "s=%d|k=%31[^|]|d=%256[^\n]", &s, k, dado);

        /* Remove espaços de padding do final */
        int lenk = (int)strlen(k);
        while (lenk > 0 && k[lenk - 1] == ' ') k[--lenk] = '\0';

        int len = (int)strlen(dado);
        while (len > 0 && dado[len - 1] == ' ') dado[--len] = '\0';

        b->registros[i].status = s;
        strncpy(b->registros[i].chave, k, 31);
        b->registros[i].chave[31] = '\0';
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
                          "s=%d|k=%-31s|d=%s",
                          b->registros[i].status,
                          b->registros[i].status == VALIDO ? b->registros[i].chave : "",
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
static void _splitBucket(stHashFile* h, const char* chave) {
    long   offsetAntigo = _lerOffsetBucket(h, chave);
    Bucket bucketAntigo;
    _lerBucket(h, offsetAntigo, &bucketAntigo);

    /* Se profLocal == profGlobal, dobrar o diretório antes */
    if (bucketAntigo.profLocal == h->profundidade) {
        _dobrarDiretorio(h);
    }


    int novoProfLocal = bucketAntigo.profLocal + 1;

    if (h->dumpArq) {
        fprintf(h->dumpArq, "[EXPANSAO] Split no bucket de offset %ld. Profundidade local subiu de %d para %d.\n", offsetAntigo, bucketAntigo.profLocal, novoProfLocal);
    }

    /* Criar dois novos buckets vazios */
    Bucket b0, b1;
    memset(&b0, 0, sizeof(Bucket));
    memset(&b1, 0, sizeof(Bucket));
    b0.profLocal = b1.profLocal = novoProfLocal;

    /* Redistribuir os registros válidos */
    for (int i = 0; i < TAM_BUCKET; i++) {
        if (bucketAntigo.registros[i].status == VALIDO) {
            /* O bit discriminante é o bit na posição bucketAntigo.profLocal */
            int novoBit = (getBits(hashString(bucketAntigo.registros[i].chave), novoProfLocal)
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
    int baseAntiga    = getBits(hashString(chave), bucketAntigo.profLocal);
    int totalEntradas = 1 << h->profundidade;

    for (int i = 0; i < totalEntradas; i++) {
        if ((i & mascAntiga) == baseAntiga) {
            int novoBit = (i >> bucketAntigo.profLocal) & 1;
            _escreverOffsetDir(h, i, (novoBit == 0) ? offsetAntigo : offsetB1);
        }
    }
    fflush(h->dir);
}

HashFile criarHashFile(const char* dirArq, const char* dadosArq, const char* dumpArq) {
    /* Tenta abrir arquivos existentes */
    FILE* dir   = fopen(dirArq,   "r+b");
    FILE* dados = fopen(dadosArq, "r+b");
    FILE* dump  = fopen(dumpArq,  "a"); // Append to keep log during execution

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
        stHashFile htmp = { dir, dados, dump, profInicial };
        Bucket bVazio;
        memset(&bVazio, 0, sizeof(Bucket));
        bVazio.profLocal = 1;
        _escreverBucket(&htmp, off0, &bVazio);
        _escreverBucket(&htmp, off1, &bVazio);
    }

    stHashFile* hash = malloc(sizeof(stHashFile));
    if (!hash) { fclose(dir); fclose(dados); if (dump) fclose(dump); return NULL; }

    hash->dir     = dir;
    hash->dados   = dados;
    hash->dumpArq = dump;

    /* Lê a profundidade global do cabeçalho do diretório */
    fseek(dir, 0, SEEK_SET);
    fread(&hash->profundidade, sizeof(int), 1, dir);

    return (HashFile)hash;
}

int getProfundidadeHash(HashFile hf) {
    if (!hf) return -1;
    return ((stHashFile*)hf)->profundidade;
}

long getEnderecoDiretorioHashFile(HashFile hash, char* chave) {
    if (!hash) return -1;
    return _lerOffsetBucket((stHashFile*)hash, chave);
}

FILE* getArquivoDadosHashFile(HashFile hash){
    stHashFile* hf = (stHashFile*)hash;
    return hf->dados;
}

void inserirDadoHashFile(HashFile hashFile, char* chave, const char* dado) {
    if (!hashFile || !chave || !dado) return;
    stHashFile* h = (stHashFile*)hashFile;

    /* Tenta inserir; se o bucket estiver cheio, faz split e repete */
    for (int tentativas = 0; tentativas < 64; tentativas++) {
        long   offsetBucket = _lerOffsetBucket(h, chave);
        Bucket b;
        _lerBucket(h, offsetBucket, &b);

        /* Se a chave já existe, apenas atualiza o dado */
        for (int i = 0; i < TAM_BUCKET; i++) {
            if (b.registros[i].status == VALIDO && strcmp(b.registros[i].chave, chave) == 0) {
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
                strncpy(b.registros[i].chave, chave, 31);
                b.registros[i].chave[31] = '\0';
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
    printf("Erro: nao foi possivel inserir chave %s apos multiplos splits.\n", chave);
}

int buscarDadosHashFile(HashFile hashFile, char* chave, char* buf, int tamBuf) {
    if (!hashFile || !chave || !buf) return 0;
    stHashFile* h = (stHashFile*)hashFile;

    long offsetBucket = _lerOffsetBucket(h, chave);
    if (offsetBucket < 0) return 0;

    Bucket b;
    _lerBucket(h, offsetBucket, &b);

    for (int i = 0; i < TAM_BUCKET; i++) {
        if (b.registros[i].status == VALIDO && strcmp(b.registros[i].chave, chave) == 0) {
            strncpy(buf, b.registros[i].dado, tamBuf - 1);
            buf[tamBuf - 1] = '\0';
            return 1;
        }
    }
    return 0;
}

void removerDadosHashFile(HashFile hashFile, char* chave) {
    if (!hashFile || !chave) return;
    stHashFile* h = (stHashFile*)hashFile;

    long offsetBucket = _lerOffsetBucket(h, chave);
    if (offsetBucket < 0) return;

    Bucket b;
    _lerBucket(h, offsetBucket, &b);

    for (int i = 0; i < TAM_BUCKET; i++) {
        if (b.registros[i].status == VALIDO && strcmp(b.registros[i].chave, chave) == 0) {
            b.registros[i].status = REMOVIDO;
            b.qtd--;
            _escreverBucket(h, offsetBucket, &b);
            return;
        }
    }
    printf("Chave %s nao encontrada para remocao.\n", chave);
}

void percorrerHashFile(HashFile hashFile, HashFileCallback cb, void* extra) {
    if (!hashFile || !cb) return;
    stHashFile* h = (stHashFile*)hashFile;

    long fim = _proximaPosFim(h);
    Bucket b;

    for (long offset = 0; offset < fim; offset += TAM_BUCKET_BYTES) {
        _lerBucket(h, offset, &b);
        for (int i = 0; i < TAM_BUCKET; i++) {
            if (b.registros[i].status == VALIDO) {
                cb(b.registros[i].chave, b.registros[i].dado, extra);
            }
        }
    }
}

void fecharHashFile(HashFile hashFile) {
    if (!hashFile) return;
    stHashFile* h = (stHashFile*)hashFile;

    /* Gera o texto representativo legivel no fim da execução */
    if (h->dumpArq) {
        fprintf(h->dumpArq, "\n\n=== REPRESENTACAO ESQUEMATICA DA HASHFILE ===\n");
        fprintf(h->dumpArq, "Profundidade Global: %d\n", h->profundidade);
        
        long fim = _proximaPosFim(h);
        fprintf(h->dumpArq, "Total de Buckets Instanciados: %ld\n\n", fim / TAM_BUCKET_BYTES);

        Bucket b;
        for (long offset = 0; offset < fim; offset += TAM_BUCKET_BYTES) {
            _lerBucket(h, offset, &b);
            fprintf(h->dumpArq, "--> [Bucket Offset: %08ld] | ProfLocal: %d | Ocupacao: %d/%d\n", offset, b.profLocal, b.qtd, TAM_BUCKET);
            if (b.qtd > 0) {
                for (int i = 0; i < TAM_BUCKET; i++) {
                    if (b.registros[i].status == VALIDO) {
                        fprintf(h->dumpArq, "      [+] Chave: %s\n", b.registros[i].chave);
                    }
                }
            } else {
                fprintf(h->dumpArq, "      (Bucket Vazio)\n");
            }
            fprintf(h->dumpArq, "\n");
        }
        fclose(h->dumpArq);
    }

    fclose(h->dir);
    fclose(h->dados);
    free(h);
}