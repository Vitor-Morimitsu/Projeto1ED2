#include "../unity/unity.h"
#include "quadra.h"
#include <string.h>

// setUp e tearDown são obrigatórios no Unity
void setUp(void) {}
void tearDown(void) {}

/* ── Testes de criarQuadra ─────────────────────── */

void test_criarQuadra_naoRetornaNULL(void) {
    Quadra q = criarQuadra();
    TEST_ASSERT_NOT_NULL(q);
    liberarQuadra(q);
}

/* ── Testes de setCEP / getCEP ─────────────────── */

void test_setCEPQuadra_retornaValorCorreto(void) {
    Quadra q = criarQuadra();
    setCEPQuadra(q, "12345-678");
    TEST_ASSERT_EQUAL_STRING("12345-678", getCEPQuadra(q));
    liberarQuadra(q);
}

/* ── Testes de setDimensoes / getters ──────────── */

void test_setDimensoesQuadra_retornaValoresCorretos(void) {
    Quadra q = criarQuadra();
    setDimensoesQuadra(q, 10.0f, 20.0f, 50.0f, 30.0f);
    TEST_ASSERT_EQUAL_FLOAT(10.0f, getXQuadra(q), 0.001f);
    TEST_ASSERT_EQUAL_FLOAT(20.0f, getYQuadra(q), 0.001f);
    TEST_ASSERT_EQUAL_FLOAT(50.0f, getWQuadra(q), 0.001f);
    TEST_ASSERT_EQUAL_FLOAT(30.0f, getHQuadra(q), 0.001f);
    liberarQuadra(q);
}

/* ── Testes de serialização / desserialização ──── */

void test_serializar_desserializar_mantemDados(void) {
    Quadra original = criarQuadra();
    setCEPQuadra(original, "99999-000");
    setDimensoesQuadra(original, 5.0f, 15.0f, 100.0f, 80.0f);
    setPreenchimentoQuadra(original, "1.0px", "blue", "red");

    char* serializado = serializarQuadra(original);
    TEST_ASSERT_NOT_NULL(serializado);

    Quadra copia = desserializarQuadra(serializado);
    TEST_ASSERT_NOT_NULL(copia);

    TEST_ASSERT_EQUAL_STRING("99999-000", getCEPQuadra(copia));
    TEST_ASSERT_EQUAL_FLOAT(5.0f,   getXQuadra(copia), 0.001f);
    TEST_ASSERT_EQUAL_FLOAT(15.0f,  getYQuadra(copia), 0.001f);
    TEST_ASSERT_EQUAL_FLOAT(100.0f, getWQuadra(copia), 0.001f);
    TEST_ASSERT_EQUAL_FLOAT(80.0f,  getHQuadra(copia), 0.001f);

    free(serializado);
    liberarQuadra(original);
    liberarQuadra(copia);
}

/* ── main ──────────────────────────────────────── */
int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_criarQuadra_naoRetornaNULL);
    RUN_TEST(test_setCEPQuadra_retornaValorCorreto);
    RUN_TEST(test_setDimensoesQuadra_retornaValoresCorretos);
    RUN_TEST(test_serializar_desserializar_mantemDados);
    return UNITY_END();
}
