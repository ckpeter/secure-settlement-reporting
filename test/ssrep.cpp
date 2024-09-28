
#include <iostream>

#include <map>

#include "emp-sh2pc/emp-sh2pc.h"
#include "emp-tool/utils/hash.h"
#include "emp-tool/circuits/number.h"

#include "parser.cpp"

#define UNMASK_REPEAT_THRESHOLD 2 // 2 means party has 3+ settlements (repeating 2+ times)

using namespace std;

void xorReconstructSubmissions(vector<Submission>& subA,
                            vector<Submission>& subB,
                            vector<SecureSubmission>& secureSubmissions) {
  for (size_t i = 0; i < subA.size(); ++i) {
    secureSubmissions.push_back(SecureSubmission(subA[i], subB[i]));
  }
}

void markDupSubmissions(vector<SecureSubmission>& secureSubmissions) {
  auto last = secureSubmissions[0];

  for (size_t i = 1; i < secureSubmissions.size(); ++i) {
    secureSubmissions[i].dup = last.bit_equal(secureSubmissions[i]);
    last = secureSubmissions[i];
  }
}

void recordParties(vector<SecureSubmission>& secureSubmissions,
                    vector<SecureParty>& secureParties) {
  for (size_t i = 0; i < secureSubmissions.size(); ++i) {
    auto sub = secureSubmissions[i];
    secureParties.push_back(SecureParty(sub.party1, sub.dup));
    secureParties.push_back(SecureParty(sub.party2, sub.dup));
  }
}

void sortAndMarkRepeatParties(vector<SecureParty>& secureParties) {
  vec_based::sort<SecureParty, SecureParty>(
    &(secureParties), nullptr, false,
    &(bit_sort_by_name));

  auto& last = secureParties[0];

  Integer zero = Integer(32, 0, PUBLIC);
  Integer one = Integer(32, 1, PUBLIC);

  Integer dupCount = Integer(32, 0, PUBLIC);

  for (size_t i = 1; i < secureParties.size(); i++) {
    Bit dup = secureParties[i].bit_equal(last);
    dupCount = zero.select(dup, dupCount);

    secureParties[i].additional = dupCount;

    dupCount = dupCount + one.select(secureParties[i].invalid, zero);
    last = secureParties[i];
  } 
}

void unmaskRepeatParties(vector<SecureParty>& secureParties,
                          map<string, uint32_t>& repeatedParties) {

  for(const auto& party : secureParties) {
    if(party.additional.reveal<uint32_t>() >= UNMASK_REPEAT_THRESHOLD &&
      !party.invalid.reveal<bool>()) {
      auto old = repeatedParties.find(textualize(party.name));
      auto oldCount = old == repeatedParties.end() ? 0 : old->second;
      auto newCount = party.additional.reveal<uint32_t>();
      repeatedParties[textualize(party.name)] = oldCount < newCount ? newCount : oldCount;
    }
  }
}

void recordStatistics(Integer& sum64, Integer& count,
  vector<Integer>& histogram, vector<uint64_t>& histogramBounds,
  vector<Integer>& yearly, vector<uint8_t>& yearlyBounds,
  vector<SecureSubmission> secureSubmissions) {
  Integer zero = Integer(32, 0, PUBLIC);
  Integer one = Integer(32, 1, PUBLIC);

  for (const auto& sub : secureSubmissions) {
    count = count + one.select(sub.dup, zero);
    auto val = sub.amount.select(sub.dup, zero);
    val = val.resize(64);        
    sum64 = sum64 + val;

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

void recordCommitmentTokens(vector<SecureSubmission>& secureSubmissions,
                            vector<string>& commitment_tokens) {
  for (const auto& sub : secureSubmissions) {
    string s = textualize(sub.commitment_token);
    commitment_tokens.push_back(s);
  }
}

int process_submissions(int party, string prefix) {
  Integer zero = Integer(32, 0, PUBLIC);
  Integer one = Integer(32, 1, PUBLIC);

  // These files MUST be of the exact same size, line count, and format.
  string fileA = prefix + "/submission_bits_a.txt";
  string fileB = prefix + "/submission_bits_b.txt";

  string file = party == ALICE ? fileA : fileB;
  cout << "Reading from file: " << file << endl;

  try {
      vector<Submission> subA = loadSubmissionsFromFile(file);
      vector<Submission> subB = loadSubmissionsFromFile(file);
      vector<SecureSubmission> secureSubmissions;
      vector<SecureParty> secureParties;
      map<string, uint32_t> repeatedParties;
      vector<string> commitment_tokens;

      // Initial statistics setup
      Integer sum64 = Integer(64, 0, PUBLIC);
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

      // *** Begin processing

      xorReconstructSubmissions(subA, subB, secureSubmissions);

      vec_based::sort<SecureSubmission, SecureSubmission>(
        &(secureSubmissions), nullptr, false,
        &(bit_sort_by_settlement_key));

      markDupSubmissions(secureSubmissions);
      
      recordParties(secureSubmissions, secureParties);
      
      sortAndMarkRepeatParties(secureParties);
      
      unmaskRepeatParties(secureParties, repeatedParties);

      recordStatistics(sum64, count, histogram, histogramBounds,
                        yearly, yearlyBounds, secureSubmissions);

      recordCommitmentTokens(secureSubmissions, commitment_tokens);

      // *** Completed processing

      if(false) {
        printSubmissions(secureSubmissions);
        printParties(secureParties);
      }

      count.resize(64);
      Integer average = sum64 / count;

      cout << "Total count of " << count.reveal<uint64_t>()
            << ", the sum amount is $" << sum64.reveal<uint64_t>()
            << ", and average is $" << average.reveal<uint64_t>() << endl;


      cout << "Commitment token samples: ";

      for(size_t i = 0; i < 3; i++) {
        if(i >= commitment_tokens.size()) {
          break;
        }

        cout << commitment_tokens[i] << " ";
      }

      cout << endl;
      
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

      cout << "Repeated parties settled >= " << (UNMASK_REPEAT_THRESHOLD + 1) << endl;
      for(const auto& party : repeatedParties) {
        cout << "* Repeat party: " << party.first << " settlements: " <<
          party.second + 1 << endl;
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
