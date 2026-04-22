#include "geo.h"
#include "hashFile.h"
#include "qry.h"
#include "svg.h"
#include "arquivoPm.h"

#define PATH_LEN 512
#define FILE_NAME_LEN 256
#define MAX_FULL_PATH 4096

int main(int argc, char* argv[]) {
    char dirEntrada[PATH_LEN] = "";
    char dirSaida[PATH_LEN] = "";
    char nomeArquivoGeo[FILE_NAME_LEN] = "";
    char nomeArquivoQry[FILE_NAME_LEN] = "";
    char nomeArquivoPm[FILE_NAME_LEN] = "";
    int hasGeo = 0, hasQry = 0, hasSaida = 0, hasPm = 0;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-e") == 0 && i + 1 < argc) {
            strcpy(dirEntrada, argv[++i]);
        } else if (strcmp(argv[i], "-o") == 0 && i + 1 < argc) {
            strcpy(dirSaida, argv[++i]);
            hasSaida = 1;
        } else if (strcmp(argv[i], "-f") == 0 && i + 1 < argc) {
            strcpy(nomeArquivoGeo, argv[++i]);
            hasGeo = 1;
        } else if (strcmp(argv[i], "-q") == 0 && i + 1 < argc) {
            strcpy(nomeArquivoQry, argv[++i]);
            hasQry = 1;
        } else if (strcmp(argv[i], "-pm") == 0 && i + 1 < argc) {
            strcpy(nomeArquivoPm, argv[++i]);  
            hasPm = 1;
        } else {
            fprintf(stderr, "Parametro desconhecido ou invalido: %s\n", argv[i]);
            return EXIT_FAILURE;
        }
    }

    if (!hasGeo || !hasSaida) {
        fprintf(stderr, "Erro: parametros obrigatorios -f (geo) e -o (saida) nao fornecidos.\n");
        return EXIT_FAILURE;
    }

    char fullPathGeo[MAX_FULL_PATH];
    snprintf(fullPathGeo, sizeof(fullPathGeo), "%s/%s", dirEntrada, nomeArquivoGeo);

    char fullPathQry[MAX_FULL_PATH];
    if (hasQry) {
        snprintf(fullPathQry, sizeof(fullPathQry), "%s/%s", dirEntrada, nomeArquivoQry);
    }

    char baseGeo[FILE_NAME_LEN];
    char *p = strrchr(nomeArquivoGeo, '/');
    strcpy(baseGeo, p ? p + 1 : nomeArquivoGeo);

    char *ext = strrchr(baseGeo, '.');
    if(ext) *ext = '\0';    

    char baseQry[FILE_NAME_LEN] = "";
    if (hasQry) {
        char *p = strrchr(nomeArquivoQry, '/');
        strcpy(baseQry, p ? p + 1 : nomeArquivoQry);    
        char *ext = strrchr(baseQry, '.');
        if(ext) *ext = '\0';
    }

    char arquivoSvgGeo[MAX_FULL_PATH];
    char arquivoSvgFinal[MAX_FULL_PATH];
    char arquivoSaidaTxt[MAX_FULL_PATH];

    snprintf(arquivoSvgGeo, sizeof(arquivoSvgGeo), "%s/%s.svg", dirSaida, baseGeo);

    if (hasQry) {
        snprintf(arquivoSvgFinal, sizeof(arquivoSvgFinal), "%s/%s-%s.svg", dirSaida, baseGeo, baseQry);
        snprintf(arquivoSaidaTxt, sizeof(arquivoSaidaTxt), "%s/%s-%s.txt", dirSaida, baseGeo, baseQry);
    }
    char pessoasHfc[MAX_FULL_PATH], pessoasHf[MAX_FULL_PATH], pessoasHfd[MAX_FULL_PATH];
    char quadrasHfc[MAX_FULL_PATH], quadrasHf[MAX_FULL_PATH], quadrasHfd[MAX_FULL_PATH];

    snprintf(pessoasHfc, sizeof(pessoasHfc), "%s/%s-pessoas.hfc", dirSaida, baseGeo);
    snprintf(pessoasHf,  sizeof(pessoasHf),  "%s/%s-pessoas.hf",  dirSaida, baseGeo);
    snprintf(pessoasHfd, sizeof(pessoasHfd), "%s/%s-pessoas.hfd", dirSaida, baseGeo);

    snprintf(quadrasHfc, sizeof(quadrasHfc), "%s/%s-quadras.hfc", dirSaida, baseGeo);
    snprintf(quadrasHf,  sizeof(quadrasHf),  "%s/%s-quadras.hf",  dirSaida, baseGeo);
    snprintf(quadrasHfd, sizeof(quadrasHfd), "%s/%s-quadras.hfd", dirSaida, baseGeo);

    //criar as duas HashFile dinâmicas no disco
    HashFile hashPessoas = criarHashFile(pessoasHfc, pessoasHf, pessoasHfd);
    if(hashPessoas == NULL){
        printf("Erro na main ao criar hashPessoas\n");
        return EXIT_FAILURE;
    }

    HashFile hashQuadras = criarHashFile(quadrasHfc, quadrasHf, quadrasHfd);
    if(hashQuadras == NULL){
        printf("Erro na main ao criar hashQuadras\n");
        return EXIT_FAILURE;
    }

    //abrir svg inicial
    FILE* arqSvgInicial = fopen(arquivoSvgGeo, "w");
    if(arqSvgInicial==NULL){
        printf("Erro: Nao foi possivel abrir o arquivo svgInicial\n");
        fecharHashFile(hashPessoas);
        fecharHashFile(hashQuadras);
        return EXIT_FAILURE;
    }
    abrirSvg(arqSvgInicial);

    //abrir geo
    FILE *arqGeo = fopen(fullPathGeo, "r");
    if (arqGeo == NULL) {
        fprintf(stderr, "ERRO: Não foi possível abrir o arquivo .geo: %s\n", fullPathGeo);
        fclose(arqSvgInicial);
        fecharHashFile(hashPessoas);
        fecharHashFile(hashQuadras);
        return EXIT_FAILURE;
    }
    lerGeo(arqGeo,arqSvgInicial,hashQuadras);

    if (hasPm) {
        char fullPathPm[MAX_FULL_PATH];
        snprintf(fullPathPm, sizeof(fullPathPm), "%s/%s", dirEntrada, nomeArquivoPm);
        FILE *arqPm = fopen(fullPathPm, "r");
        if (arqPm == NULL) {
            fprintf(stderr, "ERRO: Não foi possível abrir o arquivo .pm: %s\n", fullPathPm);
            fclose(arqSvgInicial);
            fclose(arqGeo);
            fecharHashFile(hashPessoas);
            fecharHashFile(hashQuadras);
            return EXIT_FAILURE;
        }
        lerPm(arqPm, hashPessoas, NULL);
        fclose(arqPm);
    }

    //fechar o arqGeo e svgInicial
    fecharSVG(arqSvgInicial);
    fclose(arqSvgInicial);
    fclose(arqGeo);

    //abrir o qry, txt e saidaSvg (apenas se -q foi fornecido)
    if(hasQry){
        FILE* qry = fopen(fullPathQry, "r");
        if(qry == NULL){
            printf("Erro ao abrir o arquivo qry na main\n");
            fecharHashFile(hashPessoas);
            fecharHashFile(hashQuadras);
            return EXIT_FAILURE;
        }

        FILE* txt = fopen(arquivoSaidaTxt, "w");
        if(txt == NULL){
            printf("Erro na main ao abrir o arquivo txt\n");
            fclose(qry);
            fecharHashFile(hashPessoas);
            fecharHashFile(hashQuadras);
            return EXIT_FAILURE;
        }

        FILE* saidaSvg = fopen(arquivoSvgFinal, "w");
        if(saidaSvg == NULL){
            printf("Erro na main ao abrir o saidaSvg\n");
            fclose(qry);
            fclose(txt);
            fecharHashFile(hashPessoas);
            fecharHashFile(hashQuadras);
            return EXIT_FAILURE;
        }

        // Copia o SVG geo (com as quadras) para o SVG final, sem o </svg>
        FILE* geoSvgCopia = fopen(arquivoSvgGeo, "r");
        if (geoSvgCopia != NULL) {
            char linhaCopiada[1024];
            while (fgets(linhaCopiada, sizeof(linhaCopiada), geoSvgCopia) != NULL) {
                if (strncmp(linhaCopiada, "</svg>", 6) == 0) continue;
                fputs(linhaCopiada, saidaSvg);
            }
            fclose(geoSvgCopia);
        } else {
            abrirSvg(saidaSvg);
        }

        lerQry(txt, qry, saidaSvg, hashQuadras, hashPessoas);
        fecharSVG(saidaSvg);

        fclose(saidaSvg);
        fclose(txt);
        fclose(qry);
    }

    fecharHashFile(hashQuadras);
    fecharHashFile(hashPessoas);
    return EXIT_SUCCESS;
}