#include "exchange.hpp"
#include "utility.hpp"

#include <iostream>
#include <string>
using std::string;
#include <map>
using std::map;
#include <unordered_map>
using std::unordered_map;
#include <vector>
using std::vector;
#include <algorithm>

// used to fufill orders and update order list and trade list
int Exchange::CheckOrder(const string &username, const string &asset,
                         const string &side, int amount, int price) {
  int leftover = amount;
  while (leftover != 0) {
    if (side == "Buy") {
      int sell_amount = 0;
      int sell_price = 10000; // temp big number
      string sell_username = "";
      string sell_assest;
      string sell_side = "Sell";
      for (auto element : order_list) { // looking for lowest priced seller
        if (element.asset == asset && element.side == "Sell") {
          if (element.price <= price && element.price < sell_price) {
            sell_amount = element.amount;
            sell_price = element.price;
            sell_username = element.username;
            sell_assest = element.asset;
          }
        }
      }
      if (sell_username == "") { // break out of loop if no trade found
        return leftover;
        break;
      }
      // trade
      int trade_amount;
      if (sell_amount > leftover) {
        trade_amount = leftover;
      } else {
        trade_amount = sell_amount;
      }
      trade_list_s.push_back({username, sell_username, asset});
      trade_list_i.push_back({trade_amount, price});
      // maker order update
      auto o_iter_s = std::find_if(
          order_list.begin(), order_list.end(),
          [sell_username, asset, sell_side,
           sell_price](const Order &m) -> bool {
            return (m.username == sell_username && m.asset == asset &&
                    m.side == sell_side && m.price == sell_price);
          });
      int leftover_sell_amount =
          (*o_iter_s).amount - trade_amount; // for leftover in sell order
      if (leftover_sell_amount == 0) {       // updating sell order list
        closed_order_list.push_back(
            {sell_username, sell_side, sell_assest, trade_amount, price});
        order_list.erase(o_iter_s);
      } else {
        closed_order_list.push_back(
            {sell_username, sell_side, sell_assest, trade_amount, price});
        o_iter_s->amount -= trade_amount;
      }
      auto iter_s2 = user.find({sell_username, "USD"});
      if (iter_s2 == user.end()) { // make new USD asset for seller
        MakeDeposit(sell_username, "USD", (trade_amount * price));
      } else {
        iter_s2->second += (trade_amount * price);
      }
      // update user data base for the buyer
      auto iter_b1 = user.find({username, asset});
      if (iter_b1 == user.end()) { // make new crypto asset for buyer
        MakeDeposit(username, asset, trade_amount);
      } else {
        iter_b1->second += trade_amount;
      }
      // update order list for taker
      leftover -= trade_amount;
      if (leftover == 0) { // buyer order
        closed_order_list.push_back(
            {username, side, asset, trade_amount, price});
        return 0;
      } else {
        closed_order_list.push_back(
            {username, side, asset, trade_amount, price});
      }

    } else {
      int buy_amount;
      int buy_price = 0;
      string buy_username = "";
      string buy_assest;
      string buy_side = "Buy";
      for (auto element : order_list) { // looking for lowest priced seller
        if (element.asset == asset && element.side == "Buy") {
          if (element.price >= price && element.price > buy_price) {
            buy_amount = element.amount;
            buy_price = element.price;
            buy_username = element.username;
            buy_assest = element.asset;
          }
        }
      }
      if (buy_username == "") { // break out of loop if no trade found
        return leftover;
        break;
      }
      // trade
      int trade_amount;
      if (buy_amount > leftover) {
        trade_amount = leftover;
      } else {
        trade_amount = buy_amount;
      }

      trade_list_s.push_back({buy_username, username, asset});
      trade_list_i.push_back({trade_amount, price});
      // update user database for the seller

      leftover -= trade_amount; // for leftover in sell order

      auto iter_s2 = user.find({username, "USD"});
      if (iter_s2 == user.end()) { // make new USD asset for seller
        MakeDeposit(username, "USD", (trade_amount * price));
      } else {
        iter_s2->second += (trade_amount * price);
      }
      // update user data base for the buyer
      auto iter_b1 = user.find({buy_username, asset});
      if (iter_b1 == user.end()) { // make new crypto asset for buyer
        MakeDeposit(buy_username, asset, trade_amount);
      } else {
        iter_b1->second += trade_amount;
      }
      // update order list close/open if completed

      auto o_iter_b = std::find_if(
          order_list.begin(), order_list.end(),
          [buy_username, asset, buy_side, buy_price](const Order &m) -> bool {
            return (m.username == buy_username && m.asset == asset &&
                    m.side == buy_side && m.price == buy_price);
          });
      int leftover_buy_amount =
          (*o_iter_b).amount - trade_amount; // for leftover in buy order
      if (leftover_buy_amount == 0) {        // updating buy order list
        closed_order_list.push_back(
            {buy_username, buy_side, asset, trade_amount, price});
        order_list.erase(o_iter_b);
      } else {
        closed_order_list.push_back(
            {buy_username, buy_side, asset, trade_amount, price});
        o_iter_b->amount -= trade_amount;
      }
      if (leftover == 0) { // updating sell order list
        closed_order_list.push_back(
            {username, side, asset, trade_amount, price});
        return 0;
      } else {
        closed_order_list.push_back(
            {username, side, asset, trade_amount, price});
      }
    }
  }
  return 0;
}

void Exchange::MakeDeposit(const string &username, const string &asset,
                           int amount) {
  if (user.find({username, asset}) == user.end()) {
    user.insert({{username, asset}, amount});
  } else {
    auto iter = user.find({username, asset});
    iter->second += amount;
  }
}

void Exchange::PrintUserPortfolios(std::ostream &os) const {
  os << "User Portfolios (in alphabetical order):" << std::endl;
  vector<string> v_username;
  for (auto element : user) {
    string temp = element.first.at(0);
    auto it = std::find(v_username.begin(), v_username.end(), temp);
    if (it == v_username.end()) {
      v_username.push_back(temp);
    }
  }
  vector<string> v_asset;
  for (auto element : user) {
    string temp = element.first.at(1);
    auto it = std::find(v_asset.begin(), v_asset.end(), temp);
    if (it == v_asset.end()) {
      v_asset.push_back(temp);
    }
  }
  std::sort(v_asset.begin(), v_asset.end(), std::less_equal<>{});
  for (auto element : v_username) {
    os << element << "'s Portfolio: ";
    for (auto ele_asset : v_asset) {
      vector<string> temp_v{element, ele_asset};
      if (user.find(temp_v) != user.end()) {
        auto iter = user.find(temp_v);
        if (iter->second == 0) {
          continue;
        }
        os << iter->second << " " << ele_asset << ", ";
      }
    }
    os << std::endl;
  }
}

bool Exchange::MakeWithdrawal(const std::string &username,
                              const std::string &asset, int amount) {
  auto iter = user.find({username, asset});
  if (iter != user.end()) {
    if (iter->second >= amount) {
      iter->second -= amount;
      return true;
    }
  }
  return false;
}

bool Exchange::AddOrder(const Order &order) {

  if (order.side == "Buy") {
    auto iter = user.find({order.username, "USD"});
    if (iter != user.end()) {
      if (iter->second >= order.amount * order.price) {
        // take USD away from user
        iter->second -= order.amount * order.price;
        int x = CheckOrder(order.username, order.asset, order.side,
                           order.amount, order.price);
        if (x != 0) {
          order_list.push_back(
              {order.username, order.side, order.asset, x, order.price});
        }
        return true;
      }
    }
  } else {
    auto iter = user.find({order.username, order.asset});
    if (iter != user.end()) {
      if (iter->second >= order.amount) {
        // order_list_s.push_back({order.username, order.side, order.asset});
        // order_list_i.push_back({order.amount, order.price, 1});
        iter->second -= order.amount;
        int x = CheckOrder(order.username, order.asset, order.side,
                           order.amount, order.price);
        if (x != 0) {
          order_list.push_back(
              {order.username, order.side, order.asset, x, order.price});
        }
        return true;
      }
    }
  }
  return false;
}
// dead code
/* string Exchange::FindOrder(string name, size_t &pos, vector<string> &list)
const { string new_str = ""; for(size_t i = pos; i < order_list_s.size(); ++i) {
       string temp_str =  order_list_s[i][0];
       if(temp_str == name) {
           new_str = order_list_s[i][0];
           pos = i;
           break;
       }
       bool test = false;
       if(temp_str < name) {
           for(auto element : list) {
               if(temp_str == element) {
                   test = true;
                   break;
               }
           }
           if(test == true) {
               continue;
           }
           new_str = order_list_s[i][0];
           pos = i;
       }
   }

   if (new_str == "") {
       bool gb = true;
       pos = 0;
       string new_name = "ZZZZZZZZ";
       for(size_t i = pos; i < order_list_s.size(); ++i) {
           if(order_list_s[i][0] < new_name) {
               for(auto element : list) {
                   if(element == order_list_s[i][0]) {
                       gb = false;
                       break;
                   }
               }
               if(gb == true) {
                   new_str = order_list_s[i][0];
                   pos = i;
               }
               gb = true;
           }
       }
   }
   list.push_back(new_str);
   return new_str;
}

*/

void Exchange::PrintUsersOrders(std::ostream &os) const {
  vector<string> names;
  os << "Users Orders (in alphabetical order):" << std::endl;
  for (auto element : user) {
    if (std::find(names.begin(), names.end(), element.first.at(0)) ==
        names.end()) {
      names.push_back(element.first.at(0));
    }
  }
  std::sort(names.begin(), names.end(), std::less<>{});
  for (auto i : names) {
    os << i << "'s Open Orders (in chronological order):" << std::endl;
    for (auto element : order_list) {
      if (element.username == i) {
        os << element.side << " " << element.amount << " " << element.asset
           << " at " << element.price << " USD by " << element.username
           << std::endl;
      }
    }
    os << i << "'s Filled Orders (in chronological order):" << std::endl;
    for (auto element : closed_order_list) {
      if (element.username == i) {
        os << element.side << " " << element.amount << " " << element.asset
           << " at " << element.price << " USD by " << element.username
           << std::endl;
      }
    }
  }
}
void Exchange::PrintTradeHistory(std::ostream &os) const {
  int pos = 0;
  os << "Trade History (in chronological order):" << std::endl;
  for (auto element : trade_list_s) {
    os << element[0] << " Bought " << trade_list_i[pos][0] << " of "
       << element[2] << " From " << element[1] << " for "
       << trade_list_i[pos][1] << " USD" << std::endl;
    // os << "Buyer: " << element[0] << " Seller: " << element[1] << " Asset: "
    // << element[2] << " Amount: " << trade_list_i[pos][0] << " Price: " <<
    // trade_list_i[pos][1] << std::endl;
    ++pos;
  }
}

void Exchange::PrintBidAskSpread(std::ostream &os) const {
  os << "Asset Bid Ask Spread (in alphabetical order):" << std::endl;

  vector<string> v_of_assets;
  for (auto element : order_list) {
    string temp = element.asset;
    auto it = std::find(v_of_assets.begin(), v_of_assets.end(), temp);
    if (it == v_of_assets.end()) {
      v_of_assets.push_back(temp);
    }
  }
  std::sort(v_of_assets.begin(), v_of_assets.end(), std::less<>{});

  for (auto element : v_of_assets) {
    int lowest_sell = 10000;
    int highest_buy = 0;
    for (auto ele2 : order_list) {
      if (ele2.asset == element) {
        if (ele2.price > highest_buy && ele2.side == "Buy") {
          highest_buy = ele2.price;
        }
        if (ele2.price < lowest_sell && ele2.side == "Sell") {
          lowest_sell = ele2.price;
        }
      }
    }
    if (highest_buy == 0 && lowest_sell == 10000) {
      os << element
         << ": Highest Open Buy = NA USD and Lowest Open Sell = NA USD"
         << std::endl;
    } else if (highest_buy == 0) {
      os << element
         << ": Highest Open Buy = NA USD and Lowest Open Sell = " << lowest_sell
         << " USD" << std::endl;
    } else if (lowest_sell == 10000) {
      os << element << ": Highest Open Buy = " << highest_buy
         << " USD and Lowest Open Sell = NA USD" << std::endl;
    } else {
      os << element << ": Highest Open Buy = " << highest_buy
         << " USD and Lowest Open Sell = " << lowest_sell << " USD"
         << std::endl;
    }
  }
}