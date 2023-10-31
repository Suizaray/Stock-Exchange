#pragma once
#include <iostream>
#include <sstream>
#include <string>

struct Order {
  std::string username;
  std::string side; // Can be "Buy" or "Sell"
  std::string asset;
  int amount;
  int price;
  Order() = default;
  Order(std::string user, std::string s, std::string as, int am, int p);
  Order(Order const &);
  bool operator==(Order const &);

  friend std::ostream &operator<<(std::ostream &, Order);
};

struct Trade {
  std::string buyer_username;
  std::string seller_username;
  std::string asset;
  int amount;
  int price;
};
