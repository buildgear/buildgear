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

   commands="download build clean show init help config"
   options="--help --version"
   init_options="--buildfile"
   download_options="--all"
   build_options="--keep-work --update-checksum --update-footprint --no-strip
                  --no-fakeroot --all --load-chart"
   clean_options="--all --footprint --checksum"
   show_options="--build-order --download-order --dependency --readme --version
                 --log --log-tail --log-mismatch --footprint --checksum --buildfile
                 --manifest --manifest-xml"
   config_options="--global --unset --list"
   config_keys="source_dir download_mirror_first download_timeout certificate_dir
                download_retry download_connections parallel_builds ssh_public_key
                ssh_private_key log_rotation"
   help_options=$commands

   if [ $COMP_CWORD -eq 1 ]; then
     COMPREPLY=( $(compgen -W "$commands $options" -- $cur) )
   elif [ $COMP_CWORD -gt 1 ]; then

     if [[ "$cur" == cross/* ]]; then
       builds=( $(compgen -W "`echo $CROSS | sed 's|[^ 	]\+|cross/&|g'`" -- $cur) )
     else
       builds=( $(compgen -W "$CROSS" -- $cur) )
     fi

     if [[ ${#CROSS} > 0 ]] && [[ "$cur" != cross/* ]]; then
       builds+=( $(compgen -W "cross/" -- $cur) )
       [[ $builds = "cross/" ]] && compopt -o nospace
     fi

     builds+=( $(compgen -W "$NATIVE" -P "native/" -- $cur) )
     builds+=( $(compgen -W "`echo $NATIVE | sed 's|[^ 	]\+|native/&|g'`" -- $cur) )

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
       "config")
         config_key_in_args=0
         config_opts_in_args=0
         config_list_in_args=0
         config_key_in_prev=0
         for i in "${COMP_WORDS[@]}"
         do
            for j in ${config_keys}
            do
               if [ "$j" == "$i" ]; then
                  config_key_in_args=1
               fi
               if [ "$prev" == "$j" ]; then
                  config_key_in_prev=1
               fi
            done
            for j in ${config_options}
            do
               if [ "$j" == "$i" ]; then
                  config_opts_in_args=1
               fi
            done
            if [ "$i" == "--list" ]; then
               config_list_in_args=1
            fi
         done
         if [[ "$config_key_in_prev" == 0 && "$config_opts_in_args" == 0 ]]; then
            COMPREPLY=( $(compgen -W "$config_options" -- $cur) )
         fi
         if [[ "$config_key_in_args" == 0 && "$config_list_in_args" == 0 ]]; then
            COMPREPLY+=( $(compgen -W "$config_keys" -- $cur) )
         fi
         if [[ "$prev" == "source_dir" || "$prev" == "certificate_dir" ]]; then
            COMPREPLY+=( $(compgen -d -S / -- $cur) )
            compopt -o nospace
         fi
         if [[ "$prev" == "ssh_public_key" || "$prev" == "ssh_private_key" ]]; then
            COMPREPLY+=( $(compgen -f -- $cur) )
            compopt -o filenames
         fi
         ;;
       "show")
         show_opts_in_args=0
         for i in "${COMP_WORDS[@]}"
         do
           for j in ${show_options}
           do
             if [ "$j" == "$i" ]; then
               show_opts_in_args=1
             fi
           done
         done
         if [ "$show_opts_in_args" == 0 ]; then
           COMPREPLY=( $(compgen -W "$show_options" -- $cur) )
         fi
         for i in "${COMP_WORDS[@]}"
         do
           if [[ "$i" == "--build-order" || "$i" == "--download-order" || "$i" == "--dependency" || \
                 "$i" == "--version" || "$i" == "--footprint" || "$i" == "--checksum" || \
                 "$i" == "--buildfile" || "$i" == "--manifest" || "$i" == "--manifest-xml" ]]; then
             if [ "$build_name_in_args" == 0 ]; then
               COMPREPLY+=( ${builds[@]} )
             fi
           fi
         done
         ;;
       "init")
        COMPREPLY=( $(compgen -W "$init_options" -- $cur) )
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
