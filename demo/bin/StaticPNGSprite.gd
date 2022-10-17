extends Sprite

func _ready():
	var _1 = preload("res://assets/PNGs/1.png")
	var _2 = preload("res://assets/PNGs/2.png")
	var _3 = preload("res://assets/PNGs/3.png")
	var _4 = preload("res://assets/PNGs/4.png")
	var _5 = preload("res://assets/PNGs/5.png")
	var _6 = preload("res://assets/PNGs/6.png")
	var all = [_1, _2, _3, _4, _5, _6]
	randomize()
	set_texture(all[randi()%(all.size())])
