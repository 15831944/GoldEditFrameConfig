#include "GoldMultiSignBoxEditor.h"

GoldMultiSignBoxEditor::GoldMultiSignBoxEditor(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	this->setWindowFlags(Qt::Popup | Qt::FramelessWindowHint);
	this->setWindowOpacity(1);
	this->setAttribute(Qt::WA_TranslucentBackground);
	this->setWindowTitle(QString::fromLocal8Bit("»áÇ©Ç©Ãû¿ò±à¼­Æ÷"));
	this->activateWindow();

	height = QWidget::height();
	width = QWidget::width();

	curTableWidgetIndex = -1;
}

GoldMultiSignBoxEditor::~GoldMultiSignBoxEditor()
{
}

bool	GoldMultiSignBoxEditor::event(QEvent * e)
{
	if (e->type() == QEvent::WindowDeactivate)
		this->setWindowOpacity(1.0);
	else if (e->type() == QEvent::WindowActivate)
		this->setWindowOpacity(1.0);
	
	return QWidget::event(e);
}