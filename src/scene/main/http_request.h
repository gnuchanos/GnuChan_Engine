/**************************************************************************/
/*  http_request.h                                                        */
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

#ifndef HTTP_REQUEST_H
#define HTTP_REQUEST_H

#include "core/io/http_client.h"
#include "core/os/file_access.h"
#include "core/os/thread.h"
#include "core/safe_refcount.h"
#include "node.h"
#include "scene/main/timer.h"

class HTTPRequest : public Node {
	GDCLASS(HTTPRequest, Node);

public:
	enum Result {
		RESULT_SUCCESS,
		RESULT_CHUNKED_BODY_SIZE_MISMATCH,
		RESULT_CANT_CONNECT,
		RESULT_CANT_RESOLVE,
		RESULT_CONNECTION_ERROR,
		RESULT_SSL_HANDSHAKE_ERROR,
		RESULT_NO_RESPONSE,
		RESULT_BODY_SIZE_LIMIT_EXCEEDED,
		RESULT_REQUEST_FAILED,
		RESULT_DOWNLOAD_FILE_CANT_OPEN,
		RESULT_DOWNLOAD_FILE_WRITE_ERROR,
		RESULT_REDIRECT_LIMIT_REACHED,
		RESULT_TIMEOUT

	};

private:
	bool requesting;

	String request_string;
	String url;
	int port;
	Vector<String> headers;
	bool validate_ssl;
	bool use_ssl;
	HTTPClient::Method method;
	PoolVector<uint8_t> request_data;

	bool request_sent;
	Ref<HTTPClient> client;
	PoolByteArray body;
	SafeFlag use_threads;

	bool got_response;
	int response_code;
	PoolVector<String> response_headers;

	String download_to_file;

	FileAccess *file;

	int body_len;
	SafeNumeric<int> downloaded;
	int body_size_limit;

	int redirections;

	bool _update_connection();

	int max_redirects;

	double timeout;

	void _redirect_request(const String &p_new_url);

	bool _handle_response(bool *ret_value);

	Error _parse_url(const String &p_url);
	Error _request();

	SafeFlag thread_done;
	SafeFlag thread_request_quit;

	Thread thread;

	void _request_done(int p_status, int p_code, const PoolStringArray &p_headers, const PoolByteArray &p_data);
	static void _thread_func(void *p_userdata);

protected:
	void _notification(int p_what);
	static void _bind_methods();

public:
	Error request(const String &p_url, const Vector<String> &p_custom_headers = Vector<String>(), bool p_ssl_validate_domain = true, HTTPClient::Method p_method = HTTPClient::METHOD_GET, const String &p_request_data = ""); //connects to a full url and perform request
	Error request_raw(const String &p_url, const Vector<String> &p_custom_headers = Vector<String>(), bool p_ssl_validate_domain = true, HTTPClient::Method p_method = HTTPClient::METHOD_GET, const PoolVector<uint8_t> &p_request_data_raw = PoolVector<uint8_t>()); //connects to a full url and perform request
	void cancel_request();
	HTTPClient::Status get_http_client_status() const;

	void set_use_threads(bool p_use);
	bool is_using_threads() const;

	void set_download_file(const String &p_file);
	String get_download_file() const;

	void set_download_chunk_size(int p_chunk_size);
	int get_download_chunk_size() const;

	void set_body_size_limit(int p_bytes);
	int get_body_size_limit() const;

	void set_max_redirects(int p_max);
	int get_max_redirects() const;

	Timer *timer;

	void set_timeout(double p_timeout);
	double get_timeout();

	void _timeout();

	int get_downloaded_bytes() const;
	int get_body_size() const;

	// Use empty string or -1 to unset.
	void set_http_proxy(const String &p_host, int p_port);
	void set_https_proxy(const String &p_host, int p_port);

	HTTPRequest();
	~HTTPRequest();
};

VARIANT_ENUM_CAST(HTTPRequest::Result);

#endif // HTTP_REQUEST_H
