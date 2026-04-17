if ! test -e /data/ssh/KEEP_ON_UNINSTALL ; then
    rm -rf /data/ssh
fi
rm -rf /data/adb/ssh

# remove symlink if ksu or ap
if [ "$KSU" = true ]; then
    BINDIR=/data/adb/ksu/bin
elif [ "$APATCH" = true ]; then
    BINDIR=/data/adb/ap/bin
fi

if [ "$KSU" = true ] || [ "$APATCH" = true ]; then
    for f in scp sftp sftp-server ssh ssh-keygen sshd sshd-session sshd-auth rsync; do
        rm -rf "$BINDIR/$f"
    done
fi
