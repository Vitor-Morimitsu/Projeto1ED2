#include "quadra.h"

//Arquivoo criado para testar o módulo quadra
int main()
{
    int cep = 123456789;
    float x = 25.00;
    float y = 50.32;
    float w = 60.32;
    float h = 70.70;

    Quadra novaQuadra = criarQuadra();
    setCEPQuadra(novaQuadra, cep);
    setDimensoesQuadra(novaQuadra, x,y,w,h);
    setPreenchimentoQuadra(novaQuadra,"1.0px", "black","black");

    printf("Cep da quadra: %d\n, dimensões da quadra: %f %f %f %f\n", getCEPQuadra(novaQuadra), x,y,w,h);

}