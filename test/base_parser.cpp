#include <iostream>
#include <fstream>
#include <string>
#include <bitset>
#include <stdexcept>
#include <sstream>
#include <vector>
#include <algorithm>
#include <cctype>

using namespace std;

class Submission {
public:
    bitset<480> settlement_key;
    bitset<32> amount;
    bitset<8> year_since_1900;
    bitset<64> signature_date;
    bitset<160> submitter;
    bitset<256> commitment_token;
    bitset<256> secret_hash;
    bitset<128> secret;

    static Submission parseFromLine(const string& line) {
        stringstream ss(line);
        string field;
        Submission sub;

        if (!parseField(ss, field, 480)) throw runtime_error("Field error in 'settlement_key': " + errorDetails(field, 480));
        sub.settlement_key = bitset<480>(field);

        if (!parseField(ss, field, 32)) throw runtime_error("Field error in 'amount': " + errorDetails(field, 32));
        sub.amount = bitset<32>(field);

        if (!parseField(ss, field, 8)) throw runtime_error("Field error in 'year_since_1900': " + errorDetails(field, 8));
        sub.year_since_1900 = bitset<8>(field);

        if (!parseField(ss, field, 64)) throw runtime_error("Field error in 'signature_date': " + errorDetails(field, 64));
        sub.signature_date = bitset<64>(field);

        if (!parseField(ss, field, 160)) throw runtime_error("Field error in 'submitter': " + errorDetails(field, 160));
        sub.submitter = bitset<160>(field);

        if (!parseField(ss, field, 256)) throw runtime_error("Field error in 'commitment_token': " + errorDetails(field, 256));
        sub.commitment_token = bitset<256>(field);

        if (!parseField(ss, field, 256)) throw runtime_error("Field error in 'secret_hash': " + errorDetails(field, 256));
        sub.secret_hash = bitset<256>(field);

        if (!parseField(ss, field, 128)) throw runtime_error("Field error in 'secret': " + errorDetails(field, 128));
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
        settlement_key ^= other.settlement_key;
        amount ^= other.amount;
        year_since_1900 ^= other.year_since_1900;
        signature_date ^= other.signature_date;
        submitter ^= other.submitter;
        commitment_token ^= other.commitment_token;
        secret_hash ^= other.secret_hash;
        secret ^= other.secret;
    }

    string toString() const {
        stringstream ss;
        ss << settlement_key.to_string().substr(480 - 480) << ','
           << amount.to_string().substr(32 - 32) << ','
           << year_since_1900.to_string().substr(8 - 8) << ','
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

int main(int argc, char* argv[]) {
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