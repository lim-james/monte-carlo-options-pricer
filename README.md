# Monte Carlo Options Pricer [v0.4.0]

> Studying quantitative finance fundamentals (options, bonds, discount curves) 
> through hands-on implementation of pricing models and risk calculations

## Version Notes

### v0.4.0

- Allow multiple options to be priced in one run
    - Output results to CSV for analysis
- Add put options

**OBSERVATION**
```c++
double payoff(const eu_option& option) {
    // ...
    double raw_payoff = option.type == OptionType::Call 
        ? std::max(future_price - option.strike, 0.0)
        : std::max(option.strike - future_price, 0.0);
    // ...
}
```
There's a conditional here in my hot loop, so of course I'm curious to take the
chance to profile predictive branching. Time to spin up my Pi.

### v0.3.0

```
1 options // 100000000 samples
MC payoff = 10.45 [8352ms]
BS payoff = 10.45 [0ms]
|MC - BS| = -0.00
```
> I tested the following scenario and this disparity in time is unacceptable
> Like sure, I can't be comparing a math eq to a simulation, but let's try to
> cut it down a little

- Add parallelization (std::thread) for faster simulation

**OBSERVATION**
```
1 options // 100000000 samples 
threads  time (ms) 
      1       8569   starting
      2       7005   improvement
      3       6281   improvement
      4       6568   slight tank in performance
      6       7187   drop in performance (still better than 1) 
      8       7589   drop again
     10      10262   oof this is bad
     12      10516   ...
```
> There was an improvement from 1 -> 2 threads, but upon maximising all 12 cores
> things started to breakdown. Time to investigate.

My assumption is this segment is the cause of this slow down. 
```c++
double grow(const eu_option& option) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::normal_distribution<> dist(0.0, 1.0);
    // ...
}
```
`grow` is called by every branch of my monte carlo simulation, which poses no
issue when ran in sequential, however now with multiple branches hitting it at
the same time then we have got an issue. With `rd`, `gen` and `dist` being 
`static` it is process-local and is shared across all `n` threads. Causing a 
contention everytime random is called. Cache line bounces between the threads
and boom we have **false sharing**. Let's fix it.

**FIX 1: use `thread_local` over `static`**
```
1 options // 100000000 samples 
threads  time (ms) 
      1       9039   starting
      2       4480   DAMN double threads, double perf
      3       3164   TRIPLE
      4       2362   QUAD
      6       1584   PROPORTION DROPS
      8       1218   this is just great
     10       1109   alright im quite satisfied with this
     12       1086   I should be expecting < 1000ms in an ideal world
```
With `thread_local` each thread manages their respective randomisations. No more
contention.

**FIX 2: pad results with `alignas(64)`**
```
1 options // 100000000 samples 
threads  time (ms)  aligned
      1       9039     8908   
      2       4480     4588   
      3       3164     3158   
      4       2362     2350   
      6       1584     1585   
      8       1218     1202   
     10       1109     1119   
     12       1086     1031   
```
Honestly the difference is minimal because of the number of contentions being at
most 12 (my max cores), but was fun to give it a try.

**JUST FOR FUN**
```
1 options // 1000000000 samples // 12 threads
Non-aligned  13640ms
Aligned      11718ms
```
I wanted to see the latency difference as a result of false sharing so I adapted
the code temporarily to write directly to the memory on every iteration.
```c++
void monte_carlo_call_pricing_batch(
    // ...
    double* payoff_mem
) {
    for (size_t i = 0; i < batch_size; ++i) {
        // ...
        // directly writing to shared resource every on every iteration
        *payoff_mem += payoff * batch_fraction; 
    }
}
```


### v0.2.0

- **Implement Black-Scholes analytical formula for validation**
    -  Display both Monte Carlo and analytical results with error difference
    -  Validate correctness: |MC − BS| < 0.1 for large N
- Add CLI/config file parameter input
- Include runtime statistics (execution time, sample count)
- Modularize project structure


### v0.1.0

Objective: Get C++ project going with a basic and stripped down version of a
european call-put option simulation.

* Focused on European Call-Put options
* Assuming risk-free 
* Parameters considered (simplified version)
  * Spot Price ($S_0$)
  * Strike Price (K) 
  * Time to Expiry (T)
  * Risk free rate (r)
  * Volatility (σ)  
* Monte Carlo sample size adjustable through config file


## Project Roadmap

I have documented the vision for this project and the different areas of
technicalities I would be exploring along the way.

- [v0.5.0] Greeks
  - How to calculate them w/ Black-Scholes formula + w/ Monte-Carlo using finite
    difference methods
- [v0.6.0] Variance Reduction 
- [v0.7.0] Performance + Instrumentation
  - Optimising and playing with different performance tools
- [v0.8.0] Modularity
  - Make it ready to be shipped as a package


