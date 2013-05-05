#include "modulemanager.h"

#ifdef SG_EXT_UPLOADS
#include "src/modules/uploader/moduleuploader.h"
#endif

#ifdef SG_EXT_EDIT
#include "src/modules/extedit/moduleextedit.h"
#endif

#include <QDebug>

ModuleManager::ModuleManager()
{
	_modules = new ModuleList_t();
	
#ifdef SG_EXT_UPLOADS
	ModuleUploader *uploader = new ModuleUploader();
	_modules->insert(MOD_UPLOADER , uploader);
#endif

#ifdef SG_EXT_EDIT
	ModuleExtEdit *extedit = new ModuleExtEdit();
	_modules->insert(MOD_EXT_EDIT.data(), extedit);
#endif
}

AbstractModule* ModuleManager::getModule(const QByteArray& name)
{
	if (_modules->contains(name))
	{
		return _modules->value(name);
	}
	else
	{
		return 0;
	}
}

QList<QMenu*> ModuleManager::generateModulesMenus(QStringList modules)
{
	QList< QMenu* > list;
	if (modules.isEmpty() == true)
	{		
		for (int i =0; i < _modules->keys().count(); ++i)
		{
			QMenu *menu = _modules->value(_modules->keys().at(i))->initModuleMenu();
			list.append(menu);
		}
	}
	else
	{
		for (int i = 0; i < modules.count(); ++i)
		{
			QByteArray currentKey = modules.at(i).toAscii();
			QMenu *menu = _modules->value(currentKey)->initModuleMenu();
			list.append(menu);
		}
	}
	
	return list;
}

QList<QAction*> ModuleManager::generateModulesActions(QStringList modules)
{
	QList< QAction* > list;

	if (modules.isEmpty() == true)
	{		
		for (int i =0; i < _modules->keys().count(); ++i)
		{
			QAction *action = _modules->value(_modules->keys().at(i))->initModuleAction();
			list.append(action);
		}
	}
	else
	{
		for (int i = 0; i < modules.count(); ++i)
		{
			QByteArray currentKey = modules.at(i).toAscii();
			QAction *action = _modules->value(currentKey)->initModuleAction();
			list.append(action);
		}
	}
	
	return list;
}
