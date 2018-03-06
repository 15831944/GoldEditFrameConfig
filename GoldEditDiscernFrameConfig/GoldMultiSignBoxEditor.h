#pragma once

#include <QWidget>
#include "ui_GoldMultiSignBoxEditor.h"

class GoldEditDiscernFrameConfig;

class GoldMultiSignBoxEditor : public QWidget
{
	Q_OBJECT

	friend class GoldEditDiscernFrameConfig;

public:
	GoldMultiSignBoxEditor(QWidget *parent = Q_NULLPTR);
	~GoldMultiSignBoxEditor();
	int height;
	int width;

private:
	Ui::GoldMultiSignBoxEditor ui;
	bool	event(QEvent * e);
	int curTableWidgetIndex;
};
