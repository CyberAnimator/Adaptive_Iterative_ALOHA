#include <iostream>
#include <random>
#include <list>
#include <iomanip>
#include <cstdlib>
#include <ctime>
#include <tuple>
#include <set>
#include <queue>
#include <algorithm>
#include <sstream>
#include <fstream>

struct msg
{
    int user = 0;
    int num = 0;
    int start = 0;
    int send = 0;

    msg(int _user, int _num, int _start, int _send): user(_user), num(_num), start(_start), send(_send) { };

    msg(){};

    friend bool operator<(const msg& m1, const msg& m2)
    {
        return m1.send < m2.send;
    }
};

struct user_msg_count
{
    int number = 0;
    bool is_ready = 1;
};

int get_pois_distrib(double lam)
{
    int k = 0;
    double p = 1;
    for (double t = exp(-lam); p > t; k++)
        p *= ((double)(rand() + 1)) / RAND_MAX;
    return k - 1;
}

std::tuple<double, double, double> adapt_interval_ALOHA(double lam, int M, int T, int W = 1){

    W = M - 1;
    if (lam <= .0) lam = .01;
    double lam_m = lam / M;
    double out_c = 0, out_n = 0, out_t = 0;
    std::multiset<msg> mes_q;
    std::vector<std::vector<msg>> user_mes(M, std::vector<msg>());
    std::vector<user_msg_count> user_mes_n(M, user_msg_count());

    short state;
    int t = 0;
    for (; t < T || !mes_q.empty(); t++, state = 0)
    {
        if (t < T)
        {
            for (int i = 0; i < M; i++)
            {
                int temp = get_pois_distrib(lam_m);
                for (int ii = 0; ii < temp; ii++ )
                {
                    msg m(i, user_mes_n[i].number, t, -1);

                    if(user_mes_n[i].is_ready)
                    {
                        m.send = (rand() % W) + 1 + t;
                        mes_q.insert(m);
                        user_mes_n[i].is_ready = false;
                    }
                    else user_mes[i].push_back(m);
                    user_mes_n[i].number++;
                }
            }
        }
        if(!mes_q.empty() && mes_q.begin()->send == t)
        {
            msg m0 = *mes_q.begin();
            mes_q.erase(mes_q.begin());

            if(!mes_q.empty() && mes_q.begin()->send == t)
            {

                int r = rand() % W + 1;
                m0.send = t + r;
                mes_q.insert(m0);

                while(!mes_q.empty() && mes_q.begin()->send == t)
                {
                    msg m = *mes_q.begin();
                    mes_q.erase(mes_q.begin());
                    r = rand() % W + 1;
                    m.send = t + r;
                    mes_q.insert(m);
                }
                state = -1;
            }
            else
            {
                out_c += 1;
                out_t += t - m0.start;
                out_n += t - m0.start - 1;

                if(!user_mes[m0.user].empty())
                {
                    msg m = *user_mes[m0.user].begin();
                    user_mes[m0.user].erase(user_mes[m0.user].begin());
                    m.send = t + (rand() % W) + 1;
                    mes_q.insert(m);
                }
                else user_mes_n[m0.user].is_ready = true;

                if(user_mes[m0.user].empty())
                    user_mes_n[m0.user].is_ready = true;

                state = 1;
            }
        }

        if (state == 0) { W = std::max(1., (double)W / 2);}
        else if (state < 0) { W = std::min(M << 1, W << 1);}

    }

    return {out_n / t, out_t / out_c, out_c / t};
}

void draw(int T, std::vector<int> Ms, int W = 1)
{
    std::vector<double> xs;
    for(double i = .1; i < 1.; i += .1)
        xs.push_back(i);
    std::vector<std::vector<double>> g1(Ms.size(), std::vector<double>(xs.size(), .0));
    std::vector<std::vector<double>> g2(Ms.size(), std::vector<double>(xs.size(), .0));
    std::vector<std::vector<double>> g3(Ms.size(), std::vector<double>(xs.size(), .0));

    for(size_t i = 0; i < xs.size(); i++)
    {
        for (size_t j = 0; j < Ms.size(); j++)
        {
            auto res = adapt_interval_ALOHA(xs[i], Ms[j], T, W);

            g1[j][i] = std::get<0>(res);
            g2[j][i] = std::get<1>(res);
            g3[j][i] = std::get<2>(res);
        }
    }

    std::cout << "Lambdas: \n";
    for (size_t i = 0; i < xs.size(); i++)
    {
        std::cout << xs[i] << " ";
    }
    std::cout << std::endl;

    for (size_t i = 0; i < Ms.size(); i++)
    {
        std::cout << "M = " << Ms[i] << std::endl;

        std::cout << "N(lam): \n";
        for (size_t j = 0; j < xs.size(); j++)
        {
            std::cout << g1[i][j] << " ";
        }
        std::cout << std::endl;

        std::cout << "d(lam): \n";
        for (size_t j = 0; j < xs.size(); j++)
        {
            std::cout << g2[i][j] << " ";
        }
        std::cout << std::endl;

        std::cout << "lam_out(lam): \n";
        for (size_t j = 0; j < xs.size(); j++)
        {
            std::cout << g3[i][j] << " ";
        }
        std::cout << std::endl;
        std::cout << std::endl;
    }
}



int main()
{
    srand(time(0));

    draw(1000, {2, 5, 10});
//    auto res = adapt_interval_ALOHA(0.4, 4, 10000, 1);
//    std::cout << std::get<0>(res) << " " << std::get<1>(res) << " " << std::get<2>(res);

    return 0;
}
