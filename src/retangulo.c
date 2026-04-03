#include "retangulo.h"
typedef struct Retangulo{
    int i;
    double x;
    double y;
    double w;
    double h;
    char* corb;
    char* corp;
}stRetangulo;

Retangulo criaRetangulo(int i, double x, double y, double w, double h, char* corb, char* corp){
    stRetangulo *r = (stRetangulo*)malloc(sizeof(stRetangulo));
    r->i = i;
    r->x = x;
    r->y = y;
    r->w = w;
    r->h = h;
    r->corb = (char*)malloc(strlen(corb) + 1);
    if(r->corb == NULL){
        printf("Erro ao alocar memória para cor da borda do retangulo.");
        free(r->corb);
        free(r);
        exit(1);
    }
    strcpy(r->corb, corb);

    r->corp = (char*)malloc(strlen(corp) + 1);
    if(r->corp == NULL){
        printf("Erro ao alocar memória para a cor de preenchimento do retangulo.");
        free(r->corp);
        free(r);
        exit(1);
    }
    strcpy(r->corp, corp);

    return((stRetangulo*)r);
}

int getIDRetangulo(Retangulo r){
    return((stRetangulo*)r)->i;
}

double getCoordXRetangulo(Retangulo r){
    return((stRetangulo*)r)->x;
}

double getCoordYRetangulo(Retangulo r){
    return((stRetangulo*)r)->y;
}

double getWRetangulo(Retangulo r){
    return((stRetangulo*)r)->w;
}

double getHRetangulo(Retangulo r){
    return((stRetangulo*)r)->h;
}

char* getCorBRetangulo(Retangulo r){
    return((stRetangulo*)r)->corb;
}

char* getCorPRetangulo(Retangulo r){
    return((stRetangulo*)r)->corp;
}

double getAreaRetangulo(Retangulo r){
    return((stRetangulo*)r)->w * ((stRetangulo*)r)->h;
}

void setIDRetangulo(Retangulo r, int i){
    ((stRetangulo*)r)->i = i;
}

void setCoordXRetangulo(Retangulo r, double x){
    ((stRetangulo*)r)->x = x;
}

void setCoordYRetangulo(Retangulo r, double y){
    ((stRetangulo*)r)->y = y;
}

void setWRetangulo(Retangulo r, double w){
    ((stRetangulo*)r)->w = w;
}

void setHRetangulo(Retangulo r, double h){
    ((stRetangulo*)r)->h = h;
}

void setCorBRetangulo(Retangulo r, char* corb){
    stRetangulo* ret = (stRetangulo*)r;
    free(ret->corb);
    ret->corb = (char*)malloc(strlen(corb) + 1);
    if(ret->corb == NULL){
        printf("Erro ao alocar memória para cor da borda do retangulo.");
        exit(1);
    }
    strcpy(ret->corb, corb);
}

void setCorPRetangulo(Retangulo r, char* corp){
    stRetangulo* ret = (stRetangulo*)r;
    free(ret->corp);
    ret->corp = (char*)malloc(strlen(corp) + 1);
    if(ret->corp == NULL){
        printf("Erro ao alocar memória para a cor de preenchimento do retangulo.");
        exit(1);
    }
    strcpy(ret->corp, corp);
}

void liberaRetangulo(Retangulo r){
    if(r == NULL) return;

    stRetangulo* ret = (stRetangulo*)r;
    free(ret->corb);
    free(ret->corp);

    free(ret);
}