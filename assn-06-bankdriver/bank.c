#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <assert.h>


#include "error.h"

#include "bank.h"
#include "branch.h"
#include "account.h"
#include "report.h"

#include <pthread.h>

/*
 * allocate the bank structure and initialize the branches.
 */
Bank*
Bank_Init(int numBranches, int numAccounts, AccountAmount initalAmount,
          AccountAmount reportingAmount,
          int numWorkers)
{

  Bank *bank = malloc(sizeof(Bank));

  if (bank == NULL) {
    return bank;
  }

  pthread_mutex_init(&bank ->lock, NULL);
  pthread_mutex_init(&bank ->repLock, NULL);
  pthread_mutex_init(&bank ->atomLock, NULL);
  sem_init(&bank ->workerScheduler, 0, 0);
  bank->workerDone = 0;
  pthread_mutex_init(&bank ->transferLock, NULL);
  //bank ->balance = 0;

  Branch_Init(bank, numBranches, numAccounts, initalAmount);
  Report_Init(bank, reportingAmount, numWorkers);

  //sem_init(&bank->waiter, 0, 1);
  //bank ->Threads = 16;

  return bank;
}

/*
 * get the balance of the entire bank by adding up all the balances in
 * each branch.
 */
int
Bank_Balance(Bank *bank, AccountAmount *balance)
{
  assert(bank->branches);

  pthread_mutex_lock(&bank->transferLock);
  
  // sem_wait(&bank->waiter);
  // bank ->balance = 1;
  // sem_post(&bank->waiter);

  // for(int i = 0; i < bank ->Threads; i++){
  //    sem_wait(&bank ->waiter);
  // }

  AccountAmount bankTotal = 0;
  for (unsigned int branch = 0; branch < bank->numberBranches; branch++) {
    AccountAmount branchBalance;
    int err = Branch_Balance(bank,bank->branches[branch].branchID, &branchBalance);
    if (err < 0) {
      //bank ->balance = 0;
  //     sem_wait(&bank->waiter);
  // bank ->balance = 0;
  // sem_post(&bank->waiter);
      pthread_mutex_unlock(&bank->transferLock);
      // for(int i = 0; i < bank ->Threads; i++){
      //   sem_post(&bank ->waiter);
      // }
      return err;
    }
    bankTotal += branchBalance;
  }

  *balance = bankTotal;
  
  //bank ->balance = 0;
  // sem_wait(&bank->waiter);
  // bank ->balance = 0;
  // sem_post(&bank->waiter);

  pthread_mutex_unlock(&bank->transferLock);
  // for(int i = 0; i < bank ->Threads; i++){
  //   sem_post(&bank ->waiter);
  // }

  return 0;
}

/*
 * tranverse and validate each branch.
 */
int
Bank_Validate(Bank *bank)
{
  assert(bank->branches);
  int err = 0;

  for (unsigned int branch = 0; branch < bank->numberBranches; branch++) {
    int berr = Branch_Validate(bank,bank->branches[branch].branchID);
    if (berr < 0) {
      err = berr;
    }
  }
  return err;
}

/*
 * compare the data inside two banks and see they are exactly the same;
 * it is called in BankTest.
 */
int
Bank_Compare(Bank *bank1, Bank *bank2)
{
  int err = 0;
  if (bank1->numberBranches != bank2->numberBranches) {
    fprintf(stderr, "Bank num branches mismatch\n");
    return -1;
  }

  for (unsigned int branch = 0; branch < bank1->numberBranches; branch++) {
    int berr = Branch_Compare(&bank1->branches[branch],
                              &bank2->branches[branch]);
    if (berr < 0) {
      err = berr;
    }
  }

  int cerr = Report_Compare(bank1, bank2);
  if (cerr < 0)
    err = cerr;

  return err;

}
