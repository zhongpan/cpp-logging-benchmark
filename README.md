## 文章

[《几种C++日志框架基准测试和源码分析》](http://zhongpan.tech/2019/07/04/cpp-logging-framework-benchmark/)



## 编译

1. 安装conan&CMake

2. 安装google benchmark

   ```shell
   git clone https:/github.com/google/benchmark
   cd benchmark
   git checkout v1.5.0
   conan create . benchmark/1.5.0@google/stable
   ```

3. 编译

   ```shell
   conan install -if build .
   cd build
   cmake .. -G "Visual Studio 15 2017 Win64"
   cmake --build .
   ```

   

## 运行

* 执行：

```shell
cpp_logging_benchmark.exe --benchmark_counters_tabular=true --benchmark_out_format=console --benchmark_out=result.txt
```

* 结果：

```
Running cpp_logging_benchmark.exe
Run on (4 X 2712 MHz CPU s)
CPU Caches:
  L1 Data 32K (x2)
  L1 Instruction 32K (x2)
  L2 Unified 262K (x2)
  L3 Unified 3145K (x1)
***WARNING*** Library was built as DEBUG. Timings may be affected.
------------------------------------------------------------------------------------
Benchmark                          Time             CPU   Iterations UserCounters...
------------------------------------------------------------------------------------
l4cp_a_c/32/manual_time      6225169 ns        0.000 ns           87 flush=0.536384 items_per_second=160.638/s
l4cp_a_c/512/manual_time    42497618 ns        0.000 ns           17 flush=0.720527 items_per_second=23.5307/s
l4cp_a_f/32/manual_time        63311 ns        47156 ns         9609 flush=0.0637038 items_per_second=15.7951k/s
l4cp_a_f/512/manual_time      179771 ns        52541 ns         3866 flush=0.231186 items_per_second=5.56262k/s
l4cp_s_c/32/manual_time      6647907 ns      2297794 ns          102 flush=58.3u items_per_second=150.423/s
l4cp_s_c/512/manual_time    38861011 ns     13020833 ns           18 flush=64.5u items_per_second=25.7327/s
l4cp_s_f/32/manual_time        80825 ns        78740 ns         8136 flush=3.373m items_per_second=12.3724k/s
l4cp_s_f/512/manual_time      205525 ns       205773 ns         3417 flush=4.1874m items_per_second=4.86558k/s
spd_a_c/32/manual_time        101679 ns         7571 ns        28892 flush=218.2u items_per_second=9.83492k/s
spd_a_c/512/manual_time        72659 ns         6250 ns        10000 flush=290.8u items_per_second=13.763k/s
spd_a_f/32/manual_time          5312 ns         4844 ns       100000 flush=2.1u items_per_second=188.257k/s
spd_a_f/512/manual_time         8342 ns         5156 ns       100000 flush=2.6u items_per_second=119.876k/s
spd_s_c/32/manual_time        135586 ns        48485 ns         4834 flush=1.2u items_per_second=7.37538k/s
spd_s_c/512/manual_time       382239 ns       126689 ns         1850 flush=1000n items_per_second=2.61617k/s
spd_s_f/32/manual_time          4323 ns         4469 ns       160834 flush=7.1u items_per_second=231.317k/s
spd_s_f/512/manual_time         6209 ns         6199 ns       115951 flush=6u items_per_second=161.053k/s
boost_a_c/32/manual_time      239764 ns        47985 ns         2605 flush=0.493044 items_per_second=4.17076k/s
boost_a_c/512/manual_time     495485 ns        47929 ns         1304 flush=0.580076 items_per_second=2.01823k/s
boost_a_f/32/manual_time       34062 ns        34465 ns        21308 flush=150.8u items_per_second=29.3582k/s
boost_a_f/512/manual_time      37948 ns        35325 ns        18135 flush=0.0433181 items_per_second=26.3519k/s
boost_s_c/32/manual_time      274890 ns       175915 ns         2487 flush=3.4u items_per_second=3.63782k/s
boost_s_c/512/manual_time     538324 ns       234786 ns         1331 flush=3.1u items_per_second=1.85762k/s
boost_s_f/32/manual_time       63006 ns        62591 ns        10984 flush=19.5u items_per_second=15.8716k/s
boost_s_f/512/manual_time      68686 ns        68086 ns        10327 flush=15.2u items_per_second=14.559k/s

```

* 说明：

1. 32/512表示消息字节数；
2. a/c表示异步或同步；
3. c/f表示控制台或文件。