#include "goldchoosediscernframeconfig.h"
#include "ui_goldchoosediscernframeconfig.h"
#include <QWhatsThis>

GoldChooseDiscernFrameConfig::GoldChooseDiscernFrameConfig(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::GoldChooseDiscernFrameConfig)
{
    ui->setupUi(this);
	this->setWindowTitle(QString::fromLocal8Bit("为检索文本选择签名框配置"));
	this->setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowCloseButtonHint);
	this->setWindowIcon(QIcon(":/GoldEditDiscernFrameConfig/Icon.png"));
	ui->tableWidget_findText->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
	editSignBoxConfigDlg = new GoldEditDiscernFrameConfig(this);
	initAllConnection();
	readAllFindTextConfig();
	refreshFindTextTableWidget();
}

GoldChooseDiscernFrameConfig::~GoldChooseDiscernFrameConfig()
{
    delete ui;
}

void GoldChooseDiscernFrameConfig::initAllConnection()
{
	connect(ui->button_editSignBoxConfig, SIGNAL(clicked()), this, SLOT(onBtnEditSignBoxClicked()));
	connect(ui->button_confirm, SIGNAL(clicked()), this, SLOT(onBtnConfirmClicked()));
	connect(ui->button_cancel, SIGNAL(clicked()), this, SLOT(onBtnCancelClicked()));
	connect(editSignBoxConfigDlg->ui.button_confirm, SIGNAL(clicked()), this, SLOT(onConfigNeedBeCleared()));
}

void GoldChooseDiscernFrameConfig::readAllFindTextConfig()
{
	allFindTextConfig.clear();

	if (!QFile::exists(editSignBoxConfigDlg->xmlPath) && QFile::exists(editSignBoxConfigDlg->xmlPathInConfig))
	{
		QFile::copy(editSignBoxConfigDlg->xmlPathInConfig, editSignBoxConfigDlg->xmlPath);
	}

	//通过DOM方式读取xml
	QFile file(editSignBoxConfigDlg->xmlPath);
	if (!file.open(QFile::ReadOnly | QFile::Text))
		return;
	QDomDocument document;
	if (!document.setContent(&file))
		return;
	file.close();
	QDomElement root = document.documentElement();
	if (root.isNull())
		return;
	QDomElement findTextElements = root.firstChildElement("FindTextList");
	if (findTextElements.isNull())
		return;
	QDomElement itemElements = findTextElements.firstChildElement("Item");
	while (!itemElements.isNull())
	{
		DiscernTextConfig textConfig;
		textConfig.id = itemElements.attributeNode("ID").value().toInt();
		textConfig.content = itemElements.attributeNode("Content").value();
		textConfig.commonScaleListStr = itemElements.attributeNode("CommonScaleList").value();
		textConfig.minRectContainerHeight = itemElements.attributeNode("RectContainerHeight").value().toDouble();
		textConfig.minRectContainerWidth = itemElements.attributeNode("RectContainerWidth").value().toDouble();
		textConfig.signBoxConfigId = itemElements.attributeNode("SignBoxConfigId").value().toInt();
		textConfig.multiSignBoxConfigId = itemElements.attributeNode("MultiSignBoxConfigId").value().toInt();
		textConfig.attrOffsetConfigId = itemElements.attributeNode("AttrOffsetConfigId").value().toInt();
		textConfig.blockType = itemElements.attributeNode("BlockType").value().toInt();
		itemElements = itemElements.nextSiblingElement("Item");
		allFindTextConfig.push_back(textConfig);
	}
}

void GoldChooseDiscernFrameConfig::writeAllFindTextConfig()
{
	reorderAllFindTextConfig();
	
	//通过DOM方式读取xml
	QFile file(editSignBoxConfigDlg->xmlPath);
	QDomDocument document;
	if (file.open(QFile::ReadOnly | QFile::Text))
	{
		if (!document.setContent(&file))
		{
			file.close();
			return;
		}
		file.close();
	}

	QDomElement root = document.documentElement();
	if (root.isNull())
	{
		root = document.createElement("Root");
		document.appendChild(root);
	}

	QDomElement findTextElement = root.firstChildElement("FindTextList");
	if (!findTextElement.isNull())
		root.removeChild(findTextElement);

	findTextElement = document.createElement("FindTextList");
	root.appendChild(findTextElement);

	QList<DiscernTextConfig>::iterator allFindTextIter = allFindTextConfig.begin();
	while (allFindTextIter != allFindTextConfig.end())
	{
		QDomElement itemElement = document.createElement("Item");
		itemElement.setAttribute("ID", allFindTextIter->id);
		itemElement.setAttribute("Content", allFindTextIter->content);
		itemElement.setAttribute("RectContainerWidth", allFindTextIter->minRectContainerWidth);
		itemElement.setAttribute("RectContainerHeight", allFindTextIter->minRectContainerHeight);
		itemElement.setAttribute("CommonScaleList" , allFindTextIter->commonScaleListStr);
		itemElement.setAttribute("SignBoxConfigId", allFindTextIter->signBoxConfigId);
		itemElement.setAttribute("MultiSignBoxConfigId", allFindTextIter->multiSignBoxConfigId);
		itemElement.setAttribute("AttrOffsetConfigId", allFindTextIter->attrOffsetConfigId);
		itemElement.setAttribute("BlockType", allFindTextIter->blockType);

		findTextElement.appendChild(itemElement);
		allFindTextIter++;
	}

	if (!file.open(QFile::WriteOnly | QFile::Text))
		return;
	QTextStream fileStream(&file);
	document.save(fileStream, 4, QDomNode::EncodingFromDocument);
	file.close();
}

void GoldChooseDiscernFrameConfig::reorderAllFindTextConfig()
{
	for (int i = 0; i < ui->tableWidget_findText->rowCount(); i++)
	{
		allFindTextConfig[i].signBoxConfigId = ((QComboBox *)ui->tableWidget_findText->cellWidget(i, 1))->currentIndex();
		allFindTextConfig[i].multiSignBoxConfigId = ((QComboBox *)ui->tableWidget_findText->cellWidget(i, 2))->currentIndex();
	}
}

void GoldChooseDiscernFrameConfig::refreshFindTextTableWidget()
{
	ui->tableWidget_findText->setColumnWidth(0, 160);
	ui->tableWidget_findText->setColumnWidth(1, 180);
	ui->tableWidget_findText->setColumnWidth(2, 180);
	
	while (ui->tableWidget_findText->rowCount() > 0)
	{
		ui->tableWidget_findText->removeRow(0);
	}

	QList<DiscernTextConfig>::iterator allFindTextIter = allFindTextConfig.begin();
	while (allFindTextIter != allFindTextConfig.end())
	{
		int insertRowIndex = ui->tableWidget_findText->rowCount();
		ui->tableWidget_findText->insertRow(insertRowIndex);
		QString roleName = allFindTextIter->content;
		QTableWidgetItem *roleItem = new QTableWidgetItem(roleName);
		roleItem->setTextAlignment(Qt::AlignVCenter);
		roleItem->setFlags(roleItem->flags() & ~(Qt::ItemIsEditable));
		QString toolTipText = QString::fromLocal8Bit("详细信息\n-----------------\nID号：%1\n检索字段：%2\n所在最小矩形框宽高：%3 * %4 \n常用放大比例：%5");
		toolTipText = toolTipText.arg(allFindTextIter->id).arg(allFindTextIter->content).arg(allFindTextIter->minRectContainerWidth).arg(allFindTextIter->minRectContainerHeight).arg(allFindTextIter->commonScaleListStr);
		roleItem->setToolTip(toolTipText);
		ui->tableWidget_findText->setItem(insertRowIndex, 0, roleItem);
		QComboBox *signBoxCombo = new QComboBox();
		QList<DiscernSignBoxConfig>::iterator allSignBoxConfigIter = editSignBoxConfigDlg->allSignBoxConfig.begin();
		signBoxCombo->addItem(QString::fromLocal8Bit("空选"));
		while (allSignBoxConfigIter != editSignBoxConfigDlg->allSignBoxConfig.end())
		{
			signBoxCombo->addItem(allSignBoxConfigIter->groupName);
			allSignBoxConfigIter++;
		}
		int comboCurrentIndex = allFindTextConfig[insertRowIndex].signBoxConfigId;
		if (comboCurrentIndex >= 0 && comboCurrentIndex < signBoxCombo->count())
			signBoxCombo->setCurrentIndex(comboCurrentIndex);
		else
			signBoxCombo->setCurrentIndex(0);
		ui->tableWidget_findText->setCellWidget(insertRowIndex, 1, signBoxCombo);

		QComboBox *multiSignBoxCombo = new QComboBox();
		QList<DiscernMultiSignBoxConfig>::iterator allMultiSignBoxConfigIter = editSignBoxConfigDlg->allMultiSignBoxConfig.begin();
		multiSignBoxCombo->addItem(QString::fromLocal8Bit("空选"));
		while (allMultiSignBoxConfigIter != editSignBoxConfigDlg->allMultiSignBoxConfig.end())
		{
			multiSignBoxCombo->addItem(allMultiSignBoxConfigIter->groupName);
			allMultiSignBoxConfigIter++;
		}
		comboCurrentIndex = allFindTextConfig[insertRowIndex].multiSignBoxConfigId;
		if (comboCurrentIndex >= 0 && comboCurrentIndex < multiSignBoxCombo->count())
			multiSignBoxCombo->setCurrentIndex(comboCurrentIndex);
		else
			multiSignBoxCombo->setCurrentIndex(0);
		ui->tableWidget_findText->setCellWidget(insertRowIndex, 2, multiSignBoxCombo);

		allFindTextIter++;
	}
}


void GoldChooseDiscernFrameConfig::closeEvent(QCloseEvent * e)
{
	SendAppMsg((HWND)this->winId() , (PVOID)"AppExited");
	qApp->exit();
	e->ignore();
}

void GoldChooseDiscernFrameConfig::onBtnEditSignBoxClicked()
{
	if (editSignBoxConfigDlg != 0)
	{
		disconnect(editSignBoxConfigDlg->ui.button_confirm, SIGNAL(clicked()), this, SLOT(onConfigNeedBeCleared()));
		GoldEditDiscernFrameConfig *tt = editSignBoxConfigDlg;
		editSignBoxConfigDlg = 0;
		delete tt;
	}
	editSignBoxConfigDlg = new GoldEditDiscernFrameConfig(this);
	editSignBoxConfigDlg->setWindowModality(Qt::WindowModal);
	editSignBoxConfigDlg->show();
	connect(editSignBoxConfigDlg->ui.button_confirm, SIGNAL(clicked()), this, SLOT(onConfigNeedBeCleared()));
}

void GoldChooseDiscernFrameConfig::onBtnConfirmClicked()
{
	writeAllFindTextConfig();
	SendAppMsg((HWND)this->winId(), (PVOID)"AppExited");
	qApp->exit();
}

void GoldChooseDiscernFrameConfig::onBtnCancelClicked()
{
	SendAppMsg((HWND)this->winId(), (PVOID)"AppExited");
	qApp->exit();
}

void GoldChooseDiscernFrameConfig::onConfigNeedBeCleared()
{
	QList<DiscernTextConfig>::iterator allFindTextIter = allFindTextConfig.begin();
	while (allFindTextIter != allFindTextConfig.end())
	{
		allFindTextIter->signBoxConfigId = 0;
		allFindTextIter->multiSignBoxConfigId = 0;
		allFindTextIter++;
	}

	refreshFindTextTableWidget();
	writeAllFindTextConfig();
}