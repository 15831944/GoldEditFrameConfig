#include "GoldQListWidget.h"
#include <QDebug>

GoldQListWidget::GoldQListWidget(QWidget *parent)
	: QListWidget(parent)
{
	connect(this, SIGNAL(itemDoubleClicked(QListWidgetItem *)), this, SLOT(onItemDbClicked(QListWidgetItem *)));
}

GoldQListWidget::~GoldQListWidget()
{
}

void GoldQListWidget::onItemDbClicked(QListWidgetItem * item)
{
	if (item == 0)
		return;
	item->setFlags(item->flags() | Qt::ItemIsEditable);
	QListWidget::editItem(item);
}