#include "pessoa.h"
#include "hashFile.h"
#include <stdio.h>
#include <stdlib.h>

/* Helpers ─────────────────────────────────────── */
static void imprimirPessoa(Pessoa p) {
    printf("  CPF       : %d\n",   getCPF(p));
    printf("  Nome      : %s %s\n", getNome(p), getSobrenome(p));
    printf("  Sexo      : %c\n",   getSexo(p));
    printf("  Nascimento: %02d/%02d/%d\n",
           getDiaNascimento(p), getMesNascimento(p), getAnoNascimento(p));
    printf("  Endereco  : CEP %d, face %c, n.%d, %s\n\n",
           getCEP(p), getFace(p), getNum(p), getComplemento(p));
}

static void inserir(HashFile hash, int cpf, const char* nome, const char* sob,
                    char sexo, int dia, int mes, int ano,
                    int cep, char face, int num, const char* comp) {
    Pessoa p = criarPessoa();
    setCPF(p, cpf);
    setNome(p, (char*)nome);
    setSobrenome(p, (char*)sob);
    setSexo(p, sexo);
    setNascimento(p, dia, mes, ano);
    setCEP(p, cep);
    setFace(p, face);
    setNum(p, num);
    setComplemento(p, (char*)comp);

    char* s = serializarPessoa(p);
    inserirDadoHashFile(hash, cpf, s);
    free(s);
    liberarPessoa(p);
}

static void buscarEImprimir(HashFile hash, int cpf) {
    char buf[HASHFILE_TAM_BUF];
    printf("Buscando CPF %d... ", cpf);
    if (buscarDadosHashFile(hash, cpf, buf, HASHFILE_TAM_BUF)) {
        printf("ENCONTRADO\n");
        Pessoa p = desserializarPessoa(buf);
        imprimirPessoa(p);
        liberarPessoa(p);
    } else {
        printf("NAO ENCONTRADO\n\n");
    }
}

/* ── main ────────────────────────────────────── */
int main() {
    /* Remove arquivos antigos para iniciar limpo */
    remove(HASHFILE_PESSOAS_DIR);
    remove(HASHFILE_PESSOAS_DADOS);

    HashFile hash = criarHashFile(HASHFILE_PESSOAS_DIR, HASHFILE_PESSOAS_DADOS);
    if (!hash) {
        printf("Erro ao criar HashFile\n");
        return 1;
    }
    printf("HashFile criada. Profundidade inicial: %d\n\n", getProfundidadeHash(hash));

    /* ── Inserções ── */
    printf("=== INSERCOES ===\n");
    inserir(hash, 1234567, "Vitor", "Morimitsu", 'm', 12, 3, 2004, 10001, 'N', 42, "Apto 5");
    inserir(hash, 9999001, "Ana",   "Silva",     'f',  5, 7, 1998, 20002, 'S', 10, "Casa");
    inserir(hash, 7654321, "Carlos","Oliveira",  'm', 30,11, 1985, 30003, 'L', 88, "Bloco B");
    inserir(hash, 1111111, "Julia", "Santos",    'f',  1, 1, 2000, 40004, 'O', 15, "Ap 101");
    printf("4 pessoas inseridas com sucesso!\n\n");

    /* ── Buscas ── */
    printf("=== BUSCAS ===\n");
    buscarEImprimir(hash, 1234567);
    buscarEImprimir(hash, 9999001);
    buscarEImprimir(hash, 7654321);
    buscarEImprimir(hash, 1111111);
    buscarEImprimir(hash, 9999999);  /* nao existe */

    /* ── Remoção ── */
    printf("=== REMOCAO ===\n");
    printf("Removendo CPF 9999001...\n");
    removerDadosHashFile(hash, 9999001);
    buscarEImprimir(hash, 9999001);  /* deve retornar NAO ENCONTRADO */

    /* ── Profundidade final ── */
    printf("Profundidade final do diretorio: %d\n", getProfundidadeHash(hash));

    fecharHashFile(hash);
    printf("\nArquivos gerados: pessoas.dir (binario), pessoas.txt (texto)\n");
    return 0;
}