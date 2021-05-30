#include <eosio/eosio.hpp>

using namespace eosio;

TABLE pool_record {
  name host;
  name challenger;

  //we will use host as scope
  uint64_t primary_key() const {return challenger.value;}
  EOSLIB_SERIALIZE(pool_record, (host)(challenger))
};

typedef eosio::multi_index<name("pool"), pool_record> pool_index;

// The contract
CONTRACT tictactoe : public contract {
  public:
    // Use contract's constructor
    using contract::contract;

    ACTION welcome(name host, name opponent) {
        //action can only be taken by contract account
        require_auth(get_self());
        print("Welcome, ", host, " and ", opponent,"!");
    }

    ACTION create(name challenger, name host) {
        require_auth(host);
        check(host!=challenger, "Could not challenge youself!");
        //check(is_account(challenger),"challenger account not found");

        pool_index _pool(get_self(), host.value); //host as scope
        auto itr = _pool.find(challenger.value); //just find challenger in host

        check(itr==_pool.end(), "pair already exists!"); //if found

        //ram charge to action caller
        _pool.emplace(get_first_receiver(), [&](auto& pair) {
          pair.host = host;
          pair.challenger = challenger;
      });
    }

    ACTION close(name challenger, name host) {
        pool_index _pool(get_self(), host.value);
        auto itr = _pool.find(challenger.value);
        check(itr!=_pool.end(), "pair does not exists!"); //if not found
        _pool.erase(itr);
    }    

  private:

};
