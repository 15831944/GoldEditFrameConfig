#include "GoldEditDiscernFrameConfig.h"
#include <list>

bool sortDiscernSignBoxConfigByID(const DiscernSignBoxConfig &s1, const DiscernSignBoxConfig &s2)
{
	return s1.id < s2.id;
}

void GoldEditDiscernFrameConfig::readAllSignBoxConfig()
{
	allSignBoxConfig.clear();
	
	if (!QFile::exists(xmlPath) && QFile::exists(xmlPathInConfig))
	{
		QFile::copy(xmlPathInConfig, xmlPath);
	}

	//通过DOM方式读取xml
	QFile file(xmlPath);
	if (!file.open(QFile::ReadOnly | QFile::Text))
		return;
	QDomDocument document;
	if (!document.setContent(&file))
		return;
	file.close();
	QDomElement root = document.documentElement();
	if (root.isNull())
		return;
	QDomElement signBoxElements = root.firstChildElement("SignBoxOrientationList");
	if (signBoxElements.isNull())
		return;
	QDomElement groupElements = signBoxElements.firstChildElement("Group");
	while (!groupElements.isNull())
	{
		QDomElement itemElements = groupElements.firstChildElement("Item");
		QMap<QString, DiscernSignBoxConfig::GroupItems> items;
		while (!itemElements.isNull())
		{
			DiscernSignBoxConfig::GroupItems itemsAttr;
			QString roleName = itemElements.attributeNode("RoleName").value();
			itemsAttr.showName = itemElements.attributeNode("ShowName").value();
			itemsAttr.refContent = itemElements.attributeNode("RefContent").value();
			itemsAttr.direction = itemElements.attributeNode("Direction").value().toInt();
			itemsAttr.step = itemElements.attributeNode("Step").value().toInt();
			items.insert(roleName, itemsAttr);
			itemElements = itemElements.nextSiblingElement();
		}

		DiscernSignBoxConfig signBoxConfig;
		signBoxConfig.id = groupElements.attributeNode("ID").value().toInt();
		signBoxConfig.groupName = groupElements.attributeNode("GroupName").value();
		signBoxConfig.isErased = false;
		signBoxConfig.items = items;
		allSignBoxConfig.append(signBoxConfig);
		groupElements = groupElements.nextSiblingElement("Group");
	}
}

void GoldEditDiscernFrameConfig::writeAllSignBoxConfig()
{
	if (!reorderAllSignBoxConfig())
	{
		QMessageBox::critical(this, QString::fromLocal8Bit("错误"), QString::fromLocal8Bit("图签签名框设置有误，已取消对图签设置做出更改\t\t\n"));
		return;
	}
	
	//通过DOM方式读取xml
	QFile file(xmlPath);
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

	QDomElement signBoxElement = root.firstChildElement("SignBoxOrientationList");
	if (!signBoxElement.isNull())
		root.removeChild(signBoxElement);

	signBoxElement = document.createElement("SignBoxOrientationList");
	root.appendChild(signBoxElement);

	QList<DiscernSignBoxConfig>::iterator allSignBoxConfigIter = allSignBoxConfig.begin();
	while (allSignBoxConfigIter != allSignBoxConfig.end())
	{
		QDomElement groupElement = document.createElement("Group");
		groupElement.setAttribute("ID", QString::number(allSignBoxConfigIter->id));
		groupElement.setAttribute("GroupName", allSignBoxConfigIter->groupName);
		signBoxElement.appendChild(groupElement);
		SignBoxGroupItems::iterator itemsIter = allSignBoxConfigIter->items.begin();
		while (itemsIter != allSignBoxConfigIter->items.end())
		{
			QDomElement itemElement = document.createElement("Item");
			itemElement.setAttribute("RoleName", itemsIter.key());
			itemElement.setAttribute("ShowName", itemsIter->showName);
			itemElement.setAttribute("RefContent", itemsIter->refContent);
			itemElement.setAttribute("Direction", itemsIter->direction);
			itemElement.setAttribute("Step", itemsIter->step);
			groupElement.appendChild(itemElement);
			itemsIter++;
		}
		allSignBoxConfigIter++;
	}
	if (!file.open(QFile::WriteOnly | QFile::Text))
		return;
	QTextStream fileStream(&file);
	document.save(fileStream ,4, QDomNode::EncodingFromDocument);
	file.close();
}

bool GoldEditDiscernFrameConfig::reorderAllSignBoxConfig(bool isExplicitCheck)
{
	if (isExplicitCheck && allSignBoxConfig.length() != ui.listWidget_group->count())
	{
		return false;
	}
	QList<DiscernSignBoxConfig> allSignBoxConfigBack = allSignBoxConfig;
	
	QSet<int> handledConfigIndex;
	for (int i = 0; i < ui.listWidget_group->count(); i++)
	{
		QListWidgetItem *listItem = ui.listWidget_group->item(i);
	//	listItem->setFlags(listItem->flags() | Qt::ItemIsEditable);
		int configIndex = listItem->data(Qt::AccessibleDescriptionRole).toInt();
		if (handledConfigIndex.contains(configIndex))
		{
			if (isExplicitCheck)
			{
				allSignBoxConfig = allSignBoxConfigBack;
				return false;
			}
			else
				continue;
		}
		handledConfigIndex.insert(configIndex);
		allSignBoxConfig[configIndex - 1].id = i + 1;
	}

	if (allSignBoxConfig.length() <= 0)
		return true;

	qSort(allSignBoxConfig.begin(), allSignBoxConfig.end(), sortDiscernSignBoxConfigByID);

	for (int i = 0; i < allSignBoxConfig.length(); i++)
	{
		if (allSignBoxConfig[i].isErased)
		{
			allSignBoxConfig.removeAt(i);
			i--;
		}
		else
		{
			allSignBoxConfig[i].id = i + 1;
		}

	}
	return true;
}

void GoldEditDiscernFrameConfig::refreshTableWidget(bool isAskForSave)
{
	if (isAskForSave && isTableWidgetItemModified(curTableWidgetRefConfigIndex))
	{
		QMessageBox::StandardButton returnButton = QMessageBox::question(this , QString::fromLocal8Bit("图签签名框配置") , QString::fromLocal8Bit("检测到图签条目已更改，且未被记录，是否记录本次更改？"));
		if (returnButton == QMessageBox::Yes)
		{
			if (curTableWidgetRefConfigIndex >=0 && curTableWidgetRefConfigIndex < allSignBoxConfig.length())
				onBtnApplyItemClicked(curTableWidgetRefConfigIndex);
		}
	}

	while (ui.tableWidget_item->rowCount() > 0)
	{
		ui.tableWidget_item->removeRow(0);
	}
	if (ui.listWidget_group->currentItem() == 0)
	{
		ui.tableWidget_item->setEnabled(false);
		ui.button_addItem->setEnabled(false);
		ui.button_deleteItem->setEnabled(false);
		ui.button_applyItem->setEnabled(false);
		ui.button_restoreItem->setEnabled(false);
		return;
	}
	else
	{
		ui.tableWidget_item->setEnabled(true);
		ui.button_addItem->setEnabled(true);
		ui.button_deleteItem->setEnabled(true);
		ui.button_applyItem->setEnabled(true);
		ui.button_restoreItem->setEnabled(true);
	}

	int configIndex = ui.listWidget_group->currentItem()->data(Qt::AccessibleDescriptionRole).toInt() - 1;

	if (configIndex >= 0 && configIndex < allSignBoxConfig.length())
	{
		curTableWidgetRefConfigIndex = configIndex;
		QMap<QString, DiscernSignBoxConfig::GroupItems> groupItems = allSignBoxConfig[configIndex].items;
		QMap<QString, DiscernSignBoxConfig::GroupItems>::iterator groupItemsIter = groupItems.begin();
		while (groupItemsIter != groupItems.end())
		{
			int insertRowIndex = ui.tableWidget_item->rowCount();
			ui.tableWidget_item->insertRow(insertRowIndex);
			QTableWidgetItem *roleItem = new QTableWidgetItem(groupItemsIter.key());
			roleItem->setTextAlignment(Qt::AlignCenter);
			ui.tableWidget_item->setItem(insertRowIndex, 0, roleItem);
			QTableWidgetItem *showNameItem = new QTableWidgetItem(groupItemsIter->showName);
			showNameItem->setTextAlignment(Qt::AlignCenter);
			ui.tableWidget_item->setItem(insertRowIndex, 1, showNameItem);
			QTableWidgetItem *refContentNameItem = new QTableWidgetItem(groupItemsIter->refContent);
			refContentNameItem->setTextAlignment(Qt::AlignCenter);
			ui.tableWidget_item->setItem(insertRowIndex, 2, refContentNameItem);
			QComboBox *directionCombo = new QComboBox();
			directionCombo->addItem(QString::fromLocal8Bit("左"));
			directionCombo->addItem(QString::fromLocal8Bit("右"));
			directionCombo->addItem(QString::fromLocal8Bit("上"));
			directionCombo->addItem(QString::fromLocal8Bit("下"));
			directionCombo->setCurrentIndex(groupItemsIter->direction);
			ui.tableWidget_item->setCellWidget(insertRowIndex, 3, directionCombo);
			QLineEdit *stepEdit = new QLineEdit(QString::number(groupItemsIter->step));
			stepEdit->setAlignment(Qt::AlignCenter);
			stepEdit->setValidator(new QIntValidator(0, 9));
			stepEdit->setStyleSheet("background:transparent;border:none");
			ui.tableWidget_item->setCellWidget(insertRowIndex, 4, stepEdit);

			groupItemsIter++;
		}
	}
	else
		curTableWidgetRefConfigIndex = -1;
}

bool GoldEditDiscernFrameConfig::isTableWidgetItemModified(int verifyFromConfigIndex)
{
	QMap<QString, DiscernSignBoxConfig::GroupItems> allItems;
	for (int rowIndex = 0; rowIndex < ui.tableWidget_item->rowCount(); rowIndex++)
	{
		QString roleName = ui.tableWidget_item->item(rowIndex, 0)->text();
		DiscernSignBoxConfig::GroupItems itemInfo;
		itemInfo.showName = ui.tableWidget_item->item(rowIndex, 1)->text();
		itemInfo.refContent = ui.tableWidget_item->item(rowIndex, 2)->text();
		itemInfo.direction = ((QComboBox *)(ui.tableWidget_item->cellWidget(rowIndex, 3)))->currentIndex();
		itemInfo.step = ((QLineEdit *)(ui.tableWidget_item->cellWidget(rowIndex, 4)))->text().toInt();
		if (allItems.contains(roleName))
		{
			//QMessageBox::critical(this, QString::fromLocal8Bit("错误"), QString::fromLocal8Bit("签名角色不可重复\t\t\n"));
			return true;
		}
		allItems.insert(roleName, itemInfo);
	}

	int configIndex;
	if (verifyFromConfigIndex == -2)
		configIndex = ui.listWidget_group->currentItem()->data(Qt::AccessibleDescriptionRole).toInt() - 1;
	else
		configIndex = verifyFromConfigIndex;

	if (configIndex < 0 || configIndex >= allSignBoxConfig.length())
		return false;
	else
	{
		if (isEqual(allSignBoxConfig[configIndex].items, allItems))
			return false;
		else
			return true;
	}
}

bool GoldEditDiscernFrameConfig::isEqual(const SignBoxGroupItems &first, const SignBoxGroupItems &second) const
{
	if (first.size() != second.size())
		return false;
	SignBoxGroupItems::const_iterator firstIter = first.begin();
	while (firstIter != first.end())
	{
		QString key = firstIter.key();
		if (!second.contains(key))
			return false;
		if (second[key].direction != firstIter->direction)
			return false;
		if (second[key].refContent != firstIter->refContent)
			return false;
		if (second[key].showName != firstIter->showName)
			return false;
		if (second[key].step != firstIter->step)
			return false;
		firstIter++;
	}
	return true;
}



void GoldEditDiscernFrameConfig::onBtnAddGroupClicked()
{
	QListWidgetItem *listItem = new QListWidgetItem(QString::fromLocal8Bit("新建行"));
	//listItem->setFlags(listItem->flags() | Qt::ItemIsEditable);
	listItem->setData(Qt::AccessibleDescriptionRole, QVariant(allSignBoxConfig.length() + 1));
	ui.listWidget_group->addItem(listItem);
	DiscernSignBoxConfig newGroup;
	newGroup.id = allSignBoxConfig.length() + 1;
	newGroup.groupName = listItem->text();
	newGroup.isErased = false;
	allSignBoxConfig.push_back(newGroup);
}

void GoldEditDiscernFrameConfig::onBtnDeleteGroupClicked()
{
	QList<QListWidgetItem *> selectedItems = ui.listWidget_group->selectedItems();
	QList<QListWidgetItem *>::iterator selectedItemsIter = selectedItems.end() - 1;
	while (selectedItemsIter != selectedItems.begin() - 1)
	{
		QString groupName = (*selectedItemsIter)->text();
		int rowIndex = ui.listWidget_group->row(*selectedItemsIter);
		ui.listWidget_group->setRowHidden(rowIndex, true);
		int configIndex = (*selectedItemsIter)->data(Qt::AccessibleDescriptionRole).toInt() - 1;
		if (configIndex >= 0 && configIndex < allSignBoxConfig.length())
			allSignBoxConfig[configIndex].isErased = true;
		selectedItemsIter--;
	}
}

void GoldEditDiscernFrameConfig::onListWidgetCurSelectTextChanged(const QString &)
{
	refreshTableWidget();
}

void GoldEditDiscernFrameConfig::onListWidgetItemChanged(QListWidgetItem * item)
{
	int configIndex = item->data(Qt::AccessibleDescriptionRole).toInt() - 1;
	if (configIndex >= 0 && configIndex < allSignBoxConfig.length())
	{
		allSignBoxConfig[configIndex].groupName = item->text();
		refreshTableWidget(configIndex);
	}
	else
		refreshTableWidget();
}


void GoldEditDiscernFrameConfig::onBtnAddItemClicked()
{
	int insertRowIndex = ui.tableWidget_item->rowCount();
	ui.tableWidget_item->insertRow(insertRowIndex);
	QTableWidgetItem *roleItem = new QTableWidgetItem(QString::fromLocal8Bit("新建文本"));
	roleItem->setTextAlignment(Qt::AlignCenter);
	ui.tableWidget_item->setItem(insertRowIndex, 0, roleItem);
	QTableWidgetItem *showNameItem = new QTableWidgetItem(QString::fromLocal8Bit("新建文本"));
	showNameItem->setTextAlignment(Qt::AlignCenter);
	ui.tableWidget_item->setItem(insertRowIndex, 1, showNameItem);
	QTableWidgetItem *refContentNameItem = new QTableWidgetItem(QString::fromLocal8Bit("新建文本"));
	refContentNameItem->setTextAlignment(Qt::AlignCenter);
	ui.tableWidget_item->setItem(insertRowIndex, 2, refContentNameItem);
	QComboBox *directionCombo = new QComboBox();
	directionCombo->addItem(QString::fromLocal8Bit("左"));
	directionCombo->addItem(QString::fromLocal8Bit("右"));
	directionCombo->addItem(QString::fromLocal8Bit("上"));
	directionCombo->addItem(QString::fromLocal8Bit("下"));
	directionCombo->setCurrentIndex(1);
	ui.tableWidget_item->setCellWidget(insertRowIndex, 3, directionCombo);
	QLineEdit *stepEdit = new QLineEdit(QString::number(0));
	stepEdit->setAlignment(Qt::AlignCenter);
	stepEdit->setValidator(new QIntValidator(0, 9));
	stepEdit->setStyleSheet("background:transparent;border:none");
	ui.tableWidget_item->setCellWidget(insertRowIndex, 4, stepEdit);
	ui.tableWidget_item->scrollToBottom();
	ui.tableWidget_item->editItem(roleItem);
}

void GoldEditDiscernFrameConfig::onBtnDeleteItemClicked()
{
	QList<QTableWidgetItem *>allSelectedItems =  ui.tableWidget_item->selectedItems();
	std::list<int> allSelectedRowNumbers;
	QList<QTableWidgetItem *>::iterator allSelectedItemsIter = allSelectedItems.begin();
	while (allSelectedItemsIter != allSelectedItems.end())
	{
		allSelectedRowNumbers.push_back(ui.tableWidget_item->row(*allSelectedItemsIter));
		allSelectedItemsIter++;
	}
	allSelectedRowNumbers.sort();
	allSelectedRowNumbers.unique();
	std::list<int>::reverse_iterator allSelectedRowNumbersIter = allSelectedRowNumbers.rbegin();
	while (allSelectedRowNumbersIter != allSelectedRowNumbers.rend())
	{
		if (*allSelectedRowNumbersIter >= 0 && *allSelectedRowNumbersIter < ui.tableWidget_item->rowCount())
			ui.tableWidget_item->removeRow(*allSelectedRowNumbersIter);
		allSelectedRowNumbersIter++;
	}
}

void GoldEditDiscernFrameConfig::onBtnRestoreItemClicked()
{
	refreshTableWidget(false);
}

void GoldEditDiscernFrameConfig::onBtnApplyItemClicked(int applyOnIndex)
{
	QMap<QString, DiscernSignBoxConfig::GroupItems> allItems;
	for (int rowIndex = 0; rowIndex < ui.tableWidget_item->rowCount(); rowIndex++)
	{
		QString roleName = ui.tableWidget_item->item(rowIndex, 0)->text();
		DiscernSignBoxConfig::GroupItems itemInfo;
		itemInfo.showName = ui.tableWidget_item->item(rowIndex, 1)->text();
		itemInfo.refContent = ui.tableWidget_item->item(rowIndex, 2)->text();
		itemInfo.direction = ((QComboBox *)(ui.tableWidget_item->cellWidget(rowIndex, 3)))->currentIndex();
		itemInfo.step = ((QLineEdit *)(ui.tableWidget_item->cellWidget(rowIndex, 4)))->text().toInt();
		if (allItems.contains(roleName))
		{
			QMessageBox::critical(this, QString::fromLocal8Bit("错误"), QString::fromLocal8Bit("签名角色不可重复，已撤销本次条目更改\t\t\n"));
			return;
		}
		allItems.insert(roleName, itemInfo);
	}

	int configIndex;
	if (applyOnIndex == -2)
		configIndex = ui.listWidget_group->currentItem()->data(Qt::AccessibleDescriptionRole).toInt() - 1;
	else
		configIndex = applyOnIndex;
	if (configIndex >= 0 && configIndex < allSignBoxConfig.length())
	{
		allSignBoxConfig[configIndex].items.clear();
		allSignBoxConfig[configIndex].items = allItems;
	}
}
