/**
 * @file db.c
 * @author Modar Nasser
 * @brief Mini Bank database system
 * @date 2020-11-06
 * 
 * @copyright Copyright (c) 2020
 * 
 */

#include "db.h"
#include <string.h>
#include <stdio.h>

Bank* DB_CreateBank() {
    Bank* new_bank = malloc(sizeof(Bank));

    new_bank->customers = NULL;
    new_bank->nb_customers = 0;

    new_bank->accounts = NULL;
    new_bank->nb_accounts = 0;

    // IDs start at 1, ID 0 is error
    new_bank->next_customer_id = 1;
    new_bank->next_account_id = 1;
    return new_bank;
}

void DB_FreeBank(Bank* bank) {
    for (unsigned int i = 0; i < bank->nb_customers; ++i) {
        free(bank->customers[i]);
    }
    free(bank->customers);

    for (unsigned int i = 0; i < bank->nb_accounts; ++i) {
        free(bank->accounts[i]);
    }
    free(bank->accounts);

    free(bank);
}

unsigned int DB_CreateCustomer(Bank* bank, const char* password) {
    bank->nb_customers += 1;
    // create new array of size nb_customers+1
    Customer** new_array = malloc(sizeof(Customer*) * bank->nb_customers);
    // copy data from old array to new one
    memcpy(new_array, bank->customers, sizeof(Customer*) * (bank->nb_customers-1));
    // free the old array
    free(bank->customers);
    // assign the new array to the bank
    bank->customers = new_array;

    // create new customer
    Customer* new_customer = malloc(sizeof(Customer));
    new_customer->id = bank->next_customer_id++;
    strcpy(new_customer->password, password);
    new_customer->accounts_nb = 0;
    bank->customers[bank->nb_customers-1] = new_customer;

    return new_customer->id;
}

unsigned int DB_CreateAccount(Bank* bank, unsigned int customer_id) {
    Customer* owner = bank->customers[customer_id-1];
    if (owner->accounts_nb == 10) {
        fprintf(stderr, "Error : User ID %d already reached the maximum number of accounts (10).\n", owner->id);
        return 0;
    }
    bank->nb_accounts += 1;
    // create new array of size nb_accounts+1
    Account** new_array = malloc(sizeof(Account*) * bank->nb_accounts);
    // copy data from old array to new one
    memcpy(new_array, bank->accounts, sizeof(Account*) * (bank->nb_accounts-1));
    // free the old array
    free(bank->accounts);
    // assign the new array to the bank
    bank->accounts = new_array;

    Account* new_account = malloc(sizeof(Account));
    new_account->id = bank->next_account_id++;
    new_account->owner_id = customer_id;
    new_account->balance = 0;
    new_account->oldest_transaction = -10; // -10 is on purpose
    /* How Account history works :
        Every time a new Transaction is done on the Account, 
        oldest_transaction will be incremented, when it reaches
        0 we know the history is full and we need to replace the oldest
        record, which will be the Transaction 0 first, then 1 then 2 etc.
        When olders_transaction is 10, it is reset to 0 and we repeat the process.
    */
    bank->accounts[bank->nb_accounts-1] = new_account;

    // add the Account to the Customer
    owner->accounts_ids[owner->accounts_nb++] = new_account->id;

    return new_account->id;
}

Account* DB_GetCustomerAccount(Bank* bank, unsigned int customer_id, unsigned int account_id) {
    if (customer_id > bank->nb_customers)
        return NULL;
    
    Customer* customer = bank->customers[customer_id-1];

    for (unsigned int i = 0; i < customer->accounts_nb; ++i)
        if (account_id == customer->accounts_ids[i])
            return bank->accounts[account_id-1];

    return NULL;
}

DB_Error DB_AuthenticateCustomer(Bank* bank, unsigned int customer_id, const char* password) {
    if (customer_id > bank->nb_customers)
        return ID_ERROR;
    
    if (strcmp(bank->customers[customer_id-1]->password, password) != 0) 
        return PASSWORD_ERROR;

    return NONE;
}


DB_Error DB_ExecuteTransaction(Account* account, long long value) {
    // create transaction
    Transaction new_transaction;
    new_transaction.account_id = account->id;
    new_transaction.date = time(NULL);
    new_transaction.value = value;

    // check account balance first
    if ((long long)account->balance + value < 0)
        return TRANSACTION_ERROR;
    // execute transaction
    account->balance += new_transaction.value;
    
    // add transaction to history
    if (account->oldest_transaction < 0)
        account->history[10+account->oldest_transaction] = new_transaction;
    else
        account->history[account->oldest_transaction] = new_transaction;

    if (++account->oldest_transaction == 10)
        account->oldest_transaction = 0;

    return NONE;
}

Transaction* DB_GetAccountLastTransaction(Account* account) {
    Transaction* last_transaction = NULL;

    if (account->oldest_transaction != -10) {
        // less than 10 transactions
        if (account->oldest_transaction <= 0)
            last_transaction = &(account->history[9+account->oldest_transaction]);
        // more than 10 transactions
        else
            last_transaction = &(account->history[account->oldest_transaction-1]);
    }
    return last_transaction;                 
}

Transaction* DB_GetAccountHistory(Account* account, int* nb_of_transactions) {
    *nb_of_transactions = 10;
    if (account->oldest_transaction < 0)
        *nb_of_transactions = (10+account->oldest_transaction);

    return account->history;
}
