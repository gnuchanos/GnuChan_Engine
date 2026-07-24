/**************************************************************************/
/*  model_abstraction.h                                                   */
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

#ifndef MODEL_ABSTRACTION_H
#define MODEL_ABSTRACTION_H

#include "modules/fbx/fbx_parser/FBXDocument.h"

struct ModelAbstraction {
	mutable const FBXDocParser::Model *fbx_model = nullptr;

	void set_model(const FBXDocParser::Model *p_model) {
		fbx_model = p_model;
	}

	bool has_model() const {
		return fbx_model != nullptr;
	}

	const FBXDocParser::Model *get_model() const {
		return fbx_model;
	}
};

#endif // MODEL_ABSTRACTION_H
