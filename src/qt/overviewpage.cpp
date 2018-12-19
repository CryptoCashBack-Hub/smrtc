// Copyright (c) 2011-2014 The Bitcoin developers
// Copyright (c) 2014-2015 The Dash developers
// Copyright (c) 2015-2017 The PIVX developers
// Copyright (c) 2018 The CCBC developers
// Distributed under the MIT/X11 software license, see the accompanyingF
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "overviewpage.h"
#include "ui_overviewpage.h"

#include "addressbookpage.h"
#include "amount.h"
#include "bitcoinunits.h"
#include "chainparams.h"
#include "clientmodel.h"
#include "guiconstants.h"
#include "guiutil.h"
#include "init.h"
#include "main.h"
#include "masternodeman.h"
#include "obfuscation.h"
#include "obfuscationconfig.h"
#include "optionsmodel.h"
#include "rpcblockchain.cpp"
#include "transactionfilterproxy.h"
#include "transactiontablemodel.h"
#include "walletmodel.h"

#include <QAbstractItemDelegate>
#include <QDesktopServices>
#include <QPainter>
#include <QSettings>
#include <QTimer>
#include <QUrl>

#define DECORATION_SIZE 48
#define ICON_OFFSET 16
#define NUM_ITEMS 5

extern CWallet* pwalletMain;

class TxViewDelegate : public QAbstractItemDelegate
{
	Q_OBJECT
public:
	TxViewDelegate() : QAbstractItemDelegate(), unit(BitcoinUnits::CCBC)
	{
	}

	inline void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
	{
		painter->save();

		QIcon icon = qvariant_cast<QIcon>(index.data(Qt::DecorationRole));
		QRect mainRect = option.rect;
		mainRect.moveLeft(ICON_OFFSET);
		QRect decorationRect(mainRect.topLeft(), QSize(DECORATION_SIZE, DECORATION_SIZE));
		int xspace = DECORATION_SIZE + 8;
		int ypad = 6;
		int halfheight = (mainRect.height() - 2 * ypad) / 2;
		QRect amountRect(mainRect.left() + xspace, mainRect.top() + ypad, mainRect.width() - xspace - ICON_OFFSET, halfheight);
		QRect addressRect(mainRect.left() + xspace, mainRect.top() + ypad + halfheight, mainRect.width() - xspace, halfheight);
		icon.paint(painter, decorationRect);

		QDateTime date = index.data(TransactionTableModel::DateRole).toDateTime();
		QString address = index.data(Qt::DisplayRole).toString();
		qint64 amount = index.data(TransactionTableModel::AmountRole).toLongLong();
		bool confirmed = index.data(TransactionTableModel::ConfirmedRole).toBool();
		QVariant value = index.data(Qt::ForegroundRole);
		QColor foreground = COLOR_BLACK;
		if (value.canConvert<QBrush>()) {
			QBrush brush = qvariant_cast<QBrush>(value);
			foreground = brush.color();
		}

		painter->setPen(foreground);
		QRect boundingRect;
		painter->drawText(addressRect, Qt::AlignLeft | Qt::AlignVCenter, address, &boundingRect);

		if (index.data(TransactionTableModel::WatchonlyRole).toBool()) {
			QIcon iconWatchonly = qvariant_cast<QIcon>(index.data(TransactionTableModel::WatchonlyDecorationRole));
			QRect watchonlyRect(boundingRect.right() + 5, mainRect.top() + ypad + halfheight, 16, halfheight);
			iconWatchonly.paint(painter, watchonlyRect);
		}

		if (amount < 0) {
			foreground = COLOR_NEGATIVE;
		}
		else if (!confirmed) {
			foreground = COLOR_UNCONFIRMED;
		}
		else {
			foreground = COLOR_BLACK;
		}
		painter->setPen(foreground);
		QString amountText = BitcoinUnits::formatWithUnit(unit, amount, true, BitcoinUnits::separatorAlways);
		if (!confirmed) {
			amountText = QString("[") + amountText + QString("]");
		}
		painter->drawText(amountRect, Qt::AlignRight | Qt::AlignVCenter, amountText);

		painter->setPen(COLOR_BLACK);
		painter->drawText(amountRect, Qt::AlignLeft | Qt::AlignVCenter, GUIUtil::dateTimeStr(date));

		painter->restore();
	}

	inline QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const
	{
		return QSize(DECORATION_SIZE, DECORATION_SIZE);
	}

	int unit;
};
#include "overviewpage.moc"

OverviewPage::OverviewPage(QWidget* parent) : QWidget(parent),
ui(new Ui::OverviewPage),
clientModel(0),
walletModel(0),
currentBalance(-1),
currentUnconfirmedBalance(-1),
currentImmatureBalance(-1),
//currentZerocoinBalance(-1),
//currentUnconfirmedZerocoinBalance(-1),
//currentimmatureZerocoinBalance(-1),
currentWatchOnlyBalance(-1),
currentWatchUnconfBalance(-1),
currentWatchImmatureBalance(-1),
txdelegate(new TxViewDelegate()),
filter(0)
{
	nDisplayUnit = 0; // just make sure it's not unitialized
	ui->setupUi(this);
	/*
	ui->pushButton_Website->setIcon(QIcon(":/icons/website"));
	ui->pushButton_Discord->setIcon(QIcon(":/icons/discord"));
	ui->pushButton_Github->setIcon(QIcon(":/icons/github"));
	ui->pushButton_Twitter->setIcon(QIcon(":/icons/twitter"));
	ui->pushButton_Explorer->setIcon(QIcon(":/icons/explorer"));
	*/

	// Recent transactions
	ui->listTransactions->setItemDelegate(txdelegate);
	ui->listTransactions->setIconSize(QSize(DECORATION_SIZE, DECORATION_SIZE));
	ui->listTransactions->setMinimumHeight(NUM_ITEMS * (DECORATION_SIZE + 2));
	ui->listTransactions->setAttribute(Qt::WA_MacShowFocusRect, false);

	connect(ui->listTransactions, SIGNAL(clicked(QModelIndex)), this, SLOT(handleTransactionClicked(QModelIndex)));


	// init "out of sync" warning labels
	ui->labelWalletStatus->setText("(" + tr("out of sync") + ")");
	ui->labelTransactionsStatus->setText("(" + tr("out of sync") + ")");

	//information block update
	timerinfo_mn = new QTimer(this);
	connect(timerinfo_mn, SIGNAL(timeout()), this, SLOT(updateMasternodeInfo()));
	timerinfo_mn->start(1000);

	timerinfo_blockchain = new QTimer(this);
	connect(timerinfo_blockchain, SIGNAL(timeout()), this, SLOT(updatBlockChainInfo()));
	timerinfo_blockchain->start(1000); //30sec

									   // start with displaying the "out of sync" warnings
	showOutOfSyncWarning(true);
}

void OverviewPage::handleTransactionClicked(const QModelIndex& index)
{
	if (filter)
		emit transactionClicked(filter->mapToSource(index));
}

OverviewPage::~OverviewPage()
{
	delete ui;
}

/*
void OverviewPage::getPercentage(CAmount nUnlockedBalance, CAmount nZerocoinBalance, QString& sCCBCPercentage, QString& szCCBCPercentage)
{
int nPrecision = 2;
double dzPercentage = 0.0;
if (nZerocoinBalance <= 0){
dzPercentage = 0.0;
}
else{
if (nUnlockedBalance <= 0){
dzPercentage = 100.0;
}
else{
dzPercentage = 100.0 * (double)(nZerocoinBalance / (double)(nZerocoinBalance + nUnlockedBalance));
}
}
double dPercentage = 100.0 - dzPercentage;

szCCBCPercentage = "(" + QLocale(QLocale::system()).toString(dzPercentage, 'f', nPrecision) + " %)";
sCCBCPercentage = "(" + QLocale(QLocale::system()).toString(dPercentage, 'f', nPrecision) + " %)";

}
*/

void OverviewPage::setBalance(const CAmount& balance, const CAmount& unconfirmedBalance, const CAmount& immatureBalance,
	//const CAmount& zerocoinBalance, const CAmount& unconfirmedZerocoinBalance, const CAmount& immatureZerocoinBalance,
	const CAmount& watchOnlyBalance,
	const CAmount& watchUnconfBalance,
	const CAmount& watchImmatureBalance)
{
	currentBalance = balance;
	currentUnconfirmedBalance = unconfirmedBalance;
	currentImmatureBalance = immatureBalance;
	//currentZerocoinBalance = zerocoinBalance;
	//currentUnconfirmedZerocoinBalance = unconfirmedZerocoinBalance;
	//currentimmatureZerocoinBalance = immatureZerocoinBalance;
	currentWatchOnlyBalance = watchOnlyBalance;
	currentWatchUnconfBalance = watchUnconfBalance;
	currentWatchImmatureBalance = watchImmatureBalance;

	// CCBC labels
	ui->labelBalance->setText(BitcoinUnits::floorHtmlWithUnit(nDisplayUnit, balance - immatureBalance, false, BitcoinUnits::separatorAlways));
	//ui->labelzBalance->setText(BitcoinUnits::floorHtmlWithUnit(nDisplayUnit, zerocoinBalance, false, BitcoinUnits::separatorAlways));
	ui->labelUnconfirmed->setText(BitcoinUnits::floorHtmlWithUnit(nDisplayUnit, unconfirmedBalance, false, BitcoinUnits::separatorAlways));
	ui->labelImmature->setText(BitcoinUnits::floorHtmlWithUnit(nDisplayUnit, immatureBalance, false, BitcoinUnits::separatorAlways));
	ui->labelTotal->setText(BitcoinUnits::floorHtmlWithUnit(nDisplayUnit, balance + unconfirmedBalance, false, BitcoinUnits::separatorAlways));

	// Watchonly labels
	ui->labelWatchAvailable->setText(BitcoinUnits::floorHtmlWithUnit(nDisplayUnit, watchOnlyBalance, false, BitcoinUnits::separatorAlways));
	ui->labelWatchPending->setText(BitcoinUnits::floorHtmlWithUnit(nDisplayUnit, watchUnconfBalance, false, BitcoinUnits::separatorAlways));
	ui->labelWatchImmature->setText(BitcoinUnits::floorHtmlWithUnit(nDisplayUnit, watchImmatureBalance, false, BitcoinUnits::separatorAlways));
	ui->labelWatchTotal->setText(BitcoinUnits::floorHtmlWithUnit(nDisplayUnit, watchOnlyBalance + watchUnconfBalance + watchImmatureBalance, false, BitcoinUnits::separatorAlways));


	//zCCBC labels
	/*
	QString szPercentage = "";
	QString sPercentage = "";
	CAmount nLockedBalance = 0;
	if (pwalletMain) {
	nLockedBalance = pwalletMain->GetLockedCoins();
	}
	ui->labelLockedBalance->setText(BitcoinUnits::floorHtmlWithUnit(nDisplayUnit, nLockedBalance, false, BitcoinUnits::separatorAlways));
	CAmount nTotalBalance = balance + unconfirmedBalance;
	CAmount nUnlockedBalance = nTotalBalance - nLockedBalance;
	CAmount matureZerocoinBalance = zerocoinBalance - immatureZerocoinBalance;
	getPercentage(nUnlockedBalance, zerocoinBalance, sPercentage, szPercentage);
	ui->labelBalancez->setText(BitcoinUnits::floorHtmlWithUnit(nDisplayUnit, nTotalBalance, false, BitcoinUnits::separatorAlways));
	ui->labelzBalancez->setText(BitcoinUnits::floorHtmlWithUnit(nDisplayUnit, zerocoinBalance, false, BitcoinUnits::separatorAlways));
	ui->labelzBalanceImmature->setText(BitcoinUnits::floorHtmlWithUnit(nDisplayUnit, immatureZerocoinBalance, false, BitcoinUnits::separatorAlways));
	ui->labelzBalanceUnconfirmed->setText(BitcoinUnits::floorHtmlWithUnit(nDisplayUnit, unconfirmedZerocoinBalance, false, BitcoinUnits::separatorAlways));
	ui->labelzBalanceMature->setText(BitcoinUnits::floorHtmlWithUnit(nDisplayUnit, matureZerocoinBalance, false, BitcoinUnits::separatorAlways));
	ui->labelTotalz->setText(BitcoinUnits::floorHtmlWithUnit(nDisplayUnit, nTotalBalance + zerocoinBalance, false, BitcoinUnits::separatorAlways));
	ui->labelUnLockedBalance->setText(BitcoinUnits::floorHtmlWithUnit(nDisplayUnit, nUnlockedBalance, false, BitcoinUnits::separatorAlways));
	ui->labelCCBCPercent->setText(sPercentage);
	ui->labelzCCBCPercent->setText(szPercentage);
	// Adjust bubble-help according to AutoMint settings
	QString automintHelp = tr("Current percentage of zCCBC.\nIf AutoMint is enabled this percentage will settle around the configured AutoMint percentage (default = 10%).\n");
	bool fEnableZeromint = GetBoolArg("-enablezeromint", false);
	int nZeromintPercentage = GetArg("-zeromintpercentage", 10);
	if (fEnableZeromint) {
	automintHelp += tr("AutoMint is currently enabled and set to ") + QString::number(nZeromintPercentage) + "%.\n";
	automintHelp += tr("To disable AutoMint delete set 'enablezeromint=1' to 'enablezeromint=0' in ccbc.conf.");
	}
	else {
	automintHelp += tr("AutoMint is currently disabled.\nTo enable AutoMint add 'enablezeromint=1' in ccbc.conf");
	}
	ui->labelzCCBCPercent->setToolTip(automintHelp);
	*/

	//only show immature (newly mined) balance if it's non-zero, so as not to complicate things
	//for the non-mining users
	bool showImmature = immatureBalance != 0;
	bool showWatchOnlyImmature = watchImmatureBalance != 0;

	// for symmetry reasons also show immature label when the watch-only one is shown
	ui->labelImmature->setVisible(showImmature || showWatchOnlyImmature);
	ui->labelImmatureText->setVisible(showImmature || showWatchOnlyImmature);
	ui->labelWatchImmature->setVisible(showWatchOnlyImmature); // show watch-only immature balance

	static int cachedTxLocks = 0;

	if (cachedTxLocks != nCompleteTXLocks) {
		cachedTxLocks = nCompleteTXLocks;
		ui->listTransactions->update();
	}
}

// show/hide watch-only labels

void OverviewPage::updateWatchOnlyLabels(bool showWatchOnly)
{
	ui->labelSpendable->setVisible(showWatchOnly);      // show spendable label (only when watch-only is active)
	ui->labelWatchonly->setVisible(showWatchOnly);      // show watch-only label
	ui->lineWatchBalance->setVisible(showWatchOnly);    // show watch-only balance separator line
	ui->labelWatchAvailable->setVisible(showWatchOnly); // show watch-only available balance
	ui->labelWatchPending->setVisible(showWatchOnly);   // show watch-only pending balance
	ui->labelWatchTotal->setVisible(showWatchOnly);     // show watch-only total balance

	if (!showWatchOnly) {
		ui->labelWatchImmature->hide();
	}
	else {
		ui->labelBalance->setIndent(20);
		ui->labelUnconfirmed->setIndent(20);
		ui->labelImmature->setIndent(20);
		ui->labelTotal->setIndent(20);
	}
}


void OverviewPage::setClientModel(ClientModel* model)
{
	this->clientModel = model;
	if (model) {
		// Show warning if this is a prerelease version
		connect(model, SIGNAL(alertsChanged(QString)), this, SLOT(updateAlerts(QString)));
		updateAlerts(model->getStatusBarWarnings());
	}
}

void OverviewPage::setWalletModel(WalletModel* model)
{
	this->walletModel = model;
	if (model && model->getOptionsModel()) {
		// Set up transaction list
		filter = new TransactionFilterProxy();
		filter->setSourceModel(model->getTransactionTableModel());
		filter->setLimit(NUM_ITEMS);
		filter->setDynamicSortFilter(true);
		filter->setSortRole(Qt::EditRole);
		filter->setShowInactive(false);
		filter->sort(TransactionTableModel::Date, Qt::DescendingOrder);

		ui->listTransactions->setModel(filter);
		ui->listTransactions->setModelColumn(TransactionTableModel::ToAddress);

		// Keep up to date with wallet
		setBalance(model->getBalance(), model->getUnconfirmedBalance(), model->getImmatureBalance(),
			//setBalance(model->getBalance(), model->getUnconfirmedBalance(), model->getImmatureBalance(),
			//model->getZerocoinBalance(), model->getUnconfirmedZerocoinBalance(), model->getImmatureZerocoinBalance(),
			model->getWatchBalance(), model->getWatchUnconfirmedBalance(), model->getWatchImmatureBalance());
		//connect(model, SIGNAL(balanceChanged(CAmount, CAmount, CAmount, CAmount, CAmount, CAmount, CAmount, CAmount, CAmount)), this,
		//SLOT(setBalance(CAmount, CAmount, CAmount, CAmount, CAmount, CAmount, CAmount, CAmount, CAmount)));
		connect(model, SIGNAL(balanceChanged(CAmount, CAmount, CAmount, CAmount, CAmount, CAmount)), this,
			SLOT(setBalance(CAmount, CAmount, CAmount, CAmount, CAmount, CAmount)));


		connect(model->getOptionsModel(), SIGNAL(displayUnitChanged(int)), this, SLOT(updateDisplayUnit()));

		updateWatchOnlyLabels(model->haveWatchOnly());
		//connect(model, SIGNAL(notifyWatchonlyChanged(bool)), this, SLOT(updateWatchOnlyLabels(bool)));
	}

	// update the display unit, to not use the default ("CCBC")
	updateDisplayUnit();
}

void OverviewPage::updateDisplayUnit()
{
	if (walletModel && walletModel->getOptionsModel()) {
		nDisplayUnit = walletModel->getOptionsModel()->getDisplayUnit();
		if (currentBalance != -1)
			//setBalance(currentBalance, currentUnconfirmedBalance, currentImmatureBalance, currentZerocoinBalance, currentUnconfirmedZerocoinBalance, currentimmatureZerocoinBalance,
			setBalance(currentBalance, currentUnconfirmedBalance, currentImmatureBalance,
				currentWatchOnlyBalance, currentWatchUnconfBalance, currentWatchImmatureBalance);

		// Update txdelegate->unit with the current unit
		txdelegate->unit = nDisplayUnit;

		ui->listTransactions->update();
	}
}

//All credit goes to the ESB team for developing this. https://github.com/BlockchainFor/ESBC2
void OverviewPage::updateMasternodeInfo()
{
	if (masternodeSync.IsBlockchainSynced() && masternodeSync.IsSynced()) {
		int mn1 = 0;
		int mn2 = 0;
		int mn3 = 0;
		int mn4 = 0;
		int totalmn = 0;
		std::vector<CMasternode> vMasternodes = mnodeman.GetFullMasternodeVector();
		for (auto& mn : vMasternodes) {
			switch (mn.nActiveState = true) {
			case 1:
				mn1++;
				break;
			case 2:
				mn2++;
				break;
			case 3:
				mn3++;
				break;
			case 4:
				mn4++;
				break;
			}
		}
		totalmn = mn1 + mn2 + mn3 + mn4;
		ui->labelMnTotal_Value->setText(QString::number(totalmn));
		//ui->graphMN->setMaximum(totalmn);
		//ui->graphMN->setValue(mn1);


		// TODO: need a read actual 24h blockcount from chain
		int BlockCount24h = 1440;
		// update ROI
		double BlockReward = GetBlockValue(chainActive.Height());
		double roi1 = (0.72 * BlockReward * BlockCount24h) / mn1 / COIN;
		double roi2 = (0.74 * BlockReward * BlockCount24h) / mn1 / COIN;
		double roi3 = (0.76 * BlockReward * BlockCount24h) / mn1 / COIN;
		double roi4 = (0.78 * BlockReward * BlockCount24h) / mn1 / COIN;
		double roi5 = (0.80 * BlockReward * BlockCount24h) / mn1 / COIN;
		double roi6 = (0.85 * BlockReward * BlockCount24h) / mn1 / COIN;
		double roi7 = (0.90 * BlockReward * BlockCount24h) / mn1 / COIN;

		if (chainActive.Height() <= 91000 && chainActive.Height() > 88000) { //72%
			ui->roi->setText(mn1 == 0 ? "-" : QString::number(roi1, 'f', 0).append("  CCBC"));
			ui->roi_1->setText(mn1 == 0 ? " " : QString::number(25000 / roi1, 'f', 1).append(" days"));
		}
		else if (chainActive.Height() <= 94000 && chainActive.Height() > 91000) { //74%

			ui->roi->setText(mn1 == 0 ? "-" : QString::number(roi2, 'f', 0).append("  CCBC"));
			ui->roi_1->setText(mn1 == 0 ? " " : QString::number(25000 / roi2, 'f', 1).append(" days"));
		}
		else if (chainActive.Height() <= 97000 && chainActive.Height() > 94000) { //76%

			ui->roi->setText(mn1 == 0 ? "-" : QString::number(roi3, 'f', 0).append("  CCBC"));
			ui->roi_1->setText(mn1 == 0 ? " " : QString::number(25000 / roi3, 'f', 1).append(" days"));
		}
		else if (chainActive.Height() <= 100000 && chainActive.Height() > 97000) { //78%

			ui->roi->setText(mn1 == 0 ? "-" : QString::number(roi4, 'f', 0).append("  CCBC"));
			ui->roi_1->setText(mn1 == 0 ? " " : QString::number(25000 / roi4, 'f', 1).append(" days"));
		}
		else if (chainActive.Height() <= 125000 && chainActive.Height() > 100000) { //80%

			ui->roi->setText(mn1 == 0 ? "-" : QString::number(roi5, 'f', 0).append("  CCBC"));
			ui->roi_1->setText(mn1 == 0 ? " " : QString::number(25000 / roi5, 'f', 1).append(" days"));
		}
		else if (chainActive.Height() <= 150000 && chainActive.Height() > 125000) { //85%

			ui->roi->setText(mn1 == 0 ? "-" : QString::number(roi6, 'f', 0).append("  CCBC"));
			ui->roi_1->setText(mn1 == 0 ? " " : QString::number(25000 / roi6, 'f', 1).append(" days"));
		}
		else if (chainActive.Height() > 150000) { //90%

			ui->roi->setText(mn1 == 0 ? "-" : QString::number(roi7, 'f', 0).append("  CCBC"));
			ui->roi_1->setText(mn1 == 0 ? " " : QString::number(25000 / roi7, 'f', 1).append(" days"));
		}

		// update timer
		if (timerinfo_mn->interval() == 1000)
			timerinfo_mn->setInterval(10000);
	}

	// update collateral info
	if (chainActive.Height() >= 0) {
		ui->label_lcolat->setText("25000 CCBC");
	}
}

//All credit goes to the ESB team for developing this. https://github.com/BlockchainFor/ESBC2
void OverviewPage::updatBlockChainInfo()
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


void OverviewPage::updateAlerts(const QString& warnings)
{
	this->ui->labelAlerts->setVisible(!warnings.isEmpty());
	this->ui->labelAlerts->setText(warnings);
}

void OverviewPage::showOutOfSyncWarning(bool fShow)
{
	ui->labelWalletStatus->setVisible(fShow);
	ui->labelTransactionsStatus->setVisible(fShow);
}

//Button will be released in next update when they are fine tuned.

/*
void OverviewPage::pushButton_Website()
{
QDesktopServices::openUrl(QUrl("https://ccbcoin.club/", QUrl::TolerantMode));
}

void OverviewPage::pushButton_Explorer()
{
QDesktopServices::openUrl(QUrl("https://explorer.ccbcoin.club/#/", QUrl::TolerantMode));
}

void OverviewPage::pushButton_Github()
{
QDesktopServices::openUrl(QUrl("https://github.com/CryptoCashBack-Hub/CCBC", QUrl::TolerantMode));
}

void OverviewPage::pushButton_Discord()
{
QDesktopServices::openUrl(QUrl("https://discord.gg/MVVGzv6", QUrl::TolerantMode));
}

void OverviewPage::pushButton_Twitter()
{
QDesktopServices::openUrl(QUrl("https://twitter.com/CCBcoin", QUrl::TolerantMode));
}
*/