# Monte Carlo Options Pricer [v0.1.0]

> Studying quantitative finance fundamentals (options, bonds, discount curves) 
> through hands-on implementation of pricing models and risk calculations

## Version Notes


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

- [ ] Add parallelization (std::thread) for faster simulation
- [ ] Add path-dependent options
    - [ ] Implement adjustable time-step grid per path
- [ ] Allow multiple options to be priced in one run
    - [ ] Output results to CSV/JSON for analysis
- [ ] Integrate unit tests and CI for validation
- [ ] Add CMake build system with flags for release/debug
