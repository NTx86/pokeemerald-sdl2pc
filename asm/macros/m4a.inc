	.macro song label:req, music_player:req, unknown:req
	ptrvalue \label
	.short \music_player
	.short \unknown
	space64 4
	.endm

	.macro music_player info_struct:req, track_struct:req, num_tracks:req, unknown:req
	ptrvalue \info_struct
	ptrvalue \track_struct
	.byte \num_tracks
	.space 1  /* Padding*/
	.short \unknown
	space64 4  /* Padding*/
	.endm

	.if VER_64BIT == 1
	.equiv voice_size, 0x18
	.else
	.equiv voice_size, 0xC
	.endif

	.macro voice_group label:req, starting_note
	.align 2
	.ifb \starting_note
		.global voicegroup_\label 
		voicegroup_\label:
	.else
		.set voicegroup_\label, . - \starting_note * voice_size
	.endif
	.endm

	.macro keysplit label:req, starting_note
	.ifb \starting_note
		.global keysplit_\label
		keysplit_\label:
		.set _last_note, 0
	.else
		.set keysplit_\label, . - \starting_note
		.set _last_note, \starting_note
	.endif
	.set _last_split, 0
	.endm

	.macro split index:req, ending_note:req
	.if \ending_note < _last_note
		.if _last_split == 0
			.error "split's ending_note earlier than previous keysplit's starting_note"
		.else
			.error "split's ending_note earlier than previous split's ending_note"
		.endif
	.else
	.rept \ending_note - _last_note
	  .byte \index
	.endr
	.endif
	.set _last_note, \ending_note
	.set _last_split, 1
	.endm
