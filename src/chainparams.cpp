// Copyright (c) 2010 Satoshi Nakamoto
// Copyright (c) 2009-2014 The Bitcoin developers
// Copyright (c) 2014-2015 The Dash developers
// Copyright (c) 2015-2018 The PIVX developers
// Copyright (c) 2018-2019 The CCBC developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "base58.h"
#include "chainparams.h"
#include "libzerocoin/Params.h"
#include "net.h"
#include "random.h"
#include "util.h"
#include "utilstrencodings.h"

#include <assert.h>

#include <boost/assign/list_of.hpp>

using namespace std;
using namespace boost::assign;

struct SeedSpec6 {
	uint8_t addr[16];
	uint16_t port;
};

#include "chainparamsseeds.h"

/**
* Main network
*/

//! Convert the pnSeeds6 array into usable address objects.
static void convertSeed6(std::vector<CAddress>& vSeedsOut, const SeedSpec6* data, unsigned int count)
{
	// It'll only connect to one or two seed nodes because once it connects,
	// it'll get a pile of addresses with newer timestamps.
	// Seed nodes are given a random 'last seen time' of between one and two
	// weeks ago.
	const int64_t nOneWeek = 7 * 24 * 60 * 60;
	for (unsigned int i = 0; i < count; i++) {
		struct in6_addr ip;
		memcpy(&ip, data[i].addr, sizeof(ip));
		CAddress addr(CService(ip, data[i].port));
		addr.nTime = GetTime() - GetRand(nOneWeek) - nOneWeek;
		vSeedsOut.push_back(addr);
	}
}

//   What makes a good checkpoint block?
// + Is surrounded by blocks with reasonable timestamps
//   (no blocks before with a timestamp after, none after with
//    timestamp before)
// + Contains no strange transactions
static Checkpoints::MapCheckpoints mapCheckpoints =
boost::assign::map_list_of

(0, uint256("0xa9f081734c579a25872ce366d5520482755d26ff3db73fbe7bdd4e973bc0e173"))
(100000, uint256("0x743cd780b4ab590106ca9855a90e3ce75eec935f2727e407bfe0777181e7e400"))
(150000, uint256("0xe2ab1c328e6445dd34ce2211b022bbe665a7e9c7173f1921660e7c8297c52565")); //When spork 18 was turned on

static const Checkpoints::CCheckpointData data = {
	&mapCheckpoints,
	1546789413, // * UNIX timestamp of last checkpoint block
	306249,     // * total number of transactions between genesis and last checkpoint
				//   (the tx=... number in the SetBestChain debug.log lines)
				2000        // * estimated number of transactions per day after checkpoint, was 2000
};

static Checkpoints::MapCheckpoints mapCheckpointsTestnet =
boost::assign::map_list_of
(0, uint256("0xa9f081734c579a25872ce366d5520482755d26ff3db73fbe7bdd4e973bc0e173"));
static const Checkpoints::CCheckpointData dataTestnet = {
	&mapCheckpointsTestnet,
	1534438799,
	0,
	250
};

static Checkpoints::MapCheckpoints mapCheckpointsRegtest =
boost::assign::map_list_of
(0, uint256("0xa9f081734c579a25872ce366d5520482755d26ff3db73fbe7bdd4e973bc0e173"));
static const Checkpoints::CCheckpointData dataRegtest = {
	&mapCheckpointsRegtest,
	1534438799,
	0,
	100
};


libzerocoin::ZerocoinParams* CChainParams::Zerocoin_Params() const
{
	assert(this);
	static CBigNum bnTrustedModulus(zerocoinModulus);
	static libzerocoin::ZerocoinParams ZCParams = libzerocoin::ZerocoinParams(bnTrustedModulus);

	return &ZCParams;
}

class CMainParams : public CChainParams
{
public:
	CMainParams()
	{
		networkID = CBaseChainParams::MAIN;
		vReviveRewardAddress = "SgMdQaxB7woK4dtudTCWcVizCYhBjzcfuv";   //This address will be depreicated when spork 18 is turned on and coins will be burned.
		vTreasuryRewardAddress = "SdkfmdG4b3uUt6cFZhXvbyPyAVM6GGmxr9"; //This address is held by Tfinch and will be paying out coins to team and funding of projects.
		strNetworkID = "main";
		/**
		* The message start string is designed to be unlikely to occur in normal data.
		* The characters are rarely used upper ASCII, not valid as UTF-8, and produce
		* a large 4-byte int at any alignment.
		*/
		pchMessageStart[0] = 0x1d;
		pchMessageStart[1] = 0xba;
		pchMessageStart[2] = 0xc2;
		pchMessageStart[3] = 0x45;
		vAlertPubKey = ParseHex("0427032f4aea9ade6b709caa6f302c0850e1ecdc9f4cd2619ef28dcd560afcc65bcd7a97bf58523a450a7c8e6e178c9ced2ed5ff96afd8e88284332a87f18af63f");
		nDefaultPort = 5520;
		bnProofOfWorkLimit = ~uint256(0) >> 1;
		nSubsidyHalvingInterval = 210000;
		nMaxReorganizationDepth = 100;
		nEnforceBlockUpgradeMajority = 750;
		nRejectBlockOutdatedMajority = 950;
		nToCheckBlockUpgradeMajority = 1000;
		nMinerThreads = 0;
		nTargetTimespan = 1 * 60;         // Ccbc: 1 day
		nTargetSpacing = 1 * 60;          // Ccbc: 1 minutes
		nMaturity = 10;                   // 5 Conf to mature coins
		nMasternodeCountDrift = 20;       //Was 20
		nMasternodeCollateralAmt = 25000; //masternode collateral 25K
		nMaxMoneyOut = 100000000 * COIN;  //100m coins minted

										  /** Height or Time Based Activations **/
		nLastPOWBlock = 200;
		nModifierUpdateBlock = 1; //Was 999999999
		nZerocoinStartHeight = 201;
		nAccumulatorStartHeight = 1;
		nZerocoinStartTime = 1534438799;
		nBlockEnforceSerialRange = 1;       //Enforce serial range starting this block
		nBlockRecalculateAccumulators = ~1; //Trigger a recalculation of accumulators
		nBlockFirstFraudulent = ~1;         //First block that bad serials emerged
		nBlockLastGoodCheckpoint = ~1;      //Last valid accumulator checkpoint

		const char* pszTimestamp = "National Suicide Prevention Hotline 1-800-273-8255";
		CMutableTransaction txNew;
		txNew.vin.resize(1);
		txNew.vout.resize(1);
		txNew.vin[0].scriptSig = CScript() << 486604799 << CScriptNum(4) << vector<unsigned char>((const unsigned char*)pszTimestamp, (const unsigned char*)pszTimestamp + strlen(pszTimestamp));
		txNew.vout[0].nValue = 50 * COIN;
		txNew.vout[0].scriptPubKey = CScript() << ParseHex("0431d81512c537c1e8ca5834732355740830cfe266d97ba8af2c0f72828745014f78931b94abd2383aa8ed2f8a1ec633dd902ed01edfe834f20374be4207843b43") << OP_CHECKSIG;
		genesis.vtx.push_back(txNew);
		genesis.hashPrevBlock = 0;
		genesis.hashMerkleRoot = genesis.BuildMerkleTree();
		genesis.nVersion = 1;
		genesis.nTime = 1534438799;
		genesis.nBits = 504365040;
		genesis.nNonce = 647688;

		nEnforceNewSporkKey = 1546300800; //!> Sporks signed after (GMT): Tuesday, Jan 1, 2018 12:00:00 AM GMT must use the new spork key
		nRejectOldSporkKey = 1548979200;  //!> Fully reject old spork key after (GMT): Friday, Feb 1, 2018 12:00:00 AM


		hashGenesisBlock = genesis.GetHash();

		//printf("genesis.GetHash = %s\n", genesis.GetHash().ToString().c_str());
		//printf("genesis.hashMerkleRoot = %s\n", genesis.hashMerkleRoot.ToString().c_str());
		assert(hashGenesisBlock == uint256("0xa9f081734c579a25872ce366d5520482755d26ff3db73fbe7bdd4e973bc0e173"));
		assert(genesis.hashMerkleRoot == uint256("0x598ffdff99e67551b7a05dc08dfafcf2e7a6ecea89f9b369bf830f4b21ff160f"));

		//Teams Seeder
		vSeeds.push_back(CDNSSeedData("seeder.ccbcoin.club", "seeder.ccbcoin.club"));

		base58Prefixes[PUBKEY_ADDRESS] = std::vector<unsigned char>(1, 63); //S
		base58Prefixes[SCRIPT_ADDRESS] = std::vector<unsigned char>(1, 68); //C
		base58Prefixes[SECRET_KEY] = std::vector<unsigned char>(1, 212);
		base58Prefixes[EXT_PUBLIC_KEY] = boost::assign::list_of(0x02)(0x2D)(0x27)(0x34).convert_to_container<std::vector<unsigned char> >();
		base58Prefixes[EXT_SECRET_KEY] = boost::assign::list_of(0x02)(0x21)(0x36)(0x2C).convert_to_container<std::vector<unsigned char> >();
		// BIP44 coin type is from https://github.com/satoshilabs/slips/blob/master/slip-0044.md
		base58Prefixes[EXT_COIN_TYPE] = boost::assign::list_of(0x80)(0x00)(0x00)(0x77).convert_to_container<std::vector<unsigned char> >();

		convertSeed6(vFixedSeeds, pnSeed6_main, ARRAYLEN(pnSeed6_main));

		fRequireRPCPassword = true;
		fMiningRequiresPeers = false;
		fAllowMinDifficultyBlocks = true;
		fDefaultConsistencyChecks = false;
		fRequireStandard = true;
		fMineBlocksOnDemand = false;
		fSkipProofOfWorkCheck = false;
		fTestnetToBeDeprecatedFieldRPC = false;
		fHeadersFirstSyncingActive = false;

		nPoolMaxTransactions = 3;

		strSporkKey = "0257aa5bdba5b2458ac8acb5eb80277cee5c69e21a55299b5d7ac506c49d958984";
		strSporkKeyOld = "030262a2a0679d01ab026d375c42b0a4122477123631b870e09665526c322c8899";

		strObfuscationPoolDummyAddress = "XCNAsFGy8k7amqRG26ikKyfVDwK8585Z6b";
		nStartMasternodePayments = 1534438799;

		/** Zerocoin */
		zerocoinModulus = "0xc95577b6dce0049b0a20c779af38079355abadde1a1d80c353f6cb697a7ae5a087bad39caa5798478551d0f9d91e6267716506f32412de1d19d17588765eb9502b85c6a18abdb05791cfd8b734e960281193705eeece210920cc922b3af3ceb178bf12c22eb565d5767fbf19545639be8953c2c38ffad41f3371e4aac750ac2d7bd614b3faabb453081d5d88fdbb803657a980bc93707e4b14233a2358c97763bf28f7c933206071477e8b371f229bc9ce7d6ef0ed7163aa5dfe13bc15f7816348b328fa2c1e69d5c88f7b94cee7829d56d1842d77d7bb8692e9fc7b7db059836500de8d57eb43c345feb58671503b932829112941367996b03871300f25efb5";
		nMaxZerocoinSpendsPerTransaction = 7; // Assume about 20kb each
		nMinZerocoinMintFee = 1 * ZCENT;      //high fee required for zerocoin mints
		nMintRequiredConfirmations = 20;      //the maximum amount of confirmations until accumulated in 19
		nRequiredAccumulation = 1;
		nDefaultSecurityLevel = 100;   //full security level for accumulators
		nZerocoinHeaderVersion = 4;    //Block headers must be this version once zerocoin is active
		nBudget_Fee_Confirmations = 6; // Number of confirmations for the finalization fee
	}

	const Checkpoints::CCheckpointData& Checkpoints() const
	{
		return data;
	}
};

std::string CChainParams::GetTreasuryRewardAddressAtHeight(int nHeight) const
{
	return vTreasuryRewardAddress;
}

CScript CChainParams::GetTreasuryRewardScriptAtHeight(int nHeight) const
{
	CBitcoinAddress address(GetTreasuryRewardAddressAtHeight(nHeight).c_str());
	assert(address.IsValid());
	CScript script = GetScriptForDestination(address.Get());
	return script;
}

std::string CChainParams::GetReviveRewardAddressAtHeight(int nHeight) const
{
	return vReviveRewardAddress;
}
CScript CChainParams::GetReviveRewardScriptAtHeight(int nHeight) const
{
	CBitcoinAddress address(GetReviveRewardAddressAtHeight(nHeight).c_str());
	assert(address.IsValid());
	CScript script = GetScriptForDestination(address.Get());
	return script;
}


static CMainParams mainParams;

class CTestNetParams : public CMainParams
{
public:
	CTestNetParams()
	{
		networkID = CBaseChainParams::TESTNET;
		strNetworkID = "test";
		pchMessageStart[0] = 0x1d;
		pchMessageStart[1] = 0xba;
		pchMessageStart[2] = 0xc2;
		pchMessageStart[3] = 0x45;
		vAlertPubKey = ParseHex("0427032f4aea9ade6b709caa6f302c0850e1ecdc9f4cd2619ef28dcd560afcc65bcd7a97bf58523a450a7c8e6e178c9ced2ed5ff96afd8e88284332a87f18af63f");
		nDefaultPort = 15521;
		nEnforceBlockUpgradeMajority = 750;
		nRejectBlockOutdatedMajority = 950;
		nToCheckBlockUpgradeMajority = 1000;
		nMinerThreads = 0;
		nTargetTimespan = 1 * 60; // Ccbc: 1 day
		nTargetSpacing = 1 * 60;  // Ccbc: 1 minute
		nLastPOWBlock = 200;
		nMaturity = 10;
		nMasternodeCountDrift = 4;
		nModifierUpdateBlock = 51197; //approx Mon, 17 Apr 2017 04:00:00 GMT
		nMaxMoneyOut = 100000000 * COIN;
		nZerocoinStartHeight = 201;
		nZerocoinStartTime = 1534438799;
		nBlockEnforceSerialRange = 1; //Enforce serial range starting this block
		nBlockRecalculateAccumulators = 9908000; //Trigger a recalculation of accumulators
		nBlockFirstFraudulent = 9891737; //First block that bad serials emerged
		nBlockLastGoodCheckpoint = 9891730; //Last valid accumulator checkpoint

											//! Modify the testnet genesis block so the timestamp is valid for a later start.
		genesis.nTime = 1534438799;
		genesis.nNonce = 647688;

		hashGenesisBlock = genesis.GetHash();
		//printf("genesis.GetHash = %s\n", genesis.GetHash().ToString().c_str());
		assert(hashGenesisBlock == uint256("0xa9f081734c579a25872ce366d5520482755d26ff3db73fbe7bdd4e973bc0e173"));
		assert(genesis.hashMerkleRoot == uint256("0x598ffdff99e67551b7a05dc08dfafcf2e7a6ecea89f9b369bf830f4b21ff160f"));

		vFixedSeeds.clear();
		vSeeds.clear();



		base58Prefixes[PUBKEY_ADDRESS] = std::vector<unsigned char>(1, 139); // Testnet ccbc addresses start with 'x' or 'y'
		base58Prefixes[SCRIPT_ADDRESS] = std::vector<unsigned char>(1, 19);  // Testnet ccbc script addresses start with '8' or '9'
		base58Prefixes[SECRET_KEY] = std::vector<unsigned char>(1, 239);     // Testnet private keys start with '9' or 'c' (Bitcoin defaults)
																			 // Testnet ccbc BIP32 pubkeys start with 'DRKV'
		base58Prefixes[EXT_PUBLIC_KEY] = boost::assign::list_of(0x3a)(0x80)(0x61)(0xa0).convert_to_container<std::vector<unsigned char> >();
		// Testnet ccbc BIP32 prvkeys start with 'DRKP'
		base58Prefixes[EXT_SECRET_KEY] = boost::assign::list_of(0x3a)(0x80)(0x58)(0x37).convert_to_container<std::vector<unsigned char> >();
		// Testnet ccbc BIP44 coin type is '1' (All coin's testnet default)
		base58Prefixes[EXT_COIN_TYPE] = boost::assign::list_of(0x80)(0x00)(0x00)(0x01).convert_to_container<std::vector<unsigned char> >();

		convertSeed6(vFixedSeeds, pnSeed6_test, ARRAYLEN(pnSeed6_test));

		fRequireRPCPassword = true;
		fMiningRequiresPeers = true;
		fAllowMinDifficultyBlocks = true;
		fDefaultConsistencyChecks = false;
		fRequireStandard = false;
		fMineBlocksOnDemand = false;
		fTestnetToBeDeprecatedFieldRPC = true;

		nPoolMaxTransactions = 2;
		strSporkKey = "030262a2a0679d01ab026d375c42b0a4122477123631b870e09665526c322c8899";
		strObfuscationPoolDummyAddress = "XCNAsFGy8k7amqRG26ikKyfVDwK8585Z6b";
		nStartMasternodePayments = 1534438799;
		nBudget_Fee_Confirmations = 3; // Number of confirmations for the finalization fee. We have to make this very short
									   // here because we only have a 8 block finalization window on testnet
	}
	const Checkpoints::CCheckpointData& Checkpoints() const
	{
		return dataTestnet;
	}
};
static CTestNetParams testNetParams;

class CRegTestParams : public CTestNetParams
{
public:
	CRegTestParams()
	{
		networkID = CBaseChainParams::REGTEST;
		strNetworkID = "regtest";
		strNetworkID = "regtest";
		pchMessageStart[0] = 0x1d;
		pchMessageStart[1] = 0xba;
		pchMessageStart[2] = 0xc2;
		pchMessageStart[3] = 0x45;
		nSubsidyHalvingInterval = 150;
		nEnforceBlockUpgradeMajority = 750;
		nRejectBlockOutdatedMajority = 950;
		nToCheckBlockUpgradeMajority = 1000;
		nMinerThreads = 1;
		nTargetTimespan = 1 * 60; // Ccbc: 1 day
		nTargetSpacing = 1 * 60;        // Ccbc: 1 minutes
		bnProofOfWorkLimit = ~uint256(0) >> 1;
		genesis.nTime = 1534438799;
		genesis.nBits = 504365040;
		genesis.nNonce = 647688;

		hashGenesisBlock = genesis.GetHash();
		nDefaultPort = 15222;
		//printf("genesis.GetHash = %s\n", genesis.GetHash().ToString().c_str());
		//printf("%s\n", hashGenesisBlock.ToString().c_str());
		assert(hashGenesisBlock == uint256("0xa9f081734c579a25872ce366d5520482755d26ff3db73fbe7bdd4e973bc0e173"));
		assert(genesis.hashMerkleRoot == uint256("0x598ffdff99e67551b7a05dc08dfafcf2e7a6ecea89f9b369bf830f4b21ff160f"));

		vFixedSeeds.clear(); //! Testnet mode doesn't have any fixed seeds.
		vSeeds.clear();      //! Testnet mode doesn't have any DNS seeds.

		fRequireRPCPassword = false;
		fMiningRequiresPeers = false;
		fAllowMinDifficultyBlocks = true;
		fDefaultConsistencyChecks = true;
		fRequireStandard = false;
		fMineBlocksOnDemand = true;
		fTestnetToBeDeprecatedFieldRPC = false;
	}
	const Checkpoints::CCheckpointData& Checkpoints() const
	{
		return dataRegtest;
	}
};
static CRegTestParams regTestParams;

class CUnitTestParams : public CMainParams, public CModifiableParams
{
public:
	CUnitTestParams()
	{
		networkID = CBaseChainParams::UNITTEST;
		strNetworkID = "unittest";
		nDefaultPort = 18334;
		vFixedSeeds.clear(); //! Unit test mode doesn't have any fixed seeds.
		vSeeds.clear();      //! Unit test mode doesn't have any DNS seeds.

		fRequireRPCPassword = false;
		fMiningRequiresPeers = false;
		fDefaultConsistencyChecks = true;
		fAllowMinDifficultyBlocks = false;
		fMineBlocksOnDemand = true;
	}

	const Checkpoints::CCheckpointData& Checkpoints() const
	{
		// UnitTest share the same checkpoints as MAIN
		return data;
	}

	//! Published setters to allow changing values in unit test cases
	virtual void setSubsidyHalvingInterval(int anSubsidyHalvingInterval) { nSubsidyHalvingInterval = anSubsidyHalvingInterval; }
	virtual void setEnforceBlockUpgradeMajority(int anEnforceBlockUpgradeMajority) { nEnforceBlockUpgradeMajority = anEnforceBlockUpgradeMajority; }
	virtual void setRejectBlockOutdatedMajority(int anRejectBlockOutdatedMajority) { nRejectBlockOutdatedMajority = anRejectBlockOutdatedMajority; }
	virtual void setToCheckBlockUpgradeMajority(int anToCheckBlockUpgradeMajority) { nToCheckBlockUpgradeMajority = anToCheckBlockUpgradeMajority; }
	virtual void setDefaultConsistencyChecks(bool afDefaultConsistencyChecks) { fDefaultConsistencyChecks = afDefaultConsistencyChecks; }
	virtual void setAllowMinDifficultyBlocks(bool afAllowMinDifficultyBlocks) { fAllowMinDifficultyBlocks = afAllowMinDifficultyBlocks; }
	virtual void setSkipProofOfWorkCheck(bool afSkipProofOfWorkCheck) { fSkipProofOfWorkCheck = afSkipProofOfWorkCheck; }
};

static CChainParams* pCurrentParams = 0;

const CChainParams& Params()
{
	assert(pCurrentParams);
	return *pCurrentParams;
}

CChainParams& Params(CBaseChainParams::Network network)
{
	switch (network) {
	case CBaseChainParams::MAIN:
		return mainParams;
	case CBaseChainParams::TESTNET:
		return testNetParams;
	case CBaseChainParams::REGTEST:
		return regTestParams;
	default:
		assert(false && "Unimplemented network");
		return mainParams;
	}
}

void SelectParams(CBaseChainParams::Network network)
{
	SelectBaseParams(network);
	pCurrentParams = &Params(network);
}

bool SelectParamsFromCommandLine()
{
	CBaseChainParams::Network network = NetworkIdFromCommandLine();
	if (network == CBaseChainParams::MAX_NETWORK_TYPES)
		return false;

	SelectParams(network);
	return true;
}