/**************************************************************************/
/*  extern.cpp                                                            */
/**************************************************************************/
/*  GCL Parser - Extern modulu.                                           */
/*  "@extern <node>" satirlarini bulur; self.<node> erisimi saglar.       */
/**************************************************************************/

#include "parser.h"

namespace gcl {

void extern_scan(const String &p_code, Vector<String> &r_nodes) {
	r_nodes.clear();

	/* Satir satir ilerle; "@extern" ile baslayan satirlari yakala. */
	int line_start = 0;
	const int len = p_code.length();

	while (line_start <= len) {
		int line_end = p_code.find("\n", line_start);
		if (line_end == -1) {
			line_end = len;
		}

		String line = p_code.substr(line_start, line_end - line_start);
		line = line.strip_edges();

		/* @extern xyz */
		if (line.begins_with("@extern")) {
			String rest = line.substr(7).strip_edges();
			if (!rest.empty()) {
				/* Boslukla ayrilmis ilk kelime: node adi */
				int sp = rest.find(" ");
				if (sp != -1) {
					rest = rest.substr(0, sp);
				}
				/* noktali virgul varsa at */
				int sc = rest.find(";");
				if (sc != -1) {
					rest = rest.substr(0, sc);
				}
				rest = rest.strip_edges();
				if (!rest.empty()) {
					r_nodes.push_back(rest);
				}
			}
		}

		if (line_end == len) {
			break;
		}
		line_start = line_end + 1;
	}
}

} // namespace gcl
