#include <iostream>
#include <armadillo>

using namespace std;
using namespace arma;

int main()
{
    arma::arma_rng::set_seed_random();
    mat A(4, 5, fill::randu);
    mat B(4, 5, fill::randu);

    cout << A * B.t() << endl;

    return 0;
}

