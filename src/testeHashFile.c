#include "unity.h"
#include "hashFile.h"
#include <stdio.h>
#include <string.h>

// Nomes dos arquivos temporários usados nos testes
#define TEST_DIR   "test_hash.dir"
#define TEST_DADOS "test_hash.txt"

// Limpa os arquivos antes de cada teste para garantir estado zerado
void setUp(void) {
    remove(TEST_DIR);
    remove(TEST_DADOS);
}

// Apaga os arquivos após cada teste
void tearDown(void) {
    remove(TEST_DIR);
    remove(TEST_DADOS);
}

static HashFile criarHashLimpa(void) {
    return criarHashFile(TEST_DIR, TEST_DADOS);
}

void test_criarHashFile_naoRetornaNULL(void) {
    HashFile h = criarHashLimpa();
    TEST_ASSERT_NOT_NULL(h);
    fecharHashFile(h);
}

void test_criarHashFile_profundidadeInicialEh1(void) {
    HashFile h = criarHashLimpa();
    TEST_ASSERT_EQUAL_INT(1, getProfundidadeHash(h));
    fecharHashFile(h);
}

void test_inserir_buscar_chaveSimplesEncontrada(void) {
    HashFile h = criarHashLimpa();

    inserirDadoHashFile(h, "chave1", "dado_um");

    char buf[HASHFILE_TAM_BUF];
    int encontrado = buscarDadosHashFile(h, "chave1", buf, HASHFILE_TAM_BUF);

    TEST_ASSERT_TRUE(encontrado);
    TEST_ASSERT_EQUAL_STRING("dado_um", buf);

    fecharHashFile(h);
}

void test_buscar_chaveInexistenteRetorna0(void) {
    HashFile h = criarHashLimpa();

    char buf[HASHFILE_TAM_BUF];
    int encontrado = buscarDadosHashFile(h, "nao_existe", buf, HASHFILE_TAM_BUF);

    TEST_ASSERT_FALSE(encontrado);

    fecharHashFile(h);
}

void test_inserir_multiplosRegistros_todosEncontrados(void) {
    HashFile h = criarHashLimpa();

    inserirDadoHashFile(h, "cpf_A", "dado_A");
    inserirDadoHashFile(h, "cpf_B", "dado_B");
    inserirDadoHashFile(h, "cpf_C", "dado_C");

    char buf[HASHFILE_TAM_BUF];

    buscarDadosHashFile(h, "cpf_A", buf, HASHFILE_TAM_BUF);
    TEST_ASSERT_EQUAL_STRING("dado_A", buf);

    buscarDadosHashFile(h, "cpf_B", buf, HASHFILE_TAM_BUF);
    TEST_ASSERT_EQUAL_STRING("dado_B", buf);

    buscarDadosHashFile(h, "cpf_C", buf, HASHFILE_TAM_BUF);
    TEST_ASSERT_EQUAL_STRING("dado_C", buf);

    fecharHashFile(h);
}

/* ── Teste de atualização ───────────────────────────────── */

void test_inserir_mesmaChave_atualizaDado(void) {
    HashFile h = criarHashLimpa();

    inserirDadoHashFile(h, "chave1", "dado_antigo");
    inserirDadoHashFile(h, "chave1", "dado_novo");  // atualiza

    char buf[HASHFILE_TAM_BUF];
    buscarDadosHashFile(h, "chave1", buf, HASHFILE_TAM_BUF);

    TEST_ASSERT_EQUAL_STRING("dado_novo", buf);

    fecharHashFile(h);
}

/* ── Testes de remoção ──────────────────────────────────── */

void test_remover_chaveExistente_naoEncontradaDepois(void) {
    HashFile h = criarHashLimpa();

    inserirDadoHashFile(h, "chave_rem", "algum_dado");
    removerDadosHashFile(h, "chave_rem");

    char buf[HASHFILE_TAM_BUF];
    int encontrado = buscarDadosHashFile(h, "chave_rem", buf, HASHFILE_TAM_BUF);

    TEST_ASSERT_FALSE(encontrado);

    fecharHashFile(h);
}

void test_remover_naoAfetaOutrasChaves(void) {
    HashFile h = criarHashLimpa();

    inserirDadoHashFile(h, "fica",   "dado_fica");
    inserirDadoHashFile(h, "remove", "dado_remove");
    removerDadosHashFile(h, "remove");

    char buf[HASHFILE_TAM_BUF];
    int encontrado = buscarDadosHashFile(h, "fica", buf, HASHFILE_TAM_BUF);

    TEST_ASSERT_TRUE(encontrado);
    TEST_ASSERT_EQUAL_STRING("dado_fica", buf);

    fecharHashFile(h);
}

/* ── Teste de split (muitos registros) ──────────────────── */

void test_inserirMuitasChaves_splitAutomatico(void) {
    HashFile h = criarHashLimpa();

    // Insere mais de 10 registros (tamanho do bucket) para forçar splits
    char chave[32], dado[64];
    for (int i = 0; i < 25; i++) {
        snprintf(chave, sizeof(chave), "chave_%03d", i);
        snprintf(dado,  sizeof(dado),  "dado_%03d",  i);
        inserirDadoHashFile(h, chave, dado);
    }

    // Verifica que todos os 25 ainda são encontrados após os splits
    char buf[HASHFILE_TAM_BUF];
    for (int i = 0; i < 25; i++) {
        snprintf(chave, sizeof(chave), "chave_%03d", i);
        snprintf(dado,  sizeof(dado),  "dado_%03d",  i);
        int ok = buscarDadosHashFile(h, chave, buf, HASHFILE_TAM_BUF);
        TEST_ASSERT_TRUE(ok);
        TEST_ASSERT_EQUAL_STRING(dado, buf);
    }

    // Profundidade deve ter crescido (split ocorreu)
    TEST_ASSERT_GREATER_THAN(1, getProfundidadeHash(h));

    fecharHashFile(h);
}

/* ── main ───────────────────────────────────────────────── */

int main(void) {
    UNITY_BEGIN();

    RUN_TEST(test_criarHashFile_naoRetornaNULL);
    RUN_TEST(test_criarHashFile_profundidadeInicialEh1);

    RUN_TEST(test_inserir_buscar_chaveSimplesEncontrada);
    RUN_TEST(test_buscar_chaveInexistenteRetorna0);
    RUN_TEST(test_inserir_multiplosRegistros_todosEncontrados);

    RUN_TEST(test_inserir_mesmaChave_atualizaDado);

    RUN_TEST(test_remover_chaveExistente_naoEncontradaDepois);
    RUN_TEST(test_remover_naoAfetaOutrasChaves);

    RUN_TEST(test_inserirMuitasChaves_splitAutomatico);

    return UNITY_END();
}
