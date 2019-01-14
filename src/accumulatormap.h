// Copyright (c) 2015-2018 The PIVX developers
// Copyright (c) 2018-2019 The CCBC developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.
#ifndef CCBC_ACCUMULATORMAP_H
#define CCBC_ACCUMULATORMAP_H

#include "libzerocoin/Accumulator.h"
#include "libzerocoin/Coin.h"

//A map with an accumulator for each denomination
class AccumulatorMap
{
private:
    std::map<libzerocoin::CoinDenomination, std::unique_ptr<libzerocoin::Accumulator> > mapAccumulators;
public:
    AccumulatorMap();
    bool Load(uint256 nCheckpoint);
    bool Accumulate(libzerocoin::PublicCoin pubCoin, bool fSkipValidation = false);
    CBigNum GetValue(libzerocoin::CoinDenomination denom);
    uint256 GetCheckpoint();
    void Reset();
};
#endif //CCBC_ACCUMULATORMAP_H
