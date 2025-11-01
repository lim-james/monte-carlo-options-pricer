# Monte Carlo Options Pricer [v0.1.0]

> Studying quantitative finance fundamentals (options, bonds, discount curves) 
> through hands-on implementation of pricing models and risk calculations

## Version Notes

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

**Observation**
```
1 options // 100000000 samples // 12 threads
MC payoff = 10.46 [10516ms]

1 options // 100000000 samples // 1 threads
MC payoff = 10.45 [8569ms]    <<< starting

1 options // 100000000 samples // 2 threads
MC payoff = 10.45 [7005ms]    <<< improvement

[1 options // 100000000 samples // 3 threads
MC payoff = 10.46 [6281ms]    <<< improvement

1 options // 100000000 samples // 4 threads
MC payoff = 10.45 [6568ms]    <<< slight tank in performance

1 options // 100000000 samples // 6 threads
MC payoff = 10.44 [7187ms]    <<< drop in performance (still better than 1) 

1 options // 100000000 samples // 8 threads
MC payoff = 10.43 [7589ms]    <<< drop again

1 options // 100000000 samples // 10 threads
MC payoff = 10.46 [10262ms]   <<< oof this is bad

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

- [ ] Add path-dependent options
    - [ ] Implement adjustable time-step grid per path
- [ ] Allow multiple options to be priced in one run
    - [ ] Output results to CSV/JSON for analysis
- [ ] Integrate unit tests and CI for validation
- [ ] Add CMake build system with flags for release/debug
