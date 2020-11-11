/**
 * @file cli.c
 * @author Modar Nasser
 * @brief Command Line Interface for the mini Bank database system
 * @date 2020-11-07
 * 
 * @copyright Copyright (c) 2020
 * 
 */


#include <string.h>
#include "cli.h"
#include "common/utils.h"

int CLI_LogCustomer(char* response, Bank* bank, unsigned int customer_id, const char* password) {
    DB_Error db_error = DB_AuthenticateCustomer(bank, customer_id, password);
    if (db_error == ID_ERROR) {
        sprintf(response, "KO - Customer ID %d does not exist", customer_id);
        return 0;
    }
    else if (db_error == PASSWORD_ERROR) {
        sprintf(response, "KO - Wrong password for Customer %d", customer_id);
        return 0;
    }
    return 1;
}

Account* CLI_FindCustomerAccount(char* response, Bank* bank, unsigned int customer_id, unsigned account_id) {
    Account* account = DB_GetCustomerAccount(bank, customer_id, account_id);
    if (account == NULL)
        sprintf(response, "KO - Customer ID %d does not own Account ID %d", customer_id, account_id);
    return account;
}

int CLI_ExecCmd(int argn, char** args, char* response, Bank* bank) {
    if (argn == 0) {
        strcpy(response, "KO - Please enter a command");
        return 0;
    }
            
    if (strcmp(args[0], "exit") == 0) {
        strcpy(response, "OK");
        return 1;
    }
    
    if (strcmp(args[0], "CREER") == 0) {
        if (argn < 3 ) {
            strcpy(response, "KO - Usage : CREER CLIENT <password> or CREER COMPTE <customer_id> <password>");
            return 0;
        }
        if (strcmp(args[1], "CLIENT") == 0) {
            CLI_CreateCustomer(response, bank, args[2]);
        }
        else if (strcmp(args[1], "COMPTE") == 0) {
            if (argn < 4) {
                strcpy(response, "KO - Usage : CREER COMPTE <customer_id> <password>");
                return 0;
            }
            if (!isUInt(args[2])) {
                strcpy(response, "KO - Customer ID should be an unsigned int value");
                return 0;
            }
            unsigned int cust_id = strtoul(args[2], NULL, 10);
            if (CLI_LogCustomer(response, bank, cust_id, args[3])) 
                CLI_CreateAccount(response, bank, cust_id);
        }
        return 0;
    } // endif CREER

    if (strcmp(args[0], "AJOUT") == 0 || strcmp(args[0], "RETRAIT") == 0) {
        if (argn < 5) {
            strcpy(response, "KO - Usage : AJOUT <customer_id> <account_id> <password> <amount>");
            return 0;
        }
        if (!isUInt(args[1]) || !isUInt(args[2]) || !isUInt(args[4])) {
            strcpy(response, "KO - Customer ID, Account ID and amount should be unsigned int values");
            return 0;
        }
        unsigned int cust_id = strtoull(args[1], NULL, 10);
        unsigned int acc_id = strtoull(args[2], NULL, 10);
        long long amount = strtoll(args[4], NULL, 10);
        if (strcmp(args[0], "RETRAIT") == 0)
            amount = -amount;
        if (CLI_LogCustomer(response, bank, cust_id, args[3])) {
            CLI_ExecuteTransaction(response, bank, cust_id, acc_id, &amount);
        }
        return 0;
    } // endif AJOUT

    if (strcmp(args[0], "SOLDE") == 0) {
        if (argn < 4) {
            strcpy(response, "KO - Usage : SOLDE <customer_id> <account_id> <password>");
            return 0;
        }
        if (!isUInt(args[1]) || !isUInt(args[2])) {
            strcpy(response, "KO - Customer ID and Account ID should be unsigned int values");
            return 0;
        }
        unsigned int cust_id = strtoul(args[1], NULL, 10);
        unsigned int acc_id = strtoul(args[2], NULL, 10);
        if (CLI_LogCustomer(response, bank, cust_id, args[3])) {
            CLI_FetchAccountBalance(response, bank, cust_id, acc_id);
        }
        return 0;
    } // endif SOLDE

    if (strcmp(args[0], "OPERATIONS") == 0) {
        if (argn < 4) {
            strcpy(response, "KO - Usage : OPERATIONS <customer_id> <account_id> <password>");
            return 0;
        }
        if (!isUInt(args[1]) || !isUInt(args[2])) {
            strcpy(response, "KO - Customer ID and Account ID should be unsigned int values");
            return 0;
        }
        unsigned int cust_id = strtoul(args[1], NULL, 10);
        unsigned int acc_id = strtoul(args[2], NULL, 10);
        if (CLI_LogCustomer(response, bank, cust_id, args[3])) {
            CLI_FetchAccountHistory(response, bank, cust_id, acc_id);
        }
        return 0;
    } // endif OPERATIONS

    sprintf(response, "KO - Command \"%s\" not found", args[0]);
    return 0;
}


void CLI_CreateCustomer(char* response, Bank* bank, char* password) {
    if (strlen(password) < 8) {
        strcpy(response, "KO - <password> should be at least 8 characters long");
    }
    else {
        unsigned int customer_id = DB_CreateCustomer(bank, password);
        sprintf(response, "OK - Customer ID : %d", customer_id);
    }
}

void CLI_CreateAccount(char* response, Bank* bank, unsigned int customer_id) {
    unsigned int acc_id = DB_CreateAccount(bank, customer_id);
    if (acc_id == 0)  // Customer already has 10 accounts
        sprintf(response, "KO - Can't create account for Customer %d (maximum limit reached)", customer_id);
    else 
        sprintf(response, "OK - Account ID : %d", acc_id);
}

void CLI_ExecuteTransaction(char* response, Bank* bank, unsigned int customer_id, unsigned int account_id, long long *amount) {
    Account* account;
    if ( (account = CLI_FindCustomerAccount(response, bank, customer_id, account_id)) ) {
        DB_Error db_error = DB_ExecuteTransaction(account, *amount);
        if (db_error == TRANSACTION_ERROR)
            sprintf(response, "KO - Failed to execute the transaction (Account balance : %lld)", account->balance);
        else {
            sprintf(response, "OK - Transaction successful (Account new balance : %lld $)", account->balance);
        }
    }
}

void CLI_FetchAccountBalance(char* response, Bank* bank, unsigned int customer_id, unsigned int account_id) {
    Account* account;
    if ( (account = CLI_FindCustomerAccount(response, bank, customer_id, account_id)) ) {
        sprintf(response, "RES_SOLDE - %lld $", account->balance);
        Transaction* last_transaction = DB_GetAccountLastTransaction(account);
        if (last_transaction != NULL) {
            char tmp_buffer[100];
            struct tm *h_date = localtime(&last_transaction->date);
            sprintf(tmp_buffer, " - Last transaction: %lld $ (%02d/%02d/%04d at %02d:%02d:%02d %s)",
                            last_transaction->value,
                        h_date->tm_mday, h_date->tm_mon+1, h_date->tm_year+1900,
                        h_date->tm_hour, h_date->tm_min, h_date->tm_sec, 
                        h_date->tm_zone
            );
            strcat(response, tmp_buffer);
        }
    }
}

void CLI_FetchAccountHistory(char* response, Bank* bank, unsigned int customer_id, unsigned int account_id) {
    Account* account;
    if ( (account = CLI_FindCustomerAccount(response, bank, customer_id, account_id)) ) {
        strcpy(response, "RES_OPERATIONS - ");
        int nb_transactions = account->oldest_transaction < 0 ? 10+account->oldest_transaction : 10;
        char tmp_buffer[100];
        sprintf(tmp_buffer, "Last %d transactions on Account %d :", nb_transactions, account_id);
        strcat(response, tmp_buffer);

        for (int i = 0; i < nb_transactions; ++i) {
            Transaction* tr = &account->history[i];
            struct tm *h_date = localtime(&tr->date);
            sprintf(tmp_buffer, "\n * %8s %4d $ (%02d/%02d/%04d at %02d:%02d:%02d %s)", 
                    tr->value < 0 ? "RETRAIT" : "AJOUT", abs(tr->value),
                    h_date->tm_mday, h_date->tm_mon+1, h_date->tm_year+1900,
                    h_date->tm_hour, h_date->tm_min, h_date->tm_sec, 
                    h_date->tm_zone
            );
            strcat(response, tmp_buffer);
        }
    }
}
