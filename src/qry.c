#include "qry.h"

/* ── Contexto para o callback do comandoRq ───────────────── */
typedef struct {
    FILE*     txt;
    HashFile  hashPessoas;
    char*     cep;
} CtxRq;

static void _callbackRq(char* chave, const char* dado, void* extra) {
    CtxRq* ctx = (CtxRq*)extra;
    Pessoa p = desserializarPessoa(dado);
    if (!p) return;
    if (strcmp(getCEP(p), ctx->cep) == 0) {
        fprintf(ctx->txt, "O(a) %s %s (CPF: %s) acabou de ficar sem teto!\n",
                getNome(p), getSobrenome(p), getCPF(p));
        setCEP(p, "");
        char* atualizado = serializarPessoa(p);
        inserirDadoHashFile(ctx->hashPessoas, chave, atualizado);
        free(atualizado);
    }
    liberarPessoa(p);
}

void comandoRq(FILE* txt, FILE* svg, HashFile hashQuadras, HashFile hashPessoas, char* cep) {
    if (txt == NULL || svg == NULL || hashQuadras == NULL || hashPessoas == NULL) {
        printf("Erro em comandoRq\n");
        return;
    }
    printf("remocao de quadra iniciada\n");
    // Usa percorrerHashFile para varrer moradores sem abrir o arquivo diretamente
    CtxRq ctx = { txt, hashPessoas, cep };
    percorrerHashFile(hashPessoas, _callbackRq, &ctx);
    // Busca a quadra na hash antes de remover
    char quadraBuf[512];
    if(buscarDadosHashFile(hashQuadras, cep, quadraBuf, sizeof(quadraBuf))){
        Quadra q = desserializarQuadra(quadraBuf);
        if(q){
            float x = getXQuadra(q);
            float y = getYQuadra(q);
            comandoRqSvg(svg, x, y);
            liberarQuadra(q);
        }
    }
    removerDadosHashFile(hashQuadras, cep);
}

//calcula o numero de moradores por quadra
void comandoPq(FILE* svg, HashFile hashQuadras, HashFile hashPessoas, char* cep) {
    if(!svg || !hashQuadras || !hashPessoas) return;
    printf("Inicio do comandoPq\n");
    FILE* moradores = fopen(HASHFILE_PESSOAS_DADOS, "r");
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

/*reportar estatísticas: número total de habitantes, número total de moradores, proporção moradores/habitantes, número de homens, número de mulheres, % de
habitantes homens, % habitantes mulheres, % de moradores homens, % de moradores mulheres, número total de sem-tetos, % sem-tetos homens, %sem-tetos mulheres
*/
void comandoCenso(FILE* txt, HashFile hashPessoas) {
    if(!txt || !hashPessoas) return;

    printf("Inicio do comandoCenso\n");
    FILE* arquivoDados = fopen(HASHFILE_PESSOAS_DADOS, "r");
    if(arquivoDados == NULL){
        printf("Erro em comandoCenso ao abrir o arquivo\n");
        return;
    }
    int habitantes = 0, moradores = 0, homens = 0, mulheres = 0;
    int semTeto = 0, semTetoHomens = 0, semTetoMulheres = 0;
    char linha[512];
    while(fgets(linha,sizeof(linha), arquivoDados)!=NULL){
        if(strncmp(linha, "s=",2) != 0)continue;

        int statusGavetaLida;
        char pacoteDados[256];

        if(sscanf(linha,"s=%d|k=%*[^|]|d=%256[^\n]", &statusGavetaLida, pacoteDados)){
            if(statusGavetaLida == 1){
                Pessoa p = desserializarPessoa(pacoteDados);
                if(p){
                    habitantes++;
                    if(strcmp(getCEP(p), "") != 0){
                        //possuem moradia
                        moradores++;
                    }
                    if(getSexo(p) == 'm' || getSexo(p) == 'M'){
                        homens++;
                    }else{
                        mulheres++;
                    }
                    if(strcmp(getCEP(p), "") == 0){
                        semTeto++;
                        if(getSexo(p) == 'm' || getSexo(p) == 'M'){
                            semTetoHomens++;
                        }else{
                            semTetoMulheres++;
                        }
                    }
                }
                liberarPessoa(p);
            }
        }
    }
    fclose(arquivoDados);

    if(habitantes > 0){
        float propMoradores = (float)moradores / habitantes;
        float propHomens = (float)homens / habitantes;
        float propMulheres = (float)mulheres / habitantes;
        float propMoradoresHomens = (float)homens / moradores;
        float propMoradoresMulheres = (float)mulheres / moradores;
        float propSemTeto = (float)semTeto / habitantes;
        float propSemTetoHomens = (float)semTetoHomens / semTeto;
        float propSemTetoMulheres = (float)semTetoMulheres / semTeto;

        fprintf(txt, "Numero total de habitantes: %d\n", habitantes);
        fprintf(txt, "Numero total de moradores: %d\n", moradores);
        fprintf(txt, "Proporcao moradores/habitantes: %f\n", propMoradores);
        fprintf(txt, "Numero total de homens: %d\n", homens);
        fprintf(txt, "Numero total de mulheres: %d\n", mulheres);
        fprintf(txt, "Porcentagem habitantes homens: %f\n", propHomens);
        fprintf(txt, "Porcentagem habitantes mulheres: %f\n", propMulheres);
        fprintf(txt, "Porcentagem moradores homens: %f\n", propMoradoresHomens);
        fprintf(txt, "Porcentagem moradores mulheres: %f\n", propMoradoresMulheres);
        fprintf(txt, "Numero total de sem-tetos: %d\n", semTeto);
        fprintf(txt, "Porcentagem sem-tetos homens: %f\n", propSemTetoHomens);
        fprintf(txt, "Porcentagem sem-tetos mulheres: %f\n", propSemTetoMulheres);  
    }else{
        fprintf(txt, "Nenhum habitante encontrado\n");
    }   
}

//reporta os dados sobre um habitante identificado pelo CPF
void comandoH(FILE* txt, HashFile hashPessoas,char* cpf) {
    if(txt == NULL){
        printf("Erro ao abrir o arquivo txt em comandoH\n");
    }

    char pacoteDados[256];
    if(buscarDadosHashFile(hashPessoas, cpf, pacoteDados, 256)){
        Pessoa p = desserializarPessoa(pacoteDados);
        if(p){
            printf("Printado dados do habitante em comandoH\n");
            fprintf(txt, "CPF: %s,Nome: %s ,Sobrenome: %s,Sexo: %c, Nascimento: %d/%d/%d \n", getCPF(p), getNome(p),
            getSobrenome(p),getSexo(p), getDiaNascimento(p),getMesNascimento(p),getAnoNascimento(p));
            if(strcmp(getCEP(p), "") != 0){
                fprintf(txt, "CEP: %s,Numero: %d,Complemento: %s\n", getCEP(p), getNum(p), getComplemento(p));
            }else{
                fprintf(txt, "CEP: nao encontrado\n");
            }   
            liberarPessoa(p);
        }
    } else {
        fprintf(txt, "Habitante do CPF %s nao encontrado\n", cpf);
    }   
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

void comandoRip(FILE* txt,FILE* svg, HashFile hashPessoas,HashFile hashQuadras, char* cpf) {
    if (hashPessoas == NULL) {
        printf("Erro em comandoRip\n");
        return;
    }

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

            if(strcmp(getCEP(falecido),"") != 0){
                fprintf(txt, "O falecido tinha moradia no CEP: %s, numero: %d, face: %c, complemento: %s\n", getCEP(falecido), getNum(falecido), getFace(falecido), getComplemento(falecido));    
                char quadraBuf[512];
                if(buscarDadosHashFile(hashQuadras, getCEP(falecido),quadraBuf,512)){
                    Quadra q = desserializarQuadra(quadraBuf);
                    float x = getXQuadra(q);
                    float y = getYQuadra(q);
                    comandoRipSvg(svg, x, y);
                    liberarQuadra(q);   
                }
            }
            liberarPessoa(falecido);
    
    //remove a pessoa da hash file
    removerDadosHashFile(hashPessoas, cpf);
}

//morador identificado por cpf muda-se para novo endereço
void comandoMud(FILE* svg, HashFile hashQuadras, HashFile hashPessoas, char* cpf, char* cep, char face, int num, char* complemento) {
    if(!svg || !hashQuadras || !hashPessoas){
        printf("Erro ao abrir arquvios em comandoMud\n");
        return;
    }
    char pacoteDados[512];
    if(buscarDadosHashFile(hashPessoas,cpf,pacoteDados,512)){
        Pessoa p = desserializarPessoa(pacoteDados);
        if(p != NULL){
            printf("a pessoa com esse cpf: %s vai se mudar para o endereço passado\n", cpf);
            setCEP(p, cep);
            setFace(p,face);
            setNum(p,num);
            setComplemento(p,complemento);
            char quadraBuf[512];
            if(buscarDadosHashFile(hashQuadras, cep,quadraBuf,512)){
                Quadra q = desserializarQuadra(quadraBuf);
                float x = getXQuadra(q);
                float y = getYQuadra(q);
                comandoMudSvg(svg, x, y, cpf);
                liberarQuadra(q);
            }
            char* textoFinal = serializarPessoa(p);
            inserirDadoHashFile(hashPessoas,cpf,textoFinal);
            free(textoFinal);
            liberarPessoa(p);
        }else{
            printf("Pessoa com o CPF: %s não foi encontrada\n",cpf);
        }
        
    }

}
//morador identificado por cpf é despejado 
void comandoDspj(FILE* txt, FILE* svg, HashFile hashQuadras, HashFile hashPessoas, char* cpf) {
    if(!txt || !svg || !hashQuadras || !hashPessoas){
        printf("Erro em comandoDspj\n");
        return;
    }
    char pacoteDados[512];
    if(buscarDadosHashFile(hashPessoas, cpf, pacoteDados, 512)){
        Pessoa p = desserializarPessoa(pacoteDados);
        if(p != NULL){
            printf("A pessoa com esse cpf: %s será despejada\n", cpf);
            char cepAntigo[32];
            strncpy(cepAntigo, getCEP(p), sizeof(cepAntigo) - 1);
            cepAntigo[sizeof(cepAntigo) - 1] = '\0';
            fprintf(txt, "Antigo endereco: cep = %s, Numero = %d, Complemento = %s\n", cepAntigo, getNum(p), getComplemento(p));

            //zerar o atual endereço do morador
            setCEP(p, "");
            setNum(p, -1);
            setComplemento(p, "");

            char* dadoFinal = serializarPessoa(p);
            inserirDadoHashFile(hashPessoas, getCPF(p), dadoFinal);
            free(dadoFinal);

            // Colocar um círculo preto no local da quadra onde a pessoa morava
            char quadraBuf[512];
            if(buscarDadosHashFile(hashQuadras, cepAntigo, quadraBuf, sizeof(quadraBuf))){
                Quadra q = desserializarQuadra(quadraBuf);
                if(q){
                    float x = getXQuadra(q);
                    float y = getYQuadra(q);
                    comandoDspjSvg(svg, x, y);
                    liberarQuadra(q);
                }
            }
            liberarPessoa(p);
        } else {
            printf("O cpf passado em comandoDspj nao foi encontrado\n");
        }
    } else {
        printf("Erro em comandoDspj: CPF nao encontrado na hash\n");
    }
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
            comandoCenso(txt,hashPessoas);

        }else if(strcmp(comando, "h?") == 0){
            char cpf[32];
            if(sscanf(linha, "h? %31s", cpf) == 1){
                comandoH(txt, hashPessoas,cpf);
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
                comandoRip(txt,svg,hashPessoas,hashQuadras, cpf);
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