#
#

inhibit: inhibit.cc
	c++ -o $@ `pkg-config --cflags --libs Qt5Gui Qt5Widgets Qt5DBus` $<
