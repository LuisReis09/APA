#ifndef UTILS_H
#define UTILS_H
#include "structures.h"
#include <string>

using namespace std;

Problema LerDados(string filePath);
int VerificaSolucao(vector<vector<int>> matriz, vector<int> necessidades, int cap_max, vector<Rota> rotas);

pair<int, int> MelhoresVertices(Problema p);

Problema ProblemaAleatorio(int n, int m, int c, int max);
void PrintProblema(Problema p, string p_name);


#endif