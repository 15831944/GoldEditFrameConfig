#ifndef GOLDCHOOSEDISCERNFRAMECONFIG_H
#define GOLDCHOOSEDISCERNFRAMECONFIG_H

#include <QDialog>
#include "GoldEditDiscernFrameConfig.h"
#include <QCloseEvent>
#include <Windows.h>

namespace Ui {
class GoldChooseDiscernFrameConfig;
}

//�����ı�������Ϣ
typedef struct DiscernTextConfig
{
	int id;		//id��
	QString content;		//�����ı�
	QString commonScaleListStr;		//���õķŴ����
	double minRectContainerWidth;	//�����ı�������С���ο�Ŀ��
	double minRectContainerHeight;	//�����ı�������С���ο�ĸ߶�
	int signBoxConfigId;					//ͼǩǩ��������id
	int multiSignBoxConfigId;			//��ǩǩ��������id
	int attrOffsetConfigId;					//��������λ����������id
	int blockType;								//�����ͣ�ͬGoldTools��blockType���塣

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
