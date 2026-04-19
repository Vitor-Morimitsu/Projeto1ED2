#include "unity.h"
#include "qry.h"
#include "hashFile.h"
#include "pessoa.h"
#include "quadra.h"
#include <stdio.h>
#include <string.h>

#define P_DIR   HASHFILE_PESSOAS_DIR
#define P_DADOS HASHFILE_PESSOAS_DADOS
#define Q_DIR   HASHFILE_QUADRAS_DIR
#define Q_DADOS HASHFILE_QUADRAS_DADOS
#define TXT_OUT "test_qry_out.txt"
#define SVG_OUT "test_qry_out.svg"

static HashFile hashP;
static HashFile hashQ;
static FILE*    arqTxt;
static FILE*    arqSvg;

void setUp(void) {
    remove(P_DIR); remove(P_DADOS);
    remove(Q_DIR); remove(Q_DADOS);
    remove(TXT_OUT); remove(SVG_OUT);

    hashP  = criarHashFile(P_DIR, P_DADOS);
    hashQ  = criarHashFile(Q_DIR, Q_DADOS);
    arqTxt = fopen(TXT_OUT, "w");
    arqSvg = fopen(SVG_OUT, "w");
}

void tearDown(void) {
    if (arqTxt) { fclose(arqTxt); arqTxt = NULL; }
    if (arqSvg) { fclose(arqSvg); arqSvg = NULL; }
    if (hashP)  { fecharHashFile(hashP); hashP = NULL; }
    if (hashQ)  { fecharHashFile(hashQ); hashQ = NULL; }
    remove(P_DIR); remove(P_DADOS);
    remove(Q_DIR); remove(Q_DADOS);
    remove(TXT_OUT); remove(SVG_OUT);
}

/* ── Helpers ──────────────────────────────────────────────── */

static void inserirPessoa(char* cpf, char* nome, char* sob, char sexo,
                           int d, int m, int a, char* cep, char face,
                           int num, char* comp) {
    Pessoa p = criarPessoa();
    setCPF(p, cpf); setNome(p, nome); setSobrenome(p, sob);
    setSexo(p, sexo); setNascimento(p, d, m, a);
    setCEP(p, cep); setFace(p, face); setNum(p, num); setComplemento(p, comp);
    char* s = serializarPessoa(p);
    inserirDadoHashFile(hashP, cpf, s);
    free(s); liberarPessoa(p);
}

static void inserirQuadra(char* cep, float x, float y, float w, float h) {
    Quadra q = criarQuadra();
    setCEPQuadra(q, cep);
    setDimensoesQuadra(q, x, y, w, h);
    setPreenchimentoQuadra(q, "1px", "white", "black");
    char* s = serializarQuadra(q);
    inserirDadoHashFile(hashQ, cep, s);
    free(s); liberarQuadra(q);
}

/* ── Testes de comandoNasc ────────────────────────────────── */

void test_comandoNasc_pessoaInseridaNaHash(void) {
    comandoNasc(hashP, "111.111.111-11", "Carlos", "Lima", 'm', 1, 1, 2000);

    char buf[HASHFILE_TAM_BUF];
    int ok = buscarDadosHashFile(hashP, "111.111.111-11", buf, sizeof(buf));
    TEST_ASSERT_TRUE(ok);
    TEST_ASSERT_NOT_NULL(strstr(buf, "Carlos"));
}

/* ── Testes de comandoH ───────────────────────────────────── */

void test_comandoH_pessoaEncontrada_escreveNoTxt(void) {
    inserirPessoa("222.222.222-22","Maria","Souza",'f',10,5,1995,"",'\0',0,"");

    fclose(arqTxt);
    arqTxt = fopen(TXT_OUT, "w");
    comandoH(arqTxt, hashP, "222.222.222-22");
    fclose(arqTxt); arqTxt = NULL;

    FILE* f = fopen(TXT_OUT, "r");
    char linha[512]; int achou = 0;
    while (fgets(linha, sizeof(linha), f))
        if (strstr(linha, "Maria")) { achou = 1; break; }
    fclose(f);
    TEST_ASSERT_TRUE(achou);
    arqTxt = fopen(TXT_OUT, "a");
}

void test_comandoH_pessoaInexistente_escreveNaoEncontrado(void) {
    fclose(arqTxt);
    arqTxt = fopen(TXT_OUT, "w");
    comandoH(arqTxt, hashP, "000.000.000-00");
    fclose(arqTxt); arqTxt = NULL;

    FILE* f = fopen(TXT_OUT, "r");
    char linha[512]; int achou = 0;
    while (fgets(linha, sizeof(linha), f))
        if (strstr(linha, "nao encontrado") || strstr(linha, "não encontrado"))
            { achou = 1; break; }
    fclose(f);
    TEST_ASSERT_TRUE(achou);
    arqTxt = fopen(TXT_OUT, "a");
}

/* ── Testes de comandoMud ─────────────────────────────────── */

void test_comandoMud_atualizaEnderecoPessoa(void) {
    inserirPessoa("333.333.333-33","Pedro","Alves",'m',20,8,1990,"",'\0',0,"");
    inserirQuadra("55555", 0,0,100,100);

    comandoMud(arqSvg, hashQ, hashP, "333.333.333-33", "55555", 'N', 7, "Apto1");

    char buf[HASHFILE_TAM_BUF];
    buscarDadosHashFile(hashP, "333.333.333-33", buf, sizeof(buf));
    TEST_ASSERT_NOT_NULL(strstr(buf, "55555"));
}

/* ── Testes de comandoRip ─────────────────────────────────── */

void test_comandoRip_removesPessoaDaHash(void) {
    inserirPessoa("444.444.444-44","Julia","Costa",'f',3,3,2003,"",'\0',0,"");

    comandoRip(arqTxt, arqSvg, hashP, hashQ, "444.444.444-44");

    char buf[HASHFILE_TAM_BUF];
    int ok = buscarDadosHashFile(hashP, "444.444.444-44", buf, sizeof(buf));
    TEST_ASSERT_FALSE(ok);
}

/* ── Testes de comandoDspj ───────────────────────────────── */

void test_comandoDspj_zeraCEPDaPessoa(void) {
    inserirPessoa("555.555.555-55","Lucas","Martins",'m',15,6,1988,"88888",'S',5,"Casa");
    inserirQuadra("88888", 50,50,80,60);

    comandoDspj(arqTxt, arqSvg, hashQ, hashP, "555.555.555-55");

    char buf[HASHFILE_TAM_BUF];
    buscarDadosHashFile(hashP, "555.555.555-55", buf, sizeof(buf));
    Pessoa p = desserializarPessoa(buf);
    TEST_ASSERT_EQUAL_STRING("", getCEP(p));
    liberarPessoa(p);
}

/* ── Testes de comandoRq ─────────────────────────────────── */

void test_comandoRq_removeQuadraDaHash(void) {
    inserirQuadra("99999", 0, 0, 100, 100);

    comandoRq(arqTxt, arqSvg, hashQ, hashP, "99999");

    char buf[HASHFILE_TAM_BUF];
    int ok = buscarDadosHashFile(hashQ, "99999", buf, sizeof(buf));
    TEST_ASSERT_FALSE(ok);
}

void test_comandoRq_moradorViraSemTeto(void) {
    inserirQuadra("77777", 0, 0, 100, 100);
    inserirPessoa("666.666.666-66","Bia","Ramos",'f',1,1,2001,"77777",'L',3,"Bloco");

    comandoRq(arqTxt, arqSvg, hashQ, hashP, "77777");

    char buf[HASHFILE_TAM_BUF];
    int ok = buscarDadosHashFile(hashP, "666.666.666-66", buf, sizeof(buf));
    TEST_ASSERT_TRUE(ok);
    Pessoa p = desserializarPessoa(buf);
    TEST_ASSERT_EQUAL_STRING("", getCEP(p));
    liberarPessoa(p);
}

/* ── main ───────────────────────────────────────────────── */

int main(void) {
    UNITY_BEGIN();

    RUN_TEST(test_comandoNasc_pessoaInseridaNaHash);

    RUN_TEST(test_comandoH_pessoaEncontrada_escreveNoTxt);
    RUN_TEST(test_comandoH_pessoaInexistente_escreveNaoEncontrado);

    RUN_TEST(test_comandoMud_atualizaEnderecoPessoa);

    RUN_TEST(test_comandoRip_removesPessoaDaHash);

    RUN_TEST(test_comandoDspj_zeraCEPDaPessoa);

    RUN_TEST(test_comandoRq_removeQuadraDaHash);
    RUN_TEST(test_comandoRq_moradorViraSemTeto);

    return UNITY_END();
}
