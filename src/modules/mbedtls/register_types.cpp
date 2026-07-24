/**************************************************************************/
/*  register_types.cpp                                                    */
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

#include "register_types.h"

#include "crypto_mbedtls.h"
#include "dtls_server_mbedtls.h"
#include "packet_peer_mbed_dtls.h"
#include "stream_peer_mbedtls.h"

#include "core/project_settings.h"

#if MBEDTLS_VERSION_MAJOR >= 3
#include <psa/crypto.h>
#endif

static bool godot_mbedtls_initialized = false;

#ifdef GODOT_MBEDTLS_THREADING_ALT
extern "C" {
void godot_mbedtls_mutex_init(mbedtls_threading_mutex_t *p_mutex) {
	ERR_FAIL_NULL(p_mutex);
	p_mutex->mutex = memnew(Mutex);
}

void godot_mbedtls_mutex_free(mbedtls_threading_mutex_t *p_mutex) {
	ERR_FAIL_NULL(p_mutex);
	ERR_FAIL_NULL(p_mutex->mutex);
	memdelete((Mutex *)p_mutex->mutex);
}

int godot_mbedtls_mutex_lock(mbedtls_threading_mutex_t *p_mutex) {
	ERR_FAIL_NULL_V(p_mutex, MBEDTLS_ERR_THREADING_BAD_INPUT_DATA);
	ERR_FAIL_NULL_V(p_mutex->mutex, MBEDTLS_ERR_THREADING_BAD_INPUT_DATA);
	((Mutex *)p_mutex->mutex)->lock();
	return 0;
}

int godot_mbedtls_mutex_unlock(mbedtls_threading_mutex_t *p_mutex) {
	ERR_FAIL_NULL_V(p_mutex, MBEDTLS_ERR_THREADING_BAD_INPUT_DATA);
	ERR_FAIL_NULL_V(p_mutex->mutex, MBEDTLS_ERR_THREADING_BAD_INPUT_DATA);
	((Mutex *)p_mutex->mutex)->unlock();
	return 0;
}
};
#endif

void register_mbedtls_types() {
	GLOBAL_DEF("network/ssl/enable_tls_v1.3", false);

#ifdef GODOT_MBEDTLS_THREADING_ALT
	mbedtls_threading_set_alt(
			godot_mbedtls_mutex_init,
			godot_mbedtls_mutex_free,
			godot_mbedtls_mutex_lock,
			godot_mbedtls_mutex_unlock);
#endif

#if MBEDTLS_VERSION_MAJOR >= 3
	int status = psa_crypto_init();
	ERR_FAIL_COND_MSG(status != PSA_SUCCESS, "Failed to initialize psa crypto. The mbedTLS modules will not work.");
#endif

#ifdef DEBUG_ENABLED
	if (OS::get_singleton()->is_stdout_verbose()) {
		mbedtls_debug_set_threshold(1);
	}
#endif

	godot_mbedtls_initialized = true;

	CryptoMbedTLS::initialize_crypto();
	StreamPeerMbedTLS::initialize_ssl();
	PacketPeerMbedDTLS::initialize_dtls();
	DTLSServerMbedTLS::initialize();
}

void unregister_mbedtls_types() {
	if (godot_mbedtls_initialized) {
		DTLSServerMbedTLS::finalize();
		PacketPeerMbedDTLS::finalize_dtls();
		StreamPeerMbedTLS::finalize_ssl();
		CryptoMbedTLS::finalize_crypto();

#if MBEDTLS_VERSION_MAJOR >= 3
		mbedtls_psa_crypto_free();
#endif
	}

#ifdef GODOT_MBEDTLS_THREADING_ALT
	mbedtls_threading_free_alt();
#endif
}
