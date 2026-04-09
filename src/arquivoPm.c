#include "arquivoPm.h"

void comandoP(HashFile hashFile, FILE* pessoasArq,
              int CPF, char* nome, char* sobrenome, char sexo,
              int diaNascimento, int mesNascimento, int anoNascimento) {

    Pessoa novoHabitante = criarPessoa();
    if (novoHabitante == NULL) return;

    setCPF(novoHabitante, CPF);
    setNome(novoHabitante, nome);
    setSobrenome(novoHabitante, sobrenome);
    setSexo(novoHabitante, sexo);
    setNascimento(novoHabitante, diaNascimento, mesNascimento, anoNascimento);

    fseek(pessoasArq, 0, SEEK_END);
    long offset = ftell(pessoasArq);
    escreverPessoaArquivo(novoHabitante, pessoasArq);

    inserirDadoHashFile(hashFile, CPF, offset);

    liberarPessoa(novoHabitante);
}

void comandoM(HashFile hashFile, FILE* pessoasArq,int CPF, int CEP, char face, int num, char* complemento) {
    //procura onde a pessoa está
    long offset = buscarDadosHashFile(hashFile, CPF);
    if (offset == -1) {
        printf("CPF %d não encontrado na HashFile em comandoM\n", CPF);
        return;
    }

    Pessoa habitante = lerPessoaArquivo(pessoasArq, offset);
    if (habitante == NULL) return;

    setCEP(habitante, CEP);
    setFace(habitante, face);
    setNum(habitante, num);
    setComplemento(habitante, complemento);

    fseek(pessoasArq, offset, SEEK_SET);
    escreverPessoaArquivo(habitante, pessoasArq);

    liberarPessoa(habitante);
}

void lerPm(FILE* arquivoPm, HashFile hashFile, FILE* pessoasArq) {
    if (arquivoPm == NULL) {
        printf("Erro: arquivo .pm é NULL em lerPm\n");
        return;
    }

    char linha[200];
    char comando[10];

    while (fgets(linha, sizeof(linha), arquivoPm) != NULL) {
        /* Ignora linhas em branco e comentários */
        if (linha[0] == '\n' || linha[0] == '#') continue;

        int lidos_cmd = sscanf(linha, "%9s", comando);
        if (lidos_cmd != 1) continue;

        if (strcmp(comando, "p") == 0) {
            int  CPF;
            char sexo;
            int  diaNascimento, mesNascimento, anoNascimento;
            char nome[100]      = "";
            char sobrenome[100] = "";

            /* Lê a partir do caractere após o comando */
            int lidos = sscanf(&linha[strlen(comando)]," %d %99s %99s %c %d/%d/%d",&CPF, nome, sobrenome, &sexo,&diaNascimento, &mesNascimento, &anoNascimento);
            if (lidos != 7) {
                printf("Erro ao ler parâmetros do comando 'p'\n");
                continue;
            }
            comandoP(hashFile, pessoasArq, CPF, nome, sobrenome, sexo,
                     diaNascimento, mesNascimento, anoNascimento);

        } else if (strcmp(comando, "m") == 0) {
            int  CPF, CEP, num;
            char face;
            char complemento[50] = "";

            int lidos = sscanf(&linha[strlen(comando)],
                               " %d %d %c %d %49s",
                               &CPF, &CEP, &face, &num, complemento);
            if (lidos != 5) {
                printf("Erro ao ler parâmetros do comando 'm'\n");
                continue;
            }
            comandoM(hashFile, pessoasArq, CPF, CEP, face, num, complemento);
        }
    }
    printf("Fim do arquivoPm\n");
}
