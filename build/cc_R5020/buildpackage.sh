#!/bin/sh

if [ -f "bin/$1" ]
then
	SDK_DIR=~/ROS-SDK-r5020-3.1.0
	PKG_DIR=$SDK_DIR/package
	mkdir -p $PKG_DIR/$1
	mkdir -p $PKG_DIR/$1/files
	cp bin/$1 $PKG_DIR/$1/files/

	echo "#!/bin/sh" > $PKG_DIR/$1/files/sdk.sh
	echo "" >> $PKG_DIR/$1/files/sdk.sh
	echo "RETVAL=0" >> $PKG_DIR/$1/files/sdk.sh
	echo "PROG=$1" >> $PKG_DIR/$1/files/sdk.sh
	echo "" >> $PKG_DIR/$1/files/sdk.sh
	echo "start()" >> $PKG_DIR/$1/files/sdk.sh
	echo "{" >> $PKG_DIR/$1/files/sdk.sh
	echo "  \${PROG} &" >> $PKG_DIR/$1/files/sdk.sh
	echo "  return \$?" >> $PKG_DIR/$1/files/sdk.sh
	echo "}" >> $PKG_DIR/$1/files/sdk.sh
	echo "" >> $PKG_DIR/$1/files/sdk.sh
	echo "stop()" >> $PKG_DIR/$1/files/sdk.sh
	echo "{" >> $PKG_DIR/$1/files/sdk.sh
	echo "  killall \${PROG}" >> $PKG_DIR/$1/files/sdk.sh
	echo "  RETVAL=\$?" >> $PKG_DIR/$1/files/sdk.sh
	echo "  return \$RETVAL" >> $PKG_DIR/$1/files/sdk.sh
	echo "}" >> $PKG_DIR/$1/files/sdk.sh
	echo "" >> $PKG_DIR/$1/files/sdk.sh
	echo "restart()" >> $PKG_DIR/$1/files/sdk.sh
	echo "{" >> $PKG_DIR/$1/files/sdk.sh
	echo "  stop" >> $PKG_DIR/$1/files/sdk.sh
	echo "  start" >> $PKG_DIR/$1/files/sdk.sh
	echo "}" >> $PKG_DIR/$1/files/sdk.sh
	echo "" >> $PKG_DIR/$1/files/sdk.sh
	echo "status()" >> $PKG_DIR/$1/files/sdk.sh
	echo "{" >> $PKG_DIR/$1/files/sdk.sh
	echo "  pidof -o %PPID \${PROG} >/dev/null 2>&1" >> $PKG_DIR/$1/files/sdk.sh
	echo "  RETVAL=\$?" >> $PKG_DIR/$1/files/sdk.sh
	echo "  [ \$RETVAL -eq 0 ] && echo \${PROG} is running || echo \${PROG} is stopped" >> $PKG_DIR/$1/files/sdk.sh
	echo "  return \$RETVAL" >> $PKG_DIR/$1/files/sdk.sh
	echo "}" >> $PKG_DIR/$1/files/sdk.sh
	echo "" >> $PKG_DIR/$1/files/sdk.sh
	echo "status_sync()" >> $PKG_DIR/$1/files/sdk.sh
	echo "{" >> $PKG_DIR/$1/files/sdk.sh
	echo "  return 0" >> $PKG_DIR/$1/files/sdk.sh
	echo "}" >> $PKG_DIR/$1/files/sdk.sh
	echo "" >> $PKG_DIR/$1/files/sdk.sh
	echo "case \"\$1\" in" >> $PKG_DIR/$1/files/sdk.sh
	echo "  start)" >> $PKG_DIR/$1/files/sdk.sh
	echo "  	start" >> $PKG_DIR/$1/files/sdk.sh
	echo "	;;" >> $PKG_DIR/$1/files/sdk.sh
	echo "  stop)" >> $PKG_DIR/$1/files/sdk.sh
	echo "  	stop" >> $PKG_DIR/$1/files/sdk.sh
	echo "	;;" >> $PKG_DIR/$1/files/sdk.sh
	echo "  restart)" >> $PKG_DIR/$1/files/sdk.sh
	echo "  	restart" >> $PKG_DIR/$1/files/sdk.sh
	echo "	;;" >> $PKG_DIR/$1/files/sdk.sh
	echo "  status)" >> $PKG_DIR/$1/files/sdk.sh
	echo "  	status" >> $PKG_DIR/$1/files/sdk.sh
	echo "	;;" >> $PKG_DIR/$1/files/sdk.sh
	echo "  on_startup)" >> $PKG_DIR/$1/files/sdk.sh
	echo "  	start" >> $PKG_DIR/$1/files/sdk.sh
	echo "	;;" >> $PKG_DIR/$1/files/sdk.sh
	echo "  on_link_up)" >> $PKG_DIR/$1/files/sdk.sh
#	echo "  	stop" >> $PKG_DIR/$1/files/sdk.sh
#	echo "  	start" >> $PKG_DIR/$1/files/sdk.sh
	echo "	;;" >> $PKG_DIR/$1/files/sdk.sh
	echo "  on_link_down)" >> $PKG_DIR/$1/files/sdk.sh
#	echo "  	stop" >> $PKG_DIR/$1/files/sdk.sh
	echo "	;;" >> $PKG_DIR/$1/files/sdk.sh
	echo "  on_reboot)" >> $PKG_DIR/$1/files/sdk.sh
	echo "  	stop" >> $PKG_DIR/$1/files/sdk.sh
	echo "	;;" >> $PKG_DIR/$1/files/sdk.sh
	echo "  status_sync)" >> $PKG_DIR/$1/files/sdk.sh
	echo "  	status_sync" >> $PKG_DIR/$1/files/sdk.sh
	echo "	;;" >> $PKG_DIR/$1/files/sdk.sh
	echo "  *)" >> $PKG_DIR/$1/files/sdk.sh
	echo "  	echo \$\"Usage: \$0 {start|stop|restart|status|on_startup|on_link_up|on_link_down|on_reboot|status_sync}\"" >> $PKG_DIR/$1/files/sdk.sh
	echo "  	exit 2" >> $PKG_DIR/$1/files/sdk.sh
	echo "esac" >> $PKG_DIR/$1/files/sdk.sh
	echo "" >> $PKG_DIR/$1/files/sdk.sh
	echo "exit \$?" >> $PKG_DIR/$1/files/sdk.sh

	echo "all:" > $PKG_DIR/$1/files/Makefile

	echo "include \$(TOPDIR)/rules.mk" > $PKG_DIR/$1/Makefile
	echo "" >> $PKG_DIR/$1/Makefile
	echo "PKG_NAME:=$1" >> $PKG_DIR/$1/Makefile
	echo "PKG_VERSION:=1.0.0" >> $PKG_DIR/$1/Makefile
	echo "PKG_DESC:=$1" >> $PKG_DIR/$1/Makefile
	echo "" >> $PKG_DIR/$1/Makefile
	echo "PKG_BUILD_DIR:=\$(BUILD_DIR)/\$(PKG_NAME)-\$(PKG_VERSION)" >> $PKG_DIR/$1/Makefile
	echo "" >> $PKG_DIR/$1/Makefile
	echo "PKG_UNPACK:=cp -rf files/* \$(PKG_BUILD_DIR)" >> $PKG_DIR/$1/Makefile
	echo "" >> $PKG_DIR/$1/Makefile
	echo "PKG_BUILD_PARALLEL:=1" >> $PKG_DIR/$1/Makefile
	echo "PKG_BUILD_DEPENDS:=" >> $PKG_DIR/$1/Makefile
	echo "" >> $PKG_DIR/$1/Makefile
	echo "include \$(INCLUDE_DIR)/package.mk" >> $PKG_DIR/$1/Makefile
	echo "" >> $PKG_DIR/$1/Makefile
	echo "define Package/Install" >> $PKG_DIR/$1/Makefile
	echo "	\$(INSTALL_DIR) \$(1)/usr/bin/" >> $PKG_DIR/$1/Makefile
	echo "	\$(INSTALL_BIN) files/$1 \$(1)/usr/bin/" >> $PKG_DIR/$1/Makefile
	echo "	\$(INSTALL_DIR) \$(1)/etc/sdk/" >> $PKG_DIR/$1/Makefile
	echo "	\$(INSTALL_BIN) files/sdk.sh \$(1)/etc/sdk/\$(PKG_NAME)" >> $PKG_DIR/$1/Makefile
	if [ 1 -eq 0 ]
	then
		echo "	\$(INSTALL_DIR) \$(1)/etc/router/uci" >> $PKG_DIR/$1/Makefile
		echo "	\$(INSTALL_DATA) files/uci.xml \$(1)/etc/router/uci/\$(PKG_NAME).xml" >> $PKG_DIR/$1/Makefile
	fi
	echo "endef" >> $PKG_DIR/$1/Makefile
	echo "" >> $PKG_DIR/$1/Makefile
	echo "\$(eval \$(call BuildPackage))" >> $PKG_DIR/$1/Makefile

	cd $SDK_DIR
	rm -r build_dir/target-r5020/$1-1.0.0/
	make package/$1/install
fi
