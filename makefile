PROG_1 = exemplo:mochila
PROG_2 = 2-PT
PROG_3 = 3-PD
PROG_4 = 4-PCM
PROG_5 = 5-PFM

.DEFAULT_GOAL := help

help:
	@echo "Uso:"
	@echo "  make <número>                # compila"
	@echo "  make run_<número>            # executa"
	@echo "  make clean                   # limpa todos"
	@echo "  make clean_<número>          # limpa um"



1:
	$(MAKE) -C $(PROG_1)

2:
	$(MAKE) -C $(PROG_2)

3:
	@echo "Ainda não implementado"

4:
	@echo "Ainda não implementado"

5:
	@echo "Ainda não implementado"



run:
	make help

run_1:
	$(MAKE) -C $(PROG_1) run

run_2:
	$(MAKE) -C $(PROG_2) run

run_3:
	@echo "Ainda não implementado"

run_4:
	@echo "Ainda não implementado"

run_5:
	@echo "Ainda não implementado"



clean:
	make clean_1 clean_2 clean_3 clean_4 clean_5

clean_1:
	$(MAKE) -C $(PROG_1) clean

clean_2:
	$(MAKE) -C $(PROG_2) clean

clean_3:
	@echo "Ainda não implementado"

clean_4:
	@echo "Ainda não implementado"

clean_5:
	@echo "Ainda não implementado"