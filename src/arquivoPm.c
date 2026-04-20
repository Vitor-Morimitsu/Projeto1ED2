#include "arquivoPm.h"

void comandoP(HashFile hashFile, FILE* pessoasArq,
              char* CPF, char* nome, char* sobrenome, char sexo,
              int diaNascimento, int mesNascimento, int anoNascimento) {

    Pessoa novoHabitante = criarPessoa();
    if (novoHabitante == NULL) return;

    setCPF(novoHabitante, CPF);
    setNome(novoHabitante, nome);
    setSobrenome(novoHabitante, sobrenome);
    setSexo(novoHabitante, sexo);
    setNascimento(novoHabitante, diaNascimento, mesNascimento, anoNascimento);

    char* dado = serializarPessoa(novoHabitante);
    inserirDadoHashFile(hashFile, CPF, dado);
    free(dado);

    // Mantém escrita no arquivo binário para compatibilidade com lerPessoaArquivo
    if (pessoasArq) {
        fseek(pessoasArq, 0, SEEK_END);
        escreverPessoaArquivo(novoHabitante, pessoasArq);
    }

    liberarPessoa(novoHabitante);
}

void comandoM(HashFile hashFile, FILE* pessoasArq,
              char* CPF, char* CEP, char face, int num, char* complemento) {

    char buf[HASHFILE_TAM_BUF];
    if (!buscarDadosHashFile(hashFile, CPF, buf, HASHFILE_TAM_BUF)) {
        printf("CPF %s nao encontrado na HashFile em comandoM\n", CPF);
        return;
    }

    Pessoa habitante = desserializarPessoa(buf);
    if (habitante == NULL) return;

    setCEP(habitante, CEP);
    setFace(habitante, face);
    setNum(habitante, num);
    setComplemento(habitante, complemento);

    char* dado = serializarPessoa(habitante);
    inserirDadoHashFile(hashFile, CPF, dado);
    free(dado);

    // Atualiza no arquivo binário se disponível
    if (pessoasArq) {
        fseek(pessoasArq, 0, SEEK_END);
        escreverPessoaArquivo(habitante, pessoasArq);
    }

    liberarPessoa(habitante);

    (void)pessoasArq; // suprime warning se não usado
}

void lerPm(FILE* arquivoPm, HashFile hashFile, FILE* pessoasArq) {
    if (arquivoPm == NULL) {
        printf("Erro: arquivo .pm e NULL em lerPm\n");
        return;
    }

    char linha[200];
    char comando[10];

    while (fgets(linha, sizeof(linha), arquivoPm) != NULL) {
        if (linha[0] == '\n' || linha[0] == '#') continue;

        int lidos_cmd = sscanf(linha, "%9s", comando);
        if (lidos_cmd != 1) continue;

        if (strcmp(comando, "p") == 0) {
            char CPF[32];
            char sexo;
            int  diaNascimento, mesNascimento, anoNascimento;
            char nome[100]      = "";
            char sobrenome[100] = "";

            int lidos = sscanf(&linha[strlen(comando)], " %31s %99s %99s %c %d/%d/%d",
                               CPF, nome, sobrenome, &sexo,
                               &diaNascimento, &mesNascimento, &anoNascimento);
            if (lidos != 7) {
                printf("Erro ao ler parametros do comando 'p'\n");
                continue;
            }
            comandoP(hashFile, pessoasArq, CPF, nome, sobrenome, sexo,
                     diaNascimento, mesNascimento, anoNascimento);

        } else if (strcmp(comando, "m") == 0) {
            char CPF[32], CEP[32];
            int num;
            char face;
            char complemento[50] = "";

            int lidos = sscanf(&linha[strlen(comando)],
                               " %31s %31s %c %d %49s",
                               CPF, CEP, &face, &num, complemento);
            if (lidos != 5) {
                printf("Erro ao ler parametros do comando 'm'\n");
                continue;
            }
            comandoM(hashFile, pessoasArq, CPF, CEP, face, num, complemento);
        }
    }
    printf("Fim do arquivoPm\n");
}
