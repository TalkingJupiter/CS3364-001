/**
 * @file rank_reliability.cpp
 * @brief DISCL student project: build a consensus ranking and grade each source by how much it disagrees.
 * @author Batuhan Sencer & Larry To
 * @date 2025-09-26
 *
 * What this does (plain English):
 * - We take several ranked lists (think: five different “top-N” lists).
 * - We fuse them into one consensus order by summing ranks (Borda-style).
 * - For each original list, we measure how “out of order” it is vs. the consensus
 *   using inversion counts (how many pairs are flipped).
 * - We report a reliability score in [0,1]: higher = closer to the consensus.
 *
 * Outputs:
 * - CSVs with the consensus order and per-source reliability,
 * - A short Markdown report that’s easy to drop into a lab notebook or GitHub README.
 *
 * Super short recap: fuse by sum-of-ranks → count inversions per source → reliability = 1 − inv/max_inv.
 */

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <map>
#include <set>
#include <algorithm>
#include <functional>
#include <iomanip>
#include <filesystem>
#include <cmath>
#include <utility>

using namespace std;

using ll = long long;

/**
 * @brief Inversion counter using merge sort (O(n log n)).
 *
 * Intuition:
 * An inversion is a pair (i < j) where a[i] > a[j]. During the merge step,
 * whenever a right-half element jumps ahead of the remaining left-half elements,
 * it creates (m - i) inversions at once.
 *
 * Notes for teammates:
 * - We reuse a single temp buffer and write the sorted range back into @p a.
 * - Equal values are kept stable (no inversions added when a[i] == a[j]).
 *
 * SSR: divide & conquer; add (m − i) when right beats left in merge.
 */
static long long merge_count(vector<long long>& a) {
    int n = (int)a.size();
    vector<long long> tmp(n);
    function<long long(int,int)> rec = [&](int l, int r)->long long {
        if (r - l <= 1) return 0;
        int m = (l + r) >> 1;
        long long inv = rec(l, m) + rec(m, r);
        int i = l, j = m, k = l;
        while (i < m && j < r) {
            if (a[i] <= a[j]) tmp[k++] = a[i++];
            else { tmp[k++] = a[j++]; inv += (m - i); }
        }
        while (i < m) tmp[k++] = a[i++];
        while (j < r) tmp[k++] = a[j++];
        for (int t = l; t < r; ++t) a[t] = tmp[t];
        return inv;
    };
    return rec(0, n);
}

/**
 * @brief Fenwick Tree (BIT) for prefix sums (point update, prefix query).
 *
 * Why here:
 * - We use it to count “how many smaller elements have we seen so far” in O(log n),
 *   which turns inversion counting into a right-to-left sweep.
 *
 * API:
 *   BIT bit(n);      // 1-based size
 *   bit.add(i, v);   // arr[i] += v
 *   bit.sum(i);      // sum over [1..i]
 *
 * SSR: tiny tree array that gives prefix sums fast; perfect for inversions.
 */
struct BIT {
    int n; vector<long long> f;
    BIT(int n): n(n), f(n+1, 0) {}
    void add(int i, long long v){ for(; i<=n; i+= i&-i) f[i] += v; }
    long long sum(int i){ long long s=0; for(; i>0; i-= i&-i) s += f[i]; return s; }
};

/**
 * @brief Inversion counter via BIT (Fenwick) with coordinate compression.
 *
 * How it works:
 * 1) Compress values into [1..K] so we can index the BIT.
 * 2) Scan from right to left; for value x, add how many seen values are < x:
 *      inv += bit.sum(rank(x) - 1);
 *    then mark x as seen: bit.add(rank(x), 1).
 *
 * Complexity: O(n log n) time, O(n) memory.
 *
 * SSR: compress → sweep right-to-left → use BIT prefix sums to add “smaller-seen” counts.
 */
static long long bit_count_inversions(vector<long long> a){
    // coordinate compress
    vector<long long> v = a;
    sort(v.begin(), v.end());
    v.erase(unique(v.begin(), v.end()), v.end());
    vector<int> comp(a.size());
    for(size_t i=0;i<a.size();++i){
        comp[i] = int(lower_bound(v.begin(), v.end(), a[i]) - v.begin()) + 1; // 1-based
    }
    BIT bit((int)v.size());
    long long inv = 0;
    // traverse from right to left
    for(int i=(int)comp.size()-1;i>=0;--i){
        inv += bit.sum(comp[i]-1);
        bit.add(comp[i], 1);
    }
    return inv;
}

/**
 * @brief Quicksort-style “count while partitioning” (diagnostic).
 *
 * What we’re doing:
 * - Pick a pivot; scan once. Every time we see a smaller-than-pivot after a greater-than-pivot,
 *   that’s a cross-inversion. We count those, then recurse on <pivot and >pivot buckets.
 *
 * When to trust:
 * - Great for intuition and sanity checks, but merge/BIT are our ground truth.
 * - Duplicates are treated as equal (no inversions among ties).
 *
 * SSR: pivot → count “greater-before-smaller” during partition → recurse.
 */
static long long quick_partition_count(const vector<long long>& a){
    function<long long(int,int)> rec = [&](int l, int r)->long long{
        if (r - l <= 1) return 0;
        int n = r - l;
        long long pivot = a[l + n/2];

        vector<long long> less; less.reserve(n);
        vector<long long> equal; equal.reserve(n);
        vector<long long> greater; greater.reserve(n);
        long long seen_greater = 0;
        long long cross = 0;
        for (int i=l;i<r;++i){
            if (a[i] > pivot){ greater.push_back(a[i]); seen_greater++; }
            else if (a[i] < pivot){ less.push_back(a[i]); cross += seen_greater; }
            else { equal.push_back(a[i]); }
        }

        long long inv = cross;
        if (!less.empty())   inv += quick_partition_count(less);
        if (!greater.empty()) inv += quick_partition_count(greater);
        return inv;
    };
    return rec(0, (int)a.size());
}

/**
 * @brief Run all three counters on the same array for cross-checking.
 *
 * Why:
 * - merge_count and BIT should match exactly (both O(n log n)).
 * - quick_partition_count is included as a learning/diagnostic baseline.
 *
 * SSR: copy → run merge, BIT, quick → compare.
 */
struct InvTriple { long long merge_inv, bit_inv, quick_inv; };
static InvTriple three_way_inv(vector<long long> arr){
    vector<long long> a1 = arr;
    vector<long long> a2 = arr;
    vector<long long> a3 = arr;
    long long m = merge_count(a1);
    long long b = bit_count_inversions(a2);
    long long q = quick_partition_count(a3);
    return {m,b,q};
}


/**
 * @brief CLI entry: build consensus ranking, count inversions per source, write reports.
 *
 * Usage:
 *   rank_reliability --out OUT_DIR source1.txt [source2.txt ...]
 *
 * Inputs:
 *   - Exactly 1+ source files; each file is a newline-separated list of item IDs (strings/ints),
 *     ordered from best (top) to worst (bottom).
 *
 * Processing pipeline:
 *   1) Read all sources; collect the universe of unique item IDs.
 *   2) Build per-source rank maps (1-based; "missing" items get rank = max_len + 1).
 *   3) Compute a **combined ranking** by SUM of ranks (lower sum = better; avg used as tiebreaker).
 *   4) For each source, create an array `a` of **combined positions** in that source’s order.
 *      If a source omits items, append the missing ones in combined-order at the end.
 *   5) Count inversions of `a` three ways:
 *        - merge_count (O(n log n), authoritative)
 *        - bit_count_inversions (O(n log n), authoritative)
 *        - quick_partition_count (diagnostic)
 *   6) Compute reliability = 1 − inv / (N*(N−1)/2) ∈ [0,1].
 *
 * Outputs (written to OUT_DIR):
 *   - combined_order.csv          : (position, item, sum_rank, avg_rank)
 *   - inversions_summary.csv      : (source, n, inv_merge, inv_bit, inv_quick, max_inv, reliability)
 *   - <source_name>_positions.csv : (index_in_source, combined_position) for each input source
 *   - report.md                   : brief methodology + results table
 *
 * Notes:
 *   - Merge and BIT counts should match exactly; quick may differ (diagnostic only).
 *   - Items are treated as strings; identical lines are identical items.
 *   - CSVs use commas, no quoting (safe for numeric IDs).
 *
 * Exit codes:
 *   1: bad usage (missing args)    2: missing --out or files    3: failed to open a source file
 */
int main(int argc, char** argv){
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    if (argc < 3){
        cerr << "Usage: " << argv[0] << " --out OUT_DIR source1.txt [source2.txt ...]\n";
        return 1;
    }

    string out_dir;
    vector<string> files;
    for (int i=1;i<argc;++i){
        string arg = argv[i];
        if (arg == "--out" && i+1 < argc){
            out_dir = argv[++i];
        } else {
            files.push_back(arg);
        }
    }
    if (out_dir.empty() || files.empty()){
        cerr << "Error: must pass --out and at least one source file.\n";
        return 2;
    }


    vector<vector<string>> src_items;
    vector<string> src_names;
    unordered_set<string> universe;
    for (auto& f : files){
        ifstream in(f);
        if (!in){
            cerr << "Failed to open " << f << "\n";
            return 3;
        }
        vector<string> list;
        string line;
        while (std::getline(in, line)){
            if (!line.empty() && line.back()=='\r') line.pop_back();
            if (line.size()==0) continue;
            list.push_back(line);
            universe.insert(line);
        }
        src_items.push_back(std::move(list));

        size_t pos = f.find_last_of("/\\");
        src_names.push_back(pos==string::npos ? f : f.substr(pos+1));
    }


    int S = (int)src_items.size();
    unordered_map<string, vector<int>> ranks; // item -> rank per source (1-based, or large if missing)
    int max_len = 0;
    for (int s=0;s<S;++s){
        max_len = max<int>(max_len, src_items[s].size());
    }
    int missing_rank = max_len + 1;

    for (auto& item : universe) ranks[item] = vector<int>(S, missing_rank);
    for (int s=0;s<S;++s){
        for (int i=0;i<(int)src_items[s].size();++i){
            ranks[src_items[s][i]][s] = i+1;
        }
    }


    struct Agg { string item; long long sum; double avg; };
    vector<Agg> agg;
    agg.reserve(universe.size());
    for (auto& kv : ranks){
        long long sum = 0;
        for (int s=0;s<S;++s) sum += kv.second[s];
        double avg = double(sum)/double(S);
        agg.push_back({kv.first, sum, avg});
    }
    sort(agg.begin(), agg.end(), [&](const Agg& a, const Agg& b){
        if (a.sum != b.sum) return a.sum < b.sum;
        if (a.avg != b.avg) return a.avg < b.avg;
        return a.item < b.item;
    });


    unordered_map<string,int> pos_combined;
    pos_combined.reserve(agg.size()*1.3);
    for (int i=0;i<(int)agg.size();++i){
        pos_combined[agg[i].item] = i+1;
    }

    // Prepare output dir
    std::error_code ec;
    std::filesystem::create_directories(out_dir, ec);

    // Write combined order CSV
    {
        ofstream out(out_dir + "/combined_order.csv");
        out << "position,item,sum_rank,avg_rank\n";
        for (int i=0;i<(int)agg.size();++i){
            out << (i+1) << "," << agg[i].item << "," << agg[i].sum << "," << fixed << setprecision(4) << agg[i].avg << "\n";
        }
    }

    // For each source: build position array according to combined order to count inversions
    struct Row { string src; long long n; long long inv_merge; long long inv_bit; long long inv_quick; long long max_inv; double reliability; };
    vector<Row> summary;

    long long N = (long long)agg.size();
    long long max_inv = N*(N-1)/2;

    for (int s=0;s<S;++s){
        // Create array 'a' = combined position sequence in the order of source s,
        // excluding items not present in combined (shouldn't happen).
        vector<long long> a;
        a.reserve(N);
        for (auto& it : src_items[s]){
            auto itp = pos_combined.find(it);
            if (itp != pos_combined.end()) a.push_back(itp->second);
        }
        // If some items are missing in this source (present elsewhere), append them at the end
        if ((long long)a.size() < N){
            // add missing items in combined order after the listed ones
            unordered_set<string> present(src_items[s].begin(), src_items[s].end());
            for (auto& ag : agg){
                if (!present.count(ag.item)){
                    a.push_back(pos_combined[ag.item]);
                }
            }
        }

        auto tr = three_way_inv(a);

        // Quick is only diagnostic: log as info, not a warning.
        if (tr.quick_inv != tr.merge_inv) {
            cerr << "[INFO] quick differs by "
                << (tr.merge_inv - tr.quick_inv)
                << " for " << src_names[s] << "\n";
        }

        double rel = 1.0 - (double)tr.merge_inv / (double)max_inv;
        summary.push_back({src_names[s], (long long)a.size(), tr.merge_inv, tr.bit_inv, tr.quick_inv, max_inv, rel});

        // Write the per-source positions
        ofstream out(out_dir + "/" + src_names[s] + "_positions.csv");
        out << "index_in_source,combined_position\n";
        for (size_t i=0;i<a.size();++i){
            out << (i+1) << "," << a[i] << "\n";
        }
    }

    // Write summary CSV
    {
        ofstream out(out_dir + "/inversions_summary.csv");
        out << "source,n,inv_merge,inv_bit,inv_quick,max_inv,reliability\n";
        out << std::fixed << setprecision(6);
        for (auto& r : summary){
            out << r.src << "," << r.n << "," << r.inv_merge << "," << r.inv_bit << "," << r.inv_quick << "," << r.max_inv << "," << r.reliability << "\n";
        }
    }

    // Markdown report
    // {
    //     ofstream out(out_dir + "/report.md");
    //     out << "# Ranking Reliability Report\n\n";
    //     out << "- Sources: " << S << "\n";
    //     out << "- Total unique items: " << N << "\n";
    //     out << "- Max inversions for N items: " << max_inv << "\n\n";
    //     out << "## Methodology\n";
    //     out << "We computed a **combined ranking** by summing per-source ranks (lower sum = better). For each source, we mapped its order to the combined order and counted inversions using three independent methods: (1) modified merge sort, (2) Binary Indexed Tree (Fenwick), and (3) a quicksort-style partition recursion that exactly counts cross-inversions.\n\n";
    //     out << "A **reliability score** is defined as `1 - (inversions / max_inversions)` ∈ [0,1]. Higher means closer to the consensus.\n\n";
    //     out << "## Results\n";
    //     out << "| Source | n | Inversions | Reliability |\n";
    //     out << "|---|---:|---:|---:|\n";
    //     out << std::fixed << setprecision(6);
    //     for (auto& r : summary){
    //         out << "| " << r.src << " | " << r.n << " | " << r.inv_merge << " | " << r.reliability << " |\n";
    //     }
    //     out << "\n";
    //     out << "See `combined_order.csv`, `inversions_summary.csv`, and `<source>_positions.csv` for details.\n";
    // }
    {
        ofstream out(out_dir + "/report.md");
        out << "# Ranking Reliability Report\n\n";
        out << "- Sources: " << S << "\n";
        out << "- Total unique items: " << N << "\n";
        out << "- Max inversions for N items: " << max_inv << "\n\n";
        out << "## Methodology\n";
        out << "We computed a **combined ranking** by summing per-source ranks (lower sum = better). "
            "For each source, we mapped its order to the combined order and counted inversions using "
            "two authoritative methods (Merge sort and Fenwick/BIT). A quicksort-style method is included "
            "for **diagnostic** insight only.\n\n";
        out << "A **reliability score** is defined as `1 - (inversions / max_inversions)` ∈ [0,1]. Higher means closer to the consensus.\n\n";
        out << "## Results (Merge-based)\n";
        out << "| Source | n | Inversions (merge) | Reliability |\n";
        out << "|---|---:|---:|---:|\n";
        out << std::fixed << setprecision(6);
        for (auto& r : summary){
            out << "| " << r.src << " | " << r.n << " | " << r.inv_merge << " | " << r.reliability << " |\n";
        }
        out << "\n_The quick partition counter is diagnostic and may differ; see `inversions_summary.csv` for all counters._\n";
    }

    cerr << "[INFO] Done. Wrote outputs under: " << out_dir << "\n";
    return 0;
}
