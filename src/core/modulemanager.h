#ifndef MODULEMANAGER_H
#define MODULEMANAGER_H

#include "src/modules/abstractmodule.h"

#include <QtCore/QByteArray>
#include <QtCore/QHash>
#include <QtCore/QMap>
#include <QtGui/QMenu>

#include <QtGui/QAction>

const QByteArray MOD_UPLOADER = "uploader";
const QByteArray MOD_EXT_EDIT = "extedit";

typedef QMap<QByteArray, AbstractModule*> ModuleList_t;

class ModuleManager
{
public:
    ModuleManager();
	AbstractModule* getModule(const QByteArray& name);
	QList<QMenu*> generateModulesMenus(QStringList modules = QStringList());
	QList<QAction*> generateModulesActions(QStringList modules = QStringList());
	
private:
	ModuleList_t *_modules;
};

#endif // MODULEMANAGER_H
