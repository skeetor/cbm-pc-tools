#include <iostream>
#include <string>
#include <cstdlib>
#include <set>
#include <ctime>
#include <chrono>

using namespace std;

static string createRandomString(uint32_t maxLen, const string &charset = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ")
{
    string s;

    while (s.size() < maxLen)
    {
        size_t val = rand() / ((RAND_MAX + 1u) / charset.length());

        s += charset[val];
    }

    return s;
}

static void help(void)
{
    cout << "USAGE: --hashLength N (default = 64) --count N (default = 1000000) --prefix string" << endl;
}

int main(int argc, char *argv[])
{
    const string hex = "0123456789abcdef";

    uint32_t hashLen = 64;
    size_t count = 1000000;
    string prefix;
    char *e;

    size_t i = 1;

    while (i < argc)
    {
        string par = argv[i++];

        if (par == "--hashLength")
        {
            if (i < argc)
            {
                par = argv[i++];
                hashLen = strtol(par.c_str(), &e, 10);
                continue;
            }
            else
            {
                cout << "--hashLength requires a numeric value";
                return 10;
            }
        }

        if (par == "--count")
        {
            if (i < argc)
            {
                par = argv[i++];
                count = strtol(par.c_str(), &e, 10);
                continue;
            }
            else
            {
                cout << "--count requires a numeric value";
                return 11;
            }
        }

        if (par == "--prefix")
        {
            if (i < argc)
            {
                prefix = argv[i++];
                continue;
            }
            else
            {
                cout << "--prefix requires a string value";
                return 11;
            }
        }


        if (par == "--help")
        {
            help();
            return 0;
        }
    }

    i = 0;
    set<string> cache;
    time_t curr_time;
    tm * curr_tm;
    char date_string[100];
    char time_string[100];

    while (i < count)
    {
        time(&curr_time);
        curr_tm = localtime(&curr_time);

        strftime(date_string, 50, "%F", curr_tm);
        strftime(time_string, 50, " %T", curr_tm);

        string hash = createRandomString(hashLen, hex);
        if (cache.find(hash) != cache.end())
            continue;

        cache.emplace(hash);
        cout << prefix << hash << "," << date_string << time_string << endl;

        i++;
    }

    return 0;
}
