// Copyright (c) 2011-2013 The Bitcoin developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOIN_QT_INFORMATIONPAGE_H
#define BITCOIN_QT_INFORMATIONPAGE_H

#include "amount.h"
#include "main.h"

#include <QWidget>

class ClientModel;
class TransactionFilterProxy;
class TxViewDelegate;
class WalletModel;

namespace Ui
{
class InformationPage;
}

QT_BEGIN_NAMESPACE
class QModelIndex;
QT_END_NAMESPACE

class InformationPage : public QWidget
{
    Q_OBJECT

public:
    explicit InformationPage(QWidget* parent = 0);
    ~InformationPage();

    void setClientModel(ClientModel* clientModel);
    void setWalletModel(WalletModel* walletModel);
    void showOutOfSyncWarning(bool fShow);

	//public slots:
    //void setBalance(const CAmount& balance, const CAmount& unconfirmedBalance, const CAmount& immatureBalance, const CAmount& zerocoinBalance, const CAmount& unconfirmedZerocoinBalance, const CAmount& immatureZerocoinBalance, const CAmount& watchOnlyBalance, const CAmount& watchUnconfBalance, const CAmount& watchImmatureBalance);


	//signals:
    //void transactionClicked(const QModelIndex& index);

private:
    QTimer* timer;
    QTimer* timerinfo_mn;
    QTimer* timerinfo_blockchain;
    Ui::InformationPage* ui;
    ClientModel* clientModel;
    WalletModel* walletModel;

    int nDisplayUnit;

    TxViewDelegate* txdelegate;
    TransactionFilterProxy* filter;

private slots:
    void updateDisplayUnit();
    void updateMasternodeInfo();
    void updatBlockChainInfo();
};
#endif // BITCOIN_QT_INFORMATIONPAGE_H
