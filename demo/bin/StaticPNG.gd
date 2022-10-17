extends Node2D

var counter = 0;
var increaser = 5;
var bored: bool;

func _ready():
	# start_anim();
	pass

func _process(delta):
	if counter > 0:
		get_child(0).z_index = 10;
		if not bored:
			var pos: Vector2 = self.position
			if counter <= 0.5:
				print("down");
				pos.y += 50*delta
			else:
				print("up");
				pos.y -= 50*delta
			self.position = pos
			counter -= increaser * delta;
		else:
			counter -= 0.9 * delta;
	else:
		get_child(0).z_index = -1;

func start_bored():
	bored = true
	counter = 0.9;
	get_child(0).animation = "Non-scared";

func start_anim():
	counter = 0.9;
	get_child(0).animation = "Scared";
