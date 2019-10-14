#include <map>
#include <set>
#include <list>
#include <cmath>
#include <ctime>
#include <deque>
#include <queue>
#include <stack>
#include <string>
#include <bitset>
#include <cstdio>
#include <limits>
#include <vector>
#include <climits>
#include <cstring>
#include <cstdlib>
#include <fstream>
#include <numeric>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <unordered_map>
#include <memory>

using namespace std;

class Order {
    public:
       Order() = delete;
       Order(const string &type,  const string &price, const string &quantity,
       const string &id, const bool buy_sell): _type(type),
       _price(stoi(price)), _quantity(stoi(quantity)), _id(move(id)),
       _buy_sell(buy_sell) {}
       Order(const string &id, bool cancel) : _id(move(id)), _cancel(cancel) {}
       Order(const string &price, const string &quantity,
       const string &id, const bool buy_sell): _price(stoi(price)),
       _quantity(stoi(quantity)), _id(move(id)), _buy_sell(buy_sell) {}

       bool _buy_sell ;
       bool _cancel ;
       int _quantity ;
       int _price ;
       string _id;
       string _type ;
};

class MatchingEngine {
    unordered_map<int, list<shared_ptr<Order>>> _order_map_buy; 
    unordered_map<int, list<shared_ptr<Order>>> _order_map_sell;
    unordered_map<int, int> _price_quantity_buy_map; 
    unordered_map<int, int> _price_quantity_sell_map ;
    unordered_map<string, pair<shared_ptr<Order>, list<shared_ptr<Order>>::iterator>> _orderIdmap;
    set<int, greater<int>> _buy_set;    // set of buy order prices 
    set<int, less<int>>    _sell_set;   // set of sell order prices 

    int trade_execution(shared_ptr<Order> &order, shared_ptr<Order> &curr, int &qty_traded) {
            if((order->_quantity - qty_traded) > curr->_quantity) {
                cout << "TRADE " << curr->_id << " " << curr->_price <<
                " " << curr->_quantity << " " << order->_id << " " <<
                order->_price << " " << curr->_quantity << endl;
                qty_traded += curr->_quantity;
                cancel(curr);
            }
            else {
                int curr_traded = order->_quantity - qty_traded;
                cout << "TRADE " << curr->_id << " " << curr->_price <<
                " " << curr_traded << " " << order->_id << " " <<
                order->_price << " " << curr_traded << endl;
                qty_traded += curr_traded;
                curr->_quantity -= curr_traded;
                if(!order->_buy_sell) _price_quantity_sell_map[curr->_price] -= curr_traded;
                else _price_quantity_buy_map[curr->_price] -= curr_traded;
                if(curr->_quantity == 0) cancel(curr);
            }
            return 0;
    }
    
    int trade(shared_ptr<Order> &order, int &quantity_traded) {
            int qty_traded = 0;
            if(!order->_buy_sell) { // buy order
                while(!_sell_set.empty()) {
                    auto it = _sell_set.begin();
                    if (*it > order->_price || qty_traded == order->_quantity) break;
                    while(_order_map_sell[*it].size() > 0 && qty_traded < order->_quantity) {
                        shared_ptr<Order> curr = *(_order_map_sell[*it].begin());
                        trade_execution(order, curr, qty_traded);
                    }
                }
            }
            else { // sell order
                while(!_buy_set.empty()) {
                    auto it = _buy_set.begin();
                    if (*it < order->_price || qty_traded == order->_quantity) break;
                    while(_order_map_buy[*it].size() > 0 && qty_traded < order->_quantity) {
                        shared_ptr<Order> curr = *(_order_map_buy[*it].begin());
                        trade_execution(order, curr, qty_traded);
                    }
                }
            }
            quantity_traded = qty_traded;
            return 0;
    }

    int tradeIOC(shared_ptr<Order> &order) {
            int qty_traded = 0;

            if(_orderIdmap.find(order->_id) != _orderIdmap.end()) return -1;

            trade(order, qty_traded);

            return 0;
    }
    
    int buy (shared_ptr<Order> &order) {
            int qty_traded = 0;
        
            while(!order->_id.empty() && isspace(*order->_id.rbegin()))
                order->_id.erase(order->_id.length()-1);
        
            if(order->_price <= 0 ||  
               order->_quantity <= 0 || order->_id.size() == 0) return -1;
        
            if(order->_type == "IOC") {
                tradeIOC(order);
                return 0;
            }

            if(_orderIdmap.find(order->_id) != _orderIdmap.end()) return -1;

            if(_order_map_buy.find(order->_price) == _order_map_buy.end()) {
                _buy_set.insert(order->_price);
            }
            _price_quantity_buy_map[order->_price] += order->_quantity;
            _order_map_buy[order->_price].push_back(order);
            auto it = _order_map_buy[order->_price].end() ;
            it--;
            _orderIdmap[order->_id] = make_pair(order, it);

            trade(order,  qty_traded);
            order->_quantity -= qty_traded;
            _price_quantity_buy_map[order->_price] -= qty_traded;
            if(order->_quantity == 0)
                cancel(order);

            return 0;
    }
    
    int sell (shared_ptr<Order> &order) {
            int qty_traded = 0;    

            while(!order->_id.empty() && std::isspace(*order->_id.rbegin()))
                order->_id.erase(order->_id.length()-1);
        
            if(order->_price <= 0 || 
               order->_quantity <= 0 || order->_id.size() == 0) return -1;
        
            if(order->_type == "IOC") {
                tradeIOC(order);
                return 0;
            }

            if(_orderIdmap.find(order->_id) != _orderIdmap.end()) return -1;

            if(_order_map_sell.find(order->_price) == _order_map_sell.end()) {
                _sell_set.insert(order->_price);
            }
            _price_quantity_sell_map[order->_price] += order->_quantity;
            _order_map_sell[order->_price].push_back(order);
            auto it = _order_map_sell[order->_price].end() ;
            it--;
            _orderIdmap[order->_id] = make_pair(order, it);

            trade(order,  qty_traded);
            order->_quantity -= qty_traded;
            _price_quantity_sell_map[order->_price] -= qty_traded;
            if(order->_quantity == 0)
                cancel(order);

            return 0;
    }
    
    int modify (shared_ptr<Order> &order) {
            if(_orderIdmap.find(order->_id) == _orderIdmap.end()) return -1;
            cancel(order);

            if(!order->_buy_sell)
                buy(order);
            else
                sell(order);
            return 0;
    }

    int cancel (shared_ptr<Order> &order) {
            if(_orderIdmap.find(order->_id) == _orderIdmap.end()) return -1;
            shared_ptr<Order> existing_order = _orderIdmap[order->_id].first;
            auto orderit = _orderIdmap[order->_id].second;
            _orderIdmap.erase(order->_id);
            if(!existing_order->_buy_sell) { //buy order
                _price_quantity_buy_map[existing_order->_price] -= existing_order->_quantity;
                _order_map_buy[existing_order->_price].erase(orderit);
                if(_order_map_buy[existing_order->_price].size() == 0) {
                    _buy_set.erase(existing_order->_price);
                    _price_quantity_buy_map.erase(existing_order->_price);
                    _order_map_buy.erase(existing_order->_price);
                }
            }
            else {
                _price_quantity_sell_map[existing_order->_price] -= existing_order->_quantity;
                _order_map_sell[existing_order->_price].erase(orderit);
                if(_order_map_sell[existing_order->_price].size() == 0) {
                    _sell_set.erase(existing_order->_price);
                    _price_quantity_sell_map.erase(existing_order->_price);
                    _order_map_sell.erase(existing_order->_price);
                }
            }
            return 0;
    }    
    
    int print () {
            cout << "SELL:" << endl;
            for(auto rit = _sell_set.rbegin(); rit != _sell_set.rend(); rit++) {
                int qty = _price_quantity_sell_map[*rit];
                cout << *rit << " " << qty << endl;
            }
            cout << "BUY:" << endl;
            for(auto it = _buy_set.begin(); it != _buy_set.end(); it++) {
                int qty = _price_quantity_buy_map[*it];
                cout << *it << " " << qty << endl;
            }
            return 0;
    }

    int parse(const string &line, vector<string> &tokens) {
            size_t start = 0, end = 0;
            while ((end = line.find(' ', start)) != string::npos) {
                tokens.push_back(move(line.substr(start, end - start)));
                start = end + 1;
            }
            tokens.push_back(move(line.substr(start)));
            return 0;
    }
    
    public:
        MatchingEngine() {}
    
          int process(const string &line) {
              
            vector<string> tokens;
            parse(line, tokens);
              
            if(tokens[0] == "BUY")    {
                shared_ptr<Order> order = make_shared<Order>(tokens[1], tokens[2], tokens[3], tokens[4], 0);
                buy(order);
                return 0;
            }
            if(tokens[0] == "SELL")    {
                shared_ptr<Order> order = make_shared<Order>(tokens[1], tokens[2], tokens[3], tokens[4], 1);
                sell(order);
                return 0;
            }
            if(tokens[0] == "MODIFY")    {
                shared_ptr<Order> order;
                if(tokens[2] == "BUY")
                    order = make_shared<Order>(tokens[3], tokens[4], tokens[1], 0);
                else
                    order = make_shared<Order>(tokens[3], tokens[4], tokens[1], 1);
                modify(order);
                return 0;
            }
            if(tokens[0] == "CANCEL")    {
                shared_ptr<Order> order = make_shared<Order>(tokens[1], 1);
                cancel(order);
                return 0;
            }
            if(tokens[0] == "PRINT")  {
                print();
                return 0;
            }
            return -1;
    }

};
    
int main() {
    ios_base::sync_with_stdio(false);
    char buffer[1024];
    cin.rdbuf()->pubsetbuf(buffer, sizeof(buffer));
    MatchingEngine mEngine;
    for (string line; getline(cin >> ws, line);) {
        mEngine.process(line);
    }
    return 0;
}
