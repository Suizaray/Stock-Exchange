#pragma once
#include <iostream>
#include <map>
#include <string>
#include <vector>

#include "utility.hpp"

class Exchange {
private:
  std::map<std::vector<std::string>, int> user; // database for users
  std::vector<std::vector<std::string>>
      trade_list_s; // string vector for the trade list
  std::vector<std::vector<int>> trade_list_i; // int vector for the trade list
  std::vector<Order> order_list;
  std::vector<Order> closed_order_list;

public:
  Exchange() = default;
  void MakeDeposit(const std::string &username, const std::string &asset,
                   int amount);
  void PrintUserPortfolios(std::ostream &os) const;
  bool MakeWithdrawal(const std::string &username, const std::string &asset,
                      int amount);
  bool AddOrder(const Order &order);
  void PrintUsersOrders(std::ostream &os) const;
  void PrintTradeHistory(std::ostream &os) const;
  void PrintBidAskSpread(std::ostream &os) const;
  // check for open trades and fufills all possible trades
  int CheckOrder(const std::string &username, const std::string &asset,
                 const std::string &side, int amount, int price);
  // std::string FindOrder(std::string, size_t&, std::vector<std::string>&)
  // const;
};
