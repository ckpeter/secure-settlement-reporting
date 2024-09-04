
#ifndef UTIL_CPP
#define UTIL_CPP

#include <boost/algorithm/string.hpp>

using namespace emp;
using namespace std;

#define DATA_BITS 32

Integer data_integer(int64_t val, int party=PUBLIC) {
  return Integer(DATA_BITS, val, party);
}

string lltohex(uint64_t n) {
  std::stringstream sstream;
  sstream << std::hex << n;
  string s = sstream.str();  
  std::transform(s.begin(), s.end(), s.begin(), std::ptr_fun<int, int>(std::toupper));
  return s;
}

vector<bool> atobools(int bits, string s) {
  boost::trim(s);
  
  if(bits > 0) {
    s = lltohex(atoll(s.c_str())); // parse as decimal
  }
  
  bits = abs(bits);
  vector<bool> bools(bits, 0);
  size_t base = 0;
  
  // Hex format is MS digit first, e.g. '10' => 16, not 1
  for(int k = s.size()-1; k >= 0; k--) {
    char c = s[k];    
    int val = 0;

    if(c == 'A' || c == 'a') {
      val = 10;
    } else if(c == 'B' || c == 'b') {
      val = 11;    
    } else if(c == 'C' || c == 'c') {
      val = 12;
    } else if(c == 'D' || c == 'd') {
      val = 13;
    } else if(c == 'E' || c == 'e') {
      val = 14;
    } else if(c == 'F' || c == 'f') {
      val = 15;
    } else {
      val = c - '0';
      
      if(val < 0 || val > 9) {
        cerr << "INVALID char '" << c << "' when parsing '" << s << "'\n";
        exit(1);
      }
    }
    
    bitset<4> bs(val);

    for(size_t kk = 0; kk < bs.size(); kk++) {
      if((base+kk) >= bools.size() && bs[kk] == true) {
        cerr << "Value overflow field (" << base+kk << 
          " vs " << bools.size() << "): " << s << endl; // or allow
      } else {        
        bools[base+kk] = bs[kk];
      }
    }
    
    base += bs.size();
  }
  
  // bools[0] is LSB
  return bools;
}

Integer intfrombools(int bits, vector<bool> bools, int party) {  
  bits = abs(bits);
  vector<Bit> sbs(bits, Bit(0, PUBLIC));
  
  for(size_t k = 0; k < bools.size(); k++) {    
    sbs[k] = Bit(bools[k], party);
  }
  
  Integer val(bits, 0, party);
  
  for(int k = 0; k < val.size() && k < (int) sbs.size(); k++) {    
    val.bits[k] = sbs[k];
  }  
  
  return val;
}

vector<vector<bool>> make_dummy_bools(vector<int64_t> col_sizes) {
  vector<vector<bool>> bools;
  
  for(auto col : col_sizes) {
    bools.push_back(vector<bool>(abs(col), false));
  }
  
  return bools;
}

vector<Integer> make_integers(int party, vector<int64_t> col_sizes, vector<vector<bool>> info) {
  vector<Integer> ints;
  
  for(size_t k = 0; k < col_sizes.size(); k++) {    
    ints.push_back(intfrombools(abs(col_sizes[k]), info[k], party));
  }
  
  return ints;
}

Integer intfromhex(int bits, string s, int party) {
  auto bools = atobools(bits, s);
  return intfrombools(bits, bools, party);
}

string string_to_hex(const std::string& in) {
  stringstream ss;

  ss << std::hex << std::setfill('0');
  for (size_t i = 0; in.length() > i; ++i) {
      ss << std::setw(2) << static_cast<unsigned int>(static_cast<unsigned char>(in[i]));
  }

  return ss.str(); 
}

Integer hash_string(string s, int party=PUBLIC) {
  Hash hash;

  const char *data = s.c_str();
  char dig[Hash::DIGEST_SIZE];
  
  hash.put(data, s.size());
  hash.digest(dig);
  
  string digs(dig, Hash::DIGEST_SIZE);
  string hex = string_to_hex(digs);
      
  return intfromhex(256, hex, party);
}

bool is_blank(std::string s){
  for(size_t index = 0; index < s.length(); index++){
    if(!std::isspace(s[index]))
      return false;
  }

  return true;
}

int64_t exchange_count(int party, int64_t own, NetIO *io) {
  int64_t other = 0;
  
  if(party == ALICE) {
    io->send_data(&own, sizeof(int64_t));      
    io->recv_data(&other, sizeof(int64_t));    
  } else if(party == BOB) {
    io->recv_data(&other, sizeof(int64_t));
    io->send_data(&own, sizeof(int64_t));
  }
    
  if(party == BOB) {
    other *= -1;
  }
  
  cout << "own: " << own << endl;
  cout << "other: " << other << endl;
  
  return other;
}

void print_data(vector<vector<Integer>> rows) {
  for(auto x : rows) {
    for(auto y : x) {
      cout << y.reveal<uint64_t>() << ", ";
    }
    
    cout << endl;
  }  
}

template<class Real, class Fake>
void add_data(int64_t own, int64_t other, Real real, Fake fake) {
  for(int64_t k = other; k < 0; k++) {
    // cout << "Pre-adding fake node..." << k << endl;
    fake(k);
  }
  
  for(int64_t k = 0; k < own; k++) {
    real(k);
  }
  
  for(int64_t k = 0; k < other; k++) {
    // cout << "Post-adding fake node..." << k << endl;
    fake(k);
  }
}

string gate_s(uint64_t count) {
  string unit = "";
  
  if(count > 1000 * 1) {
    unit = "K";
    count /= 1000;
  }
  
  if(count > 1000 * 1) {
    unit = "M";
    count /= 1000;
  }
  
  return to_string(count) + unit;
}

int64_t gate_out(int64_t last=-1, string title="") {
  auto val = CircuitExecution::circ_exec->num_and();
  auto diff = val - last;
  
  if(last >= 0) {
    cout << "GATES used by " << title << ": " << gate_s(diff) << endl;
  }
  
  return val;
}

template<class Func>
void log_gates(Func func, string title) {
  auto base = gate_out();
  func();
  gate_out(base, title);
}

namespace array_based {
  template<typename T>
  Bit compare(T a, T b) {
    return a > b;
  }

  template<typename T, typename D>
  void cmp_swap(T*key, D*data, int i, int j, Bit acc,
                   Bit (*comparator)(T, T)) {
      Bit to_swap = (comparator(key[i], key[j]) == acc);
      swap(to_swap, key[i], key[j]);
      if(data != nullptr)
          swap(to_swap, data[i], data[j]);
  }

  inline int greatestPowerOfTwoLessThan(int n) {
      int k = 1;
      while (k < n)
          k = k << 1;
      return k >> 1;
  }

  template<typename T, typename D>
  void bitonic_merge(T* key, D* data, int lo, int n, Bit acc,
                   Bit (*comparator)(T, T)) {
      if (n > 1) {
          int m = greatestPowerOfTwoLessThan(n);
          for (int i = lo; i < lo + n - m; i++)
              cmp_swap(key, data, i, i + m, acc, comparator);
          bitonic_merge(key, data, lo, m, acc, comparator);
          bitonic_merge(key, data, lo + m, n - m, acc, comparator);
      }
  }

  template<typename T, typename D>
  void bitonic_sort(T * key, D * data, int lo, int n, Bit acc,
                   Bit (*comparator)(T, T)) {
      if (n > 1) {
          int m = n / 2;
          bitonic_sort(key, data, lo, m, !acc, comparator);
          bitonic_sort(key, data, lo + m, n - m, acc, comparator);
          bitonic_merge(key, data, lo, n, acc, comparator);
      }
  }

  template <typename T, typename D = Bit>
  void sort(T * key, int size, D* data = nullptr, Bit acc = true,
                   Bit (*comparator)(T, T)=&compare) {
      bitonic_sort(key, data, 0, size, acc, comparator);
  }
};

namespace vec_based {  
  template<typename T>
  void vector_swap(vector<T> *v, int j, int k, Bit real) {
    T newj = v->at(j).select(real, v->at(k));
    T newk = v->at(k).select(real, v->at(j));
    
    v->at(j) = newj;
    v->at(k) = newk;
  }
  
  template<typename T>
  Bit compare(T a, T b) {
    return a > b;
//  return a > b;
  }

  template<typename T, typename D>
  void cmp_swap(vector<T> *key, vector<D> *data, int i, int j,
                       Bit acc, Bit (*comparator)(T, T)) {
      Bit to_swap = (comparator(key->at(i), key->at(j)) == acc);    
    
      // swap(to_swap, key[i], key[j]);
      vector_swap(key, i, j, to_swap);
    
      if(data != nullptr)
          // swap(to_swap, data[i], data[j]);
          vector_swap(data, i, j, to_swap);
  }

  inline int greatestPowerOfTwoLessThan(int n) {
      int k = 1;
      while (k < n)
          k = k << 1;
      return k >> 1;
  }

  template<typename T, typename D>
  void bitonic_merge(vector<T>* key, vector<D>* data, int lo,
                            int n, Bit acc, Bit (*comparator)(T, T)) {
      if (n > 1) {
          int m = greatestPowerOfTwoLessThan(n);
          for (int i = lo; i < lo + n - m; i++)
              cmp_swap(key, data, i, i + m, acc, comparator);
          bitonic_merge(key, data, lo, m, acc, comparator);
          bitonic_merge(key, data, lo + m, n - m, acc, comparator);
      }
  }

  template<typename T, typename D>
  void bitonic_sort(vector<T> * key, vector<T> * data,
                           int lo, int n, Bit acc, Bit (*comparator)(T, T)) {
      if (n > 1) {
          int m = n / 2;
          bitonic_sort<T, D>(key, data, lo, m, !acc, comparator);
          bitonic_sort<T, D>(key, data, lo + m, n - m, acc, comparator);
          bitonic_merge<T, D>(key, data, lo, n, acc, comparator);
      }
  }

  template <typename T, typename D = Bit>
  void sort(vector<T> * key, vector<T> * data = nullptr, Bit acc = true,
                   Bit (*comparator)(T, T)=&vec_based::compare) {
      bitonic_sort<T, D>(key, data, 0, key->size(), acc, comparator);
  }
}

#endif