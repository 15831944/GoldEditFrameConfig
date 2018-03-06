#ifndef GOLDCHOOSEDISCERNFRAMECONFIG_H
#define GOLDCHOOSEDISCERNFRAMECONFIG_H

#include <QDialog>
#include "GoldEditDiscernFrameConfig.h"
#include <QCloseEvent>
#include <Windows.h>

namespace Ui {
class GoldChooseDiscernFrameConfig;
}

//检索文本配置信息
typedef struct DiscernTextConfig
{
	int id;		//id号
	QString content;		//检索文本
	QString commonScaleListStr;		//常用的放大比例
	double minRectContainerWidth;	//检索文本所在最小矩形框的宽度
	double minRectContainerHeight;	//检索文本所在最小矩形框的高度
	int signBoxConfigId;					//图签签名框配置id
	int multiSignBoxConfigId;			//会签签名框配置id
	int attrOffsetConfigId;					//属性文字位置属性配置id
	int blockType;								//块类型，同GoldTools中blockType定义。

	DiscernTextConfig()
	{
		signBoxConfigId = -1;
		multiSignBoxConfigId = -1;
	}
}DiscernTextConfig;

class GoldChooseDiscernFrameConfig : public QDialog
{
    Q_OBJECT

public:
    explicit GoldChooseDiscernFrameConfig(QWidget *parent = 0);
    ~GoldChooseDiscernFrameConfig();

private:
    Ui::GoldChooseDiscernFrameConfig *ui;
	QList<DiscernTextConfig> allFindTextConfig;
	GoldEditDiscernFrameConfig *editSignBoxConfigDlg;

private:
	void initAllConnection();
	void readAllFindTextConfig();
	void writeAllFindTextConfig();
	void reorderAllFindTextConfig();
	void refreshFindTextTableWidget();

protected:
	void closeEvent(QCloseEvent * e);

private slots:
	void onBtnEditSignBoxClicked();
	void onBtnConfirmClicked();
	void onBtnCancelClicked();
	void onConfigNeedBeCleared();
};

#endif // GOLDCHOOSEDISCERNFRAMECONFIG_H
