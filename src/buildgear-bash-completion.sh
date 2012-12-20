#
#  Bash auto completion script for buildgear:
#
#  buildgear <command> [<options>] [<build name>]
#

_buildgear()
{
   local cur prev

   COMPREPLY=()
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
     case "$command" in
       "download")
         COMPREPLY=( $(compgen -W "$download_options" -- $cur) )
         ;;
       "build")
         COMPREPLY=( $(compgen -W "$build_options" -- $cur) )
         ;;
       "clean")
         COMPREPLY=( $(compgen -W "$clean_options" -- $cur) )
         ;;
       "show")
         COMPREPLY=( $(compgen -W "$show_options" -- $cur) )
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
