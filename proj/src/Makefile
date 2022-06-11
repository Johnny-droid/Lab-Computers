# name of the program (Minix service)
PROG=proj

.PATH: ${.CURDIR}/timer/
.PATH: ${.CURDIR}/util/
.PATH: ${.CURDIR}/keyboard/
.PATH: ${.CURDIR}/video_card/
.PATH: ${.CURDIR}/sprites/
.PATH: ${.CURDIR}/mouse/
.PATH: ${.CURDIR}/leaderboard/
.PATH: ${.CURDIR}/rtc/

# source code files to be compiled
SRCS = proj.c game.c video_card.c keyboard.c mouse.c timer.c utils.c leaderboard.c rtc.c

# additional compilation flags
# "-Wall -Wextra -Werror -I . -std=c11 -Wno-unused-parameter" are already set
CFLAGS += -pedantic -DLAB3  -D__LCOM_OPTIMIZED__

# list of library dependencies (for Lab 2, only LCF library)
DPADD += ${LIBLCF}
LDADD += -llcf

# include LCOM's makefile that does all the "heavy lifting"
.include <minix.lcom.mk>


