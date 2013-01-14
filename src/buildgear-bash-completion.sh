#
#  Bash auto completion script for buildgear:
#
#  buildgear <command> [<options>] [<build name>]
#

_find_root()
{
   root_dir=${PWD}
   while [ ! -d "${root_dir}/.buildgear" ]; do
      root_dir=`dirname ${root_dir}`
      if [ "$root_dir" = "/" ]; then
         root_dir=''
         break
      fi
   done;
   echo $root_dir
}

_native_builds()
{
   echo $(grep -Ris -m 1 --include=Buildfile -- 'name=' $1'/buildfiles/native' | awk -F '=' '{ print $2 }')
}

_cross_builds()
{
   echo $(grep -Ris -m 1 --include=Buildfile -- 'name=' $1'/buildfiles/cross' | awk -F '=' '{ print $2 }')
}

_buildgear()
{
   local cur prev

   COMPREPLY=()
   ROOTDIR=`_find_root`
   CROSS=$(_cross_builds $ROOTDIR)
   NATIVE=`_native_builds $ROOTDIR`
   build_name_in_args=0

   all_builds=( $CROSS )
   all_builds+=( `echo $NATIVE | sed 's|[^ 	]\+|native/&|g'` )
   all_builds+=( `echo $CROSS | sed 's|[^ 	]\+|cross/&|g'` )

   cur=${COMP_WORDS[COMP_CWORD]}
   prev=${COMP_WORDS[COMP_CWORD-1]}
   command=${COMP_WORDS[1]}

   commands="download build clean show init help"
   options="--help --version"
   download_options="--all"
   build_options="--keep-work --update-checksum --update-footprint --no-strip
                  --no-fakeroot --all"
   clean_options="--all"
   show_options="--build-order --download-order --dependency --readme --version
                 --log --log-tail --log-mismatch"
   help_options=$commands

   if [ $COMP_CWORD -eq 1 ]; then
     COMPREPLY=( $(compgen -W "$commands $options" -- $cur) )
   elif [ $COMP_CWORD -gt 1 ]; then

     if [[ "$cur" == cross/* ]]; then
       builds=( $(compgen -W "`echo $CROSS | sed 's|[^ 	]\+|cross/&|g'`" -- $cur) )
     else
       builds=( $(compgen -W "$CROSS" -- $cur) )
     fi
     builds+=( $(compgen -W "$NATIVE" -P "native/" -- $cur) )
     builds+=( $(compgen -W "`echo $NATIVE | sed 's|[^ 	]\+|native/&|g'`" -- $cur) )
     builds+=( $(compgen -W "cross/" -- $cur) )
     [[ $builds = "cross/" ]] && compopt -o nospace

     for i in "${COMP_WORDS[@]}"
     do
       for j in "${all_builds[@]}"
       do
         if [ "$i" == "$j" ]; then
           build_name_in_args=1
         fi
       done
     done
     case "$command" in
       "download")
         COMPREPLY=( $(compgen -W "$download_options" -- $cur) )
         if [ "$build_name_in_args" == 0 ]; then
           COMPREPLY+=( ${builds[@]} )
         fi
         ;;
       "build")
         if [ "$build_name_in_args" == 0 ]; then
           COMPREPLY+=( ${builds[@]} )
         else
           COMPREPLY+=( $(compgen -W "$build_options" -- $cur) )
         fi
         ;;
       "clean")
         COMPREPLY=( $(compgen -W "$clean_options" -- $cur) )
         if [ "$build_name_in_args" == 0 ]; then
           COMPREPLY+=( ${builds[@]} )
         fi
         ;;
       "show")
         COMPREPLY=( $(compgen -W "$show_options" -- $cur) )
         for i in "${COMP_WORDS[@]}"
         do
           if [[ "$i" == "--build-order" || "$i" == "--download-order" || "$i" == "--dependency" || "$i" == "--version" ]]; then
             if [ "$build_name_in_args" == 0 ]; then
               COMPREPLY+=( ${builds[@]} )
             fi
           fi
         done
         ;;
       "init")
         ;;
       "help")
         COMPREPLY=( $(compgen -W "$help_options" -- $cur) )
         ;;
       *)
         ;;
     esac
   fi

   return 0
} &&
complete -F _buildgear buildgear
