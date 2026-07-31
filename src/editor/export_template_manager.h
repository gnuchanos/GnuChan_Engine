#ifndef EXPORT_TEMPLATE_MANAGER_H
#define EXPORT_TEMPLATE_MANAGER_H

#include "editor/editor_settings.h"
#include "scene/gui/dialogs.h"
#include "scene/gui/file_dialog.h"
#include "scene/gui/tree.h"

class ExportTemplateManager : public AcceptDialog {
	GDCLASS(ExportTemplateManager, AcceptDialog);

	bool current_version_exists = false;

	Label *current_value;
	Label *current_missing_label;
	Label *current_installed_label;

	HBoxContainer *current_installed_hb;
	LineEdit *current_installed_path;
	Button *current_open_button;
	Button *current_uninstall_button;

	Button *install_file_button;

	enum TemplatesAction {
		OPEN_TEMPLATE_FOLDER,
		UNINSTALL_TEMPLATE,
	};

	Tree *installed_table;

	ConfirmationDialog *uninstall_confirm;
	String uninstall_version;
	FileDialog *install_file_dialog;

	void _update_template_status();

	void _install_file();
	bool _install_file_selected(const String &p_file, bool p_skip_progress = false);

	void _uninstall_template(const String &p_version);
	void _uninstall_template_confirmed();

	void _installed_table_button_cbk(Object *p_item, int p_column, int p_id);

	void _open_template_folder(const String &p_version);

protected:
	void _notification(int p_what);
	static void _bind_methods();

public:
	void popup_manager();

	ExportTemplateManager();
};

#endif // EXPORT_TEMPLATE_MANAGER_H
