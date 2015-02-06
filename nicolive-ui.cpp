#include <QtCore>
#include <QtWidgets>
#include <obs-module.h>
#include "nicolive.h"
#include "nicolive-ui.h"

static QWidget *getMainWindowWidget() {
	for(QWidget *widget: QApplication::topLevelWidgets()) {
		if (widget->objectName() == "OBSBasic") {
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

extern "C" void nicolive_streaming_click()
{
	QWidget *main_widget = getMainWindowWidget();
	if (main_widget == nullptr) {
		nicolive_log_error("not found Main Window");
		return;
	}

	QPushButton *stream_button = main_widget->findChild<QPushButton *>(
		"streamButton");
	if (stream_button == nullptr) {

	}

	stream_button->click();
}
