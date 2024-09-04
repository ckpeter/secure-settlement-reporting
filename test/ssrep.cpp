
#include <iostream>

#include <map>

#include "emp-sh2pc/emp-sh2pc.h"
#include "emp-tool/utils/hash.h"
#include "emp-tool/circuits/number.h"

#include "parser.cpp"

using namespace std;

void mpc_run() {
  auto a = Integer(32, 5, ALICE);
  auto b = Integer(32, 55555, BOB);
  auto c = Integer(32, 100, PUBLIC);
  auto sum = a + b + c;

  cout << "Hello! The sum is " << sum.reveal<int>() << endl;
}

int main(int argc, char** argv) {
  if(argc < 2) {
    cerr << "You need to pass in party!\n" <<
    "Usage: prog alice_1_or_bob_2 [port=9999] [addr=127.0.0.1]" << endl;
    exit(1);
  }

  int party = atoi(argv[1]);
  int port = 9999;

  if(argc >= 3) {
    port = atoi(argv[2]);
  }

  const char* addr = argc >= 4 ? argv[3] : "127.0.0.1";
  addr = party == ALICE ? nullptr : addr;

  if(addr == nullptr) {
    cout << "Alice listening for Bob on port " << port << "...\n";
  }

  NetIO * io = new NetIO(addr, port);
  setup_semi_honest(io, party);
  auto start = std::chrono::system_clock::now();
  

  // ***** Run program logic
  cout << "\n===== Program running...\n";
  
  parser_run(party);
  // mpc_run();
  
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
