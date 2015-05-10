import sys

INDENT_STRING = "  "

level = 0
for line in sys.stdin.readlines():
    if line.startswith( "Leaving test " ):
        level -= 1
    sys.stdout.write( "{}{}".format( level * INDENT_STRING, line ) )
    if line.startswith( "Entering test " ):
        level += 1
