#!/bin/sh

if [ -f "$1" ]
then
	PKG_DIR=./$(1)_pkg
	mkdir -p $PKG_DIR/usr
	mkdir -p $PKG_DIR/usr/bin
	cp $1 $PKG_DIR/usr/bin/

	mkdir -p $PKG_DIR/CONTROL
	echo "Package: $1" > $PKG_DIR/CONTROL/control
	echo "Priority: optional" >> $PKG_DIR/CONTROL/control
	echo "Section: Misc" >> $PKG_DIR/CONTROL/control
	echo "Version: 1.0" >> $PKG_DIR/CONTROL/control
	echo "Architecture: arm" >> $PKG_DIR/CONTROL/control
	echo "Maintainer: sswroom@yahoo.com" >> $PKG_DIR/CONTROL/control
	echo "Depends:" >> $PKG_DIR/CONTROL/control
	echo "Description: $1" >> $PKG_DIR/CONTROL/control

	echo "#!/bin/sh" > $PKG_DIR/CONTROL/postinst

	echo "#!/bin/sh" > $PKG_DIR/CONTROL/preinst
	echo "if ipkg-cl list_installed | grep '^$1 '; then" >> $PKG_DIR/CONTROL/preinst
	echo "	echo \"This package is already installed on the system, please\"" >> $PKG_DIR/CONTROL/preinst
	echo "	echo \"uninstall the previous version first\"" >> $PKG_DIR/CONTROL/preinst
	echo "	exit 1" >> $PKG_DIR/CONTROL/preinst
	echo "fi" >> $PKG_DIR/CONTROL/preinst
	echo "" >> $PKG_DIR/CONTROL/preinst
	echo "exit 0" >> $PKG_DIR/CONTROL/preinst

	echo "#!/bin/sh" > $PKG_DIR/CONTROL/postrm

fi
