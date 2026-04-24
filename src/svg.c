#include "svg.h"

void abrirSvg(FILE* arqSvg){
    if(arqSvg == NULL){
        printf("Erro em abrirSvg\n");
    }
    fprintf(arqSvg, "<svg xmlns:svg=\"http://www.w3.org/2000/svg\" xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\" width=\"15000\" height=\"15000\">\n");
}

void desenharRetanguloSVG(FILE* arqSvg, Quadra q){
    if(!arqSvg || !q){
        printf("Erro em desenharRetanguloSVG\n");
        return;
    }
    fprintf(arqSvg, "<rect x=\"%f\" y=\"%f\" width=\"%f\" height=\"%f\" fill=\"%s\" stroke=\"%s\" stroke-width=\"1\" />\n",
            getXQuadra(q),getYQuadra(q),getWQuadra(q),getHQuadra(q), getCfillQuadra(q), getCstrkQuadra(q));
}

//coloca um x no local da ancora da quadra removida
void comandoRqSvg(FILE* svg, float x, float y, float w, float h){
    if(!svg){
        printf("Erro em comandoRqSvg\n");
        return;
    }
    fprintf(svg, "<line x1=\"%f\" y1=\"%f\" x2=\"%f\" y2=\"%f\" stroke=\"red\" stroke-width=\"2\" />\n", x, y, x+w, y+h);
    fprintf(svg, "<line x1=\"%f\" y1=\"%f\" x2=\"%f\" y2=\"%f\" stroke=\"red\" stroke-width=\"2\" />\n", x+w, y, x, y+h);
}

void comandoMudSvg(FILE* svg, float x, float y, float w, float h, char face, char* cpf){
    if(!svg){
        printf("Erro em comandoMudSvg\n");
        return;
    }
    /* Rosa dos ventos INVERTIDA: N=baixo, S=cima, L=esquerda, O=direita */
    float mx, my;
    float lado = 12.0f; /* tamanho do quadradinho marcador */
    if(face == 'N' || face == 'n') {
        /* face Norte = borda inferior da quadra */
        mx = x + w / 2.0f;
        my = y + h;
    } else if(face == 'S' || face == 's') {
        /* face Sul = borda superior */
        mx = x + w / 2.0f;
        my = y;
    } else if(face == 'L' || face == 'l') {
        /* face Leste = borda esquerda */
        mx = x;
        my = y + h / 2.0f;
    } else { /* face O */
        /* face Oeste = borda direita */
        mx = x + w;
        my = y + h / 2.0f;
    }
    fprintf(svg, "<rect x=\"%f\" y=\"%f\" width=\"%f\" height=\"%f\" fill=\"red\" stroke=\"darkred\" stroke-width=\"1\" />\n",
            mx - lado/2.0f, my - lado/2.0f, lado, lado);
    if(cpf) fprintf(svg, "<text x=\"%f\" y=\"%f\" fill=\"white\" font-size=\"3\" text-anchor=\"middle\" dominant-baseline=\"middle\">%s</text>\n",
            mx, my, cpf);
}

//coloca uma cruz vermelha no local do endereço
void comandoRipSvg(FILE* svg,float x, float y){
    if(!svg){
        printf("Erro em comandoRipSvg\n");
        return;
    }
    fprintf(svg, "<line x1=\"%f\" y1=\"%f\" x2=\"%f\" y2=\"%f\" stroke=\"red\" stroke-width=\"4\" />\n", x, y-20, x, y+20);
    fprintf(svg, "<line x1=\"%f\" y1=\"%f\" x2=\"%f\" y2=\"%f\" stroke=\"red\" stroke-width=\"4\" />\n", x-20, y, x+20, y);
}

//colocar numero de moradores em cada face da quadra e na quadra como um todo no centro
void comandoPqSvg(FILE* svg, Quadra q, int total, int n, int s, int l, int o){
    if(!svg || !q){
        printf("Erro em comandoPqSvg\n");
        return;
    }
    float qx = getXQuadra(q);
    float qy = getYQuadra(q);
    float qw = getWQuadra(q);
    float qh = getHQuadra(q);

    // -- TOTALIZADOR: Bem no centro da quadra (x + metade da largura)
    fprintf(svg, "<text x=\"%f\" y=\"%f\" fill=\"black\" font-size=\"14\">Total: %d</text>\n", 
            qx + (qw/2), qy + (qh/2), total);
    
    // -- NORTE: Inferior da quadra (Eixo Y cresce, base da tela)
    fprintf(svg, "<text x=\"%f\" y=\"%f\" fill=\"red\" font-size=\"14\">%d</text>\n", 
            qx + (qw/2), qy + qh + 10, n);
            
    // -- SUL: Superior da quadra (Eixo Y menor, topo da tela)
    fprintf(svg, "<text x=\"%f\" y=\"%f\" fill=\"red\" font-size=\"14\">%d</text>\n", 
            qx + (qw/2), qy - 10, s);
            
    // -- LESTE: Esquerda da quadra (Eixo X menor)
    fprintf(svg, "<text x=\"%f\" y=\"%f\" fill=\"red\" font-size=\"14\">%d</text>\n", 
            qx - 15, qy + (qh/2), l);
            
    // -- OESTE: Direita da quadra (Eixo X cresce)
    fprintf(svg, "<text x=\"%f\" y=\"%f\" fill=\"red\" font-size=\"14\">%d</text>\n", 
            qx + qw + 5, qy + (qh/2), o);
}

//vai desenhar um círculo preto no local do despejo
void comandoDspjSvg(FILE* svg, float x, float y){
    if(!svg){
        printf("Erro em comandoDspjSvg\n");
        return;
    }
    fprintf(svg, "<circle cx=\"%f\" cy=\"%f\" r=\"15\" fill=\"black\" />\n", x, y);
}

void fecharSVG(FILE* arqSvg){
    if(!arqSvg){
        printf("Erro em fecharSvg\n");
        return;
    }
    fprintf(arqSvg, "</svg>\n");
}

