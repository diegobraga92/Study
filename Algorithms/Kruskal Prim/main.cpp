#include <iostream>
#include <list>
#include <vector>
#include <algorithm>
#include <limits>
#include "FilaDePrioridade.h"
#include "UnionFind.h"

struct Aresta
{
	Aresta() {};
	Aresta(const double& peso_, const int& origem_, const int& destino_) : origem(origem_), destino(destino_), peso(peso_) {};
	// Armazena o peso da aresta e o vertice de origem e destino
	double peso = -1;
	int origem = -1;
	int destino = -1;
};

struct Vertice
{
	Vertice() {};
	Vertice(const double& coordX_, const double& coordY_) : coordX(coordX_), coordY(coordY_) {};

	// Vetor que armazena todos os vertices adjacentes, guardando <peso da aresta, vertice>
	std::vector<std::pair<double, int> > adjacentes;
	// Define peso inicial do Vertice como double max()
	double peso = std::numeric_limits<double>::max();
	// Define pai do Vertice
	int pai = -1;
	// Define se o Vertice já está presente na MST
	bool naMst = false;

	double coordX = 0.0;
	double coordY = 0.0;
	int grupo = 0;

	// Vetor com ponteiro para todas as arestas da MST que tem esse vertice como origem ou destino. Usado para agrupar
	std::vector<Aresta*> arestas;
};

struct Grafo {
	// Vetor que armazena todas as vertices que fazem parte do grafo
	std::vector<Vertice> vertices;
	
	// Vetor que armazena todas as arestas
	std::vector<Aresta> arestas;

	// Lista que armazena todas as arestas que compõe a MST
	std::list<Aresta> arestasMST;

	bool kruskal = false;

	// Construtor
	Grafo(const bool& kruskal_) { kruskal = kruskal_; };

	// Adiciona um vertice e cria arestas com todos os outros vertices existentes
	void adicionarVertice(const double& coordX, const double& coordY)
	{
		vertices.push_back(Vertice(coordX, coordY));

		double peso = 0;

		for (auto vertice = 0; vertice < (int)vertices.size() - 1; vertice++)
		{
			peso = sqrt(pow(coordX - vertices[vertice].coordX, 2) + pow(coordY - vertices[vertice].coordY, 2));
			adicionarAresta(peso, vertices.size() - 1, vertice);
		}
	}

	// Se for usar algoritmo de Kruskal, cria uma nova Aresta, se for Prim adiciona vertices adjacentes
	void adicionarAresta(const double& peso, const int& verticeA, const int& verticeB)
	{
		if(kruskal)
			arestas.push_back(Aresta(peso, verticeA, verticeB));
		else
		{
			vertices[verticeA].adjacentes.push_back(std::make_pair(peso, verticeB));
			vertices[verticeB].adjacentes.push_back(std::make_pair(peso, verticeA));
		}
	}

	// Adiciona aresta na solução e seu ponteiro nos vertices que ela conecta. Usado para agrupar o resultado de Prim
	void adicionarArestaMST(const double& peso, const int& verticeA, const int& verticeB)
	{
		Aresta novaAresta(peso, verticeA, verticeB);
		arestasMST.push_back(novaAresta);
		vertices[verticeA].arestas.push_back(&(arestasMST.back()));
		vertices[verticeB].arestas.push_back(&(arestasMST.back()));
	}
	void adicionarArestaMST(const Aresta& aresta) { adicionarArestaMST(aresta.peso, aresta.origem, aresta.destino); }

	int size() { return (int)vertices.size(); }
};

// Função para agrupar os vertices. Corre por todos as arestas conectadas a um vertice e os classifica com o mesmo grupo
// Para quando acabam as arestas ou quando é encontrada uma arestada excluida (peso -1)
void agruparPrim(Grafo& grafo, const int& indice, const int& grupo)
{
	// Verifica se o vertice já não está em um grupo
	if (grafo.vertices[indice].grupo == 0)
	{
		grafo.vertices[indice].grupo = grupo;

		for (auto aresta : grafo.vertices[indice].arestas)
		{
			if (aresta->peso >= 0)
			{
				if (aresta->origem == indice)
					agruparPrim(grafo, aresta->destino, grupo);
				else
					agruparPrim(grafo, aresta->origem, grupo);
			}
		}
	}
}

void executarPrim(Grafo& grafo, const int& numArestasRemover)
{
	// Fila de prioridade com pares, par representa <peso, vertice> para cada adjacente da MST atual
	// Usada para pegar o vertice adjance do MST cuja aresta tem o menor custo
	FilaDePrioridade filaDePrioridade;

	// Seleciona 0 como vertice inicial
	filaDePrioridade.push(std::make_pair(0, 0));
	grafo.vertices[0].peso = 0;

	while (!filaDePrioridade.empty())
	{
		// Pega o vertice de peso mínimo da fila de prioridade
		int vertice = filaDePrioridade.top().second;
		filaDePrioridade.pop();
		
		// Pula se o vertice selecionado já estiver na MST
		if (grafo.vertices[vertice].naMst) continue;
		
		// Se o vertice tiver um pai, adiciona a aresta na MST do grafo
		if (grafo.vertices[vertice].pai >= 0 && grafo.vertices[vertice].naMst == false)
			grafo.adicionarArestaMST(grafo.vertices[vertice].peso, grafo.vertices[vertice].pai, vertice);

		grafo.vertices[vertice].naMst = true;

		// Itera por todos os vertices adjacentes de Vertice
		for (auto adjacente : grafo.vertices[vertice].adjacentes)
		{
			// Verifica se o vertice adjacente já não está na MST
			// e se o peso dessa aresta é menor que o peso armazenado no vertice adjacente
			if (!grafo.vertices[adjacente.second].naMst && adjacente.first < grafo.vertices[adjacente.second].peso)
			{
				// Atualiza peso do vertice adjacente e adiciona na filaDePrioridade
				grafo.vertices[adjacente.second].peso = adjacente.first;
				grafo.vertices[adjacente.second].pai = vertice;
				filaDePrioridade.push(adjacente);
			}
		}
	}

	// Ordena arestas por peso e invalida as (numArestasRemover) maiores arestas, colocando seu peso como -1
	grafo.arestasMST.sort([](const Aresta& arestaA, const Aresta& arestaB)
	{ return arestaA.peso > arestaB.peso; });

	std::list<Aresta>::iterator it = grafo.arestasMST.begin();
	for (int i = 0; i < numArestasRemover; i++)
	{
		it->peso = -1;
		it++;
	}
	
	// Corre por todos os vertices os classificando em grupos
	int grupo = 0;
	for (auto indice = 0; indice < grafo.size(); indice++)
	{
		if (grafo.vertices[indice].grupo == 0)
		{
			grupo++;
			agruparPrim(grafo, indice, grupo);
		}
	}
	
}

void executarKruskal(Grafo& grafo, const int& numGrupos)
{
	// Ordena arestas por peso crescente
	std::sort(grafo.arestas.begin(), grafo.arestas.end(), [](const Aresta& arestaA, const Aresta& arestaB)
		{ return arestaA.peso < arestaB.peso; });

	// Cria um objecto UnionFind (vector)
	UnionFind unionFind(grafo.size());

	int qtdGruposAtual = grafo.size();

	// Percorre todas as arestas
	for(auto aresta = 0; aresta < (int)grafo.arestas.size(); aresta++)
	{
		// Para se a quantidade de grupos já foi alcançanda
		if (qtdGruposAtual == numGrupos) break;

		// Verifica se a adição dessas arestas não forma um ciclo, 
		// verificando se as raizes dos dois vertices são diferentes
		int raizOrigem = unionFind.find(grafo.arestas[aresta].origem);
		int raizDestino = unionFind.find(grafo.arestas[aresta].destino);

		if (raizOrigem != raizDestino)
		{
			// Faz a união dos dois vertices e substrai 1 da quantidade de grupos atual
			unionFind.Union(grafo.arestas[aresta].origem, grafo.arestas[aresta].destino);
			qtdGruposAtual--;
		}
	}

	// Percorre o vetor do UnionFind e atribui a raiz como o grupo de cada vertice
	for (auto indice = 0; indice < grafo.size(); indice++)
	{
		grafo.vertices[indice].grupo = unionFind.find(indice);
	}
}

int main(int argc, char* argv[])
{
	bool kruskal = false;

	// Verifica se foi passada a flag -k ou -kruskal
	for (int args = 1; args < argc; args++)
	{
		if (std::string(argv[args]) == "-k" || std::string(argv[args]) == "-kruskal")
			kruskal = true;
	}

	/*********************** Entrada de Valores ***********************/
	// Lê o número de entradas
	int numeroPontos;
	std::cin >> numeroPontos;

	double coordX, coordY;
	Grafo grafo(kruskal);

	// Cria os vertices e arestas/adjacentes conforme as coordenadas passadas
	for (auto p = 0; p < numeroPontos; p++)
	{
		std::cin >> coordX >> coordY;
		grafo.adicionarVertice(coordX, coordY);
	}

	/************************** Processamento *************************/
	if (kruskal)
		executarKruskal(grafo, 7);
	else
		executarPrim(grafo, 6);

	/*********************** Saída de Valores ***********************/
	for (auto vertice : grafo.vertices)
		std::cout << vertice.grupo << "\n";

    return 0;
}

