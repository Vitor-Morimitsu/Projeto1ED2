#include "unity.h"
#include "geo.h"
#include "hashFile.h"
#include "svg.h"
#include <stdio.h>
#include <string.h>

#define TEST_GEO_DIR   "test_geo.dir"
#define TEST_GEO_DADOS "test_geo.txt"
#define TEST_SVG_OUT   "test_geo_out.svg"
#define TEST_GEO_FILE  "test_input.geo"

void setUp(void) {
    remove(TEST_GEO_DIR);
    remove(TEST_GEO_DADOS);
    remove(TEST_SVG_OUT);
    remove(TEST_GEO_FILE);
}

void tearDown(void) {
    remove(TEST_GEO_DIR);
    remove(TEST_GEO_DADOS);
    remove(TEST_SVG_OUT);
    remove(TEST_GEO_FILE);
}

/* ── Helpers ──────────────────────────────────────────────── */

// Cria um arquivo .geo temporário com o conteúdo passado
static FILE* criarGeoTemp(const char* conteudo) {
    FILE* f = fopen(TEST_GEO_FILE, "w");
    if (f) { fputs(conteudo, f); fclose(f); }
    return fopen(TEST_GEO_FILE, "r");
}

/* ── Testes de lerGeo ─────────────────────────────────────── */

void test_lerGeo_quadraSimplesInseridaNaHash(void) {
    HashFile hash = criarHashFile(TEST_GEO_DIR, TEST_GEO_DADOS);
    FILE* svg  = fopen(TEST_SVG_OUT, "w");
    FILE* geo  = criarGeoTemp("q 10001 0.0 0.0 100.0 50.0\n");

    TEST_ASSERT_NOT_NULL(hash);
    TEST_ASSERT_NOT_NULL(svg);
    TEST_ASSERT_NOT_NULL(geo);

    lerGeo(geo, svg, hash);
    fclose(geo);
    fclose(svg);

    // Confirma que a quadra foi inserida na hash com a chave "10001"
    char buf[256];
    int encontrado = buscarDadosHashFile(hash, "10001", buf, sizeof(buf));
    TEST_ASSERT_TRUE(encontrado);

    fecharHashFile(hash);
}

void test_lerGeo_cqDefineCorParaProximaQuadra(void) {
    HashFile hash = criarHashFile(TEST_GEO_DIR, TEST_GEO_DADOS);
    FILE* svg = fopen(TEST_SVG_OUT, "w");
    // cq define sw cfill cstrk; depois q cria a quadra com essas cores
    FILE* geo = criarGeoTemp(
        "cq 2px blue red\n"
        "q 20002 10.0 10.0 80.0 40.0\n"
    );

    lerGeo(geo, svg, hash);
    fclose(geo);
    fclose(svg);

    char buf[256];
    buscarDadosHashFile(hash, "20002", buf, sizeof(buf));
    // O dado serializado deve conter "blue" (cfill) e "red" (cstrk)
    TEST_ASSERT_NOT_NULL(strstr(buf, "blue"));
    TEST_ASSERT_NOT_NULL(strstr(buf, "red"));

    fecharHashFile(hash);
}

void test_lerGeo_linhaVaziaIgnorada(void) {
    HashFile hash = criarHashFile(TEST_GEO_DIR, TEST_GEO_DADOS);
    FILE* svg = fopen(TEST_SVG_OUT, "w");
    FILE* geo = criarGeoTemp("\n\nq 30003 5.0 5.0 60.0 60.0\n\n");

    lerGeo(geo, svg, hash);
    fclose(geo);
    fclose(svg);

    char buf[256];
    int ok = buscarDadosHashFile(hash, "30003", buf, sizeof(buf));
    TEST_ASSERT_TRUE(ok);

    fecharHashFile(hash);
}

void test_lerGeo_svgContemRect(void) {
    HashFile hash = criarHashFile(TEST_GEO_DIR, TEST_GEO_DADOS);
    FILE* svg = fopen(TEST_SVG_OUT, "w");
    FILE* geo = criarGeoTemp("q 40004 0.0 0.0 200.0 100.0\n");

    lerGeo(geo, svg, hash);
    fclose(geo);
    fclose(svg);

    // Lê o SVG gerado e verifica que contém a tag <rect
    FILE* svgLido = fopen(TEST_SVG_OUT, "r");
    TEST_ASSERT_NOT_NULL(svgLido);

    char linha[512];
    int achouRect = 0;
    while (fgets(linha, sizeof(linha), svgLido)) {
        if (strstr(linha, "<rect")) { achouRect = 1; break; }
    }
    fclose(svgLido);
    TEST_ASSERT_TRUE(achouRect);

    fecharHashFile(hash);
}

/* ── main ───────────────────────────────────────────────── */

int main(void) {
    UNITY_BEGIN();

    RUN_TEST(test_lerGeo_quadraSimplesInseridaNaHash);
    RUN_TEST(test_lerGeo_cqDefineCorParaProximaQuadra);
    RUN_TEST(test_lerGeo_linhaVaziaIgnorada);
    RUN_TEST(test_lerGeo_svgContemRect);

    return UNITY_END();
}
