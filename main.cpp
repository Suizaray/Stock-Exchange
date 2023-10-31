#include <iostream>
#include <map>
#include <string>
#include <vector>
#include<sstream>

#include "exchange.hpp"
#include "utility.hpp"

int main() {
 Exchange e;
  std::ostringstream oss;
  e.MakeDeposit("Nahum", "BTC", 1000);
  e.MakeDeposit("Nahum", "USD", 100000);
  e.MakeDeposit("Dolson", "USD", 555555);
  e.MakeDeposit("Ofria", "ETH", 678);
  e.MakeDeposit("Zaabar", "USD", 12121212);
  e.MakeDeposit("Zaabar", "LTC", 4563);
  // BTC rough price 1000
  // ETH rough price 100
  // LTC rough price 500
  e.AddOrder({"Nahum", "Sell", "BTC", 5, 1100});
  e.AddOrder({"Nahum", "Sell", "BTC", 100, 1200});
  e.AddOrder({"Nahum", "Buy", "BTC", 7, 800});
  e.AddOrder({"Dolson", "Buy", "BTC", 1, 950});
  e.AddOrder({"Ofria", "Sell", "ETH", 12, 156});
  e.AddOrder({"Ofria", "Sell", "ETH", 10, 160});
  e.AddOrder({"Zaabar", "Sell", "LTC", 10, 550});
  e.AddOrder({"Zaabar", "Buy", "LTC", 10, 450});
  e.PrintUserPortfolios(std::cout);
  oss.str("");
  e.PrintUserPortfolios(oss);
  e.PrintUsersOrders(std::cout);
  oss.str("");
  e.PrintUsersOrders(oss);
  std::cout << std::endl << std::endl;
  e.AddOrder({"Nahum", "Buy", "LTC", 55, 600});
  e.AddOrder({"Nahum", "Buy", "ETH", 30, 158});
  e.AddOrder({"Ofria", "Sell", "ETH", 10, 140});
  e.AddOrder({"Zaabar", "Buy", "BTC", 10, 1500});
  e.AddOrder({"Zaabar", "Buy", "ETH", 200, 1200});
  e.AddOrder({"Nahum", "Sell", "BTC", 30, 158});
  e.AddOrder({"Dolson", "Buy", "BTC", 20, 2000});
  e.PrintTradeHistory(std::cout);
  oss.str("");
  e.PrintTradeHistory(oss);
  std::cout << std::endl;
  e.PrintUserPortfolios(std::cout);
  oss.str("");
  e.PrintUserPortfolios(oss);
  std::cout << std::endl;
  e.PrintUsersOrders(std::cout);
  oss.str("");
  e.PrintUsersOrders(oss);
  return 0;
}