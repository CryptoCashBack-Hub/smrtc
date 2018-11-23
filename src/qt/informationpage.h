// Copyright (c) 2011-2013 The Bitcoin developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOIN_QT_INFORMATIONPAGE_H
#define BITCOIN_QT_INFORMATIONPAGE_H

#include "guiutil.h"

#include <QKeyEvent>
#include <QWidget>

class ClientModel;
class OptionsModel;


namespace Ui
{
class InformationPage;
}

QT_BEGIN_NAMESPACE
class QUrl;
QT_END_NAMESPACE

/** Widget showing the transaction list for a wallet, including a filter row.
    Using the filter row, the user can view or export a subset of the transactions.
  */
class InformationPage : public QWidget
{
    Q_OBJECT

public:
    explicit InformationPage(QWidget* parent = 0);
    ~InformationPage();

    void setClientModel(ClientModel* clientModel);
    void setModel(WalletModel* model);

private:
    WalletModel* model;

private:
    Ui::InformationPage* ui;
    ClientModel* clientModel;
    WalletModel* model;



    QTimer* timer;
    QTimer* timerinfo_mn;
    QTimer* timerinfo_blockchain;
    int nDisplayUnit;

private slots:
    void updateDisplayUnit();
    void updateMasternodeInfo();
    void updatBlockChainInfo();
};
#endif // BITCOIN_QT_INFORMATIONPAGE_H
