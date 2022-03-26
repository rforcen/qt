#pragma once

#include <algorithm>
#include <vector>

using std::vector, std::reverse;

class MasterMind {
public:
  template <class T> vector<vector<T>> permutations(vector<T> d, int k) {
    vector<vector<T>> res;

    if (k <= (int)d.size())
      do {
        res.push_back(vector<T>(d.begin(), d.begin() + k)); // save d[:k]
        reverse(d.begin() + k, d.end());
      } while (next_permutation(d.begin(), d.end()));

    return res;
  }
  typedef vector<vector<int>> Perms;

  Perms allPerms = permutations<int>({1, 2, 3, 4, 5, 6}, 4),
        matchPerms = allPerms;

  class Probe {
  public:
    vector<int> v;
    int b, w;
  };

  vector<Probe> probes;

  bool find(vector<int> v, int i) {
    return std::find(v.begin(), v.end(), i) != v.end();
  }

  // find matching
  void findMatches() {
    Perms mp;
    for (auto &perm : matchPerms) {
      bool isValid = true;
      for (auto &probe : probes) {
        int b = 0, w = 0;
        for (auto i = 0; i < 4; i++) {
          if (probe.v[i] == perm[i])
            b++;
          else if (find(perm, probe.v[i]))
            w++;
        }
        if (!(b == probe.b and w == probe.w)) {
          isValid = false;
          break;
        }
      }
      if (isValid)
        mp.push_back(perm);
    }

    if (mp.size() > 0)
      matchPerms = mp;
  }
};
