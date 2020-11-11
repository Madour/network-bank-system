/**
 * @file cli.h
 * @author Modar Nasser
 * @brief Command Line Interface for the mini Bank database system
 * @date 2020-11-07
 * 
 * @copyright Copyright (c) 2020
 * 
 */

#ifndef CLI_H
#define CLI_H

#include <stdlib.h>
#include <stdio.h>
#include "db.h"

/**
 * @brief Check if password and customer_id are correct
 * 
 * @param response Response string
 * @param bank Pointer to the Bank database
 * @param customer_id 
 * @param password
 * @return int 1 if password is correct, 0 otherwise
 */
int CLI_LogCustomer(char* response, Bank* bank, unsigned int customer_id, const char* password);

/**
 * @brief Returns the Account that match customer_id and account_id
 * 
 * @param response Response string
 * @param bank Pointer to the Bank database
 * @param customer_id 
 * @param account_id 
 * @return Account* Pointer to the Account, NULL if not found
 */
Account* CLI_FindCustomerAccount(char* response, Bank* bank, unsigned int customer_id, unsigned account_id);

/**
 * @brief Execute the Client command
 * 
 * @param argn Number of arguments
 * @param args Array of argumets
 * @param response Response string
 * @param bank Pointer to the Bank database
 * @return int 1 if connection should be closed, 0 otherwise
 */
int CLI_ExecCmd(int argn, char** args, char* response, Bank* bank);

/**
 * @brief Creates a new Customer in the database
 * 
 * @param response Response string
 * @param bank Pointer to the Bank database
 * @param password New Customer password
 */
void CLI_CreateCustomer(char* response, Bank* bank, char* password);

/**
 * @brief Creates a new Account for the given customer_id
 * 
 * @param response Response string
 * @param bank Pointer to the Bank database
 * @param customer_id 
 */
void CLI_CreateAccount(char* response, Bank* bank, unsigned int customer_id);

/**
 * @brief Execute transaction on the given account_id
 * 
 * @param response Response string
 * @param bank Pointer to the Bank database
 * @param customer_id 
 * @param account_id 
 * @param amount Amount to add to the Account
 */
void CLI_ExecuteTransaction(char* response, Bank* bank, unsigned int customer_id, unsigned int account_id, long long *amount);

/**
 * @brief Get Account balance, print result it in response
 * 
 * @param response Response string
 * @param bank Pointer to the Bank database
 * @param customer_id 
 * @param account_id 
 */
void CLI_FetchAccountBalance(char* response, Bank* bank, unsigned int customer_id, unsigned int account_id);

/**
 * @brief Get Account transactions history, print result in response
 * 
 * @param response Response string
 * @param bank Pointer to the Bank database
 * @param customer_id 
 * @param account_id 
 */
void CLI_FetchAccountHistory(char* response, Bank* bank, unsigned int customer_id, unsigned int account_id);

#endif//CLI_H
