#include "quadra.h"
#include "hashFile.h"
#include <stdio.h>
#include <stdlib.h>

/* Helper ─────────────────────────────────────── */
static void imprimirQuadra(Quadra q) {
    printf("  CEP       : %s\n",          getCEPQuadra(q));
}

static void inserirQ(HashFile hash, char* cep, float x, float y, float w, float h,
                     const char* sw, const char* fill, const char* cstrk) {
    Quadra q = criarQuadra();
    setCEPQuadra(q, cep);
    setDimensoesQuadra(q, x, y, w, h);
    setPreenchimentoQuadra(q, (char*)sw, (char*)fill, (char*)cstrk);

    char* s = serializarQuadra(q);
    inserirDadoHashFile(hash, cep, s);
    free(s);
    liberarQuadra(q);
}

static void buscarEImprimir(HashFile hash, char* cep) {
    char buf[HASHFILE_TAM_BUF];
    printf("Buscando CEP %s... ", cep);
    if (buscarDadosHashFile(hash, cep, buf, HASHFILE_TAM_BUF)) {
        printf("ENCONTRADO — dado: %s\n", buf);
        Quadra q = desserializarQuadra(buf);
        imprimirQuadra(q);
        liberarQuadra(q);
    } else {
        printf("NAO ENCONTRADO\n");
    }
}

/* ── main ────────────────────────────────────── */
int main() {
    /* Remove arquivos antigos para iniciar limpo */
    remove(HASHFILE_QUADRAS_DIR);
    remove(HASHFILE_QUADRAS_DADOS);

    HashFile hash = criarHashFile(HASHFILE_QUADRAS_DIR, HASHFILE_QUADRAS_DADOS);
    if (!hash) {
        printf("Erro ao criar HashFile\n");
        return 1;
    }
    printf("HashFile criada. Profundidade inicial: %d\n\n", getProfundidadeHash(hash));

    /* ── Inserções ── */
    printf("=== INSERCOES ===\n");
    inserirQ(hash, "10001-000", 25.0f,  50.32f, 60.32f, 70.70f, "1.0px", "black",  "blue");
    inserirQ(hash, "20002-111", 10.0f,  15.00f,100.00f, 80.00f, "2.0px", "red",    "black");
    inserirQ(hash, "30003-222",  0.0f,   0.00f, 50.00f, 50.00f, "0.5px", "white",  "grey");
    inserirQ(hash, "40004-333", 12.5f, 100.00f, 80.00f, 40.00f, "1.5px", "yellow", "green");
    printf("4 quadras inseridas com sucesso!\n\n");

    /* ── Buscas ── */
    printf("=== BUSCAS ===\n");
    buscarEImprimir(hash, "10001-000");
    buscarEImprimir(hash, "20002-111");
    buscarEImprimir(hash, "30003-222");
    buscarEImprimir(hash, "40004-333");
    buscarEImprimir(hash, "99999-999");  /* nao existe */

    /* ── Remoção ── */
    printf("\n=== REMOCAO ===\n");
    printf("Removendo CEP 20002-111...\n");
    removerDadosHashFile(hash, "20002-111");
    buscarEImprimir(hash, "20002-111");  /* deve retornar NAO ENCONTRADO */

    /* ── Profundidade final ── */
    printf("\nProfundidade final do diretorio: %d\n", getProfundidadeHash(hash));

    fecharHashFile(hash);
    printf("Arquivos gerados: quadras.dir (binario), quadras.txt (texto)\n");
    return 0;
}