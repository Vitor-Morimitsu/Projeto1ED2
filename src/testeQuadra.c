#include "unity.h"
#include "quadra.h"
#include <string.h>

void setUp(void) {}
void tearDown(void) {}

/* ── criarQuadra ─────────────────────────────────────────── */

void test_criarQuadra_naoRetornaNULL(void) {
    Quadra q = criarQuadra();
    TEST_ASSERT_NOT_NULL(q);
    liberarQuadra(q);
}

/* ── set / get CEP ───────────────────────────────────────── */

void test_setCEPQuadra_getCEPQuadra_retornaValorCorreto(void) {
    Quadra q = criarQuadra();
    setCEPQuadra(q, "12345-678");
    TEST_ASSERT_EQUAL_STRING("12345-678", getCEPQuadra(q));
    liberarQuadra(q);
}

/* ── set / get dimensões ─────────────────────────────────── */

void test_setDimensoes_getX_retornaCorreto(void) {
    Quadra q = criarQuadra();
    setDimensoesQuadra(q, 10.0f, 20.0f, 50.0f, 30.0f);
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 10.0f, getXQuadra(q));
    liberarQuadra(q);
}

void test_setDimensoes_getY_retornaCorreto(void) {
    Quadra q = criarQuadra();
    setDimensoesQuadra(q, 10.0f, 20.0f, 50.0f, 30.0f);
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 20.0f, getYQuadra(q));
    liberarQuadra(q);
}

void test_setDimensoes_getW_retornaCorreto(void) {
    Quadra q = criarQuadra();
    setDimensoesQuadra(q, 10.0f, 20.0f, 50.0f, 30.0f);
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 50.0f, getWQuadra(q));
    liberarQuadra(q);
}

void test_setDimensoes_getH_retornaCorreto(void) {
    Quadra q = criarQuadra();
    setDimensoesQuadra(q, 10.0f, 20.0f, 50.0f, 30.0f);
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 30.0f, getHQuadra(q));
    liberarQuadra(q);
}

/* ── set / get preenchimento ─────────────────────────────── */

void test_setPreenchimento_getSw_retornaCorreto(void) {
    Quadra q = criarQuadra();
    setPreenchimentoQuadra(q, "2.0px", "blue", "red");
    TEST_ASSERT_EQUAL_STRING("2.0px", getSwQuadra(q));
    liberarQuadra(q);
}

void test_setPreenchimento_getCfill_retornaCorreto(void) {
    Quadra q = criarQuadra();
    setPreenchimentoQuadra(q, "1.0px", "green", "black");
    TEST_ASSERT_EQUAL_STRING("green", getCfillQuadra(q));
    liberarQuadra(q);
}

void test_setPreenchimento_getCstrk_retornaCorreto(void) {
    Quadra q = criarQuadra();
    setPreenchimentoQuadra(q, "1.0px", "green", "black");
    TEST_ASSERT_EQUAL_STRING("black", getCstrkQuadra(q));
    liberarQuadra(q);
}

/* ── serialização / desserialização ──────────────────────── */

void test_serializar_naoRetornaNULL(void) {
    Quadra q = criarQuadra();
    setCEPQuadra(q, "99999-000");
    char* s = serializarQuadra(q);
    TEST_ASSERT_NOT_NULL(s);
    free(s);
    liberarQuadra(q);
}

void test_serializar_desserializar_mantemTodosOsDados(void) {
    Quadra original = criarQuadra();
    setCEPQuadra(original, "99999-000");
    setDimensoesQuadra(original, 5.0f, 15.0f, 100.0f, 80.0f);
    setPreenchimentoQuadra(original, "1.0px", "blue", "red");

    char* s = serializarQuadra(original);
    Quadra copia = desserializarQuadra(s);
    TEST_ASSERT_NOT_NULL(copia);

    TEST_ASSERT_EQUAL_STRING("99999-000", getCEPQuadra(copia));
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 5.0f,   getXQuadra(copia));
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 15.0f,  getYQuadra(copia));
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 100.0f, getWQuadra(copia));
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 80.0f,  getHQuadra(copia));
    TEST_ASSERT_EQUAL_STRING("1.0px", getSwQuadra(copia));
    TEST_ASSERT_EQUAL_STRING("blue",  getCfillQuadra(copia));
    TEST_ASSERT_EQUAL_STRING("red",   getCstrkQuadra(copia));

    free(s);
    liberarQuadra(original);
    liberarQuadra(copia);
}

void test_desserializar_stringNULL_retornaNULL(void) {
    Quadra q = desserializarQuadra(NULL);
    TEST_ASSERT_NULL(q);
}

/* ── main ───────────────────────────────────────────────── */

int main(void) {
    UNITY_BEGIN();

    RUN_TEST(test_criarQuadra_naoRetornaNULL);

    RUN_TEST(test_setCEPQuadra_getCEPQuadra_retornaValorCorreto);

    RUN_TEST(test_setDimensoes_getX_retornaCorreto);
    RUN_TEST(test_setDimensoes_getY_retornaCorreto);
    RUN_TEST(test_setDimensoes_getW_retornaCorreto);
    RUN_TEST(test_setDimensoes_getH_retornaCorreto);

    RUN_TEST(test_setPreenchimento_getSw_retornaCorreto);
    RUN_TEST(test_setPreenchimento_getCfill_retornaCorreto);
    RUN_TEST(test_setPreenchimento_getCstrk_retornaCorreto);

    RUN_TEST(test_serializar_naoRetornaNULL);
    RUN_TEST(test_serializar_desserializar_mantemTodosOsDados);
    RUN_TEST(test_desserializar_stringNULL_retornaNULL);

    return UNITY_END();
}