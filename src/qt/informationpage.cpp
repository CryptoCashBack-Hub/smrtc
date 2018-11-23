// Copyright (c) 2011-2014 The Bitcoin developers
// Copyright (c) 2014-2015 The Dash developers
// Copyright (c) 2015-2017 The PIVX developers
// Copyright (c) 2018 The CCBC developers
// Distributed under the MIT/X11 software license, see the accompanyingF
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "informationpage.h"
#include "ui_informationpage.h"

#include "bitcoinunits.h"
#include "clientmodel.h"
#include "guiconstants.h"
#include "guiutil.h"
#include "init.h"
#include "obfuscation.h"
#include "obfuscationconfig.h"
#include "optionsmodel.h"
#include "transactionfilterproxy.h"
#include "transactiontablemodel.h"
#include "walletmodel.h"
#include "masternodeman.h"
#include "main.h"
#include "chainparams.h"
#include "amount.h"
#include "addressbookpage.h"
#include "rpcblockchain.cpp"

#include <QAbstractItemDelegate>
#include <QPainter>
#include <QSettings>
#include <QTimer>
#include <QUrl>
#include <QDesktopServices>

#define DECORATION_SIZE 48
#define ICON_OFFSET 16
#define NUM_ITEMS 5

extern CWallet* pwalletMain;

#include "InformationPage.moc"

void InformationPage::setModel(WalletModel* model)
{

}

void InformationPage::setClientModel(ClientModel* clientModel)
{
    this->clientModel = clientModel;

    if (clientModel) {
        connect(clientModel, SIGNAL(numBlocksChanged(int)), this, SLOT(updateSmartFeeLabel()));
    }
}



//All credit goes to the ESB team for developing this. https://github.com/BlockchainFor/ESBC2
void InformationPage::updateMasternodeInfo()
{
		if (masternodeSync.IsBlockchainSynced() && masternodeSync.IsSynced()) {
       

			int mn1 = 0;
			int mn2 = 0;
			int mn3 = 0;
			int mn4 = 0;
			int totalmn = 0;
			std::vector<CMasternode> vMasternodes = mnodeman.GetFullMasternodeVector();
				for (auto& mn : vMasternodes)
				{
					switch (mn.nActiveState = true)
					{
						case 1:
							mn1++; break;
						case 2:
							mn2++; break;
						case 3:
							mn3++; break;
						case 4:
							mn4++; break;
					}

				}
					totalmn = mn1 + mn2 + mn3 + mn4;
					//ui->labelMnTotal_Value->setText(QString::number(totalmn));
					ui->graphMN->setMaximum(totalmn);
					ui->graphMN->setValue(mn1);

					// TODO: need a read actual 24h blockcount from chain
					int BlockCount24h = 1440;
					double BlockReward = GetBlockValue(chainActive.Height());
                    double roi1 = (0.72 * BlockReward * BlockCount24h) / mn1 / COIN;
                    double roi2 = (0.74 * BlockReward * BlockCount24h) / mn1 / COIN;
                    double roi3 = (0.76 * BlockReward * BlockCount24h) / mn1 / COIN;
                    double roi4 = (0.78 * BlockReward * BlockCount24h) / mn1 / COIN;
                    double roi5 = (0.80 * BlockReward * BlockCount24h) / mn1 / COIN;
                    double roi6 = (0.85 * BlockReward * BlockCount24h) / mn1 / COIN;
                    double roi7 = (0.90 * BlockReward * BlockCount24h) / mn1 / COIN;
                    
					if (chainActive.Height() <= 91000 && chainActive.Height() > 88000){ //72%  
                        ui->roi->setText(mn1 == 0 ? "-" : QString::number(roi1, 'f', 0).append("  |"));
                        ui->roi_1->setText(mn1 == 0 ? " " : QString::number(25000 / roi1, 'f', 1).append(" days"));
                        ui->label_lcolat->setText("25000 CCBC");
					}else if (chainActive.Height() <= 94000 && chainActive.Height() > 91000) { //74%                      
                        ui->roi->setText(mn1 == 0 ? "-" : QString::number(roi2, 'f', 0).append("  |"));
                        ui->roi_1->setText(mn1 == 0 ? " " : QString::number(25000 / roi2, 'f', 1).append(" days"));
                        ui->label_lcolat->setText("25000 CCBC");
					}else if (chainActive.Height() <= 97000 && chainActive.Height() > 94000) { //76%                       
                        ui->roi->setText(mn1 == 0 ? "-" : QString::number(roi3, 'f', 0).append("  |"));
                        ui->roi_1->setText(mn1 == 0 ? " " : QString::number(25000 / roi3, 'f', 1).append(" days"));
                        ui->label_lcolat->setText("25000 CCBC");
					}else if (chainActive.Height() <= 100000 && chainActive.Height() > 97000) { //78%
                        ui->roi->setText(mn1 == 0 ? "-" : QString::number(roi4, 'f', 0).append("  |"));
                        ui->roi_1->setText(mn1 == 0 ? " " : QString::number(25000 / roi4, 'f', 1).append(" days"));
                        ui->label_lcolat->setText("25000 CCBC");
					}else if (chainActive.Height() <= 125000 && chainActive.Height() > 100000) { //80%
                        ui->roi->setText(mn1 == 0 ? "-" : QString::number(roi5, 'f', 0).append("  |"));
                        ui->roi_1->setText(mn1 == 0 ? " " : QString::number(25000 / roi5, 'f', 1).append(" days"));
                        ui->label_lcolat->setText("25000 CCBC");
					}else if (chainActive.Height() <= 150000 && chainActive.Height() > 125000) { //85%
                        ui->roi->setText(mn1 == 0 ? "-" : QString::number(roi6, 'f', 0).append("  |"));
                        ui->roi_1->setText(mn1 == 0 ? " " : QString::number(25000 / roi6, 'f', 1).append(" days"));
                        ui->label_lcolat->setText("25000 CCBC");
                    }else if (chainActive.Height() <= 175000 && chainActive.Height() > 150000) { //90%
						ui->roi->setText(mn1 == 0 ? "-" : QString::number(roi7, 'f', 0).append("  |"));
                        ui->roi_1->setText(mn1 == 0 ? " " : QString::number(25000 / roi7, 'f', 1).append(" days"));
                        ui->label_lcolat->setText("25000 CCBC");
                    }
                    // update timer
                    if (timerinfo_mn->interval() == 1000)
                        timerinfo_mn->setInterval(10000);
        }
}

//All credit goes to the ESB team for developing this. https://github.com/BlockchainFor/ESBC2
void InformationPage::updatBlockChainInfo()
{
    if (masternodeSync.IsBlockchainSynced()) {
        int CurrentBlock = (int)chainActive.Height();
        int64_t netHashRate = chainActive.GetNetworkHashPS(24, CurrentBlock - 1);
        double BlockReward = GetBlockValue(chainActive.Height());
        double BlockRewardesbcoin = static_cast<double>(BlockReward / COIN);
        double CurrentDiff = GetDifficulty();

        ui->label_CurrentBlock_value->setText(QString::number(CurrentBlock));

        ui->label_Nethash->setText(tr("Difficulty:"));
        ui->label_Nethash_value->setText(QString::number(CurrentDiff, 'f', 4));

        ui->label_CurrentBlockReward_value->setText(QString::number(BlockRewardesbcoin, 'f', 1));

        ui->label_Supply_value->setText(QString::number(chainActive.Tip()->nMoneySupply / COIN).append(" CCBC"));
    }
}
