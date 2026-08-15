/**************************************************************************/
/*  executor_dll.h                                                       */
/**************************************************************************/
/*  GCL Executor - #extern "dll" / #register dis kutuphane destegi.      */
/*  Kullanici DLL/SO yukleyip fonksiyonlarini GCL icinden cagirir:        */
/*                                                                        */
/*    #extern "dll"                                                       */
/*    #register "MesajVer", "C:/oyun/bin/plugin.dll"                      */
/*    void Ready() { MesajVer("merhaba"); }                               */
/*                                                                        */
/*  Desteklenen C imzalari (Windows __cdecl, diger platformlar C):        */
/*    int Func(void)                  -> 0 arguman, int doner              */
/*    int Func(char * / void *)       -> string arguman, int doner         */
/*    int Func(double * / const void*)-> sayi arguman, int doner           */
/*  Amac: gelistiriciye motor disinda yuksek-kontrollu islem ani kazandir- */
/*  mak; imza bilgisi ureticiye aittir.                                   */
/**************************************************************************/

#ifndef GCL_EXECUTOR_DLL_H
#define GCL_EXECUTOR_DLL_H

#include "executor.h"
#include "executor_core.h"
#include "executor_format.h"

#include "core/error_macros.h"
#include "core/os/os.h"
#include "core/print_string.h"
#include "core/project_settings.h"

namespace gcl {
namespace executor_dll {

using executor_format::variant_str;
using executor_format::variant_real;
using executor_format::variant_int;
using executor_format::is_ident_char;

/* "#register "Ad", "path/to/lib.dll"[, "sembol"]" direktifini isler.
   Kutuphaneyi acar (Windows: .dll, diger: .so), sembolu alir ve
   p_types.dll_funcs'e kaydeder. Donus: true = islendi. */
inline bool handle_dll_register(const String &p_line, GCLTypeRegistry &p_types) {
	String s = p_line.strip_edges();
	if (!s.begins_with("#register ")) {
		return false;
	}

	/* Argumanlari virgulle ayir: "Ad", "yol"[, "sembol"] */
	String args_region = s.substr(10).strip_edges();
	Vector<String> args;
	int a_start = 0;
	for (int j = 0; j <= args_region.length(); j++) {
		if (j == args_region.length() || args_region[j] == ',') {
			String a = args_region.substr(a_start, j - a_start).strip_edges();
			if (!a.empty()) {
				args.push_back(a);
			}
			a_start = j + 1;
		}
	}
	if (args.size() < 2) {
		return true; /* hatali imza: sessizce yok sayilir */
	}

	String name = args[0];
	String path = args[1];
	if (name.length() >= 2 && name[0] == '"' && name[name.length() - 1] == '"') {
		name = name.substr(1, name.length() - 2);
	}
	if (path.length() >= 2 && path[0] == '"' && path[path.length() - 1] == '"') {
		path = path.substr(1, path.length() - 2);
	}
	if (name.empty() || path.empty()) {
		return true;
	}

	/* Opsiyonel 3. arguman: gercek DLL sembol adi (GCL adindan farkliysa). */
	String symbol_name = name;
	if (args.size() >= 3) {
		String third = args[2];
		if (third.length() >= 2 && third[0] == '"' && third[third.length() - 1] == '"') {
			third = third.substr(1, third.length() - 2);
		}
		if (!third.empty()) {
			symbol_name = third;
		}
	}

	GCLDllFunc rec;
	rec.name = name;
	rec.path = path;

	/* Kutuphaneyi ac. p_also_set_library_path: DLL bagimliliklarini da bul. */
	Error err = OS::get_singleton()->open_dynamic_library(ProjectSettings::get_singleton()->globalize_path(path), rec.handle, true);
	if (err != OK || !rec.handle) {
		ERR_PRINT("GCL: failed to open dynamic library '" + path + "'.");
		return true;
	}
	p_types.dll_handles.push_back(rec.handle);

	/* Sembolu al. */
	err = OS::get_singleton()->get_dynamic_library_symbol_handle(rec.handle, symbol_name, rec.symbol, true);
	if (err != OK || !rec.symbol) {
		ERR_PRINT("GCL: symbol '" + symbol_name + "' not found in '" + path + "'.");
		return true;
	}
	rec.valid = true;

	p_types.dll_funcs[StringName(name)] = rec;
	return true;
}

/* C-fonksiyon giris noktalari. Desteklenen imzalar:
     int Func(void)          -> 0 arguman, int doner
     int Func(char *arg)     -> 1 string arguman
     int Func(double *arg)   -> 1 sayi arguman */
inline bool dll_invoke(const GCLDllFunc &p_func, const Array &p_args, Variant *r_retval = nullptr) {
	if (!p_func.valid || !p_func.symbol) {
		return false;
	}

	if (p_args.size() == 0) {
		typedef int (*GclFn0)(void);
		GclFn0 fn = reinterpret_cast<GclFn0>(p_func.symbol);
		if (fn) {
			int r = fn();
			if (r_retval) {
				*r_retval = r;
			}
			return true;
		}
		return false;
	}

	Variant first = p_args[0];
	if (first.get_type() == Variant::STRING) {
		typedef int (*GclFnStr)(void *);
		GclFnStr fn = reinterpret_cast<GclFnStr>(p_func.symbol);
		if (fn) {
			String s = variant_str(first);
			int r = fn((void *)s.c_str());
			if (r_retval) {
				*r_retval = r;
			}
			return true;
		}
		return false;
	}

	typedef int (*GclFnNum)(const void *);
	GclFnNum fn = reinterpret_cast<GclFnNum>(p_func.symbol);
	if (fn) {
		double v = variant_real(first);
		int r = fn((const void *)&v);
		if (r_retval) {
			*r_retval = r;
		}
		return true;
	}
	return false;
}

/* "<DllAdi>(args)" cagrisini isler. p_types.dll_funcs kaydindaki sembolu
   argumanlarla birlikte cagirir; sonuc r_retval'a yazilir. */
inline bool handle_dll_call(const String &p_line, const Map<StringName, Variant> &p_members, GCLTypeRegistry &p_types, Variant *r_retval = nullptr) {
	String s = p_line.strip_edges().replace(";", "").strip_edges();
	if (s.empty() || s.find("(") == -1) {
		return false;
	}

	int open = s.find("(");
	String fname = s.substr(0, open).strip_edges();
	if (fname.empty()) {
		return false;
	}
	for (int i = 0; i < fname.length(); i++) {
		if (!is_ident_char(fname[i])) {
			return false;
		}
	}
	if (!p_types.dll_funcs.has(StringName(fname))) {
		return false;
	}

	int depth = 0;
	bool in_str = false;
	int close = -1;
	const int L = s.length();
	for (int j = open; j < L; j++) {
		CharType c = s[j];
		if (c == '"') {
			in_str = !in_str;
		} else if (!in_str) {
			if (c == '(') {
				depth++;
			} else if (c == ')') {
				depth--;
				if (depth == 0) {
					close = j;
					break;
				}
			}
		}
	}
	if (close == -1 || close != L - 1) {
		return false;
	}

	Array call_args;
	String args_region = s.substr(open + 1, close - open - 1);
	int a_start = 0;
	for (int j = 0; j <= args_region.length(); j++) {
		if (j == args_region.length() || args_region[j] == ',') {
			String arg = args_region.substr(a_start, j - a_start).strip_edges();
			if (!arg.empty()) {
				call_args.push_back(executor_core::initialize_value(arg, p_members));
			}
			a_start = j + 1;
		}
	}

	return dll_invoke(p_types.dll_funcs[StringName(fname)], call_args, r_retval);
}

} // namespace executor_dll

} // namespace gcl

#endif // GCL_EXECUTOR_DLL_H
