#include "GoldEditDiscernFrameConfig.h"
#include <list>

void GoldEditDiscernFrameConfig::DrawMultiTablePreview()
{
	while (ui.tableWidget_multiItem_preview->rowCount() > 0)
		ui.tableWidget_multiItem_preview->removeRow(0);

	while (ui.tableWidget_multiItem_preview->columnCount() > 0)
		ui.tableWidget_multiItem_preview->removeColumn(0);

	if (curMultiItemsInTableWidget.size() <= 0)
	{
		CenterMultiTablePreview();
		return;
	}

	//enum Direction { Left = 0, Right, Top, Bottom };
	MultiSignBoxGroupItems::iterator curMultiItemsInTableWidgetIter = curMultiItemsInTableWidget.begin();
	int direction = curMultiItemsInTableWidgetIter->direction;
	
	std::list<int> maxSearchCountList;
	while (curMultiItemsInTableWidgetIter != curMultiItemsInTableWidget.end())
	{
		maxSearchCountList.push_back(curMultiItemsInTableWidgetIter->maxRefSearchCount);
		curMultiItemsInTableWidgetIter++;
	}
	maxSearchCountList.sort();
	int allMaxSearchCount;
	if (maxSearchCountList.size() > 0)
		allMaxSearchCount = *(--maxSearchCountList.end());
	else
		allMaxSearchCount = -1;

	switch (direction)
	{
	case DiscernMultiSignBoxConfig::Left:
		for (int index = 1; index <= allMaxSearchCount; index++)
		{
			curMultiItemsInTableWidgetIter = curMultiItemsInTableWidget.begin();
			while (curMultiItemsInTableWidgetIter != curMultiItemsInTableWidget.end())
			{
				int curMaxSearchCount = curMultiItemsInTableWidgetIter->maxRefSearchCount;
				if (curMaxSearchCount < index)
				{
					curMultiItemsInTableWidgetIter++;
					continue;
				}

				int curColumnCount = ui.tableWidget_multiItem_preview->columnCount();
				int curRowCount = ui.tableWidget_multiItem_preview->rowCount();

				//索引文本
				ui.tableWidget_multiItem_preview->insertRow(curRowCount);
				curRowCount++;
				if (curColumnCount <= 0)
				{
					ui.tableWidget_multiItem_preview->insertColumn(0);
					curColumnCount = 1;
				}

				QTableWidgetItem *item = new QTableWidgetItem(curMultiItemsInTableWidgetIter->refContent);
				item->setTextAlignment(Qt::AlignCenter);
				ui.tableWidget_multiItem_preview->setItem(curRowCount - 1, curColumnCount - 1, item);

				//签名框
				//考虑到当前表格中已有当前签名，判断当前表格中最大的签名框索引
				int ii = 1;
				while ((ui.tableWidget_multiItem_preview->findItems(curMultiItemsInTableWidgetIter.key() + QString::number(ii), Qt::MatchFixedString)).size() > 0)
				{
					ii++;
				}
				for (int i = 0; i < curMultiItemsInTableWidgetIter->maxRepeat; i++)
				{
					int columnSet = curColumnCount - 1 - 1 - curMultiItemsInTableWidgetIter->step - i;
					while (columnSet < 0)
					{
						ui.tableWidget_multiItem_preview->insertColumn(0);
						curColumnCount = ui.tableWidget_multiItem_preview->columnCount();
						columnSet = curColumnCount - 1 - 1 - curMultiItemsInTableWidgetIter->step - i;
					}
					QString roleName = curMultiItemsInTableWidgetIter.key() + QString::number(i + ii);
					QTableWidgetItem *item = new QTableWidgetItem(roleName);
					item->setTextAlignment(Qt::AlignCenter);
					item->setTextColor(QColor(0, 255, 0));
					ui.tableWidget_multiItem_preview->setItem(curRowCount - 1, columnSet, item);
				}

				curMultiItemsInTableWidgetIter++;
			}
		}
		break;

	case DiscernMultiSignBoxConfig::Right:
		for (int index = 1; index <= allMaxSearchCount; index++)
		{
			curMultiItemsInTableWidgetIter = curMultiItemsInTableWidget.begin();
			while (curMultiItemsInTableWidgetIter != curMultiItemsInTableWidget.end())
			{
				int curMaxSearchCount = curMultiItemsInTableWidgetIter->maxRefSearchCount;
				if (curMaxSearchCount < index)
				{
					curMultiItemsInTableWidgetIter++;
					continue;
				}

				int curColumnCount = ui.tableWidget_multiItem_preview->columnCount();
				int curRowCount = ui.tableWidget_multiItem_preview->rowCount();

				//索引文本
				ui.tableWidget_multiItem_preview->insertRow(curRowCount);
				curRowCount++;
				if (curColumnCount <= 0)
				{
					ui.tableWidget_multiItem_preview->insertColumn(0);
					curColumnCount = 1;
				}

				QTableWidgetItem *item = new QTableWidgetItem(curMultiItemsInTableWidgetIter->refContent);
				item->setTextAlignment(Qt::AlignCenter);
				ui.tableWidget_multiItem_preview->setItem(curRowCount - 1, 0, item);

				//签名框
				//考虑到当前表格中已有当前签名，判断当前表格中最大的签名框索引
				int ii = 1;
				while ((ui.tableWidget_multiItem_preview->findItems(curMultiItemsInTableWidgetIter.key() + QString::number(ii), Qt::MatchFixedString)).size() > 0)
				{
					ii++;
				}
				for (int i = 0; i < curMultiItemsInTableWidgetIter->maxRepeat; i++)
				{
					int columnSet = curMultiItemsInTableWidgetIter->step + i + 1;
					while (curColumnCount < columnSet + 1)
					{
						ui.tableWidget_multiItem_preview->insertColumn(curColumnCount);
						curColumnCount = ui.tableWidget_multiItem_preview->columnCount();
					}
					
					QString roleName = curMultiItemsInTableWidgetIter.key() + QString::number(i + ii);
					QTableWidgetItem *item = new QTableWidgetItem(roleName);
					item->setTextAlignment(Qt::AlignCenter);
					item->setTextColor(QColor(0, 255, 0));
					ui.tableWidget_multiItem_preview->setItem(curRowCount - 1, columnSet, item);
				}

				curMultiItemsInTableWidgetIter++;
			}
		}
		break;

	case DiscernMultiSignBoxConfig::Top:
		for (int index = 1; index <= allMaxSearchCount; index++)
		{
			curMultiItemsInTableWidgetIter = curMultiItemsInTableWidget.begin();
			while (curMultiItemsInTableWidgetIter != curMultiItemsInTableWidget.end())
			{
				int curMaxSearchCount = curMultiItemsInTableWidgetIter->maxRefSearchCount;
				if (curMaxSearchCount < index)
				{
					curMultiItemsInTableWidgetIter++;
					continue;
				}

				int curColumnCount = ui.tableWidget_multiItem_preview->columnCount();
				int curRowCount = ui.tableWidget_multiItem_preview->rowCount();

				//索引文本
				ui.tableWidget_multiItem_preview->insertColumn(curColumnCount);
				curColumnCount++;
				if (curRowCount <= 0)
				{
					ui.tableWidget_multiItem_preview->insertRow(0);
					curRowCount = 1;
				}

				QTableWidgetItem *item = new QTableWidgetItem(curMultiItemsInTableWidgetIter->refContent);
				item->setTextAlignment(Qt::AlignCenter);
				ui.tableWidget_multiItem_preview->setItem(curRowCount - 1, curColumnCount - 1, item);

				//签名框
				//考虑到当前表格中已有当前签名，判断当前表格中最大的签名框索引
				int ii = 1;
				while ((ui.tableWidget_multiItem_preview->findItems(curMultiItemsInTableWidgetIter.key() + QString::number(ii), Qt::MatchFixedString)).size() > 0)
				{
					ii++;
				}
				for (int i = 0; i < curMultiItemsInTableWidgetIter->maxRepeat; i++)
				{
					int rowSet = curRowCount - 1 - 1 - curMultiItemsInTableWidgetIter->step - i;
					while (rowSet < 0)
					{
						ui.tableWidget_multiItem_preview->insertRow(0);
						curRowCount = ui.tableWidget_multiItem_preview->rowCount();
						rowSet = curRowCount - 1 - 1 - curMultiItemsInTableWidgetIter->step - i;
					}
					QString roleName = curMultiItemsInTableWidgetIter.key() + QString::number(i + ii);
					QTableWidgetItem *item = new QTableWidgetItem(roleName);
					item->setTextAlignment(Qt::AlignCenter);
					item->setTextColor(QColor(0, 255, 0));
					ui.tableWidget_multiItem_preview->setItem(rowSet, curColumnCount - 1, item);
				}
				curMultiItemsInTableWidgetIter++;
			}
		}
		break;
	case DiscernMultiSignBoxConfig::Bottom:
		for (int index = 1; index <= allMaxSearchCount; index++)
		{
			curMultiItemsInTableWidgetIter = curMultiItemsInTableWidget.begin();
			while (curMultiItemsInTableWidgetIter != curMultiItemsInTableWidget.end())
			{
				int curMaxSearchCount = curMultiItemsInTableWidgetIter->maxRefSearchCount;
				if (curMaxSearchCount < index)
				{
					curMultiItemsInTableWidgetIter++;
					continue;
				}

				int curColumnCount = ui.tableWidget_multiItem_preview->columnCount();
				int curRowCount = ui.tableWidget_multiItem_preview->rowCount();

				//索引文本
				ui.tableWidget_multiItem_preview->insertColumn(curColumnCount);
				curColumnCount++;
				if (curRowCount <= 0)
				{
					ui.tableWidget_multiItem_preview->insertRow(0);
					curRowCount = 1;
				}

				QTableWidgetItem *item = new QTableWidgetItem(curMultiItemsInTableWidgetIter->refContent);
				item->setTextAlignment(Qt::AlignCenter);
				ui.tableWidget_multiItem_preview->setItem(0, curColumnCount - 1, item);

				//签名框
				//考虑到当前表格中已有当前签名，判断当前表格中最大的签名框索引
				int ii = 1;
				while ((ui.tableWidget_multiItem_preview->findItems(curMultiItemsInTableWidgetIter.key() + QString::number(ii), Qt::MatchFixedString)).size() > 0)
				{
					ii++;
				}
				for (int i = 0; i < curMultiItemsInTableWidgetIter->maxRepeat; i++)
				{
					int rowSet = curMultiItemsInTableWidgetIter->step + i + 1;
					while (curRowCount < rowSet + 1)
					{
						ui.tableWidget_multiItem_preview->insertRow(curRowCount);
						curRowCount = ui.tableWidget_multiItem_preview->rowCount();
					}

					QString roleName = curMultiItemsInTableWidgetIter.key() + QString::number(i + ii);
					QTableWidgetItem *item = new QTableWidgetItem(roleName);
					item->setTextAlignment(Qt::AlignCenter);
					item->setTextColor(QColor(0, 255, 0));
					ui.tableWidget_multiItem_preview->setItem(rowSet, curColumnCount - 1, item);
				}
				curMultiItemsInTableWidgetIter++;
			}
		}
		break;
	default:
		break;
	}
	
	CenterMultiTablePreview();
}

void GoldEditDiscernFrameConfig::CenterMultiTablePreview()
{
	ui.tableWidget_multiItem_preview->setFixedHeight(30 * ui.tableWidget_multiItem_preview->rowCount());
	ui.tableWidget_multiItem_preview->setFixedWidth(60 * ui.tableWidget_multiItem_preview->columnCount());

	ui.scrollArea_multiItem_preview_contents->setFixedSize(ui.tableWidget_multiItem_preview->size());
	ui.scrollArea_multiItem_preview->setAlignment(Qt::AlignCenter);
	ui.scrollArea_multiItem_preview->horizontalScrollBar()->setStyleSheet("");
	ui.scrollArea_multiItem_preview->verticalScrollBar()->setStyleSheet("");
	
	/*
	if (!this->isVisible())
		return;
	if (this->isMaximized())
		return;

	double newWidth = this->width();
	double newHeight = this->height();

	if (ui.tableWidget_multiItem_preview->width() > ui.widget_multiItem_preview->width())
	{
		newWidth = this->width() + ui.tableWidget_multiItem_preview->width() - ui.widget_multiItem_preview->width();
	}
	if (ui.tableWidget_multiItem_preview->height() > ui.widget_multiItem_preview->height())
	{
		newHeight = this->height() + ui.tableWidget_multiItem_preview->height() - ui.widget_multiItem_preview->height();
	}

	QDesktopWidget desktop;
	int titleBarHeight = this->frameGeometry().height() - this->geometry().height();
	if (newWidth > desktop.availableGeometry(this).width())
		newWidth = desktop.availableGeometry(this).width();
	if (newHeight > desktop.availableGeometry(this).height() - titleBarHeight)
		newHeight = desktop.availableGeometry(this).height() - titleBarHeight;
	int newLeft = this->geometry().x() - (newWidth - this->width()) / 2;
	int newTop = this->geometry().y() - (newHeight - this->height()) / 2;
	if (newLeft < 0)
		newLeft = 0;
	if (newTop < titleBarHeight)
		newTop = titleBarHeight;
	animation.setTargetObject(this);
	animation.setPropertyName("geometry");
	animation.setDuration(100);
	animation.setStartValue(this->geometry());
	animation.setEndValue(QRect(newLeft , newTop , newWidth , newHeight));
	animation.start();
*/	
}