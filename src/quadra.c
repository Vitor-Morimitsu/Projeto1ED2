#include "quadra.h"
#include <string.h>

typedef struct {
    char  CEP[32];
    float x, y, w, h;   // coordenadas e dimensões (float para SVG)
    char  sw[16];        // espessura da borda  (ex: "1.0px")
    char  fill[32];      // cor de preenchimento (ex: "black")
    char  cstrk[32];     // cor da borda
} stQuadra;

Quadra criarQuadra() {
    stQuadra* novaQuadra = malloc(sizeof(stQuadra)); // corrigido: sizeof(stQuadra)
    if (novaQuadra == NULL) {
        printf("Erro em criarQuadra\n");
        return NULL;
    }
    novaQuadra->CEP[0] = '\0';
    novaQuadra->x = novaQuadra->y = novaQuadra->w = novaQuadra->h = 0.0f;
    novaQuadra->sw[0] = novaQuadra->fill[0] = novaQuadra->cstrk[0] = '\0';

    return (Quadra)novaQuadra;
}

void setCEPQuadra(Quadra q, char* cepQuadra) {
    if (q == NULL) {
        printf("Erro em setCEPQuadra\n");
        return;
    }
    strncpy(((stQuadra*)q)->CEP, cepQuadra, 31);
    ((stQuadra*)q)->CEP[31] = '\0';
}

void setDimensoesQuadra(Quadra q, float x, float y, float w, float h) {
    if (q == NULL) {
        printf("Erro em setDimensoesQuadra\n");
        return;
    }
    stQuadra* quadra = (stQuadra*)q;
    quadra->x = x;
    quadra->y = y;
    quadra->w = w;
    quadra->h = h;
}

void setPreenchimentoQuadra(Quadra q, char* sw, char* cfill, char* cstrk) {
    if (q == NULL) {
        printf("Erro em setPreenchimentoQuadra\n");
        return;
    }
    stQuadra* quadra = (stQuadra*)q;
    strncpy(quadra->sw,    sw,    sizeof(quadra->sw)    - 1);
    strncpy(quadra->fill,  cfill, sizeof(quadra->fill)  - 1);
    strncpy(quadra->cstrk, cstrk, sizeof(quadra->cstrk) - 1);
    quadra->sw[sizeof(quadra->sw) - 1]       = '\0';
    quadra->fill[sizeof(quadra->fill) - 1]   = '\0';
    quadra->cstrk[sizeof(quadra->cstrk) - 1] = '\0';
}

char* getCEPQuadra(Quadra q) {
    return ((stQuadra*)q)->CEP;
}

float getXQuadra(Quadra q){
    return ((stQuadra*)q)->x;
}

float getYQuadra(Quadra q){
    return ((stQuadra*)q)->y;
}

float getWQuadra(Quadra q){
    return ((stQuadra*)q)->w;
}

float getHQuadra(Quadra q){
    return ((stQuadra*)q)->h;
}

void liberarQuadra(Quadra q) {
    if (q == NULL) {
        printf("Erro em liberarQuadra\n");
        return;
    }
    free(q);
}

char* serializarQuadra(Quadra q) {
    if (q == NULL) return NULL;
    stQuadra* quadra = (stQuadra*)q;
    char* buf = malloc(256);
    if (!buf) return NULL;
    /* Formato: CEP|x|y|w|h|sw|fill|cstrk */
    snprintf(buf, 256, "%s|%.4f|%.4f|%.4f|%.4f|%s|%s|%s",
             quadra->CEP, quadra->x, quadra->y, quadra->w, quadra->h,
             quadra->sw, quadra->fill, quadra->cstrk);
    return buf;
}

Quadra desserializarQuadra(const char* s) {
    if (s == NULL) return NULL;
    stQuadra* q = malloc(sizeof(stQuadra));
    if (!q) return NULL;
    memset(q, 0, sizeof(stQuadra));
    /* %[^|] le ate o proximo '|' (suporta espacos dentro dos campos) */
    sscanf(s, "%31[^|]|%f|%f|%f|%f|%15[^|]|%31[^|]|%31[^|]",
           q->CEP, &q->x, &q->y, &q->w, &q->h,
           q->sw, q->fill, q->cstrk);
    return (Quadra)q;
}

void escreverQuadraArquivo(Quadra q, FILE* arq) {
    if (q == NULL || arq == NULL) {
        printf("Erro em escreverQuadraArquivo\n");
        return;
    }
    fwrite((stQuadra*)q, sizeof(stQuadra), 1, arq);
}

Quadra lerQuadraArquivo(FILE* arq, long offset) {
    if (arq == NULL) return NULL;
    stQuadra* q = malloc(sizeof(stQuadra));
    if (!q) return NULL;
    fseek(arq, offset, SEEK_SET);
    if (fread(q, sizeof(stQuadra), 1, arq) != 1) {
        free(q);
        return NULL;
    }
    return (Quadra)q;
}