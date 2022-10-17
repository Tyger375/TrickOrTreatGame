extends Sprite

func _ready():
	var _1 = preload("res://assets/Vehicles/Vehicle1.png")
	var _2 = preload("res://assets/Vehicles/Vehicle2.png")
	var _3 = preload("res://assets/Vehicles/Vehicle3.png")
	var all = [_1, _2, _3]
	randomize()
	set_texture(all[randi()%(all.size())])
