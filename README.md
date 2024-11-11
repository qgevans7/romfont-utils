# romfont-utils
Tools for manipulating PBM fonts and generating C headers for small console fonts

WARNING: DO NOT USE
Currently, this only works in a very minimal way. Only 4-pixel wide fonts (2 to a byte in the header) work,
p1bm2h must be invoked manually with ASCII PBM input on stdin, outputting a C header on stdout
(e.g.: `./p1bm2h < font.pbm > font.h`). The goal is to eventually make this  a general tool useful to other people
working on embedded/retro projects with bitmap framebuffers, but this turned out not to be nearly as simple as I
envisioned, so I'll be working in baby steps, starting with what I end up actually needing for my pointless
retro OS project so that this doesn't become one of my characteristic show-stopping  distractions.

If it's useful to you as a starting point, please make a pull request. There's not much shape to it yet, so I'm
likely to accept basically anything, so long as you don't mind my tweaking it to fit my code style.

(PS: Sorry about the globals. I'm using the excuse that old v7/BSD stdin->stdout utilities were often like this)
