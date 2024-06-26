#include <did.admin.hpp>
#include <did.ntoken/did.ntoken.hpp>
#include <amax.token/amax.token.hpp>
#include<math.hpp>

#include <utils.hpp>

static constexpr eosio::name active_permission{"active"_n};
static constexpr eosio::name TOKEN  { "amax.token"_n };
static constexpr eosio::name DTOKEN { "did.ntoken"_n };
static constexpr eosio::symbol AMAX { symbol(symbol_code("AMAX"), 8) };

#define TRANSFER(bank, to, quantity, memo) \
    {	token::transfer_action act{ bank, { {_self, active_perm} } };\
			act.send( _self, to, quantity , memo );}


#define REBIND(bank, src, desc, quantity) \
    {	didtoken::rebind_action act{ bank, { {_self, active_perm} } };\
			act.send( src, desc, quantity );}

namespace amax {

using namespace std;

void did_admin::onpaytransfer(const name& from, const name& to, const asset& quant, const string& memo) {
   if (from == get_self() || to != get_self()) return;

   auto bank = get_first_receiver();
   CHECK( bank == TOKEN, "only accept AMAX for rebind payment" )
   CHECK( from != to, "cannot transfer to self" );
   CHECK( quant >= asset_from_string("0.10000000 AMAX"), "Please pay at least 0.1 AMAX in order to rebind your DID" )
   CHECK( memo != "",  "Expected memo format: 'rebind:${did_id}:${account}'" )

   auto parts                 = split( memo, ":" );
   CHECK( parts.size() == 3,  "Expected memo format: 'rebind:${did_id}:${account}'" )
   CHECK( parts[0] == "rebind", "memo string must start with rebind" )
   auto did_id                = to_uint64( parts[1], "Not a DID ID from did.ntoken" );
   auto dest                  = name( parts[2] );
   CHECK( is_account( dest ), "dest account does not exist: " + string(parts[2]) )

   auto burn_quant = asset_from_string("0.05000000 AMAX");   //0.05 AMAX
   TRANSFER( TOKEN, "oooo"_n, burn_quant, "did rebind" )

   auto did = nasset(did_id, 0, 1);
   REBIND( DTOKEN, from, dest, did )
}

} //namespace amax