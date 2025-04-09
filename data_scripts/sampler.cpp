#include <random>
#include <string>
#include <fstream>
#include <iostream>
#include <functional>
#include <iomanip>

int main(int argc, char *argv[])
{
    if (argc < 6)
    {
        std::cerr << "Usage:\n"
                  << "  " << argv[0] << " uniform    a b       n out.csv\n"
                  << "  " << argv[0] << " normal     mu sigma n out.csv\n";
        return 1;
    }

    std::string dist_name = argv[1];
    size_t argi = 2;

    // Parse parameters
    double p1 = std::stod(argv[argi++]);
    double p2 = 0;
    if (dist_name == "uniform" ||
        dist_name == "normal" ||
        dist_name == "gamma")
    {
        if (argi + 1 >= (size_t)argc)
        {
            std::cerr << "Error: not enough parameters for " << dist_name << "\n";
            return 1;
        }
        p2 = std::stod(argv[argi++]);
    }

    size_t n = std::stoull(argv[argi++]);
    std::string out_csv = argv[argi++];

    // RNG
    std::random_device rd;
    std::mt19937_64 gen(rd());

    // Set up the chosen distribution
    // We'll use a lambda that returns a double when called
    std::function<double()> sampler;

    if (dist_name == "uniform")
    {
        double a = p1, b = p2;
        std::uniform_real_distribution<double> dist(a, b);
        sampler = [&]
        { return dist(gen); };
    }
    else if (dist_name == "normal")
    {
        double mu = p1, sigma = p2;
        std::normal_distribution<double> dist(mu, sigma);
        sampler = [&]
        { return dist(gen); };
    }
    else
    {
        std::cerr << "Error: unknown distribution '" << dist_name << "'\n";
        return 1;
    }

    // Open output CSV
    std::ofstream fout(out_csv);
    if (!fout)
    {
        std::cerr << "Error: cannot open " << out_csv << " for writing\n";
        return 1;
    }
    fout << "sample\n";
    fout << std::fixed << std::setprecision(6);

    // Generate and write
    for (size_t i = 0; i < n; ++i)
    {
        double x = sampler();
        // If you need to restrict to [0,63], you could either:
        //  - clamp: x = std::min(63.0, std::max(0.0, x));
        //  - or reject & resample:
        //      do { x = sampler(); } while (x < 0.0 || x > 63.0);
        fout << x << "\n";
    }

    std::cout << "Wrote " << n << " samples from '" << dist_name
              << "' to " << out_csv << "\n";
    return 0;
}