#include "unity.h"
#include "pessoa.h"
#include <string.h>

void setUp(void) {}
void tearDown(void) {}

/* ── criarPessoa ─────────────────────────────────────────── */

void test_criarPessoa_naoRetornaNULL(void) {
    Pessoa p = criarPessoa();
    TEST_ASSERT_NOT_NULL(p);
    liberarPessoa(p);
}

/* ── set / get CPF ───────────────────────────────────────── */

void test_setCPF_getCPF_retornaValorCorreto(void) {
    Pessoa p = criarPessoa();
    setCPF(p, "123.456.789-00");
    TEST_ASSERT_EQUAL_STRING("123.456.789-00", getCPF(p));
    liberarPessoa(p);
}

/* ── set / get nome e sobrenome ──────────────────────────── */

void test_setNome_getNome_retornaValorCorreto(void) {
    Pessoa p = criarPessoa();
    setNome(p, "Vitor");
    TEST_ASSERT_EQUAL_STRING("Vitor", getNome(p));
    liberarPessoa(p);
}

void test_setSobrenome_getSobrenome_retornaValorCorreto(void) {
    Pessoa p = criarPessoa();
    setSobrenome(p, "Morimitsu");
    TEST_ASSERT_EQUAL_STRING("Morimitsu", getSobrenome(p));
    liberarPessoa(p);
}

/* ── set / get sexo ──────────────────────────────────────── */

void test_setSexo_getSexo_masculino(void) {
    Pessoa p = criarPessoa();
    setSexo(p, 'm');
    TEST_ASSERT_EQUAL_CHAR('m', getSexo(p));
    liberarPessoa(p);
}

void test_setSexo_getSexo_feminino(void) {
    Pessoa p = criarPessoa();
    setSexo(p, 'f');
    TEST_ASSERT_EQUAL_CHAR('f', getSexo(p));
    liberarPessoa(p);
}

/* ── set / get nascimento ────────────────────────────────── */

void test_setNascimento_retornaValoresCorretos(void) {
    Pessoa p = criarPessoa();
    setNascimento(p, 12, 3, 2004);
    TEST_ASSERT_EQUAL_INT(12,   getDiaNascimento(p));
    TEST_ASSERT_EQUAL_INT(3,    getMesNascimento(p));
    TEST_ASSERT_EQUAL_INT(2004, getAnoNascimento(p));
    liberarPessoa(p);
}

/* ── set / get endereço ──────────────────────────────────── */

void test_setCEP_getCEP_retornaValorCorreto(void) {
    Pessoa p = criarPessoa();
    setCEP(p, "01310-100");
    TEST_ASSERT_EQUAL_STRING("01310-100", getCEP(p));
    liberarPessoa(p);
}

void test_setFace_getFace_retornaValorCorreto(void) {
    Pessoa p = criarPessoa();
    setFace(p, 'N');
    TEST_ASSERT_EQUAL_CHAR('N', getFace(p));
    liberarPessoa(p);
}

void test_setNum_getNum_retornaValorCorreto(void) {
    Pessoa p = criarPessoa();
    setNum(p, 42);
    TEST_ASSERT_EQUAL_INT(42, getNum(p));
    liberarPessoa(p);
}

void test_setComplemento_getComplemento_retornaValorCorreto(void) {
    Pessoa p = criarPessoa();
    setComplemento(p, "Apto 5");
    TEST_ASSERT_EQUAL_STRING("Apto 5", getComplemento(p));
    liberarPessoa(p);
}

/* ── serialização / desserialização ──────────────────────── */

void test_serializar_naoRetornaNULL(void) {
    Pessoa p = criarPessoa();
    setCPF(p, "111.222.333-44");
    char* s = serializarPessoa(p);
    TEST_ASSERT_NOT_NULL(s);
    free(s);
    liberarPessoa(p);
}

void test_serializar_desserializar_mantemTodosOsDados(void) {
    Pessoa original = criarPessoa();
    setCPF(original,        "123.456.789-00");
    setNome(original,       "Ana");
    setSobrenome(original,  "Silva");
    setSexo(original,       'f');
    setNascimento(original, 5, 7, 1998);
    setCEP(original,        "20002-111");
    setFace(original,       'S');
    setNum(original,        10);
    setComplemento(original,"Casa");

    char* s = serializarPessoa(original);
    Pessoa copia = desserializarPessoa(s);
    TEST_ASSERT_NOT_NULL(copia);

    TEST_ASSERT_EQUAL_STRING("123.456.789-00", getCPF(copia));
    TEST_ASSERT_EQUAL_STRING("Ana",            getNome(copia));
    TEST_ASSERT_EQUAL_STRING("Silva",          getSobrenome(copia));
    TEST_ASSERT_EQUAL_CHAR('f',                getSexo(copia));
    TEST_ASSERT_EQUAL_INT(5,                   getDiaNascimento(copia));
    TEST_ASSERT_EQUAL_INT(7,                   getMesNascimento(copia));
    TEST_ASSERT_EQUAL_INT(1998,                getAnoNascimento(copia));
    TEST_ASSERT_EQUAL_STRING("20002-111",      getCEP(copia));
    TEST_ASSERT_EQUAL_CHAR('S',                getFace(copia));
    TEST_ASSERT_EQUAL_INT(10,                  getNum(copia));
    TEST_ASSERT_EQUAL_STRING("Casa",           getComplemento(copia));

    free(s);
    liberarPessoa(original);
    liberarPessoa(copia);
}

void test_desserializar_stringNULL_retornaNULL(void) {
    Pessoa p = desserializarPessoa(NULL);
    TEST_ASSERT_NULL(p);
}

/* ── main ───────────────────────────────────────────────── */

int main(void) {
    UNITY_BEGIN();

    RUN_TEST(test_criarPessoa_naoRetornaNULL);

    RUN_TEST(test_setCPF_getCPF_retornaValorCorreto);
    RUN_TEST(test_setNome_getNome_retornaValorCorreto);
    RUN_TEST(test_setSobrenome_getSobrenome_retornaValorCorreto);
    RUN_TEST(test_setSexo_getSexo_masculino);
    RUN_TEST(test_setSexo_getSexo_feminino);
    RUN_TEST(test_setNascimento_retornaValoresCorretos);

    RUN_TEST(test_setCEP_getCEP_retornaValorCorreto);
    RUN_TEST(test_setFace_getFace_retornaValorCorreto);
    RUN_TEST(test_setNum_getNum_retornaValorCorreto);
    RUN_TEST(test_setComplemento_getComplemento_retornaValorCorreto);

    RUN_TEST(test_serializar_naoRetornaNULL);
    RUN_TEST(test_serializar_desserializar_mantemTodosOsDados);
    RUN_TEST(test_desserializar_stringNULL_retornaNULL);

    return UNITY_END();
}