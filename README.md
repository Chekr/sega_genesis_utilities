SEGA GENESIS UTILITIES
========

Utilities for facilitating Sega Genesis development


--------------------------------
--------------------------------

project    :  make_palette_from_img
purpose    :  Generate a palette in a desired format from a bitmap image
build with :  > make
builds to  :  bin/palette_maker.elf

flags: 
-f [filename] -> 24-bit bitmap image file to use for the palette generation
-t [type] -> TYPES: "MEGA_DRIVE", "MASTER_SYSTEM", "GAMEGEAR", "TMS" (bugged), "RGB" (default)

ex:
> palette_maker.elf -f tile.bmp -t MASTER_SYSTEM > ms_palette.txt

--------------------------------
--------------------------------

project    :  bmp_to_tiles
purpose    :  Generate a palette in a desired format from a bitmap image
build with :  > make
builds to  :  bin/bmp_to_tiles.elf

flags: 
-f [filename] -> 24-bit bitmap image file to make into tiles
-p [palette filename] -> pre-generated palette to use
-t [type] -> TYPES: "MEGA_DRIVE", "MASTER_SYSTEM", "GAMEGEAR", "TMS", "RGB" (default)

ex:
> bmp_to_tiles.elf -f tilesheet.bmp -t MASTER_SYSTEM -p ms_palette.txt > tiles.txt

--------------------------------
--------------------------------

project    :  3d_to_2d_coords (UNFINISHED)
purpose    :  project 3d coordinates to 2d
build with :  > make
