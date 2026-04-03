#ifndef PESSOA_H
#define PESSOA_H

#include <stdio.h>
#include <stdlib.h>

/*
    Arquivo .h responsável por criar e gerenciar o módulo pessoas.
*/

typedef void* Pessoa;

/// @brief Cria e retorna uma nova pessoa
/// @return Pessoa
Pessoa criarPessoa();

/// @brief Define o CPF de uma pessoa
/// @param p Pessoa já existente
/// @param cpf CPF que será atribuído à pessoa
void setCPF(Pessoa p, int cpf);

/// @brief Define o nome de uma pessoa
/// @param p Pessoa já existente
/// @param nome Nome que será atribuído à uma pessoa
void setNome(Pessoa p, char* nome);

/// @brief Define o sobrenome de uma pessoa
/// @param p Pessoa já existente
/// @param sobrenome Sobrenome que será atribuído à uma pessoa
void setSobrenome(Pessoa p, char* sobrenome);

/// @brief Define o sexo de uma pessoa
/// @param p Pessoa já existente
/// @param sexo Sexo que será atribuído à uma pessoa
void setSexo(Pessoa p, int sexo);

/// @brief Define o nascimento de uma pessoa
/// @param p Pessoa já existente
/// @param dia Dia do nascimento
/// @param mes Mês do nascimento
/// @param ano Ano do nascimento
void setNascimento(Pessoa p, int dia, int mes, int ano);

/// @brief Define o CEP de um morador
/// @param p Pessoa já existente
/// @param CEP CEP
void setCEP(Pessoa p, int CEP);

/// @brief Define a face da quadra em que uma pessoa mora
/// @param p Pessoa já existente
/// @param face Face da quadra 
void setFace(Pessoa p, char face);

/// @brief Define o numero da moradia onde uma pessoa mora
/// @param p Pessoa já existente
/// @param num Numero da moradia
void setNum(Pessoa p, int num);

/// @brief Define o complemento de onde uma pessoa mora
/// @param p Pessoa já existente
/// @param complemento Complemento que será adicionado
void setComplemento(Pessoa p, char* complemento);

/// @brief Retorna o CPF de uma pessoa
/// @param p Pessoa já existente
/// @return CPF
int getCPF(Pessoa p);

/// @brief Retorna o nome da pessoa
/// @param p Pessoa já existente
/// @return Nome
char* getNome(Pessoa p);

/// @brief Retorna o sobrenome da pessoa
/// @param p Pessoa já existente
/// @return Sobrenome
char* getSobrenome(Pessoa p);

/// @brief Retorna o sexo de uma pessoa (0 - homem, 1 - mulher)
/// @param p Pessoa já existente
/// @return Sexo
int getSexo(Pessoa p);

/// @brief Retorna o dia do nascimento de uma pessoa
/// @param p Pessoa já existente
/// @return Dia do nascimento
int getDiaNascimento(Pessoa p);

/// @brief Retorna o mes do nascimento de uma pessoa
/// @param p Pessoa já existente
/// @return Mes do nascimento
int getMesNascimento(Pessoa p);

/// @brief Retorna o ano de nascimento de uma pessoa
/// @param p Pessoa já existente
/// @return Ano de nascimento
int getAnoNascimento(Pessoa p);

/// @brief Retorna o CEP de uma pessoa
/// @param p Pessoa já existente
/// @return CEP
int getCEP(Pessoa p);

/// @brief Retorna a face da quadra em que uma pessoa mora
/// @param p Pessoa já existente
/// @return Letra da face
char getFace(Pessoa p);

/// @brief Retorna o numero da moradia de uma pessoa
/// @param p Pessoa já existente
/// @return Numero da moradia
int getNum(Pessoa p);

/// @brief Retorna o complemento da moradia de uma pessoa
/// @param p Pessoa já existente
/// @return Complemento
char* getComplemento(Pessoa p);

/// @brief Libera o espaço ocupado por uma pessoa na memória
/// @param p Pessoa que será apagada
void liberarPessoa(Pessoa p);

#endif