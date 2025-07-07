#include "../src/core/database.hpp"

int main()
{
  DatabaseServer dbs("database/");
  if (dbs.start())
  {
    std::cout << "DB Server is running...\n";
    std::cout << "Press ENTER to stop.\n";
    std::cin.get();
    dbs.stop();
  }
  else
  {
    std::cerr << "Failed to start DB Server.\n";
  }
  return 0;
}
