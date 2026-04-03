#ifndef QUADRA_H
#define QUADRA_H

#include <stdio.h>
#include <stdlib.h>

/*
    Arquivo .h destinado à criação e manupulação de quadras
*/

typedef void* Quadra;

/// @brief Cria e retorna uma quadra
/// @return Quadra
Quadra criarQuadra();

/// @brief Define o CEP de uma quadra
/// @param q Quadra Pré existente
/// @param cepQuadra CEP que será definido
void setCEPQuadra(Quadra q, int cepQuadra);

/// @brief Define as dimensões da quadra
/// @param q Quadra já existente
/// @param x Localização da âncora no eixo x
/// @param y Localização da âncora no eixo y
/// @param w Tamanho do comprimento
/// @param h Tamanho da altura
void setDimensoesQuadra(Quadra q, int x, int y, int w, int h);

/// @brief Define os parâmetros de preenchimento da quadra
/// @param q Quadra já existente
/// @param sw Espessura da borda
/// @param cfill Cor de preenchimento
/// @param cstrk Cor preenchimento da borda
void setPreenchimentoQuadra(Quadra q, int sw, char* cfill, char* cstrk);

/// @brief Retorna o CEP de uma quadra 
/// @param q Quadra já existente
/// @return CEP
int getCEPQuadra(Quadra q);








#endif