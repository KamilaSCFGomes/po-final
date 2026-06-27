/*---------------- File: main.cpp  ---------------------+
|        Modelo PLI - Problema do Transporte            |
+-------------------------------------------------------+ */

#include <bits/stdc++.h>
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
int O; // Quantidade de origens
vector<int> oferta; // Oferta das origens
int D; // Quantidade de destinos
vector<int> demanda; // Demanda de cada destino
int C; // Quantidade de caminhos
vector<vector<int>> transporte; //Conjunto de opcoes de transporte

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
	IloArray<IloNumVarArray> x(env);
	for( i = 0; i < C; i++ ){
		x.add(IloNumVarArray(env));
		for( j = 0; j < C; j++ ){
			x[i].add(IloIntVar(env, 0, INT_MAX));
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
	int soma = 0;
	//FUNCAO OBJETIVO ---------------------------------------------
	printf("OBJETIVO\n");
	sum.clear();
	for(i=0; i<O; i++){
		for(j=0; j<D; j++){
			if(transporte[i][j] == infinito){continue;}
			sum += (transporte[i][j] * x[i][j]);
		}
	}
	model.add(IloMinimize(env, sum)); //Minimizacao

	//RESTRICOES ---------------------------------------------	
	
	//R1 - Oferta das origens
	printf("RESTRICAO 1\n");
	for(i=0; i<O; i++){ // para toda origem
		sum.clear();
		for(j=0; j<D; j++){
			if(transporte[i][j] == infinito){continue;}
			sum += x[i][j];
		}
		printf("%d <= %d\n",sum, oferta[i]);
		model.add(sum <= oferta[i]);
		numberRes++;
	}

	//R2 - Demandas dos destinos
	printf("RESTRICAO 2\n");
	for(i=0; i<D; i++){ // para toda origem
		printf("sum: %d ",sum);
		sum.clear();
		printf(" -> %d\n", sum);
		for(j=0; j<O; j++){
			if(transporte[j][i] == infinito){continue;}
			sum += x[j][i];
		}
		printf("%d >= %d\n",sum, demanda[i]);
		model.add(sum >= demanda[i]);
		numberRes++;
	}
	printf("FIM\n");


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
		printf("\t");
    	for(int j=0; j<D; j++) printf("%3d ",j);
		printf("\n");
    	for(i=0; i<O; i++){
        	printf("%3d\t",i);
        	for(int j=0; j<D; j++){
				value = IloRound(cplex.getValue(x[i][j]));
	        	printf("%3d ",value);
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

	cin >> O >> D >> C;
    oferta.resize(O);
    transporte.resize(O);
	demanda.resize(D);

	for(i=0; i<O; i++){
		cin >> oferta[i];
		transporte[i].resize(D);
		for(j=0; j<D; j++) transporte[i][j] = infinito;
	}
    
    for(i=0; i<D; i++){
        cin >> demanda[i];
    }

	for(i=0; i<C; i++){
        int ori, des, custo;
        cin >> ori >> des >> custo;
        transporte[ori][des] = custo;
    }

	printf("Verificacao da leitura dos dados:\n");
	printf("Num. Origens: %d\n", O);
    printf("Origem - Oferta:\n");
    for(i=0; i<O; i++) printf("%d: %d\n", i, oferta[i]);
    printf("Num. Destinos: %d\n", D);
    printf("Destino - Demanda:\n");
    for(i=0; i<D; i++) printf("%d: %d\n", i, demanda[i]);
    printf("Num. Caminhos: %d\n", C);
    printf("Matriz de caminhos:\n");
    printf("\t");
    for(int j=0; j<D; j++) printf("%3d ",j);
	printf("\n");
    for(i=0; i<O; i++){
        printf("%3d\t",i);
        for(int j=0; j<D; j++){
			if(transporte[i][j] == infinito) printf("  - ");
	        else printf("%3d ",transporte[i][j]);
        }
        printf("\n");
    }
	printf("\n");

	cplex();

    return 0;
}
