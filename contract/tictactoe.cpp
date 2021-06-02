#include <eosio/eosio.hpp>
#include <vector>

using namespace eosio;
using namespace std;

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

    TABLE game_record {
      uint64_t id;
      vector<std::uint8_t> board{0,0,0,0,0,0,0,0,0};
      name challenger;
      name host;
      name turn;
      name winner;

      bool is_empty_cell(uint16_t row, uint16_t col) {return (board[row*3+col]==0);}
      bool is_valid_movement(name by, uint16_t row, uint16_t col) {
        if (winner==name() && by==turn && is_empty_cell(row, col) ) {
          uint8_t set=1;
          if (by==host) {
            board[row*3+col]=set;
            turn=challenger;
          }
          else {
            set=2;
            board[row*3+col]=set;
            turn=host;
          }
          
          uint8_t test=((board[row*3+0]&board[row*3+1]&board[row*3+2]) 
              |(board[0*3+col]&board[1*3+col]&board[2*3+col])
              |(board[0]&board[4]&board[8])
              |(board[2]&board[4]&board[6]));
        
          if (test==set)
            winner=by;

          return true;
        }
        return false;
      }  

      uint64_t primary_key() const {return id;}
      uint128_t secondary_key() const {return combine_ids(host.value,challenger.value);}
      EOSLIB_SERIALIZE(game_record, (id)(board)(challenger)(host)(turn)(winner))
    };

    typedef eosio::multi_index<name("game"), game_record,
      eosio::indexed_by<name("gameskey"), eosio::const_mem_fun<game_record, uint128_t, &game_record::secondary_key>>
    > game_index;

    TABLE leader_board {
      name winner;
      uint32_t count;
      uint64_t primary_key() const {return winner.value;}
      uint32_t secondary_key() const {return -count;}
      EOSLIB_SERIALIZE(leader_board, (winner)(count))
    };

    typedef eosio::multi_index<name("leader"), leader_board,
      eosio::indexed_by<name("leaderskey"), eosio::const_mem_fun<leader_board, uint32_t, &leader_board::secondary_key>>
    > leader_index;    

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

    ACTION restart(name challenger, name host, name by) {      
      check(has_auth(host) || has_auth(challenger), "please auth yourself!");
      check(by==challenger || by==host, "only challenger or host can restart game!");
      check(game_exists(challenger, host), "game not found!");
      game_index _game(get_self(), get_self().value);
      auto _gameskey = _game.get_index<name("gameskey")>();
      auto itr = _gameskey.find(combine_ids(host.value, challenger.value));          
      if (itr==_gameskey.end()) {
        //ram charge to same_payer
        _game.emplace(get_first_receiver(), [&](auto& game) { 
          game.id = _game.available_primary_key();
          //fill(game.board.begin(), game.board.end(), 0);
          game.challenger=challenger;
          game.host=host;
          game.turn=by;
          game.winner=name();
        });
      } else {
        _game.modify(*itr, same_payer, [&]( auto& game ) {          
          fill(game.board.begin(), game.board.end(), 0);   
          game.turn=by;
          game.winner=name();
        });
      }
    }

    ACTION move(name challenger, name host, name by, uint16_t row, uint16_t col) {
      check(has_auth(host) || has_auth(challenger), "please auth yourself!");
      check(by==challenger || by==host, "only challenger or host can restart game!");
      check(row<3 && col < 3, "invalid row or col!");
      check(game_exists(challenger, host), "game not found!");
      game_index _game(get_self(), get_self().value);
      auto _gameskey = _game.get_index<name("gameskey")>();
      auto itr = _gameskey.find(combine_ids(host.value, challenger.value));
      check(itr!=_gameskey.end(), "restart game first!");
      check((*itr).winner==name(), "game over!");
      _game.modify(*itr, same_payer, [&]( auto& game ) {
        check(game.is_valid_movement(by, row, col), "invalid movement!");  
      });
      if ((*itr).winner!=name()) {
        leader_index _leader(get_self(), get_self().value);
        auto itrl = _leader.find((*itr).winner.value);
        if (itrl == _leader.end()) {
          _leader.emplace(get_first_receiver(), [&](auto& leader) { 
            leader.winner=(*itr).winner;
            leader.count=1;
          });
        }
        else {
          _game.modify(*itrl, same_payer, [&]( auto& leader) {          
            leader.count++;
          });
        }
      }
    }

    ACTION close(name challenger, name host) {
        //find by host (primary key)
        check(has_auth(host) || has_auth(challenger), "please auth yourself!");
        pool_index _pool(get_self(), get_self().value);
        //find using secondary key
        auto _poolskey = _pool.get_index<name("poolskey")>();
        auto itrc = _poolskey.find(combine_ids(host.value, challenger.value));          
        if (itrc!=_poolskey.end())
          _poolskey.erase(itrc);

        //erase child table
        game_index _game(get_self(), get_self().value);
        auto _gameskey = _game.get_index<name("gameskey")>();
        auto itrgc = _gameskey.find(combine_ids(host.value, challenger.value));          
        if (itrgc!=_gameskey.end())
          _gameskey.erase(itrgc);

        else {
          auto itrp = _poolskey.find(combine_ids(challenger.value, host.value));          
          if (itrp!=_poolskey.end())
            _poolskey.erase(itrp);

          //erase child table
          auto itrgp = _gameskey.find(combine_ids(challenger.value, host.value));
          if (itrgp!=_gameskey.end())
            _gameskey.erase(itrgp);            

          else
            check(false, "pair not found!");
        }

    }    

  private:
    // concatenation of ids example
    static uint128_t combine_ids(const uint64_t &x, const uint64_t &y) {
      return (uint128_t{x} << 64) | y;
    }

    bool game_exists(name challenger, name host) {
      pool_index _pool(get_self(), get_self().value);
      auto _poolskey = _pool.get_index<name("poolskey")>();
      auto itrc = _poolskey.find(combine_ids(host.value, challenger.value));  
      return (itrc!= _poolskey.end());
    }
};
