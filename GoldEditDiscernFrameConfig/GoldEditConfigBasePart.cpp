#include "GoldEditDiscernFrameConfig.h"


GoldEditDiscernFrameConfig::GoldEditDiscernFrameConfig(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);
	this->setWindowFlags(this->windowFlags() | Qt::WindowMinMaxButtonsHint);
	ui.tableWidget_multiItem_preview->setAttribute(Qt::WA_TransparentForMouseEvents);

	//QSettings settings("HKEY_CURRENT_USER\\SOFTWARE\\JinQu\\CADTools",QSettings::NativeFormat);
	//QString jqCADToolsPath = settings.value("AppLocation").toString();
	//if (jqCADToolsPath.isEmpty())
	//{
	//	jqCADToolsPath = qApp->applicationDirPath();
	//	QMessageBox::critical(this, QString::fromLocal8Bit("错误"), QString::fromLocal8Bit("读取金曲智能CAD工具包目录失败，请重新安装。\t\t\n"));
	//	SendAppMsg((HWND)parent->winId(), (PVOID)"AppExited");
	//	qApp->quit();
	//}
	
	QString jqCADToolsPath = qApp->applicationDirPath();

	xmlPath = jqCADToolsPath + QString("\\discernframeconfig.xml");
	xmlPathInConfig = jqCADToolsPath + QString("\\template\\discernframeconfig.xml");

	curTableWidgetRefConfigIndex = -1;
	curMultiTableWidgetRefConfigIndex = -1;
	curMultiItemsInTableWidget.clear();
	curMultiRoleNameInTableWidget.clear();

	readAllSignBoxConfig();
	reorderAllSignBoxConfig(false);
	readAllMultiSignBoxConfig();
	reorderAllMultiSignBoxConfig(false);
	initAllWidget();
	initAllConnection();
	refreshTableWidget();
	refreshMultiTableWidget();

	onTabWidgetCurrentChanged(-1);
	DrawMultiTablePreview();
}

void GoldEditDiscernFrameConfig::initAllWidget()
{
	//图签相关（一对一）	
	QList<DiscernSignBoxConfig>::iterator allSignBoxConfigIter = allSignBoxConfig.begin();
	while (allSignBoxConfigIter != allSignBoxConfig.end())
	{
		QListWidgetItem *listItem = new QListWidgetItem(allSignBoxConfigIter->groupName);
		listItem->setFlags(listItem->flags() | Qt::ItemIsEditable);
		listItem->setData(Qt::AccessibleDescriptionRole, QVariant(allSignBoxConfigIter->id));
		ui.listWidget_group->addItem(listItem);
		allSignBoxConfigIter++;
	}

	ui.tableWidget_item->horizontalHeader()->setFixedHeight(30);
	ui.tableWidget_item->verticalHeader()->setFixedWidth(30);
	ui.tableWidget_item->horizontalHeader()->setDefaultAlignment(Qt::AlignCenter);
	ui.tableWidget_item->verticalHeader()->setDefaultAlignment(Qt::AlignCenter);
	ui.tableWidget_item->horizontalHeader()->setVisible(true);
	ui.tableWidget_item->verticalHeader()->setVisible(true);

	//会签相关（一对多）	
	QList<DiscernMultiSignBoxConfig>::iterator allMultiSignBoxConfigIter = allMultiSignBoxConfig.begin();
	while (allMultiSignBoxConfigIter != allMultiSignBoxConfig.end())
	{
		QListWidgetItem *listItem = new QListWidgetItem(allMultiSignBoxConfigIter->groupName);
		listItem->setFlags(listItem->flags() | Qt::ItemIsEditable);
		listItem->setData(Qt::AccessibleDescriptionRole, QVariant(allMultiSignBoxConfigIter->id));
		ui.listWidget_multiGroup->addItem(listItem);
		allMultiSignBoxConfigIter++;
	}

	ui.dock_editMultiItem->setWindowTitle(QString::fromLocal8Bit("编辑会签条目"));
	multiSignBoxEditor = new GoldMultiSignBoxEditor(this);

	ui.tableWidget_multiItem_preview->installEventFilter(this);
}

void GoldEditDiscernFrameConfig::initAllConnection()
{
	//基本界面控件
	connect(ui.tabWidget_mainFrame_item, SIGNAL(currentChanged(int)), this, SLOT(onTabWidgetCurrentChanged(int)));
	connect(ui.button_confirm, SIGNAL(clicked()), this, SLOT(onBtnConfirmClicked()));
	connect(ui.button_cancel, SIGNAL(clicked()), this, SLOT(close()));


	//图签相关（一对一）
	connect(ui.button_addGroup, SIGNAL(clicked()), this, SLOT(onBtnAddGroupClicked()));
	connect(ui.button_deleteGroup, SIGNAL(clicked()), this, SLOT(onBtnDeleteGroupClicked()));
	connect(ui.listWidget_group, SIGNAL(currentTextChanged(const QString &)), this, SLOT(onListWidgetCurSelectTextChanged(const QString &)));
	connect(ui.listWidget_group, SIGNAL(itemChanged(QListWidgetItem *)), this, SLOT(onListWidgetItemChanged(QListWidgetItem *)));

	connect(ui.button_addItem, SIGNAL(clicked()), this, SLOT(onBtnAddItemClicked()));
	connect(ui.button_deleteItem, SIGNAL(clicked()), this, SLOT(onBtnDeleteItemClicked()));
	connect(ui.button_restoreItem, SIGNAL(clicked()), this, SLOT(onBtnRestoreItemClicked()));
	connect(ui.button_applyItem, SIGNAL(clicked()), this, SLOT(onBtnApplyItemClicked()));


	//会签相关（一对多）
	connect(ui.button_addMultiGroup, SIGNAL(clicked()), this, SLOT(onBtnAddMultiGroupClicked()));
	connect(ui.button_deleteMultiGroup, SIGNAL(clicked()), this, SLOT(onBtnDeleteMultiGroupClicked()));
	connect(ui.listWidget_multiGroup, SIGNAL(currentTextChanged(const QString &)), this, SLOT(onMultiListWidgetCurSelectTextChanged(const QString &)));
	connect(ui.listWidget_multiGroup, SIGNAL(itemChanged(QListWidgetItem *)), this, SLOT(onMultiListWidgetItemChanged(QListWidgetItem *)));

	connect(ui.button_addMultiItem, SIGNAL(clicked()), this, SLOT(onBtnAddMultiItemClicked()));
	connect(ui.button_deleteMultiItem, SIGNAL(clicked()), this, SLOT(onBtnDeleteMultiItemClicked()));
	connect(ui.button_restoreMultiItem, SIGNAL(clicked()), this, SLOT(onBtnRestoreMultiItemClicked()));
	connect(ui.button_applyMultiItem, SIGNAL(clicked()), this, SLOT(onBtnApplyMultiItemClicked()));

	connect(ui.dock_editMultiItem, SIGNAL(topLevelChanged(bool)), this, SLOT(onMultiDockWidgetTopLevelChanged(bool)));
	connect(multiSignBoxEditor->ui.button_apply, SIGNAL(clicked()), this, SLOT(onMultiSignBoxEditorBtnApplyClicked()));

	connect(ui.combo_multiItem_direction, SIGNAL(currentIndexChanged(int)), this, SLOT(onMultiComboDirectionSelChanged(int)));
}

bool GoldEditDiscernFrameConfig::eventFilter(QObject * object, QEvent * event)
{
	return QDialog::eventFilter(object, event);
}

bool	GoldEditDiscernFrameConfig::event(QEvent * e)
{
	return QDialog::event(e);
}

void GoldEditDiscernFrameConfig::closeEvent(QCloseEvent * e)
{
	ui.dock_editMultiItem->setFloating(false);
	e->accept();
}

void GoldEditDiscernFrameConfig::onBtnConfirmClicked()
{
	refreshTableWidget();
	writeAllSignBoxConfig();
	refreshMultiTableWidget();
	writeAllMultiSignBoxConfig();
	this->close();
}

void GoldEditDiscernFrameConfig::onTabWidgetCurrentChanged(int index)
{
	switch (index)
	{
	case 0:
		ui.widget_group->show();
		ui.widget_multiGroup->hide();
		this->setWindowTitle(QString::fromLocal8Bit("编辑签名框配置 - 图签模板"));
		ui.dock_editMultiItem->hide();
		break;
	case 1:
		ui.widget_group->hide();
		ui.widget_multiGroup->show();
		this->setWindowTitle(QString::fromLocal8Bit("编辑签名框配置 - 会签模板"));
		ui.dock_editMultiItem->show();
		break;
	default:
		ui.widget_group->show();
		ui.widget_multiGroup->hide();
		ui.tabWidget_mainFrame_item->setCurrentIndex(0);
		this->setWindowTitle(QString::fromLocal8Bit("编辑签名框配置 - 图签模板"));
		ui.dock_editMultiItem->hide();
		break;
	}
}