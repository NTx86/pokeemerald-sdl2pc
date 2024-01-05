FortreeCity_DecorationShop_MapScripts::
	.byte 0

FortreeCity_DecorationShop_EventScript_PokefanM::
	msgbox FortreeCity_DecorationShop_Text_MerchandiseSentToPC, MSGBOX_NPC
	end

FortreeCity_DecorationShop_EventScript_Girl::
	msgbox FortreeCity_DecorationShop_Text_BuyingDeskForDolls, MSGBOX_NPC
	end

FortreeCity_DecorationShop_EventScript_ClerkDesks::
	lock
	faceplayer
	message gText_HowMayIServeYou
	waitmessage
	pokemartdecoration FortreeCity_DecorationShop_PokemartDecor_Desks
	msgbox gText_PleaseComeAgain, MSGBOX_DEFAULT
	release
	end

	.align 2
FortreeCity_DecorationShop_PokemartDecor_Desks:
	.short DECOR_SMALL_DESK
	.short DECOR_POKEMON_DESK
	.short DECOR_HEAVY_DESK
	.short DECOR_RAGGED_DESK
	.short DECOR_COMFORT_DESK
	.short DECOR_BRICK_DESK
	.short DECOR_CAMP_DESK
	.short DECOR_HARD_DESK
	.short DECOR_NONE
	release
	end

FortreeCity_DecorationShop_EventScript_ClerkChairs::
	lock
	faceplayer
	message gText_HowMayIServeYou
	waitmessage
	pokemartdecoration FortreeCity_DecorationShop_PokemartDecor_Chairs
	msgbox gText_PleaseComeAgain, MSGBOX_DEFAULT
	release
	end

	.align 2
FortreeCity_DecorationShop_PokemartDecor_Chairs:
	.short DECOR_SMALL_CHAIR
	.short DECOR_POKEMON_CHAIR
	.short DECOR_HEAVY_CHAIR
	.short DECOR_RAGGED_CHAIR
	.short DECOR_COMFORT_CHAIR
	.short DECOR_BRICK_CHAIR
	.short DECOR_CAMP_CHAIR
	.short DECOR_HARD_CHAIR
	.short DECOR_NONE
	release
	end

FortreeCity_DecorationShop_Text_MerchandiseSentToPC:
	.string "Merchandise you buy here is sent to\n"
	.string "your own PC.\p"
	.string "That's fantastic! I wish they could\n"
	.string "also deliver me home like that.$"

FortreeCity_DecorationShop_Text_BuyingDeskForDolls:
	.string "I'm buying a pretty desk and I'm\n"
	.string "putting my cute DOLLS on it.\p"
	.string "If I don't, when I decorate my\n"
	.string "SECRET BASE, my DOLLS will get\l"
	.string "dirty or poked with splinters.$"

