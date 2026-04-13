#include "qry.h"

void comandoRq(FILE* txt, FILE* svg, HashFile hashQuadras, HashFile hashPessoas, char* cep) {
    if (txt == NULL || svg == NULL || hashQuadras == NULL || hashPessoas == NULL) {
        printf("Erro em comandoRq\n");
        return;
    }
    //ver as pessoas que moram no cep
    int endereco =buscarDadosHashFile(hashPessoas, cep, HASHFILE_TAM_BUF, HASHFILE_TAM_BUF);
    if(endereco == 0){
        //não achou a pessoa
        return;
    }

    //comparar com os endereços das pessoas para ver quem mora lá
    while(endereco != NULL){
        
    }

    //remover a quadra
    removerDadosHashFile(hashQuadras, cep);
}

void comandoPq(FILE* svg, HashFile hashQuadras, HashFile hashPessoas, char* cep) {
    // TODO: implementar
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
        printf("CPF %d nao encontrado em comandoRip\n", cpf);
        return;
    }

    Pessoa falecido = desserializarPessoa(buf);

    fprintf(txt, "cpf do falecido: %d, nome do falecido: %s, sobrenome do falecido: %s, " "sexo do falecido: %c, data de nascimento: %d/%d/%d\n",
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

void comandoMud(FILE* svg, HashFile hashQuadras, HashFile hashPessoas,char* cpf, char* cep, char face, int num, char* complemento) {
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
        if(linha[0] == '\n' ||linha[0] == '\r' ){
            continue;
        }
        if(sscanf(linha, "%s", comando) != 1){
            continue;
        }       

        if(strcmp(comando, "rq") == 0){
            char* cep;
            if(sscanf(linha, "rq %s", cep) == 1 ){
                comandoRq(txt,svg,hashQuadras,hashPessoas, cep);
            }
        }else if(strcmp(comando, "pq") == 0){
            char* cep;
            if(sscanf(linha,"pq %s", cep) == 1){
                comandoPq(svg,hashQuadras,hashPessoas,cep);
            }
        }else if(strcmp(comando, "censo") == 0){
            //produzir estatística dos habitantes --------------------------------------------falta implementar


        }else if(strcmp(comando, "h?") == 0){
            char* cpf;
            if(sscanf(linha, "h? %s", cpf) == 1){
                comandoH(txt, hashPessoas);
            }

        }else if(strcmp(comando, "nasc") == 0){
            char* cpf;
            char* nome;
            char* sobrenome;
            char sexo;
            int diaAniver;
            int mesAniver;
            int anoAniver;
            if(sscanf(linha,"nasc %s %s %s %c %d/%d/%d"), cpf,nome,sobrenome,&sexo, &diaAniver,&mesAniver,&anoAniver == 7){
                comandoNasc(hashPessoas,cpf,nome,sobrenome,sexo,diaAniver, mesAniver,anoAniver);
            }
        }else if(strcmp(comando, "rip") == 0){
            char* cpf;
            if(sscanf(linha,"rip %s", cpf) == 1){
                comandoRip(txt,hashPessoas, cpf);
            }
        }else if(strcmp(comando, "mud") == 0){
            char* cpf;
            char* cep;
            char face;
            int num;
            char* complemento;
            if(sscanf(linha, "mud %s %s %c %d %s", cpf,cep,&face,&num, complemento) == 5){
                comandoMud(svg, hashQuadras,hashPessoas,cpf,cep,face,num,complemento);
            }
        }else if(strcmp(comando, "dspj") == 0){
            char* cpf;
            if(sscanf(linha,"dspj %s", cpf) == 1){
                comandoDspj(txt,svg,hashQuadras,hashPessoas,cpf);
            }
        }else{
            printf("comando qry inexistente\n");
        }
    }
