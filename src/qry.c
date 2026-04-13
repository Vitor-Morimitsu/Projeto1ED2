#include "qry.h"

void comandoRq(FILE* txt, FILE* svg, HashFile hashQuadras, HashFile hashPessoas, char* cep) {
    if (txt == NULL || svg == NULL || hashQuadras == NULL || hashPessoas == NULL) {
        printf("Erro em comandoRq\n");
        return;
    }
    printf("remoção de quadra iniciada\n");
    FILE* arquivoDados = fopen(getArquivoDadosHashFile(hashPessoas), "r+");
    if(arquivoDados == NULL){
        pritnf("Erro em comandoRq ao abrir o arquivo de dados da hash\n");
        return;
    }
    char linha[512];
    while(fgets(linha, sizeof(linha), arquivoDados)!= NULL){
        //pula lixo
        if(strncmp(linha, "s=",2) != 0)continue;
        int status;
        char pacoteDados[256];
        if(sscanf(linha,"s=%d|k=%*[^|]|d=%256[^\n]", &status,pacoteDados) == 2){
            if(status == 1){
                Pessoa p = desserializarPessoa(pacoteDados);
                if(p) {
                    if(strcmp(getCEP(p), cep) == 0) {
                        fprintf(txt, "O(a) %s %s (CPF: %s) acabou de ficar sem teto!\n", getNome(p), getSobrenome(p), getCPF(p));                        
                        // Atualizando o registro dele para virar sem-teto
                        setCEP(p, "");
                        char* moradorAtualizado = serializarPessoa(p);
                        inserirDadoHashFile(hashPessoas, getCPF(p), moradorAtualizado);
                        free(moradorAtualizado);
                    }
                    liberarPessoa(p);
                }
            }
        } 

    }

    removerDadosHashFile(hashQuadras, cep);
}

//calcula o numero de moradores por quadra
void comandoPq(FILE* svg, HashFile hashQuadras, HashFile hashPessoas, char* cep) {
    if(!svg || !hashQuadras || !hashPessoas) return;
    printf("Inicio do comandoPq\n");
    FILE* moradores = fopen(getArquivoDadosHashFile(hashPessoas), "r+");
    if(moradores == NULL){
        printf("Erro em comandoPq ao abrir o arquivo\n");
        return;
    }

    int status;
    char pacoteDados[512];
    char linha[512];
    int contadorTotal = 0;
    int contadorNorte = 0;
    int contadorLeste = 0;
    int contadorOeste=0;
    int contadorSul=0;

    while(fgets(linha, sizeof(linha), moradores) != NULL){
        if(strncmp(linha, "s=",2) != 0)continue;
        if(sscanf(linha, "s=%d|k=%*[^|]|d=%256[^\n]", &status,pacoteDados)){
            if(status ==1){
                Pessoa p = desserializarPessoa(pacoteDados);
                if(p){
                    if(strcmp(getCEP(p), cep) == 0){
                        char face = getFace(p);
                        if(face == 'n' || face == 'N'){
                            contadorNorte++;
                        }else if(face == 's' || face == 'S'){
                            contadorSul++;
                        }else if(face == 'l' || face == 'L'){
                            contadorLeste++;
                        }else if(face == 'o' || face == 'O'){
                            contadorOeste++;
                        }
                        contadorTotal++;
                    }
                }
                liberarPessoa(p);
            }
        }
    }
    fclose(moradores);
    char quadraBuf[512];
    if(buscarDadosHashFile(hashQuadras, cep, quadraBuf, 512)){
        Quadra q = desserializarQuadra(quadraBuf);
        
        float qx = getXQuadra(q);
        float qy = getYQuadra(q);
        float qw = getWQuadra(q);
        float qh = getHQuadra(q);
        // -- TOTALIZADOR: Bem no centro da quadra (x + metade da largura)
        fprintf(svg, "<text x=\"%f\" y=\"%f\" fill=\"black\" font-size=\"14\">Total: %d</text>\n", 
                qx + (qw/2), qy + (qh/2), contadorTotal);
        
        // -- NORTE: Em cima da quadra (Eixo Y menor)
        fprintf(svg, "<text x=\"%f\" y=\"%f\" fill=\"blue\" font-size=\"10\">%d</text>\n", 
                qx + (qw/2), qy - 10, contadorNorte);
                
        // -- SUL: Abaixo da quadra (Eixo Y desce com a Altura inteira)
        fprintf(svg, "<text x=\"%f\" y=\"%f\" fill=\"blue\" font-size=\"10\">%d</text>\n", 
                qx + (qw/2), qy + qh + 10, contadorSul);
                
        // -- LESTE: À direita (Eixo X cresce)
        fprintf(svg, "<text x=\"%f\" y=\"%f\" fill=\"blue\" font-size=\"10\">%d</text>\n", 
                qx + qw + 5, qy + (qh/2), contadorLeste);
                
        // -- OESTE: À esquerda (Eixo X menor)
        fprintf(svg, "<text x=\"%f\" y=\"%f\" fill=\"blue\" font-size=\"10\">%d</text>\n", 
                qx - 15, qy + (qh/2), contadorOeste);

        liberarQuadra(q);
    }
}

void comandoCenso(FILE* txt, HashFile hashPessoas) {
    // TODO: implementar
}

void comandoH(FILE* txt, HashFile hashPessoas) {
    // TODO: implementar
}

void comandoNasc(HashFile hashPessoas, char* cpf, char* nome, char* sobrenome,char sexo, int diaNasc, int mesNasc, int anoNasc) {
    if (hashPessoas == NULL) {
        printf("Erro em comandoNasc\n");
        return;
    }

    /* Cria e preenche a nova Pessoa */
    Pessoa nova = criarPessoa();
    if (nova == NULL) {
        printf("Erro ao criar pessoa em comandoNasc\n");
        return;
    }
    setCPF(nova, cpf);
    setNome(nova, nome);
    setSobrenome(nova, sobrenome);
    setSexo(nova, sexo);
    setNascimento(nova, diaNasc, mesNasc, anoNasc);

    /* Serializa o registro completo e insere na HashFile */
    char* dado = serializarPessoa(nova);
    inserirDadoHashFile(hashPessoas, cpf, dado);
    free(dado);

    liberarPessoa(nova);
}

void comandoRip(FILE* txt, HashFile hashPessoas, char* cpf) {
    if (hashPessoas == NULL) {
        printf("Erro em comandoRip\n");
        return;
    }

    /* Busca o registro ANTES de remover para poder imprimir os dados */
    char buf[HASHFILE_TAM_BUF];
    if (!buscarDadosHashFile(hashPessoas, cpf, buf, HASHFILE_TAM_BUF)) {
        printf("CPF %s nao encontrado em comandoRip\n", cpf);
        return;
    }

    Pessoa falecido = desserializarPessoa(buf);

    fprintf(txt, "cpf do falecido: %s, nome do falecido: %s, sobrenome do falecido: %s, sexo do falecido: %c, data de nascimento: %d/%d/%d\n",
            getCPF(falecido),
            getNome(falecido),
            getSobrenome(falecido),
            getSexo(falecido),
            getDiaNascimento(falecido),
            getMesNascimento(falecido),
            getAnoNascimento(falecido));

    liberarPessoa(falecido);

    /* Remove logicamente da HashFile */
    removerDadosHashFile(hashPessoas, cpf);
}

void comandoMud(FILE* svg, HashFile hashQuadras, HashFile hashPessoas, char* cpf, char* cep, char face, int num, char* complemento) {
    // TODO: implementar
}

void comandoDspj(FILE* txt, FILE* svg, HashFile hashQuadras, HashFile hashPessoas, char* cpf) {
    // TODO: implementar
}

void lerQry(FILE* txt, FILE* qry, FILE* svg, HashFile hashQuadras, HashFile hashPessoas) {
    if(txt == NULL || qry == NULL){
        printf("Erro em lerQry\n");
        return;
    }

    char linha[256];
    char comando[256];

    while(fgets(linha, sizeof(linha), qry) != NULL){
        if(linha[0] == '\n' || linha[0] == '\r' ){
            continue;
        }
        if(sscanf(linha, "%s", comando) != 1){
            continue;
        }       

        if(strcmp(comando, "rq") == 0){
            char cep[32];
            if(sscanf(linha, "rq %31s", cep) == 1 ){
                comandoRq(txt,svg,hashQuadras,hashPessoas, cep);
            }
        }else if(strcmp(comando, "pq") == 0){
            char cep[32];
            if(sscanf(linha,"pq %31s", cep) == 1){
                comandoPq(svg,hashQuadras,hashPessoas,cep);
            }
        }else if(strcmp(comando, "censo") == 0){
            //produzir estatística dos habitantes --------------------------------------------falta implementar
        }else if(strcmp(comando, "h?") == 0){
            char cpf[32];
            if(sscanf(linha, "h? %31s", cpf) == 1){
                comandoH(txt, hashPessoas);
            }
        }else if(strcmp(comando, "nasc") == 0){
            char cpf[32];
            char nome[64];
            char sobrenome[64];
            char sexo;
            int diaAniver;
            int mesAniver;
            int anoAniver;
            if(sscanf(linha, "nasc %31s %63s %63s %c %d/%d/%d", cpf, nome, sobrenome, &sexo, &diaAniver, &mesAniver, &anoAniver) == 7){
                comandoNasc(hashPessoas,cpf,nome,sobrenome,sexo,diaAniver, mesAniver,anoAniver);
            }
        }else if(strcmp(comando, "rip") == 0){
            char cpf[32];
            if(sscanf(linha,"rip %31s", cpf) == 1){
                comandoRip(txt,hashPessoas, cpf);
            }
        }else if(strcmp(comando, "mud") == 0){
            char cpf[32];
            char cep[32];
            char face;
            int num;
            char complemento[64];
            if(sscanf(linha, "mud %31s %31s %c %d %63s", cpf, cep, &face, &num, complemento) == 5){
                comandoMud(svg, hashQuadras,hashPessoas,cpf,cep,face,num,complemento);
            }
        }else if(strcmp(comando, "dspj") == 0){
            char cpf[32];
            if(sscanf(linha,"dspj %31s", cpf) == 1){
                comandoDspj(txt,svg,hashQuadras,hashPessoas,cpf);
            }
        }else{
            printf("comando qry inexistente\n");
        }
    }
}