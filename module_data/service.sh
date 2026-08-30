#!/system/bin/sh
MODDIR=${0%/*}

[ -f /data/ssh/no-autostart ] || "$MODDIR/opensshd.init" start
