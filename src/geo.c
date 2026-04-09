#include "geo.h"

void lerGeo(FILE* geo, FILE* svg,HashFile hashFile){
    if(geo == NULL|| svg == NULL ){
        printf("Erro em lerGeo\n");
        return;
    }

    char linha[50];
    char comando[5];

    while(fgets(linha, sizeof(linha), geo) != NULL){
        if (linha[0] == '\n' || linha[0] == '#') continue;
        int lidos_cmd = sscanf(linha, "%9s", comando);
        if (lidos_cmd != 1) continue;
        
        char sw[50];
        char cfill[50];
        char cstrk[50];
        if(strcmp(comando, 'q') == 0){
            //criar uma quadra
            Quadra novaQuadra = criarQuadra();
            if(novaQuadra == NULL){
                printf("erro ao criar quadro em lerGeo\n");
            }
            int CEP, x,y,w,h;
            int lidos = sscanf(&linha[strlen(comando)]," %d %d %d %d %d",&CEP,&x,&y,&w,&h);
            setCEPQuadra(novaQuadra,CEP);
            setDimensoesQuadra(novaQuadra, x,y,w,h);
            setPreenchimentoQuadra(novaQuadra, sw, cfill,cstrk);

            //inserir quadra no hashfile de quadras
            long endereco = buscarDadosHashFile(hashFile, CEP);
            inserirDadoHashFile(hashFile, CEP, endereco);
        }else if(strcmp(comando,'cq') == 0){
            //vai receber as cores de preenchimento, da borda e espessura das bordas das quadras
            int lidos = sscanf(&linha[strlen(comando)], " %s %s %s", sw, cfill, cstrk);        
            
            //quando for criar as quadras, elas serão criadas com essas cores
        }
    }
}