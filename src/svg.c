#include "svg.h"

void abrirSvg(FILE* arqSvg){
    if(arqSvg == NULL){
        printf("Erro em abrirSvg\n");
    }
    fprintf(arqSvg, "<svg xmlns:svg=\"http://www.w3.org/2000/svg\" ""xmlns=\"http://www.w3.org/2000/svg\" ""version=\"1.1\n");
}

void desenharRetanguloSVG(FILE* arqSvg, Retangulo r){
    if(!arqSvg || !r){
        printf("Erro em desenharRetanguloSVG\n");
        return;
    }
    fprintf(arqSvg, "<rect x=\"%f\" y=\"%f\" width=\"%f\" height=\"%f\" fill=\"%s\" stroke=\"%s\" stroke-width=\"1\" />\n",
            getCoordXRetangulo(r), getCoordYRetangulo(r), getWRetangulo(r), getHRetangulo(r),
            getCorPRetangulo(r), getCorBRetangulo(r));
}

//coloca um x no local da ancora da quadra removida
void comandoRqSvg(FILE* svg, float x, float y){
    if(!svg){
        printf("Erro em comandoRqSvg\n");
        return;
    }
    fprintf(svg, "<line x1=\"%f\" y1=\"%f\" x2=\"%f\" y2=\"%f\" stroke=\"red\" stroke-width=\"2\" />\n", x-5, y-5, x+5, y+5);
    fprintf(svg, "<line x1=\"%f\" y1=\"%f\" x2=\"%f\" y2=\"%f\" stroke=\"red\" stroke-width=\"2\" />\n", x+5, y-5, x-5, y+5);
}

void comandoMudSvg(FILE* svg, float x , float y, char* cpf){
    if(!svg){
        printf("Erro em comandoMudSvg\n");
        return;
    }
    fprintf(svg, "<rect x=\"%f\" y=\"%f\" width=\"10\" height=\"10\" fill=\"none\" stroke=\"red\" stroke-width=\"1\" />\n", x-5, y-5);
    if(cpf) fprintf(svg, "<text x=\"%f\" y=\"%f\" fill=\"black\" font-size=\"8\">%s</text>\n", x-5, y+15, cpf);
}

//coloca uma cruz vermelha no local do endereço
void comandoRipSvg(FILE* svg,float x, float y){
    if(!svg){
        printf("Erro em comandoRipSvg\n");
        return;
    }
    fprintf(svg, "<line x1=\"%f\" y1=\"%f\" x2=\"%f\" y2=\"%f\" stroke=\"red\" stroke-width=\"2\" />\n", x, y-6, x, y+6);
    fprintf(svg, "<line x1=\"%f\" y1=\"%f\" x2=\"%f\" y2=\"%f\" stroke=\"red\" stroke-width=\"2\" />\n", x-6, y, x+6, y);
}

//colocar numero de moradores em cada face da quadra e na quadra como um todo no centro
void comandoPqSvg(FILE* svg, float x, float y){
    if(!svg){
        printf("Erro em comandoPqSvg\n");
        return;
    }
    //-----------------------------------------terminar
}

//vai desenhar um círculo preto no local do despejo
void comandoDspjSvg(FILE* svg, float x, float y){
    if(!svg){
        printf("Erro em comandoDspjSvg\n");
        return;
    }
    fprintf(svg, "<circle cx=\"%f\" cy=\"%f\" r=\"4\" fill=\"black\" />\n", x, y);
}

void fecharSVG(FILE* arqSvg){
    if(!arqSvg){
        printf("Erro em fecharSvg\n");
        return;
    }
    fprintf(arqSvg, "</svg>\n");
}

