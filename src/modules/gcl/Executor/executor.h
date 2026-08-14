/**************************************************************************/
/*  executor.h                                                            */
/**************************************************************************/
/*  GCL Executor - Minimal yorumlayici.                                   */
/*  Fonksiyon govdesindeki PrintF/print cagrilarini calistirir.           */
/**************************************************************************/

#ifndef GCL_EXECUTOR_H
#define GCL_EXECUTOR_H

#include "core/map.h"
#include "core/ustring.h"
#include "core/variant.h"
#include "core/vector.h"

namespace gcl {

/* Kullanici tanimli tip bilgisi (typedef / enum / struct / union). */
struct GCLTypeInfo {
	bool is_enum;   /* enum mu */
	bool is_struct; /* struct/union mu */
	Vector<String> members; /* enum: sabit adlari; struct: alan adlari */

	GCLTypeInfo() :
			is_enum(false),
			is_struct(false) {
	}
};

/* Calisma zamani tip kayit defteri (GCLScriptInstance tasir). */
struct GCLTypeRegistry {
	Map<StringName, String> aliases;    /* typedef adi -> orijinal tip */
	Map<StringName, GCLTypeInfo> types; /* "Color", "Player", ... */
	String source;                      /* calistirilan script kaynagi (fonksiyon/class icin) */
	Map<StringName, Dictionary> classes; /* "FATHER" -> class sablonu (base/src/head) */
	int call_depth;                     /* kullanici fonksiyon cagri derinligi (recursion guard) */

	/* Time.Sleep non-blocking: motor axarken bu script'in devamini erteletir.
	   Sleep YALNIZCA cagrildigi fonksiyonun kalan satirlarini erteler;
	   Update/UpdatePhysics govdesi beklerken her frame calismaya devam eder.
	   sleep_armed: Sleep satiri bir kez kuruldu (aynı satir her frame yeniden
	   tetiklendigi surece sleep_until kaydirilmasin). sleep_rest_captured:
	   kalan satirlar + scope ilk tetiklemede bir kez kopyalandi. Sure dolunca
	   gcl_script.cpp sleep_rest'i sleep_scope ile bir kez calistirir. */
	bool sleep_pending;
	bool sleep_armed;
	bool sleep_rest_captured;
	double sleep_until;
	String sleep_rest;
	Map<StringName, Variant> sleep_scope;

	GCLTypeRegistry() :
			call_depth(0),
			sleep_pending(false),
			sleep_armed(false),
			sleep_rest_captured(false),
			sleep_until(0.0) {
	}
};

/* p_code icinde "<p_func>(...)" tanimini ara; r_after_paren "{" konumunu alir.
   Donus tipi "void" olmak zorunda degil: "void foo()", "int foo()",
   "gcChar Call()" gibi tum imzalar bulunur. */
bool find_func_def(const String &p_code, const String &p_func, int &r_after_paren);

/* Satirlari yorumlar. r_exit: 0=normal, 1=break, 2=continue, 3=return.
   p_retval != nullptr ise "return <ifade>" degeri oraya yazilir. */
void executor_run_ex(const String &p_body, Map<StringName, Variant> &p_members, GCLTypeRegistry &p_types, int &r_exit, Variant *p_retval = nullptr);

/* p_code icinde "void <p_func>( ... ) { govde }" ara, govdeyi r_body'ye alir. */
bool executor_find_body(const String &p_code, const String &p_func, String &r_body);

/* Kaynak kod icindeki tum fonksiyon govdelerini ({ ... }) cikarir;
   geriye global scope kodu (degisken tanimlari / atamalar) kalir. */
String executor_strip_bodies(const String &p_code);

/* Govdedeki PrintF/print cagrilarini yorumlar (printf calismaz);
   degerler p_members'tan okunur, atamalar (<tip> <isim> = ...)
   p_members'a yazilir. p_types: typedef/enum/struct/union kayitlari. */
void executor_run(const String &p_body, Map<StringName, Variant> &p_members, GCLTypeRegistry &p_types);

} // namespace gcl

#endif // GCL_EXECUTOR_H
