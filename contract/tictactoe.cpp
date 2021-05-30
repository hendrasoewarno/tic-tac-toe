#include <eosio/eosio.hpp>

using namespace eosio;

// The contract
CONTRACT tictactoe : public contract {
  public:

    TABLE pool_record {
      name host;
      name challenger;

      //we will use host as scope
      uint64_t primary_key() const {return challenger.value;}
      EOSLIB_SERIALIZE(pool_record, (host)(challenger))
    };
    typedef eosio::multi_index<name("pool"), pool_record> pool_index;

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

        //find host in challenger scope
        pool_index _poolc(get_self(), challenger.value); //challenger as scope
        auto itrc = _poolc.find(host.value); //just find host in challenger
        check(itrc==_poolc.end(), "pair already exists!"); //if found

        //find challenger in host scope
        pool_index _poolh(get_self(), host.value); //host as scope
        auto itrh = _poolh.find(challenger.value); //just find challenger in host
        check(itrh==_poolh.end(), "pair already exists!"); //if found        

        //ram charge to action caller
        _poolh.emplace(get_first_receiver(), [&](auto& pair) {
          pair.host = host;
          pair.challenger = challenger;
      });
    }

    ACTION close(name challenger, name host) {
        //find challenger in host scope
        pool_index _poolh(get_self(), host.value);
        auto itrh = _poolh.find(challenger.value);
        if (itrh!=_poolh.end())
          _poolh.erase(itrh);
        else {
          //find host in challenger scope
          pool_index _poolc(get_self(), challenger.value);
          auto itrc = _poolc.find(host.value);
          if (itrc!=_poolc.end())
            _poolc.erase(itrc);
          else
            check(false, "pair not found!");

        }
    }    

  private:

};
