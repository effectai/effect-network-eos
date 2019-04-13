#pragma once

#include <eosiolib/eosio.hpp>
#include <eosiolib/print.hpp>
#include <eosiolib/types.h>
#include <eosiolib/asset.hpp>
#include <eosiolib/crypto.h>
#include <eosiolib/singleton.hpp>
#include <string>

using namespace eosio;

class [[eosio::contract("swap")]] swap : public contract {
 public:
  static const int64_t MIN_TX_VALUE = 0;
  static const int64_t MAX_TX_VALUE = 10000000000;

  swap(eosio::name receiver, eosio::name code, eosio::datastream<const char*> ds) :
    eosio::contract(receiver, code, ds), _nep5(_self, _self.value),
    _bookkeeper(_self, _self.value) {};

  [[eosio::action]]
    void init(name token_contract,
              symbol_code token_symbol,
              std::string issue_memo);

  [[eosio::action]]
    void posttx(name bookkeeper,
                std::vector<char> rawtx,
                name to,
                fixed_bytes<20> asset_hash,
                int64_t value);

  [[eosio::action]]
    void issue(checksum256 txid);

  [[eosio::action]]
    void mkbookkeeper(name account);

  [[eosio::action]]
    void rmbookkeeper(name account);

 private:
  capi_checksum256 neo_hash(const std::vector<char> data);

  struct [[eosio::table]] config {
    name token_contract;
    symbol_code token_symbol;
    std::string issue_memo;
  };

  struct [[eosio::table]] nep5 {
    uint64_t id;
    checksum256 txid;
    fixed_bytes<20> asset_hash;
    int64_t value;
    name to;
    uint64_t issued;
    uint64_t primary_key() const { return id; }
    checksum256 by_txid() const { return txid; }
  };

  struct [[eosio::table]] bookkeeper {
    name account;
    uint64_t primary_key() const { return account.value; }
  };

  typedef singleton<"config"_n, config> config_table;
  typedef multi_index<"nep5"_n, nep5, indexed_by<"txid"_n, const_mem_fun<nep5, checksum256, &nep5::by_txid>>> nep5_table;
  typedef multi_index<"bookkeeper"_n, bookkeeper> bookkeeper_table;

  nep5_table _nep5;
  bookkeeper_table _bookkeeper;
};