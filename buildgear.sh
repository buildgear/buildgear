#!/bin/bash
#
#  buildgear script
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

info() {
	echo " $1"
}

warning() {
	info "WARNING: $1"
}

error() {
	info "ERROR: $1"
}

get_filename() {
	local FILE="`echo $1 | sed 's|^.*://.*/||g'`"

	if [ "$FILE" != "$1" ]; then
		FILE="$BG_SOURCE_DIR/$FILE"
	fi

	echo $FILE
}

check_buildfile() {
	if [ ! "$name" ]; then
		error "Variable 'name' not specified in $BG_BUILDFILE."
		exit 1
	elif [ ! "$version" ]; then
		error "Variable 'version' not specified in $BG_BUILDFILE."
		exit 1
	elif [ ! "$release" ]; then
		error "Variable 'release' not specified in $BG_BUILDFILE."
		exit 1
	elif [ "`type -t build`" != "function" ]; then
		error "Function 'build' not specified in $BG_BUILDFILE."
		exit 1
	fi
}

check_create_directory() {
	if [ ! -d $1 ]; then
      mkdir -p $1
	elif [ ! -w $1 ]; then
		error "Directory '$1' not writable."
		exit 1
	elif [ ! -x $1 ] || [ ! -r $1 ]; then
		error "Directory '$1' not readable."
		exit 1
	fi
}

download_file() {
	info "Downloading '$1'."

	if [ ! "`type -p curl`" ]; then
		error "Command 'curl' not found. Please install curl."
		exit 1
	fi

	LOCAL_FILENAME=`get_filename $1`
	LOCAL_FILENAME_PARTIAL="$LOCAL_FILENAME.partial"
	DOWNLOAD_CMD="--progress-bar --ftp-pasv --retry 3 --retry-delay 2 \
					  --output $LOCAL_FILENAME_PARTIAL $1"

	if [ -f "$LOCAL_FILENAME_PARTIAL" ]; then
		info "Partial download found, trying to resume"
		RESUME_CMD="--continue-at -"
	fi

	while true; do
		curl $RESUME_CMD $DOWNLOAD_CMD
		error=$?
		if [ $error != 0 ] && [ "$RESUME_CMD" ]; then
			info "Partial download failed, restarting"
			rm -f "$LOCAL_FILENAME_PARTIAL"
			RESUME_CMD=""
		else
			break
		fi
	done
	
	if [ $error != 0 ]; then
		error "Downloading '$1' failed."
		exit 1
	fi
	
	mv -f "$LOCAL_FILENAME_PARTIAL" "$LOCAL_FILENAME"
}

download_source() {
	local FILE LOCAL_FILENAME

	for FILE in ${source[@]}; do
		LOCAL_FILENAME=`get_filename $FILE`
		if [ ! -e $LOCAL_FILENAME ]; then
			if [ "$LOCAL_FILENAME" = "$FILE" ]; then
				error "Source file '$LOCAL_FILENAME' not found (can not be downloaded, URL not specified)."
				exit 1
			else
				if [ "$BG_DOWNLOAD" = "yes" ]; then
					download_file $FILE
				else
					error "Source file '$LOCAL_FILENAME' not found (use option -d to download)."
					exit 1
				fi
			fi
		fi
	done
}

unpack_source() {
	local FILE LOCAL_FILENAME COMMAND
	
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

		if [ $? != 0 ]; then
			if [ "$BG_KEEP_WORK" = "no" ]; then
				rm -rf $BG_WORK_DIR
			fi
			error "Building '$TARGET' failed."
			exit 1
		fi
	done
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

make_footprint() {
	pkginfo --footprint $TARGET | \
		sed "s|\tlib/modules/`uname -r`/|\tlib/modules/<kernel-version>/|g" | \
		sort -k 3
}

check_sha256sum() {
	local FILE="$BG_WORK_DIR/.tmp"

	#cd $BG_ROOT
	
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
			error "Sha256sum mismatch found:"
			cat $FILE.sha256sum.diff

			if [ "$BG_KEEP_WORK" = "no" ]; then
				rm -rf $BG_WORK_DIR
			fi

			if [ "$BG_CHECK_SHA256SUM" = "yes" ]; then
				error "Sha256sum not ok."
				exit 1
			fi

			error "Building '$TARGET' failed."
			exit 1
		fi
	else
		if [ "$BG_CHECK_SHA256SUM" = "yes" ]; then
			if [ "$BG_KEEP_WORK" = "no" ]; then
				rm -rf $BG_WORK_DIR
			fi
			info "Sha256sum not found."
			exit 1
		fi
		
		warning "Sha256sum not found, creating new."
		make_sha256sum > $BG_SHA256SUM
	fi

	if [ "$BG_CHECK_SHA256SUM" = "yes" ]; then
		if [ "$BG_KEEP_WORK" = "no" ]; then
			rm -rf $BG_WORK_DIR
		fi
		info "Sha256sum ok."
		exit 0
	fi
}

strip_files() {
	local FILE FILTER
	
	cd $PKG
	
	if [ -f $BG_ROOT/$BG_NOSTRIP ]; then
		FILTER="grep -v -f $BG_ROOT/$BG_NOSTRIP"
	else
		FILTER="cat"
	fi

	find . -type f -printf "%P\n" | $FILTER | while read FILE; do
		if file -b "$FILE" | grep '^.*ELF.*executable.*not stripped$' &> /dev/null; then
			strip --strip-all "$FILE"
		elif file -b "$FILE" | grep '^.*ELF.*shared object.*not stripped$' &> /dev/null; then
			strip --strip-unneeded "$FILE"
		elif file -b "$FILE" | grep '^current ar archive$' &> /dev/null; then
			strip --strip-debug "$FILE"
		fi
	done
}

compress_manpages() {
	local FILE DIR TARGET

	cd $PKG
	
	find . -type f -path "*/man/man*/*" | while read FILE; do
		if [ "$FILE" = "${FILE%%.gz}" ]; then
			gzip -1 "$FILE"
		fi
	done
	
	find . -type l -path "*/man/man*/*" | while read FILE; do
		TARGET=`readlink -n "$FILE"`
		TARGET="${TARGET##*/}"
		TARGET="${TARGET%%.gz}.gz"
		rm -f "$FILE"
		FILE="${FILE%%.gz}.gz"
		DIR=`dirname "$FILE"`

		if [ -e "$DIR/$TARGET" ]; then
			ln -sf "$TARGET" "$FILE"
		fi
	done
}

check_footprint() {
	local FILE="$BG_WORK_DIR/.tmp"
	
	cd $BG_ROOT
	
	if [ -f $TARGET ]; then
		make_footprint > $FILE.footprint
		if [ -f $BG_FOOTPRINT ]; then
			sort -k 3 $BG_FOOTPRINT > $FILE.footprint.orig
			diff -w -t -U 0 $FILE.footprint.orig $FILE.footprint | \
				sed '/^@@/d' | \
				sed '/^+++/d' | \
				sed '/^---/d' | \
				sed 's/^+/NEW		 /g' | \
				sed 's/^-/MISSING	/g' > $FILE.footprint.diff
			if [ -s $FILE.footprint.diff ]; then
				error "Footprint mismatch found:"
				cat $FILE.footprint.diff
				BUILD_SUCCESSFUL="no"
			fi
		else
			warning "Footprint not found, creating new."
			mv $FILE.footprint $BG_FOOTPRINT
		fi
	else
		error "Package '$TARGET' was not found."
		BUILD_SUCCESSFUL="no"
	fi
}

build_package() {
	local BUILD_SUCCESSFUL="no"
	
	export PKG="$BG_WORK_DIR/pkg"
	export SRC="$BG_WORK_DIR/src"
	umask 022
	
	#cd $BG_ROOT
	rm -rf $BG_WORK_DIR
	mkdir -p $SRC $PKG
	
	#if [ "$BG_IGNORE_SHA256SUM" = "no" ]; then
	#	check_sha256sum
	#fi

	if [ "$UID" != "0" ]; then
		warning "Packages should be built as root."
	fi
	
	info "Building '$TARGET'."
	
	unpack_source
	
	cd $SRC
	(set -e -x ; build)
	
	if [ $? = 0 ]; then
		if [ "$PKGMK_NO_STRIP" = "no" ]; then
			strip_files
		fi
		
		compress_manpages
		
		cd $PKG
		info "Build result:"
		tar czvvf $TARGET *
		
		if [ $? = 0 ]; then
			BUILD_SUCCESSFUL="yes"

			if [ "$PKGMK_IGNORE_FOOTPRINT" = "yes" ]; then
				warning "Footprint ignored."
			else
				check_footprint
			fi
		fi
	fi
	
	if [ "$BG_KEEP_WORK" = "no" ]; then
		rm -rf $BG_WORK_DIR
	fi
	
	if [ "$BUILD_SUCCESSFUL" = "yes" ]; then
		info "Building '$TARGET' succeeded."
	else
		if [ -f $TARGET ]; then
			touch -r $BG_ROOT/$BG_BUILDFILE $TARGET &> /dev/null
		fi
		error "Building '$TARGET' failed."
		exit 1
	fi
}

install_package() {
	local COMMAND
	
	info "Installing '$TARGET'."
	
	if [ "$PKGMK_INSTALL" = "install" ]; then
		COMMAND="pkgadd $TARGET"
	else
		COMMAND="pkgadd -u $TARGET"
	fi
	
	cd $BG_ROOT
	echo "$COMMAND"
	$COMMAND
	
	if [ $? = 0 ]; then
		info "Installing '$TARGET' succeeded."
	else
		error "Installing '$TARGET' failed."
		exit 1
	fi
}

clean() {
	local FILE LOCAL_FILENAME
	
	if [ -f $TARGET ]; then
		info "Removing $TARGET"
		rm -f $TARGET
	fi
	
	for FILE in ${source[@]}; do
		LOCAL_FILENAME=`get_filename $FILE`
		if [ -e $LOCAL_FILENAME ] && [ "$LOCAL_FILENAME" != "$FILE" ]; then
			info "Removing $LOCAL_FILENAME"
			rm -f $LOCAL_FILENAME
		fi
	done
}

update_footprint() {
	if [ ! -f $TARGET ]; then
		error "Unable to update footprint. File '$TARGET' not found."
		exit 1
	fi
	
	make_footprint > $BG_FOOTPRINT
	touch $TARGET
	
	info "Footprint updated."
}

build_needed() {
	local FILE RESULT
	
	RESULT="yes"
	if [ -f $TARGET ]; then
		RESULT="no"
		for FILE in $BG_BUILDFILE ${source[@]}; do
			FILE=`get_filename $FILE`
			if [ ! -e $FILE ] || [ ! $TARGET -nt $FILE ]; then
				RESULT="yes"
				break
			fi
		done
	fi
	
	echo $RESULT
}

interrupted() {
	echo ""
	error "Interrupted."
	
	if [ "$BG_KEEP_WORK" = "no" ]; then
		rm -rf $BG_WORK_DIR
	fi
	
	exit 1
}

print_help() {
	echo "usage: `basename $BG_COMMAND` [options]"
	echo "options:"
	echo "  -i,	--install				 build and install package"
	echo "  -u,	--upgrade				 build and install package (as upgrade)"
	echo "  -r,	--recursive			  search for and build packages recursively"
	echo "  -d,	--download				download missing source file(s)"
	echo "  -do,  --download-only		 do not build, only download missing source file(s)"
	echo "  -utd, --up-to-date			 do not build, only check if package is up to date"
	echo "  -uf,  --update-footprint	 update footprint using result from last build"
	echo "  -if,  --ignore-footprint	 build package without checking footprint"
	echo "  -um,  --update-sha256sum		 update sha256sum"
	echo "  -im,  --ignore-sha256sum		 build package without checking sha256sum"
	echo "  -cm,  --check-sha256sum		  do not build, only check sha256sum"
	echo "  -ns,  --no-strip				do not strip executable binaries or libraries"
	echo "  -f,	--force					build package even if it appears to be up to date"
	echo "  -c,	--clean					remove package and downloaded files"
	echo "  -kw,  --keep-work			  keep temporary working directory"
	echo "  -cf,  --config-file <file>  use alternative configuration file"
	echo "  -v,	--version				 print version and exit "
	echo "  -h,	--help					 print help and exit"
}

parse_options() {
	if [ ! "$2" ]; then
		echo "`basename $BG_COMMAND`: option $1 requires an argument"
		exit 1
	fi
	BG_BUILDFILE=$2
	while [ "$1" ]; do
		case $1 in
			-i|--install)
				PKGMK_INSTALL="install" ;;
			-u|--upgrade)
				PKGMK_INSTALL="upgrade" ;;
			download)
				BG_DOWNLOAD="yes" ;;
			-do|--download-only)
				BG_DOWNLOAD="yes"
				BG_DOWNLOAD_ONLY="yes" ;;
			build)
				BG_BUILD="yes" ;;
			-utd|--up-to-date)
				PKGMK_UP_TO_DATE="yes" ;;
			-uf|--update-footprint)
				PKGMK_UPDATE_FOOTPRINT="yes" ;;
			-if|--ignore-footprint)
				PKGMK_IGNORE_FOOTPRINT="yes" ;;
			-us|--update-sha256sum)
				BG_UPDATE_SHA256SUM="yes" ;;
			-is|--ignore-sha256sum)
				BG_IGNORE_SHA256SUM="yes" ;;
			-cs|--check-sha256sum)
				BG_CHECK_SHA256SUM="yes" ;;
			-ns|--no-strip)
				PKGMK_NO_STRIP="yes" ;;
			-f|--force)
				PKGMK_FORCE="yes" ;;
			-c|--clean)
				PKGMK_CLEAN="yes" ;;
			-kw|--keep-work)
				BG_KEEP_WORK="yes" ;;
			-t|--target)
				BG_BUILD_TYPE="target" ;;
			-h|--host)
				BG_BUILD_TYPE="host" ;;
			-v|--version)
				echo "`basename $BG_COMMAND` (pkgutils) $BG_VERSION"
				exit 0 ;;
			-h|--help)
				print_help
				exit 0 ;;
#			*)
#				echo "`basename $BG_COMMAND`: invalid option $1"
#				exit 1 ;;
		esac
		shift
	done
}

main() {
	local FILE TARGET
	
	parse_options "$@"

	BG_BUILDFILE_DIR="`dirname $BG_BUILDFILE`"
	
	for FILE in $BG_BUILDFILE $BG_BUILD_CFG; do
		if [ ! -f $FILE ]; then
			error "File '$FILE' not found."
			exit 1
		fi
		. $FILE
	done

   BG_WORK_DIR="$BG_BUILD_DIR/work/$BG_TYPE/$name"
   BG_PACKAGE_DIR="$BG_BUILD_DIR/package/$BG_TYPE"
   BG_SYSROOT_DIR="$BG_BUILD_DIR/work/sysroot/$BG_TYPE"

	check_create_directory "$BG_SOURCE_DIR"
	check_create_directory "$BG_WORK_DIR"
	check_create_directory "$BG_PACKAGE_DIR"
	check_create_directory "$BG_SYSROOT_DIR"
	
	check_buildfile
	
	TARGET="$BG_PACKAGE_DIR/$name#$version-$release.pkg.tar.gz"

	cd $BG_BUILDFILE_DIR

	if [ "$BG_DOWNLOAD" = "yes" ]; then
		download_source
		if [ "$BG_IGNORE_SHA256SUM" = "no" ]; then
			check_sha256sum
		fi
		
		if [ "$BG_UPDATE_SHA256SUM" = "yes" ]; then
			make_sha256sum > $BG_SHA256SUM
			info "Sha256sum updated."
			exit 0
		fi
	fi

	if [ "`build_needed`" = "no" ] && [ "$BG_BUILD" = "yes" ]; then
		info "Package '$TARGET' is up to date."
	else
		build_package
	fi

	exit 0
	
	# Original main continues from this point onwards
	
	if [ "$PKGMK_CLEAN" = "yes" ]; then
		clean
		exit 0
	fi
	
	if [ "$PKGMK_UPDATE_FOOTPRINT" = "yes" ]; then
		update_footprint
		exit 0
	fi
	
	if [ "$BG_UPDATE_SHA256SUM" = "yes" ]; then
		download_source
		make_sha256sum > $BG_SHA256SUM
		info "Md5sum updated."
		exit 0
	fi
	
	if [ "$BG_DOWNLOAD_ONLY" = "yes" ]; then
		download_source
		exit 0
	fi
	
	if [ "$PKGMK_UP_TO_DATE" = "yes" ]; then
		if [ "`build_needed`" = "yes" ]; then
			info "Package '$TARGET' is not up to date."
		else
			info "Package '$TARGET' is up to date."
		fi
		exit 0
	fi
	
	if [ "`build_needed`" = "no" ] && [ "$PKGMK_FORCE" = "no" ] && [ "$BG_CHECK_SHA256SUM" = "no" ]; then
		info "Package '$TARGET' is up to date."
	else
		download_source
		build_package
	fi
	
	if [ "$PKGMK_INSTALL" != "no" ]; then
		install_package
	fi
	
	exit 0
}

trap "interrupted" SIGHUP SIGINT SIGQUIT SIGTERM

export LC_ALL=POSIX

readonly BG_VERSION="#VERSION#"
readonly BG_COMMAND="$0"
readonly BG_ROOT="$PWD"

BG_BUILD_CFG=".buildgear/build.cfg"
BG_BUILDFILE="Buildfile"
BG_FOOTPRINT=".footprint"
BG_SHA256SUM=".sha256sum"
BG_NOSTRIP=".nostrip"

BG_BUILD_DIR="$PWD/build"
BG_SOURCE_DIR="$BG_BUILD_DIR/source"

PKGMK_INSTALL="no"
BG_DOWNLOAD="no"
BG_DOWNLOAD_ONLY="no"
BG_BUILD="no"
PKGMK_UP_TO_DATE="no"
PKGMK_UPDATE_FOOTPRINT="no"
PKGMK_IGNORE_FOOTPRINT="yes"
PKGMK_FORCE="no"
BG_KEEP_WORK="no"
BG_UPDATE_SHA256SUM="no"
BG_IGNORE_SHA256SUM="no"
BG_CHECK_SHA256SUM="yes"
PKGMK_NO_STRIP="no"
PKGMK_CLEAN="no"

main "$@"

# End of file
