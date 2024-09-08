
#include <iostream>

#include <map>

#include "emp-sh2pc/emp-sh2pc.h"
#include "emp-tool/utils/hash.h"
#include "emp-tool/circuits/number.h"

#include "parser.cpp"

using namespace std;

void secureReconstructSubmissions(vector<Submission>& subA,
                            vector<Submission>& subB,
                            vector<SecureSubmission>& secureSubmissions) {
  for (size_t i = 0; i < subA.size(); ++i) {
    secureSubmissions.push_back(SecureSubmission(subA[i], subB[i]));
  }
}

void markDuplicates(vector<SecureSubmission>& secureSubmissions) {
  auto last = secureSubmissions[0];

  for (size_t i = 1; i < secureSubmissions.size(); ++i) {
    secureSubmissions[i].dup = last.bit_equal(secureSubmissions[i]);
    last = secureSubmissions[i];
  }
}

void recordParties(vector<SecureSubmission>& secureSubmissions,
                    vector<SecureParty>& secureParties) {
  for (size_t i = 0; i < secureSubmissions.size(); ++i) {
    secureParties.push_back(SecureParty(secureSubmissions[i].party1));
    secureParties.push_back(SecureParty(secureSubmissions[i].party2));
  }
}

void markRepeatParties(vector<SecureParty>& secureParties) {
  vec_based::sort<SecureParty, SecureParty>(
    &(secureParties), nullptr, false,
    &(bit_sort_by_name));

  auto lastParty = secureParties[0];

  Integer zero = Integer(32, 0, PUBLIC);
  Integer one = Integer(32, 1, PUBLIC);

  lastParty = secureParties[0];
  Integer dupCount = Integer(32, 0, PUBLIC);

  for (size_t i = 1; i < secureParties.size(); i++) {
    Bit dup = secureParties[i].bit_equal(lastParty);
    dupCount = zero.select(dup, dupCount);

    secureParties[i].count = dupCount;

    dupCount = dupCount + one;
    lastParty = secureParties[i];
  } 
}

void unmaskRepeatParties(vector<SecureParty>& secureParties,
                          map<string, uint32_t>& repeatedParties) {
  uint32_t threshold = 3;

  for(const auto& party : secureParties) {
    if(party.count.reveal<uint32_t>() >= threshold) {
      auto old = repeatedParties.find(textualize(party.name));
      auto oldCount = old == repeatedParties.end() ? 0 : old->second;
      auto newCount = party.count.reveal<uint32_t>();
      repeatedParties[textualize(party.name)] = oldCount < newCount ? newCount : oldCount;
    }
  }
}

void recordStatistics(Integer& sum, Integer& count,
  vector<Integer>& histogram, vector<uint64_t>& histogramBounds,
  vector<Integer>& yearly, vector<uint8_t>& yearlyBounds,
  vector<SecureSubmission> secureSubmissions) {
  Integer zero = Integer(32, 0, PUBLIC);
  Integer one = Integer(32, 1, PUBLIC);

  for (const auto& sub : secureSubmissions) {
    auto val = sub.amount.select(sub.dup, zero);          
    sum = sum + val;
    count = count + one.select(sub.dup, zero);

    for(size_t i = 0; i < histogramBounds.size()-1; i++) {
      auto sel = (!sub.dup) &
        (val >= Integer(32, histogramBounds[i], PUBLIC)) &
        (val < Integer(32, histogramBounds[i + 1], PUBLIC));

      histogram[i] = histogram[i] + zero.select(sel, one);
    }

    for(size_t i = 0; i < yearlyBounds.size()-1; i++) {            
      auto sel = (!sub.dup) &
        (sub.year_since_2000 >= Integer(8, yearlyBounds[i], PUBLIC)) &
        (sub.year_since_2000 < Integer(8, yearlyBounds[i + 1], PUBLIC));

      yearly[i] = yearly[i] + zero.select(sel, one);
    }
  }
}

int process_submissions(int party, string prefix) {
  Integer zero = Integer(32, 0, PUBLIC);
  Integer one = Integer(32, 1, PUBLIC);

  cout << "Using data from prefix: " << prefix << endl;

  // These files MUST be of the exact same size, line count, and format.
  string fileA = prefix + "/submission_bits_a.txt";
  string fileB = prefix + "/submission_bits_b.txt";

  string file = party == ALICE ? fileA : fileB;

  try {
      vector<Submission> subA = loadSubmissionsFromFile(file);
      vector<Submission> subB = loadSubmissionsFromFile(file);
      vector<SecureSubmission> secureSubmissions;
      vector<SecureParty> secureParties;
      map<string, uint32_t> repeatedParties;
      Integer sum = Integer(32, 0, PUBLIC);
      Integer count = Integer(32, 0, PUBLIC);
      vector<Integer> histogram;
      vector<uint64_t> histogramBounds = {0, 1000, 2000, 3000, 4000, 5000, 6000, 7000, 8000, 9000};
      for(size_t i = 0; i < histogramBounds.size(); i++) {
        histogram.push_back(Integer(32, 0, PUBLIC));
      }
      vector<Integer> yearly;
      vector<uint8_t> yearlyBounds = {0, 18, 19, 20, 21, 22, 23, 24};
      for(size_t i = 0; i < yearlyBounds.size(); i++) {
        yearly.push_back(Integer(32, 0, PUBLIC));
      }

      secureReconstructSubmissions(subA, subB, secureSubmissions);

      vec_based::sort<SecureSubmission, SecureSubmission>(
        &(secureSubmissions), nullptr, false,
        &(bit_sort_by_settlement_key));

      markDuplicates(secureSubmissions);
      
      recordParties(secureSubmissions, secureParties);
      
      markRepeatParties(secureParties);
      
      unmaskRepeatParties(secureParties, repeatedParties);

      recordStatistics(sum, count, histogram, histogramBounds,
                        yearly, yearlyBounds, secureSubmissions);

      printSubmissions(secureSubmissions);

      Integer average = sum / count;

      cout << "Total count of " << count.reveal<uint64_t>()
            << ", the sum amount is $" << sum.reveal<uint64_t>()
            << ", and average is $" << average.reveal<uint64_t>() << endl;

      cout << "Histogram:" << endl;
      for(size_t i = 0; i < histogram.size()-1; i++) {
        cout << histogramBounds[i] << " - " << histogramBounds[i + 1] << 
          ": " << histogram[i].reveal<uint64_t>() << endl;
      }

      cout << "Yearly:" << endl;
      for(size_t i = 0; i < yearly.size()-1; i++) {
        cout << (uint64_t) yearlyBounds[i] + 2000 << " - " <<
          (uint64_t) yearlyBounds[i + 1] + 2000 << ": " <<
          yearly[i].reveal<uint64_t>() << endl;
      }

      for(const auto& party : repeatedParties) {
        cout << "Repeat party: " << party.first << " count: " <<
          party.second << endl;
      }
  } catch (const exception& e) {
      cerr << "Exception occurred: " << e.what() << endl;
      return 1;
  }

  return 0;
}

int main(int argc, char** argv) {
  if(argc < 3) {
    cerr << "You need to pass in party and data dir prefix!\n" <<
    "Usage: prog alice_1_or_bob_2 data_dir [port=9999] [addr=127.0.0.1]" << endl;
    exit(1);
  }

  int party = atoi(argv[1]);
  string prefix = std::string(argv[2]);
  int port = 9999;

  if(argc >= 4) {
    port = atoi(argv[3]);
  }

  const char* addr = argc >= 5 ? argv[4] : "127.0.0.1";
  addr = party == ALICE ? nullptr : addr;

  if(addr == nullptr) {
    cout << "Alice listening for Bob on port " << port << "...\n";
  }

  NetIO * io = new NetIO(addr, port);
  setup_semi_honest(io, party);
  auto start = std::chrono::system_clock::now();
  

  // ***** Run program logic
  cout << "\n===== Program running...\n";
  
  process_submissions(party, prefix);
  
  cout << "\n===== Program completed ";  
  // ***** End program logic


  //auto gcount = (PlainCircExec*)CircuitExecution::circ_exec->gid; // if plain protocol
  auto gcount = CircuitExecution::circ_exec->num_and();

  cout << " (" << (std::chrono::system_clock::now() - start).count() / 1000.0/1000/1000
    << " s | " << gcount / 1000.0 / 1000
    << " M gates)" << endl;

  delete io;
  return 0;
}
