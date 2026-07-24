/**************************************************************************/
/*  editor_asset_installer.h                                              */
/**************************************************************************/
/*                         This file is part of:                          */
/*                             GNUCHAN ENGINE                             */
/*                    https://github.com/gnuchanos/GnuChan_Engine         */
/**************************************************************************/
/* Copyright (C) 2025 GnuChan Engine contributors (see AUTHORS.md).      */
/*                                                                        */
/* This program is free software: you can redistribute it and/or modify   */
/* it under the terms of the GNU General Public License as published by   */
/* the Free Software Foundation, either version 3 of the License, or      */
/* (at your option) any later version.                                    */
/*                                                                        */
/* This program is distributed in the hope that it will be useful,        */
/* but WITHOUT ANY WARRANTY; without even the implied warranty of         */
/* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the           */
/* GNU General Public License for more details.                           */
/*                                                                        */
/* You should have received a copy of the GNU General Public License      */
/* along with this program. If not, see <https://www.gnu.org/licenses/>.  */
/**************************************************************************/

#ifndef EDITOR_ASSET_INSTALLER_H
#define EDITOR_ASSET_INSTALLER_H

#include "scene/gui/dialogs.h"
#include "scene/gui/tree.h"
class EditorAssetInstaller : public ConfirmationDialog {
	GDCLASS(EditorAssetInstaller, ConfirmationDialog);

	Tree *tree;
	Label *asset_contents;
	String package_path;
	String asset_name;
	AcceptDialog *error;
	Map<String, TreeItem *> status_map;
	bool updating;
	void _update_subitems(TreeItem *p_item, bool p_check, bool p_first = false);
	void _uncheck_parent(TreeItem *p_item);
	void _item_edited();
	virtual void ok_pressed();

protected:
	static void _bind_methods();

public:
	void open(const String &p_path, int p_depth = 0);

	void set_asset_name(const String &p_asset_name);
	String get_asset_name() const;

	EditorAssetInstaller();
};

#endif // EDITOR_ASSET_INSTALLER_H
