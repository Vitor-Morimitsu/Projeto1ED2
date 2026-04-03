#include "quadra.h"

typedef struct{
    int CEP;
    int x;
    int y;
    int w;
    int h;
    int sw;
    char* fill;
    char* cstrk;
}stQuadra;

Quadra criarQuadra(){
    stQuadra *novaQuadra = malloc(sizeof(novaQuadra));
    if(novaQuadra == NULL){
        printf("Erro em criarQuadra\n");
        return NULL;
    }

    return((Quadra*)novaQuadra);

}

void setCEPQuadra(Quadra q, int cepQuadra){
    if(q == NULL){
        printf("Erro em setCepQuadra\n");
        return;
    }
    stQuadra* quadra = (stQuadra*)q;

    quadra->CEP = cepQuadra;
}

void setDimensoesQuadra(Quadra q, int x, int y, int w, int h){
    if(q == NULL){
        printf("Erro em setDimensoesQuadra\n");
        return;
    }
    stQuadra* quadra = (stQuadra*)q;
    quadra->x = x;
    quadra->y = y;
    quadra->w = w;
    quadra->h = h;

}

void setPreenchimentoQuadra(Quadra q, int sw, char* cfill, char* cstrk){
    if(q == NULL){
        printf("Erro em setPreenchimentoQuadra");
        return;
    }
    stQuadra* quadra = (stQuadra*)q;
    quadra->sw = sw;
    strcpy(quadra->fill, cfill);
    strcpy(quadra->cstrk, cstrk);
}

int getCEPQuadra(Quadra q){
    return ((stQuadra*)q)->CEP;
}

void liberarQuadra(Quadra q){
    if(q == NULL){
        printf("Erro em liberarQuadra\n");
        return;
    }

    stQuadra* quadra = (stQuadra*)q;
    free(quadra->cstrk);
    free(quadra->fill);
    free(quadra);
}