#include "export_template_manager.h"

#include "core/io/zip_io.h"
#include "core/os/dir_access.h"
#include "core/os/file_access.h"
#include "core/version.h"
#include "editor_node.h"
#include "editor_scale.h"
#include "progress_dialog.h"

void ExportTemplateManager::_update_template_status() {
	DirAccess *da = DirAccess::create(DirAccess::ACCESS_FILESYSTEM);
	const String &templates_dir = EditorSettings::get_singleton()->get_templates_dir();

	Error err = da->change_dir(templates_dir);
	ERR_FAIL_COND_MSG(err != OK, "Could not access templates directory at '" + templates_dir + "'.");

	Set<String> templates;
	da->list_dir_begin();
	if (err == OK) {
		String c = da->get_next();
		while (c != String()) {
			if (da->current_is_dir() && !c.begins_with(".")) {
				templates.insert(c);
			}
			c = da->get_next();
		}
	}
	da->list_dir_end();
	memdelete(da);

	String current_version = VERSION_FULL_CONFIG;
	current_value->set_text(current_version);

	if (templates.has(current_version)) {
		current_missing_label->hide();
		current_installed_label->show();
		current_installed_hb->show();
		current_version_exists = true;
		current_installed_path->set_text(templates_dir.plus_file(current_version));
	} else {
		current_installed_label->hide();
		current_missing_label->show();
		current_installed_hb->hide();
		current_version_exists = false;
	}

	installed_table->clear();
	TreeItem *installed_root = installed_table->create_item();

	for (Set<String>::Element *E = templates.back(); E; E = E->prev()) {
		String version_string = E->get();
		if (version_string == current_version) {
			continue;
		}

		TreeItem *ti = installed_table->create_item(installed_root);
		ti->set_text(0, version_string);

		ti->add_button(0, get_icon("Folder", "EditorIcons"), OPEN_TEMPLATE_FOLDER, false, TTR("Open the folder containing these templates."));
		ti->add_button(0, get_icon("Remove", "EditorIcons"), UNINSTALL_TEMPLATE, false, TTR("Uninstall these templates."));
	}

	minimum_size_changed();
	update();
}

void ExportTemplateManager::_install_file() {
	install_file_dialog->popup_centered_ratio();
}

bool ExportTemplateManager::_install_file_selected(const String &p_file, bool p_skip_progress) {
	FileAccess *fa = nullptr;
	zlib_filefunc_def io = zipio_create_io_from_file(&fa);

	unzFile pkg = unzOpen2(p_file.utf8().get_data(), &io);
	if (!pkg) {
		EditorNode::get_singleton()->show_warning(TTR("Can't open the export templates file."));
		return false;
	}
	int ret = unzGoToFirstFile(pkg);

	int fc = 0;
	String version;
	String contents_dir;

	while (ret == UNZ_OK) {
		unz_file_info info;
		char fname[16384];
		ret = unzGetCurrentFileInfo(pkg, &info, fname, 16384, nullptr, 0, nullptr, 0);

		String file = String::utf8(fname);
		if (file.ends_with("version.txt")) {
			Vector<uint8_t> data;
			data.resize(info.uncompressed_size);

			unzOpenCurrentFile(pkg);
			ret = unzReadCurrentFile(pkg, data.ptrw(), data.size());
			unzCloseCurrentFile(pkg);

			String data_str;
			data_str.parse_utf8((const char *)data.ptr(), data.size());
			data_str = data_str.strip_edges();

			if (data_str.get_slice_count(".") < 3) {
				EditorNode::get_singleton()->show_warning(vformat(TTR("Invalid version.txt format inside the export templates file: %s."), data_str));
				unzClose(pkg);
				return false;
			}

			version = data_str;
			contents_dir = file.get_base_dir().trim_suffix("/").trim_suffix("\\");
		}

		if (file.get_file().size() != 0) {
			fc++;
		}

		ret = unzGoToNextFile(pkg);
	}

	if (version == String()) {
		EditorNode::get_singleton()->show_warning(TTR("No version.txt found inside the export templates file."));
		unzClose(pkg);
		return false;
	}

	DirAccessRef d = DirAccess::create(DirAccess::ACCESS_FILESYSTEM);
	String template_path = EditorSettings::get_singleton()->get_templates_dir().plus_file(version);
	Error err = d->make_dir_recursive(template_path);
	if (err != OK) {
		EditorNode::get_singleton()->show_warning(TTR("Error creating path for extracting templates:") + "\n" + template_path);
		unzClose(pkg);
		return false;
	}

	EditorProgress *p = nullptr;
	if (!p_skip_progress) {
		p = memnew(EditorProgress("ltask", TTR("Extracting Export Templates"), fc));
	}

	fc = 0;
	ret = unzGoToFirstFile(pkg);
	while (ret == UNZ_OK) {
		unz_file_info info;
		char fname[16384];
		unzGetCurrentFileInfo(pkg, &info, fname, 16384, nullptr, 0, nullptr, 0);

		String file_path(String::utf8(fname).simplify_path());
		String file = file_path.get_file();

		if (file.size() == 0) {
			ret = unzGoToNextFile(pkg);
			continue;
		}

		Vector<uint8_t> data;
		data.resize(info.uncompressed_size);

		unzOpenCurrentFile(pkg);
		unzReadCurrentFile(pkg, data.ptrw(), data.size());
		unzCloseCurrentFile(pkg);

		String base_dir = file_path.get_base_dir().trim_suffix("/");

		if (base_dir != contents_dir && base_dir.begins_with(contents_dir)) {
			base_dir = base_dir.substr(contents_dir.length(), file_path.length()).trim_prefix("/");
			file = base_dir.plus_file(file);

			DirAccessRef da = DirAccess::create(DirAccess::ACCESS_FILESYSTEM);
			ERR_CONTINUE(!da);

			String output_dir = template_path.plus_file(base_dir);

			if (!DirAccess::exists(output_dir)) {
				Error mkdir_err = da->make_dir_recursive(output_dir);
				ERR_CONTINUE(mkdir_err != OK);
			}
		}

		if (p) {
			p->step(TTR("Importing:") + " " + file, fc);
		}

		String to_write = template_path.plus_file(file);
		FileAccessRef f = FileAccess::open(to_write, FileAccess::WRITE);

		if (!f) {
			ret = unzGoToNextFile(pkg);
			fc++;
			ERR_CONTINUE_MSG(true, "Can't open file from path '" + String(to_write) + "'.");
		}

		f->store_buffer(data.ptr(), data.size());

#ifndef WINDOWS_ENABLED
		FileAccess::set_unix_permissions(to_write, (info.external_fa >> 16) & 0x01FF);
#endif

		ret = unzGoToNextFile(pkg);
		fc++;
	}

	if (p) {
		memdelete(p);
	}
	unzClose(pkg);

	_update_template_status();
	return true;
}

void ExportTemplateManager::_uninstall_template(const String &p_version) {
	uninstall_confirm->set_text(vformat(TTR("Remove templates for the version '%s'?"), p_version));
	uninstall_confirm->popup_centered();
	uninstall_version = p_version;
}

void ExportTemplateManager::_uninstall_template_confirmed() {
	DirAccessRef da = DirAccess::create(DirAccess::ACCESS_FILESYSTEM);
	const String &templates_dir = EditorSettings::get_singleton()->get_templates_dir();

	Error err = da->change_dir(templates_dir);
	ERR_FAIL_COND_MSG(err != OK, "Could not access templates directory at '" + templates_dir + "'.");
	err = da->change_dir(uninstall_version);
	ERR_FAIL_COND_MSG(err != OK, "Could not access templates directory at '" + templates_dir.plus_file(uninstall_version) + "'.");

	err = da->erase_contents_recursive();
	ERR_FAIL_COND_MSG(err != OK, "Could not remove all templates in '" + templates_dir.plus_file(uninstall_version) + "'.");

	da->change_dir("..");
	err = da->remove(uninstall_version);
	ERR_FAIL_COND_MSG(err != OK, "Could not remove templates directory at '" + templates_dir.plus_file(uninstall_version) + "'.");

	_update_template_status();
}

void ExportTemplateManager::_installed_table_button_cbk(Object *p_item, int p_column, int p_id) {
	TreeItem *ti = Object::cast_to<TreeItem>(p_item);
	if (!ti) {
		return;
	}

	switch (p_id) {
		case OPEN_TEMPLATE_FOLDER: {
			String version_string = ti->get_text(0);
			_open_template_folder(version_string);
		} break;

		case UNINSTALL_TEMPLATE: {
			String version_string = ti->get_text(0);
			_uninstall_template(version_string);
		} break;
	}
}

void ExportTemplateManager::_open_template_folder(const String &p_version) {
	const String &templates_dir = EditorSettings::get_singleton()->get_templates_dir();
	OS::get_singleton()->shell_open("file://" + templates_dir.plus_file(p_version));
}

void ExportTemplateManager::popup_manager() {
	_update_template_status();
	popup_centered(Size2(720, 280) * EDSCALE);
}

void ExportTemplateManager::_notification(int p_what) {
	switch (p_what) {
		case NOTIFICATION_ENTER_TREE:
		case NOTIFICATION_THEME_CHANGED: {
			current_value->add_font_override("font", get_font("bold", "EditorFonts"));
			current_missing_label->add_color_override("font_color", get_color("error_color", "Editor"));
			current_installed_label->add_color_override("font_color", get_color("disabled_font_color", "Editor"));
		} break;
	}
}

void ExportTemplateManager::_bind_methods() {
	ClassDB::bind_method("_open_template_folder", &ExportTemplateManager::_open_template_folder);
	ClassDB::bind_method("_uninstall_template", &ExportTemplateManager::_uninstall_template);
	ClassDB::bind_method("_uninstall_template_confirmed", &ExportTemplateManager::_uninstall_template_confirmed);
	ClassDB::bind_method("_install_file", &ExportTemplateManager::_install_file);
	ClassDB::bind_method("_installed_table_button_cbk", &ExportTemplateManager::_installed_table_button_cbk);
	ClassDB::bind_method("_install_file_selected", &ExportTemplateManager::_install_file_selected);
}

ExportTemplateManager::ExportTemplateManager() {
	set_title(TTR("Export Template Manager"));
	set_hide_on_ok(true);
	get_ok()->set_text(TTR("Close"));

	VBoxContainer *main_vb = memnew(VBoxContainer);
	add_child(main_vb);

	// Current version controls.
	HBoxContainer *current_hb = memnew(HBoxContainer);
	main_vb->add_child(current_hb);

	Label *current_label = memnew(Label);
	current_label->set_text(TTR("Current Version:"));
	current_hb->add_child(current_label);

	current_value = memnew(Label);
	current_hb->add_child(current_value);

	// Status labels
	current_missing_label = memnew(Label);
	current_missing_label->set_h_size_flags(Control::SIZE_EXPAND_FILL);
	current_missing_label->set_align(Label::ALIGN_RIGHT);
	current_missing_label->set_text(TTR("Export templates are missing. Install from a file."));
	current_hb->add_child(current_missing_label);

	current_installed_label = memnew(Label);
	current_installed_label->set_h_size_flags(Control::SIZE_EXPAND_FILL);
	current_installed_label->set_align(Label::ALIGN_RIGHT);
	current_installed_label->set_text(TTR("Export templates are installed and ready to be used."));
	current_hb->add_child(current_installed_label);
	current_installed_label->hide();

	current_installed_hb = memnew(HBoxContainer);
	main_vb->add_child(current_installed_hb);

	current_installed_path = memnew(LineEdit);
	current_installed_path->set_editable(false);
	current_installed_path->set_h_size_flags(Control::SIZE_EXPAND_FILL);
	current_installed_hb->add_child(current_installed_path);

	current_open_button = memnew(Button);
	current_open_button->set_text(TTR("Open Folder"));
	current_open_button->set_tooltip(TTR("Open the folder containing installed templates for the current version."));
	current_installed_hb->add_child(current_open_button);
	current_open_button->connect("pressed", this, "_open_template_folder", varray(VERSION_FULL_CONFIG));

	current_uninstall_button = memnew(Button);
	current_uninstall_button->set_text(TTR("Uninstall"));
	current_uninstall_button->set_tooltip(TTR("Uninstall templates for the current version."));
	current_installed_hb->add_child(current_uninstall_button);
	current_uninstall_button->connect("pressed", this, "_uninstall_template", varray(VERSION_FULL_CONFIG));

	main_vb->add_child(memnew(HSeparator));

	// Install from file section
	HBoxContainer *install_hb = memnew(HBoxContainer);
	main_vb->add_child(install_hb);

	install_file_button = memnew(Button);
	install_file_button->set_text(TTR("Install from File"));
	install_file_button->set_tooltip(TTR("Install templates from a local .tpz file."));
	install_hb->add_child(install_file_button);
	install_file_button->connect("pressed", this, "_install_file");

	main_vb->add_child(memnew(HSeparator));

	// Other installed versions table
	HBoxContainer *installed_versions_hb = memnew(HBoxContainer);
	main_vb->add_child(installed_versions_hb);
	Label *installed_label = memnew(Label);
	installed_label->set_text(TTR("Other Installed Versions:"));
	installed_versions_hb->add_child(installed_label);

	installed_table = memnew(Tree);
	installed_table->set_hide_root(true);
	installed_table->set_custom_minimum_size(Size2(0, 100) * EDSCALE);
	installed_table->set_v_size_flags(Control::SIZE_EXPAND_FILL);
	main_vb->add_child(installed_table);
	installed_table->connect("button_pressed", this, "_installed_table_button_cbk");

	// Dialogs
	uninstall_confirm = memnew(ConfirmationDialog);
	uninstall_confirm->set_title(TTR("Uninstall Template"));
	add_child(uninstall_confirm);
	uninstall_confirm->connect("confirmed", this, "_uninstall_template_confirmed");

	install_file_dialog = memnew(FileDialog);
	install_file_dialog->set_title(TTR("Select Template File"));
	install_file_dialog->set_access(FileDialog::ACCESS_FILESYSTEM);
	install_file_dialog->set_mode(FileDialog::MODE_OPEN_FILE);
	install_file_dialog->add_filter("*.tpz ; " + TTR("GnuChanIDE Export Templates"));
	install_file_dialog->connect("file_selected", this, "_install_file_selected", varray(false));
	add_child(install_file_dialog);
}
