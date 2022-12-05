#include "redpackdb.hpp"
#include <aplink.farm/wasm_db.hpp>

using namespace std;
using namespace amax;
using namespace wasm::db;

#define CHECKC(exp, code, msg) \
   { if (!(exp)) eosio::check(false, string("$$$") + to_string((int)code) + string("$$$ ") + msg); }

static constexpr name AMAX_TOKEN{"amax.token"_n};

enum class err: uint8_t {
   INVALID_FORMAT       = 0,
   TYPE_INVALID         = 1,
   FEE_NOT_FOUND        = 2,
   QUANTITY_NOT_ENOUGH  = 3,
   NOT_POSITIVE         = 4,
   SYMBOL_MISMATCH      = 5,
   EXPIRED              = 6, 
   PWHASH_INVALID       = 7,
   RECORD_NO_FOUND      = 8,
   NOT_REPEAT_RECEIVE   = 9,
   NOT_EXPIRED          = 10,
   ACCOUNT_INVALID      = 11,
   FEE_NOT_POSITIVE     = 12,
   VAILD_TIME_INVALID   = 13,
   MIN_UNIT_INVALID     = 14,
   RED_PACK_EXIST       = 15,
   DID_NOT_AUTH         = 16,
   UNDER_MAINTENANCE    = 17,
   TOO_MANY_TYPES       = 18,
   PARAM_ERROR          = 19,
   STATUS_ERROR         = 20
};

enum class redpack_type: uint8_t {
   RANDOM       = 0,
   MEAN         = 1,
   DID_RANDOM   = 10,
   DID_MEAN     = 11
   
};

class [[eosio::contract("nft.redpack")]] redpack: public eosio::contract {
private:
    dbc                 _db;
    global_singleton    _global;
    global_t            _gstate;

public:
    using contract::contract;

    redpack(eosio::name receiver, eosio::name code, datastream<const char*> ds):
        _db(_self),
        contract(receiver, code, ds),
        _global(_self, _self.value)
    {
        _gstate = _global.exists() ? _global.get() : global_t{};
    }

    ~redpack() {
        _global.set( _gstate, get_self() );
    }


    //[[eosio::action]]
    [[eosio::on_notify("amax.token::transfer")]] void feetransfer(name from, name to, asset quantity, string memo);
    
    [[eosio::on_notify("*::transfer")]] void ontransfer( const name& from, const name& to, const vector<nasset>& assets, const string& memo  );

    [[eosio::action]] void claimredpack( const name& claimer, const name& code, const string& pwhash );

    [[eosio::action]] void cancel( const name& code );

    [[eosio::action]] void addfee( const nsymbol& nft_coin, const asset& fee, const name& fee_contract, const name& redpack_contract);

    [[eosio::action]] void delfee( const nsymbol& coin );

    [[eosio::action]] void setconf( const name& admin, const uint16_t& hours, const bool& enable_did );

    [[eosio::action]] void delredpacks( const name& code );

    asset _calc_fee(const asset& fee, const uint64_t count);

private:

}; //contract redpack