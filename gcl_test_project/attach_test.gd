extends SceneTree

func _initialize():
	print("loading GCL...")
	var s = load("res://test.gcl")
	print("GCL loaded: ", s)
	var n = Node.new()
	n.set_script(s)
	print("GCL attached: ", n.get_script())
	n.queue_free()
	quit()
