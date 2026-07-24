/**************************************************************************/
/*  aes_context.h                                                         */
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

#ifndef AES_CONTEXT_H
#define AES_CONTEXT_H

#include "core/crypto/crypto_core.h"
#include "core/reference.h"

class AESContext : public Reference {
	GDCLASS(AESContext, Reference);

public:
	enum Mode {
		MODE_ECB_ENCRYPT,
		MODE_ECB_DECRYPT,
		MODE_CBC_ENCRYPT,
		MODE_CBC_DECRYPT,
		MODE_MAX
	};

private:
	Mode mode;
	CryptoCore::AESContext ctx;
	PoolByteArray iv;

protected:
	static void _bind_methods();

public:
	Error start(Mode p_mode, PoolByteArray p_key, PoolByteArray p_iv = PoolByteArray());
	PoolByteArray update(PoolByteArray p_src);
	PoolByteArray get_iv_state();
	void finish();

	AESContext();
};

VARIANT_ENUM_CAST(AESContext::Mode);

#endif // AES_CONTEXT_H
