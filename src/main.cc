#include <cstdlib>
#include <iostream>

#include "cql/cql_server.hh"

using namespace cql;

int main(int argc, char *argv[]) {
  if (argc < 2) {
    std::cout << "usage: " << argv[0] << " <port>" << std::endl;
    exit(EXIT_SUCCESS);
  }

  uint16_t port = strtol(argv[1], NULL, 10);
  CQLServer server(port);
  server.start().join();

  return EXIT_SUCCESS;
}
