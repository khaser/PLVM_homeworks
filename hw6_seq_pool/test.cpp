#include <iostream>
#include <iomanip>
#include <stdint.h>
#include <stdlib.h>
#include <sys/resource.h>
#include <sys/time.h>

#include "seq_pool.h"

using namespace std;

static void get_usage(struct rusage& usage) {
  if (getrusage(RUSAGE_SELF, &usage)) {
    perror("Cannot get usage");
    exit(EXIT_SUCCESS);
  }
}

struct Node {
  Node* next;
  unsigned node_id;
};

static inline Node* create_list(unsigned n) {
  Node* list = nullptr;
  for (unsigned i = 0; i < n; i++)
    list = new Node({list, i});
  return list;
}

static inline void delete_list(Node* list) {
  while (list) {
    Node* node = list;
    list = list->next;
    delete node;
  }
}

static inline Node* create_list_with_pool(unsigned n, SeqPool& pool) {
  Node* list = nullptr;
  for (unsigned i = 0; i < n; i++) {
    list = (Node*) pool.alloc(sizeof(Node));
    *list = {list, i};
  }
  return list;
}

static inline void test(unsigned n) {
  struct rusage start, finish;
  get_usage(start);
  // delete_list(create_list(n));
  {
    SeqPool pool(n * sizeof(Node), sizeof(Node));
    create_list_with_pool(n, pool);

    // // Case to check true-positive notification
    // SeqPool pool((n - 1000) * sizeof(Node), sizeof(Node));
    // create_list_with_pool(n, pool);

    // // Case to check false-positive notification
    // char* inval = (char*) 105000;
    // *inval = 0;
  }
  get_usage(finish);

  struct timeval diff;
  timersub(&finish.ru_utime, &start.ru_utime, &diff);
  uint64_t time_used = diff.tv_sec * 1000000 + diff.tv_usec;
  cout << "Time used: " << time_used << " usec\n";

  uint64_t mem_used = (finish.ru_maxrss - start.ru_maxrss) * 4096;
  cout << "Memory used: " << mem_used << " bytes\n";

  auto mem_required = n * sizeof(Node);
  auto overhead = (mem_used - mem_required) * double(100) / mem_used;
  cout << "Overhead: " << std::fixed << std::setw(4) << std::setprecision(1)
       << overhead << "%\n";
}

int main(const int argc, const char* argv[]) {
  test(10000000);
  return EXIT_SUCCESS;
}
// vim: ts=2:sw=2
