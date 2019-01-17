<hr />
<div align="center">
    <img src="https://ccbcoin.club/img/headerccb.png" alt="Logo" width='300px' height='auto'/>
</div>
<hr />

[![Build Status](https://travis-ci.org/SubiPlatform/subi.svg?branch=master)](https://travis-ci.org/CryptoCashBack-Hub/CCBC) <a href="https://discord.gg/7z4wmzp"><img src="https://discordapp.com/api/guilds/454464504290476033/embed.png" alt="Discord server" /><a href="https://twitter.com/CCBcoin"><img src="https://img.shields.io/twitter/follow/CCBcoin.svg?style=social&logo=twitter" alt="follow on Twitter"></a>

[Website](https://ccbcoin.club/) — [Wallet](https://github.com/CryptoCashBack-Hub/CCBC/releases/) — [Block Explorer](https://explorer.ccbcoin.club/) — [Discord](https://discord.gg/7z4wmzp) — [Forum](https://bitcointalk.org/index.php?topic=5089897.0) — [Telegram](https://t.me/CCBCofficial) — [Twitter](https://twitter.com/CCBcoin)


### CCBC is a fork of [PIVX](https://github.com/PIVX-Project/PIVX) that forked [Dash](https://github.com/dashpay/dash) that forked [Bitcoin](https://github.com/bitcoin/bitcoinp)


# Ccbc Core integration/staging repository


### Coin Specs
<table>
<tr><td>Algo</td><td>Quark</td></tr>
<tr><td>Block Time</td><td>60 Seconds</td></tr>
<tr><td>Difficulty Retargeting</td><td>Every Block</td></tr>
<tr><td>Max Coin Supply </td><td>100,000,000 CCBC</td></tr>
<tr><td>Premine</td><td>8,000,000 CCBC</td></tr>
<tr><td>MN Port</td><td>5520</td></tr>
<tr><td>RPC Port</td><td>15520</td></tr>
</table>


### Reward Distribution Per Block

<table>
<th colspan=4>PoW Phase</th>
<tr><th>Block Height</th><th>Reward Amount</th><th>Notes</th><th>Duration (Days)</th></tr>
<tr><td>0</td><td>50 CCBC</td><td>Genesis Block</td><td>0 Days</td></tr>
<tr><td>1</td><td>8,000,000 CCBC</td><td>Initial Premine</td><td>0 Days</td></tr>
<tr><td>2-200</td><td>0 CCBC</td><td rowspan=1>Closed Mining</td><td>1 Days</td></tr>
<tr><th colspan=4>PoS / MN Phase</th></tr>
<tr><td>200-25000</td><td>1 CCBC</td><td rowspan=1>Coin Swap</td><td>17.22 Days</td></tr>
<tr><td>25000-50000</td><td>25 CCBC</td><td rowspan=1>First Increase </td><td>17.36 Days</td></tr>
<tr><td>50000-75000</td><td>50 CCBC</td><td rowspan=1>Second Increase </td><td>17.36 Days</td></tr>
<tr><td>75000-100000</td><td>85 CCBC</td><td rowspan=1>Final Increase </td><td>17.36 Days</td></tr>
<tr><td>100000-125000</td><td>75 CCBC</td><td rowspan=1>First Reduction </td><td>17.36 Days</td></tr>
<tr><td>125000-168000</td><td>50 CCBC</td><td rowspan=1>Second Reduction </td><td>30 Days</td></tr>
<tr><td>168000-297600</td><td>25 CCBC</td><td rowspan=1>Thrid Reduction </td><td>90 Days</td></tr>
<tr><td>297600-556800</td><td>10 CCBC</td><td rowspan=1>Final Reduction </td><td>180 Days</td></tr>
<tr><td>556800+</td><td>5 CCBC</td><td rowspan=1>Till Max Supply </td><td>57,026.38 Days</td></tr>
</table>


### Masternode Rewards Per Block, 25000 CCBC Collateral

<table>
<tr><th>Block Height</th><th>MN Reward Amount</th><th>PoS Reward Amount</th><th>Duration (Days)</th><th>Development Fee</th></tr>
<tr><td>201-25000</td><td>60%</td><td>40%</td><td>17.2 Days</td><td>0%</td></tr>
<tr><td>25000-60000 </td><td>60%</td><td>40%</td><td>17.22 Days</td><td>0%</td></tr>
<tr><td>60000-65000 </td><td>65%</td><td>35%</td><td>3.47 Days</td><td>10%</td></tr>
<tr><td>65000-70000 </td><td>66%</td><td>34%</td><td>3.47 Days</td><td>10%</td></tr>
<tr><td>70000-75000 </td><td>67%</td><td>33%</td><td>3.47 Days</td><td>10%</td></tr>
<tr><td>75000-80000 </td><td>68%</td><td>32%</td><td>3.47 Days</td><td>10%</td></tr>
<tr><td>80000-85000 </td><td>69%</td><td>31%</td><td>2.08 Days</td><td>10%</td></tr>
<tr><td>85000-88000 </td><td>70%</td><td>30%</td><td>2.08 Days</td><td>10%</td></tr>
<tr><td>88000-91000 </td><td>72%</td><td>28%</td><td>2.08 Days</td><td>10%</td></tr>
<tr><td>91000-94000 </td><td>74%</td><td>26%</td><td>2.08 Days</td><td>10%</td></tr>
<tr><td>94000-97000 </td><td>76%</td><td>24%</td><td>2.08 Days</td><td>10%</td></tr>
<tr><td>97000-100000 </td><td>78%</td><td>22%</td><td>2.08 Days</td><td>10%</td></tr>
<tr><td>100000-125000 </td><td>80%</td><td>20%</td><td>17.36 Days</td><td>10%</td></tr>
<tr><td>125000-150000 </td><td>85%</td><td>15%</td><td>17.36 Days</td><td>5%</td></tr>
<tr><td>150000-End Supply </td><td>90%</td><td>10%</td><td>17.36 Days</td><td>5%</td></tr>
</table>

MN Port: 5520
RPC Port: 5521
Testnet: 15520
