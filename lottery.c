#include "lottery.h"
#include <stdio.h>
#include <string.h>

//Nome unico do algoritmo. Deve ter 4 caracteres.
const char lottName[]="LOTT";
int s;
//=====Funcoes Auxiliares=====

//Funcao que seleciona os tickets para a realição do sorteio
int totalTickets(Process* plist){
    int total = 0;
    Process* p = plist;
    LotterySchedParams *schedParams;

    for (p = plist; p != NULL; p = processGetNext(p))
  {
        schedParams = processGetSchedParams(p);

    if (processGetStatus(p) != PROC_WAITING && processGetStatus(p) != PROC_TERMINATING)
      total  += schedParams->num_tickets;
  }
    return total;
}


//=====Funcoes da API=====

//Funcao chamada pela inicializacao do S.O. para a incializacao do escalonador
//conforme o algoritmo Lottery Scheduling
//Deve envolver a inicializacao de possiveis parametros gerais
//Deve envolver o registro do algoritmo junto ao escalonador
void lottInitSchedInfo() {
    SchedInfo *si = malloc(sizeof(SchedInfo));
    strcpy(si->name, lottName);
    si->initParamsFn = lottInitSchedParams;
    si->scheduleFn = lottSchedule;
    si->releaseParamsFn = lottReleaseParams;
    s = schedRegisterScheduler(si);
}

//Inicializa os parametros de escalonamento de um processo p, chamada
//normalmente quando o processo e' associado ao slot de Lottery
void lottInitSchedParams(Process *p, void *params) {
    schedSetScheduler(p,params,s);
}

//Retorna o proximo processo a obter a CPU, conforme o algortimo Lottery
Process* lottSchedule(Process *plist) {
    int total_tickets = totalTickets(plist);
	int nSorteado = rand() % total_tickets;
    int soma = 0;
    LotterySchedParams *sp;
    Process *p;
    for (p = plist; p != NULL; p = processGetNext(p))
    {
        sp = processGetSchedParams(p);

        if (processGetStatus(p) != PROC_WAITING && processGetStatus(p) != PROC_TERMINATING) {
            soma += sp->num_tickets;
            if (nSorteado <= soma)
                return p;
        }
    }
	return NULL;
}

//Libera os parametros de escalonamento de um processo p, chamada
//normalmente quando o processo e' desassociado do slot de Lottery
//Retorna o numero do slot ao qual o processo estava associado
int lottReleaseParams(Process *p) {
    int slot = processGetSchedSlot(p);
    processSetSchedParams(p,NULL);
    processSetSchedSlot(p,-1);
    free(processGetSchedParams(p));

    return slot;
}

//Transfere certo numero de tickets do processo src para o processo dst.
//Retorna o numero de tickets efetivamente transfeirdos (pode ser menos)
int lottTransferTickets(Process *src, Process *dst, int tickets) {
	//...
    LotterySchedParams *params_src = processGetSchedParams(src);
    LotterySchedParams *params_dst = processGetSchedParams(dst);

    if(params_src->num_tickets == 1)
        return 0;
    else if(params_src->num_tickets < tickets){
        tickets = params_src->num_tickets - 1;
    }

    params_src->num_tickets = params_src->num_tickets - tickets;
    params_dst->num_tickets = params_dst->num_tickets + tickets;

    return tickets;
}