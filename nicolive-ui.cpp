#include <QtCore>
#include <QtWidgets>
#include <obs-module.h>
#include "nicolive.h"
#include "nicolive-ui.h"

static QWidget *getActiveWindowWidget() {
	for(QWidget *widget: QApplication::topLevelWidgets()) {
		if (widget->isActiveWindow()) {
			debug("active window: %s",
				widget->objectName().toStdString().c_str());
			return widget;
		}
	}
	return nullptr;
}

extern "C" void nicolive_mbox_error(const char *message)
{
	QWidget *parent;
	parent = getActiveWindowWidget();
	if (parent != nullptr) {
		QMessageBox::critical(parent,
				QString(obs_module_text("MessageTitleError")),
				QString(message));
	} else {
		error("not found Qt Active Window");
	}
}

extern "C" void nicolive_mbox_warn(const char *message)
{
	QWidget *parent;
	parent = getActiveWindowWidget();
	if (parent != nullptr) {
		QMessageBox::warning(parent,
				QString(obs_module_text("MessageTitleWarn")),
				QString(message));
	} else {
		error("not found Qt Active Window");
	}
}

extern "C" void nicolive_mbox_info(const char *message)
{
	QWidget *parent;
	parent = getActiveWindowWidget();
	if (parent != nullptr) {
		QMessageBox::information(parent,
				QString(obs_module_text("MessageTitleInfo")),
				QString(message));
	} else {
		error("not found Qt Active Window");
	}
}
