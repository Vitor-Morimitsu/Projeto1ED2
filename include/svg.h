#ifndef SVG_H
#define SVG_H
/*
    Arquivo responsável por manipular um svg para o primeiro trabalho de ED2
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "quadra.h"

//a rosa dos ventos é invertida, Sul está no norte,etc

/*
    Função responsável por abrir o arquivo Svg passado como parâmetro aberto na main
*/
void abrirSvg(FILE* arqSvg);

/*
    Função responsável por gerar uma quadra em SVG.
*/
void desenharRetanguloSVG(FILE* arqSvg, Quadra q);

///colocar um pequeno X vermelho no local da âncora da quadra removida.
void comandoRqSvg(FILE* svg, float x, float y);

//marcar o endereço de destino com um pequeno quadrado vermelho no local de destino. Colocar o cpf dentro do quadrado (usar fonte minúscula)
void comandoMudSvg(FILE* svg, float x , float y, char* cpf);

/// Colocar uma pequena cruz vermelha no local do endereço (se morador)
void comandoRipSvg(FILE* svg,float x, float y);

///colocar número de moradores de cada face (próximo ao limite da face) e,no centro da quadra, o número total de moradores da quadra
void comandoPqSvg(FILE* svg, Quadra q, int total, int n, int s, int l, int o);

/// @brief colocar um pequeno círculo preto no local do despejo.
/// @param svg Arquivo svg
/// @param x coordenada x do círculo
/// @param y coordenada y do círculo
void comandoDspjSvg(FILE* svg, float x, float y);

/// @brief Realiza o comando de encerramento do arquivo svg
/// @param arqSvg Arquivo
void fecharSVG(FILE* arqSvg);

#endif