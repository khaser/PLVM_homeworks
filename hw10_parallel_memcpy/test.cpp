#include <cassert>
#include <chrono>
#include <condition_variable>
#include <cstring>
#include <format>
#include <iostream>
#include <ranges>
#include <thread>
#include <vector>

class MemcpyService {
  std::vector<std::jthread> workers;
  const size_t cnt_workers;

  std::mutex mut;
  std::condition_variable cv;
  std::atomic_size_t in_progress;

  struct Task {
    void* dest;
    const void* src;
    size_t size;
  };

  Task job;


public:
  MemcpyService(size_t cnt_workers) : cnt_workers(cnt_workers) {
    in_progress.store(0);
    for (auto i : std::ranges::iota_view(0ull, cnt_workers)) {
      workers.emplace_back(worker_job, i, this);
    }
  }

  ~MemcpyService() {
    {
      std::unique_lock lock(mut);
      for (auto& th : workers) {
        th.request_stop();
      }
    }
    in_progress.store(cnt_workers, std::memory_order_release);
    cv.notify_all();
  }

private:
  static void worker_job(std::stop_token stoken, int worker_id, MemcpyService* service) {
    while (!stoken.stop_requested()) {
      {
        std::unique_lock lock(service->mut);
        service->cv.wait(lock, [&] { return service->in_progress.load(std::memory_order_acquire) != 0; });
        if (stoken.stop_requested()) {
          break;
        }
      }
      auto task = task_for_worker(worker_id, service);
      memcpy(task.dest, task.src, task.size);
      {
        std::unique_lock lock(service->mut);
        if (service->in_progress.fetch_sub(1, std::memory_order_release) == 1) {
          service->cv.notify_all();
        } else {
          service->cv.wait(lock, [&] { return service->in_progress.load(std::memory_order_acquire) == 0; });
        }
      }
    }
  }

  static Task task_for_worker(int worker_id, MemcpyService* service) {
    auto job = service->job;
    // each worker copy (size / cnt_workers) bytes
    // main thread copy remaining part
    size_t to_copy = job.size / service->cnt_workers;
    size_t offset = to_copy * worker_id;
    return {
      (char*) job.dest + offset,
      (char*) job.src + offset,
      to_copy
    };
  }

public:
  void* parallel_memcpy(void* dst, const void* src, size_t size) {
    if (cnt_workers == 0) {
      return memcpy((char*) dst, (char*) src, size);
    }
    std::unique_lock lock(mut);
    job.dest = dst;
    job.src = src;
    job.size = size;
    in_progress.store(cnt_workers, std::memory_order_release);
    cv.notify_all();

    size_t to_copy = size % (size / cnt_workers);
    memcpy((char*) dst + size - to_copy, (char*) src + size - to_copy, to_copy);
    cv.wait(lock, [&] { return in_progress.load(std::memory_order_acquire) == 0; });

    return dst;
  }

};

uint8_t* generate_data(size_t data_sz) {
  uint8_t* data = new uint8_t[data_sz];
  data[0] = 163;
  for (size_t i = 1; i < data_sz; ++i) {
    data[i] = data[i - 1] * 194 + 17;
  }
  return data;
}

int main() {
  constexpr size_t DATA_SZ = 256 << 20;
  uint8_t* src = generate_data(DATA_SZ);

  std::cout << "Benchmarking parallel_memcpy...\n";
  for (auto workers : std::ranges::iota_view(0, 9)) {
    uint8_t* dst = new uint8_t[DATA_SZ];
    {
      MemcpyService service(workers);
      using clock = std::chrono::high_resolution_clock;
      auto start = clock::now();

      service.parallel_memcpy(dst, src, DATA_SZ);

      std::chrono::duration<double> time(clock::now() - start);
      std::cout << "Workers: " << workers << "\ttime: " << time << '\n';
      assert(std::memcmp(src, dst, DATA_SZ) == 0);
    }
    delete[] dst;
  }

  delete[] src;

  return 0;
}
// vim: ts=2:sw=2
