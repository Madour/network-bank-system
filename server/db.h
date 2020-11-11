/**
 * @file db.h
 * @author Modar Nasser
 * @brief Mini Bank database system
 * @date 2020-11-06
 * 
 * @copyright Copyright (c) 2020
 * 
 */

#ifndef DB_H
#define DB_H

#include <stdlib.h>
#include <time.h>


typedef enum DB_Error {
    NONE,
    ID_ERROR,
    PASSWORD_ERROR,
    TRANSACTION_ERROR
} DB_Error;

typedef struct Transaction {
    long long value;
    unsigned int account_id;
    time_t date;
} Transaction;

typedef struct Account {
    unsigned int id;
    unsigned int owner_id;
    unsigned long long balance;
    Transaction history[10];
    int oldest_transaction;
} Account;

typedef struct Customer {
    unsigned int id;
    char password[255];
    int accounts_ids[10];   // un client ne peux pas avoir plus de 10 comptes
    unsigned int accounts_nb;
} Customer;

typedef struct Bank {
    Customer** customers;
    Account** accounts;
    unsigned int nb_customers;
    unsigned int next_customer_id;
    unsigned int nb_accounts;
    unsigned int next_account_id;
} Bank;

/**
 * @brief Create a Bank and return a pointer to the struct
 * 
 * @return Bank* Pointer to the bank
 */
Bank* DB_CreateBank();

/**
 * @brief Free all the memory used
 * 
 * @param bank Pointer to the bank
 */
void DB_FreeBank(Bank* bank);

/**
 * @brief Create a new Customer in the Bank
 * 
 * @param bank Pointer to the Bank
 * @return unsigned int ID of the new Customer
 */
unsigned int DB_CreateCustomer(Bank* bank, const char* password);

/**
 * @brief Create a new Account for a Customer
 * 
 * @param bank Pointer to the Bank
 * @param customer_id Customer ID
 * @return unsigned int ID of the new Account
 */
unsigned int DB_CreateAccount(Bank* bank, unsigned int customer_id);

/**
 * @brief Get pointer to the Account requested
 * 
 * @param bank Pointer to Bank
 * @param customer_id Customer ID
 * @param account_id Account ID to get
 * @return Account* Pointer to the Account, NULL if not found
 */
Account* DB_GetCustomerAccount(Bank* bank, unsigned int customer_id, unsigned int account_id);

/**
 * @brief Check if customer id exists and if password matchs
 * 
 * @param bank Pointer to the Bank
 * @param customer_id Customer ID
 * @param password Password of the customer
 * @return DB_Error NONE when no errors, ID_ERROR or PASSWORD_ERROR otherwise
 */
DB_Error DB_AuthenticateCustomer(Bank* bank, unsigned int customer_id, const char* password);

/**
 * @brief Execute a Transaction on the given Account
 * 
 * @param account Pointer to Account queried
 * @param transaction Transaction type (ADD or WITHDRAW)
 * @param value value to ADD or WITHDRAW to the Account
 * @return DB_Error NONE when no errors, or TRANSACTION_ERROR otherwise
 */
DB_Error DB_ExecuteTransaction(Account* account, long long value);

/**
 * @brief Returns the most transaction on the Account
 * 
 * @param account Pointer to the Account
 * @return Transaction* Pointer to the most recent transaction, NULL if no Transactions found
 */
Transaction* DB_GetAccountLastTransaction(Account* account);

/**
 * @brief Return array of Transactions for the given Account. Returns the number of Transaction via the int pointer.
 * 
 * @param account Pointer to Account queried
 * @param nb_of_transactions Pointer to int, will be filled with the size of the returned array
 * @return Transaction* Pointer to array of Transaction
 */
Transaction* DB_GetAccountHistory(Account* account, int* nb_of_transactions);

#endif
