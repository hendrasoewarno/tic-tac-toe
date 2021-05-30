#include <eosio/eosio.hpp>

using namespace eosio;

// The contract
CONTRACT tictactoe : public contract {
  public:

    TABLE pool_record {
      name host;
      name challenger;

      uint64_t primary_key() const {return host.value;}
      uint128_t secondary_key() const {return combine_ids(host.value,challenger.value);}
      EOSLIB_SERIALIZE(pool_record, (host)(challenger))
    };

    typedef eosio::multi_index<name("pool"), pool_record,
      eosio::indexed_by<name("poolskey"), eosio::const_mem_fun<pool_record, uint128_t, &pool_record::secondary_key>>
    > pool_index;

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

        pool_index _pool(get_self(), get_self().value);
        //find using secondary key
        auto _poolskey = _pool.get_index<name("poolskey")>(); //set secondary key
        auto itrh = _poolskey.find(combine_ids(host.value, challenger.value));
        auto itrc = _poolskey.find(combine_ids(challenger.value, host.value));
        check(itrh==_poolskey.end() && itrc==_poolskey.end(), "pair already exists!"); //if found
        
        //ram charge to action caller
        _pool.emplace(get_first_receiver(), [&](auto& pair) { 
          pair.host = host;
          pair.challenger = challenger;
        });
    }

    ACTION close(name challenger, name host) {
        //find by host (primary key)
        pool_index _pool(get_self(), get_self().value);
        //find using secondary key
        auto _poolskey = _pool.get_index<name("poolskey")>();
        auto itrc = _poolskey.find(combine_ids(host.value, challenger.value));          
        if (itrc!=_poolskey.end())
          _poolskey.erase(itrc);
        else {
          auto itrp = _poolskey.find(combine_ids(challenger.value, host.value));          
          if (itrp!=_poolskey.end())
            _poolskey.erase(itrp);
          else
            check(false, "pair not found!");
        }
    }    

  private:

    // concatenation of ids example
    static uint128_t combine_ids(const uint64_t &x, const uint64_t &y) {
      return (uint128_t{x} << 64) | y;
    }
};
