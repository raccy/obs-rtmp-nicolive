#include <QtCore>
#include <QtWidgets>
#include <obs-module.h>
#include "nicolive.h"
#include "nicolive-ui.h"

static QWidget *findTopLevelWidget(const char *name) {
	for(QWidget *widget: QApplication::topLevelWidgets()) {
		if (widget->objectName() == name) {
			return widget;
		}
	}
	return nullptr;
}

static QWidget *getActiveWindowWidget() {
	for(QWidget *widget: QApplication::topLevelWidgets()) {
		if (widget->isActiveWindow()) {
			nicolive_log_debug("active window: %s",
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
		nicolive_log_error("not found Qt Active Window");
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
		nicolive_log_error("not found Qt Active Window");
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
		nicolive_log_error("not found Qt Active Window");
	}
}

extern "C" void nicolive_msg_error(bool available_gui, const char *gui_message,
		const char *cui_message)
{
	if (available_gui)
		nicolive_mbox_error(gui_message);
	else
		nicolive_log_error("%s", cui_message);
}

extern "C" void nicolive_msg_warn(bool available_gui, const char *gui_message,
		const char *cui_message)
{
	if (available_gui)
		nicolive_mbox_warn(gui_message);
	else
		nicolive_log_warn("%s", cui_message);
}

extern "C" void nicolive_msg_info(bool available_gui, const char *gui_message,
		const char *cui_message)
{
	if (available_gui)
		nicolive_mbox_info(gui_message);
	else
		nicolive_log_info("%s", cui_message);
}
