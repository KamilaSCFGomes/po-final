# Trabalho Prático Final - Pesquisa Operacional
Este é um Trabalho avaliativo para a disciplina de Pesquisa Operacional, que consiste na implementação de modelos de PLI em alguns problemas clássicos.

> Alunos: Kamila S. de C. F. Gomes e Pedro Henrique A. Dornelas

# Dependências

É necessário ter o ILOGCPLEX versão instalado.

# Arquivos

Cada pasta representa um problema diferente que resolvido para o trabalho.

As entradas para cada problema estão no arquivo `in.txt`. Para cada problema, há um comentário explicando a formatação das entradas no início do arquivo `main.cpp`.

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