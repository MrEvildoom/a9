#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/errno.h>
#include <assert.h>
#include "uthread.h"
#include "queue.h"
#include "disk.h"

queue_t pending_read_queue;
int areyoudoneyet;

void interrupt_service_routine() {
  // TODO
  // printf("\n made it to ISR");
  areyoudoneyet = 0;
}

// initiate next read once u have result from previous read


void handleOtherReads (void* resultv, void* countv) {
  // TODO
  if (*(int*) countv == 0) {
    printf("\n%d", *(int*) resultv);
    free(resultv);
    free(countv);
    exit (EXIT_SUCCESS);
  } else {
    areyoudoneyet = 1;
    disk_schedule_read(resultv, *(int*) resultv);
    while (areyoudoneyet == 1) {
      // do nothing... just wait until ISR sets it to 0
    }
    *(int*) countv = *(int*) countv - 1;
    handleOtherReads(resultv, countv);
  }
}
  

void handleFirstRead (void* resultv, void* countv) { 
  // both resultv and countv = address of starting block number
  areyoudoneyet = 1;
  int* counter = malloc(sizeof(int)); // the number of reads we have to do
  int* nextadd = malloc(sizeof(int)); // the address of next block to read
  disk_schedule_read(nextadd, *(int*) resultv);
  while (areyoudoneyet == 1) {
    // do nothing... just wait until ISR sets it to 0... then you know counter has been updated.
    // printf("\nvalue of areyoudoneyet: %d", areyoudoneyet);
    // printf("\nvalue of next add after ISR handlefirstread: %d", *(int*) nextadd);
  }
  // printf("\nvalue of areyoudoneyet: %d", areyoudoneyet);
  // printf("\nvalue of next add after ISR handlefirstread: %d", *(int*) nextadd);
  *counter = *nextadd;
  handleOtherReads(nextadd, counter);
}

int main (int argc, char** argv) {
  // Command Line Arguments
  static char* usage = "usage: treasureHunt starting_block_number";
  int starting_block_number;
  char *endptr;
  if (argc == 2)
    starting_block_number = strtol (argv [1], &endptr, 10);
  if (argc != 2 || *endptr != 0) {
    printf ("argument error - %s \n", usage);
    return EXIT_FAILURE;
  }

  // Initialize
  uthread_init (1);
  disk_start (interrupt_service_routine);
  pending_read_queue = queue_create();


  // Start the Hunt
  // TODO
  handleFirstRead(&starting_block_number, &starting_block_number);
  while (1); // inifinite loop so that main doesn't return before hunt completes
}
