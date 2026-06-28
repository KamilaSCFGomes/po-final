/*---------------- File: main.cpp  ---------------------+
| Modelo PLI - Problema de Fluxo de Custo Minimo (PFCM) |
+------------------------------------------------------*/

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
int N; // Quantidade de nos (vertices) ////////////////////////////////////////////////////////////////////////////////
int M; // Quantidade de arcos (arestas) ///////////////////////////////////////////////////////////////////////////////
vector<int> b; // Saldo de cada no: oferta (b>0), demanda (b<0) ou transbordo (b=0) ////////////////////////////////////
vector<vector<int>> custo; // Matriz de custos de cada arco ////////////////////////////////////////////////////////////
vector<vector<int>> capacidade; // Matriz de capacidades (uij) de cada arco ////////////////////////////////////////////

int infinito = std::numeric_limits<int>::max();

void cplex(){
    //CPLEX
	IloEnv env; //Define o ambiente do CPLEX

	//Variaveis --------------------------------------------- 
	int i, j; //Auxiliares
	int numberVar = 0; //Total de Variaveis
	int numberRes = 0; //Total de Restricoes


	//---------- MODELAGEM ---------------

	//Definicao - Variaveis de Decisao 2 dimensoes (x_ij) não binárias (discretas)
	//Limite inferior (lij) = 0 (capacidade minima de fluxo) e limite superior = capacidade[i][j] (uij) //////////////////////
	IloArray<IloNumVarArray> x(env);
	for( i = 0; i < N; i++ ){
		x.add(IloNumVarArray(env));
		for( j = 0; j < N; j++ ){
            
			if(custo[i][j] == infinito){
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
	for(i=0; i<N; i++){
		for(j=0; j<N; j++){
			if(custo[i][j] == infinito){continue;}
			sum += (custo[i][j] * x[i][j]);
		}
	}
	model.add(IloMinimize(env, sum)); //Minimizacao

	//RESTRICOES ---------------------------------------------	
	
	//R1 - Conservacao (balanco) de fluxo em cada no: //////////////////////
	//Fluxo de saida - Fluxo de entrada = saldo do no (oferta/demanda/transbordo) //////////////////////
	for(i=0; i<N; i++){ // para todo no //////////////////////
		sum.clear();
		for(j=0; j<N; j++){
			if(custo[i][j] == infinito){continue;}
			sum += x[i][j];
		}
		sum2.clear();
		for(j=0; j<N; j++){
			if(custo[j][i] == infinito){continue;}
			sum2 += x[j][i];
		}
		model.add(sum - sum2 == b[i]);
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
				if(custo[i][j] == infinito){
					cout << setw(4) << "#";
					continue;
				}
				value = IloRound(cplex.getValue(x[i][j]));
				printf(" %3d", static_cast<int>(value));
        	}
        	printf("\n");
    	}
		printf("\n");
		
		cout << "Funcao Objetivo Valor = " << objValue << endl;
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
	b.resize(N);
	custo.resize(N);
	capacidade.resize(N);

	for(i=0; i<N; i++){
		custo[i].resize(N);
		capacidade[i].resize(N);
		for(j=0; j<N; j++) custo[i][j] = infinito;
	}

	for(i=0; i<N; i++){
		cin >> b[i];
	}

	for(i=0; i<M; i++){
        int ori, des, cap, cus;
        cin >> ori >> des >> cap >> cus;
        custo[ori][des] = cus;
        capacidade[ori][des] = cap;
    }

	printf("Verificacao da leitura dos dados:\n");
	printf("Num. Nos: %d\n", N);
    printf("No - Saldo (oferta/demanda/transbordo):\n");
    for(i=0; i<N; i++) printf("%d: %d\n", i, b[i]);
    printf("Num. Arcos: %d\n", M);
    printf("Matriz de custos:\n");
    printf("    ");
    for(int j=0; j<N; j++) printf("%3d ",j);
	printf("\n");
    for(i=0; i<N; i++){
        printf("%3d ",i);
        for(int j=0; j<N; j++){
			if(custo[i][j] == infinito) printf("  # ");
	        else printf("%3d ",custo[i][j]);
        }
        printf("\n");
    }
	printf("\n");
	printf("Matriz de capacidades:\n");
    printf("    ");
    for(int j=0; j<N; j++) printf("%3d ",j);
	printf("\n");
    for(i=0; i<N; i++){
        printf("%3d ",i);
        for(int j=0; j<N; j++){
			if(custo[i][j] == infinito) printf("  # ");
            else if(capacidade[i][j] >= 9999) printf(" INF");
	        else printf("%3d ",capacidade[i][j]);
        }
        printf("\n");
    }
	printf("\n");

	cplex();

    return 0;
}