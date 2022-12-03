#include <benchmark/benchmark.h>
#include <string.h>
#include <set>

// ===========================================================================================
static void BM_StringCreation(benchmark::State& state) {
  for (auto _ : state)
    std::string empty_string;
}
// Register the function as a benchmark
BENCHMARK(BM_StringCreation);

// ===========================================================================================
static void BM_StringCopy(benchmark::State& state) {
  std::string x = "hello";
  for (auto _ : state)
    std::string copy(x);
}
BENCHMARK(BM_StringCopy);

// ===========================================================================================
static void BM_memcpy(benchmark::State& state) {
  char* src = new char[state.range(0)];
  char* dst = new char[state.range(0)];
  memset(src, 'x', state.range(0));
  for (auto _ : state)
    memcpy(dst, src, state.range(0));
  state.SetBytesProcessed(int64_t(state.iterations()) *
                          int64_t(state.range(0)));
  delete[] src;
  delete[] dst;
}

// The following invocation will pick a few appropriate arguments in the specified range and 
// will generate a benchmark for each such argument.
BENCHMARK(BM_memcpy)->Range(8, 8<<10);

// By default the arguments in the range are generated in multiples of eight and the command 
// above selects [ 8, 64, 512, 4k, 8k ]. In the following code the range multiplier is 
// changed to multiples of two.
BENCHMARK(BM_memcpy)->RangeMultiplier(2)->Range(8, 8<<10);

// ===========================================================================================
static void BM_DenseRange(benchmark::State& state) {
  for(auto _ : state) {
    std::vector<int> v(state.range(0), state.range(0));
    benchmark::DoNotOptimize(v.data());
    benchmark::ClobberMemory();
  }
}

// Now arguments generated are [ 0, 128, 256, 384, 512, 640, 768, 896, 1024 ].
BENCHMARK(BM_DenseRange)->DenseRange(0, 1024, 128);

// ===========================================================================================
std::set<int> ConstructRandomSet(int size) {
  std::set<int> s;
  for (int i = 0; i < size; ++i) s.insert(s.end(), i);
  return s;
}

static void BM_SetInsert(benchmark::State& state) {
  std::set<int> data;
  for (auto _ : state) {
    state.PauseTiming();
    data = ConstructRandomSet(state.range(0));
    state.ResumeTiming();
    for (int j = 0; j < state.range(1); ++j)
      data.insert(rand());
  }
}

// You might have a benchmark that depends on two or more inputs. For example, the following 
// code defines a family of benchmarks for measuring the speed of set insertion
BENCHMARK(BM_SetInsert)->Ranges({{1<<10, 2<<10}, {128, 256}});

// Use a custom function to generate inputs
static void CustomArguments(benchmark::internal::Benchmark* b) {
  for (int i = 0; i <= 5; ++i)
    for (int j = 32; j <= 512*512; j *= 8)
      b->Args({i, j});
}

// Specify a custom name for the benchmark
BENCHMARK(BM_SetInsert)->Name("CustomArgBM")->Apply(CustomArguments);

// ===========================================================================================

BENCHMARK_MAIN();

