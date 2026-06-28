/*------------------- File: main.cpp  -----------------------+
|         Modelo PLI - Problema de Fluxo Maximo (PFM)        |
+-----------------------------------------------------------*/

/*----------------- Formatação da entrada ------------------+
|															|
+----------------------------------------------------------*/

#include <bits/stdc++.h>
#include <iomanip>
#include <ilcplex/ilocplex.h>

using namespace std;
ILOSTLBEGIN //MACRO - "using namespace" for ILOCPEX

//CPLEX Parameters
#define CPLEX_TIME_LIM 3600 //3600 segundos
//#define CPLEX_COMPRESSED_TREE_MEM_LIM 8128 //8GB
//#define CPLEX_WORK_MEM_LIM 4096 //4GB
//#define CPLEX_VARSEL_MODE 0
/*
* VarSel Modes:
* -1 Branch on variable with minimum infeasibility
* 0 Branch variable automatically selected
* 1 Branch on variable with maximum infeasibility
* 2 Branch based on pseudo costs
* 3 Strong branching
* 4 Branch based on pseudo reduced costs
*
* Default: 0
*/

// Conjuntos do Problema
int N; // Quantidade de nos (vertices)
int M; // Quantidade de arcos (arestas)
int fonte; // origem do fluxo maximo //////////////////////////////////////////////////////////////////
int sumidouro; // sumidouro destino do fluxo maximo /////////////////////////////////////////////////////////////
vector<vector<int>> capacidade; // Matriz de capacidades (uij) de cada arco ////////////////////////////////////////////
vector<vector<bool>> existe; // Matriz de existencia de arcos (true = arco existe) ////////////////////////////////////

int infinito = std::numeric_limits<int>::max();

void cplex(){
    //CPLEX
	IloEnv env; //Define o ambiente do CPLEX

	//Variaveis --------------------------------------------- 
	int i, j; //Auxiliares
	int numberVar = 0; //Total de Variaveis
	int numberRes = 0; //Total de Restricoes


	//---------- MODELAGEM ---------------
	//Definicao - Variaveis de Decisao 2 dimensoes (x_ij) nao binarias (discretas)
	//Limite inferior (lij) = 0 (capacidade minima de fluxo) e limite superior = capacidade[i][j] (uij)
	IloArray<IloNumVarArray> x(env);
	for( i = 0; i < N; i++ ){
		x.add(IloNumVarArray(env));
		for( j = 0; j < N; j++ ){
            
			if(!existe[i][j]){ // arco inexistente ///////////////////////////////////////////////////////////////////
				x[i].add(IloNumVar(env, 0.0, 0.0));
			} else {
				x[i].add(IloNumVar(env, 0.0, capacidade[i][j]));
			}
			numberVar++;
		}
	}

	//Definicao do ambiente modelo ------------------------------------------
	IloModel model ( env );
	
	//Definicao do ambiente expressoes, para os somatorios ---------------------------------
	//Nota: Os somatorios podem ser reaproveitados usando o .clear(),
	//com excecao de quando existe mais de um somatorio em uma mesma restricao.
	IloExpr sum(env); /// Expression for Sum
	IloExpr sum2(env); /// Expression for Sum2

	//FUNCAO OBJETIVO ---------------------------------------------
	sum.clear();
	for(j=0; j<N; j++){
		if(!existe[fonte][j]){continue;}
		sum += x[fonte][j];
	}
	model.add(IloMaximize(env, sum)); // Maximizacao do fluxo que sai da fonte /////////////

	//RESTRICOES ---------------------------------------------	
	
	//R1 - Conservacao (balanco) de fluxo em cada no intermediario: /////////////////////
	//Fluxo de saida - Fluxo de entrada = 0 (nos intermediarios) ////////////////////////
	for(i=0; i<N; i++){ // para todo no
		if(i == fonte || i == sumidouro) continue; // fonte e sumidouro nao tem restricao de balanco ///////////
		sum.clear();
		for(j=0; j<N; j++){
			if(!existe[i][j]){continue;}
			sum += x[i][j];
		}
		sum2.clear();
		for(j=0; j<N; j++){
			if(!existe[j][i]){continue;}
			sum2 += x[j][i];
		}
		model.add(sum - sum2 == 0); // balanco de fluxo: saida - entrada = 0 ////////////
		numberRes++;
	}


	//------ EXECUCAO do MODELO ----------
	time_t timer, timer2;
	IloNum value, objValue;
	double runTime;
	string status;
	
	//Informacoes ---------------------------------------------	
	printf("--------Informacoes da Execucao:----------\n\n");
	printf("#Var: %d\n", numberVar);
	printf("#Restricoes: %d\n", numberRes);
	cout << "Memory usage after variable creation:  " << env.getMemoryUsage() / (1024. * 1024.) << " MB" << endl;
	
	IloCplex cplex(model);
	cout << "Memory usage after cplex(Model):  " << env.getMemoryUsage() / (1024. * 1024.) << " MB" << endl;

	//Setting CPLEX Parameters
	cplex.setParam(IloCplex::TiLim, CPLEX_TIME_LIM);
	//cplex.setParam(IloCplex::TreLim, CPLEX_COMPRESSED_TREE_MEM_LIM);
	//cplex.setParam(IloCplex::WorkMem, CPLEX_WORK_MEM_LIM);
	//cplex.setParam(IloCplex::VarSel, CPLEX_VARSEL_MODE);

	time(&timer);
	cplex.solve();//COMANDO DE EXECUCAO
	time(&timer2);
	
	//cout << "Solution Status: " << cplex.getStatus() << endl;
	//Results
	bool sol = true;
	/*
	Possible Status:
	- Unknown	 
	- Feasible	 
	- Optimal	 
	- Infeasible	 
	- Unbounded	 
	- InfeasibleOrUnbounded	 
	- Error
	*/
	switch(cplex.getStatus()){
		case IloAlgorithm::Optimal: 
			status = "Optimal";
			break;
		case IloAlgorithm::Feasible: 
			status = "Feasible";
			break;
		default: 
			status = "No Solution";
			sol = false;
	}

	cout << endl << endl;
	cout << "Status da FO: " << status << endl;

	if(sol){ 

		//Results
		//int Nbin, Nint, Ncols, Nrows, Nnodes, Nnodes64;
		objValue = cplex.getObjValue();
		runTime = difftime(timer2, timer);
		//Informacoes Adicionais
		//Nbin = cplex.getNbinVars();
		//Nint = cplex.getNintVars();
		//Ncols = cplex.getNcols();
		//Nrows = cplex.getNrows();
		//Nnodes = cplex.getNnodes();
		//Nnodes64 = cplex.getNnodes64();
		//float gap; gap = cplex.getMIPRelativeGap();
		
		cout << "Variaveis de decisao: " << endl;
		printf("    ");
    	for(int j=0; j<N; j++) printf("%3d ",j);
		printf("\n");
    	for(i=0; i<N; i++){
        	printf("%3d",i);
        	for(int j=0; j<N; j++){
				if(!existe[i][j]){ // arco inexistente ///////////////////////////////////////////
					cout << setw(4) << "#";
					continue;
				}
				value = IloRound(cplex.getValue(x[i][j]));
				printf(" %3d", static_cast<int>(value));
        	}
        	printf("\n");
    	}
		printf("\n");
		
		cout << "Funcao Objetivo Valor (Fluxo Maximo) = " << objValue << endl;
		printf("..(%.6lf seconds).\n\n", runTime);

	}else{
		printf("No Solution!\n");
	}

	//Free Memory
	cplex.end();
	sum.end();
	sum2.end();

	cout << "Memory usage before end:  " << env.getMemoryUsage() / (1024. * 1024.) << " MB" << endl;
	env.end();
}
int main(){

	//Leitura dos dados:
	//A partir de um arquivo (in.txt)
	int i, j;

	cin >> N >> M;
	cin >> fonte >> sumidouro;
	capacidade.resize(N);
	existe.resize(N);

	for(i=0; i<N; i++){
		capacidade[i].resize(N, 0); // inicializa com 0 (sem arco) /////////////
		existe[i].resize(N, false); // inicializa com false (sem arco) //////////
	}

	for(i=0; i<M; i++){
        int ori, des, cap;
        cin >> ori >> des >> cap; // sem custo no PFM //////////////////////////////
        capacidade[ori][des] = cap;
        existe[ori][des] = true; // marca arco como existente ////////////////
    }

	printf("Verificacao da leitura dos dados:\n");
	printf("Num. Nos: %d\n", N);
    printf("Fonte: %d | Sumidouro: %d\n", fonte, sumidouro);
    printf("Num. Arcos: %d\n", M);
	printf("\n");
	printf("Matriz de capacidades:\n");
    printf("    ");
    for(int j=0; j<N; j++) printf("%3d ",j);
	printf("\n");
    for(i=0; i<N; i++){
        printf("%3d ",i);
        for(int j=0; j<N; j++){
			if(!existe[i][j]) printf("  # "); // arco inexistente ///////////////////////
            else if(capacidade[i][j] >= 9999) printf(" INF");
	        else printf("%3d ",capacidade[i][j]);
        }
        printf("\n");
    }
	printf("\n");

	cplex();

    return 0;
}