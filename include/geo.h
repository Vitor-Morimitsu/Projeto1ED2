#ifndef GEO_H
#define GEO_H

#include <stdio.h>
#include <stdlib.h>
#include "quadra.h"
#include "hashFile.h"
#include "svg.h"

/*
    Arquivo .geo responsável por ler um arquivo geo e gerar as formas em svg.
*/

/// @brief Função responsável por ler um arquivo geo,criar as quadras e gerar um svg correspondente
/// @param geo Arquivo .geo já aberto
/// @param svg Arquivo .svg já aberto
void lerGeo(FILE* geo, FILE* svg, HashFile hashFile);

#endif