#pragma once

#include <QListWidget>

class GoldQListWidget : public QListWidget
{
	Q_OBJECT

public:
	GoldQListWidget(QWidget *parent);
	~GoldQListWidget();

private slots:
	void onItemDbClicked(QListWidgetItem * item);
};
