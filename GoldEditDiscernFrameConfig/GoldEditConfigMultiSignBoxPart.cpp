#include "GoldEditDiscernFrameConfig.h"
#include <list>

bool sortDiscernMultiSignBoxConfigByID(const DiscernMultiSignBoxConfig &s1, const DiscernMultiSignBoxConfig &s2)
{
	return s1.id < s2.id;
}

void GoldEditDiscernFrameConfig::readAllMultiSignBoxConfig()
{
	allMultiSignBoxConfig.clear();
	
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
	QDomElement signBoxElements = root.firstChildElement("MultiSignBoxOrientationList");
	if (signBoxElements.isNull())
		return;
	QDomElement groupElements = signBoxElements.firstChildElement("Group");
	while (!groupElements.isNull())
	{
		QDomElement itemElements = groupElements.firstChildElement("Item");
		QMap<QString, DiscernMultiSignBoxConfig::GroupItems> items;
		while (!itemElements.isNull())
		{
			DiscernMultiSignBoxConfig::GroupItems itemsAttr;
			QString roleName = itemElements.attributeNode("RoleNamePrefix").value();
			itemsAttr.showNamePrefix = itemElements.attributeNode("ShowNamePrefix").value();
			itemsAttr.refContent = itemElements.attributeNode("RefContent").value();
			itemsAttr.direction = itemElements.attributeNode("Direction").value().toInt();
			itemsAttr.step = itemElements.attributeNode("Step").value().toInt();
			itemsAttr.maxRepeat = itemElements.attributeNode("MaxRepeat").value().toInt();
			itemsAttr.maxRefSearchCount = itemElements.attributeNode("MaxRefSearchCount").value().toInt();
			items.insert(roleName, itemsAttr);
			itemElements = itemElements.nextSiblingElement();
		}

		DiscernMultiSignBoxConfig multiSignBoxConfig;
		multiSignBoxConfig.id = groupElements.attributeNode("ID").value().toInt();
		multiSignBoxConfig.groupName = groupElements.attributeNode("GroupName").value();
		multiSignBoxConfig.isErased = false;
		multiSignBoxConfig.items = items;
		allMultiSignBoxConfig.append(multiSignBoxConfig);
		groupElements = groupElements.nextSiblingElement("Group");
	}
}

void GoldEditDiscernFrameConfig::writeAllMultiSignBoxConfig()
{
	if (!reorderAllMultiSignBoxConfig())
	{
		QMessageBox::critical(this, QString::fromLocal8Bit("错误"), QString::fromLocal8Bit("会签签名框设置有误，已取消对会签设置做出更改\t\t\n"));
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

	QDomElement signBoxElement = root.firstChildElement("MultiSignBoxOrientationList");
	if (!signBoxElement.isNull())
		root.removeChild(signBoxElement);

	signBoxElement = document.createElement("MultiSignBoxOrientationList");
	root.appendChild(signBoxElement);

	QList<DiscernMultiSignBoxConfig>::iterator allMultiSignBoxConfigIter = allMultiSignBoxConfig.begin();
	while (allMultiSignBoxConfigIter != allMultiSignBoxConfig.end())
	{
		QDomElement groupElement = document.createElement("Group");
		groupElement.setAttribute("ID", QString::number(allMultiSignBoxConfigIter->id));
		groupElement.setAttribute("GroupName", allMultiSignBoxConfigIter->groupName);
		signBoxElement.appendChild(groupElement);
		MultiSignBoxGroupItems::iterator itemsIter = allMultiSignBoxConfigIter->items.begin();
		while (itemsIter != allMultiSignBoxConfigIter->items.end())
		{
			QDomElement itemElement = document.createElement("Item");
			itemElement.setAttribute("RoleNamePrefix", itemsIter.key());
			itemElement.setAttribute("ShowNamePrefix", itemsIter->showNamePrefix);
			itemElement.setAttribute("RefContent", itemsIter->refContent);
			itemElement.setAttribute("Direction", itemsIter->direction);
			itemElement.setAttribute("Step", itemsIter->step);
			itemElement.setAttribute("MaxRepeat", itemsIter->maxRepeat);
			itemElement.setAttribute("MaxRefSearchCount", itemsIter->maxRefSearchCount);
			groupElement.appendChild(itemElement);
			itemsIter++;
		}
		allMultiSignBoxConfigIter++;
	}
	if (!file.open(QFile::WriteOnly | QFile::Text))
		return;
	QTextStream fileStream(&file);
	document.save(fileStream, 4, QDomNode::EncodingFromDocument);
	file.close();
}

bool GoldEditDiscernFrameConfig::reorderAllMultiSignBoxConfig(bool isExplicitCheck)
{
	if (isExplicitCheck && allMultiSignBoxConfig.length() != ui.listWidget_multiGroup->count())
	{
		return false;
	}
	QList<DiscernMultiSignBoxConfig> allMultiSignBoxConfigBack = allMultiSignBoxConfig;
	
	QSet<int> handledConfigIndex;
	for (int i = 0; i < ui.listWidget_multiGroup->count(); i++)
	{
		QListWidgetItem *listItem = ui.listWidget_multiGroup->item(i);
		//listItem->setFlags(listItem->flags() | Qt::ItemIsEditable);
		int configIndex = listItem->data(Qt::AccessibleDescriptionRole).toInt();
		if (handledConfigIndex.contains(configIndex))
		{
			if (isExplicitCheck)
			{
				allMultiSignBoxConfig = allMultiSignBoxConfigBack;
				return false;
			}
			else
				continue;
		}
		handledConfigIndex.insert(configIndex);
		allMultiSignBoxConfig[configIndex - 1].id = i + 1;
	}

	if (allMultiSignBoxConfig.length() <= 1)
		return true;

	qSort(allMultiSignBoxConfig.begin(), allMultiSignBoxConfig.end(), sortDiscernMultiSignBoxConfigByID);

	for (int i = 0; i < allMultiSignBoxConfig.length(); i++)
	{
		if (allMultiSignBoxConfig[i].isErased)
		{
			allMultiSignBoxConfig.removeAt(i);
			i--;
		}
		else
		{
			allMultiSignBoxConfig[i].id = i + 1;
		}
	}

	return true;
}

void GoldEditDiscernFrameConfig::refreshMultiTableWidget(bool isAskForSave)
{
	if (isAskForSave && isMultiTableWidgetItemModified(curMultiTableWidgetRefConfigIndex))
	{
		QMessageBox::StandardButton returnButton = QMessageBox::question(this, QString::fromLocal8Bit("会签签名框配置"), QString::fromLocal8Bit("检测到会签条目已更改，且未被记录，是否记录本次更改？"));
		if (returnButton == QMessageBox::Yes)
		{
			if (curMultiTableWidgetRefConfigIndex >= 0 && curMultiTableWidgetRefConfigIndex < allMultiSignBoxConfig.length())
				onBtnApplyMultiItemClicked(curMultiTableWidgetRefConfigIndex);
		}
	}

	while (ui.tableWidget_multiItem->rowCount() > 0)
	{
		QPushButton *cellButton = (QPushButton *)ui.tableWidget_multiItem->cellWidget(0, 1);
		if (cellButton != 0)
			disconnect(cellButton, SIGNAL(clicked()), this, SLOT(onMultiTableWidgetButtonClicked()));
		ui.tableWidget_multiItem->removeRow(0);
	}
	if (ui.listWidget_multiGroup->currentItem() == 0)
	{
		ui.combo_multiItem_direction->setEnabled(false);
		ui.tableWidget_multiItem->setEnabled(false);
		ui.button_addMultiItem->setEnabled(false);
		ui.button_deleteMultiItem->setEnabled(false);
		ui.button_applyMultiItem->setEnabled(false);
		ui.button_restoreMultiItem->setEnabled(false);
		return;
	}
	else
	{
		ui.combo_multiItem_direction->setEnabled(true);
		ui.tableWidget_multiItem->setEnabled(true);
		ui.button_addMultiItem->setEnabled(true);
		ui.button_deleteMultiItem->setEnabled(true);
		ui.button_applyMultiItem->setEnabled(true);
		ui.button_restoreMultiItem->setEnabled(true);
	}

	int configIndex = ui.listWidget_multiGroup->currentItem()->data(Qt::AccessibleDescriptionRole).toInt() - 1;

	if (configIndex >= 0 && configIndex < allMultiSignBoxConfig.length())
	{
		curMultiItemsInTableWidget.clear();
		curMultiTableWidgetRefConfigIndex = configIndex;
		curMultiItemsInTableWidget = allMultiSignBoxConfig[configIndex].items;
		curMultiRoleNameInTableWidget.clear();
		MultiSignBoxGroupItems groupItems = allMultiSignBoxConfig[configIndex].items;
		MultiSignBoxGroupItems::iterator groupItemsIter = groupItems.begin();
		if (groupItems.size() > 0)
			ui.combo_multiItem_direction->setCurrentIndex(groupItemsIter->direction);
		while (groupItemsIter != groupItems.end())
		{
			int insertRowIndex = ui.tableWidget_multiItem->rowCount();
			ui.tableWidget_multiItem->insertRow(insertRowIndex);
			QString roleName = groupItemsIter.key();
			QTableWidgetItem *tableItem = new QTableWidgetItem(roleName);
			tableItem->setTextAlignment(Qt::AlignCenter);
			tableItem->setFlags(tableItem->flags() & ~(Qt::ItemIsEditable));
			ui.tableWidget_multiItem->setItem(insertRowIndex, 0, tableItem);
			QPushButton *button = new QPushButton(QString::fromLocal8Bit("设置"));
			button->setStyleSheet("border:none;background:transparent;color:blue;");
			button->setCursor(Qt::PointingHandCursor);
			connect(button, SIGNAL(clicked()), this, SLOT(onMultiTableWidgetButtonClicked()));
			ui.tableWidget_multiItem->setCellWidget(insertRowIndex, 1, button);
			curMultiRoleNameInTableWidget.push_back(roleName);
			groupItemsIter++;
		}
	}
	else
	{
		curMultiTableWidgetRefConfigIndex = -1;
		curMultiItemsInTableWidget.clear();
	}

	DrawMultiTablePreview();
}

bool GoldEditDiscernFrameConfig::isMultiTableWidgetItemModified(int verifyFromConfigIndex)
{
	int configIndex;
	if (verifyFromConfigIndex == -2)
		configIndex = ui.listWidget_multiGroup->currentItem()->data(Qt::AccessibleDescriptionRole).toInt() - 1;
	else
		configIndex = verifyFromConfigIndex;


	if (configIndex < 0 || configIndex >= allMultiSignBoxConfig.length())
		return false;
	else
	{
		if (isEqual(allMultiSignBoxConfig[configIndex].items, curMultiItemsInTableWidget))
			return false;
		else
			return true;
	}
}
bool GoldEditDiscernFrameConfig::isEqual(const MultiSignBoxGroupItems &first, const MultiSignBoxGroupItems &second) const
{
	if (first.size() != second.size())
		return false;
	MultiSignBoxGroupItems::const_iterator firstIter = first.begin();
	while (firstIter != first.end())
	{
		QString key = firstIter.key();
		if (!second.contains(key))
			return false;
		if (second[key].direction != firstIter->direction)
			return false;
		if (second[key].refContent != firstIter->refContent)
			return false;
		if (second[key].showNamePrefix != firstIter->showNamePrefix)
			return false;
		if (second[key].step != firstIter->step)
			return false;
		if (second[key].maxRepeat != firstIter->maxRepeat)
			return false;
		if (second[key].maxRefSearchCount != firstIter->maxRefSearchCount)
			return false;
		firstIter++;
	}
	return true;
}


void GoldEditDiscernFrameConfig::onBtnAddMultiGroupClicked()
{
	QListWidgetItem *listItem = new QListWidgetItem(QString::fromLocal8Bit("新建行"));
	//listItem->setFlags(listItem->flags() | Qt::ItemIsEditable);
	listItem->setData(Qt::AccessibleDescriptionRole, QVariant(allMultiSignBoxConfig.length() + 1));
	ui.listWidget_multiGroup->addItem(listItem);
	DiscernMultiSignBoxConfig newGroup;
	newGroup.id = allMultiSignBoxConfig.length() + 1;
	newGroup.groupName = listItem->text();
	newGroup.isErased = false;
	allMultiSignBoxConfig.push_back(newGroup);
}

void GoldEditDiscernFrameConfig::onBtnDeleteMultiGroupClicked()
{
	QList<QListWidgetItem *> selectedItems = ui.listWidget_multiGroup->selectedItems();
	QList<QListWidgetItem *>::iterator selectedItemsIter = selectedItems.end() - 1;
	while (selectedItemsIter != selectedItems.begin() - 1)
	{
		QString groupName = (*selectedItemsIter)->text();
		int rowIndex = ui.listWidget_multiGroup->row(*selectedItemsIter);
		ui.listWidget_multiGroup->setRowHidden(rowIndex, true);
		int configIndex = (*selectedItemsIter)->data(Qt::AccessibleDescriptionRole).toInt() - 1;
		if (configIndex >= 0 && configIndex < allMultiSignBoxConfig.length())
			allMultiSignBoxConfig[configIndex].isErased = true;
		selectedItemsIter--;
	}
}

void GoldEditDiscernFrameConfig::onMultiListWidgetCurSelectTextChanged(const QString &)
{
	refreshMultiTableWidget();
}

void GoldEditDiscernFrameConfig::onMultiListWidgetItemChanged(QListWidgetItem * item)
{
	int configIndex = item->data(Qt::AccessibleDescriptionRole).toInt() - 1;
	if (configIndex >= 0 && configIndex < allMultiSignBoxConfig.length())
	{
		allMultiSignBoxConfig[configIndex].groupName = item->text();
		refreshMultiTableWidget(configIndex);
	}
	else
		refreshMultiTableWidget();
}

void GoldEditDiscernFrameConfig::onBtnAddMultiItemClicked()
{
	int i = 1;
	while (curMultiItemsInTableWidget.contains(QString::fromLocal8Bit("新建项") + QString::number(i)))
	{
		i++;
	}

	int insertRowIndex = ui.tableWidget_multiItem->rowCount();
	ui.tableWidget_multiItem->insertRow(insertRowIndex);
	QTableWidgetItem *tableItem = new QTableWidgetItem(QString::fromLocal8Bit("新建项") + QString::number(i));
	tableItem->setTextAlignment(Qt::AlignCenter);
	tableItem->setFlags(tableItem->flags() & ~(Qt::ItemIsEditable));
	ui.tableWidget_multiItem->setItem(insertRowIndex, 0, tableItem);
	QPushButton *button = new QPushButton(QString::fromLocal8Bit("设置"));
	button->setStyleSheet("border:none;background:transparent;color:blue;");
	button->setCursor(Qt::PointingHandCursor);
	connect(button, SIGNAL(clicked()), this, SLOT(onMultiTableWidgetButtonClicked()));
	ui.tableWidget_multiItem->setCellWidget(insertRowIndex, 1, button);
	DiscernMultiSignBoxConfig::GroupItems newItems;
	newItems.refContent = QString::fromLocal8Bit("新建项") + QString::number(i);
	curMultiItemsInTableWidget.insert(QString::fromLocal8Bit("新建项") + QString::number(i), newItems);
	curMultiRoleNameInTableWidget.push_back(QString::fromLocal8Bit("新建项") + QString::number(i));

	DrawMultiTablePreview();
}

void GoldEditDiscernFrameConfig::onBtnDeleteMultiItemClicked()
{
	QList<QTableWidgetItem *>allSelectedItems = ui.tableWidget_multiItem->selectedItems();
	std::list<int> allSelectedRowNumbers;
	QList<QTableWidgetItem *>::iterator allSelectedItemsIter = allSelectedItems.begin();
	while (allSelectedItemsIter != allSelectedItems.end())
	{
		allSelectedRowNumbers.push_back(ui.tableWidget_multiItem->row(*allSelectedItemsIter));
		allSelectedItemsIter++;
	}
	allSelectedRowNumbers.sort();
	allSelectedRowNumbers.unique();
	std::list<int>::reverse_iterator allSelectedRowNumbersIter = allSelectedRowNumbers.rbegin();
	while (allSelectedRowNumbersIter != allSelectedRowNumbers.rend())
	{
		if (*allSelectedRowNumbersIter >= 0 && *allSelectedRowNumbersIter < ui.tableWidget_multiItem->rowCount())
			ui.tableWidget_multiItem->removeRow(*allSelectedRowNumbersIter);
		QPushButton *cellButton = (QPushButton *)ui.tableWidget_multiItem->cellWidget(*allSelectedRowNumbersIter, 1);
		if (cellButton != 0)
			disconnect(cellButton, SIGNAL(clicked()), this, SLOT(onMultiTableWidgetButtonClicked()));
		curMultiItemsInTableWidget.remove(curMultiRoleNameInTableWidget[*allSelectedRowNumbersIter]);
		curMultiRoleNameInTableWidget.removeAt(*allSelectedRowNumbersIter);
		allSelectedRowNumbersIter++;
	}

	DrawMultiTablePreview();
}

void GoldEditDiscernFrameConfig::onBtnRestoreMultiItemClicked()
{
	refreshMultiTableWidget(false);
}
void GoldEditDiscernFrameConfig::onBtnApplyMultiItemClicked(int applyOnConfigIndex)
{
	int configIndex;
	if (applyOnConfigIndex == -2)
		configIndex = ui.listWidget_multiGroup->currentItem()->data(Qt::AccessibleDescriptionRole).toInt() - 1;
	else
		configIndex = applyOnConfigIndex;
	if (configIndex >= 0 && configIndex < allMultiSignBoxConfig.length())
	{
		allMultiSignBoxConfig[configIndex].items.clear();
		allMultiSignBoxConfig[configIndex].items = curMultiItemsInTableWidget;
	}
}

void GoldEditDiscernFrameConfig::onMultiTableWidgetButtonClicked()
{
	QPushButton *cellButton = (QPushButton *)sender();
	int rowIndex = -1;
	for (int i = 0; i < ui.tableWidget_multiItem->rowCount(); i++)
	{
		if (cellButton == (QPushButton *)ui.tableWidget_multiItem->cellWidget(i, 1))
		{
			rowIndex = i;
			break;
		}
	}
	if (rowIndex == -1)
		return;

	if (rowIndex >= 0 && rowIndex < curMultiRoleNameInTableWidget.length())
	{
		QString roleNamePrefix = curMultiRoleNameInTableWidget[rowIndex];
		QString showNamePrefix = curMultiItemsInTableWidget[roleNamePrefix].showNamePrefix;
		QString refContent = curMultiItemsInTableWidget[roleNamePrefix].refContent;
		int step = curMultiItemsInTableWidget[roleNamePrefix].step;
		//int direction = curMultiItemsInTableWidget[roleNamePrefix].direction;
		int maxRepeat = curMultiItemsInTableWidget[roleNamePrefix].maxRepeat;
		int maxSearchRefCount = curMultiItemsInTableWidget[roleNamePrefix].maxRefSearchCount;
		if (refContent.isEmpty())
		{
			refContent = roleNamePrefix;
			curMultiItemsInTableWidget[roleNamePrefix].refContent = refContent;
		}
		multiSignBoxEditor->ui.lineEdit_roleName->setText(roleNamePrefix);
		multiSignBoxEditor->ui.lineEdit_refContent->setText(refContent);
		multiSignBoxEditor->ui.combo_step->setCurrentIndex(step);
		multiSignBoxEditor->ui.combo_maxRepeat->setCurrentIndex(maxRepeat - 1);
		multiSignBoxEditor->ui.combo_maxRefSearchCount->setCurrentIndex(maxSearchRefCount - 1);

		multiSignBoxEditor->curTableWidgetIndex = rowIndex;
		
		if (!multiSignBoxEditor->isVisible())
		{
			animation.setTargetObject(multiSignBoxEditor);
			animation.setPropertyName("geometry");
			animation.setDuration(100);
			QDesktopWidget deskTopWidget;
			QRect screenRect = deskTopWidget.availableGeometry(this);
			QPoint cellPosToThis = cursor().pos();
			QPoint cellGlobalPos = cursor().pos(qApp->primaryScreen());
			int startX = cellPosToThis.x();
			int startY = cellPosToThis.y();
			int endX = startX;
			int endY = startY;
			if (screenRect.height() < cellGlobalPos.y() + multiSignBoxEditor->height)
			{
				endY = startY - multiSignBoxEditor->height;
			}
			if (screenRect.width() < cellGlobalPos.x() + multiSignBoxEditor->width)
			{
				startX = cellPosToThis.x() - multiSignBoxEditor->width;
				endX = startX;
			}
			multiSignBoxEditor->resize(multiSignBoxEditor->width, 1);
			multiSignBoxEditor->move(startX, startY);
			multiSignBoxEditor->show();
			animation.setStartValue(QRect(startX , startY , multiSignBoxEditor->width , 0));
			animation.setEndValue(QRect(endX, endY , multiSignBoxEditor->width, multiSignBoxEditor->height));
			animation.start();
		}

		multiSignBoxEditor->activateWindow();
	}
}

void GoldEditDiscernFrameConfig::onMultiDockWidgetTopLevelChanged(bool isFloating)
{
	if (isFloating)
	{
		int curHeight = ui.dock_editMultiItem->height();
		QRect curRect = ui.dock_editMultiItem->geometry();
		ui.dock_editMultiItem->setGeometry(curRect.x() - 5, curRect.y() - 300 + curHeight - 5, curRect.width(), 300);
	}
}

void GoldEditDiscernFrameConfig::onMultiSignBoxEditorBtnApplyClicked()
{
	if (multiSignBoxEditor->curTableWidgetIndex < 0
		|| multiSignBoxEditor->curTableWidgetIndex >= ui.tableWidget_multiItem->rowCount())
	{
		QMessageBox::critical(this, QString::fromLocal8Bit("错误"), QString::fromLocal8Bit("内部出现错误，请再次选择条目编辑\t\t\n"));
	}
	
	if (multiSignBoxEditor->ui.lineEdit_refContent->text().isEmpty()
		|| multiSignBoxEditor->ui.lineEdit_roleName->text().isEmpty()
		|| multiSignBoxEditor->ui.combo_maxRefSearchCount->currentIndex() < 0
		|| multiSignBoxEditor->ui.combo_maxRepeat->currentIndex() < 0
		|| multiSignBoxEditor->ui.combo_step->currentIndex() < 0)
	{
		QMessageBox::warning(this, QString::fromLocal8Bit("警告"), QString::fromLocal8Bit("检测到部分条目为空值，请确认并再次应用\t\t\n"));
	}

	QString roleName = multiSignBoxEditor->ui.lineEdit_roleName->text();
	int findRoleNameIndex = curMultiRoleNameInTableWidget.indexOf(roleName);
	if (findRoleNameIndex >= 0 && findRoleNameIndex != multiSignBoxEditor->curTableWidgetIndex)
	{
		QMessageBox::warning(this, QString::fromLocal8Bit("警告"), QString::fromLocal8Bit("检测到了此签名框已设置，请设置新的签名框\t\t\n"));
	}

	QString oldRoleName = curMultiRoleNameInTableWidget[multiSignBoxEditor->curTableWidgetIndex];
	curMultiRoleNameInTableWidget[multiSignBoxEditor->curTableWidgetIndex] = roleName;
	curMultiItemsInTableWidget.remove(oldRoleName);
	DiscernMultiSignBoxConfig::GroupItems items;
	items.direction = ui.combo_multiItem_direction->currentIndex();
	items.maxRefSearchCount = multiSignBoxEditor->ui.combo_maxRefSearchCount->currentIndex() + 1;
	items.maxRepeat = multiSignBoxEditor->ui.combo_maxRepeat->currentIndex() + 1;
	items.refContent = multiSignBoxEditor->ui.lineEdit_refContent->text();
	items.showNamePrefix = multiSignBoxEditor->ui.lineEdit_roleName->text();
	items.step = multiSignBoxEditor->ui.combo_step->currentIndex();
	curMultiItemsInTableWidget.insert(roleName, items);

	ui.tableWidget_multiItem->item(multiSignBoxEditor->curTableWidgetIndex , 0)->setText(roleName);

	DrawMultiTablePreview();
}

void GoldEditDiscernFrameConfig::onMultiComboDirectionSelChanged(int sel)
{
	MultiSignBoxGroupItems::iterator curMultiItemsInTableWidgetIter = curMultiItemsInTableWidget.begin();
	while (curMultiItemsInTableWidgetIter != curMultiItemsInTableWidget.end())
	{
		curMultiItemsInTableWidgetIter->direction = sel;
		curMultiItemsInTableWidgetIter++;
	}
	DrawMultiTablePreview();
}