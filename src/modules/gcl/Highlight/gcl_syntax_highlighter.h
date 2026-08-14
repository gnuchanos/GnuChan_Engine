/**************************************************************************/
/*  gcl_syntax_highlighter.h                                              */
/**************************************************************************/
/*  GCL icin ozel editor syntax highlighter.                              */
/*  Kullanici tarafindan verilen 10 renk, her kategoriye yayilir:          */
/*    54016f -> kontrol kelimeleri (if/else/for/while/return...)          */
/*    700294 -> tip kelimeleri (int, float, int8, void...)                */
/*    9603c6 -> identity: degisken / fonksiyon adlari                     */
/*    ad06e4 -> = operatörü ve atamalar                                   */
/*    d863ff -> @extern, #include gibi direktifler                        */
/*    4c0080 -> # yorum                                                   */
/*    5b0099 -> sayilar / degerler (30, 3.14, true/false)                 */
/*    6c01b5 -> string icerigi                                            */
/*    8200db -> string icindeki %d %5d %.2f %Lf specifier'lar             */
/*    9700ff -> PrintF / print fonksiyon adlari                           */
/**************************************************************************/

#ifndef GCL_SYNTAX_HIGHLIGHTER_H
#define GCL_SYNTAX_HIGHLIGHTER_H

#ifdef TOOLS_ENABLED

#include "scene/gui/TextEdit_Node/text_edit.h"

namespace gcl {

class GCLSyntaxHighlighter : public SyntaxHighlighter {
public:
	/* 10 renk (kullanici paleti, kategori kategorisine yayilir). */
	Color control_color; /* 54016f */
	Color type_color; /* 700294 */
	Color identity_color; /* 9603c6 */
	Color operator_color; /* ad06e4 */
	Color directive_color; /* d863ff */
	Color comment_color; /* 4c0080 */
	Color number_color; /* 5b0099 */
	Color string_color; /* 6c01b5 */
	Color spec_color; /* 8200db */
	Color function_color; /* 9700ff */

	virtual String get_name() const;
	virtual List<String> get_supported_languages();
	virtual void _update_cache();
	virtual Map<int, TextEdit::HighlighterInfo> _get_line_syntax_highlighting(int p_line);
};

/* Editor'e highlighter fabrikasini kaydeder (register_gcl_types cagirir). */
void register_gcl_syntax_highlighter();

} // namespace gcl

#endif // TOOLS_ENABLED

#endif // GCL_SYNTAX_HIGHLIGHTER_H
