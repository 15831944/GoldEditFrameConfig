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

//一对一配置结构体 (图签)
typedef struct DiscernSignBoxConfig
{
	typedef struct GroupItems
	{
		QString showName;		//显示信息
		QString refContent;		//参照矩形框文本
		int direction;		//签名框位置方向
		int step;		//与角色框相距矩形框的个数

		GroupItems()
		{
			direction = 0;
			step = 0;
		}
	}GroupItems;

	int id;			//id号
	QString groupName;			//组名
	bool isErased;		//是否已从界面列表中移除，isErased为true时,保存到xml的时候该项将
	QMap<QString, GroupItems> items;			//所有角色，其中key为角色名，value为角色签名框方位信息
}DiscernSignBoxConfig;

typedef QMap<QString, DiscernSignBoxConfig::GroupItems> SignBoxGroupItems;


//一对多配置结构体 (会签)
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
		QString showNamePrefix;		//显示信息前缀
		QString refContent;		//参照矩形框文本
		int direction;		//签名框位置方向
		int step;		//签名框与参照文本矩形框跳跃的个数
		int maxRepeat;		//每个refContent文本附近最多可插入的签名框数量
		int maxRefSearchCount;		//refContent文本在图纸中最多出现次数

		GroupItems()
		{
			direction = 0;
			step = 0;
			maxRepeat = 1;
			maxRefSearchCount = 1;
		}
	}GroupItems; 

	int id;			//id号
	QString groupName;			//组名
	bool isErased;		//是否已从界面列表中移除，isErased为true时,保存到xml的时候该项将忽略
	QMap<QString, GroupItems> items;			//所有角色，其中key为角色名前缀，value为角色签名框方位信息
}DiscernMultiSignBoxConfig;

typedef  QMap<QString, DiscernMultiSignBoxConfig::GroupItems> MultiSignBoxGroupItems;



class GoldChooseDiscernFrameConfig;

//签名框配置类
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

//界面基本控件特殊属性及信号绑定
private:
	void initAllConnection();					//初始化所有部件QT信号-槽
	void initAllWidget();						//初始化所有部件特殊属性

private slots:
	void onBtnConfirmClicked();			//点击确认按钮后保存到文件
	void onTabWidgetCurrentChanged(int index);			//用户切换了tab	



//图签相关（一对一）
private:
	QList<DiscernSignBoxConfig> allSignBoxConfig;			//存储图签所有的签名框配置信息
	int curTableWidgetRefConfigIndex;			//当前tablewidget所对应的allSignBoxConfig索引

	void readAllSignBoxConfig();			//从配置文件读取图签上所有签名框配置
	void writeAllSignBoxConfig();			//保存图签上所有签名框配置至配置文件
	bool reorderAllSignBoxConfig(bool isExplicitCheck = true);		//根据id排序allSignBoxConfig
	void refreshTableWidget(bool isAskForSave = true);				//根据选中的listWidget条目刷新tableWidget
	bool isTableWidgetItemModified(int verifyFromConfigIndex = -2);		//对比tablewidget与allSignBoxConfig上指定索引verifyFromConfigIndex中的内容，判断内容是否发生更改；索引值为-2时，将自动计算对应allSignBoxConfig的索引。
	bool isEqual(const SignBoxGroupItems &first, const SignBoxGroupItems &second) const;		//判断两QMap<QString, DiscernSignBoxConfig::GroupItems>实例是否相等

private slots:
	void onBtnAddGroupClicked();							//listWidget添加条目响应函数，并同步更新至allSignBoxConfig
	void onBtnDeleteGroupClicked();						//listWidget删除条目响应函数，并同步更新至allSignBoxConfig
	void onListWidgetCurSelectTextChanged(const QString &);		//listWidget选择项变更响应函数，并刷新tablewidget
	void onListWidgetItemChanged(QListWidgetItem * item);		//响应listWidget条目其他更改，1).删除的恰好是当前选择项，需要根据新的焦点刷新tablewidget; 2)用户修改了listWidget中的文字，需要更新将groupName同步更新到allSignBoxConfig中。

	void onBtnAddItemClicked();			//tablewidget添加条目响应函数
	void onBtnDeleteItemClicked();		//tablewidget删除条目响应函数
	void onBtnRestoreItemClicked();	//撤销当前在tablewidget上所做的更改
	void onBtnApplyItemClicked(int applyOnConfigIndex = -2);			//将tablewidget上的内容保存到指定索引applyOnConfigIndex的allSignBoxConfig上；索引值为-2时，将自动计算对应allSignBoxConfig的索引。



//会签相关（一对多）
private:
	QList<DiscernMultiSignBoxConfig> allMultiSignBoxConfig;
	int curMultiTableWidgetRefConfigIndex;
	GoldMultiSignBoxEditor *multiSignBoxEditor;			//会签编辑器界面

	QStringList curMultiRoleNameInTableWidget;		//当前tablewidget中的所有角色名
	MultiSignBoxGroupItems curMultiItemsInTableWidget;		//当前tablewidget中所有的items信息

	void readAllMultiSignBoxConfig();
	void writeAllMultiSignBoxConfig();
	bool reorderAllMultiSignBoxConfig(bool isExplicitCheck = true);
	void refreshMultiTableWidget(bool isAskForSave = true);	
	bool isMultiTableWidgetItemModified(int verifyFromConfigIndex = -2);	
	bool isEqual(const MultiSignBoxGroupItems &first, const MultiSignBoxGroupItems &second) const;

	void DrawMultiTablePreview();			//根据条目绘制会签预览图
	void CenterMultiTablePreview();			//根据预览图更新控件大小及位置


private slots :
	void onBtnAddMultiGroupClicked();
	void onBtnDeleteMultiGroupClicked();
	void onMultiListWidgetCurSelectTextChanged(const QString &);	
	void onMultiListWidgetItemChanged(QListWidgetItem * item);

	void onBtnAddMultiItemClicked();	
	void onBtnDeleteMultiItemClicked();
	void onBtnRestoreMultiItemClicked();
	void onBtnApplyMultiItemClicked(int applyOnConfigIndex = -2);
	void onMultiTableWidgetButtonClicked();		//响应表格中的设置按钮，进入编辑界面
	void onMultiDockWidgetTopLevelChanged(bool isFloating);		//响应dockwidget的floating信息
	void onMultiSignBoxEditorBtnApplyClicked();
	void onMultiComboDirectionSelChanged(int sel);
	
};
