#include "GoldEditDiscernFrameConfig.h"
#include "GoldChooseDiscernFrameConfig.h"
#include <QApplication>
#include "windows.h"

HWND AppParentHwnd = NULL;

int main(int argc, char *argv[])
{
	QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication a(argc, argv);
	
	QSettings settings("HKEY_CURRENT_USER\\SOFTWARE\\JinQu\\CADTools", QSettings::NativeFormat);
	QString jqCADToolsPath = settings.value("AppLocation").toString();
	if (jqCADToolsPath.isEmpty())
	{
		QMessageBox::critical(NULL, QString::fromLocal8Bit("错误"), QString::fromLocal8Bit("读取金曲智能CAD工具包目录失败，请重新安装。\t\t\n"));
		SendAppMsg(NULL, (PVOID)"AppExited");
		return 0;
	}

	QString argv1;
	if (argc >= 2)
		argv1 = argv[1];
	long hwndL = argv1.toLong();
	GoldChooseDiscernFrameConfig *chooseDlg = new GoldChooseDiscernFrameConfig();
	if (hwndL > 0)
	{
		AppParentHwnd = (HWND)hwndL;
		QWidget *widget = new QWidget();
		widget->setWindowFlags(Qt::FramelessWindowHint);
		widget->setFixedSize(1,1);
		widget->setWindowOpacity(0.05);
		chooseDlg->setParent(widget);
		chooseDlg->setWindowFlags(chooseDlg->windowFlags() | Qt::Window);
		widget->show();
		chooseDlg->show();
		SetWindowLong((HWND)widget->winId(), GWLP_HWNDPARENT, hwndL);
		SendAppMsg((HWND)widget->winId(), (PVOID)"AppLaunched");
		chooseDlg->activateWindow();
	}
	else
	{
		chooseDlg->show();
	}
	return a.exec();
}
