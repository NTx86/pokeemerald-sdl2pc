	.macro song label:req, music_player:req, unknown:req
	.int \label
	.short \music_player
	.short \unknown
	.endm

	.macro music_player info_struct:req, track_struct:req, num_tracks:req, unknown:req
	.int \info_struct
	.int \track_struct
	.byte \num_tracks
	.space 1  /* Padding*/
	.short \unknown
	.endm
