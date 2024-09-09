#include <iostream>
#include <fstream>
#include <string>
#include <bitset>
#include <stdexcept>
#include <sstream>
#include <vector>
#include <algorithm>
#include <cctype>

#include "emp-sh2pc/emp-sh2pc.h"
#include "emp-tool/utils/hash.h"
#include "emp-tool/circuits/number.h"

#include <boost/dynamic_bitset.hpp>

#include "utils.cpp"

using namespace std;

unsigned char reverseBits(unsigned char n) {
    unsigned char result = 0;
    for (int i = 0; i < 8; ++i) {
        result <<= 1;           // Shift the result to the left
        result |= (n & 1);      // Copy the least significant bit of n to the result
        n >>= 1;                // Shift n to the right
    }
    return result;
}

string textualize(Integer k) {
  boost::dynamic_bitset<> x(k.size());

  for (int i = 0; i < k.size(); i++) {
    x[i] = k[i].reveal<bool>(); // normal order
  }  

  std::string result;

  for(size_t k = 0; k < x.size()/8; k++) {
    std::bitset<8> byte;

    for(size_t i = 0; i < 8; i++) {
      byte[i] = x[k*8 + i];
    }

    unsigned char c = (unsigned char) byte.to_ulong();
    c = reverseBits(c);    
    result += c;
  }

  if(x.size() % 8 > 0) {
    result += "?"; // partial byte rendered as unknown
  }

  return result;
}

class Submission {
public:
    bitset<16> day_since_2000;
    bitset<320> party1;
    bitset<320> party2;
    bitset<32> amount;
    bitset<8> year_since_2000;
    bitset<64> signature_date;
    bitset<160> submitter;
    bitset<256> commitment_token;
    bitset<256> secret_hash;
    bitset<128> secret;

    static Submission parseFromLine(const string& line) {
        stringstream ss(line);
        string field;
        Submission sub;

        if (!parseField(ss, field, 16)) throw runtime_error("Field error in 'day_since_2000': " + errorDetails(field, 16));
        sub.day_since_2000 = bitset<16>(field);

        if (!parseField(ss, field, 320)) throw runtime_error("Field error in 'party1': " + errorDetails(field, 480));
        reverse(field.begin(), field.end()); // reverse string to put into bitset
        sub.party1 = bitset<320>(field);
        
        if (!parseField(ss, field, 320)) throw runtime_error("Field error in 'party2': " + errorDetails(field, 480));
        reverse(field.begin(), field.end()); // reverse string to put into bitset
        sub.party2 = bitset<320>(field);

        if (!parseField(ss, field, 32)) throw runtime_error("Field error in 'amount': " + errorDetails(field, 32));
        sub.amount = bitset<32>(field);

        if (!parseField(ss, field, 8)) throw runtime_error("Field error in 'year_since_2000': " + errorDetails(field, 8));
        sub.year_since_2000 = bitset<8>(field);

        if (!parseField(ss, field, 64)) throw runtime_error("Field error in 'signature_date': " + errorDetails(field, 64));
        sub.signature_date = bitset<64>(field);

        if (!parseField(ss, field, 160)) throw runtime_error("Field error in 'submitter': " + errorDetails(field, 160));
        reverse(field.begin(), field.end()); // reverse string to put into bitset
        sub.submitter = bitset<160>(field);

        if (!parseField(ss, field, 256)) throw runtime_error("Field error in 'commitment_token': " + errorDetails(field, 256));
        reverse(field.begin(), field.end()); // reverse string to put into bitset
        sub.commitment_token = bitset<256>(field);

        if (!parseField(ss, field, 256)) throw runtime_error("Field error in 'secret_hash': " + errorDetails(field, 256));
        reverse(field.begin(), field.end()); // reverse string to put into bitset
        sub.secret_hash = bitset<256>(field);

        if (!parseField(ss, field, 128)) throw runtime_error("Field error in 'secret': " + errorDetails(field, 128));
        reverse(field.begin(), field.end()); // reverse string to put into bitset
        sub.secret = bitset<128>(field);

        return sub;
    }

    static bool parseField(stringstream& ss, string& field, size_t expectedLength) {
        if (!getline(ss, field, ',') || field.length() != expectedLength) {
            return false;
        }
        return true;
    }

    static string errorDetails(const string& field, size_t expected) {
        return "Expected " + to_string(expected) + ", Got " + to_string(field.length()) + " '" + field + "'";
    }

    void xorWith(const Submission& other) {
        day_since_2000 ^= other.day_since_2000;
        party1 ^= other.party1;
        party2 ^= other.party2;
        amount ^= other.amount;
        year_since_2000 ^= other.year_since_2000;
        signature_date ^= other.signature_date;
        submitter ^= other.submitter;
        commitment_token ^= other.commitment_token;
        secret_hash ^= other.secret_hash;
        secret ^= other.secret;
    }

    string toString() const {
        stringstream ss;
        ss << day_since_2000.to_string().substr(16 - 16) << ','
            << party1.to_string().substr(480 - 480) << ','
            << party2.to_string().substr(480 - 480) << ','
           << amount.to_string().substr(32 - 32) << ','
           << year_since_2000.to_string().substr(8 - 8) << ','
           << signature_date.to_string().substr(64 - 64) << ','
           << submitter.to_string().substr(160 - 160) << ','
           << commitment_token.to_string().substr(256 - 256) << ','
           << secret_hash.to_string().substr(256 - 256) << ','
           << secret.to_string().substr(128 - 128);
        return ss.str();
    }
};

vector<Submission> loadSubmissionsFromFile(const string& filename) {
    ifstream file(filename);
    vector<Submission> submissions;
    if (!file.is_open()) {
        throw runtime_error("Cannot open file: " + filename);
    }

    string line;
    while (getline(file, line)) {
        line.erase(remove_if(line.begin(), line.end(), ::isspace), line.end());  // Remove all whitespace
        submissions.push_back(Submission::parseFromLine(line));
    }
    file.close();
    return submissions;
}

void writeSubmissionsToFile(const string& filename, const vector<Submission>& submissions) {
    ofstream file(filename);
    if (!file.is_open()) {
        throw runtime_error("Cannot open file: " + filename);
    }

    for (const auto& sub : submissions) {
        file << sub.toString() << '\n';
    }
    file.close();
}

vector<Submission> xorSubmissions(const vector<Submission>& subsA, const vector<Submission>& subsB) {
    if (subsA.size() != subsB.size()) {
        throw runtime_error("The number of submissions in each file does not match.");
    }

    vector<Submission> result;
    for (size_t i = 0; i < subsA.size(); ++i) {
        Submission temp = subsA[i];
        temp.xorWith(subsB[i]);
        result.push_back(temp);
    }
    return result;
}

template <size_t size> Integer makeInteger(const bitset<size>& bits, int party) {
  vector<Bit> secureBits;

  for (size_t i = 0; i < bits.size(); i++) {
    secureBits.push_back(Bit(bits[i], party));
  }

  return Integer(secureBits);
}

class SecureParty : public Swappable<SecureParty> {
public:
  Integer name;
  Bit invalid;
  Integer additional;

  SecureParty(Integer name, Bit invalid, Integer additional=Integer(32, 0, PUBLIC)) {
    this->name = name;
    this->invalid = invalid;
    this->additional = additional;
  }

  SecureParty select(const Bit & sel, const SecureParty& rhs) const {
    SecureParty nval(
      this->name.select(sel, rhs.name),
      this->invalid.select(sel, rhs.invalid),
      this->additional.select(sel, rhs.additional)
      );

    return nval;
  }

  Bit bit_equal(const SecureParty& rhs) const {
    return (this->name == rhs.name);
  }
};

class SecureSubmission : public Swappable<SecureSubmission> {
public:
  Integer day_since_2000;
  Integer party1;
  Integer party2;
  Integer amount;
  Integer year_since_2000;
  Integer signature_date;
  Bit dup;

  SecureSubmission(Integer day_since_2000, Integer party1, Integer party2,
                   Integer amount, Integer year_since_2000, Integer signature_date) {
    this->day_since_2000 = day_since_2000;
    this->party1 = party1;
    this->party2 = party2;

    this->amount = amount;
    this->year_since_2000 = year_since_2000;
    this->signature_date = signature_date;
  }

  SecureSubmission(Submission a, Submission b) {
    this->day_since_2000 = makeInteger(a.day_since_2000, ALICE) ^ makeInteger(b.day_since_2000, BOB);
    this->party1 = makeInteger(a.party1, ALICE) ^ makeInteger(b.party1, BOB);
    this->party2 = makeInteger(a.party2, ALICE) ^ makeInteger(b.party2, BOB);
    
    this->amount = makeInteger(a.amount, ALICE) ^ makeInteger(b.amount, BOB);
    this->year_since_2000 = makeInteger(a.year_since_2000, ALICE) ^ makeInteger(b.year_since_2000, BOB);
    this->signature_date = makeInteger(a.signature_date, ALICE) ^ makeInteger(b.signature_date, BOB);
  }

  SecureSubmission select(const Bit & sel, const SecureSubmission& rhs) const {
    SecureSubmission nval(
      this->day_since_2000.select(sel, rhs.day_since_2000),
      this->party1.select(sel, rhs.party1),
      this->party2.select(sel, rhs.party2),

      this->amount.select(sel, rhs.amount),
      this->year_since_2000.select(sel, rhs.year_since_2000),
      this->signature_date.select(sel, rhs.signature_date)
      );

    return nval;
  }

  Bit bit_equal(const SecureSubmission& rhs) const {
    return
      (this->day_since_2000 == rhs.day_since_2000) &
      (this->party1 == rhs.party1) &
      (this->party2 == rhs.party2) &
      (this->amount == rhs.amount) &
      (this->year_since_2000 == rhs.year_since_2000) &
      (this->signature_date == rhs.signature_date);
  }
};

Bit bit_sort_by_amount(SecureSubmission a, SecureSubmission b) {
  return a.amount < b.amount;
}

Bit bit_sort_by_settlement_key(SecureSubmission a, SecureSubmission b) {
  Bit party2_val = a.party2 < b.party2;
  Bit party1_val = a.party1 < b.party1;
  Bit party1_eq = a.party1 == b.party1;
  Bit day_since_2000_val = a.day_since_2000 < b.day_since_2000;
  Bit day_since_2000_eq = a.day_since_2000 == b.day_since_2000;

  Bit val = day_since_2000_val;
  val = val.select(day_since_2000_eq, party1_val);
  val = val.select(day_since_2000_eq & party1_eq, party2_val);

  return val;
}

Bit bit_sort_by_name(SecureParty a, SecureParty b) {
  return a.name < b.name;
}

string textualizeSub(SecureSubmission sub) {
  return std::to_string(sub.day_since_2000.reveal<uint32_t>()) + "/" +
    textualize(sub.party1) + "/" + textualize(sub.party2);
}

void printSubmissions(vector<SecureSubmission> secureSubmissions) {
  cout << "Secure submissions printout:" << endl;
  size_t printed = 0;

  for(const auto& sub : secureSubmissions) {
    cout <<
    " " << textualizeSub(sub) <<
    " $" << sub.amount.reveal<uint64_t>() << 
    " @Y" << sub.year_since_2000.reveal<uint64_t>() <<
    " dup: " << sub.dup.reveal<bool>() <<
    endl;

    printed++;

    if(printed > 10) {
      cout << "snip..." << endl;
      break;
    }
  }
}

void printParties(vector<SecureParty> secureParties) {
  cout << "Secure parties printout:" << endl;
  size_t printed = 0;

  for(const auto& party : secureParties) {
    cout << " " << textualize(party.name)
      << " additional: " << party.additional.reveal<uint32_t>()
      << " invalid: " << party.invalid.reveal<bool>() << endl;

    printed++;

    if(printed > 10) {
      cout << "snip..." << endl;
      break;
    }
  }
}

int xxmain(int argc, char* argv[]) {
    string fileA = "submission_bits_a.txt";
    string fileB = "submission_bits_b.txt";

    if (argc > 2) {
        fileA = argv[1];
        fileB = argv[2];
    }

    try {
        vector<Submission> subA = loadSubmissionsFromFile(fileA);
        vector<Submission> subB = loadSubmissionsFromFile(fileB);

        vector<Submission> xoredSubmissions = xorSubmissions(subA, subB);
        writeSubmissionsToFile("parsed_bits.txt", xoredSubmissions);
    } catch (const exception& e) {
        cerr << "Exception occurred: " << e.what() << endl;
        return 1;
    }

    return 0;
}