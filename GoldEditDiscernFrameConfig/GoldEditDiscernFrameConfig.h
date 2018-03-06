#pragma once

#include <QDialog>
#include <QString>
#include <QXmlQuery>
#include <QMap>
#include <QList>
#include <QStyleFactory>
#include <QtAlgorithms>
#include <QDomDocument>
#include <QFile>
#include <QComboBox>
#include <QTextStream>
#include <QLineEdit>
#include <QMessageBox>
#include <QSettings>
#include <QApplication>
#include <QEvent>
#include <QDebug>
#include <QPropertyAnimation>
#include <QDesktopWidget>
#include <QScrollBar>
#include <QCloseEvent>
#include <windows.h>
#include "ui_GoldEditDiscernFrameConfig.h"
#include "GoldMultiSignBoxEditor.h"

extern HWND AppParentHwnd;

#define SendAppMsg(SelfHwnd , Message) \
COPYDATASTRUCT sendInfo;\
sendInfo.cbData = sizeof(Message);\
sendInfo.dwData = sizeof(Message);\
sendInfo.lpData = Message;\
if (AppParentHwnd != NULL) \
::SendMessage(AppParentHwnd, WM_COPYDATA, (WPARAM)(SelfHwnd), (LPARAM)&sendInfo);

//һ��һ���ýṹ�� (ͼǩ)
typedef struct DiscernSignBoxConfig
{
	typedef struct GroupItems
	{
		QString showName;		//��ʾ��Ϣ
		QString refContent;		//���վ��ο��ı�
		int direction;		//ǩ����λ�÷���
		int step;		//���ɫ�������ο�ĸ���

		GroupItems()
		{
			direction = 0;
			step = 0;
		}
	}GroupItems;

	int id;			//id��
	QString groupName;			//����
	bool isErased;		//�Ƿ��Ѵӽ����б����Ƴ���isErasedΪtrueʱ,���浽xml��ʱ����
	QMap<QString, GroupItems> items;			//���н�ɫ������keyΪ��ɫ����valueΪ��ɫǩ����λ��Ϣ
}DiscernSignBoxConfig;

typedef QMap<QString, DiscernSignBoxConfig::GroupItems> SignBoxGroupItems;


//һ�Զ����ýṹ�� (��ǩ)
typedef struct DiscernMultiSignBoxConfig
{
	enum ItemDirection
	{
		Left = 0,
		Right,
		Top,
		Bottom
	};

	typedef struct GroupItems
	{
		QString showNamePrefix;		//��ʾ��Ϣǰ׺
		QString refContent;		//���վ��ο��ı�
		int direction;		//ǩ����λ�÷���
		int step;		//ǩ����������ı����ο���Ծ�ĸ���
		int maxRepeat;		//ÿ��refContent�ı��������ɲ����ǩ��������
		int maxRefSearchCount;		//refContent�ı���ͼֽ�������ִ���

		GroupItems()
		{
			direction = 0;
			step = 0;
			maxRepeat = 1;
			maxRefSearchCount = 1;
		}
	}GroupItems; 

	int id;			//id��
	QString groupName;			//����
	bool isErased;		//�Ƿ��Ѵӽ����б����Ƴ���isErasedΪtrueʱ,���浽xml��ʱ��������
	QMap<QString, GroupItems> items;			//���н�ɫ������keyΪ��ɫ��ǰ׺��valueΪ��ɫǩ����λ��Ϣ
}DiscernMultiSignBoxConfig;

typedef  QMap<QString, DiscernMultiSignBoxConfig::GroupItems> MultiSignBoxGroupItems;



class GoldChooseDiscernFrameConfig;

//ǩ����������
class GoldEditDiscernFrameConfig : public QDialog
{
	Q_OBJECT

	friend class GoldQListWidget;
	friend class GoldChooseDiscernFrameConfig;

private:
	Ui::GoldEditDiscernFrameConfig ui;
	QString xmlPath;
	QString xmlPathInConfig;
	QPropertyAnimation animation;

public:
    GoldEditDiscernFrameConfig(QWidget *parent = Q_NULLPTR);

public:
	bool eventFilter(QObject * o, QEvent * e);
	bool	event(QEvent * e);
	void closeEvent(QCloseEvent * e);

//��������ؼ��������Լ��źŰ�
private:
	void initAllConnection();					//��ʼ�����в���QT�ź�-��
	void initAllWidget();						//��ʼ�����в�����������

private slots:
	void onBtnConfirmClicked();			//���ȷ�ϰ�ť�󱣴浽�ļ�
	void onTabWidgetCurrentChanged(int index);			//�û��л���tab	



//ͼǩ��أ�һ��һ��
private:
	QList<DiscernSignBoxConfig> allSignBoxConfig;			//�洢ͼǩ���е�ǩ����������Ϣ
	int curTableWidgetRefConfigIndex;			//��ǰtablewidget����Ӧ��allSignBoxConfig����

	void readAllSignBoxConfig();			//�������ļ���ȡͼǩ������ǩ��������
	void writeAllSignBoxConfig();			//����ͼǩ������ǩ���������������ļ�
	bool reorderAllSignBoxConfig(bool isExplicitCheck = true);		//����id����allSignBoxConfig
	void refreshTableWidget(bool isAskForSave = true);				//����ѡ�е�listWidget��Ŀˢ��tableWidget
	bool isTableWidgetItemModified(int verifyFromConfigIndex = -2);		//�Ա�tablewidget��allSignBoxConfig��ָ������verifyFromConfigIndex�е����ݣ��ж������Ƿ������ģ�����ֵΪ-2ʱ�����Զ������ӦallSignBoxConfig��������
	bool isEqual(const SignBoxGroupItems &first, const SignBoxGroupItems &second) const;		//�ж���QMap<QString, DiscernSignBoxConfig::GroupItems>ʵ���Ƿ����

private slots:
	void onBtnAddGroupClicked();							//listWidget�����Ŀ��Ӧ��������ͬ��������allSignBoxConfig
	void onBtnDeleteGroupClicked();						//listWidgetɾ����Ŀ��Ӧ��������ͬ��������allSignBoxConfig
	void onListWidgetCurSelectTextChanged(const QString &);		//listWidgetѡ��������Ӧ��������ˢ��tablewidget
	void onListWidgetItemChanged(QListWidgetItem * item);		//��ӦlistWidget��Ŀ�������ģ�1).ɾ����ǡ���ǵ�ǰѡ�����Ҫ�����µĽ���ˢ��tablewidget; 2)�û��޸���listWidget�е����֣���Ҫ���½�groupNameͬ�����µ�allSignBoxConfig�С�

	void onBtnAddItemClicked();			//tablewidget�����Ŀ��Ӧ����
	void onBtnDeleteItemClicked();		//tablewidgetɾ����Ŀ��Ӧ����
	void onBtnRestoreItemClicked();	//������ǰ��tablewidget�������ĸ���
	void onBtnApplyItemClicked(int applyOnConfigIndex = -2);			//��tablewidget�ϵ����ݱ��浽ָ������applyOnConfigIndex��allSignBoxConfig�ϣ�����ֵΪ-2ʱ�����Զ������ӦallSignBoxConfig��������



//��ǩ��أ�һ�Զࣩ
private:
	QList<DiscernMultiSignBoxConfig> allMultiSignBoxConfig;
	int curMultiTableWidgetRefConfigIndex;
	GoldMultiSignBoxEditor *multiSignBoxEditor;			//��ǩ�༭������

	QStringList curMultiRoleNameInTableWidget;		//��ǰtablewidget�е����н�ɫ��
	MultiSignBoxGroupItems curMultiItemsInTableWidget;		//��ǰtablewidget�����е�items��Ϣ

	void readAllMultiSignBoxConfig();
	void writeAllMultiSignBoxConfig();
	bool reorderAllMultiSignBoxConfig(bool isExplicitCheck = true);
	void refreshMultiTableWidget(bool isAskForSave = true);	
	bool isMultiTableWidgetItemModified(int verifyFromConfigIndex = -2);	
	bool isEqual(const MultiSignBoxGroupItems &first, const MultiSignBoxGroupItems &second) const;

	void DrawMultiTablePreview();			//������Ŀ���ƻ�ǩԤ��ͼ
	void CenterMultiTablePreview();			//����Ԥ��ͼ���¿ؼ���С��λ��


private slots :
	void onBtnAddMultiGroupClicked();
	void onBtnDeleteMultiGroupClicked();
	void onMultiListWidgetCurSelectTextChanged(const QString &);	
	void onMultiListWidgetItemChanged(QListWidgetItem * item);

	void onBtnAddMultiItemClicked();	
	void onBtnDeleteMultiItemClicked();
	void onBtnRestoreMultiItemClicked();
	void onBtnApplyMultiItemClicked(int applyOnConfigIndex = -2);
	void onMultiTableWidgetButtonClicked();		//��Ӧ����е����ð�ť������༭����
	void onMultiDockWidgetTopLevelChanged(bool isFloating);		//��Ӧdockwidget��floating��Ϣ
	void onMultiSignBoxEditorBtnApplyClicked();
	void onMultiComboDirectionSelChanged(int sel);
	
};
