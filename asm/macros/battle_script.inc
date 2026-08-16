/* commands*/
	.macro attackcanceler
	.byte B_SCR_OP_ATTACKCANCELER
	.endm

	.macro accuracycheck failPtr:req, move:req
	.byte B_SCR_OP_ACCURACYCHECK
	ptrvalue \failPtr
	.short \move
	.endm

	.macro attackstring
	.byte B_SCR_OP_ATTACKSTRING
	.endm

	.macro ppreduce
	.byte B_SCR_OP_PPREDUCE
	.endm

	.macro critcalc
	.byte B_SCR_OP_CRITCALC
	.endm

	.macro damagecalc
	.byte B_SCR_OP_DAMAGECALC
	.endm

	.macro typecalc
	.byte B_SCR_OP_TYPECALC
	.endm

	.macro adjustnormaldamage
	.byte B_SCR_OP_ADJUSTNORMALDAMAGE
	.endm

	.macro adjustnormaldamage2
	.byte B_SCR_OP_ADJUSTNORMALDAMAGE2
	.endm

	.macro attackanimation
	.byte B_SCR_OP_ATTACKANIMATION
	.endm

	.macro waitanimation
	.byte B_SCR_OP_WAITANIMATION
	.endm

	.macro healthbarupdate battler:req
	.byte B_SCR_OP_HEALTHBARUPDATE
	.byte \battler
	.endm

	.macro datahpupdate battler:req
	.byte B_SCR_OP_DATAHPUPDATE
	.byte \battler
	.endm

	.macro critmessage
	.byte B_SCR_OP_CRITMESSAGE
	.endm

	.macro effectivenesssound
	.byte B_SCR_OP_EFFECTIVENESSSOUND
	.endm

	.macro resultmessage
	.byte B_SCR_OP_RESULTMESSAGE
	.endm

	.macro printstring id:req
	.byte B_SCR_OP_PRINTSTRING
	.short \id
	.endm

	.macro printselectionstring id:req
	.byte B_SCR_OP_PRINTSELECTIONSTRING
	.short \id
	.endm

	.macro waitmessage time:req
	.byte B_SCR_OP_WAITMESSAGE
	.short \time
	.endm

	.macro printfromtable ptr:req
	.byte B_SCR_OP_PRINTFROMTABLE
	ptrvalue \ptr
	.endm

	.macro printselectionstringfromtable ptr:req
	.byte B_SCR_OP_PRINTSELECTIONSTRINGFROMTABLE
	ptrvalue \ptr
	.endm

	.macro seteffectwithchance
	.byte B_SCR_OP_SETEFFECTWITHCHANCE
	.endm

	.macro seteffectprimary
	.byte B_SCR_OP_SETEFFECTPRIMARY
	.endm

	.macro seteffectsecondary
	.byte B_SCR_OP_SETEFFECTSECONDARY
	.endm

	.macro clearstatusfromeffect battler:req
	.byte B_SCR_OP_CLEARSTATUSFROMEFFECT
	.byte \battler
	.endm

	.macro tryfaintmon battler:req
	.byte B_SCR_OP_TRYFAINTMON
	.byte \battler
	.byte FALSE
	ptrvalue NULL
	.endm

	.macro tryfaintmon_spikes battler:req, ptr:req
	.byte B_SCR_OP_TRYFAINTMON
	.byte \battler
	.byte TRUE
	ptrvalue \ptr
	.endm

	.macro dofaintanimation battler:req
	.byte B_SCR_OP_DOFAINTANIMATION
	.byte \battler
	.endm

	.macro cleareffectsonfaint battler:req
	.byte B_SCR_OP_CLEAREFFECTSONFAINT
	.byte \battler
	.endm

	.macro jumpifstatus battler:req, status1:req, ptr:req
	.byte B_SCR_OP_JUMPIFSTATUS
	.byte \battler
	.int \status1
	ptrvalue \ptr
	.endm

	.macro jumpifstatus2 battler:req, status2:req, ptr:req
	.byte B_SCR_OP_JUMPIFSTATUS2
	.byte \battler
	.int \status2
	ptrvalue \ptr
	.endm

	.macro jumpifability battler:req, ability:req, ptr:req
	.byte B_SCR_OP_JUMPIFABILITY
	.byte \battler
	.byte \ability
	ptrvalue \ptr
	.endm

	.macro jumpifsideaffecting battler:req, sidestatus:req, ptr:req
	.byte B_SCR_OP_JUMPIFSIDEAFFECTING
	.byte \battler
	.short \sidestatus
	ptrvalue \ptr
	.endm

	.macro jumpifstat battler:req, ifflag:req, stat:req, value:req, ptr
	.byte B_SCR_OP_JUMPIFSTAT
	.byte \battler
	.byte \ifflag
	.byte \stat
	.byte \value
	ptrvalue \ptr
	.endm

	.macro jumpifstatus3condition battler:req, status3:req, condition:req, ptr:req
	.byte B_SCR_OP_JUMPIFSTATUS3CONDITION
	.byte \battler
	.int \status3
	.byte \condition
	ptrvalue \ptr
	.endm

	.macro jumpiftype battler:req, type:req, ptr:req
	.byte B_SCR_OP_JUMPIFTYPE
	.byte \battler
	.byte \type
	ptrvalue \ptr
	.endm

	.macro getexp battler:req
	.byte B_SCR_OP_GETEXP
	.byte \battler
	.endm

	.macro checkteamslost ptr:req
	.byte B_SCR_OP_CHECKTEAMSLOST
	ptrvalue \ptr
	.endm

	.macro movevaluescleanup
	.byte B_SCR_OP_MOVEVALUESCLEANUP
	.endm

	.macro setmultihit value:req
	.byte B_SCR_OP_SETMULTIHIT
	.byte \value
	.endm

	.macro decrementmultihit value:req
	.byte B_SCR_OP_DECREMENTMULTIHIT
	ptrvalue \value
	.endm

	.macro goto ptr:req
	.byte B_SCR_OP_GOTO
	ptrvalue \ptr
	.endm

	.macro jumpifbyte ifflag:req, val:req, byte:req, ptr:req
	.byte B_SCR_OP_JUMPIFBYTE
	.byte \ifflag
	ptrvalue \val
	.byte \byte
	ptrvalue \ptr
	.endm

	.macro jumpifhalfword ifflag:req, val:req, hword:req, ptr:req
	.byte B_SCR_OP_JUMPIFHALFWORD
	.byte \ifflag
	ptrvalue \val
	.short \hword
	ptrvalue \ptr
	.endm

	.macro jumpifword ifflag:req, val:req, word:req, ptr:req
	.byte B_SCR_OP_JUMPIFWORD
	.byte \ifflag
	ptrvalue \val
	.int \word
	ptrvalue \ptr
	.endm

	.macro jumpifarrayequal val1:req, val2:req, size:req, ptr:req
	.byte B_SCR_OP_JUMPIFARRAYEQUAL
	ptrvalue \val1
	ptrvalue \val2
	.byte \size
	ptrvalue \ptr
	.endm

	.macro jumpifarraynotequal val1:req, val2:req, size:req, ptr:req
	.byte B_SCR_OP_JUMPIFARRAYNOTEQUAL
	ptrvalue \val1
	ptrvalue \val2
	.byte \size
	ptrvalue \ptr
	.endm

	.macro setbyte ptr:req, byte:req
	.byte B_SCR_OP_SETBYTE
	ptrvalue \ptr
	.byte \byte
	.endm

	.macro addbyte ptr:req, byte:req
	.byte B_SCR_OP_ADDBYTE
	ptrvalue \ptr
	.byte \byte
	.endm

	.macro subbyte ptr:req, byte:req
	.byte B_SCR_OP_SUBBYTE
	ptrvalue \ptr
	.byte \byte
	.endm

	.macro copyarray dest:req, src:req, size:req
	.byte B_SCR_OP_COPYARRAY
	ptrvalue \dest
	ptrvalue \src
	.byte \size
	.endm

	.macro copyarraywithindex dest:req, src:req, index:req, size:req
	.byte B_SCR_OP_COPYARRAYWITHINDEX
	ptrvalue \dest
	ptrvalue \src
	ptrvalue \index
	.byte \size
	.endm

	.macro orbyte ptr:req, byte:req
	.byte B_SCR_OP_ORBYTE
	ptrvalue \ptr
	.byte \byte
	.endm

	.macro orhalfword ptr:req, hword:req
	.byte B_SCR_OP_ORHALFWORD
	ptrvalue \ptr
	.short \hword
	.endm

	.macro orword ptr:req, word:req
	.byte B_SCR_OP_ORWORD
	ptrvalue \ptr
	.int \word
	.endm

	.macro bicbyte ptr:req, byte:req
	.byte B_SCR_OP_BICBYTE
	ptrvalue \ptr
	.byte \byte
	.endm

	.macro bichalfword ptr:req, hword:req
	.byte B_SCR_OP_BICHALFWORD
	ptrvalue \ptr
	.short \hword
	.endm

	.macro bicword ptr:req, word:req
	.byte B_SCR_OP_BICWORD
	ptrvalue \ptr
	.int \word
	.endm

	.macro pause time:req
	.byte B_SCR_OP_PAUSE
	.short \time
	.endm

	.macro waitstate
	.byte B_SCR_OP_WAITSTATE
	.endm

	.macro healthbar_update battler:req
	.byte B_SCR_OP_HEALTHBAR_UPDATE
	.byte \battler
	.endm

	.macro return
	.byte B_SCR_OP_RETURN
	.endm

	.macro end
	.byte B_SCR_OP_END
	.endm

	.macro end2
	.byte B_SCR_OP_END2
	.endm

	.macro end3
	.byte B_SCR_OP_END3
	.endm

	.macro jumpifaffectedbyprotect ptr:req
	.byte B_SCR_OP_JUMPIFAFFECTEDBYPROTECT
	ptrvalue \ptr
	.endm

	.macro call ptr:req
	.byte B_SCR_OP_CALL
	ptrvalue \ptr
	.endm

	.macro jumpiftype2 battler:req, type:req, ptr:req
	.byte B_SCR_OP_JUMPIFTYPE2
	.byte \battler
	.byte \type
	ptrvalue \ptr
	.endm

	.macro jumpifabilitypresent ability:req, ptr:req
	.byte B_SCR_OP_JUMPIFABILITYPRESENT
	.byte \ability
	ptrvalue \ptr
	.endm

	.macro endselectionscript
	.byte B_SCR_OP_ENDSELECTIONSCRIPT
	.endm

	.macro playanimation battler:req, animType:req, arg=NULL
	.byte B_SCR_OP_PLAYANIMATION
	.byte \battler
	.byte \animType
	ptrvalue \arg
	.endm

	.macro playanimation_var battler:req, animType:req, arg=NULL
	.byte B_SCR_OP_PLAYANIMATION_VAR
	.byte \battler
	ptrvalue \animType
	ptrvalue \arg
	.endm

	.macro setgraphicalstatchangevalues
	.byte B_SCR_OP_SETGRAPHICALSTATCHANGEVALUES
	.endm

	.macro playstatchangeanimation battler:req, stats:req, statchange:req
	.byte B_SCR_OP_PLAYSTATCHANGEANIMATION
	.byte \battler
	.byte \stats
	.byte \statchange
	.endm

	.macro moveend endMode:req, endState:req
	.byte B_SCR_OP_MOVEEND
	.byte \endMode
	.byte \endState
	.endm

	/* Help macros for 5 uses of moveend command*/

	/* All cases*/
	.macro moveendall
	setbyte sMOVEEND_STATE, 0
	moveend 0, 0
	.endm

	/* Chosen case*/
	.macro moveendcase case:req
	setbyte sMOVEEND_STATE, \case
	moveend 1, 0
	.endm

	/* All cases from (inclusive)*/
	.macro moveendfrom from:req
	setbyte sMOVEEND_STATE, \from
	moveend 0, 0
	.endm

	/* All cases from 0 to (not inclusive)*/
	.macro moveendto to:req
	setbyte sMOVEEND_STATE, 0
	moveend 2, \to
	.endm

	/* Cases from (inclusive) to (not inclusive)*/
	.macro moveendfromto from:req, to:req
	setbyte sMOVEEND_STATE, \from
	moveend 2, \to
	.endm

	.macro typecalc2
	.byte B_SCR_OP_TYPECALC2
	.endm

	.macro returnatktoball
	.byte B_SCR_OP_RETURNATKTOBALL
	.endm

	.macro getswitchedmondata battler:req
	.byte B_SCR_OP_GETSWITCHEDMONDATA
	.byte \battler
	.endm

	.macro switchindataupdate battler:req
	.byte B_SCR_OP_SWITCHINDATAUPDATE
	.byte \battler
	.endm

	.macro switchinanim battler:req, dontclearsubstitutebit:req
	.byte B_SCR_OP_SWITCHINANIM
	.byte \battler
	.byte \dontclearsubstitutebit
	.endm

	.macro jumpifcantswitch battler:req, ptr:req
	.byte B_SCR_OP_JUMPIFCANTSWITCH
	.byte \battler
	ptrvalue \ptr
	.endm

	.macro openpartyscreen battler:req, ptr:req
	.byte B_SCR_OP_OPENPARTYSCREEN
	.byte \battler
	ptrvalue \ptr
	.endm

	.macro switchhandleorder battler:req, state:req
	.byte B_SCR_OP_SWITCHHANDLEORDER
	.byte \battler
	.byte \state
	.endm

	.macro switchineffects battler:req
	.byte B_SCR_OP_SWITCHINEFFECTS
	.byte \battler
	.endm

	.macro trainerslidein battler:req
	.byte B_SCR_OP_TRAINERSLIDEIN
	.byte \battler
	.endm

	.macro playse song:req
	.byte B_SCR_OP_PLAYSE
	.short \song
	.endm

	.macro fanfare song:req
	.byte B_SCR_OP_FANFARE
	.short \song
	.endm

	.macro playfaintcry battler:req
	.byte B_SCR_OP_PLAYFAINTCRY
	.byte \battler
	.endm

	.macro endlinkbattle
	.byte B_SCR_OP_ENDLINKBATTLE
	.endm

	.macro returntoball battler:req
	.byte B_SCR_OP_RETURNTOBALL
	.byte \battler
	.endm

	.macro handlelearnnewmove learnedMovePtr:req, nothingToLearnPtr:req, isFirstMove:req
	.byte B_SCR_OP_HANDLELEARNNEWMOVE
	ptrvalue \learnedMovePtr
	ptrvalue \nothingToLearnPtr
	.byte \isFirstMove
	.endm

	.macro yesnoboxlearnmove forgotMovePtr:req
	.byte B_SCR_OP_YESNOBOXLEARNMOVE
	ptrvalue \forgotMovePtr
	.endm

	.macro yesnoboxstoplearningmove noPtr:req
	.byte B_SCR_OP_YESNOBOXSTOPLEARNINGMOVE
	ptrvalue \noPtr
	.endm

	.macro hitanimation battler:req
	.byte B_SCR_OP_HITANIMATION
	.byte \battler
	.endm

	.macro getmoneyreward
	.byte B_SCR_OP_GETMONEYREWARD
	.endm

	.macro updatebattlermoves battler:req
	.byte B_SCR_OP_UPDATEBATTLERMOVES
	.byte \battler
	.endm

	.macro swapattackerwithtarget
	.byte B_SCR_OP_SWAPATTACKERWITHTARGET
	.endm

	.macro incrementgamestat stat:req
	.byte B_SCR_OP_INCREMENTGAMESTAT
	.byte \stat
	.endm

	.macro drawpartystatussummary battler:req
	.byte B_SCR_OP_DRAWPARTYSTATUSSUMMARY
	.byte \battler
	.endm

	.macro hidepartystatussummary battler:req
	.byte B_SCR_OP_HIDEPARTYSTATUSSUMMARY
	.byte \battler
	.endm

	.macro jumptocalledmove notChosenMove:req
	.byte B_SCR_OP_JUMPTOCALLEDMOVE
	.byte \notChosenMove
	.endm

	.macro statusanimation battler:req
	.byte B_SCR_OP_STATUSANIMATION
	.byte \battler
	.endm

	.macro status2animation battler:req, status2:req
	.byte B_SCR_OP_STATUS2ANIMATION
	.byte \battler
	.int \status2
	.endm

	.macro chosenstatusanimation battler:req, isStatus2:req, status:req
	.byte B_SCR_OP_CHOSENSTATUSANIMATION
	.byte \battler
	.byte \isStatus2
	.int \status
	.endm

	.macro yesnobox
	.byte B_SCR_OP_YESNOBOX
	.endm

	.macro cancelallactions
	.byte B_SCR_OP_CANCELALLACTIONS
	.endm

	.macro adjustsetdamage
	.byte B_SCR_OP_ADJUSTSETDAMAGE
	.endm

	.macro removeitem battler:req
	.byte B_SCR_OP_REMOVEITEM
	.byte \battler
	.endm

	.macro atknameinbuff1
	.byte B_SCR_OP_ATKNAMEINBUFF1
	.endm

	.macro drawlvlupbox
	.byte B_SCR_OP_DRAWLVLUPBOX
	.endm

	.macro resetsentmonsvalue
	.byte B_SCR_OP_RESETSENTMONSVALUE
	.endm

	.macro setatktoplayer0
	.byte B_SCR_OP_SETATKTOPLAYER0
	.endm

	.macro makevisible battler:req
	.byte B_SCR_OP_MAKEVISIBLE
	.byte \battler
	.endm

	.macro recordlastability battler:req
	.byte B_SCR_OP_RECORDLASTABILITY
	.byte \battler
	.endm

	.macro buffermovetolearn
	.byte B_SCR_OP_BUFFERMOVETOLEARN
	.endm

	.macro jumpifplayerran ptr:req
	.byte B_SCR_OP_JUMPIFPLAYERRAN
	ptrvalue \ptr
	.endm

	.macro hpthresholds battler:req
	.byte B_SCR_OP_HPTHRESHOLDS
	.byte \battler
	.endm

	.macro hpthresholds2 battler:req
	.byte B_SCR_OP_HPTHRESHOLDS2
	.byte \battler
	.endm

	.macro useitemonopponent
	.byte B_SCR_OP_USEITEMONOPPONENT
	.endm

	.macro various battler:req, id:req
	.byte B_SCR_OP_VARIOUS
	.byte \battler
	.byte \id
	.endm

	.macro setprotectlike
	.byte B_SCR_OP_SETPROTECTLIKE
	.endm

	.macro tryexplosion
	.byte B_SCR_OP_TRYEXPLOSION
	.endm

	.macro setatkhptozero
	.byte B_SCR_OP_SETATKHPTOZERO
	.endm

	.macro jumpifnexttargetvalid ptr:req
	.byte B_SCR_OP_JUMPIFNEXTTARGETVALID
	ptrvalue \ptr
	.endm

	.macro tryhealhalfhealth ptr:req, battler:req
	.byte B_SCR_OP_TRYHEALHALFHEALTH
	ptrvalue \ptr
	.byte \battler
	.endm

	.macro trymirrormove
	.byte B_SCR_OP_TRYMIRRORMOVE
	.endm

	.macro setrain
	.byte B_SCR_OP_SETRAIN
	.endm

	.macro setreflect
	.byte B_SCR_OP_SETREFLECT
	.endm

	.macro setseeded
	.byte B_SCR_OP_SETSEEDED
	.endm

	.macro manipulatedamage mode:req
	.byte B_SCR_OP_MANIPULATEDAMAGE
	.byte \mode
	.endm

	.macro trysetrest ptr:req
	.byte B_SCR_OP_TRYSETREST
	ptrvalue \ptr
	.endm

	.macro jumpifnotfirstturn ptr:req
	.byte B_SCR_OP_JUMPIFNOTFIRSTTURN
	ptrvalue \ptr
	.endm

	.macro nop
	.byte B_SCR_OP_NOP
	.endm

	.macro jumpifcantmakeasleep ptr:req
	.byte B_SCR_OP_JUMPIFCANTMAKEASLEEP
	ptrvalue \ptr
	.endm

	.macro stockpile
	.byte B_SCR_OP_STOCKPILE
	.endm

	.macro stockpiletobasedamage ptr:req
	.byte B_SCR_OP_STOCKPILETOBASEDAMAGE
	ptrvalue \ptr
	.endm

	.macro stockpiletohpheal ptr:req
	.byte B_SCR_OP_STOCKPILETOHPHEAL
	ptrvalue \ptr
	.endm

	.macro negativedamage
	.byte B_SCR_OP_NEGATIVEDAMAGE
	.endm

	.macro statbuffchange flags:req, jumpptr:req
	.byte B_SCR_OP_STATBUFFCHANGE
	.byte \flags
	ptrvalue \jumpptr
	.endm

	.macro normalisebuffs
	.byte B_SCR_OP_NORMALISEBUFFS
	.endm

	.macro setbide
	.byte B_SCR_OP_SETBIDE
	.endm

	.macro confuseifrepeatingattackends
	.byte B_SCR_OP_CONFUSEIFREPEATINGATTACKENDS
	.endm

	.macro setmultihitcounter val:req
	.byte B_SCR_OP_SETMULTIHITCOUNTER
	.byte \val
	.endm

	.macro initmultihitstring
	.byte B_SCR_OP_INITMULTIHITSTRING
	.endm

	.macro forcerandomswitch ptr:req
	.byte B_SCR_OP_FORCERANDOMSWITCH
	ptrvalue \ptr
	.endm

	.macro tryconversiontypechange ptr:req
	.byte B_SCR_OP_TRYCONVERSIONTYPECHANGE
	ptrvalue \ptr
	.endm

	.macro givepaydaymoney
	.byte B_SCR_OP_GIVEPAYDAYMONEY
	.endm

	.macro setlightscreen
	.byte B_SCR_OP_SETLIGHTSCREEN
	.endm

	.macro tryKO ptr:req
	.byte B_SCR_OP_TRYKO
	ptrvalue \ptr
	.endm

	.macro damagetohalftargethp
	.byte B_SCR_OP_DAMAGETOHALFTARGETHP
	.endm

	.macro setsandstorm
	.byte B_SCR_OP_SETSANDSTORM
	.endm

	.macro weatherdamage
	.byte B_SCR_OP_WEATHERDAMAGE
	.endm

	.macro tryinfatuating ptr:req
	.byte B_SCR_OP_TRYINFATUATING
	ptrvalue \ptr
	.endm

	.macro updatestatusicon battler:req
	.byte B_SCR_OP_UPDATESTATUSICON
	.byte \battler
	.endm

	.macro setmist
	.byte B_SCR_OP_SETMIST
	.endm

	.macro setfocusenergy
	.byte B_SCR_OP_SETFOCUSENERGY
	.endm

	.macro transformdataexecution
	.byte B_SCR_OP_TRANSFORMDATAEXECUTION
	.endm

	.macro setsubstitute
	.byte B_SCR_OP_SETSUBSTITUTE
	.endm

	.macro mimicattackcopy ptr:req
	.byte B_SCR_OP_MIMICATTACKCOPY
	ptrvalue \ptr
	.endm

	.macro metronome
	.byte B_SCR_OP_METRONOME
	.endm

	.macro dmgtolevel
	.byte B_SCR_OP_DMGTOLEVEL
	.endm

	.macro psywavedamageeffect
	.byte B_SCR_OP_PSYWAVEDAMAGEEFFECT
	.endm

	.macro counterdamagecalculator ptr:req
	.byte B_SCR_OP_COUNTERDAMAGECALCULATOR
	ptrvalue \ptr
	.endm

	.macro mirrorcoatdamagecalculator ptr:req
	.byte B_SCR_OP_MIRRORCOATDAMAGECALCULATOR
	ptrvalue \ptr
	.endm

	.macro disablelastusedattack ptr:req
	.byte B_SCR_OP_DISABLELASTUSEDATTACK
	ptrvalue \ptr
	.endm

	.macro trysetencore ptr:req
	.byte B_SCR_OP_TRYSETENCORE
	ptrvalue \ptr
	.endm

	.macro painsplitdmgcalc ptr:req
	.byte B_SCR_OP_PAINSPLITDMGCALC
	ptrvalue \ptr
	.endm

	.macro settypetorandomresistance ptr:req
	.byte B_SCR_OP_SETTYPETORANDOMRESISTANCE
	ptrvalue \ptr
	.endm

	.macro setalwayshitflag
	.byte B_SCR_OP_SETALWAYSHITFLAG
	.endm

	.macro copymovepermanently ptr:req
	.byte B_SCR_OP_COPYMOVEPERMANENTLY
	ptrvalue \ptr
	.endm

	.macro trychoosesleeptalkmove ptr:req
	.byte B_SCR_OP_TRYCHOOSESLEEPTALKMOVE
	ptrvalue \ptr
	.endm

	.macro setdestinybond
	.byte B_SCR_OP_SETDESTINYBOND
	.endm

	.macro trysetdestinybondtohappen
	.byte B_SCR_OP_TRYSETDESTINYBONDTOHAPPEN
	.endm

	.macro remaininghptopower
	.byte B_SCR_OP_REMAININGHPTOPOWER
	.endm

	.macro tryspiteppreduce ptr:req
	.byte B_SCR_OP_TRYSPITEPPREDUCE
	ptrvalue \ptr
	.endm

	.macro healpartystatus
	.byte B_SCR_OP_HEALPARTYSTATUS
	.endm

	.macro cursetarget ptr:req
	.byte B_SCR_OP_CURSETARGET
	ptrvalue \ptr
	.endm

	.macro trysetspikes ptr:req
	.byte B_SCR_OP_TRYSETSPIKES
	ptrvalue \ptr
	.endm

	.macro setforesight
	.byte B_SCR_OP_SETFORESIGHT
	.endm

	.macro trysetperishsong ptr:req
	.byte B_SCR_OP_TRYSETPERISHSONG
	ptrvalue \ptr
	.endm

	.macro rolloutdamagecalculation
	.byte B_SCR_OP_ROLLOUTDAMAGECALCULATION
	.endm

	.macro jumpifconfusedandstatmaxed stat:req, ptr:req
	.byte B_SCR_OP_JUMPIFCONFUSEDANDSTATMAXED
	.byte \stat
	ptrvalue \ptr
	.endm

	.macro furycuttercalc
	.byte B_SCR_OP_FURYCUTTERCALC
	.endm

	.macro friendshiptodamagecalculation
	.byte B_SCR_OP_FRIENDSHIPTODAMAGECALCULATION
	.endm

	.macro presentdamagecalculation
	.byte B_SCR_OP_PRESENTDAMAGECALCULATION
	.endm

	.macro setsafeguard
	.byte B_SCR_OP_SETSAFEGUARD
	.endm

	.macro magnitudedamagecalculation
	.byte B_SCR_OP_MAGNITUDEDAMAGECALCULATION
	.endm

	.macro jumpifnopursuitswitchdmg ptr:req
	.byte B_SCR_OP_JUMPIFNOPURSUITSWITCHDMG
	ptrvalue \ptr
	.endm

	.macro setsunny
	.byte B_SCR_OP_SETSUNNY
	.endm

	.macro maxattackhalvehp ptr:req
	.byte B_SCR_OP_MAXATTACKHALVEHP
	ptrvalue \ptr
	.endm

	.macro copyfoestats ptr:req
	.byte B_SCR_OP_COPYFOESTATS
	ptrvalue \ptr
	.endm

	.macro rapidspinfree
	.byte B_SCR_OP_RAPIDSPINFREE
	.endm

	.macro setdefensecurlbit
	.byte B_SCR_OP_SETDEFENSECURLBIT
	.endm

	.macro recoverbasedonsunlight ptr:req
	.byte B_SCR_OP_RECOVERBASEDONSUNLIGHT
	ptrvalue \ptr
	.endm

	.macro hiddenpowercalc
	.byte B_SCR_OP_HIDDENPOWERCALC
	.endm

	.macro selectfirstvalidtarget
	.byte B_SCR_OP_SELECTFIRSTVALIDTARGET
	.endm

	.macro trysetfutureattack ptr:req
	.byte B_SCR_OP_TRYSETFUTUREATTACK
	ptrvalue \ptr
	.endm

	.macro trydobeatup endPtr:req, failPtr:req
	.byte B_SCR_OP_TRYDOBEATUP
	ptrvalue \endPtr
	ptrvalue \failPtr
	.endm

	.macro setsemiinvulnerablebit
	.byte B_SCR_OP_SETSEMIINVULNERABLEBIT
	.endm

	.macro clearsemiinvulnerablebit
	.byte B_SCR_OP_CLEARSEMIINVULNERABLEBIT
	.endm

	.macro setminimize
	.byte B_SCR_OP_SETMINIMIZE
	.endm

	.macro sethail
	.byte B_SCR_OP_SETHAIL
	.endm

	.macro trymemento ptr:req
	.byte B_SCR_OP_TRYMEMENTO
	ptrvalue \ptr
	.endm

	.macro setforcedtarget
	.byte B_SCR_OP_SETFORCEDTARGET
	.endm

	.macro setcharge
	.byte B_SCR_OP_SETCHARGE
	.endm

	.macro callenvironmentattack
	.byte B_SCR_OP_CALLENVIRONMENTATTACK
	.endm

	.macro cureifburnedparalyzedorpoisoned ptr:req
	.byte B_SCR_OP_CUREIFBURNEDPARALYZEDORPOISONED
	ptrvalue \ptr
	.endm

	.macro settorment ptr:req
	.byte B_SCR_OP_SETTORMENT
	ptrvalue \ptr
	.endm

	.macro jumpifnodamage ptr:req
	.byte B_SCR_OP_JUMPIFNODAMAGE
	ptrvalue \ptr
	.endm

	.macro settaunt ptr:req
	.byte B_SCR_OP_SETTAUNT
	ptrvalue \ptr
	.endm

	.macro trysethelpinghand ptr:req
	.byte B_SCR_OP_TRYSETHELPINGHAND
	ptrvalue \ptr
	.endm

	.macro tryswapitems ptr:req
	.byte B_SCR_OP_TRYSWAPITEMS
	ptrvalue \ptr
	.endm

	.macro trycopyability ptr:req
	.byte B_SCR_OP_TRYCOPYABILITY
	ptrvalue \ptr
	.endm

	.macro trywish turnNumber:req, ptr:req
	.byte B_SCR_OP_TRYWISH
	.byte \turnNumber
	ptrvalue \ptr
	.endm

	.macro trysetroots ptr:req
	.byte B_SCR_OP_TRYSETROOTS
	ptrvalue \ptr
	.endm

	.macro doubledamagedealtifdamaged
	.byte B_SCR_OP_DOUBLEDAMAGEDEALTIFDAMAGED
	.endm

	.macro setyawn ptr:req
	.byte B_SCR_OP_SETYAWN
	ptrvalue \ptr
	.endm

	.macro setdamagetohealthdifference ptr:req
	.byte B_SCR_OP_SETDAMAGETOHEALTHDIFFERENCE
	ptrvalue \ptr
	.endm

	.macro scaledamagebyhealthratio
	.byte B_SCR_OP_SCALEDAMAGEBYHEALTHRATIO
	.endm

	.macro tryswapabilities ptr:req
	.byte B_SCR_OP_TRYSWAPABILITIES
	ptrvalue \ptr
	.endm

	.macro tryimprison ptr:req
	.byte B_SCR_OP_TRYIMPRISON
	ptrvalue \ptr
	.endm

	.macro trysetgrudge ptr:req
	.byte B_SCR_OP_TRYSETGRUDGE
	ptrvalue \ptr
	.endm

	.macro weightdamagecalculation
	.byte B_SCR_OP_WEIGHTDAMAGECALCULATION
	.endm

	.macro assistattackselect ptr:req
	.byte B_SCR_OP_ASSISTATTACKSELECT
	ptrvalue \ptr
	.endm

	.macro trysetmagiccoat ptr:req
	.byte B_SCR_OP_TRYSETMAGICCOAT
	ptrvalue \ptr
	.endm

	.macro trysetsnatch ptr:req
	.byte B_SCR_OP_TRYSETSNATCH
	ptrvalue \ptr
	.endm

	.macro trygetintimidatetarget ptr:req
	.byte B_SCR_OP_TRYGETINTIMIDATETARGET
	ptrvalue \ptr
	.endm

	.macro switchoutabilities battler:req
	.byte B_SCR_OP_SWITCHOUTABILITIES
	.byte \battler
	.endm

	.macro jumpifhasnohp battler:req, ptr:req
	.byte B_SCR_OP_JUMPIFHASNOHP
	.byte \battler
	ptrvalue \ptr
	.endm

	.macro getsecretpowereffect
	.byte B_SCR_OP_GETSECRETPOWEREFFECT
	.endm

	.macro pickup
	.byte B_SCR_OP_PICKUP
	.endm

	.macro docastformchangeanimation
	.byte B_SCR_OP_DOCASTFORMCHANGEANIMATION
	.endm

	.macro trycastformdatachange
	.byte B_SCR_OP_TRYCASTFORMDATACHANGE
	.endm

	.macro settypebasedhalvers ptr:req
	.byte B_SCR_OP_SETTYPEBASEDHALVERS
	ptrvalue \ptr
	.endm

	.macro setweatherballtype
	.byte B_SCR_OP_SETWEATHERBALLTYPE
	.endm

	.macro tryrecycleitem ptr:req
	.byte B_SCR_OP_TRYRECYCLEITEM
	ptrvalue \ptr
	.endm

	.macro settypetoenvironment ptr:req
	.byte B_SCR_OP_SETTYPETOENVIRONMENT
	ptrvalue \ptr
	.endm

	.macro pursuitdoubles ptr:req
	.byte B_SCR_OP_PURSUITDOUBLES
	ptrvalue \ptr
	.endm

	.macro snatchsetbattlers
	.byte B_SCR_OP_SNATCHSETBATTLERS
	.endm

	.macro removelightscreenreflect
	.byte B_SCR_OP_REMOVELIGHTSCREENREFLECT
	.endm

	.macro handleballthrow
	.byte B_SCR_OP_HANDLEBALLTHROW
	.endm

	.macro givecaughtmon
	.byte B_SCR_OP_GIVECAUGHTMON
	.endm

	.macro trysetcaughtmondexflags ptr:req
	.byte B_SCR_OP_TRYSETCAUGHTMONDEXFLAGS
	ptrvalue \ptr
	.endm

	.macro displaydexinfo
	.byte B_SCR_OP_DISPLAYDEXINFO
	.endm

	.macro trygivecaughtmonnick ptr:req
	.byte B_SCR_OP_TRYGIVECAUGHTMONNICK
	ptrvalue \ptr
	.endm

	.macro subattackerhpbydmg
	.byte B_SCR_OP_SUBATTACKERHPBYDMG
	.endm

	.macro removeattackerstatus1
	.byte B_SCR_OP_REMOVEATTACKERSTATUS1
	.endm

	.macro finishaction
	.byte B_SCR_OP_FINISHACTION
	.endm

	.macro finishturn
	.byte B_SCR_OP_FINISHTURN
	.endm

	.macro trainerslideout position:req
	.byte B_SCR_OP_TRAINERSLIDEOUT
	.byte \position
	.endm

/* various command changed to more readable macros*/
	.macro cancelmultiturnmoves battler:req
	various \battler, VARIOUS_CANCEL_MULTI_TURN_MOVES
	.endm

	.macro setmagiccoattarget battler:req
	various \battler, VARIOUS_SET_MAGIC_COAT_TARGET
	.endm

	.macro getifcantrunfrombattle battler:req
	various \battler, VARIOUS_IS_RUNNING_IMPOSSIBLE
	.endm

	.macro getmovetarget battler:req
	various \battler, VARIOUS_GET_MOVE_TARGET
	.endm

	.macro getbattlerfainted battler:req
	various \battler, VARIOUS_GET_BATTLER_FAINTED
	.endm

	.macro resetintimidatetracebits battler:req
	various \battler, VARIOUS_RESET_INTIMIDATE_TRACE_BITS
	.endm

	.macro updatechoicemoveonlvlup battler:req
	various \battler, VARIOUS_UPDATE_CHOICE_MOVE_ON_LVL_UP
	.endm

	.macro resetplayerfainted
	various BS_ATTACKER, VARIOUS_RESET_PLAYER_FAINTED
	.endm

	.macro palaceflavortext battler:req
	various \battler, VARIOUS_PALACE_FLAVOR_TEXT
	.endm

	.macro arenajudgmentwindow
	various BS_ATTACKER, VARIOUS_ARENA_JUDGMENT_WINDOW
	.endm

	.macro arenaopponentmonlost
	various BS_ATTACKER, VARIOUS_ARENA_OPPONENT_MON_LOST
	.endm

	.macro arenaplayermonlost
	various BS_ATTACKER, VARIOUS_ARENA_PLAYER_MON_LOST
	.endm

	.macro arenabothmonlost
	various BS_ATTACKER, VARIOUS_ARENA_BOTH_MONS_LOST
	.endm

	.macro forfeityesnobox battler:req
	various \battler, VARIOUS_EMIT_YESNOBOX
	.endm

	.macro arenadrawreftextbox
	various BS_ATTACKER, VARIOUS_DRAW_ARENA_REF_TEXT_BOX
	.endm

	.macro arenaerasereftextbox
	various BS_ATTACKER, VARIOUS_ERASE_ARENA_REF_TEXT_BOX
	.endm

	.macro arenajudgmentstring id:req
	various \id, VARIOUS_ARENA_JUDGMENT_STRING
	.endm

	.macro arenawaitmessage id:req
	various \id, VARIOUS_ARENA_WAIT_STRING
	.endm

	.macro waitcry battler:req
	various \battler, VARIOUS_WAIT_CRY
	.endm

	.macro returnopponentmon1toball battler:req
	various \battler, VARIOUS_RETURN_OPPONENT_MON1
	.endm

	.macro returnopponentmon2toball battler:req
	various \battler, VARIOUS_RETURN_OPPONENT_MON2
	.endm

	.macro volumedown
	various BS_ATTACKER, VARIOUS_VOLUME_DOWN
	.endm

	.macro volumeup
	various BS_ATTACKER, VARIOUS_VOLUME_UP
	.endm

	.macro setalreadystatusedmoveattempt battler:req
	various \battler, VARIOUS_SET_ALREADY_STATUS_MOVE_ATTEMPT
	.endm

	.macro palacetryescapestatus battler:req
	various \battler, VARIOUS_PALACE_TRY_ESCAPE_STATUS
	.endm

	.macro setoutcomeonteleport battler:req
	various \battler, VARIOUS_SET_TELEPORT_OUTCOME
	.endm

	.macro playtrainerdefeatbgm battler:req
	various \battler, VARIOUS_PLAY_TRAINER_DEFEATED_MUSIC
	.endm

/* helpful macros*/
	.macro setstatchanger stat:req, stages:req, down:req
	setbyte sSTATCHANGER, \stat | \stages << 4 | \down << 7
	.endm

	.macro setmoveeffect effect:req
	setbyte cEFFECT_CHOOSER, \effect
	.endm

	.macro chosenstatus1animation battler:req, status:req
	chosenstatusanimation \battler, 0x0, \status
	.endm

	.macro chosenstatus2animation battler:req, status:req
	chosenstatusanimation \battler, 0x1, \status
	.endm

	.macro sethword dst:req, value:req
	setbyte \dst, (\value) & 0xFF
	setbyte \dst + 1, ((\value) >> 8) & 0xFF
	.endm

	.macro setword dst:req, value:req
	setbyte \dst, (\value) & 0xFF
	setbyte \dst + 1, ((\value) >> 8) & 0xFF
	setbyte \dst + 2, ((\value) >> 16) & 0xFF
	setbyte \dst + 3, ((\value) >> 24) & 0xFF
	.endm

	.macro copybyte dst:req, src:req
	copyarray \dst, \src, 0x1
	.endm

	.macro copyhword dst:req, src:req
	copyarray \dst, \src, 0x2
	.endm

	.macro copyword dst:req, src:req
	copyarray \dst, \src, 0x4
	.endm

	.macro jumpifbytenotequal byte1:req, byte2:req, jumpptr:req
	jumpifarraynotequal \byte1, \byte2, 0x1, \jumpptr
	.endm

	.macro jumpifbyteequal byte1:req, byte2:req, jumpptr:req
	jumpifarrayequal \byte1, \byte2, 0x1, \jumpptr
	.endm

	.macro jumpifmove move:req, jumpptr:req
	jumpifhalfword CMP_EQUAL, gCurrentMove, \move, \jumpptr
	.endm

	.macro jumpifnotmove move:req, jumpptr:req
	jumpifhalfword CMP_NOT_EQUAL, gCurrentMove, \move, \jumpptr
	.endm

	.macro jumpifstatus3 battler:req, status:req, jumpptr:req
	jumpifstatus3condition \battler, \status, FALSE, \jumpptr
	.endm

	.macro jumpifnostatus3 battler:req, status:req, jumpptr:req
	jumpifstatus3condition \battler, \status, TRUE, \jumpptr
	.endm

	.macro jumpifmovehadnoeffect jumpptr:req
	jumpifbyte CMP_COMMON_BITS, gMoveResultFlags, MOVE_RESULT_NO_EFFECT, \jumpptr
	.endm

	.macro jumpifbattletype flags:req, jumpptr:req
	jumpifword CMP_COMMON_BITS, gBattleTypeFlags, \flags, \jumpptr
	.endm

	.macro jumpifnotbattletype flags:req, jumpptr:req
	jumpifword CMP_NO_COMMON_BITS, gBattleTypeFlags, \flags, \jumpptr
	.endm
