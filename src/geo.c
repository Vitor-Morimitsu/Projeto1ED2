#include "geo.h"
#include <string.h>

void lerGeo(FILE* geo, FILE* svg, HashFile hashFile) {
    if (geo == NULL || svg == NULL) {
        printf("Erro em lerGeo\n");
        return;
    }

    char linha[256];
    char comando[10];

    /* Cores e espessura correntes (definidas pelo comando 'cq', usadas pelo 'q') */
    char sw[16]    = "1.0px";
    char cfill[32] = "black";
    char cstrk[32] = "black";

    while (fgets(linha, sizeof(linha), geo) != NULL) {
        /* Ignora linhas vazias e comentários */
        if (linha[0] == '\n' || linha[0] == '#') continue;

        int lidos_cmd = sscanf(linha, "%9s", comando);
        if (lidos_cmd != 1) continue;

        if (strcmp(comando, "q") == 0) {
            /* Lê: q CEP x y w h */
            int   CEP;
            float x, y, w, h;
            int lidos = sscanf(&linha[strlen(comando)], " %d %f %f %f %f",
                               &CEP, &x, &y, &w, &h);
            if (lidos != 5) {
                printf("Erro ao ler comando 'q': formato invalido\n");
                continue;
            }

            /* Cria e preenche a quadra */
            Quadra novaQuadra = criarQuadra();
            if (novaQuadra == NULL) {
                printf("Erro ao criar quadra em lerGeo\n");
                continue;
            }
            setCEPQuadra(novaQuadra, CEP);
            setDimensoesQuadra(novaQuadra, x, y, w, h);
            setPreenchimentoQuadra(novaQuadra, sw, cfill, cstrk);

            /* Serializa e insere o registro completo na HashFile */
            char* dado = serializarQuadra(novaQuadra);
            inserirDadoHashFile(hashFile, CEP, dado);
            free(dado);

            liberarQuadra(novaQuadra);

        } else if (strcmp(comando, "cq") == 0) {
            /* Lê: cq sw cfill cstrk — define as cores para as próximas quadras */
            int lidos = sscanf(&linha[strlen(comando)], " %15s %31s %31s",
                               sw, cfill, cstrk);
            if (lidos != 3) {
                printf("Erro ao ler comando 'cq': formato invalido\n");
            }
        }
    }
}