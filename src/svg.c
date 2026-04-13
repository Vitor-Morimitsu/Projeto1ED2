#include "svg.h"

void abrirSvg(FILE* arqSvg){
    if(arqSvg == NULL){
        printf("Erro em abrirSvg\n");
    }
    fprintf(arqSvg, "<svg xmlns:svg=\"http://www.w3.org/2000/svg\" ""xmlns=\"http://www.w3.org/2000/svg\" ""version=\"1.1\n");
}

void desenharRetanguloSVG(FILE* arqSvg, Retangulo r);

void inserirXSvg(FILE* svg, float x, float y);

void inserirCruzSvg(FILE* svg,float x, float y);

void inserirMoradoresSvg(FILE* svg, float x, float y);

void fecharSVG(FILE* arqSvg){
    fprintf(arqSvg, "</svg>\n");
}

