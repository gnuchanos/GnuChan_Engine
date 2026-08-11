/**************************************************************************/
/*  node_reference.h                                                      */
/**************************************************************************/
/*                         This file is part of:                          */
/*                             GNUCHANIDE ENGINE                          */
/*                        https://github.com/gnuchanos                    */
/**************************************************************************/
/*  node_refferance: script-visible type name for safe Node references.  */
/*    - Holds a safe reference (ObjectID) to a Node. It does NOT own the */
/*      node and does NOT keep it alive. If the node is freed,           */
/*      get_node() returns null instead of a dangling pointer.           */
/*    - This is a VALUE-TYPE used by the FPS pickup/drop system so the   */
/*      script knows which object to drop without find() at runtime.     */
/*                                                                        */
/*  Script usage:                                                         */
/*    var hand: node_refferance = null                                   */
/*    hand = node_refferance.new()                                       */
/*    hand.node = hedef   # reference to any Node                        */
/*    $"Head/Camera".drop = hand   # tells the camera what to drop       */
/*                                                                        */
/*  Permission is hereby granted, free of charge, to any person obtaining */
/*  a copy of this software and associated documentation files (the       */
/*  "Software"), to deal in the Software without restriction, including   */
/*  without limitation the rights to use, copy, modify, merge, publish,   */
/*  distribute, sublicense, and/or sell copies of the Software, and to    */
/*  permit persons to whom the Software is furnished to do so, subject to */
/*  the following conditions:                                             */
/*                                                                        */
/*  The above copyright notice and this permission notice shall be        */
/*  included in all copies or substantial portions of the Software.       */
/*                                                                        */
/*  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,       */
/*  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF    */
/*  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. */
/*  IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY  */
/*  CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,  */
/*  TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE     */
/*  SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                */
/**************************************************************************/

#ifndef NODE_REFERENCE_H
#define NODE_REFERENCE_H

#include "core/object.h"

class node_refferance : public Object {
	GDCLASS(node_refferance, Object);

	ObjectID node_id;

protected:
	static void _bind_methods();

public:
	/* Store the reference. Passing null clears it. */
	void set_node(Object *p_node);

	/* The referenced node if it is still alive, otherwise null. */
	Object *get_node() const;

	/* Convenience: is the referenced node still alive? */
	bool is_valid() const;

	node_refferance();
};

#endif // NODE_REFERENCE_H
