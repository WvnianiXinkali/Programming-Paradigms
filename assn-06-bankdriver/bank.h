#ifndef _BANK_H
#define _BANK_H

#include <semaphore.h>

typedef struct Bank {
  unsigned int numberBranches;
  struct       Branch  *branches;
  struct       Report  *report;
  
  pthread_mutex_t lock;

  pthread_mutex_t repLock;

  pthread_mutex_t atomLock;
  sem_t workerScheduler;
  int workerDone;

  pthread_mutex_t transferLock;
  // int balance;
  // sem_t waiter;
  // int Threads;

} Bank;

#include "account.h"

int Bank_Balance(Bank *bank, AccountAmount *balance);

Bank *Bank_Init(int numBranches, int numAccounts, AccountAmount initAmount,
                AccountAmount reportingAmount,
                int numWorkers);

int Bank_Validate(Bank *bank);
int Bank_Compare(Bank *bank1, Bank *bank2);



#endif /* _BANK_H */
