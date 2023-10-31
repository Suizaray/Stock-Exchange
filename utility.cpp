#include "utility.hpp"
#include "exchange.hpp"
#include <iostream>
#include <sstream>
#include <string>

Order::Order(std::string user, std::string s, std::string as, int am, int p)
    : username(user), side(s), asset(as), amount(am), price(p) {}

Order::Order(const Order &o) {
  username = o.username;
  side = o.side;
  asset = o.asset;
  amount = o.amount;
  price = o.price;
}

bool Order::operator==(Order const &o) {
  if (username == o.username && side == o.side && asset == o.asset &&
      amount == o.amount && price == o.price) {
    return true;
  }
  return false;
}

std::ostream &operator<<(std::ostream &os, Order o) {
  os << o.side << " " << o.amount << " " << o.asset << " at " << o.price
     << " USD by " << o.username;
  return os;
}
