# Trabalho Prático Final - Pesquisa Operacional
Este é um Trabalho avaliativo para a disciplina de Pesquisa Operacional, que consiste na implementação de modelos de PLI em alguns problemas clássicos.

> Alunos: Kamila S. de C. F. Gomes e Pedro Henrique A. Dornelas

# Dependências

É necessário ter o ILOGCPLEX instalado.

# Arquivos

Cada pasta representa um problema diferente que resolvido para o trabalho.

As entradas para cada problema estão no arquivo `in.txt`. No caso de entradas com grafos, a primeira linha deve conter dois inteiros N e M que indicam o número de vértices e arestas, respectivamente, e as próximas M linhas devem ser formadas por três inteiros A, B e C, que indicam que existe uma ligação de valor C entre os vértices A, B.


O código para a resolução do problema está no arquivo `main.cpp`.

O arquivo `makefile` contém as informações para compilação do programa.

# Execução

Para compilar um problema, digite no terminal o comando `make`, seguido do número do probema desejado.

```
make [Número do problema]
```

Esse comando irá gerar um arquivo objeto `main.o` e um arquivo executável `main.exe` na pasta do problema.

Para executar um problema, digite no terminal o comando `make run`, seguido do número do programa desejado.

```
make run_[Número do problema]
```

Para limpar os arquivos `main.o` e `main.exe`, digite no terminal o comando `make clean` para limpar todas as pastas, ou especifique o número de um problema para limpar apenas a pasta daquele problema.

```
make clean
```
```
make clean_[Número do problema]
```