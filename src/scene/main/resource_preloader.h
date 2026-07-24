/**************************************************************************/
/*  resource_preloader.h                                                  */
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

#ifndef RESOURCE_PRELOADER_H
#define RESOURCE_PRELOADER_H

#include "scene/main/node.h"

class ResourcePreloader : public Node {
	GDCLASS(ResourcePreloader, Node);

	Map<StringName, RES> resources;

	void _set_resources(const Array &p_data);
	Array _get_resources() const;
	PoolVector<String> _get_resource_list() const;

protected:
	static void _bind_methods();

public:
	void add_resource(const StringName &p_name, const RES &p_resource);
	void remove_resource(const StringName &p_name);
	void rename_resource(const StringName &p_from_name, const StringName &p_to_name);
	bool has_resource(const StringName &p_name) const;
	RES get_resource(const StringName &p_name) const;

	void get_resource_list(List<StringName> *p_list);

	ResourcePreloader();
};

#endif // RESOURCE_PRELOADER_H
