#ifndef CYCLONE_CONSTANTS
#define CYCLONE_CONSTANTS

// Constants file
#define BYTE_SIZE 8
#define CLEAR_1ST_BIT_MASK 254
#define LINESIZE 1000
#define RESULTS_TAG_LENGTH 12
#define DEFAULT_MARKBITS 8
#define START_OF_ARGS 2
#define MIN_STATES 2
#define MAX_MARK_BITS 16
#define BYTES_PER_INT 4
#define BYTES_PER_ADDR sizeof(void*)

#define CHECKED_KEY 209
#define CHECKED_LOCK_KEY 309
#define ASSOC_KEY 409
#define ASSOC_LOCK_KEY 509
#define TRAJ_COUNT_KEY 609
#define TRAJ_LOCK_KEY 709
#define TRAJ_END_KEY 809
// #define EDGE_ARRAY_KEY 909
#define CHILD_WAIT_KEY 1009
#define CHILD_WAIT_LOCK_KEY 1109

#define LOCK_ASSOCIATIONS while(!(__sync_bool_compare_and_swap(associationsLock, 0, 1))){}
#define UNLOCK_ASSOCIATIONS while(!(__sync_bool_compare_and_swap(associationsLock, 1, 0))){}

#define LOCK_TRAJ while(!(__sync_bool_compare_and_swap(sharedTrajLock, 0, 1))){}
#define UNLOCK_TRAJ while(!(__sync_bool_compare_and_swap(sharedTrajLock, 1, 0))){}

#define LOCK_CHILD_WAIT while(!(__sync_bool_compare_and_swap(childWaitLock, 0, 1))){}
#define UNLOCK_CHILD_WAIT while(!(__sync_bool_compare_and_swap(childWaitLock, 1, 0))){}

#define LOCK_CHECKED while(!(__sync_bool_compare_and_swap((int *)(checkedLock + curState), 0, 1))){}
#define UNLOCK_CHECKED while(!(__sync_bool_compare_and_swap((int *)(checkedLock + curState), 1, 0))){}

#define uchar unsigned char
#define unlong unsigned long int

#endif // CYCLONE_CONSTANTS