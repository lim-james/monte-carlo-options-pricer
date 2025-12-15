#include "pricer/concurrency/work_queue.h"
#include "pricer/model/european.h"

#include <random>
#include <thread>

namespace pricer {

class MCWorker {
private:

    model::EuropeanOption option_;
    std::mt19937 gen_;
    double* payoff_output_;

    std::jthread thread_;

public:

    MCWorker(
        const model::EuropeanOption& option,
        unsigned int random_seed,
        unsigned int thread_id,
        double* payoff_output,
        WorkQueue& queue
    );

private:

    inline unsigned int generate_thread_seed(unsigned int seed, unsigned int thread_id);
    inline double apply_discount(double future_price, double market_rate, double time_to_expiry);

    double calculate_future_price();
    double calculate_discounted_payoff();

    void poll(WorkQueue& queue);

};

}
