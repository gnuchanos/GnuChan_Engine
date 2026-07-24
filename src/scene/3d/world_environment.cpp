/**************************************************************************/
/*  world_environment.cpp                                                 */
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

#include "world_environment.h"
#include "scene/main/viewport.h"

void WorldEnvironment::_notification(int p_what) {
	if (p_what == Spatial::NOTIFICATION_ENTER_WORLD || p_what == Spatial::NOTIFICATION_ENTER_TREE) {
		if (environment.is_valid()) {
			if (get_viewport()->find_world()->get_environment().is_valid()) {
				WARN_PRINT("World already has an environment (Another WorldEnvironment?), overriding.");
			}
			get_viewport()->find_world()->set_environment(environment);
			add_to_group("_world_environment_" + itos(get_viewport()->find_world()->get_scenario().get_id()));
		}

	} else if (p_what == Spatial::NOTIFICATION_EXIT_WORLD || p_what == Spatial::NOTIFICATION_EXIT_TREE) {
		if (environment.is_valid() && get_viewport()->find_world()->get_environment() == environment) {
			get_viewport()->find_world()->set_environment(Ref<Environment>());
			remove_from_group("_world_environment_" + itos(get_viewport()->find_world()->get_scenario().get_id()));
		}
	}
}

void WorldEnvironment::set_environment(const Ref<Environment> &p_environment) {
	if (is_inside_tree() && environment.is_valid() && get_viewport()->find_world()->get_environment() == environment) {
		get_viewport()->find_world()->set_environment(Ref<Environment>());
		remove_from_group("_world_environment_" + itos(get_viewport()->find_world()->get_scenario().get_id()));
		//clean up
	}

	environment = p_environment;
	if (is_inside_tree() && environment.is_valid()) {
		if (get_viewport()->find_world()->get_environment().is_valid()) {
			WARN_PRINT("World already has an environment (Another WorldEnvironment?), overriding.");
		}
		get_viewport()->find_world()->set_environment(environment);
		add_to_group("_world_environment_" + itos(get_viewport()->find_world()->get_scenario().get_id()));
	}

	update_configuration_warning();
}

Ref<Environment> WorldEnvironment::get_environment() const {
	return environment;
}

String WorldEnvironment::get_configuration_warning() const {
	String warning = Node::get_configuration_warning();
	if (!environment.is_valid()) {
		if (warning != String()) {
			warning += "\n\n";
		}
		warning += TTR("WorldEnvironment requires its \"Environment\" property to contain an Environment to have a visible effect.");
		return warning;
	}

	if (/*!is_visible_in_tree() ||*/ !is_inside_tree()) {
		return String();
	}

	List<Node *> nodes;
	get_tree()->get_nodes_in_group("_world_environment_" + itos(get_viewport()->find_world()->get_scenario().get_id()), &nodes);

	if (nodes.size() > 1) {
		if (warning != String()) {
			warning += "\n\n";
		}
		warning += TTR("Only one WorldEnvironment is allowed per scene (or set of instanced scenes).");
	}

	// Commenting this warning for now, I think it makes no sense. If anyone can figure out what its supposed to do, feedback welcome. Else it should be deprecated.
	//if (environment.is_valid() && get_viewport() && !get_viewport()->get_camera() && environment->get_background() != Environment::BG_CANVAS) {
	//	return TTR("This WorldEnvironment is ignored. Either add a Camera (for 3D scenes) or set this environment's Background Mode to Canvas (for 2D scenes).");
	//}

	return warning;
}

void WorldEnvironment::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_environment", "env"), &WorldEnvironment::set_environment);
	ClassDB::bind_method(D_METHOD("get_environment"), &WorldEnvironment::get_environment);
	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "environment", PROPERTY_HINT_RESOURCE_TYPE, "Environment"), "set_environment", "get_environment");
}

WorldEnvironment::WorldEnvironment() {
}
