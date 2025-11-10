# Embedded Systems Task: Bank Accounts Conundrum (C Implementation)

This project solves the multi-threaded bank account transfer problem. It demonstrates concurrent programming principles using POSIX threads, focusing specifically on synchronization and deadlock prevention.

## Requirements and Constraints

* **Language:** C (using POSIX threads).
* **Goal:** Two threads periodically transfer funds between two accounts.
* **Synchronization:** Uses two mutexes (one per account) and a strict lock ordering (`Account 1` then `Account 2`) to prevent deadlock.
* **Termination:** The program stops when either:
    1.  A thread encounters **insufficient funds** in its source account.
    2.  The **global deadline** (set by the user) expires.
* **Output:** Logs successful transfers, prints final balances, transfer counts, and the termination reason.

## Synchronization Explanation

The most critical part of this concurrent system is the transfer function, which needs access to **two** shared resources (`source` account and `destination` account).

1.  **Race Condition Prevention:** A mutex is assigned to each account. All reads/writes to an account's balance or transfer count occur only while that account's mutex is held.
2.  **Deadlock Prevention:** Both threads need to lock two mutexes simultaneously to perform a transfer. If Thread 1 locks M1 then M2, and Thread 2 locks M2 then M1, a deadlock can occur.
    * **Solution:** The `lock_pair` utility function ensures **all threads always attempt to lock Account 1's mutex first, followed by Account 2's mutex.** This consistent ordering eliminates the circular dependency required for a deadlock.

## Compilation and Execution

### Prerequisites

You need a C compiler (like GCC) and the POSIX threads library.

### Building the Project

Use the provided `Makefile` to compile the application:

```bash
make bank_c

### Execution Command
./bank_c