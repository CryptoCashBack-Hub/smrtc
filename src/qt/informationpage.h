// Copyright (c) 2011-2013 The Bitcoin developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOIN_QT_INFORMATIONPAGE_H
#define BITCOIN_QT_INFORMATIONPAGE_H

#include "guiutil.h"

#include <QKeyEvent>
#include <QWidget>

class WalletModel;
QT_BEGIN_NAMESPACE
class QFrame;
class QLineEdit;
class QMenu;
QT_END_NAMESPACE

/** Widget showing the transaction list for a wallet, including a filter row.
    Using the filter row, the user can view or export a subset of the transactions.
  */
class InformationPage : public QWidget
{
    Q_OBJECT

public:


    void setModel(WalletModel* model);



private:
    WalletModel* model;

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
