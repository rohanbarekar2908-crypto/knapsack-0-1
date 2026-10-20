#include <algorithm>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

struct Cargo {
    string name;
    int weight;
    long long utility;
};

// Best known state for one capacity: highest utility, then lightest weight.
struct State {
    long long utility = 0;
    int weight = 0;
};

class TruckLoader {
private:
    int capacity;
    vector<Cargo> cargo;

public:
    explicit TruckLoader(int cap) : capacity(cap) {}

    void addCargo(const string& name, int weight, long long utility) {
        cargo.push_back({name, weight, utility});
    }

    void solve() const {
        int n = static_cast<int>(cargo.size());

        // 1D rolling DP: dp[w] = best state using the items seen so far
        // with at most w units of capacity.
        vector<State> dp(capacity + 1);

        // take[i][w] records whether item i was loaded when the truck
        // had capacity w after processing items 0..i.
        vector<vector<char>> take(n, vector<char>(capacity + 1, 0));

        for (int i = 0; i < n; ++i) {
            const Cargo& c = cargo[i];
            // Go downwards so each item can be used at most once.
            for (int w = capacity; w >= c.weight; --w) {
                State candidate;
                candidate.utility = dp[w - c.weight].utility + c.utility;
                candidate.weight = dp[w - c.weight].weight + c.weight;

                bool better = candidate.utility > dp[w].utility ||
                              (candidate.utility == dp[w].utility &&
                               candidate.weight < dp[w].weight);
                if (better) {
                    dp[w] = candidate;
                    take[i][w] = 1;
                }
            }
        }

        // Walk back from the last item to recover what was loaded.
        vector<int> chosen;
        int w = capacity;
        for (int i = n - 1; i >= 0; --i) {
            if (take[i][w]) {
                chosen.push_back(i);
                w -= cargo[i].weight;
            }
        }
        reverse(chosen.begin(), chosen.end());

        printReport(chosen, dp[capacity]);
    }

private:
    void printReport(const vector<int>& chosen, const State& best) const {
        cout << "\n=========== LOADING PLAN ===========\n";
        if (chosen.empty()) {
            cout << "Nothing can be loaded.\n";
        } else {
            cout << left << setw(18) << "Item" << setw(10) << "Weight"
                 << setw(10) << "Utility" << "Per unit\n";
            cout << string(46, '-') << '\n';
            for (int idx : chosen) {
                const Cargo& c = cargo[idx];
                cout << left << setw(18) << c.name << setw(10) << c.weight
                     << setw(10) << c.utility << fixed << setprecision(2)
                     << static_cast<double>(c.utility) / c.weight << '\n';
            }
        }
        cout << string(46, '-') << '\n';
        cout << "Total utility : " << best.utility << '\n';
        cout << "Total weight  : " << best.weight << " / " << capacity << '\n';
        cout << "Space left    : " << capacity - best.weight << '\n';
        cout << "(Ties in utility are resolved by choosing the lighter load.)\n";
    }
};

int main() {
    int capacity, n;

    cout << "Truck capacity: ";
    if (!(cin >> capacity) || capacity < 0) {
        cerr << "Capacity must be a non-negative integer.\n";
        return 1;
    }

    cout << "Number of items: ";
    if (!(cin >> n) || n < 0) {
        cerr << "Item count must be a non-negative integer.\n";
        return 1;
    }

    TruckLoader loader(capacity);

    cout << "Enter each item as: name weight utility  (name has no spaces)\n";
    for (int i = 0; i < n; ++i) {
        string name;
        int weight;
        long long utility;
        if (!(cin >> name >> weight >> utility) || weight <= 0 || utility < 0) {
            cerr << "Invalid item. Weight must be > 0 and utility >= 0.\n";
            return 1;
        }
        loader.addCargo(name, weight, utility);
    }

    loader.solve();
    return 0;
}

/*
Sample input:
10
4
Water 5 10
Tent 4 40
Stove 6 30
Food 3 50

Expected: Tent + Food, total utility 90, total weight 7, space left 3.
*/
