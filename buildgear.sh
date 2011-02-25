#!/bin/bash
#
#  buildgear build/add/remove script
#
#  Copyright (c) 2011 Martin Lund
#  Copyright (c) 2000-2005 Per Liden
# 
#  This script file is originally written for pkgutils by Per Liden.
#  It is adopted and modified for buildgear by Martin Lund.
#
#  This program is free software; you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation; either version 2 of the License, or
#  (at your option) any later version.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this program; if not, write to the Free Software
#  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, 
#  USA.
#

bg_put() {
   if [ "$VERBOSE" = "yes" ]; then
      echo "$1" > /proc/$BG_PID/fd/2
   fi
}

info() {
	echo "$1"
}

warning() {
	echo "WARNING: $1"
   echo "     Warning     '$BUILD_TYPE/$name'  ($1)" > /proc/$BG_PID/fd/2
}

error() {
	echo "ERROR: $1"
   echo "     Error       '$BUILD_TYPE/$name'  ($1)" > /proc/$BG_PID/fd/2
}

log_action() {
   echo "======== $1 '$NAME' ==========================================="
   bg_put "    $1    '$BUILD_TYPE/$name'"
}

get_filename() {
	local FILE="`echo $1 | sed 's|^.*://.*/||g'`"

	if [ "$FILE" != "$1" ]; then
		FILE="$BG_SOURCE_DIR/$FILE"
   else
      FILE="$BG_BUILD_FILE_DIR/$FILE"
	fi

	echo $FILE
}

check_buildfile() {
	if [ ! "$name" ]; then
		error "Variable 'name' not specified in $BG_BUILDFILE"
		exit 1
	elif [ ! "$version" ]; then
		error "Variable 'version' not specified in $BG_BUILDFILE"
		exit 1
	elif [ ! "$release" ]; then
		error "Variable 'release' not specified in $BG_BUILDFILE"
		exit 1
	elif [ "`type -t build`" != "function" ]; then
		error "Function 'build' not specified in $BG_BUILDFILE"
		exit 1
	fi
}

check_create_directory() {
	if [ ! -d $1 ]; then
      mkdir -p $1
	elif [ ! -w $1 ]; then
		error "Directory '$1' not writable"
		exit 1
	elif [ ! -x $1 ] || [ ! -r $1 ]; then
		error "Directory '$1' not readable"
		exit 1
	fi
}

make_footprint() {
	tar tvf $BG_PACKAGE | \
      awk '{print $1 "\t" $6}' | \
		sort -k 2
}

make_sha256sum() {
	local FILE LOCAL_FILENAMES
	
	if [ "$source" ]; then
		for FILE in ${source[@]}; do
			LOCAL_FILENAMES="$LOCAL_FILENAMES `get_filename $FILE`"
		done

		sha256sum $LOCAL_FILENAMES | sed -e 's|  .*/|  |' | sort -k 2
	fi
}

check_sha256sum() {
	local FILE="$BG_WORK_DIR/.tmp"
	
	if [ -f $BG_SHA256SUM ]; then
		make_sha256sum > $FILE.sha256sum
		sort -k 2 $BG_SHA256SUM > $FILE.sha256sum.orig
		diff -w -t -U 0 $FILE.sha256sum.orig $FILE.sha256sum | \
			sed '/^@@/d' | \
			sed '/^+++/d' | \
			sed '/^---/d' | \
			sed 's/^+/  NEW       /g' | \
			sed 's/^-/  MISSING   /g' > $FILE.sha256sum.diff
		if [ -s $FILE.sha256sum.diff ]; then
			error "Sha256sum mismatch found"
			cat $FILE.sha256sum.diff

			if [ "$BG_CHECK_SHA256SUM" = "yes" ]; then
				error "Sha256sum mismatch"
				exit 1
			fi
			exit 1
		fi
	else
		if [ "$BG_CHECK_SHA256SUM" = "yes" ]; then
			info "Sha256sum not found"
			exit 1
		fi
		
		warning "Sha256sum not found, creating new"
		make_sha256sum > $BG_SHA256SUM
	fi

	if [ "$BG_CHECK_SHA256SUM" = "yes" ]; then
		info "Sha256sum ok"
		exit 0
	fi
}

do_checksum() {
	
   log_action "Checksum "
   
   if [ "$source" ]; then
      check_sha256sum
   fi
}

do_extract() {
	local FILE LOCAL_FILENAME COMMAND

   log_action "Extract  "

   check_create_directory "$BG_WORK_DIR"
   check_create_directory "$PKG"
   check_create_directory "$SRC"
	
	for FILE in ${source[@]}; do
		LOCAL_FILENAME=`get_filename $FILE`
		case $LOCAL_FILENAME in
			*.tar.gz|*.tar.Z|*.tgz)
				COMMAND="tar -C $SRC --use-compress-program=gzip -xf $LOCAL_FILENAME" ;;
			*.tar.bz2)
				COMMAND="tar -C $SRC --use-compress-program=bzip2 -xf $LOCAL_FILENAME" ;;
			*.zip)
				COMMAND="unzip -qq -o -d $SRC $LOCAL_FILENAME" ;;
			*.tar.xz| *.txz)
				COMMAND="tar -C $SRC --use-compress-program=xz -xf $LOCAL_FILENAME" ;;
			*)
				COMMAND="cp $LOCAL_FILENAME $SRC" ;;
		esac

		echo "$COMMAND"

		$COMMAND

	done
}

do_build() {
   
   log_action "Build    "
   
   cd $SRC
   
   (set -e -x ; build)

   if [ "$?" != "0" ]; then
      error "build() failed"
      exit 1
   fi
   
   cd $BG_ROOT_DIR
}

do_strip() {
	local FILE FILTER
	
   log_action "Strip    "
   
	cd $PKG
	
	if [ -f $BG_ROOT/$BG_NOSTRIP ]; then
		FILTER="grep -v -f $BG_ROOT/$BG_NOSTRIP"
	else
		FILTER="cat"
	fi

   if [ "$BUILD_TYPE" == "target" ]; then
      STRIP="$TARGET-strip"
   else
      STRIP="strip"
   fi

	find . -type f -printf "%P\n" | $FILTER | while read FILE; do
		if file -b "$FILE" | grep '^.*ELF.*executable.*not stripped$' &> /dev/null; then
			$STRIP --strip-all "$FILE"
		elif file -b "$FILE" | grep '^.*ELF.*shared object.*not stripped$' &> /dev/null; then
			$STRIP --strip-unneeded "$FILE"
		elif file -b "$FILE" | grep '^current ar archive$' &> /dev/null; then
			$STRIP --strip-debug "$FILE"
		fi
	done
   
   cd $BG_ROOT_DIR
}

do_package() {
   
   log_action "Package  "
   
   cd $PKG
   
   tar czvvf $BG_PACKAGE *
   
   cd $BG_ROOT_DIR
}

do_footprint() {
	local FILE
   
   log_action "Footprint"
   
   FILE="$BG_WORK_DIR/.tmp"
	
	if [ -f $BG_PACKAGE ]; then
		make_footprint > $FILE.footprint
		if [ -f $BG_FOOTPRINT ]; then
			sort -k 2 $BG_FOOTPRINT > $FILE.footprint.orig
			diff -w -t -U 0 $FILE.footprint.orig $FILE.footprint | \
				sed '/^@@/d' | \
				sed '/^+++/d' | \
				sed '/^---/d' | \
				sed 's/^+/NEW		 /g' | \
				sed 's/^-/MISSING	/g' > $FILE.footprint.diff
			if [ -s $FILE.footprint.diff ]; then
				warning "Footprint mismatch found"
				cat $FILE.footprint.diff
				BUILD_SUCCESSFUL="no"
			fi
		else
			warning "Footprint not found, creating new"
			mv $FILE.footprint $BG_FOOTPRINT
		fi
	else
		error "Package '$BG_PACKAGE' was not found"
		BUILD_SUCCESSFUL="no"
	fi
}

do_clean() {
   
   log_action "Clean    "
   
   if [ -d $BG_WORK_DIR ]
   then
      rm -rf $BG_WORK_DIR
   fi
   
   if [ "$?" != "0" ]; then
      error "Clean failed"
      exit 1
   fi
}

do_add() {
   
   log_action "Add      "
   
   if [ -d $BG_SYSROOT_DIR ]; then
      echo tar -C $BG_SYSROOT_DIR -xf $BG_PACKAGE
      tar -C $BG_SYSROOT_DIR -xf $BG_PACKAGE
   fi
   
   if [ "$?" != "0" ]; then
      error "Add failed"
      exit 1
   fi
   
}

do_remove() {
   
   log_action "Remove   "
   
   if [ -d $BG_SYSROOT_DIR ]; then
      cd $BG_SYSROOT_DIR
      tar -tvf $BG_PACKAGE | awk '{print $6}' | xargs rm -rf
      cd $BG_ROOT
   fi
}

parse_options() {
	BG_BUILDFILE=$1
}

main() {
	local FILE BG_PACKAGE
   
   # respawn with out redirected to log file
   exec &>> $BUILD_LOG_FILE
   
   # Clear aliases
   unalias -a
   
   # Sanitize environment
   GREP_OPTIONS=""
   
	parse_options "$@"
   
   . $BG_BUILDFILE
   
   check_buildfile

   # All paths must be based on BG_ROOT! (FIXME)
   # Deduct all paths from BG_ROOT and BG_BUILD_TYPE, BG_BUILD_FILE_DIR!
   BG_ROOT_DIR="$PWD"
   BG_BUILD_FILE_DIR="`dirname $BG_BUILDFILE`"
   BG_SOURCE_DIR="$BG_ROOT_DIR/$SOURCE_DIR"
   BG_WORK_DIR="$WORK_DIR/$BUILD_TYPE/$name"
   BG_PACKAGE_DIR="$PACKAGE_DIR/$BUILD_TYPE"
   BG_SYSROOT_DIR="$BG_ROOT_DIR/build/work/$BUILD_TYPE/sysroot"
   BG_HOST_SYSROOT_DIR="$BG_ROOT_DIR/build/work/host/sysroot"
   BG_TARGET_SYSROOT_DIR="$BG_ROOT_DIR/build/work/BG_PACKAGE/sysroot"
   BG_SHA256SUM="$BG_BUILD_FILE_DIR/.sha256sum"
   BG_FOOTPRINT="$BG_BUILD_FILE_DIR/.footprint"
   BG_NOSTRIP="$BG_BUILD_FILE_DIR/.nostrip"
   BG_PACKAGE="$BG_ROOT_DIR/$BG_PACKAGE_DIR/$name#$version-$release.pkg.tar.gz"

   PKG="$BG_ROOT_DIR/$BG_WORK_DIR/pkg"
   SRC="$BG_ROOT_DIR/$BG_WORK_DIR/src"
   BUILD="$BG_ROOT_DIR/build"
   
   umask 022

   # Include buildfiles configuration
	if [ -f $1 ]; then
      . $BUILD_FILES_CONFIG
   fi

	check_create_directory "$BG_PACKAGE_DIR"
	check_create_directory "$BG_SYSROOT_DIR"
   check_create_directory "$SRC"
   check_create_directory "$PKG"

   # Action sequence
   
   if [ "$ACTION" == "build" ]; then
      do_checksum
      do_extract
      do_build
      do_strip
      do_package
      do_footprint
      do_clean
   elif [ "$ACTION" == "add" ]; then
      do_add
   elif [ "$ACTION" == "remove" ]; then
      do_remove
   fi
   exit 0
}

main "$@"

BG_CHECK_SHA256SUM="yes"

# End of file
