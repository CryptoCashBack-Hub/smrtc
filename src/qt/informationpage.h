// Copyright (c) 2011-2013 The Bitcoin developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOIN_QT_INFORMATIONPAGE_H
#define BITCOIN_QT_INFORMATIONPAGE_H

#include "guiutil.h"
#include "peertablemodel.h"

#include "net.h"

#include <QCompleter>
#include <QDialog>

class ClientModel;

namespace Ui
{
class InformationPage;
}

QT_BEGIN_NAMESPACE
class QItemSelection;
QT_END_NAMESPACE

/** Local Bitcoin RPC console. */
class InformationPage : public QWidget
{
    Q_OBJECT

public:
    explicit InformationPage(QWidget* parent = 0);
    ~InformationPage();

    void setClientModel(ClientModel* model);

    enum MessageClass {
        MC_ERROR,
        MC_DEBUG,
        CMD_REQUEST,
        CMD_REPLY,
        CMD_ERROR
    };

    enum TabTypes {
        TAB_INFO = 0,
        TAB_CONSOLE = 1,
        TAB_GRAPH = 2,
        TAB_PEERS = 3,
        TAB_REPAIR = 4
    };

protected:
    virtual bool eventFilter(QObject* obj, QEvent* event);

private slots:
    void on_lineEdit_returnPressed();
    void on_tabWidget_currentChanged(int index);
    void resizeEvent(QResizeEvent* event);
    void showEvent(QShowEvent* event);
    void hideEvent(QHideEvent* event);

public slots:
    void clear();
    void setTabFocus(enum TabTypes tabType);

private:
    static QString FormatBytes(quint64 bytes);
   
    enum ColumnWidths {
        ADDRESS_COLUMN_WIDTH = 170,
        SUBVERSION_COLUMN_WIDTH = 140,
        PING_COLUMN_WIDTH = 80
    };

    Ui::InformationPage* ui;
    ClientModel* clientModel;
    QStringList history;
    QCompleter* autoCompleter;

private:
    QTimer* timer;
    QTimer* timerinfo_mn;
    QTimer* timerinfo_blockchain;
    WalletModel* walletModel;

    int nDisplayUnit;

private slots:
    void updateDisplayUnit();
    void updateMasternodeInfo();
    void updatBlockChainInfo();
};

#endif // BITCOIN_QT_INFORMATIONPAGE_H