/**************************************************************************/
/*  gltf_template_convert.h                                               */
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

#ifndef GLTF_TEMPLATE_CONVERT_H
#define GLTF_TEMPLATE_CONVERT_H

#include "core/array.h"
#include "core/dictionary.h"
#include "core/set.h"

namespace GLTFTemplateConvert {
template <class T>
static Array to_array(const Vector<T> &p_inp) {
	Array ret;
	for (int i = 0; i < p_inp.size(); i++) {
		ret.push_back(p_inp[i]);
	}
	return ret;
}

template <class T>
static Array to_array(const Set<T> &p_inp) {
	Array ret;
	typename Set<T>::Element *elem = p_inp.front();
	while (elem) {
		ret.push_back(elem->get());
		elem = elem->next();
	}
	return ret;
}

template <class T>
static void set_from_array(Vector<T> &r_out, const Array &p_inp) {
	r_out.clear();
	for (int i = 0; i < p_inp.size(); i++) {
		r_out.push_back(p_inp[i]);
	}
}

template <class T>
static void set_from_array(Set<T> &r_out, const Array &p_inp) {
	r_out.clear();
	for (int i = 0; i < p_inp.size(); i++) {
		r_out.insert(p_inp[i]);
	}
}

template <class K, class V>
static Dictionary to_dict(const Map<K, V> &p_inp) {
	Dictionary ret;
	for (typename Map<K, V>::Element *E = p_inp.front(); E; E = E->next()) {
		ret[E->key()] = E->value();
	}
	return ret;
}

template <class K, class V>
static void set_from_dict(Map<K, V> &r_out, const Dictionary &p_inp) {
	r_out.clear();
	Array keys = p_inp.keys();
	for (int i = 0; i < keys.size(); i++) {
		r_out[keys[i]] = p_inp[keys[i]];
	}
}
} //namespace GLTFTemplateConvert

#endif // GLTF_TEMPLATE_CONVERT_H
