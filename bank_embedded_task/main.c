#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include <string.h>

// --- Global Data Structures and Synchronization Primitives ---

/**
 * @struct Account
 * @brief Structure to hold data for a single bank account.
 * * All fields except 'id' are shared resources accessed concurrently  
 * */    
typedef struct {
    double balance;                   // Current account balance
    double transfer_amount;           // Amount thread will try to transfer
    long successful_transfers;        // Counter for successful transfers
    pthread_mutex_t mutex;            // Mutex for protecting this account's data
    int id;                           // Account identifier (1 or 2)
} Account;

// Global flag to signal termination to all threads.
// Must be volatile as it can be changed by the main thread and read by worker threads.
volatile int is_terminated = 0;

// Global buffer to store the reason for program termination.
char termination_reason[100] = "";

// Global shared accounts (defined here to be accessible by all functions)
Account account1;
Account account2;

// Global input parameters collected from the user
long transfer_period_ms;  // Period between transfers in milliseconds
long deadline_sec;        // Global program deadline in seconds

// --- Utility Functions for Deadlock Prevention ---

/**
 * @brief Locks two mutexes in a guaranteed, consistent order to prevent deadlock.
 * * We enforce the order: Account 1's mutex -> Account 2's mutex.
 * This is crucial because both threads attempt to lock two resources.
 */
void lock_pair(pthread_mutex_t *m1, pthread_mutex_t *m2) {
    // Check if the first mutex passed is Account 1's mutex
    if (m1 == &account1.mutex) {
        pthread_mutex_lock(m1);
        pthread_mutex_lock(m2);
    } else { 
        // If m1 is Account 2's mutex, lock m2 (Account 1's mutex) first
        pthread_mutex_lock(m2);
        pthread_mutex_lock(m1);
    }
}

/**
 * @brief Unlocks two mutexes. Order doesn't matter for unlocking.
 */
void unlock_pair(pthread_mutex_t *m1, pthread_mutex_t *m2) {
    pthread_mutex_unlock(m1);
    pthread_mutex_unlock(m2);
}

// --- Core Transfer Logic ---

/**
 * @brief Attempts to transfer funds between a source and destination account.
 * * This function handles synchronization, fund checking, transfer, and logging.
 * * @param source Pointer to the source account.
 * @param dest Pointer to the destination account.
 * @return 0 on successful transfer, -1 on failure (insufficient funds or termination).
 */
int transfer_funds(Account *source, Account *dest) {
    // Acquire Locks (Deadlock-free)
    // We lock the two mutexes in a consistent, deadlock-free order.
    lock_pair(&source->mutex, &dest->mutex);

    // Check for External Termination
    if (is_terminated) { 
        unlock_pair(&source->mutex, &dest->mutex);
        return -1; 
    }
    
    // Check for Insufficient Funds
    if (source->balance < source->transfer_amount) {
        // Termination condition 1: Insufficient funds
        // This thread is responsible for setting the termination state and reason.
        snprintf(termination_reason, 100, "Insufficient funds in Account %d", source->id);
        is_terminated = 1; 
        
        unlock_pair(&source->mutex, &dest->mutex);
        return -1; // Indicate failure
    }

    // Perform Transfer
    source->balance -= source->transfer_amount;
    dest->balance += source->transfer_amount;
    source->successful_transfers++;

    // Log Successful Transfer
    printf("SUCCESS: %d -> %d | Amt: %.2f | New Balance %d: %.2f\n",
           source->id, dest->id, source->transfer_amount, source->id, source->balance);

    // Release Locks
    unlock_pair(&source->mutex, &dest->mutex);
    return 0; // Indicate success
}

// --- Thread Functions ---

/**
 * @brief Thread routine for transferring funds from Account 1 to Account 2.
 * The first transfer occurs immediately
 */
void* thread_transfer_1_to_2(void *arg) {
    struct timespec ts;
    // Convert transfer period from milliseconds to nanoseconds for nanosleep
    ts.tv_sec = 0;
    ts.tv_nsec = transfer_period_ms * 1000000L; 

    while (!is_terminated) {
        // Attempt the transfer
        if (transfer_funds(&account1, &account2) != 0) {
            break; // Exit loop if transfer failed or program terminated
        }
        
        // Wait for the specified transfer period
        nanosleep(&ts, NULL);
    }
    return NULL;
}

/**
 * @brief Thread routine for transferring funds from Account 2 to Account 1.
 * The first transfer occurs immediately
 */
void* thread_transfer_2_to_1(void *arg) {
    struct timespec ts;
    // Convert transfer period from milliseconds to nanoseconds for nanosleep
    ts.tv_sec = 0;
    ts.tv_nsec = transfer_period_ms * 1000000L; 

    while (!is_terminated) {
        // Attempt the transfer
        if (transfer_funds(&account2, &account1) != 0) {
            break; // Exit loop if transfer failed or program terminated
        }
        
        // Wait for the specified transfer period
        nanosleep(&ts, NULL);
    }
    return NULL;
}

// --- Main Program Setup ---

/**
 * @brief Collects all necessary initial parameters from the user
 */
void get_user_input() {
    printf("--- Bank Account Conundrum Setup ---\n");
    printf("Account 1 Initial Balance: ");
    scanf("%lf", &account1.balance);
    printf("Account 1 Transfer Amount: ");
    scanf("%lf", &account1.transfer_amount);

    printf("Account 2 Initial Balance: ");
    scanf("%lf", &account2.balance);
    printf("Account 2 Transfer Amount: ");
    scanf("%lf", &account2.transfer_amount);

    printf("Transfer Period (ms): ");
    scanf("%ld", &transfer_period_ms);

    printf("Global Deadline (seconds): ");
    scanf("%ld", &deadline_sec);
    printf("------------------------------------\n");
}

int main() {
    pthread_t t1, t2;

    // Initialize Accounts and Mutexes
    account1.id = 1;
    account1.successful_transfers = 0;
    pthread_mutex_init(&account1.mutex, NULL); // Initialize Account 1 mutex

    account2.id = 2;
    account2.successful_transfers = 0;
    pthread_mutex_init(&account2.mutex, NULL); // Initialize Account 2 mutex
    
    // Collect User Input
    get_user_input();

    // Start Worker Threads
    printf("Starting concurrent transfers for a maximum of %ld seconds...\n", deadline_sec);
    pthread_create(&t1, NULL, thread_transfer_1_to_2, NULL);
    pthread_create(&t2, NULL, thread_transfer_2_to_1, NULL);

    // Deadline Timer (Termination Condition 2)
    // The main thread sleeps for the duration of the deadline.
    sleep(deadline_sec);
    
    // If the timer expires and no insufficient funds error has occurred yet:
    if (!is_terminated) {
        is_terminated = 1; // Signal all threads to stop
        strcpy(termination_reason, "Global deadline expired");
    }

    // Join Threads
    // Wait for the worker threads to recognize 'is_terminated' and finish their loops.
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    
    // Final Output
    printf("\n--- Final Results ---\n");
    printf("Final Balance Account 1: %.2f\n", account1.balance);
    printf("Final Balance Account 2: %.2f\n", account2.balance);
    printf("Successful Transfers (1->2): %ld\n", account1.successful_transfers); 
    printf("Successful Transfers (2->1): %ld\n", account2.successful_transfers);
    printf("Termination Reason: %s\n", termination_reason);
    printf("---------------------\n");

    // Cleanup
    pthread_mutex_destroy(&account1.mutex);
    pthread_mutex_destroy(&account2.mutex);

    return 0;
}