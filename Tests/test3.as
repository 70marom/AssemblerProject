.data	1,-1,1,1980
.data  -1980
.data   #111
.data   6.9
.data

cmp #545454, tst
cmp r1, #-545454
sub #410
sub #5,
bne
add  #1, #-1

yo  r1, r2
cmp  #test, r1
stop boop
jmp .entry

r1: .string "invalid label"
10101010: .string "whats up tal?"
.string 50
.something 50
.entry
.extern
.struct 50, 50
.struct "50", "50"
