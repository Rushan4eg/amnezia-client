#!/bin/sh
set -e

mkdir -p /data/tlsfront

# Build config.toml (variables substituted on the host by Amnezia before upload)
rm -f /data/config.toml

{
    echo "### Amnezia Telemt — generated"
    echo "[general]"
    echo "use_middle_proxy = $TELEMT_USE_MIDDLE_PROXY"
    echo "log_level = \"normal\""
    if [ -n "$TELEMT_TAG" ]; then
        echo "ad_tag = \"$TELEMT_TAG\""
    fi
    echo ""
    echo "[general.modes]"
    echo "classic = false"
    echo "secure = $TELEMT_TOML_SECURE"
    echo "tls = $TELEMT_TOML_TLS"
    echo ""
    echo "[general.links]"
    echo "show = \"*\""
    if [ -n "$TELEMT_PUBLIC_HOST" ]; then
        echo "public_host = \"$TELEMT_PUBLIC_HOST\""
    fi
    echo "public_port = $TELEMT_PORT"
    echo ""
    echo "[server]"
    echo "port = $TELEMT_PORT"
    echo ""
    echo "[server.api]"
    echo "enabled = true"
    echo "listen = \"0.0.0.0:9091\""
    echo "whitelist = [\"0.0.0.0/0\", \"127.0.0.0/8\"]"
    echo ""
    echo "[[server.listeners]]"
    echo "ip = \"0.0.0.0\""
    echo ""
    echo "[censorship]"
    echo "tls_domain = \"$TELEMT_TLS_DOMAIN\""
    echo "mask = $TELEMT_MASK"
    echo "tls_emulation = $TELEMT_TLS_EMULATION"
    echo "tls_front_dir = \"/data/tlsfront\""
    echo ""
    echo "[access.users]"
    echo "$TELEMT_USER_NAME = \"$TELEMT_SECRET\""
} > /data/config.toml

echo "$TELEMT_SECRET" > /data/.amnezia-secret
chmod 600 /data/.amnezia-secret 2>/dev/null || true

# Brief run to fetch canonical links from HTTP API (see telemt docs/API.md)
/usr/local/bin/telemt /data/config.toml >/tmp/telemt-amnezia-cfg.log 2>&1 &
TPID=$!

TG_LINK=""
for _i in 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20; do
    if curl -sf "http://127.0.0.1:9091/v1/users" -o /tmp/telemt-users.json 2>/dev/null; then
        TG_LINK=$(jq -r '(.[0].links.tls // [])[0] // empty' /tmp/telemt-users.json 2>/dev/null || true)
        if [ -z "$TG_LINK" ]; then
            TG_LINK=$(jq -r '(.[0].links.secure // [])[0] // empty' /tmp/telemt-users.json 2>/dev/null || true)
        fi
        if [ -z "$TG_LINK" ]; then
            TG_LINK=$(jq -r '(.[0].links.classic // [])[0] // empty' /tmp/telemt-users.json 2>/dev/null || true)
        fi
        if [ -n "$TG_LINK" ]; then
            break
        fi
    fi
    sleep 1
done

kill "$TPID" 2>/dev/null || true
wait "$TPID" 2>/dev/null || true

TME_LINK=""
if [ -n "$TG_LINK" ]; then
    TME_LINK=$(echo "$TG_LINK" | sed 's|^tg://proxy?|https://t.me/proxy?|')
fi

echo "[*] Telemt configuration"
echo "[*] Secret:    $TELEMT_SECRET"
echo "[*] tg:// link:   $TG_LINK"
echo "[*] t.me link:    $TME_LINK"
