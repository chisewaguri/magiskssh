#!/bin/sh
set -eu

root=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)
webroot=$root/module_data/webroot

test -f "$webroot/index.html"
test -f "$webroot/style.css"
test -f "$webroot/app.js"
test -f "$webroot/vendor/kernelsu.js"
test -f "$root/module_data/common/ksu-ssh-webui"

grep -Fq './vendor/kernelsu.js' "$webroot/app.js"
grep -Fq '/data/adb/ssh/bin/ksu-ssh-webui' "$webroot/app.js"
grep -Fq 'chmod 755 /data/adb/ssh/bin/ksu-ssh-webui' "$root/module_data/customize.sh"
grep -Fq 'aria-label=' "$webroot/index.html"
grep -Fq 'Allow shell password login' "$webroot/index.html"
grep -Fq 'Allow root password login' "$webroot/index.html"
grep -Fq 'Requires shell password login and root key login' "$webroot/app.js"
grep -Fq 'prefers-color-scheme: dark' "$webroot/style.css"
grep -Fq 'prefers-reduced-motion: reduce' "$webroot/style.css"

if find "$webroot" -type f \( -name '*.html' -o -name '*.css' -o -name '*.js' \) \
    -exec grep -Eq 'https?://|src="//' {} +; then
    echo "webui contains a remote asset" >&2
    exit 1
fi

if grep -Eq '\bexec\s*\(' "$webroot/app.js"; then
    echo "webui uses exec instead of spawn" >&2
    exit 1
fi

printf 'webui static tests passed\n'
