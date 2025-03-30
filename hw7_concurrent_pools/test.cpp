#include <iostream>
#include <iomanip>
#include <stdint.h>
#include <stdlib.h>
#include <sys/resource.h>
#include <sys/time.h>

#include <functional>
#include <thread>

#include "seq_pool.h"
#include "lock_free_seq_pool.h"

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

static inline Node* create_list(unsigned n, std::function<Node*()> alloc) {
  Node* list = nullptr;
  for (unsigned i = 0; i < n; i++) {
    Node* nlist = alloc();
    *nlist = {list, i};
    list = nlist;
  }
  return list;
}

// #define NJOBS 4
#define NJOBS 16
// #define VARIANT mutexPool // should be defined via compile flags
#define __BENCH_FUNC(x) x##Bench
#define _BENCH_FUNC(x) __BENCH_FUNC(x)
#define BENCH_FUNC _BENCH_FUNC(VARIANT)

static void launch_theads(std::function<void()> job) {
  std::jthread jobs[NJOBS];
  for (int i = 0; i < NJOBS; ++i) {
      jobs[i] = std::jthread(job);
  }
}

static inline void mallocBench(int n) {
  auto job = [&n] () {
    Node* list = create_list(n, [] () { return new Node; });
    while (list) {
      Node* node = list;
      list = list->next;
      delete node;
    }
  };
  launch_theads(job);
}

static inline void mutexPoolBench(int n) {
  SeqPool pool(n * NJOBS * sizeof(Node), sizeof(Node));
  std::mutex mut;
  auto job = [&n, &pool, &mut] () {
    Node* list = create_list(n, [&pool, &mut] () {
        lock_guard lock(mut);
        return (Node*) pool.alloc(sizeof(Node));
    });
  };
  launch_theads(job);
}

static inline void localPoolsBench(int n) {
  auto job = [&n] () {
    SeqPool pool(n * sizeof(Node), sizeof(Node));
    Node* list = create_list(n, [&pool] () { return (Node*) pool.alloc(sizeof(Node)); });
  };

  launch_theads(job);
}


static inline void lockFreePoolBench(int n) {
  LockFreeSeqPool pool(n * NJOBS * sizeof(Node), sizeof(Node));

  auto job = [&n, &pool] () {
    Node* list = create_list(n, [&pool] () { return (Node*) pool.alloc(sizeof(Node)); });
  };

  launch_theads(job);
}

static inline void test(unsigned n) {
  struct rusage start, finish;

  get_usage(start);

  BENCH_FUNC(n);

  get_usage(finish);

  struct timeval diff;
  timersub(&finish.ru_utime, &start.ru_utime, &diff);
  uint64_t time_used = diff.tv_sec * 1000000 + diff.tv_usec;
  cout << "Time used: " << time_used << " usec\n";

  uint64_t mem_used = (finish.ru_maxrss - start.ru_maxrss) * 4096;
  cout << "Memory used: " << mem_used << " bytes\n";

  auto mem_required = n * sizeof(Node) * NJOBS;
  auto overhead = (mem_used - mem_required) * double(100) / mem_used;
  cout << "Overhead: " << std::fixed << std::setw(4) << std::setprecision(1)
       << overhead << "%\n";
}

int main(const int argc, const char* argv[]) {
  test(10000000);
  return EXIT_SUCCESS;
}
// vim: ts=2:sw=2
